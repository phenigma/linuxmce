#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "DeviceData_Impl.h"
#include "Command_Impl.h"
#include "Event_Impl.h"
#include "PreformedCommand.h"

using namespace DCE;

// Entry point for the MessageQueue thread
void* MessageQueueThread_DCECI(void* param) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
    Command_Impl* p = (Command_Impl*)param;
    p->ProcessMessageQueue();
    return NULL;
}

Command_Impl::Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode, class Router *pRouter)
    : HandleRequestSocket(DeviceID, ServerAddress,"Command_Impl1 Dev #" + StringUtils::itos(DeviceID)),
    m_MessageQueueMutex("MessageQueue")
{
    m_pRouter=pRouter;
    m_pcRequestSocket=NULL;
    m_bLocalMode=bLocalMode;
    m_bReload=false;
    m_bQuit=false;
    m_pPrimaryDeviceCommand = this;
    m_bHandleChildren = false;
    m_pParent = NULL;
    m_MessageQueueMutex.Init(NULL);
    m_bKillSpawnedDevicesOnExit=false;
    pthread_cond_init(&m_MessageQueueCond, NULL);
    if(pthread_create(&m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot create message processing queue");
    }
/*
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS,"DeviceCommand Constructor %p, device ID: %d",this,DeviceID);
#endif
*/
}

Command_Impl::Command_Impl(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter)
    : HandleRequestSocket(pData->m_iPK_Device, "", "Command_Impl2 Dev #"),
    m_MessageQueueMutex("MessageQueue")
{
    m_pRouter=pRouter;
    m_pcRequestSocket=NULL;
    m_bReload=false;
    m_bQuit=false;
    m_bLocalMode=false;
    m_pPrimaryDeviceCommand = pPrimaryDeviceCommand;
    m_pData = pData;
    m_pEvent = pEvent;
    m_bHandleChildren = false;
    m_MessageQueueMutex.Init(NULL);
    m_bKillSpawnedDevicesOnExit=false;
    pthread_cond_init(&m_MessageQueueCond, NULL);
    if(pthread_create(&m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot create message processing queue");
    }

    // Connect the primary device's event processor
    // to the Data class so Data class changes can be sent.
    m_pData->m_pEvent_Impl = m_pPrimaryDeviceCommand->m_pEvent;
/*
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS,"DeviceCommand Constructor2 %p, device ID: %d",this,pData->m_iPK_Device);
#endif
*/
}

Command_Impl::~Command_Impl()
{
    if (m_pData)
        delete m_pData;
    if (this==m_pPrimaryDeviceCommand && m_pEvent)
        delete m_pEvent;
    if( m_bKillSpawnedDevicesOnExit )
        KillSpawnedDevices();
}

bool Command_Impl::Connect()
{
    // Must have an event socket to proceed.
    if (!m_pEvent)
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"No event handler for device ID: %d", this->m_DeviceID);
        return false;
    }
    if (m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
    {
        m_pEvent->m_pClientSocket->Connect();
        if (m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"No client socket for device ID: %d", this->m_DeviceID);
            return false;
        }
    }
    if (!ClientSocket::Connect())
    {
#if (!defined(UNDER_CE) || !defined(DEBUG))
        g_pPlutoLogger->Write(LV_CRITICAL,"DeviceCommand connect failed %p, device ID: %d",this,this->m_DeviceID);
#endif
        if (m_pEvent)
            m_pEvent->m_pClientSocket->Disconnect();
        return false;
    }

    return true;
}

void Command_Impl::ReplaceParams(::std::string replacement) {

    if (replacement != "") {
    string param1,param2;
    bool done;
    int param_value;

    string::size_type pos;
    pos=0;
    done=false;

    do {
		param1=StringUtils::Tokenize(replacement,::std::string(","),pos);
		param2=StringUtils::Tokenize(replacement,::std::string(","),pos);

       param_value=atoi(param1.c_str());

       if ((param1!="")&&(param2!="")&&(param_value!=0)) {
          m_pData->m_Parameters.erase(param_value);
          m_pData->m_Parameters.insert(pair<int, string>(param_value,param2));
       } else {
           done=true;
       }

    } while (!done);

    }
}

void Command_Impl::CreateChildren()
{
    for(int i=0;i<(int)m_pData->m_vectDeviceData_Impl_Children.size();i++)
    {
        DeviceData_Impl *pDeviceData_Impl_Child = m_pData->m_vectDeviceData_Impl_Children[i];

        // This device has it's own executible.  Try to spawn it.  If that fails, we will try to create it ourselves
        if( pDeviceData_Impl_Child->m_bImplementsDCE && !pDeviceData_Impl_Child->m_bIsEmbedded )
        {
            if( SpawnChildDevice(pDeviceData_Impl_Child) )
                continue;
        }
        Event_Impl *pEvent = m_pEvent->CreateEvent(pDeviceData_Impl_Child->m_iPK_DeviceTemplate, m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_iPK_Device);
        if (!pEvent)
        {
            pEvent = new Event_Impl(m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_iPK_Device);
            g_pPlutoLogger->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom event handler for.  It will not fire events.",
                pDeviceData_Impl_Child->m_iPK_DeviceTemplate);
        }
        Command_Impl *pCommand = m_pPrimaryDeviceCommand->CreateCommand(pDeviceData_Impl_Child->m_iPK_DeviceTemplate, m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent);
        if (!pCommand)
        {
            pCommand = new Command_Impl(m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent, m_pRouter);
            g_pPlutoLogger->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom handler for.  This is normal for IR.", pDeviceData_Impl_Child->m_iPK_DeviceTemplate);
        }
        pCommand->m_pParent = this;
        pCommand->CreateChildren();
        m_mapCommandImpl_Children[pDeviceData_Impl_Child->m_iPK_Device] = pCommand;
    }
}

bool Command_Impl::SpawnChildDevice(class DeviceData_Impl *pDeviceData_Impl_Child,string Display)
{
#ifndef WIN32
    string Prefix="";

    if( !FileUtils::FileExists( Prefix + "Spawn_Device.sh" ) )
    {
        Prefix = "./";
        if( !FileUtils::FileExists( Prefix + "Spawn_Device.sh" ) )
        {
            Prefix = "/usr/bin/pluto";
            if( !FileUtils::FileExists( Prefix + "Spawn_Device.sh" ) )
            {
                g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Spawn_Device.sh");
                return false;
            }
        }
    }

	g_pPlutoLogger->Write(LV_STATUS,"Spawning device: %d %s on display: %s ip: %s",pDeviceData_Impl_Child->m_iPK_Device,pDeviceData_Impl_Child->m_sCommandLine.c_str(),Display.c_str(),m_IPAddress.c_str());
    system( ("screen -d -m -h 3000 -S " + StringUtils::itos(pDeviceData_Impl_Child->m_iPK_Device) + "_" + pDeviceData_Impl_Child->m_sCommandLine +
        " sh -x " + Prefix + "Spawn_Device.sh " + StringUtils::itos(pDeviceData_Impl_Child->m_iPK_Device) + " " + m_IPAddress + " " + Display + " " + pDeviceData_Impl_Child->m_sCommandLine).c_str() );
    m_vectSpawnedDevices.push_back(StringUtils::itos(pDeviceData_Impl_Child->m_iPK_Device) + "_" + pDeviceData_Impl_Child->m_sCommandLine);
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    m_vectSpawnedDevices.push_back(StringUtils::itos(pDeviceData_Impl_Child->m_iPK_Device) + "_" + pDeviceData_Impl_Child->m_sCommandLine);
#endif
    return true;
}

void Command_Impl::KillSpawnedDevices()
{
    for(size_t s=0;s<m_vectSpawnedDevices.size();++s)
    {
        system((string("") + "screen -list | grep " + m_vectSpawnedDevices[s] + " | cut -f 1 -d '.' | cut -f 2 -d '\t' | xargs kill -9").c_str());
//         g_pPlutoLogger->Write(LV_WARNING,"Need to kill %s",m_vectSpawnedDevices[s].c_str());
    }
}

void Command_Impl::ReceivedString(string Line)
{
    MapCommand_Impl::iterator i;

    for(i=m_mapCommandImpl_Children.begin();i!=m_mapCommandImpl_Children.end();++i)
    {
        (*i).second->ReceivedString(Line);
    }
}

bool Command_Impl::ReceivedMessage(Message *pMessage)
{
    // The class that overrides this will try to handle it
    // If it can't it will pass it to this function.
    map<long, string>::iterator p;

    if (pMessage->m_MessageType == MESSAGETYPE_SYSCOMMAND && pMessage->m_ID == SYSCOMMAND_QUIT)
    {
        SendString("BYE");
        Sleep(250);
        OnQuit();
/* see notes in header file 1/2/2004
        Disconnect();
        if (m_pEvent && m_pEvent->m_pClientSocket)
            m_pEvent->m_pClientSocket->Disconnect();
*/
        return true;
    }
    if (pMessage->m_MessageType == MESSAGETYPE_SYSCOMMAND && pMessage->m_ID == SYSCOMMAND_RELOAD)
    {
        SendString("BYE");
        Sleep(250);
        OnReload();
        return true;
    }
    if (m_bHandleChildren && pMessage->m_DeviceIDTo != m_DeviceID)
    {
        // This is slightly inelegant because we don't want to change
        // the base class implementation in order to do this.  If the target device ID is not this device
        // and we're handling children, set the target device ID to the child, change the target,
        // and run it back through ReceivedMessage so the higher level class processes it as
        // it's own.
        m_TargetDeviceID  = pMessage->m_DeviceIDTo;
        pMessage->m_DeviceIDTo = m_DeviceID;
        return ReceivedMessage(pMessage);
    }
    if (pMessage->m_MessageType == MESSAGETYPE_DATAPARM_REQUEST && pMessage->m_DeviceIDTo == m_DeviceID)
    {
/* todo 2.0
            p = m_pData->m_Parameters.find(pMessage->m_ID);
            if (p==m_pData->m_Parameters.end())
            {
                SendString("ERR Parameter not found");
            }
            else
            {
                RequestingParameter(pMessage->m_ID);
                SendMessage(new Message(m_DeviceID, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_ID, (*p).second.c_str()));
            }
*/
        return true;
    }
    else if (pMessage->m_MessageType == MESSAGETYPE_DATAPARM_CHANGE && pMessage->m_DeviceIDTo == m_DeviceID)
    {
        p = pMessage->m_Parameters.begin();
        if (p!=pMessage->m_Parameters.end())
        {
            string ValueOld = m_pData->m_Parameters[(*p).first];
            m_pData->m_Parameters[(*p).first] = (*p).second;
            g_pPlutoLogger->Write(LV_STATUS, "Updating dataparm %d with %s (Device %d).", (*p).first, (*p).second.c_str(), m_DeviceID);
            SendString("OK");
            OnDataChange((*p).first, ValueOld, (*p).second);
        }
        else
        {
            g_pPlutoLogger->Write(LV_WARNING, "Got a data parm change without the value to change.");
            SendString("ERR Input");
        }
        return true;
    }
    else
    {
        MapCommand_Impl::iterator i;

        for(i=m_mapCommandImpl_Children.begin();i!=m_mapCommandImpl_Children.end();++i)
        {
            if ((*i).second->ReceivedMessage(pMessage))
                return true;
        }
    }
    return false;
}

Command_Impl *Command_Impl::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
    if (m_pPrimaryDeviceCommand!=this)
    {
        return m_pPrimaryDeviceCommand->CreateCommand(PK_DeviceTemplate, pPrimaryDeviceCommand, pData, pEvent);
    }
    return NULL;
}

// This method should not be called directly.  It is to be called by the queue handler only
void Command_Impl::ProcessMessageQueue()
{
    pthread_mutex_lock(&m_MessageQueueMutex.mutex);
    while(true)
    {
        while( m_MessageQueue.size()==0 )
        {
            pthread_cond_wait(&m_MessageQueueCond,&m_MessageQueueMutex.mutex);
        }

        list<Message *> copyMessageQueue;

        // Make a copy so that we're not holding the lock while we send
        list<Message *>::iterator itMessageQueue;
        for(itMessageQueue=m_MessageQueue.begin();itMessageQueue!=m_MessageQueue.end();++itMessageQueue)
		{
			Message *pMessage = *itMessageQueue;
            copyMessageQueue.push_back(*itMessageQueue);
		}

        m_MessageQueue.clear();
        pthread_mutex_unlock(&m_MessageQueueMutex.mutex);
        for(itMessageQueue=copyMessageQueue.begin();itMessageQueue!=copyMessageQueue.end();++itMessageQueue)
        {
			Message *pMessage = *itMessageQueue;
            m_pEvent->SendMessage(*itMessageQueue);
        }
        pthread_mutex_lock(&m_MessageQueueMutex.mutex);
    }
}

void Command_Impl::QueueMessage(Message *pMessage)
{
    if( m_bLocalMode )
        return;

    PLUTO_SAFETY_LOCK_ERRORSONLY(mq,m_MessageQueueMutex);
    m_MessageQueue.push_back(pMessage);
    mq.Release();
    pthread_cond_broadcast(&m_MessageQueueCond);
}

bool Command_Impl::SendMessageWithConfirm(Message *pMessage,string &Response)
{
    pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
    bool bResponse = m_pcRequestSocket->SendMessage(pMessage,Response);
    return bResponse && Response=="OK";
}

bool Command_Impl::SendCommand(PreformedCommand &pPreformedCommand,int iConfirmation,string *p_sResponse)
{
    if( m_bLocalMode )
        return true; // If it's just a test, local mode without a router

    // Just put it in the queue.  The queue will delete pPreformedCommand.m_pMessage after sending
    if( iConfirmation==0 || (iConfirmation==-1 && !pPreformedCommand.m_pcResponse) )
    {
        pPreformedCommand.m_pMessage->m_eExpectedResponse=ER_None;
        QueueMessage(pPreformedCommand.m_pMessage);
        return true;
    }

    // We need a response.  It will be a string if there are no out parameters
    if( !pPreformedCommand.m_pcResponse )
    {
        pPreformedCommand.m_pMessage->m_eExpectedResponse=ER_DeliveryConfirmation;  // i.e. just an "OK"
        string sResponse; // We'll use this only if a response wasn't passed in
        if( !p_sResponse )
            p_sResponse = &sResponse;

        bool bResult = m_pcRequestSocket->SendMessage(pPreformedCommand.m_pMessage,*p_sResponse);
        return bResult && *p_sResponse=="OK";
    }
    // There are out parameters, we need to get a message back in return
    pPreformedCommand.m_pMessage->m_eExpectedResponse=ER_ReplyMessage;
    Message *pResponse = m_pcRequestSocket->SendReceiveMessage(pPreformedCommand.m_pMessage);
    if( !pResponse || pResponse->m_ID!=0 )
        return false;
    pPreformedCommand.ParseResponse(pResponse);
    return true;
}
