/* 
	Command_Impl
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

/**
 *
 * @file Command_Impl.cpp
 * @brief source file for the Command_Impl class
 * @author
 * @todo notcommented
 *
 */


#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "DCERouter/DCERouter.h"
#include "DeviceData_Impl.h"
#include "Command_Impl.h"
#include "Event_Impl.h"
#include "PreformedCommand.h"

using namespace DCE;

/**
 * @brief entry point for the MessageQueue thread
 */
void* MessageQueueThread_DCECI( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	Command_Impl* p = (Command_Impl*)param;
	p->ProcessMessageQueue();
	return NULL;
}

void *WatchDogThread( void *pData )
{
#ifdef WIN32

	Command_Impl *pCommand_Impl = (Command_Impl *)pData;

	Sleep(3000);
	bool bConnectionLost = false;

	while( !pCommand_Impl->m_bStopWatchdog)
	{
		g_pPlutoLogger->Write(LV_STATUS, "About to send PING to the router.");
		string sResponse = pCommand_Impl->SendReceiveString( "PING " + StringUtils::itos( pCommand_Impl->m_dwPK_Device ) );
		g_pPlutoLogger->Write(LV_STATUS, "Sent PING to the router.");

		if ( sResponse != "OOPS!!!" )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Before Disconnect.");
			pCommand_Impl->Disconnect();
			g_pPlutoLogger->Write(LV_STATUS, "After Disconnect.");
			
			bConnectionLost = true;
			g_pPlutoLogger->Write( LV_CRITICAL, "Connection for client socket reported %s, device %d", 
				sResponse.c_str(), pCommand_Impl->m_dwPK_Device );
			break;
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Received PONG from the router.");
			Sleep(5000);
		}
	}

	if(bConnectionLost)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Before MB_ICONEXCLAMATION.");
		::MessageBeep( MB_ICONEXCLAMATION );
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "WatchDog stopped");
	pCommand_Impl->m_bWatchdogRunning = false;

#endif

	return NULL;
}

Command_Impl::Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode, class Router *pRouter )
	: HandleRequestSocket( DeviceID, ServerAddress, "Command_Impl1 Dev #" + StringUtils::itos(DeviceID) ), m_listMessageQueueMutex( "MessageQueue" )
{
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bLocalMode = bLocalMode;
	m_bReload = false;
	m_bQuit = false;
	m_pPrimaryDeviceCommand = this;
	m_bHandleChildren = false;
	m_pParent = NULL;
	m_listMessageQueueMutex.Init( NULL );
	m_bKillSpawnedDevicesOnExit = false;
	m_bGeneric = false;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_dwMessageInterceptorCounter=0;
	if(pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create message processing queue" );
	}
}

Command_Impl::Command_Impl( Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter )
	: HandleRequestSocket( pData->m_dwPK_Device, "", "Command_Impl2 Dev #" ), m_listMessageQueueMutex( "MessageQueue" )
{
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bReload = false;
	m_bQuit = false;
	m_bLocalMode = false;
	m_pPrimaryDeviceCommand = pPrimaryDeviceCommand;
	m_pData = pData;
	m_pEvent = pEvent;
	m_bHandleChildren = false;
	m_listMessageQueueMutex.Init(NULL);
	m_bKillSpawnedDevicesOnExit = false;
	m_bGeneric = false;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_dwMessageInterceptorCounter=0;
	if(pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create message processing queue" );
	}

	// Connect the primary device's event processor
	// to the Data class so Data class changes can be sent.
	m_pData->m_pEvent_Impl = m_pPrimaryDeviceCommand->m_pEvent;
}

Command_Impl::~Command_Impl()
{
	if (m_pData)
		delete m_pData;
	if (this==m_pPrimaryDeviceCommand && m_pEvent)
		delete m_pEvent;
	if( m_bKillSpawnedDevicesOnExit )
		KillSpawnedDevices();
	if( m_pcRequestSocket )
		delete m_pcRequestSocket;
	m_pcRequestSocket=NULL;
}

Command_Impl *Command_Impl::CreateCommand( int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent )
{
	if ( m_pPrimaryDeviceCommand != this )
	{
		return m_pPrimaryDeviceCommand->CreateCommand( iPK_DeviceTemplate, pPrimaryDeviceCommand, pData, pEvent );
	}
	return NULL;
}

void Command_Impl::CreateChildren()
{
	for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
	{
		DeviceData_Impl *pDeviceData_Impl_Child = m_pData->m_vectDeviceData_Impl_Children[i];

		// This device has it's own executible. Try to spawn it. If that fails, we will try to create it ourselves
		if( pDeviceData_Impl_Child->m_bImplementsDCE && !pDeviceData_Impl_Child->m_bIsEmbedded )
		{
			if( SpawnChildDevice(pDeviceData_Impl_Child) )
				continue;
		}
		Event_Impl *pEvent = m_pEvent->CreateEvent( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
		if ( !pEvent )
		{
			pEvent = new Event_Impl( m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
			g_pPlutoLogger->Write( LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom event handler for.  It will not fire events.",
					pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
		}
		Command_Impl *pCommand = m_pPrimaryDeviceCommand->CreateCommand( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent );
		if ( !pCommand )
		{
			pCommand = new Command_Impl(m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent, m_pRouter);
			pCommand->m_bGeneric=true;
			g_pPlutoLogger->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom handler for.  This is normal for IR.", pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
		}
		pCommand->m_pParent = this;
		pCommand->CreateChildren();
		m_mapCommandImpl_Children[pDeviceData_Impl_Child->m_dwPK_Device] = pCommand;
	}
}

bool Command_Impl::SpawnChildDevice( class DeviceData_Impl *pDeviceData_Impl_Child, string sDisplay )
{
#ifndef WIN32
	string sPrefix="";

	// searching for the Spawn_Device script in some locations
	if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
	{
		sPrefix = "./";
		if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
		{
			sPrefix = "/usr/bin/pluto";
			if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Spawn_Device.sh");
				return false;
			}
		}
	}

	g_pPlutoLogger->Write( LV_STATUS, "Spawning device: %d %s on display: %s ip: %s", pDeviceData_Impl_Child->m_dwPK_Device, pDeviceData_Impl_Child->m_sCommandLine.c_str(), sDisplay.c_str(), m_sIPAddress.c_str() );
	system( ("screen -d -m -h 3000 -S " + StringUtils::itos( pDeviceData_Impl_Child->m_dwPK_Device ) + "_" + pDeviceData_Impl_Child->m_sCommandLine +
			" sh -x " + sPrefix + "Spawn_Device.sh " + StringUtils::itos(pDeviceData_Impl_Child->m_dwPK_Device) + " " + m_sIPAddress + " " + sDisplay + " " + pDeviceData_Impl_Child->m_sCommandLine).c_str() );
	m_vectSpawnedDevices.push_back( StringUtils::itos( pDeviceData_Impl_Child->m_dwPK_Device ) + "_" + pDeviceData_Impl_Child->m_sCommandLine );
#else
	#ifndef WINCE
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory ( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory ( &pi, sizeof(pi) );
		CreateProcess( "C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		m_vectSpawnedDevices.push_back( StringUtils::itos( pDeviceData_Impl_Child->m_dwPK_Device ) + "_" + pDeviceData_Impl_Child->m_sCommandLine ); // push back the spawned device
	#endif
#endif
	return true;
}

void Command_Impl::KillSpawnedDevices()
{
#ifndef WIN32
	for( size_t s=0; s < m_vectSpawnedDevices.size(); ++s )
		system( (string("") + "screen -list | grep " + m_vectSpawnedDevices[s] + " | cut -f 1 -d '.' | cut -f 2 -d '\t' | xargs kill -9" ).c_str() );
	/** @todo check comments */
	//g_pDCELogger->Write(LV_WARNING,"Need to kill %s",m_vectSpawnedDevices[s].c_str());
#endif
}

void Command_Impl::ReplaceParams( ::std::string sReplacement ) {

	if ( sReplacement != "" )
	{
		string sParam1, sParam2;
		bool bDone;
		int iParam_value;
	
		string::size_type pos;
		pos = 0;
		bDone = false;
	
		do
		{
			sParam1 = StringUtils::Tokenize( sReplacement, ::std::string(","), pos ); // get the first token
			sParam2 = StringUtils::Tokenize( sReplacement, ::std::string(","), pos ); // get the second token
	
			iParam_value = atoi( sParam1.c_str() ); // convert it to int
		
			if (( sParam1 != "" ) && ( sParam2 != "" ) && ( iParam_value != 0 ))
			{
				m_pData->m_mapParameters.erase( iParam_value ); // erase the old value
				m_pData->m_mapParameters.insert( pair<int, string>( iParam_value, sParam2 ) ); // insert a new one
			}
			else bDone = true;
	
		} while ( !bDone );
	}
}

bool Command_Impl::Connect()
{
	bool bResult = true;

	// Must have an event socket to proceed.
	if (!m_pEvent)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"No event handler for device ID: %d", this->m_dwPK_Device);
		bResult = false;
	}

	if (bResult && m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
	{
		m_pEvent->m_pClientSocket->Connect();
		if (m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"No client socket for device ID: %d", this->m_dwPK_Device);
			bResult = false;
		}
	}
	if (bResult && !ClientSocket::Connect())
	{
#if (!defined(UNDER_CE) || !defined(DEBUG))
		g_pPlutoLogger->Write(LV_CRITICAL,"DeviceCommand connect failed %p, device ID: %d",this,this->m_dwPK_Device);
#endif
	
		if (m_pEvent)
			m_pEvent->m_pClientSocket->Disconnect();

		bResult = false;
	}

	m_bStopWatchdog = false;
	m_bWatchdogRunning = false;

#ifdef WIN32
	//StartWatchDog(10000);	
#endif

	return bResult;
}

void Command_Impl::ReceivedString( string sLine )
{
	MapCommand_Impl::iterator i;

	for( i=m_mapCommandImpl_Children.begin(); i != m_mapCommandImpl_Children.end(); ++i )
	{
		(*i).second->ReceivedString( sLine );
	}
}

bool Command_Impl::ReceivedMessage( Message *pMessage )
{
	// The class that overrides this will try to handle it
	// If it can't it will pass it to this function.
	map<long, string>::iterator p;

	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND && pMessage->m_dwID == SYSCOMMAND_QUIT )
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
	
	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND && pMessage->m_dwID == SYSCOMMAND_RELOAD )
	{
		SendString("BYE");
		Sleep(250);
		OnReload();
		return true;
	}
	
	if ( m_bHandleChildren && pMessage->m_dwPK_Device_To != m_dwPK_Device )
	{
		// This is slightly inelegant because we don't want to change
		// the base class implementation in order to do this.  If the target device ID is not this device
		// and we're handling children, set the target device ID to the child, change the target,
		// and run it back through ReceivedMessage so the higher level class processes it as
		// it's own.
		m_iTargetDeviceID  = pMessage->m_dwPK_Device_To;
		pMessage->m_dwPK_Device_To = m_dwPK_Device;
		return ReceivedMessage( pMessage );
	}
	
	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_DATAPARM_REQUEST && pMessage->m_dwPK_Device_To == m_dwPK_Device )
	{
			/** @todo:
			p = m_pData->m_mapParameters.find(pMessage->m_dwID);
			if (p==m_pData->m_mapParameters.end())
			{
				SendString("ERR Parameter not found");
			}
			else
			{
				RequestingParameter(pMessage->m_dwID);
				SendMessage(new Message(m_dwPK_Device, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_dwID, (*p).second.c_str()));
			}
			*/
		return true;
	}
	else 
		if ( pMessage->m_dwMessage_Type == MESSAGETYPE_DATAPARM_CHANGE && pMessage->m_dwPK_Device_To == m_dwPK_Device )
		{
			p = pMessage->m_mapParameters.begin();
			if ( p != pMessage->m_mapParameters.end() )
			{
				string ValueOld = m_pData->m_mapParameters[(*p).first];
				m_pData->m_mapParameters[(*p).first] = (*p).second;
				g_pPlutoLogger->Write( LV_STATUS, "Updating data parm %d with %s (Device %d).", (*p).first, (*p).second.c_str(), m_dwPK_Device );
				SendString( "OK" );
				OnDataChange( (*p).first, ValueOld, (*p).second );
			}
			else
			{
				g_pPlutoLogger->Write( LV_WARNING, "Got a data parm change without the value to change." );
				SendString( "ERR Input" );
			}
			return true;
		}
		else
		{
			MapCommand_Impl::iterator i;
	
			for( i=m_mapCommandImpl_Children.begin(); i != m_mapCommandImpl_Children.end(); ++i )
			{
				if ((*i).second->ReceivedMessage( pMessage ))
					return true;
			}
		}
	
	return false;
}

void Command_Impl::QueueMessageToRouter( Message *pMessage )
{
	if( m_bLocalMode )
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY( mq, m_listMessageQueueMutex );
	m_listMessageQueue.push_back( pMessage );
	mq.Release();
	pthread_cond_broadcast( &m_listMessageQueueCond );
}

void Command_Impl::ProcessMessageQueue()
{
	pthread_mutex_lock( &m_listMessageQueueMutex.mutex );
	while( true )
	{
		while( m_listMessageQueue.size() == 0 )
		{
			pthread_cond_wait( &m_listMessageQueueCond, &m_listMessageQueueMutex.mutex );
		}

		list<Message*> copyMessageQueue;

		// Make a copy so that we're not holding the lock while we send
		list<Message *>::iterator itMessageQueue;
		for( itMessageQueue=m_listMessageQueue.begin(); itMessageQueue != m_listMessageQueue.end(); ++itMessageQueue )
		{
			Message *pMessage = *itMessageQueue;
			copyMessageQueue.push_back( *itMessageQueue );
		}

		m_listMessageQueue.clear();
		pthread_mutex_unlock( &m_listMessageQueueMutex.mutex );
		for( itMessageQueue = copyMessageQueue.begin(); itMessageQueue != copyMessageQueue.end(); ++itMessageQueue )
		{
			Message *pMessage = *itMessageQueue;
			m_pEvent->SendMessage( *itMessageQueue );
		}
		pthread_mutex_lock( &m_listMessageQueueMutex.mutex );
	}
}

bool Command_Impl::InternalSendCommand( PreformedCommand &pPreformedCommand, int iConfirmation, string *p_sResponse )
{
	if( m_bLocalMode )
		return true; // If it's just a test, local mode without a router

	// Just put it in the queue.  The queue will delete pPreformedCommand.m_pMessage after sending
	if( iConfirmation == 0 || ( iConfirmation == -1 && !pPreformedCommand.m_pcResponse ) )
	{
		pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_None;
		QueueMessageToRouter( pPreformedCommand.m_pMessage );
		return true;
	}

	// We need a response.  It will be a string if there are no out parameters
	if( !pPreformedCommand.m_pcResponse )
	{
		pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
		string sResponse; // We'll use this only if a response wasn't passed in
		if( !p_sResponse )
			p_sResponse = &sResponse;

		bool bResult = m_pcRequestSocket->SendMessage( pPreformedCommand.m_pMessage, *p_sResponse );
		return bResult && *p_sResponse == "OK";
	}
	// There are out parameters, we need to get a message back in return
	pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = m_pcRequestSocket->SendReceiveMessage( pPreformedCommand.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
		return false;
	pPreformedCommand.ParseResponse( pResponse );
	return true;
}

void Command_Impl::StartWatchDog( clock_t Timeout )
{
	m_bStopWatchdog = false;
	m_clockTimeout = clock() + Timeout;
	if ( m_bWatchdogRunning ) return;
	m_bWatchdogRunning = true;
    pthread_create( &m_pThread, NULL, WatchDogThread, (void *)this );
    Sleep(1);
}

void Command_Impl::StopWatchDog()
{
    if ( m_pThread != 0 )
    {
        m_bStopWatchdog = true;
        pthread_join( m_pThread, NULL );
        m_pThread = 0;
    }
}

void Command_Impl::RegisterMsgInterceptor(MessageInterceptorFn pMessageInterceptorFn,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID)
{
	if( m_pRouter )
	{
		m_pRouter->RegisterMsgInterceptor(
			new MessageInterceptorCallBack(this, pMessageInterceptorFn), PK_Device_From, PK_Device_To, PK_DeviceTemplate, PK_DeviceCategory, MessageType, MessageID );
	}
	else
	{
		m_mapMessageInterceptorFn[m_dwMessageInterceptorCounter] = pMessageInterceptorFn;
		Message *pMessage = new Message(m_dwPK_Device,0,PRIORITY_NORMAL,MESSAGETYPE_REGISTER_INTERCEPTOR,m_dwMessageInterceptorCounter,6,
			PARM_FROM, StringUtils::itos(PK_Device_From).c_str(), PARM_TO, StringUtils::itos(PK_Device_To).c_str(),PARM_TEMPLATE, StringUtils::itos(PK_DeviceTemplate).c_str(),
			PARM_CATEGORY, StringUtils::itos(PK_DeviceCategory).c_str(), PARM_MESSAGE_TYPE, StringUtils::itos(MessageType).c_str(), PARM_MESSAGE_ID, StringUtils::itos(MessageID).c_str());
		SendMessageToRouter(pMessage);
		m_dwMessageInterceptorCounter++;
	}
}

void Command_Impl::InterceptedMessage(Message *pMessage)
{
	MessageInterceptorFn pMessageInterceptorFn = m_mapMessageInterceptorFn_Find(pMessage->m_dwID);
	if( !pMessageInterceptorFn || pMessage->m_vectExtraMessages.size()!=1)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got intercepted message, but there is no corresponding callback");
		return;
	}


	Message *pMessageOriginal = pMessage->m_vectExtraMessages[0];
	CALL_MEMBER_FN(*this,pMessageInterceptorFn) (NULL, pMessageOriginal,
		m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessageOriginal->m_dwPK_Device_From),
		m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessageOriginal->m_dwPK_Device_To));
}

