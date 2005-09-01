/*
	ClientSocket

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
 * @file ClientSocket.cpp
 * @brief source file for the ClientSocket class
 * @author
 * @todo notcommented
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

#ifdef WINCE
	#include "wince.h"
#endif


ClientSocket::ClientSocket( int iDeviceID, string sIPAddress, string sName ) : Socket( sName, sIPAddress )
{
	m_dwPK_Device = iDeviceID;
	m_eLastError = cs_err_None;
	/** @todo check comment */
	//	if( g_pDCELogger ) // This won't be created yet if this is the server logger socket
	//		g_pDCELogger->Write(LV_SOCKET,"Created client socket %p device: %d ip: %s",this,m_DeviceID,m_IPAddress.c_str());
}


ClientSocket::~ClientSocket()
{
	Disconnect();
}

bool ClientSocket::Connect( int PK_DeviceTemplate,string sExtraInfo )
{
	bool bSuccess = false;

	// flush the socket buffer
	m_pcCurInsockBuffer = NULL;

	if ( m_Socket != INVALID_SOCKET )
	{
#ifdef DEBUG
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write( LV_SOCKET, "ClientSocket::Connect - disconnecting previous socket this: %p device: %d socket %d", this, m_dwPK_Device, m_Socket );
#ifndef WINCE
		else // no logger
			cerr << "ClientSocket::Connect - disconnecting previous socket this: device: " << m_dwPK_Device << " Socket: " << (int) m_Socket << endl;
#endif

#endif
		Disconnect();
	}

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );

	/** @todo check comment */
	//int b = 1;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));

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
#ifndef WINCE
					else // no logger
						cerr << "gethostbyname for " << sAddress << " failed, Last Error Code " << ec << " device: " << m_dwPK_Device << endl;
#endif
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

#ifndef WINCE
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_CRITICAL, "Connect() failed, Error Code %d (%s))", ec, strerror(ec));
				else // no logger
					cerr << "Connect() failed, Error Code" << ec << strerror(ec) << endl;
#endif
			}

		}
		/** @todo check comment */
		//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
		if ( !bSuccess)
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL, "ClientSocket::Connect() not successful");
#ifndef WINCE
			else // no logger
				cerr << "ClientSocket::Connect() not successful" << endl;
#endif
			Disconnect();
		}
	}
	else
	{
			if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "socket() failed, is the TCP stack initialized?, device: %d", m_dwPK_Device);
#ifndef WINCE
			else // no logger
				cerr << "socket() failed, is the TCP stack initialized?, device: " << m_dwPK_Device << endl;
#endif
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
		OnConnect( PK_DeviceTemplate, sExtraInfo );
	}
	return bSuccess;
}

bool ClientSocket::OnConnect( int PK_DeviceTemplate,string sExtraInfo )
{
	SendString( "HELLO " + StringUtils::itos( m_dwPK_Device ) + (PK_DeviceTemplate ? " T=" + StringUtils::itos(PK_DeviceTemplate) : string("")) + (sExtraInfo.length() ? " " + sExtraInfo : "") );
	string sResponse;
	if ( !ReceiveString( sResponse ) )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write( LV_CRITICAL, "Lost connection device: %d", m_dwPK_Device );

		return false;
	}

	string::size_type pos = sResponse.find("IP=");
	if( pos!=string::npos )
	{
		string::size_type space = sResponse.find(' ',pos+1);
		m_sIPAddress = space==string::npos ? sResponse.substr(pos+3) : sResponse.substr(pos+3,space-pos-3);
	}
	pos = sResponse.find("MAC=");
	if( pos!=string::npos )
	{
		string::size_type space = sResponse.find(' ',pos+1);
		m_sMacAddress = space==string::npos ? sResponse.substr(pos+4) : sResponse.substr(pos+3,space-pos-4);
	}

	if ( sResponse.length()<2 || sResponse.substr(0,2) != "OK" )
	{
		if( sResponse=="NEED RELOAD" )
			m_eLastError=cs_err_NeedReload;
		else if( sResponse.substr(0,24)=="NOT IN THIS INSTALLATION" || sResponse.substr(0,10)=="BAD DEVICE" )
			m_eLastError=cs_err_BadDevice;
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write( LV_CRITICAL, "Connection for client socket reported %s, device %d last error %d", 
				sResponse.c_str(), m_dwPK_Device,(int) m_eLastError );
		SendString( "CORRUPT SOCKET" );
		Sleep( 500 );
		Disconnect();
		return false;
	}

	int PK_Device_New = 0;
	if( sResponse.length()>3 )
		PK_Device_New = atoi(sResponse.substr(3).c_str());

	if( PK_Device_New )
	{
		if( m_dwPK_Device && m_dwPK_Device != PK_Device_New )
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write( LV_CRITICAL, "Server reported we are dev %d but we are device %d", PK_Device_New, m_dwPK_Device );
		}
		else
			m_dwPK_Device=PK_Device_New;
	}

	return true;
}

void ClientSocket::Disconnect()
{

	if ( m_Socket != INVALID_SOCKET )
	{
        if(g_pPlutoLogger)
            g_pPlutoLogger->Write( LV_WARNING, "void ClientSocket::Disconnect() on this socket: %p (m_Socket: %d)", this, m_Socket);

		// this will usually force it out from the select.
		Close();
	}
}

