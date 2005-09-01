/*
	ServerSocket

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
 * @file ServerSocket.cpp
 * @brief source file for the ServerSocket class
 * @author
 * @todo notcommented
 *
 */



#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"

#include "SocketListener.h"
#include "ServerSocket.h"

using namespace DCE;

void *ServerSocket::BeginWapClientThread( void *SvSock )
{
	ServerSocket *pCS = (ServerSocket *)SvSock;

//	g_pPlutoLogger->Write(LV_STATUS, "ServerSocket::BeginWapClientThread() enter: %p", pCS);

	// i don't know if this is usefull here. We are doing the same check below.
	if( !pCS->m_bThreadRunning )
	{
//		delete pCS;  // TODO: HACK -- we've got a socket leak here
//		g_pPlutoLogger->Write(LV_STATUS, "ServerSocket::BeginWapClientThread() pCS->m_bThreadRunning false, %p", pCS);
		return NULL; // Should have been set in the constructor
	}

	// TODO: HACK -- we've got a socket leak here.
	// We can't close the socket unless it was marked as running in a separate thread.
	// If we close it even when it isn't marked not a single device will be able to connect to it.
	//
	// The caller can't tell if this is the until a converation is taking place on the connection.
	// Se:
	// 		- we enter the thread
	// 		- look to see if this is the case and remove the socket in this case.
	if( pCS->_Run() )
		delete pCS;

	return NULL;
}

ServerSocket::ServerSocket( SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress, string sMacAddress ) :
	Socket( sName, sIPAddress, sMacAddress ), m_ConnectionMutex( "connection " + sName )
{
	m_dwPK_Device = (long unsigned int)-1;
	m_Socket = Sock;
	m_pListener = pListener;
	m_bThreadRunning=true;

	m_ConnectionMutex.Init( NULL );
}

ServerSocket::~ServerSocket()
{
#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "ServerSocket::~ServerSocket() Deleting socket @%p. Socket id in destructor: %d.", this, m_Socket );
#endif

	Close();

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "ServerSocket::~ServerSocket(): @%p Is it running %d?", this, m_bThreadRunning);
#endif

	while( m_bThreadRunning )
		Sleep(10);

	m_pListener->RemoveSocket(this);

	// Shutdown of client sockets is either performed by their loops,
	// or is triggered by the shutdown of the socket listener.

	// Wait for any outstanding locks to finish
	PLUTO_SAFETY_LOCK( cm, m_ConnectionMutex );
	cm.Release();

	/** @todo check comment */
	//Don't need this since we're deleted from the thread we start
	//pthread_join(m_ClientThreadID, NULL);
	//	printf("Join done m_ClientThreadID=%d\n", (int)m_ClientThreadID);

	pthread_mutex_destroy( &m_ConnectionMutex.mutex );
}


void ServerSocket::Run() {
	int iResult = pthread_create( &m_ClientThreadID, NULL, BeginWapClientThread, (void *)this );
	if ( iResult != 0 )
	{
		m_bThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Pthread create returned %d %s dev %d ptr %p", (int)iResult, m_sName.c_str(), m_dwPK_Device,this );
	}
	else
		pthread_detach( m_ClientThreadID );

}

bool ServerSocket::_Run()
{
#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "Running socket %p... m_bTerminate: %d", this, m_pListener->m_bTerminate );
#endif

	string sMessage;
	while( !m_pListener->m_bTerminate )
	{
		if ( !ReceiveString( sMessage ) || m_pListener->m_bTerminate )
		{
			break;
		}
		if ( sMessage.length() >= 4 && sMessage.substr(0,4) == "TIME" )
		{
			time_t t;
			time(&t);
			struct tm lt;
			struct tm gt;
			{
				PLUTO_SAFETY_LOCK( cm, m_ConnectionMutex );
				memcpy( &lt, localtime(&t), sizeof(struct tm) );
				memcpy( &gt, gmtime(&t), sizeof(struct tm) );
			}
			char s[128];
			sprintf( s, "TIME|%d|%d|%d|%d|%d|%d|UGMT|%lu|BIAS|%lu|%d",
				lt.tm_hour, lt.tm_min, lt.tm_sec, lt.tm_mon+1, lt.tm_mday,
				lt.tm_year+1900, (long unsigned int)t, (long unsigned int) mktime(&lt) - mktime(&gt), lt.tm_isdst );
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Device %d requested time: sending %s",m_dwPK_Device,s);
#endif
			SendString( s );
			continue;
		}
		/** @todo check comment */
		// g_pPlutoLogger->Write(LV_SOCKET, "TCPIP: Received %s", sMessage.c_str());

		if ( sMessage.length() >= 5 && sMessage.substr(0,5) == "HELLO")
		{
			m_dwPK_Device = atoi( sMessage.substr(6).c_str() );

			if( m_dwPK_Device == DEVICEID_MESSAGESEND )
			{
				SendString("OK");
				continue;
			}

			string::size_type pos=sMessage.find("T=");
			int PK_DeviceTemplate = 0;
			if( pos!=string::npos )
				PK_DeviceTemplate = atoi(sMessage.substr(pos+2).c_str());

			pos=sMessage.find("M=");
			string sMacAddress;
			if( pos!=string::npos && sMessage.length()>=pos + 19 )
				sMacAddress = sMessage.substr(pos+2,17);

			// See if the device sent us a device template only
			if( !m_dwPK_Device && PK_DeviceTemplate )
				m_dwPK_Device = m_pListener->GetDeviceID(PK_DeviceTemplate,sMacAddress.length() ? sMacAddress : m_sIPAddress);

			if( !m_dwPK_Device )
				SendString( "BAD DEVICE" );
			else
			{
				int iResponse = m_pListener->ConfirmDeviceTemplate( m_dwPK_Device, PK_DeviceTemplate );
				if( iResponse==3 )
				{
					SendString( "NEED RELOAD" );
					g_pPlutoLogger->Write(LV_WARNING,"Device %d registered but the router needs to be reloaded",m_dwPK_Device);
					m_bThreadRunning=false;
					return true;
				}
				else if( iResponse==0 )
				{
					SendString( "NOT IN THIS INSTALLATION" );
					g_pPlutoLogger->Write(LV_CRITICAL,"Device %d registered but it doesn't exist",m_dwPK_Device);
					m_bThreadRunning=false;
					return true;
				}
				if( PK_DeviceTemplate && iResponse!=2 )
					g_pPlutoLogger->Write(LV_STATUS,"Device %d connected as foreign template %d",m_dwPK_Device, PK_DeviceTemplate);
				SendString( "OK " + StringUtils::itos(m_dwPK_Device) );
				m_sName += sMessage;
			}

#ifdef TEST_DISCONNECT
			if ( m_dwPK_Device == TEST_DISCONNECT )
				m_pListener->m_pTestDisconnectEvent = this;
 #endif

			m_pListener->RegisterEventHandler( this, m_dwPK_Device );
			continue;
		}

		if (  sMessage.length() >= 14 && sMessage.substr(0,14) == "REQUESTHANDLER" )
		{
			SendString( "OK" );
			m_dwPK_Device = atoi( sMessage.substr(14).c_str() );
#ifdef TEST_DISCONNECT
			if (m_dwPK_Device == TEST_DISCONNECT)
			{
				m_pListener->m_pTestDisconnectCmdHandler = this;
			}
#endif

			m_pListener->RegisterCommandHandler( this, m_dwPK_Device );

			// This is a request handler. It will only be used for messages
			// originating FROM other sources, so no thread is necessary.
			// The socket will be periodically checked by the core
			// or closed if replaced by another command handler.
			m_bThreadRunning=false;

			return false;
		}

		if ( sMessage == "BYE" )
		{
			SendString( "OK" );
			break;
		}

		if ( m_dwPK_Device == -1 )
		{
			g_pPlutoLogger->Write( LV_WARNING, "Received %s, but device hasn't identified itself yet.", sMessage.c_str() );
		}
		else
		{
			if ( sMessage.substr(0,7) == "MESSAGE" )
			{
				Message *pMessage = ReceiveMessage( atoi(sMessage.substr(8).c_str()) );
				if ( pMessage )
				{
					if ( pMessage->m_vectExtraMessages.size() )
					{
						for( size_t s=0; s < pMessage->m_vectExtraMessages.size(); ++s )
						{
							Message *pMessage_Child = pMessage->m_vectExtraMessages[s];
							if ( pMessage_Child->m_dwPK_Device_To != pMessage->m_dwPK_Device_To )
							{
								// This embedded message has a different destination.  We need to send it separately
								m_pListener->ReceivedMessage( this, pMessage_Child );
								pMessage->m_vectExtraMessages[s] = NULL; // So it doesn't go to the primary destination also
							}
						}
					}
					m_pListener->ReceivedMessage( this, pMessage );
				}
			}
			else m_pListener->ReceivedString( this, sMessage );
		}
	}

	// This is handled in the destructor.
/*   	if ( !SOCKFAIL( m_Socket ) )
	{
		closesocket( m_Socket );
		close(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
*/
	g_pPlutoLogger->Write( LV_WARNING, "TCPIP: Closing connection to %d (%s)", m_dwPK_Device,m_pListener->m_sName.c_str() );

	m_pListener->OnDisconnected( m_dwPK_Device );

	m_bThreadRunning=false;
	return true;
}

void ServerSocket::PingFailed()
{
	Close();
	m_pListener->PingFailed( this, m_dwPK_Device );
}

