/**
 *
 * @file ServerSocket.cpp
 * @brief source file for the ServerSocket class
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
#include "DCE/Logger.h"
#include "Message.h"

#include "SocketListener.h"
#include "ServerSocket.h"

using namespace DCE;

void *BeginWapClientThread( void *SvSock )
{	
	ServerSocket *pCS = (ServerSocket *)SvSock;
	pCS->Run();
	return NULL;
}

ServerSocket::ServerSocket( SocketListener *pListener, SOCKET Sock, string sName ) : 
	m_ConnectionMutex( "connection " + sName ), Socket( sName )
{
	m_dwPK_Device = -1;
	m_Socket = Sock;
	m_pListener = pListener;
	
	m_ConnectionMutex.Init( NULL );
	
	int iResult = pthread_create( &m_ClientThreadID, NULL, BeginWapClientThread, (void *)this );
	if ( iResult != 0 )
		g_pPlutoLogger->Write( LV_CRITICAL, "Pthread create returned %d %s dev %d ptr %p", (int)iResult, m_sName.c_str(), m_dwPK_Device,this );
	else
		pthread_detach( m_ClientThreadID );
}

ServerSocket::~ServerSocket()
{
	// Shutdown of client sockets is either performed by their loops,
	// or is triggered by the shutdown of the socket listener.
	
	// Wait for any outstanding locks to finish
	PLUTO_SAFETY_LOCK( cm, m_ConnectionMutex );
	cm.Release();

	pthread_mutex_destroy( &m_ConnectionMutex.mutex );
}

void ServerSocket::Run()
{
	string sMessage;
	while( !m_pListener->m_bTerminate )
	{
		if ( !ReceiveString( sMessage ) || m_pListener->m_bTerminate )
		{
			break;
		}
		if ( sMessage.substr(0,4) == "TIME" )
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
			g_pPlutoLogger->Write(LV_STATUS, "Device %d requested time: sending %s",m_dwPK_Device,s);
			SendString( s );
			continue;
		}

		if (sMessage.substr(0,5) == "HELLO")
		{
			SendString( "OK" );
			m_dwPK_Device = atoi( sMessage.substr(6).c_str() );
			m_sName += sMessage;

#ifdef LL_DEBUG_FILE
			strcat( m_pcSockLogFile,sMessage.c_str() );
			strcat( m_pcSockLogErrorFile,sMessage.c_str() );
#endif
#ifdef TEST_DISCONNECT	
			if ( m_dwPK_Device == TEST_DISCONNECT )
				m_pListener->m_pTestDisconnectEvent = this;
 #endif
			size_t VersionPos = sMessage.find( "," );
/** @todo version control
			if( VersionPos!=string::npos && atoi(sMessage.substr(VersionPos+1).c_str())!=FILE_VERSION )
			{
				g_pPlutoLogger->Write(LV_WARNING, "Received %s, but current file version is %d.", sMessage.c_str(),FILE_VERSION);
			}
*/
			continue;
		}
		
		if ( sMessage.substr(0,14) == "REQUESTHANDLER" )
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
			return;
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
   	if ( !SOCKFAIL( m_Socket ) )
	{
		closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
	g_pPlutoLogger->Write( LV_WARNING, "TCPIP: Closing connection to %d (%s)", m_dwPK_Device,m_pListener->m_sName.c_str() );
	
	m_pListener->OnDisconnected( m_dwPK_Device );
	
	delete this;
	return;
}

