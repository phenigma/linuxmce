/*
 * $Id: Router.cpp,v 1.21 2004/10/28 05:11:28 mihaid Exp $
 */

#include <string>
using namespace std;

#include <math.h>
#ifdef WIN32
#include <WinSock.h>
#else
#include <sys/vfs.h>
#include <dirent.h>
#endif
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Message.h"
#include "ServerSocket.h"
#include <sys/stat.h>
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "AlarmManager.h"
#include "DCE/DCEConfig.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Command_Pipe.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceGroup.h"
#include "pluto_main/Table_Device_DeviceGroup.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceTemplate_DeviceCommandGroup.h"
#include "pluto_main/Table_DeviceCommandGroup.h"
#include "pluto_main/Table_DeviceCommandGroup_Command.h"
#include "pluto_main/Table_DeviceTemplate_Input.h"
#include "pluto_main/Table_DeviceTemplate_DSPMode.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Room.h"

#ifndef WIN32
#include "dlfcn.h" // for Plugins (Dynamically Loaded Shared Objects)
#endif

#ifdef WIN32
#include<conio.h>
#endif

using namespace DCE;
extern DCEConfig g_DCEConfig;

void* MessageQueueThread_DCER(void* param) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
    Router* pc = (Router*)param;
    pc->ProcessQueue();

    return NULL;
}

Router::Router(int PK_Device,int PK_Installation,string BasePath,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort,int ListenPort) :
    SocketListener("Router Dev #" + StringUtils::itos(PK_Device)), m_CoreMutex("core"), MySqlHelper(DBHost,DBUser,DBPassword,DBName,DBPort)
{
    m_sBasePath=BasePath;
    m_bReload = false;
    m_bQuit = false;
    m_dwPK_Device=PK_Device;
    m_dwPK_Installation=PK_Installation;
    m_pDeviceStructure=NULL;
    m_pRow_Device_Me=NULL;
    m_sDBHost=DBHost;
    m_sDBUser=DBUser;
    m_sDBPassword=DBPassword;
    m_sDBName=DBName;
    m_dwIDBPort=DBPort;
    m_bIsLoading=false;

    m_CoreMutex.Init(NULL);
    pthread_mutex_init(&m_MessageQueueMutex,NULL);
    pthread_cond_init(&m_MessageQueueCond, NULL);

    if(pthread_create(&m_pthread_queue_id, NULL, MessageQueueThread_DCER, (void*)this))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot create device event thread1");
    }

    // Parse the config file
    char buf[200];
    gethostname(buf,sizeof(buf));

    m_Routing_DeviceToController.clear();
    m_Routing_ControllerToIP.clear();

    m_Port = ListenPort;

    m_pDatabase_pluto_main = new Database_pluto_main();

    if(!m_pDatabase_pluto_main->Connect(m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName,m_dwIDBPort) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
    }

    if( !m_dwPK_Installation && g_DCEConfig.m_iPK_Installation>0 )
		m_dwPK_Installation = g_DCEConfig.m_iPK_Installation;

	if( !m_dwPK_Device && g_DCEConfig.m_iPK_Device_Computer>0 )
	{
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST)
			+ " AND " + DEVICE_FK_DEVICE_CONTROLLEDVIA_FIELD + "=" + StringUtils::itos(g_DCEConfig.m_iPK_Device_Computer), &vectRow_Device);
        if( vectRow_Device.size()==1 )
		{
			Row_Device *pRow_Device = vectRow_Device[0];
			m_dwPK_Device = pRow_Device->PK_Device_get();
		}
	}

    if( !m_dwPK_Installation && !m_dwPK_Device )
    {
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST), &vectRow_Device);
        if( vectRow_Device.size()!=1 )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine my device ID automatically.  # of records in DB: %d",(int) vectRow_Device.size());
            cerr << "Sorry...   I cannot figure out what my device ID is automatically.  To do this there must be only" << endl;
            cerr << "1 record in the database for a device with the device template of 1 (DCE Router).  There are " << (int) vectRow_Device.size() << endl;
            cerr << "You will need to specify the device id or the installation id on the command line." << endl;
            exit(1);
        }
        m_pRow_Device_Me=vectRow_Device[0];
    }

    if( !m_pRow_Device_Me && m_dwPK_Device )
    {
        m_pRow_Device_Me = m_pDatabase_pluto_main->Device_get()->GetRow(m_dwPK_Device);
        if( m_pRow_Device_Me && m_dwPK_Installation && m_pRow_Device_Me->FK_Installation_get()!=PK_Installation )
        {
            g_pPlutoLogger->Write(LV_WARNING,"You specified an installation ID of %d on the command line, but also a device of %d which belongs to installation %d",m_dwPK_Installation,m_dwPK_Device,m_pRow_Device_Me->FK_Installation_get());
        }
    }

    if( !m_pRow_Device_Me )
    {
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation) + " AND " + string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST),&vectRow_Device);
        if( vectRow_Device.size()!=1 )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine my device ID automatically.  # of records in DB: %d",(int) vectRow_Device.size());
            cerr << "Sorry...   I cannot figure out what my device ID is automatically.  To do this there must be only" << endl;
            cerr << "1 record in the database for a device with the device template type of 1 (DCE Router) in installation " << m_dwPK_Installation << ".  There are " << (int) vectRow_Device.size() << endl;
            cerr << "You will need to specify the device id on the command line." << endl;
            exit(1);
        }
        m_pRow_Device_Me=vectRow_Device[0];
    }

    m_dwPK_Device = m_pRow_Device_Me->PK_Device_get();
    m_dwPK_Installation = m_pRow_Device_Me->FK_Installation_get();

    if( m_sBasePath=="" )
    {
        // Let's find it
#ifdef WIN32
        m_sBasePath="/pluto/";
#else
        m_sBasePath="/usr/pluto/";
#endif
    }

    g_pPlutoLogger->Write(LV_STATUS,"Running as device: %d installation: %d using path: %s",m_dwPK_Device,m_dwPK_Installation,m_sBasePath.c_str());

    m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(4);      //4 = number of worker threads

    Configure();
}

Router::~Router()
{
    delete m_pDeviceStructure;

    map<int,Command *>::iterator iC;
    for(iC = m_mapCommand.begin(); iC != m_mapCommand.end(); ++iC)
        delete (*iC).second;

    m_pAlarmManager->Stop();
    delete m_pAlarmManager;
    m_pAlarmManager = NULL;

	delete m_pDatabase_pluto_main;
}

void Router::RegisterAllPlugins()
{
    vector<Row_Device *> vectRow_Device;
    m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICE_CONTROLLEDVIA_FIELD) + "=" + StringUtils::itos(m_dwPK_Device), &vectRow_Device);
    for(size_t s=0;s<vectRow_Device.size();++s )
    {
        Row_Device *pRow_Device = vectRow_Device[s];

        if( !pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get() )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"The router has a child device %d (%s).  But this isn't a valid plug-in.  ImplementsDCE must be true.",
                pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_getrow()->Description_get().c_str());
            continue;
        }
        string CommandLine = pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get();
        if( CommandLine.size()==0 )
            CommandLine = FileUtils::ValidCPPName( pRow_Device->FK_DeviceTemplate_getrow()->Description_get() );

#ifdef AUDIDEMO
        Command_Impl *pPlugIn = CreatePlugInHardCoded(pRow_Device->PK_Device_get(), pRow_Device->FK_DeviceTemplate_get(), CommandLine);
#else
        Command_Impl *pPlugIn = CreatePlugIn(pRow_Device->PK_Device_get(), pRow_Device->FK_DeviceTemplate_get(), CommandLine);
#endif
        if( !pPlugIn )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Cannot create plug-in for device: %d.  Be sure the file %s exists",
                pRow_Device->PK_Device_get(),CommandLine.c_str());
        }
        else
        {
            m_mapPlugIn[pRow_Device->PK_Device_get()]=pPlugIn;
            ListCommand_Impl *pListCommand_Impl = m_mapPlugIn_DeviceTemplate[pRow_Device->FK_DeviceTemplate_get()];
            if( !pListCommand_Impl )
            {
                pListCommand_Impl = new ListCommand_Impl();
                m_mapPlugIn_DeviceTemplate[pRow_Device->FK_DeviceTemplate_get()]=pListCommand_Impl;
            }
            pListCommand_Impl->push_back(pPlugIn);
        }
    }

    map<int,class Command_Impl *>::iterator it;
    for(it=m_mapPlugIn.begin();it!=m_mapPlugIn.end();++it)
	{
		Command_Impl *pPlugIn = (*it).second;
		g_pPlutoLogger->Write(LV_STATUS,"Registering plugin: %s",pPlugIn->m_sName.c_str());
        pPlugIn->Register();
	}
}

// sCommandLine = Plugin filename
// PK_Device = 4 for now
// Description: Loads SO/DLL; Calls RegisterAsPlugin(...) from that
// Return: Result from RegisterAsPlugin(...) on success; NULL on error
Command_Impl *Router::CreatePlugIn(int PK_Device, int PK_DeviceTemplate, string sCommandLine)
{
    typedef class Command_Impl * (* RAP_FType) (class Router *, int, string);
    RAP_FType RegisterAsPlugin;
    string ErrorMessage;
    char MS_ErrorMessage[1024];

    void * so_handle;

    if (sCommandLine == "")
        return NULL;

#ifndef WIN32
    sCommandLine += ".so";
    if (sCommandLine.find("/") == string::npos)
        sCommandLine = "./" + sCommandLine;
    so_handle = dlopen(sCommandLine.c_str(), RTLD_NOW);
#else
    so_handle = LoadLibrary(sCommandLine.c_str());
#endif

    if (so_handle == NULL)
    {
#ifndef WIN32
        ErrorMessage = dlerror();
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
        ErrorMessage = MS_ErrorMessage;
#endif
        g_pPlutoLogger->Write(LV_CRITICAL, "Can't open plug-in file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
        return NULL;
    }

#ifndef WIN32
    RegisterAsPlugin = (RAP_FType) dlsym(so_handle, "RegisterAsPlugIn");
#else
    RegisterAsPlugin = (RAP_FType) GetProcAddress((HMODULE) so_handle, "RegisterAsPlugIn");
#endif

    if (RegisterAsPlugin == NULL)
    {
#ifndef WIN32
        ErrorMessage = dlerror();
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
        ErrorMessage = MS_ErrorMessage;
#endif
        g_pPlutoLogger->Write(LV_CRITICAL, "Failed to load symbol 'RegisterAsPlugin' from file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
        return NULL;
    }

    g_pPlutoLogger->Write(LV_WARNING, "Loaded plug-in device: %d master device: %d -- %s",PK_Device, PK_DeviceTemplate, sCommandLine.c_str());

    string LogFile = StringUtils::itos(PK_Device) + "_" + sCommandLine + "_" + StringUtils::itos(PK_DeviceTemplate) + ".log";
    char sz[100];
    strcpy(sz,LogFile.c_str());

    // HACK ___ when I pass in a string > 16 characters, even a copy like this, it crashes when the string is destroyed as the function exits!!!!  But a constant string works????
//  return RegisterAsPlugin(this, PK_Device, sz);
    return RegisterAsPlugin(this, PK_Device, "dce_router"); // hack!!!
}

void Router::RegisterMsgInterceptor(Message *pMessage)
{
	int PK_Device_From = atoi( pMessage->m_mapParameters[PARM_FROM].c_str() );
	int PK_Device_To = atoi( pMessage->m_mapParameters[PARM_TO].c_str() );
	int PK_DeviceTemplate = atoi( pMessage->m_mapParameters[PARM_TEMPLATE].c_str() );
	int PK_DeviceCategory = atoi( pMessage->m_mapParameters[PARM_CATEGORY].c_str() );
	int MessageType = atoi( pMessage->m_mapParameters[PARM_MESSAGE_TYPE].c_str() );
	int MessageID = atoi( pMessage->m_mapParameters[PARM_MESSAGE_ID].c_str() );
	
	RegisterMsgInterceptor(
			new MessageInterceptorCallBack(pMessage->m_dwPK_Device_From, pMessage->m_dwID),
			PK_Device_From,PK_Device_To,PK_DeviceTemplate,PK_DeviceCategory,MessageType,MessageID );
}

void Router::ReceivedMessage(Socket *pSocket, Message *pMessageWillBeDeleted)
{
    SafetyMessage SafetyMessage(pMessageWillBeDeleted);

	if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_REGISTER_INTERCEPTOR )
	{
		RegisterMsgInterceptor( (*SafetyMessage) );
		return;
	}

    if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_MASTERDEVICE || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_CATEGORY || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_GROUP )
    {
        // We're going to have to come up with the list of matching devices
        if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_MASTERDEVICE )
            (*SafetyMessage)->m_sPK_Device_List_To=GetDevicesByDeviceTemplate((*SafetyMessage)->m_dwPK_Device_Template,(*SafetyMessage)->m_eBroadcastLevel);
        else if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_CATEGORY )
            (*SafetyMessage)->m_sPK_Device_List_To=GetDevicesByCategory((*SafetyMessage)->m_dwPK_Device_Category_To,(*SafetyMessage)->m_eBroadcastLevel);
//      else if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_GROUP )  TODO
//          (*SafetyMessage)->m_sDeviceIDTo=GetDevicesByDeviceTemplate((*SafetyMessage)->m_PK_DeviceTemplate,(*SafetyMessage)->m_eBroadcastLevel);

        (*SafetyMessage)->m_dwPK_Device_To=DEVICEID_LIST;
    }

    if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_LIST && (*SafetyMessage)->m_sPK_Device_List_To.length()==0 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Received message type: %d id: %d from: %d going to an empty device list md: %d cat: %d",
            (*SafetyMessage)->m_dwMessage_Type,(*SafetyMessage)->m_dwID,(*SafetyMessage)->m_dwPK_Device_From,(*SafetyMessage)->m_dwPK_Device_Template,(*SafetyMessage)->m_dwPK_Device_Category_To);
        ErrorResponse(pSocket,(*SafetyMessage));
        return;
    }

    DeviceData_Router *pDeviceFrom = m_mapDeviceData_Router_Find(pMessageWillBeDeleted->m_dwPK_Device_From);
    DeviceData_Router *pDeviceTo = m_mapDeviceData_Router_Find(pMessageWillBeDeleted->m_dwPK_Device_To);

    list<class MessageInterceptorCallBack *>::iterator itInterceptors;
    for(itInterceptors=m_listMessageInterceptor_Global.begin();
        itInterceptors!=m_listMessageInterceptor_Global.end();++itInterceptors)
    {
        class MessageInterceptorCallBack *pMessageInterceptorCallBack = (*itInterceptors);

        class Command_Impl *pPlugIn = pMessageInterceptorCallBack->m_pPlugIn;
        MessageInterceptorFn pMessageInterceptorFn = pMessageInterceptorCallBack->m_pMessageInterceptorFn;

        CALL_MEMBER_FN(*pPlugIn,pMessageInterceptorFn) (pSocket, pMessageWillBeDeleted, pDeviceFrom, pDeviceTo);
    }

    // See if we have a handler for this type of message
    map<int,class MessageTypeInterceptor *>::iterator itMessageType = m_mapMessageTypeInterceptor.find(pMessageWillBeDeleted->m_dwMessage_Type);
    if( itMessageType!=m_mapMessageTypeInterceptor.end() )
    {
        class MessageTypeInterceptor *pMessageTypeInterceptors = (*itMessageType).second;
        CheckInterceptor(pMessageTypeInterceptors,pSocket,pMessageWillBeDeleted,pDeviceFrom,pDeviceTo);
    }
    // Check against all the Interceptors that didn't specify a message type
    itMessageType = m_mapMessageTypeInterceptor.find(0);
    if( itMessageType!=m_mapMessageTypeInterceptor.end() )
    {
        class MessageTypeInterceptor *pMessageTypeInterceptors = (*itMessageType).second;
        CheckInterceptor(pMessageTypeInterceptors,pSocket,pMessageWillBeDeleted,pDeviceFrom,pDeviceTo);
    }

    string Desc="",sCommand="";
    Command *pCommand=NULL;

    if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND )
    {
        Command *pCommand = mapCommand_Find((*SafetyMessage)->m_dwID);
        if( pCommand )
        {
            sCommand = pCommand->m_sDescription;
            Desc = "Command:\x1b[35;1m" + sCommand + "\x1b[0m";
        }
    }
    else if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EVENT )
        Desc = "Event:\x1b[32;1m" + m_mapEventNames[(*SafetyMessage)->m_dwID] + "\x1b[0m";

    int LogType = (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EVENT ? LV_EVENT : ((*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND ? LV_ACTION : LV_STATUS);
    if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EVENT || (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND ) {
        if ((*SafetyMessage)->m_dwPK_Device_To == DEVICEID_LIST)
        {
            string DeviceList;
            string::size_type pos=0;
            while(true)
            {
                string Device = StringUtils::Tokenize((*SafetyMessage)->m_sPK_Device_List_To,",",pos);
                if( !Device.size() )
                    break;
                DeviceData_Router *pDeviceData_Router = m_mapDeviceData_Router[ atoi(Device.c_str()) ];
                if( pDeviceData_Router )
                    DeviceList += pDeviceData_Router->m_sDescription + "(" + Device + "),";
                else
                    DeviceList += "**unknown dev: " + Device + ",";
            }

            g_pPlutoLogger->Write(LogType, "Received Message from %d (\x1b[36;1m%s\x1b[0m) to %s type %d id %d %s, parameters:",
                (*SafetyMessage)->m_dwPK_Device_From,
                (pDeviceFrom ? pDeviceFrom->m_sDescription.c_str() : "unknown"),
                DeviceList.c_str(),
                (*SafetyMessage)->m_dwMessage_Type, (*SafetyMessage)->m_dwID,Desc.c_str());
        }
        else if ((*SafetyMessage)->m_dwPK_Device_To != DEVICEID_LOGGER)
            g_pPlutoLogger->Write(LogType, "Received Message from %d (\x1b[36;1m%s\x1b[0m) to %d (\x1b[36;1m%s\x1b[0m), type %d id %d %s, parameters:",
                (*SafetyMessage)->m_dwPK_Device_From,
                (pDeviceFrom ? pDeviceFrom->m_sDescription.c_str() : "unknown"),(*SafetyMessage)->m_dwPK_Device_To,
                (pDeviceTo ? pDeviceTo->m_sDescription.c_str() : "unknown"),
                (*SafetyMessage)->m_dwMessage_Type, (*SafetyMessage)->m_dwID,Desc.c_str());
    }

    if ((*SafetyMessage)->m_dwPK_Device_To != DEVICEID_LOGGER)
    {
        map<long,string>::iterator i;
        for(i=(*SafetyMessage)->m_mapParameters.begin();i!=(*SafetyMessage)->m_mapParameters.end();++i)
        {
            Desc="";
            if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND )
                Desc = m_mapCommandParmNames[(*i).first];
            else if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EVENT )
                Desc = m_mapEventParmNames[(*i).first];
            if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EVENT || (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND )
                g_pPlutoLogger->Write(LogType, "  Parameter %d(%s): %s", (*i).first, Desc.c_str(), (*i).second.c_str());
        }
    }

    int tempid = (*SafetyMessage)->m_dwPK_Device_To;
    if(tempid == m_dwPK_Device)
        tempid = DEVICEID_DCEROUTER;
    switch(tempid)
    {
    case DEVICEID_LOGGER:
    case DEVICEID_DCEROUTER:
        if ((*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND)
        {
            switch((*SafetyMessage)->m_dwID)
            {
            case SYSCOMMAND_QUIT:
                m_bQuit=true;
                break;
            case SYSCOMMAND_RELOAD:
                m_bReload=true;
                break;
            }
            return;
        }
        else if( (*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_LOG )
        {
            Logger::Entry e;
            e.SerializeRead( (*SafetyMessage)->m_mapData_Lengths[0], (*SafetyMessage)->m_mapData_Parameters[0] );
            g_pPlutoLogger->WriteEntry(e);
        }

        break;

    default:
        if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND && (*SafetyMessage)->m_eExpectedResponse==ER_None )
        {
            // Do this on a separate thread.  Sometimes devices will send a message requesting a response, holding a blocked mutex
            // while the device they're waiting for a response from sends them a command.  It's safer to send messages on separate
            // threads whenever possible
            AddMessageToQueue(SafetyMessage.Detach());
            return;
        }

        RealSendMessage(pSocket,&SafetyMessage);

        break;
    }

    ErrorResponse(pSocket,(*SafetyMessage));
}

bool Router::ReceivedString(Socket *pSocket, string Line)
{
    if (Line.substr(0,4)=="PING")
    {
        pSocket->SendString("PONG");
        return true;
    }
    if (Line.substr(0,5)=="DPING")
    {
#ifdef DEBUG
        clock_t clk = clock();
#endif
        // A DPING request causes the server to turn around
        // and request a ping from the device itself,
        // ensuring that both connections are up.   Will
        // return "FAIL" if the device does not respond
        // to pings on it's command handler channel correctly.
#ifdef DEBUG
        g_pPlutoLogger->Write(LV_STATUS, "%s", Line.c_str());
#endif
        PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);
        int DeviceIDTo = atoi(Line.substr(6).c_str());
        int RouteToDevice = DEVICEID_NULL;
        map<int,int>::iterator iRoute;
        iRoute = m_Routing_DeviceToController.find(DeviceIDTo);
        if (iRoute!=m_Routing_DeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
        slCore.Release();
        string s;
        if (RouteToDevice > 0)
        {
            DeviceClientMap::iterator iDeviceConnection;
            PLUTO_SAFETY_LOCK(slListener,m_CoreMutex);
            iDeviceConnection = m_mapCommandHandlers.find(RouteToDevice);
            slListener.Release();
            if (iDeviceConnection != m_mapCommandHandlers.end())
            {
                ServerSocket *pDeviceConnection = (*iDeviceConnection).second, *pFailedSocket = NULL;
                if( pDeviceConnection )
                {
                    PLUTO_SAFETY_LOCK(slConnMutex,(pDeviceConnection->m_ConnectionMutex))
#ifdef DEBUG
                    g_pPlutoLogger->Write(LV_STATUS, "Got DPING at %d, sending PING at %d",(int) clk,(int) clock());
#endif
                    if (pDeviceConnection->SendString("PING"))
                    {
                        if (pDeviceConnection->ReceiveString(s) && s=="PONG")
                        {
                            pSocket->SendString("DPONG");
#ifdef DEBUG
                            g_pPlutoLogger->Write(LV_STATUS, "%s sent DPONG at %d", Line.c_str(),(int) clock());
#endif
                            return true;
                        }
                    }
                }
            }
        }
        pSocket->SendString("FAIL");
        g_pPlutoLogger->Write(LV_CRITICAL, "%s sent FAIL at %d got %s", Line.c_str(),(int) clock(),s.c_str());
        return true;
    }
    ServerSocket *pServerSocket = /*dynamic_cast<*/(ServerSocket *)/*>*/(pSocket);

    if (Line=="CONFIG")
    {
        class DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(pServerSocket->m_dwPK_Device);
        if (!pDevice)
        {
            pSocket->SendString(string("ERROR"));
            g_pPlutoLogger->Write(LV_CRITICAL, "Device ID %d is requesting its configuration, but it does not exist.", pServerSocket->m_dwPK_Device);
        }
        else
        {
            if( m_bIsLoading && (pDevice->m_dwPK_Device_ControlledVia != m_dwPK_Device || !pDevice->m_bImplementsDCE) )
            {
                pServerSocket->SendString("WAIT");  // We haven't finished loading the plugins
            }
            else
            {
                pServerSocket->SendString(string("CONFIG ")+StringUtils::itos((int)pDevice->m_iConfigSize));
                pServerSocket->SendData(pDevice->m_iConfigSize, pDevice->m_pMySerializedData);
            }
        }
        return true;
    }
    if( Line=="DEVICELIST" )
    {
        pServerSocket->SendString(string("DEVICELIST ")+StringUtils::itos(m_dwIDeviceStructure_Size));
        pServerSocket->SendData(m_dwIDeviceStructure_Size, m_pDeviceStructure);

        return true;
    }
    if (Line=="CORRUPT SOCKET")
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Upon connection, the socket received an invalid response (socket %d).  Terminating.", pSocket->m_Socket);
        closesocket(pSocket->m_Socket);
    }
    if( Line=="CONFIG_TIMESTAMP" )
    {
        string Result = "0";
        struct stat filestat;
        ::stat((m_sBasePath+"DeviceConfig.txt").c_str(), &filestat);
        Result = StringUtils::itos((int) filestat.st_mtime);
        pSocket->SendString(Result);
    }
    g_pPlutoLogger->Write(LV_WARNING, "Router: Don't know how to handle %s.", Line.c_str());
    pSocket->SendString("ERROR");
    return false;
}


void Router::OnDisconnected(int DeviceID)
{
    my_thread_end();
//  mysql_thread_end();
    m_RunningDevices.erase(DeviceID);
}

void Router::RegisteredCommandHandler(int DeviceID)
{
    PLUTO_SAFETY_LOCK(sl,m_CoreMutex);

    my_thread_init();
    DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(DeviceID);
    if( !pDevice )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Device: %d tried to register but it doesn't exist",DeviceID);
        return;
    }

    if( pDevice->m_bForceReloadOnFirstConnect )
    {
        pDevice->m_bForceReloadOnFirstConnect = false;
        pDevice->m_bIsReady=false;
        g_pPlutoLogger->Write(LV_STATUS, "Controller %d's first connection since the server rebooted it, reloading.", DeviceID);
        sl.Release();
        ReceivedMessage(NULL, new Message(0, DeviceID, PRIORITY_NORMAL, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD, 0));
    }

    // This device has registered, so we can always route its messages back to itself
    m_Routing_DeviceToController[DeviceID] = DeviceID;

    pDevice->m_bIsReady=true;
}


void Router::DoReload()
{
    m_bIsLoading=true;

    g_pPlutoLogger->Write(LV_STATUS, "Starting Reload....");

    m_bClosed = true;
    // TODO - reload config
    // This should have stopped everything that is automatically startable
    // Now, restart everything else (like controllers)
    DeviceClientMap::iterator iDC;
    for(iDC = m_mapCommandHandlers.begin(); iDC!=m_mapCommandHandlers.end(); ++iDC)
    {
        ServerSocket *pDeviceConnection = (*iDC).second;
        PLUTO_SAFETY_LOCK(slConnMutex,(pDeviceConnection->m_ConnectionMutex))
        {
            pDeviceConnection->SendMessage(new Message(0, (*iDC).first, PRIORITY_URGENT, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD, 0));
        }
    }

    m_bClosed=false;
    m_bReload=false;
}

bool Router::Run()
{
    m_bIsLoading=true;
    StartListening();
    g_pPlutoLogger->Write(LV_STATUS, "Version: $Id: Router.cpp,v 1.21 2004/10/28 05:11:28 mihaid Exp $");
    RegisterAllPlugins();
    m_bIsLoading=false;
    g_pPlutoLogger->Write(LV_STATUS, "Plug-ins loaded");

    //TODO keep track of the 'status'..and set it here
//  if(m_pCorpClient)
//  {
//      m_pCorpClient->Send_StartupEvent(CORPSERVER_STATUS_OK);
//      SendPackageVersions();
//  }
    // The main loop of this app doesn't do anything!
    while(!m_bQuit && m_bRunning)
    {
        // TODO: Check m_mapCommandHandlers and see if we've lost any
        // command connections.

        if (m_bReload)
        {
            DoReload();
            return true;
        }
        Sleep(1000);

    }
    g_pPlutoLogger->Write(LV_STATUS, "PlutoServer: All Done!");
    return false;
}

void Router::OutputChildren(DeviceData_Impl *pDevice,string &Response)
{
    Response+=StringUtils::itos((int) pDevice->m_vectDeviceData_Impl_Children.size()) + "|";
    for(int i=0;i<(int)pDevice->m_vectDeviceData_Impl_Children.size();++i)
    {
        DeviceData_Impl *pChildDevice = pDevice->m_vectDeviceData_Impl_Children[i];
        Response += StringUtils::itos(pChildDevice->m_dwPK_Device) + "|" + StringUtils::itos(pChildDevice->m_dwPK_DeviceTemplate) + "|";
        OutputChildren(pChildDevice,Response);
    }
}


bool Router::GetVideoFrame(int CameraDevice, void* &ImageData, int &ImageLength)
{
    /* todo
    PLUTO_SAFETY_LOCK(s,m_CoreMutex);
    int RouteToDevice = DEVICEID_NULL;
    map<int,int>::iterator iRoute;
    iRoute = m_Routing_VideoDeviceToController.find(CameraDevice);
    if (iRoute!=m_Routing_VideoDeviceToController.end())
    {
        RouteToDevice = (*iRoute).second;
    }
    if(RouteToDevice == DEVICEID_NULL)
    {
        iRoute = m_Routing_DeviceToController.find(CameraDevice);
        if (iRoute!=m_Routing_DeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
    }
    if (RouteToDevice > 0)
    {
        Message *pMessage = new Message(0,CameraDevice, PRIORITY_LOW, MESSAGETYPE_REQUEST, REQUESTTYPE_VIDEO_FRAME, 3,
                C_DEVICEDATA_DEVICELIST_CONST, StringUtils::itos(CameraDevice).c_str(),
                C_COMMANDPARAMETER_IMAGE_WIDTH_CONST, "140",
                C_COMMANDPARAMETER_IMAGE_HEIGHT_CONST, "105");
        DeviceClientMap::iterator iDeviceConnection;
        iDeviceConnection = m_mapCommandHandlers.find(RouteToDevice);
        if (iDeviceConnection != m_mapCommandHandlers.end())
        {
            ServerSocket *pDeviceConnection = (*iDeviceConnection).second, *pFailedSocket = NULL;
            PLUTO_SAFETY_LOCK(slConnMutex,(pDeviceConnection->m_ConnectionMutex))
            if (!pDeviceConnection->SendMessage(pMessage))
            {
                    g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure sending message to device %d", pDeviceConnection, pDeviceConnection->m_dwPK_Device);
            }
            else
            {
                string sResponse;
                if (pDeviceConnection->ReceiveString(sResponse))
                {
                    if (sResponse.substr(0,7) == "MESSAGE")
                    {
                        Message *pResponse = pDeviceConnection->ReceiveMessage(atoi(sResponse.substr(8).c_str()));
                        if (pResponse)
                        {
                            ImageLength = pResponse->m_mapData_Lengths[C_EVENTPARAMETER_GRAPHIC_IMAGE_CONST];
                            ImageData = pResponse->m_mapData_Parameters[C_EVENTPARAMETER_GRAPHIC_IMAGE_CONST];
                            pResponse->m_mapData_Parameters.clear();
                            delete pResponse;
                            return true;
                        }
                        else
                        {
                            g_pPlutoLogger->Write(LV_WARNING, "%d GetVideoFrame, failed to receive message.", pDeviceConnection->m_dwPK_Device);
                        }
                    }
                    else
                    {
                        g_pPlutoLogger->Write(LV_WARNING, "%d GetVideoFrame, Destination responded with %s.", pDeviceConnection->m_dwPK_Device, sResponse.c_str());
                    }
                }
            }
        }
    }
*/
    return false;
}


bool Router::GetParameter(int ToDevice,int ParmType, string &sResult)
{
    PLUTO_SAFETY_LOCK(slock,m_CoreMutex);
    ServerSocket *pSocket=NULL;

    map<int,int>::iterator iRoute = m_Routing_DeviceToController.find(ToDevice);
    if (iRoute!=m_Routing_DeviceToController.end())
    {
        PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
        DeviceClientMap::iterator iDeviceConnection = m_mapCommandHandlers.find((*iRoute).second);
        if (iDeviceConnection != m_mapCommandHandlers.end())
        {
            pSocket = (*iDeviceConnection).second;
        }
    }

    slock.Release();
    if( pSocket )
    {
        PLUTO_SAFETY_LOCK(slConn,pSocket->m_ConnectionMutex);
        if (!pSocket->SendMessage(new Message(0, ToDevice, PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_REQUEST, ParmType, 0)))
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure sending dataparm request to device %d",
                pSocket, ToDevice);

            // TODO :  The socket failed, core needs to remove client socket

        }
        if (pSocket->ReceiveString(sResult) && sResult.substr(0,7)=="MESSAGE")
        {
            Message *pMessage=pSocket->ReceiveMessage(atoi(sResult.substr(8).c_str()));
            if (pMessage)
            {
                sResult = pMessage->m_mapParameters[ParmType];
                delete pMessage;
                return true;
            }
        }
    }
    return false;
}

bool Router::GetParameterWithDefinedMessage(Message *sendMessage, string &sResult)
{
  int messageID;
  PLUTO_SAFETY_LOCK(slock,m_CoreMutex);

    g_pPlutoLogger->Write(LV_CRITICAL, "GetParameterWithDefinedMessage %s", StringUtils::itos(sendMessage->m_dwID).c_str());
  messageID = sendMessage->m_dwID;

    map<int,int>::iterator iRoute = m_Routing_DeviceToController.find(sendMessage->m_dwPK_Device_To);
    if (iRoute!=m_Routing_DeviceToController.end())
    {
        PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
        DeviceClientMap::iterator iDeviceConnection = m_mapCommandHandlers.find((*iRoute).second);
        ServerSocket *pSocket = (*iDeviceConnection).second;
        if (iDeviceConnection != m_mapCommandHandlers.end())
        {
            PLUTO_SAFETY_LOCK(slConn,((*iDeviceConnection).second->m_ConnectionMutex));
            if (!pSocket->SendMessage(sendMessage)) //This evidentally overwrite sendMessage, do not use sendMessage after here
            {
                g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure sending dataparm request to device %d", (*iDeviceConnection).second, sendMessage->m_dwPK_Device_To);

                // TODO :  The socket failed, core needs to remove client socket

            }
            if (pSocket->ReceiveString(sResult) && sResult.substr(0,7)=="MESSAGE")
            {
                Message *pMessage=pSocket->ReceiveMessage(atoi(sResult.substr(8).c_str()));
                if (pMessage)
                {
                    sResult = pMessage->m_mapParameters[messageID];
                    delete pMessage;
                    return true;
                }
            }
        }
    }
    return false;
}

int Router::FindClosestRelative(int MasterDeviceType, int CurrentDevice)
{
    DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(CurrentDevice);
    if( pDevice )
    {
        int DeviceID = pDevice->m_dwPK_Device_ControlledVia;
        if( DeviceID && (pDevice = m_mapDeviceData_Router_Find(DeviceID)))
        {
            for(int i=0;i<(int)pDevice->m_vectDeviceData_Impl_Children.size();++i)
            {
                DeviceData_Impl *pChildDevice = pDevice->m_vectDeviceData_Impl_Children[i];
                if (pChildDevice->m_dwPK_DeviceTemplate == MasterDeviceType)
                {
                    return pChildDevice->m_dwPK_Device;
                }
            }
            // Not here.  Go up one.
            return FindClosestRelative(MasterDeviceType, DeviceID);
        }
    }
    return -1;
}

string Router::GetPublicIP()
{
    g_pPlutoLogger->Write(LV_STATUS,"Trying to obtain public IP");
#ifndef WIN32
    char buf[20];
    FILE* f = popen("wget -q -O - http://www.1control.com/cgi-bin/remoteaddress.pl", "r");
    if(f == NULL)
    {
#endif
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't get the my public IP");
        return "";
#ifndef WIN32
    }
    else
    {
        fgets(buf, sizeof(buf), f);
        fclose(f);
        size_t len = strlen(buf);
        if(buf[len-1] == '\n');
            buf[len-1] = 0;
        g_pPlutoLogger->Write(LV_CORPCLIENT, "My public IP is '%s'",buf);
        return string(buf);
    }
#endif
}


void Router::CleanFileName(string &FileName)
{
    string::size_type s;
    while( (s=FileName.find(':'))!=string::npos )
        FileName.replace(s,1,"-");

    while( (s=FileName.find('\''))!=string::npos )
        FileName.replace(s,1,"");

    while( (s=FileName.find('\"'))!=string::npos )
        FileName.replace(s,1,"");
}

bool Router::DeviceIsRegistered(int PK_Device)
{
    PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);

    int RouteToDevice = DEVICEID_NULL;

    map<int,int>::iterator iRoute;
    iRoute = m_Routing_DeviceToController.find(PK_Device);
    if (iRoute!=m_Routing_DeviceToController.end())
    {
        RouteToDevice = (*iRoute).second;
    }
    slCore.Release();

    if (!RouteToDevice)
        return false;

    DeviceClientMap::iterator iDeviceConnection;
    PLUTO_SAFETY_LOCK(slListener,m_CoreMutex);
    iDeviceConnection = m_mapCommandHandlers.find(RouteToDevice);
    slListener.Release();
    return iDeviceConnection != m_mapCommandHandlers.end();
}


void Router::AddMessageToQueue(Message *pMessage)
{
    /*
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS,"adding message from %d to %d type %d id %d to queue size was: %d",
        pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) m_MessageQueue.size());
#endif
    */
    pthread_mutex_lock(&m_MessageQueueMutex);
    m_MessageQueue.push_back(pMessage);
    pthread_mutex_unlock(&m_MessageQueueMutex);
    pthread_cond_broadcast(&m_MessageQueueCond);
}

void Router::ProcessQueue()
{
    pthread_mutex_lock(&m_MessageQueueMutex);
    while(true)
    {
        while( m_MessageQueue.size()==0 )
        {
            pthread_cond_wait(&m_MessageQueueCond,&m_MessageQueueMutex);
        }

        // We are holding the mutex
        Message *pMessage = m_MessageQueue.front();
        m_MessageQueue.pop_front();
        g_pPlutoLogger->Write(LV_STATUS,"sending message from %d to %d type %d id %d to queue now size: %d",
            pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) m_MessageQueue.size());
        pthread_mutex_unlock(&m_MessageQueueMutex);

        SafetyMessage pSafetyMessage(pMessage);
//      g_pPlutoLogger->Write(LV_STATUS,"Calling realsendmessage from queue");
        RealSendMessage(NULL,&pSafetyMessage);
        pthread_mutex_lock(&m_MessageQueueMutex);
//      g_pPlutoLogger->Write(LV_STATUS,"finished Calling realsendmessage from queue");
    }
}

void Router::HandleCommandPipes(Socket *pSocket,SafetyMessage *pSafetyMessage)
{
	Command *pCommand = mapCommand_Find((*(*pSafetyMessage))->m_dwID);
	int PK_Device = (*(*pSafetyMessage))->m_dwPK_Device_To;
	DeviceData_Router *pDeviceData_Router = m_mapDeviceData_Router_Find(PK_Device);
	if( !pCommand || !pDeviceData_Router )
	{
		if( pCommand && pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST && PK_Device==DEVICEID_NULL )
			m_bQuit=true;
		return;
	}

	if( pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST )
	{
		for(map<int,Pipe *>::iterator it=pDeviceData_Router->m_mapPipe_Available.begin();it!=pDeviceData_Router->m_mapPipe_Available.end();++it)
		{
			Pipe *pPipe = (*it).second;
			Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get(),
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0);
			ReceivedMessage(NULL,pMessage);
		}
		pDeviceData_Router->m_mapPipe_Active.clear();
	}
	else if( pCommand->m_dwPK_Command==COMMAND_Generic_On_CONST )
	{
		bool bDontSetInputs=false;
		if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_Dont_Set_Inputs_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() ||
			(*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_Dont_Set_Inputs_CONST]=="1") 
				bDontSetInputs=true;

		string sPipesDevices;
		if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PK_Device_Pipes_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
			sPipesDevices = "," + (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST] + ",";
		for(map<int,Pipe *>::iterator it=pDeviceData_Router->m_mapPipe_Available.begin();it!=pDeviceData_Router->m_mapPipe_Available.end();++it)
		{
			Pipe *pPipe = (*it).second;
			if( sPipesDevices.length()==0 || sPipesDevices.find("," + StringUtils::itos((*it).first) + ",")!=string::npos )
				pDeviceData_Router->m_mapPipe_Active[pPipe->m_pRow_Device_Device_Pipe->FK_Pipe_get()]=pPipe;

			if( !bDontSetInputs )
			{
				if( !pPipe->m_pRow_Device_Device_Pipe->FK_Command_Input_isNull() )
				{
					Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get(),
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,0);

					Message *pMessage2 = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get(),
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Input_Select_CONST,1,
						COMMANDPARAMETER_PK_Command_Input_CONST,StringUtils::itos(pPipe->m_pRow_Device_Device_Pipe->FK_Command_Input_get()).c_str());
					pMessage->m_vectExtraMessages.push_back(pMessage2);
					ReceivedMessage(NULL,pMessage);
				}
				if( !pPipe->m_pRow_Device_Device_Pipe->FK_Command_Output_isNull() )
				{
					Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get(),
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Output_Select_CONST,1,
						COMMANDPARAMETER_PK_Command_Output_CONST,StringUtils::itos(pPipe->m_pRow_Device_Device_Pipe->FK_Command_Output_get()).c_str());
					ReceivedMessage(NULL,pMessage);
				}
			}
		}
	}
	else if( pCommand->m_listPipe.size() )
	{
		for(list<int>::iterator it=pCommand->m_listPipe.begin();it!=pCommand->m_listPipe.end();++it)
		{
			Pipe *pPipe;
			if( (pPipe=pDeviceData_Router->m_mapPipe_Active_Find(*it))!=NULL )
			{
				(*(*pSafetyMessage))->m_dwPK_Device_To = pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get();
				RealSendMessage(pSocket,pSafetyMessage);
			}
			(*(*pSafetyMessage))->m_dwPK_Device_To = PK_Device;
		}
	}
}

void Router::RealSendMessage(Socket *pSocket,SafetyMessage *pSafetyMessage)
{
    if ( (*(*pSafetyMessage))->m_dwPK_Device_To==DEVICEID_LIST )
    {
        // Loop back and call ourselves for each message
        string::size_type pos=0;
        string sPK_Device;
        while( (sPK_Device=StringUtils::Tokenize((*(*pSafetyMessage))->m_sPK_Device_List_To,",",pos)).length() )
        {
            (*(*pSafetyMessage))->m_dwPK_Device_To=atoi(sPK_Device.c_str());
            RealSendMessage(pSocket,pSafetyMessage);
        }
        return;
    }

	// We have to do this here after we've parsed the DEVICEID_LIST
    DeviceData_Router *pDeviceTo = m_mapDeviceData_Router_Find((*(*pSafetyMessage))->m_dwPK_Device_To);
	if( pDeviceTo && pDeviceTo->m_pDevice_RouteTo )
		(*(*pSafetyMessage))->m_dwPK_Device_To=pDeviceTo->m_pDevice_RouteTo->m_dwPK_Device;

    if ( (*(*pSafetyMessage))->m_dwMessage_Type==MESSAGETYPE_COMMAND )
		HandleCommandPipes(pSocket,pSafetyMessage);
//  g_pPlutoLogger->Write(LV_STATUS,"begin realsendmessage before lock");
    PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);
    int RouteToDevice = DEVICEID_NULL;

//  g_pPlutoLogger->Write(LV_STATUS,"realsendmessage from %d to %d type %d id %d ",
//      (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);

    map<int,int>::iterator iRoute;
    /* NOT needed for 2.0  -- a video plug-in will handle this
    if ( (*(*pSafetyMessage))->m_dwMessage_Type == MESSAGETYPE_REQUEST && (*(*pSafetyMessage))->m_dwID == REQUESTTYPE_VIDEO_FRAME)
    {
        iRoute = m_Routing_VideoDeviceToController.find((*(*pSafetyMessage))->m_dwPK_Device_To);
        if (iRoute!=m_Routing_VideoDeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
    }
    */
    if(RouteToDevice == DEVICEID_NULL)
    {
        iRoute = m_Routing_DeviceToController.find((*(*pSafetyMessage))->m_dwPK_Device_To);
        if (iRoute!=m_Routing_DeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
    }
    slCore.Release();
//g_pPlutoLogger->Write(LV_STATUS,"realsendmessage after core release from %d to %d type %d id %d ",
//  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);

    if (RouteToDevice > 0)
    {
        DeviceClientMap::iterator iDeviceConnection;
        PLUTO_SAFETY_LOCK(slListener,m_CoreMutex);
        iDeviceConnection = m_mapCommandHandlers.find(RouteToDevice);
        slListener.Release();
        if (iDeviceConnection != m_mapCommandHandlers.end())
        {
            ServerSocket *pDeviceConnection = (*iDeviceConnection).second, *pFailedSocket = NULL;

            if( !pDeviceConnection )
            {
                g_pPlutoLogger->Write(LV_CRITICAL,"pDeviceconnection is null in Router::receivedocmessage from: %d, to: %d, type: %d",
                    (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type);
                ErrorResponse(pSocket,(*(*pSafetyMessage)));
                return;
            }

            {
//g_pPlutoLogger->Write(LV_STATUS,"realsendmessage before device conn mutex from %d to %d type %d id %d ",
//  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);

#ifdef DEBUG
                DeviceData_Router *pDest = m_mapDeviceData_Router_Find(pDeviceConnection->m_dwPK_Device);
                if( !pDest )
                {
                    g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find destination device: %d",pDeviceConnection->m_dwPK_Device);
                }
                g_pPlutoLogger->Write(LV_SOCKET, "Ready to send message type %d id %d to %d %s on socket %d(%p) using lock: %p",
                    (*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID,pDeviceConnection->m_dwPK_Device,
                    (pDest ? pDest->m_sDescription.c_str() : "*UNKNOWN DEVICE*"),
                    pDeviceConnection->m_iSocketCounter,pDeviceConnection,&pDeviceConnection->m_ConnectionMutex);

                string Message = "Device " + StringUtils::itos(pDeviceConnection->m_dwPK_Device) + " " + (pDest ? pDest->m_sDescription : "*UNKNOWN DEVICE*");
                PLUTO_SAFETY_LOCK_MESSAGE(slConnMutex,pDeviceConnection->m_ConnectionMutex,Message)
#else
                PLUTO_SAFETY_LOCK(slConnMutex,pDeviceConnection->m_ConnectionMutex)
#endif

//g_pPlutoLogger->Write(LV_STATUS,"realsendmessage after device conn mutex from %d to %d type %d id %d ",
//  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);

#ifdef DEBUG
                // Let's see how long it takes to send
                clock_t clk = clock();
#endif
    #ifdef LL_DEBUG
                g_pPlutoLogger->Write(LV_SOCKET, "%d Sending....", pDeviceConnection->m_dwPK_Device);
    #endif
                int MessageType = (*(*pSafetyMessage))->m_dwMessage_Type;
                int ID = (*(*pSafetyMessage))->m_dwID;
                /* AB 8/5, changed this since we still use the message later
                bool bResult = pDeviceConnection->SendMessage(pSafetyMessage->Detach()); */
                bool bResult = pDeviceConnection->SendMessage(pSafetyMessage->m_pMessage,false);
/*
#ifdef DEBUG
                g_pPlutoLogger->Write(LV_SOCKET, "Response %d to realsendmessage type %d id %d socket %d using lock: %p  expected response: %d",
                    bResult ? 1 : 0,MessageType,ID,pDeviceConnection->m_Socket,&pDeviceConnection->m_ConnectionMutex,(int) (*(*pSafetyMessage))->m_eExpectedResponse);
#endif
*/
#ifdef DEBUG
g_pPlutoLogger->Write(LV_SOCKET, "Got response: %d to message type %d id %d to %d %s on socket %d using lock: %p",
	(bResult ? 1 : 0),
    (*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID,pDeviceConnection->m_dwPK_Device,
    (pDest ? pDest->m_sDescription.c_str() : "*UNKNOWN DEVICE*"),
    pDeviceConnection->m_Socket,&pDeviceConnection->m_ConnectionMutex);
#endif
                if (!bResult)
                {
                    g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure sending message to device %d", pDeviceConnection,pDeviceConnection->m_dwPK_Device);
                    pFailedSocket = pDeviceConnection;
                }
                else
                {
#ifdef DEBUG
                    clock_t clk2 = clock();
                    if( clk2-clk>(CLOCKS_PER_SEC*4) )
                        g_pPlutoLogger->Write(LV_CRITICAL,"Took %d secs (%d ticks) to send message to %d",(int) ((clk2-clk)/CLOCKS_PER_SEC),(int) (clk2-clk),pDeviceConnection->m_dwPK_Device);
#endif

                    if( (*(*pSafetyMessage))->m_eExpectedResponse==ER_None )
                    {
                        g_pPlutoLogger->Write(LV_SOCKET,"No response expected.  returning");
                        return;
                    }
                    string sResponse;
                    if (pDeviceConnection->ReceiveString(sResponse))
                    {

if ( ID == 54 )
{
    g_pPlutoLogger->Write(LV_WARNING, "Got a String for 54: %s", sResponse.c_str());
}

/*
#ifdef DEBUG
                        if( clock()-clk2>(CLOCKS_PER_SEC*4) )
                            g_pPlutoLogger->Write(LV_CRITICAL,"Took %d secs (ticks: %d) to receive response from %d",(int) ((clock()-clk2)/CLOCKS_PER_SEC),(int) (clock()-clk2),pDeviceConnection->m_dwPK_Device);
                        g_pPlutoLogger->Write(LV_STATUS, "%d Destination realsendmessage responded with %s.", pDeviceConnection->m_dwPK_Device, sResponse.c_str());
#endif
*/
                        if (sResponse.substr(0,7)  == "MESSAGE")
                        {
                            g_pPlutoLogger->Write(LV_STATUS, "1 pSocket=%p", pSocket);
                            if (pSocket && !(*(*pSafetyMessage))->m_bRespondedToMessage)
                            {
if ( ID == 54 )
{
    g_pPlutoLogger->Write(LV_WARNING, "Reading message response id for %s", sResponse.substr(8).c_str());
}
								DCE::Message *pMessage = pDeviceConnection->ReceiveMessage(atoi(sResponse.substr(8).c_str()));
if ( ID == 54 )
{
    g_pPlutoLogger->Write(LV_WARNING, "Response message was read (%d) with %d params: %s %s", pMessage->m_dwID,
        pMessage->m_mapParameters.size(),
        pMessage->m_mapParameters[13].c_str(),
        pMessage->m_mapParameters[59].c_str());
//      pMessage->m_mapParameters);
}

if ( pSocket->SendMessage(pMessage) == false && ID == 54)
{
    g_pPlutoLogger->Write(LV_WARNING, "Failed to forward response message properly" );
}
                                (*(*pSafetyMessage))->m_bRespondedToMessage = true;
                            }
                        }
                        else if (sResponse.substr(0,7)=="BYE" && MessageType==MESSAGETYPE_SYSCOMMAND )
                        {
                            // delete the other sockets
                            g_pPlutoLogger->Write(LV_STATUS, "Got BYE in response to message.  Dropping socket");
                            DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(pDeviceConnection->m_dwPK_Device);
                            // cause the server to remove the socket
// AB 1-4-2004 TODO -- have to fix this.  it crashed???
//                              pFailedSocket = pDeviceConnection;
//                              FlagForRemoval(pDeviceConnection->m_dwPK_Device);
                        }
                        else
                        {
                            if( ( (*(*pSafetyMessage))->m_eExpectedResponse==ER_DeliveryConfirmation || (*(*pSafetyMessage))->m_eExpectedResponse==ER_ReplyString )
                                && !(*(*pSafetyMessage))->m_bRespondedToMessage && pSocket)
                            {
                                (*(*pSafetyMessage))->m_bRespondedToMessage=true;
                                pSocket->SendString(sResponse);  // Will be okay if it was successful
                            }
                            // If the socket was waiting for a message, we'll just report an error at the end of the loop if a response wasn't already sent
                            g_pPlutoLogger->Write(LV_STATUS, "3 iMessageType= %d pSocket=%p ",MessageType, pSocket);
                            // This wasn't a request, so the sender is not concerned with the response.  No need to keep it waiting
                            // around for a reponse. We'll just log it.
    //                      g_pPlutoLogger->Write(LV_CRITICAL, "Destination responded with %s.", sResponse.c_str());
                        }
                    }
                    else
                    {
                        g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure waiting for response to message from device %d",
                            pDeviceConnection,pDeviceConnection->m_dwPK_Device );
                        pFailedSocket = pDeviceConnection;
                    }
                }
            }
//          g_pPlutoLogger->Write(LV_STATUS, "realsendmessage - before failed socket");
            if (pFailedSocket)
            {
                RemoveSocket(pFailedSocket);
            }
        }
        else
        {
            g_pPlutoLogger->Write(LV_WARNING, "The target device %d has not registered.",(*(*pSafetyMessage))->m_dwPK_Device_To);
            // If this is a request we have to let the sender know so it doesn't wait in vain.
            // also, send responses back to corpserver
        }
    }
    ErrorResponse(pSocket,(*(*pSafetyMessage)));
//  g_pPlutoLogger->Write(LV_STATUS, "realsendmessage - end");
}

// Will send an error if somebody's waiting for a response and didn't get it
void Router::ErrorResponse(Socket *pSocket,Message *pMessage)
{
    if( pMessage->m_eExpectedResponse==ER_None || pMessage->m_bRespondedToMessage || !pSocket )
        return;  // No response necessary
    if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
    {
        pMessage->m_bRespondedToMessage=true;
        g_pPlutoLogger->Write(LV_CRITICAL,"Sender: %d sent message type: %d ID: %d and expected a message reply",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID);
        Message *pMessageOut=new Message(0,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,-1,0);
        pSocket->SendMessage(pMessageOut);
    }
    if( pMessage->m_eExpectedResponse==ER_ReplyString )
    {
        pMessage->m_bRespondedToMessage=true;
        g_pPlutoLogger->Write(LV_CRITICAL,"Sender: %d sent message type: %d ID: %d and expected a reply string",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID);
        pSocket->SendString("No Response");
    }
    if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation )
    {
        pMessage->m_bRespondedToMessage=true;
        g_pPlutoLogger->Write(LV_CRITICAL,"Sender: %d sent message type: %d ID: %d and expected a confirmation",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID);
        pSocket->SendString("Delivery failed");
    }
}


void Router::ParseDevice(int MasterDeviceID, int ParentDeviceID, class DeviceData_Impl *pDevice)
{
//  if( pDevice->m_dwPK_DeviceCategory == DEVICETEMPLATE_Pluto_Core_CONST )
//      m_dwPK_Device = pDevice->m_dwPK_Device;

    if ( !pDevice->WithinCategory( DEVICECATEGORY_Computers_CONST ) && MasterDeviceID == DEVICEID_DCEROUTER )
    {
        MasterDeviceID = pDevice->m_dwPK_Device;
    }

    if (MasterDeviceID == DEVICEID_DCEROUTER)
    {
        g_pPlutoLogger->Write(LV_STATUS,"Created the server device: %d %s",pDevice->m_dwPK_Device,pDevice->m_sDescription.c_str());
        m_Routing_DeviceToController[pDevice->m_dwPK_Device] = pDevice->m_dwPK_Device;
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS,"Created device %d %s (mdl: %d) routed to: %d",pDevice->m_dwPK_Device,
            pDevice->m_sDescription.c_str(),pDevice->m_dwPK_DeviceTemplate,MasterDeviceID);
        m_Routing_DeviceToController[pDevice->m_dwPK_Device] = MasterDeviceID;
    }

    for(size_t i=0;i<pDevice->m_vectDeviceData_Impl_Children.size();i++)
    {
        DeviceData_Impl *pChild = pDevice->m_vectDeviceData_Impl_Children[i];
        ParseDevice(MasterDeviceID, pDevice->m_dwPK_Device, pChild);
    }
}

void Router::Configure()
{
    AllDevices allDevices; // We're going to want a serialized copy of all the device information

    // Get the rooms
    vector<Row_Room *> vectRow_Room;
    GetDatabase()->Room_get()->GetRows("1=1",&vectRow_Room);  // All rows
    for(size_t s=0;s<vectRow_Room.size();++s)
    {
        Row_Room *pRow_Room = vectRow_Room[s];
        Room *pRoom = new Room(pRow_Room->PK_Room_get(),pRow_Room->Description_get());
        m_mapRoom[pRoom->m_dwPK_Room] = pRoom;
    }

    // Get the device categories
    vector<Row_DeviceCategory *> vectDeviceCategories;
    GetDatabase()->DeviceCategory_get()->GetRows("1=1",&vectDeviceCategories);  // All rows
    for(size_t s=0;s<vectDeviceCategories.size();++s)
    {
        Row_DeviceCategory *pRow_DeviceCategory = vectDeviceCategories[s];
        DeviceCategory *pDeviceCategory = new DeviceCategory(pRow_DeviceCategory->PK_DeviceCategory_get(),
            pRow_DeviceCategory->FK_DeviceCategory_Parent_get(),pRow_DeviceCategory->Description_get());

        m_mapDeviceCategory[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;
        allDevices.m_mapDeviceCategory[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;  // All devices will want this structure too
    }

    // Map them up to the parents
    Map_DeviceCategory::iterator itCat;
    for(itCat=m_mapDeviceCategory.begin();itCat!=m_mapDeviceCategory.end();++itCat)
    {
        DeviceCategory *pCat = (*itCat).second;
        if( pCat->m_dwPK_DeviceCategory_Parent )
        {
            DeviceCategory *pParent = m_mapDeviceCategory_Find(pCat->m_dwPK_DeviceCategory_Parent);
            pCat->m_pDeviceCategory_Parent = pParent;
        }
    }

    vector<Row_Device *> vectDevices;
    GetDatabase()->Device_get()->GetRows(
        string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation),&vectDevices);

    for(size_t s=0;s<vectDevices.size();++s)
    {
        Row_Device *pRow_Device = vectDevices[s];

        string CommandLine=pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get();
        if( CommandLine.size()==0 && pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get() )
            CommandLine = FileUtils::ValidCPPName(pRow_Device->FK_DeviceTemplate_getrow()->Description_get());

        DeviceData_Router *pDevice = new DeviceData_Router(pRow_Device,m_mapRoom_Find(pRow_Device->FK_Room_get()),CommandLine);

        m_mapDeviceData_Router[pDevice->m_dwPK_Device]=pDevice;

		vector<Row_Device_Device_Pipe *> vectRow_Device_Device_Pipe;
		pRow_Device->Device_Device_Pipe_FK_Device_From_getrows(&vectRow_Device_Device_Pipe);
		for(size_t sp=0;sp<vectRow_Device_Device_Pipe.size();++sp)
		{
			Row_Device_Device_Pipe *pRow_Device_Device_Pipe = vectRow_Device_Device_Pipe[sp];
			pDevice->m_mapPipe_Available[pRow_Device_Device_Pipe->FK_Pipe_get()] = new Pipe(pRow_Device_Device_Pipe);
		}

		ListDeviceData_Router *pListDeviceData_Router = m_mapDeviceByTemplate_Find(pDevice->m_dwPK_DeviceTemplate);
        if( !pListDeviceData_Router )
        {
            pListDeviceData_Router = new ListDeviceData_Router();
            m_mapDeviceByTemplate[pDevice->m_dwPK_DeviceTemplate] = pListDeviceData_Router;
        }
        pListDeviceData_Router->push_back(pDevice);

        pListDeviceData_Router = m_mapDeviceByCategory_Find(pDevice->m_dwPK_DeviceCategory);
        if( !pListDeviceData_Router )
        {
            pListDeviceData_Router = new ListDeviceData_Router();
            m_mapDeviceByCategory[pDevice->m_dwPK_DeviceCategory] = pListDeviceData_Router;
        }
        pListDeviceData_Router->push_back(pDevice);

		// We also need a minimal version from the topmost base class that we'll serialize in alldevices and send to each device that registers so they have a complete device tree
        DeviceData_Base *pDevice_Base = new DeviceData_Base(
            pRow_Device->PK_Device_get(),pRow_Device->FK_Installation_get(),pRow_Device->FK_DeviceTemplate_get(),pRow_Device->FK_Device_ControlledVia_get(),
            pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get(),pRow_Device->FK_Room_get(),pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get()==1,pRow_Device->FK_DeviceTemplate_getrow()->IsEmbedded_get()==1,
            pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get(),pRow_Device->FK_DeviceTemplate_getrow()->IsPlugIn_get()==1,pRow_Device->Description_get(), pRow_Device->IPaddress_get(),pRow_Device->MACaddress_get());
        allDevices.m_mapDeviceData_Base[pDevice_Base->m_dwPK_Device]=pDevice_Base;

        // Get a list of all the commands each device supports
        vector<Row_DeviceTemplate_DeviceCommandGroup *> vectRow_DeviceTemplate_DeviceCommandGroup;
        pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_DeviceCommandGroup);
        for(size_t cmds=0;cmds<vectRow_DeviceTemplate_DeviceCommandGroup.size();++cmds)
        {
            Row_DeviceTemplate_DeviceCommandGroup *pRow_DeviceTemplate_DeviceCommandGroup = vectRow_DeviceTemplate_DeviceCommandGroup[cmds];
            // Get all the commands in this device group
            vector<Row_DeviceCommandGroup_Command *> vectRow_DeviceCommandGroup_Command;
            pRow_DeviceTemplate_DeviceCommandGroup->FK_DeviceCommandGroup_getrow()->DeviceCommandGroup_Command_FK_DeviceCommandGroup_getrows(&vectRow_DeviceCommandGroup_Command);
            for(size_t cmd=0;cmd<vectRow_DeviceCommandGroup_Command.size();++cmd)
            {
                Row_DeviceCommandGroup_Command *pRow_DeviceCommandGroup_Command = vectRow_DeviceCommandGroup_Command[cmd];

                pDevice_Base->m_mapCommands[pRow_DeviceCommandGroup_Command->FK_Command_get()] = pRow_DeviceCommandGroup_Command->FK_Command_getrow()->Description_get();
                pDevice->m_mapCommands[pRow_DeviceCommandGroup_Command->FK_Command_get()] = pRow_DeviceCommandGroup_Command->FK_Command_getrow()->Description_get();
            }

        }

        pDevice->m_pDeviceCategory=m_mapDeviceCategory_Find(pDevice->m_dwPK_DeviceCategory);

        vector<Row_Device_DeviceData *> vectDeviceParms;
        GetDatabase()->Device_DeviceData_get()->GetRows(
            string(DEVICE_DEVICEDATA_FK_DEVICE_FIELD) + "=" + StringUtils::itos(pDevice->m_dwPK_Device),&vectDeviceParms);

        for(size_t s2=0;s2<vectDeviceParms.size();++s2)
        {
            Row_Device_DeviceData *pRow_DeviceParm = vectDeviceParms[s2];
            pDevice->m_mapParameters[ pRow_DeviceParm->FK_DeviceData_get()] = pRow_DeviceParm->IK_DeviceData_get();
        }
    }

    // Now match up children to parents
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
        DeviceData_Router *pDevice_Parent = m_mapDeviceData_Router_Find(pDevice->m_dwPK_Device_ControlledVia);
        if( pDevice_Parent )
        {
            pDevice->m_pDevice_ControlledVia = pDevice_Parent;
            pDevice_Parent->m_vectDeviceData_Impl_Children.push_back(pDevice);
        }
    }

	// Now match up route to's
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
        DeviceData_Router *pDevice_RouteTo = m_mapDeviceData_Router_Find(pDevice->m_pRow_Device->FK_Device_RouteTo_get());
        if( pDevice_RouteTo )
            pDevice->m_pDevice_RouteTo = pDevice_RouteTo;
    }

    // Get the device groups
    vector<Row_DeviceGroup *> vectDeviceGroup;
	GetDatabase()->DeviceGroup_get()->GetRows(string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation),&vectDeviceGroup);

    for(size_t s=0;s<vectDeviceGroup.size();++s)
    {
        Row_DeviceGroup *pRow_DeviceGroup = vectDeviceGroup[s];
        DeviceGroup *pDeviceGroup = new DeviceGroup(pRow_DeviceGroup->PK_DeviceGroup_get(),pRow_DeviceGroup->Description_get());

		vector<Row_Device_DeviceGroup *> vectRow_Device_DeviceGroup;
		pRow_DeviceGroup->Device_DeviceGroup_FK_DeviceGroup_getrows(&vectRow_Device_DeviceGroup);
		for(size_t s2=0;s2<vectRow_Device_DeviceGroup.size();++s2)
		{
			Row_Device_DeviceGroup *pRow_Device_DeviceGroup = vectRow_Device_DeviceGroup[s2];
			pDeviceGroup->m_vectPK_Device.push_back(pRow_Device_DeviceGroup->FK_Device_get());
		}
        allDevices.m_mapDeviceGroup[pDeviceGroup->m_dwPK_DeviceGroup] = pDeviceGroup;
    }

    // Serialize everything
    allDevices.SerializeWrite();
    m_dwIDeviceStructure_Size=(unsigned long) allDevices.CurrentSize();
    m_pDeviceStructure=new char[m_dwIDeviceStructure_Size];
    memcpy(m_pDeviceStructure,allDevices.m_pcDataBlock,m_dwIDeviceStructure_Size);

    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;

        pDevice->DeviceData_Impl::SerializeWrite();   // Have each device pre-serialize all it's data
        pDevice->m_pMySerializedData=pDevice->m_pcDataBlock;  // When we serialize this device's children or parents, this would get overwritten if we didn't make a copy
        pDevice->m_iConfigSize = (unsigned long) (pDevice->m_pcCurrentPosition-pDevice->m_pcDataBlock);
        pDevice->m_pcDataBlock=NULL; // So the framework doesn't delete this when we serialize another object in the chain
        if( !pDevice->m_pDevice_ControlledVia )
            ParseDevice(DEVICEID_DCEROUTER, DEVICEID_DCEROUTER,pDevice);
    }

    // Build some static arrays
    vector<Row_Command *> vectRow_Command;
    GetDatabase()->Command_get()->GetRows("1=1",&vectRow_Command);  // All rows
    for(size_t s=0;s<vectRow_Command.size();++s)
    {
        Row_Command *pRow_Command = vectRow_Command[s];
		Command *pCommand = new Command(pRow_Command->PK_Command_get(),pRow_Command->Description_get());
		vector<Row_Command_Pipe *> vectRow_Command_Pipe;
		pRow_Command->Command_Pipe_FK_Command_getrows(&vectRow_Command_Pipe);
		for(size_t sp=0;sp<vectRow_Command_Pipe.size();++sp)
		{
			Row_Command_Pipe *pRow_Command_Pipe = vectRow_Command_Pipe[sp];
			pCommand->m_listPipe.push_back( pRow_Command_Pipe->FK_Pipe_get() );
		}
        m_mapCommand[pRow_Command->PK_Command_get()]=pCommand;
    }

	// Build all the command groups
    vector<Row_CommandGroup *> vectRow_CommandGroup;
    GetDatabase()->CommandGroup_get()->GetRows("1=1",&vectRow_CommandGroup);  // All rows
    for(size_t s=0;s<vectRow_CommandGroup.size();++s)
    {
        Row_CommandGroup *pRow_CommandGroup = vectRow_CommandGroup[s];
		CommandGroup *pCommandGroup = new CommandGroup(pRow_CommandGroup->PK_CommandGroup_get(),pRow_CommandGroup->FK_Array_get());
		m_mapCommandGroup[pRow_CommandGroup->PK_CommandGroup_get()]=pCommandGroup;
		vector<Row_CommandGroup_Command *> vectRow_CommandGroup_Command;
		pRow_CommandGroup->CommandGroup_Command_FK_CommandGroup_getrows(&vectRow_CommandGroup_Command);
		for(size_t s2=0;s2<vectRow_CommandGroup_Command.size();++s2)
		{
			Row_CommandGroup_Command *pRow_CommandGroup_Command = vectRow_CommandGroup_Command[s2];
			CommandGroup_Command *pCommandGroup_Command = new CommandGroup_Command(
				m_mapCommand[pRow_CommandGroup_Command->FK_Command_get()], 
				m_mapDeviceData_Router_Find(pRow_CommandGroup_Command->FK_Device_get()));
			pCommandGroup->m_vectCommandGroup_Command.push_back(pCommandGroup_Command);
			vector<Row_CommandGroup_Command_CommandParameter *> vectRow_CommandGroup_Command_CommandParameter;
			pRow_CommandGroup_Command->CommandGroup_Command_CommandParameter_FK_CommandGroup_Command_getrows(&vectRow_CommandGroup_Command_CommandParameter);
			for(size_t s3=0;s3<vectRow_CommandGroup_Command_CommandParameter.size();++s3)
			{
				Row_CommandGroup_Command_CommandParameter *pRow_CommandGroup_Command_CommandParameter = vectRow_CommandGroup_Command_CommandParameter[s3];
				pCommandGroup_Command->m_mapParameter[pRow_CommandGroup_Command_CommandParameter->FK_CommandParameter_get()]=
					pRow_CommandGroup_Command_CommandParameter->IK_CommandParameter_get();

			}

		}

    }

    vector<Row_Event *> vectRow_Event;
    GetDatabase()->Event_get()->GetRows("1=1",&vectRow_Event);  // All rows
    for(size_t s=0;s<vectRow_Event.size();++s)
    {
        Row_Event *pRow_Event = vectRow_Event[s];
        m_mapEventNames[pRow_Event->PK_Event_get()]=pRow_Event->Description_get();
    }

    vector<Row_CommandParameter *> vectRow_CommandParameter;
    GetDatabase()->CommandParameter_get()->GetRows("1=1",&vectRow_CommandParameter);  // All rows
    for(size_t s=0;s<vectRow_CommandParameter.size();++s)
    {
        Row_CommandParameter *pRow_CommandParameter = vectRow_CommandParameter[s];
        m_mapCommandParmNames[pRow_CommandParameter->PK_CommandParameter_get()]=pRow_CommandParameter->Description_get();
    }

    vector<Row_EventParameter *> vectRow_EventParameter;
    GetDatabase()->EventParameter_get()->GetRows("1=1",&vectRow_EventParameter);  // All rows
    for(size_t s=0;s<vectRow_EventParameter.size();++s)
    {
        Row_EventParameter *pRow_EventParameter = vectRow_EventParameter[s];
        m_mapEventParmNames[pRow_EventParameter->PK_EventParameter_get()]=pRow_EventParameter->Description_get();
    }
	
	//free device data map
	Map_DeviceData_Base::iterator iD;
	for(iD = allDevices.m_mapDeviceData_Base.begin();
			iD != allDevices.m_mapDeviceData_Base.end(); iD++) {
		delete (*iD).second;
	}

}

// tempoarary hack.  need to optimize & handle broadcats
string Router::GetDevicesByDeviceTemplate(int PK_DeviceTemplate,eBroadcastLevel BroadcastLevel)
{
    string Result="";
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
if( !pDevice )
{
#pragma warning("this shouldn't happen");
g_pPlutoLogger->Write(LV_CRITICAL,"m_mapDeviceData_Router has a NULL in it");
continue;
}
        if( pDevice->m_dwPK_DeviceTemplate!=PK_DeviceTemplate )
            continue;
        if( Result.length() )
            Result += ",";
        Result += StringUtils::itos(pDevice->m_dwPK_Device);
    }
    return Result;
}

// tempoarary hack.  need to optimize & handle broadcats
string Router::GetDevicesByCategory(int PK_DeviceCategory,eBroadcastLevel BroadcastLevel)
{
    string Result="";
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
if( !pDevice )
{
#pragma warning("this shouldn't happen");
g_pPlutoLogger->Write(LV_CRITICAL,"m_mapDeviceData_Router has a NULL in it");
continue;
}
        if( pDevice->m_dwPK_DeviceCategory!=PK_DeviceCategory )
            continue;
        if( Result.length() )
            Result += ",";
        Result += StringUtils::itos(pDevice->m_dwPK_Device);
    }
    return Result;
}
