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

#ifndef WINCE
#include "DCERouter.h"
#endif

#include "DeviceData_Impl.h"
#include "Command_Impl.h"
#include "Event_Impl.h"
#include "PreformedCommand.h"

using namespace DCE;

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
Command_Impl *g_pCommand_Impl=NULL;

/**
 * @brief entry point for the MessageQueue thread
 */
void* MessageQueueThread_DCECI( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	Command_Impl* p = (Command_Impl*)param;
	if( p->m_bMessageQueueThreadRunning )  // The constructor should have set this to true
		p->ProcessMessageQueue();
	p->m_bMessageQueueThreadRunning=false;
	
	g_pPlutoLogger->Write(LV_SOCKET,"Exiting MessageQueueThread_DCECI thread...");

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

		if ( sResponse != "BUBU" )
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
	m_bKillSpawnedDevicesOnExit=true;
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bLocalMode = bLocalMode;
	m_bReload = false;
	m_pPrimaryDeviceCommand = this;
	m_bHandleChildren = false;
	m_bMessageQueueThreadRunning = true;
	m_pParent = NULL;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_listMessageQueueMutex.Init( NULL, &m_listMessageQueueCond );
	m_bGeneric = false;
	m_dwMessageInterceptorCounter=0;
	if(pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
	{
		m_bMessageQueueThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create message processing queue" );
	}
#ifdef LL_DEBUG
	SendString("COMMENT COMMAND " + StringUtils::itos(DeviceID));
	m_pEvent->SendString("COMMENT EVENT " + StringUtils::itos(DeviceID));
	m_pRequestHandler->SendString("COMMENT REQHAND " + StringUtils::itos(DeviceID));
#endif
}

Command_Impl::Command_Impl( Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter )
	: HandleRequestSocket( pData->m_dwPK_Device, "", "Command_Impl2 Dev #" ), m_listMessageQueueMutex( "MessageQueue" )
{
	m_bKillSpawnedDevicesOnExit=true;
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bReload = false;
	m_bLocalMode = false;
	m_pPrimaryDeviceCommand = pPrimaryDeviceCommand;
	m_pData = pData;
	m_pEvent = pEvent;
	m_bHandleChildren = false;
	m_bMessageQueueThreadRunning = true;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_listMessageQueueMutex.Init( NULL, &m_listMessageQueueCond );
	m_bGeneric = false;
	m_dwMessageInterceptorCounter=0;
	if(pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
	{
		m_bMessageQueueThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create message processing queue" );
	}

	// Connect the primary device's event processor
	// to the Data class so Data class changes can be sent.
	m_pData->m_pEvent_Impl = m_pPrimaryDeviceCommand->m_pEvent;
}

Command_Impl::~Command_Impl()
{
	g_pCommand_Impl = NULL;
	g_pPlutoLogger->Write( LV_STATUS, "Waiting for message queue thread to quit" );

	m_bQuit=true;
	time_t tTime = time(NULL);

	while( m_bMessageQueueThreadRunning )
	{
		pthread_cond_broadcast( &m_listMessageQueueCond );
		Sleep(10); // Should happen right away
		if( tTime + 5 < time(NULL) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"m_bMessageQueueThreadRunning had blocked!!!!");
			exit(1);
		}
	}
	g_pPlutoLogger->Write( LV_STATUS, "Message queue thread quit" );

	if (m_pData)
		delete m_pData;
	if (this==m_pPrimaryDeviceCommand && m_pEvent)
		delete m_pEvent;
	if( m_pcRequestSocket )
		delete m_pcRequestSocket;

	m_pEvent=m_pcRequestSocket=NULL;
	if( m_bKillSpawnedDevicesOnExit )
	{
		g_pPlutoLogger->Write( LV_STATUS, "About to call kill spawned devices" );
		KillSpawnedDevices();
	}

	for(list<Message *>::iterator it = m_listMessageQueue.begin(); it != m_listMessageQueue.end(); it++)
	{
		delete *it;
	}
	m_listMessageQueue.clear();

	g_pPlutoLogger->Write( LV_STATUS, "~Command_Impl finished" );
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
			" /bin/bash -x " + sPrefix + "Spawn_Device.sh " + StringUtils::itos(pDeviceData_Impl_Child->m_dwPK_Device) + " " + m_sIPAddress + " " + sDisplay + " " + pDeviceData_Impl_Child->m_sCommandLine).c_str() );
#else
	#ifndef WINCE
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory ( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory ( &pi, sizeof(pi) );
		CreateProcess( "C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	#endif
#endif

	string sCommand = StringUtils::itos( pDeviceData_Impl_Child->m_dwPK_Device ) + "_" + pDeviceData_Impl_Child->m_sCommandLine;
	m_vectSpawnedDevices.push_back( sCommand ); // push back the spawned device

	FILE *file = fopen( ("/var/log/pluto/spawned_devices_" + StringUtils::itos( m_dwPK_Device )).c_str(),"ab" );
	if( file )
	{
		fwrite( (sCommand + "\n").c_str(), sCommand.length()+1, 1, file );
		fclose(file);
	}

	return true;
}

void Command_Impl::KillSpawnedDevices()
{
#ifndef WIN32
	g_pPlutoLogger->Write(LV_STATUS, "Need to kill %d child devices", (int)m_vectSpawnedDevices.size());
	string sSpawnedDevices("");
	for( size_t s=0; s < m_vectSpawnedDevices.size(); ++s )
	{
		sSpawnedDevices += m_vectSpawnedDevices[s] + " ";
//		string sCmd = string("") + "screen -list | grep '" + m_vectSpawnedDevices[s] + "' | cut -f 1 -d '.' | cut -f 2 -d '\t' | xargs kill -9";
//		g_pPlutoLogger->Write(LV_WARNING,"Killing spawned device %s with %s",m_vectSpawnedDevices[s].c_str(),sCmd.c_str());
//		system( sCmd.c_str() );
	}
	g_pPlutoLogger->Write(LV_STATUS, "Initiating kill for: %s", sSpawnedDevices.c_str());
//	string sCmd = string("/usr/pluto/bin/KillScreens.sh ") + sSpawnedDevices;
//	g_pPlutoLogger->Write(LV_STATUS, "Cmd: %s", sCmd.c_str());
// Temporary hack - for some unexplainable reason, the KillScreens script, although it appears correct
// is actually causing *everything* to die, including the screen session and bash that runs Orbiter, preventing them from 
// ever restarting.  Temporarily remove this, and let the spawned devices re-spawn themselves, not being tied to Orbiter
	//	system(sCmd.c_str());
	g_pPlutoLogger->Write(LV_STATUS, "Killing completed.");
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

bool Command_Impl::Connect(int iPK_DeviceTemplate)
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
		m_pEvent->m_pClientSocket->Connect(iPK_DeviceTemplate);
		if (m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"No client socket for device ID: %d", this->m_dwPK_Device);
			bResult = false;
		}
	}
	if (bResult && !ClientSocket::Connect(iPK_DeviceTemplate))
	{
#if (!defined(UNDER_CE) || !defined(DEBUG))
		g_pPlutoLogger->Write(LV_CRITICAL,"DeviceCommand connect failed %p, device ID: %d",this,this->m_dwPK_Device);
#endif

		if (m_pEvent)
			m_pEvent->m_pClientSocket->Disconnect();

		bResult = false;
	}

	if( m_Socket == INVALID_SOCKET )
		bResult = false;


	if( m_pData && m_pData->m_bUsePingToKeepAlive )
	{
		// No need to start the ping loop for this socket because the CommandImpl
		// is an incoming socket, and the server will have already started this
		m_pEvent->m_pClientSocket->StartPingLoop();
		m_pcRequestSocket->m_pClientSocket->StartPingLoop();
		m_pEvent->m_pClientSocket->m_pSocket_PingFailure=this;
		m_pcRequestSocket->m_pClientSocket->m_pSocket_PingFailure=this;
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
	if( pMessage->m_dwMessage_Type == MESSAGETYPE_MESSAGE_INTERCEPTED )
	{
		InterceptedMessage(pMessage);
		return true;
	}

	// The class that overrides this will try to handle it
	// If it can't it will pass it to this function.
	map<long, string>::iterator p;

	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND )
	{
		if( pMessage->m_dwID == SYSCOMMAND_QUIT )
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
		else if( pMessage->m_dwID == SYSCOMMAND_SEGFAULT )
		{
			char *pNULLPointer=NULL;
			strcpy(pNULLPointer,"this always crashes any os");
		}
		else if( pMessage->m_dwID == SYSCOMMAND_DEADLOCK )
		{
			PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
			mq.m_bReleased=true;
			PLUTO_SAFETY_LOCK( sSM, m_SocketMutex );
			sSM.m_bReleased=true;
		}
	}

	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND && pMessage->m_dwID == SYSCOMMAND_RELOAD )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got a reload command");
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

	return false;
}

void Command_Impl::QueueMessageToRouter( Message *pMessage )
{
	if( m_bLocalMode )
		return;

g_pPlutoLogger->Write(LV_WARNING,"m_listMessageQueue(%d) adding Type %d ID %d To %d to queue of size: %d",m_dwPK_Device,
pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_To,(int) m_listMessageQueue.size());
	PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
	m_listMessageQueue.push_back( pMessage );
	pthread_cond_broadcast( &m_listMessageQueueCond );
	mq.Release();
g_pPlutoLogger->Write(LV_WARNING,"m_listMessageQueue(%d) done adding Type %d ID %d To %d to queue of size: %d",m_dwPK_Device,
pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_To,(int) m_listMessageQueue.size());
}

void Command_Impl::ProcessMessageQueue()
{
	PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
	while( !m_bQuit )
	{
g_pPlutoLogger->Write(LV_WARNING,"m_listMessageQueue(%d) in while loop with %d messages",m_dwPK_Device,
(int) m_listMessageQueue.size());
		while( m_listMessageQueue.size() == 0 )
		{
			mq.CondWait();
g_pPlutoLogger->Write(LV_WARNING,"m_listMessageQueue(%d) woke up with %d messages",m_dwPK_Device,
(int) m_listMessageQueue.size());
			if( m_bQuit )
				return;
		}

		list<Message*> copyMessageQueue;

		// Make a copy so that we're not holding the lock while we send
		list<Message *>::iterator itMessageQueue;
		for( itMessageQueue=m_listMessageQueue.begin(); itMessageQueue != m_listMessageQueue.end(); ++itMessageQueue )
		{
			Message *pMessage = *itMessageQueue;
			copyMessageQueue.push_back( *itMessageQueue );
		}

g_pPlutoLogger->Write(LV_WARNING,"copied queue and clearing it m_listMessageQueue(%d) has %d messages",m_dwPK_Device,
(int) m_listMessageQueue.size());

		m_listMessageQueue.clear();
		mq.Release();
g_pPlutoLogger->Write(LV_WARNING,"sending copy of m_listMessageQueue(%d) with %d messages",m_dwPK_Device,
(int) copyMessageQueue.size());
		for( itMessageQueue = copyMessageQueue.begin(); itMessageQueue != copyMessageQueue.end(); ++itMessageQueue )
		{
			Message *pMessage = *itMessageQueue;
			m_pEvent->SendMessage( *itMessageQueue );
		}
		mq.Relock();
g_pPlutoLogger->Write(LV_WARNING,"sendt copy of m_listMessageQueue(%d) with %d messages",m_dwPK_Device,
(int) copyMessageQueue.size());
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
	{
		if(pResponse)
			delete pResponse;


		return false;
	}

	pPreformedCommand.ParseResponse( pResponse );
	string sResponse = pResponse->m_mapParameters[0];
	if( p_sResponse )
		*p_sResponse = sResponse;

	bool bResult = sResponse=="OK";

	delete pResponse;
	return bResult;
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
#ifndef WINCE
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
#endif
}

void Command_Impl::InterceptedMessage(Message *pMessage)
{
#ifndef WINCE
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
#endif
}

void Command_Impl::ExecCommandGroup(int PK_CommandGroup)
{
	if( !PK_CommandGroup )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Ignoring ExecCommandGroup 0");
		return;
	}
	Message *pMessage = new Message(m_dwPK_Device,0,PRIORITY_NORMAL,MESSAGETYPE_EXEC_COMMAND_GROUP,PK_CommandGroup,0);
	QueueMessageToRouter(pMessage);
}

string Command_Impl::GetState( int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_STATE " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}

string Command_Impl::GetStatus( int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_STATUS " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}

bool Command_Impl::SetState( string sState, int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return false;

	return m_pcRequestSocket->m_pClientSocket->SendString("SET_STATE " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) + " " + sState );
}

bool Command_Impl::SetStatus( string sStatus, int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return false;

	return m_pcRequestSocket->m_pClientSocket->SendString("SET_STATUS " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) + " " + sStatus );
}

extern pluto_pthread_mutex_t *m_LL_DEBUG_Mutex;
extern pluto_pthread_mutex_t *g_mapLockMutex;
void Command_Impl::DeleteGlobalAllocs()
{
	delete m_LL_DEBUG_Mutex;
	m_LL_DEBUG_Mutex=NULL;

	delete g_mapLockMutex;
	g_mapLockMutex=NULL;

}

void Command_Impl::WaitForMessageQueue()
{
	while(true)
	{
		PLUTO_SAFETY_LOCK_ERRORSONLY( mq, m_listMessageQueueMutex );
g_pPlutoLogger->Write(LV_WARNING,"queue size is: %d",(int) m_listMessageQueue.size());
for(list<Message *>::iterator it = m_listMessageQueue.begin(); it != m_listMessageQueue.end(); it++)
{
Message *pMessage = *it;
g_pPlutoLogger->Write(LV_WARNING,"Message from %d to %d type %d id %d",
  pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,
  pMessage->m_dwMessage_Type,pMessage->m_dwID);

}
		if( m_listMessageQueue.size()==0 )
			return;
Sleep(500);
	}
}
