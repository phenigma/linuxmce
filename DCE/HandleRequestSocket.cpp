/* 
	HandleRequestSocket
	
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
 * @file HandleRequestSocket.cpp
 * @brief source file for the HandleRequestSocket class
 * @author
 * @todo notcommented
 *
 */



#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Socket.h"
#include "Message.h"
#include "HandleRequestSocket.h"

using namespace DCE;

void *BeginHandleRequestThread( void *HRqSock )
{
	HandleRequestSocket *pCS = (HandleRequestSocket *)HRqSock;
	pCS->RunThread();
	return NULL;
}

HandleRequestSocket::HandleRequestSocket( int iDeviceID, string sIPAddress, string sName ) : 
	ClientSocket( iDeviceID, sIPAddress, sName )
{
	m_RequestHandlerThread = 0;
	
	/** @todo check comment */
	//g_pDCELogger->Write(LV_SOCKET,"Created handle request socket %p device: %d ip: %s",this,DeviceID,IPAddress.c_str());
}

HandleRequestSocket::~HandleRequestSocket()
{
	DisconnectAndWait();
}

void HandleRequestSocket::Disconnect()
{
	// We override Disconnect so our processor thread knows the difference
	// between an explicit disconnect and an unexpected one.

	g_pPlutoLogger->Write( LV_SOCKET, "RequestSocket::Disconnect %p device: %d", 
		this, m_dwPK_Device );

	m_bTerminate = true;
	ClientSocket::Disconnect();
}

void HandleRequestSocket::DisconnectAndWait()
{
#ifdef DEBUG
	g_pPlutoLogger->Write( LV_SOCKET, "~HandleRequestSocket %p device: %d ip: %s", this, m_dwPK_Device, m_sIPAddress.c_str() );
#endif
	Disconnect();
	if ( m_RequestHandlerThread )
	{
		pthread_join( m_RequestHandlerThread, NULL );
		m_RequestHandlerThread = 0;
	}
}

bool HandleRequestSocket::OnConnect( string sExtraInfo )
{
	SendString( "REQUESTHANDLER " + StringUtils::itos( m_dwPK_Device ) );
	string sResponse;
	if ( !ReceiveString( sResponse ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Requesthandler %p (device: %d) lost connection. %s", this, m_dwPK_Device, m_sName.c_str() );
		return false;
	}
	if ( sResponse == "CLOSED" ) return false;

	if ( sResponse != "OK" )
	{
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		LACA_B4_4( "Connection for requesthandler %p (device: %d) reported %s. %s", this, m_dwPK_Device, sResponse.c_str(), m_sName.c_str() );
#endif
		g_pPlutoLogger->Write( LV_CRITICAL, "Connection for requesthandler %p (device: %d) reported %s. %s", this, m_dwPK_Device, sResponse.c_str(), m_sName.c_str() );
		return false;
	}
	m_bRunning = true;
	m_bTerminate = false;
    pthread_create( &m_RequestHandlerThread, NULL, BeginHandleRequestThread, (void *)this );
	return true;
}

void HandleRequestSocket::RunThread()
{
#ifdef UNDER_CE
	__try
	{
#endif

	m_bUnexpected = false;
	string sMessage = "";

	while( !m_bTerminate )
	{
		if ( !ReceiveString( sMessage ) )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Receive string failed in HandleRequestSocket %d:%s %s", sMessage.size(), sMessage.c_str(), m_sName.c_str() );
			if (m_bTerminate==false)
			{
				PlutoLock::DumpOutstandingLocks();
				m_bUnexpected = true;
				m_bTerminate = true;
			}
		}
		else
		{
			if ( sMessage == "CORRUPT SOCKET" )
			{
				g_pPlutoLogger->Write( LV_STATUS, "Socket flagged as corrupted %p device: %d", this, m_dwPK_Device );
				m_bTerminate = true;
			}
			if ( sMessage.substr(0,4) == "PING" )
			{
#ifdef DEBUG
#ifdef WINDOWS
				LACA_B4_0( "got ping, sending pong %s", m_sName.c_str() );
				g_pPlutoLogger->Write( LV_STATUS, "got ping, sending pong %p device: %d %s", this, m_dwPK_Device, m_sName.c_str() );
#endif
#endif
				SendString( "PONG" );
			}
			else
			{
#ifdef DEBUG
				g_pPlutoLogger->Write( LV_STATUS, "Received %s %p device: %d", sMessage.c_str(), this, m_dwPK_Device);
#endif
				if ( sMessage.substr(0,7)  == "MESSAGE" )	
				{
					Message *pMessage = ReceiveMessage( atoi( sMessage.substr(8).c_str() ) );
					if ( pMessage )
					{
#ifdef DEBUG
						g_pPlutoLogger->Write( LV_STATUS, "Received Message type %d ID %d from %d to %d (device: %d)",
							pMessage->m_dwMessage_Type, pMessage->m_dwID, pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To, m_dwPK_Device );
#endif
						if ( !ReceivedMessage( pMessage ) )
						{
							#ifdef LOG_ALL_CONTROLLER_ACTIVITY
								LACA_B4_6( "Could not find a handler for message - from %d to %d Type: %d ID: %d (device: %d) %s",
									pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To,
									pMessage->m_dwMessage_Type, pMessage->m_dwID, m_dwPK_Device,m_sName.c_str() );
							#endif
							g_pPlutoLogger->Write( LV_STATUS, "Could not find a handler for message - from %d to %d Type: %d ID: %d (device: %d) %s",
								pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To,
								pMessage->m_dwMessage_Type, pMessage->m_dwID, m_dwPK_Device, m_sName.c_str() );
							SendString( "UNHANDLED" );
						}					
						delete pMessage;
					}
					else
					{
						#ifdef LOG_ALL_CONTROLLER_ACTIVITY
							LACA_B4_3( "Incomplete message received %p (device: %d) %s", this, m_dwPK_Device, m_sName.c_str() );
						#endif
						g_pPlutoLogger->Write( LV_CRITICAL, "Incomplete message received %p (device: %d) %s", this, m_dwPK_Device, m_sName.c_str() );
					}
				}
				else
					ReceivedString( sMessage );
			}
		}
	}
	m_bRunning = false;

//HACK this is crashing! :(	
g_pPlutoLogger->Write( LV_STATUS, "Closing event handler connection...");
//	g_pPlutoLogger->Write( LV_STATUS, "Closing event handler connection %d (%d,%s), Terminate: %d %s\n", 
//		m_dwPK_Device, (int) m_bUnexpected, sMessage.c_str(), (int) m_bTerminate, m_sName.c_str() );
	if ( m_bUnexpected )
	{
		OnUnexpectedDisconnect();
	}
#ifdef UNDER_CE
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		/*
		::PostQuitMessage( 255 );
		
		PROCESS_INFORMATION pi;
		TCHAR tfn[81];
		::GetModuleFilesName( NULL, tfn, sizeof(tfn) );	
		CreateProcess( tfn, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, &pi );
		*/
	}
#endif
	return;
}

