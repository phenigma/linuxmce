/**
 *
 * @file ClientSocket.cpp
 * @brief source file for the ClientSocket class
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */


#include "PlutoUtils/CommonIncludes.h"	

#ifndef WIN32
#include <fcntl.h>
#endif

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "ClientSocket.h"

#define DCE_DEFAULT_PORT 3450

using namespace DCE;

void *WatchDogThread( void *pData )
{
	ClientSocket *pClientSocket = (ClientSocket *)pData;
	while( !pClientSocket->m_bStopWatchdog && clock() < pClientSocket->m_clockTimeout ) // till the watch dog is stopped or we get a clock timeout
		Sleep( 10 );
	
	if ( clock() >= pClientSocket->m_clockTimeout ) // timeout
	{
		g_pPlutoLogger->Write( LV_WARNING, "Connection timed out" );
#ifdef _WIN32
		::MessageBeep( MB_ICONEXCLAMATION );
#endif
		pClientSocket->Disconnect();
	}

	pClientSocket->m_bWatchdogRunning = false;
	return NULL;
}

ClientSocket::ClientSocket( int iDeviceID, string sIPAddress, string sName ) : Socket( sName )
{
	m_dwPK_Device = iDeviceID;
	m_sIPAddress = sIPAddress;
}

bool ClientSocket::Connect( string sExtraInfo )
{
	bool bSuccess = false;
	
	// flush the socket buffer
	m_pcCurInsockBuffer = NULL;

	if ( m_Socket != INVALID_SOCKET )
	{
#ifdef DEBUG
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write( LV_SOCKET, "ClientSocket::Connect - disconnecting previous socket this: %p device: %d socket %d", this, m_dwPK_Device, m_Socket );
		else // no logger
			cerr << "ClientSocket::Connect - disconnecting previous socket this: device: " << m_dwPK_Device << " Socket: " << (int) m_Socket << endl;
#endif
		Disconnect();
	}

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );

	if ( m_Socket != INVALID_SOCKET )
	{
		int iRetries = 0, iRet;
		struct sockaddr_in  addrT;
		struct hostent*   phe;

		for ( bSuccess = false;  iRetries < MAX_RETRIES && !bSuccess;  iRetries++ ) // keep tring until success or MAX_RETRIES is reached
		{
 			memset( &addrT, 0, sizeof( addrT ) );
			addrT.sin_family = AF_INET;
			string sAddress;
			int iPort;
			StringUtils::AddressAndPortFromString( m_sIPAddress, DCE_DEFAULT_PORT, sAddress, iPort );
			addrT.sin_port = htons( iPort );

			unsigned long dwAddr = inet_addr( sAddress.c_str() );

			if ( dwAddr == INADDR_NONE )
			{ // server address is a name
				phe = gethostbyname( sAddress.c_str() );
				if ( !phe )
				{
#ifdef _WIN32
					int ec = WSAGetLastError();
#else
					int ec = h_errno;	
#endif 
					if( g_pPlutoLogger )
						g_pPlutoLogger->Write( LV_CRITICAL, "gethostbyname for '%s', failed, Last Error Code %d, Device: %d", sAddress.c_str(), ec, m_dwPK_Device );
					else // no logger
						cerr << "gethostbyname for " << sAddress << " failed, Last Error Code " << ec << " device: " << m_dwPK_Device << endl;

					Disconnect();
					break;
				}
				addrT.sin_addr.s_addr = *(long *)phe->h_addr; 
			}
			else
			{ // Convert nnn.nnn address to a usable one
				addrT.sin_addr.s_addr = dwAddr; 
			}

			iRet = connect( m_Socket, (sockaddr *)&addrT, sizeof(addrT) );
			if ( !SOCKFAIL( iRet ) ) 
			{
#ifdef _WIN32
				/** @todo: inet_ntop doesn't exist in Windows */
				m_sMyIPAddress = "unsupported";
#else
				// Determine the local IP address
				char pcAddress[512];
				socklen_t len = sizeof( struct sockaddr );
				getsockname ( m_Socket, (struct sockaddr*) &addrT, &len );
				inet_ntop( AF_INET, &( addrT.sin_addr.s_addr), pcAddress, 32 );
				m_sMyIPAddress = pcAddress;
#endif
				bSuccess = true;
			}
			else
			{
#ifdef _WIN32
				int ec = WSAGetLastError();
#else
				int ec = errno;
#endif
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_CRITICAL, "Connect() failed, Error Code %d", ec);
				else // no logger
					cerr << "Connect() failed, Error Code" << ec << endl;
			}
			
		}
		
		if ( !bSuccess)
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL, "ClientSocket::Connect() not successful");
			else // no logger
				cerr << "ClientSocket::Connect() not successful" << endl;
			Disconnect();
		}
	}
	else
	{
			if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "socket() failed, is the TCP stack initialized?, device: %d", m_dwPK_Device);
			else // no logger
				cerr << "socket() failed, is the TCP stack initialized?, device: " << m_dwPK_Device << endl;
	}
	if ( bSuccess )
	{
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket( m_Socket, FIONBIO, &on );
#else
		long dwFlags = fcntl( m_Socket, F_GETFL );
		dwFlags |= O_NONBLOCK;
		fcntl( m_Socket, F_SETFL, dwFlags );
#endif
		OnConnect( sExtraInfo );
	}
	return bSuccess;    
}

bool ClientSocket::OnConnect( string sExtraInfo )
{
	SendString( "HELLO " + StringUtils::itos( m_dwPK_Device ) + "," + sExtraInfo );
	string sResponse;
	if ( !ReceiveString( sResponse ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Lost connection device: %d", m_dwPK_Device );
	}
	else if ( sResponse != "OK" )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Connection for client socket reported %s, device %d", sResponse.c_str(), m_dwPK_Device );
		SendString( "CORRUPT SOCKET" );
		Sleep( 500 );
		Disconnect();
	}
	else int k=2;
	return true;
}

void ClientSocket::Disconnect()
{
	if ( m_Socket != INVALID_SOCKET )
	{
		closesocket( m_Socket );
		m_Socket= INVALID_SOCKET;
	}
}

void ClientSocket::StartWatchDog( clock_t Timeout )
{
	m_bStopWatchdog = false;
	m_clockTimeout = clock() + Timeout;
	if ( m_bWatchdogRunning ) return;
	m_bWatchdogRunning = true;
    pthread_create( &m_pThread, NULL, WatchDogThread, (void *)this );
    Sleep(1);
}

void ClientSocket::StopWatchDog()
{
    if ( m_pThread != 0 )
    {
        m_bStopWatchdog = true;
        pthread_join( m_pThread, NULL );
        m_pThread = 0;
    }
}

