/**
 *
 * @file SocketListener.cpp
 * @brief source file for the SocketListener class
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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <fcntl.h>

#include "DCE/Logger.h"
#include "ServerSocket.h"
#include "SocketListener.h"

using namespace DCE;

void *BeginListenerThread( void *pSockListener )
{
	SocketListener *pListener = (SocketListener *)pSockListener;
	pListener->Run(); // start the listener
	return NULL;
}

SocketListener::SocketListener( string sName ) : m_ListenerMutex( "listener " + sName )
{
	m_sName = sName;
	m_ListenerThreadID = 0;
	m_ListenerMutex.Init( NULL );
}

SocketListener::~SocketListener()
{
	m_bTerminate = true;
	g_pPlutoLogger->Write( LV_SOCKET, "~SocketListener %d", m_Socket );
	if ( m_Socket != INVALID_SOCKET )
	{
		g_pPlutoLogger->Write( LV_SOCKET, "closing listener socket %d", m_Socket );
		closesocket( m_Socket ); // closing the socket
		m_Socket = INVALID_SOCKET; // now it is invalid
	}
	if ( m_ListenerThreadID ) pthread_join( m_ListenerThreadID, 0 ); // wait for it to finish
  	pthread_mutex_destroy( &m_ListenerMutex.mutex ); // killing the mutex
}

void SocketListener::StartListening( int iPortNumber )
{
	m_bClosed = false;
	m_bTerminate = false;
	m_bRunning = true;
	m_iListenPort = iPortNumber;
    pthread_create( &m_ListenerThreadID, NULL, BeginListenerThread, (void *)this );
}

void SocketListener::Run()
{
	struct sockaddr_in  addrT;

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );

	if ( m_Socket == INVALID_SOCKET ) // error
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Couldn't create listener socket.\r" );
		m_bRunning = false;
		return;
	}
	
	int iOpt = 1;
	if( setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (SOCKOPTTYPE)&iOpt, sizeof(iOpt) ) ) // error
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "setsockopt failed" ); // couldn't set the sock option
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	}
	
	g_pPlutoLogger->Write( LV_SOCKET, "TCPIP: Listening on %d.", m_iListenPort ); // from now trying to start listening

	memset( &addrT, 0, sizeof(addrT) ); // clearing it
	addrT.sin_family = AF_INET;
	addrT.sin_port = htons( m_iListenPort );
	addrT.sin_addr.s_addr = INADDR_ANY;

	if ( SOCKFAIL( bind( m_Socket, (struct sockaddr*)&addrT, sizeof(addrT) ) ) ) // test if we can bind the socket with the desired properties @todo ask
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Failed to bind to port.\r" );
	}
	else if ( SOCKFAIL( listen( m_Socket, SOMAXCONN ) ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Failed to listen to port.\r" );
	}
	else // no errors
 	{
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket( m_Socket, FIONBIO, &on );
#else
		long dwFlags = fcntl( m_Socket, F_GETFL );
		dwFlags |= O_NONBLOCK;
		fcntl( m_Socket, F_SETFL, dwFlags );
#endif
		while( !m_bTerminate ) // while the listener dosen't terminate
		{
			fd_set rfds;
			struct timeval tv;

			int iRet = 0;
		
			while( !m_bTerminate && iRet != 1 )
			{
				FD_ZERO( &rfds );
				FD_SET( m_Socket, &rfds );
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				fflush( stdout );
				iRet = select( (int) (m_Socket+1), &rfds, NULL, NULL, &tv );
			}
			if( m_bTerminate )
				break;

			struct sockaddr_in addr;
			socklen_t len = sizeof( struct sockaddr );
			SOCKET newsock = accept( m_Socket, (sockaddr *)&addr, &len ); // creating a new socket
			if ( newsock != INVALID_SOCKET )
			{
				if ( m_bClosed )
				{
					g_pPlutoLogger->Write( LV_STATUS, "Got an incoming connection, but the server state is closed." );
					send( newsock, "CLOSED\n", 6, 0 );
  					closesocket( newsock );
				}
				else
				{
					g_pPlutoLogger->Write( LV_SOCKET, "TCPIP: Accepting incoming connection on socket %d, port %d, from IP %s.", newsock, m_iListenPort, inet_ntoa(addr.sin_addr) );
#ifdef WIN32
					unsigned long on = 1;
					::ioctlsocket( newsock, FIONBIO, &on );
#else
					long dwFlags = fcntl( newsock, F_GETFL );
					dwFlags |= O_NONBLOCK;
					fcntl( newsock, F_SETFL, dwFlags );
#endif
					Socket *has = CreateSocket( newsock, "Incoming_Conn Socket " + StringUtils::itos(newsock) + " " + inet_ntoa( addr.sin_addr ) );
					PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
					m_listClients.push_back( has ); // add a new client to the client list
					lm.Release();
				}
			}
			else
			{
				if ( !m_bTerminate )
				{
					g_pPlutoLogger->Write( LV_CRITICAL, "Invalid connection socket (accept failed).\r" );
					Sleep( 1000 );
				}
			}
			Sleep( 0 );
		}
	}

	closesocket( m_Socket );

	m_Socket = INVALID_SOCKET;
	m_bRunning = false;
	
	return;
}

Socket *SocketListener::CreateSocket( SOCKET newsock, string sName )
{
	Socket *pSocket = new ServerSocket( this, newsock, sName );
	return pSocket;
}				

void SocketListener::RemoveSocket( Socket *Socket )
{
	PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );

	m_listClients.remove( Socket ); // removing it from the clients map
	closesocket( Socket->m_Socket );

	// See if we have this socket mapped to a device ID and removing the coresponding entries
	ServerSocket *pServerSocket = (ServerSocket *)(Socket);
	if ( pServerSocket )
	{
		int iDeviceID = pServerSocket->m_dwPK_Device;
		Socket->m_Socket = INVALID_SOCKET;
		if( iDeviceID >= 0 )
		{
			DeviceClientMap::iterator i = m_mapCommandHandlers.find( iDeviceID );
			if ( i != m_mapCommandHandlers.end() && (*i).second == Socket )
			{
				g_pPlutoLogger->Write( LV_REGISTRATION, "Command handler for \x1b[34;1m%d\x1b[0m closed.", iDeviceID );
				OnDisconnected( iDeviceID );
				m_mapCommandHandlers.erase( iDeviceID );
			}
			else
			{
				g_pPlutoLogger->Write( LV_REGISTRATION, "Event connection for \x1b[34;1m%d\x1b[0m closed.", iDeviceID );
			}
		}
	}
}

void SocketListener::RegisterCommandHandler( ServerSocket *Socket, int iDeviceID )
{
	PLUTO_SAFETY_LOCK( ll, m_ListenerMutex );
	Socket->m_sName += " dev " + StringUtils::itos( iDeviceID );
	Socket->m_ConnectionMutex.m_sName += " dev " + StringUtils::itos( iDeviceID );
	Socket->m_SocketMutex.m_sName += " dev " + StringUtils::itos( iDeviceID );
	DeviceClientMap::iterator iDC = m_mapCommandHandlers.find( iDeviceID ); // search if device is allready in the map
	if ( iDC != m_mapCommandHandlers.end() )
	{
		g_pPlutoLogger->Write( LV_REGISTRATION, "!!! Replacing command handler on device ID \x1b[34;1m%d!\x1b[0m", iDeviceID );
		ll.Release();
		RemoveSocket( (*iDC).second ); // then remove it
		ll.Relock();
	}
	m_mapCommandHandlers[iDeviceID] = Socket; // assigning it the new specified socket
	Socket->SetReceiveTimeout( 30 );
	ll.Release();
	g_pPlutoLogger->Write( LV_REGISTRATION, "Device ID \x1b[34;1m%d's command handler\x1b[0m registered.", iDeviceID );
	RegisteredCommandHandler( iDeviceID );
}


string SocketListener::GetIPAddress()
{
	char acBuf[256];
	gethostname( acBuf, sizeof(acBuf) );
	
	struct hostent *he = gethostbyname ( acBuf );

	char* addr = he->h_addr_list[0];
	if (addr) // got an address
	{
		struct in_addr  inAddr;
		memmove (&inAddr, addr, 4);
		return string(inet_ntoa(inAddr));
	}
	return "127.0.0.1"; // nothing, using loopback
}

string SocketListener::GetIPAddress( int iDeviceID )
{
	ServerSocket* ss = m_mapCommandHandlers[iDeviceID];
	if( ss == NULL ) return "";

	sockaddr_in inAddr;
	socklen_t sl = sizeof(inAddr);
	if( getpeername( ss->m_Socket, (sockaddr*)&inAddr, &sl ) ) return "";
	return  string( inet_ntoa( inAddr.sin_addr ) );
	
}

bool SocketListener::SendMessage( int iDeviceID, Message *pMessage )
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find( iDeviceID );
	if ( i == m_mapCommandHandlers.end() ) return false; // couldn't find the device id
	(*i).second->SendMessage( pMessage ); // all is ok, sending the message
	return true;
}

bool SocketListener::SendString( int iDeviceID, string sLine )
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find( iDeviceID );
	if ( i == m_mapCommandHandlers.end() ) return false; // couldn't find the device id
	(*i).second->SendString( sLine ); // sending the string
	return true;
}

bool SocketListener::SendData( int iDeviceID, int iLength, const char *pcData )
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find( iDeviceID );
	if ( i == m_mapCommandHandlers.end() ) return false; // couldn't find the device id
	(*i).second->SendData( iLength, pcData ); // sending the data
	return true;
}

