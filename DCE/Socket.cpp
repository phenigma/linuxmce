/* 
	Socket
	
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
 * @file Socket.cpp
 * @brief source file for the Socket class
 * @author
 * @todo notcommented
 *
 */



#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"

#ifndef WIN32 
	#include <fcntl.h>
#endif

#ifdef WINCE
	#include "wince.h"
#endif

#include "Message.h"
#include "Socket.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"


#ifdef LL_DEBUG_FILE
	pluto_pthread_mutex_t *m_LL_DEBUG_Mutex=NULL;
#endif

#ifdef PLUTOSERVER
	const char *Module="PlutoServer";
#else 
  #ifdef APPDAEMON
	const char *Module="AppDaemon";
  #else
    const char *Module="Unknown";
  #endif
#endif

using namespace DCE;

static int SocketCounter=0;

Socket::Socket(string Name) : m_SocketMutex("socket mutex " + Name)
{
	m_iSocketCounter = SocketCounter++;
	m_sName = Name;

#ifdef LL_DEBUG_FILE
	m_pcSockLogFile = new char[200];
	m_pcSockLogErrorFile = new char[200];

#  ifdef UNDER_CE
	char c[50];
	SYSTEMTIME lt;
	::GetLocalTime(&lt);
	sprintf( c, "%d-%d-%d %d-%d-%d %d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, m_iSocketCounter );
#  else
	time_t ts;
	time( &ts );
	struct tm *t = localtime( &ts );
	char c[50];
	snprintf( c, sizeof(c), "%02d-%02d-%02d %d-%02d-%02d %d", (int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year-100, (int)t->tm_hour, (int)t->tm_min, (int)t->tm_sec, m_iSocketCounter );
#  endif



#ifdef WIN32
	sprintf( m_pcSockLogFile, "\\socklog-%s-%s-%s", c, Module, m_sName.c_str() );
	sprintf( m_pcSockLogErrorFile, "\\socklog_error-%s-%s-%s", c, Module, m_sName.c_str() );
#else
	sprintf( m_pcSockLogFile, "/tmp/socklog-%s-%s-%s", c, Module, m_sName.c_str() );
	sprintf( m_pcSockLogErrorFile, "/tmp/socklog_error-%s-%s-%s", c, Module, m_sName.c_str() );
#endif

	FILE *f = fopen( m_pcSockLogFile, "a" );
	if( f )
	{
		fprintf( f, "Socket opened\n" );
		fclose( f );
	}

#endif

	m_pcInSockBuffer = new char[INSOCKBUFFER_SIZE];
	m_pcCurInsockBuffer = NULL;
	m_Socket = INVALID_SOCKET;
	m_iReceiveTimeout = 0;
	pthread_mutexattr_init( &m_SocketMutexAttr );
	pthread_mutexattr_settype( &m_SocketMutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_SocketMutex.Init( &m_SocketMutexAttr );
#ifdef LL_DEBUG_FILE

	if( m_LL_DEBUG_Mutex == NULL )
	{
		m_LL_DEBUG_Mutex = new pluto_pthread_mutex_t( "debug" );
		m_LL_DEBUG_Mutex->Init( NULL );
	}
#endif
}

Socket::~Socket()
{
#ifdef DEBUG
	g_pPlutoLogger->Write( LV_SOCKET, "deleting socket %p %s", this, m_sName.c_str() );
#endif
	if ( m_Socket != INVALID_SOCKET )
		closesocket( m_Socket );
	m_Socket = INVALID_SOCKET;

	if(NULL != m_pcInSockBuffer)
	{
		delete[] m_pcInSockBuffer; 
		m_pcInSockBuffer = NULL;
	}

	pthread_mutexattr_destroy( &m_SocketMutexAttr );
	pthread_mutex_destroy( &m_SocketMutex.mutex );
#ifdef LL_DEBUG_FILE

	/** @todo check comment */
	// Don't do this since other sockets are using the mutex
	// pthread_mutex_destroy(m_LL_DEBUG_Mutex);
	FILE *f = fopen( m_pcSockLogFile, "a" );
	if( f )
	{
		fprintf( f, "Socket closed\n" );
		fclose( f );
	}
	delete[] m_pcSockLogFile;
	delete[] m_pcSockLogErrorFile;

#endif
}

bool Socket::SendMessage( Message *pMessage, bool bDeleteMessage )
{
	char *pcData;
	unsigned long dwSize;
	pMessage->ToData( dwSize, pcData, true ); // converts the message to data
	bool bReturnValue = SendData( dwSize, pcData ); // and sends it
	delete[] pcData; // free heap

	if ( bDeleteMessage ) delete pMessage; // clear pMessage if requested

	return bReturnValue; // the return of SendData
}

Message *Socket::SendReceiveMessage( Message *pMessage )
{
	pMessage->m_eExpectedResponse=ER_ReplyMessage;
	PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, m_SocketMutex );  // Don't log anything but failures
	
	/** @todo check comment */
	// HACK Message->m_eExpectedResponse=ER_Message;

	if( !SendMessage( pMessage ) ) // message couldn't be send
		return NULL;

	string sResult;
	Message *pOutMessage;
	if ( ReceiveString( sResult ) && sResult.substr(0,7)=="MESSAGE" ) // got the response we expected
	{
		pOutMessage = ReceiveMessage( atoi( sResult.substr( 8 ).c_str() ) );
		return pOutMessage; // return the response
	}
	
	return NULL; // what we got wasn't what we expected it to be
}

Message *Socket::ReceiveMessage( int iLength )
{
#ifdef UNDER_CE
	__try
#else
	try
#endif
	{
		char *pcBuffer = new char[iLength+1]; // freeing after we create the Message object from it or after error before return
		if( !pcBuffer ) // couldn't alloc on heap
		{
			
				g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage - failed to allocate buffer - socket: %d %s", m_Socket, m_sName.c_str() );
		}
		else
		{
			if ( ReceiveData( iLength, pcBuffer) ) // we got something back
			{
				pcBuffer[iLength]=0;
				Message *pMessage = new Message( iLength, pcBuffer[0] ? pcBuffer : pcBuffer + 1 ); // making a message from the data
				delete[] pcBuffer; // freeing the buffer we no longer need
			#ifdef LOG_ALL_CONTROLLER_ACTIVITY
				LACA_B4_5( "Received Message from: %d to: %d type: %d id: %d %s", 
					pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To, pMessage->m_dwMessage_Type, pMessage->m_dwID, m_sName.c_str() );
			#endif
				return pMessage;
			}
			delete[] pcBuffer;
			g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage - failed ReceiveData - socket: %d %s", m_Socket, m_sName.c_str() );
			PlutoLock::DumpOutstandingLocks();
		}
	}
#ifdef UNDER_CE
	__except(EXCEPTION_EXECUTE_HANDLER)
#else
	catch( ... ) // an exception was thrown
#endif
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage - out of memory? socket: %d %s", m_Socket, m_sName.c_str() );
		PlutoLock::DumpOutstandingLocks();
	}
	
	return NULL; // errors, just returning NULL
}

bool Socket::SendData( int iSize, const char *pcData )
{
	if( m_Socket == INVALID_SOCKET ) // m_Socket wasn't propperlly initializated
		return false;

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( iSize > 200 )
		LACA_B4_3( "Sending b4 lock big block: %d %d %s", iSize, pcData, m_sName.c_str() );
	else
	{
		if( m_sName.substr(0,6) != "Logger" )

			LACA_B4_4( "Sending b4 lock: (%d) %s thread: %p %s", iSize, pcData, pthread_self(), m_sName.c_str() );
	}
#endif

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( m_sName.substr(0,6) != "Logger" )
		LACA_B4_4( "Sending after lock: (%d) %s %p %s", iSize, pcData, pthread_self(), m_sName.c_str() );

#endif
	
	/** @todo check comment */
	// pthread_mutex_lock(&m_DCESocketMutex);  AB 1-25-2004 - use safety lock instead

#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	char *pcTmp = new char[iSize+1]; // freeing it after writing data to the file
	memcpy( pcTmp, pcData, iSize );
	pcTmp[iSize] = 0;
	char *pc = pcTmp;
	while( ( pc < pcTmp + iSize ) && (*pc) ) // replacing \b with |
	{
		if ( *pc == '\b' ) *pc = '|';
		pc++;
	}
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
#  ifdef UNDER_CE
	char ac[50];
	SYSTEMTIME lt;
	::GetLocalTime( &lt );
	sprintf( ac, "%d/%d/%d %d:%d:%d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond );
#  else
	time_t ts;
	time( &ts );
	struct tm *t = localtime( &ts );
	char ac[50];
	snprintf( ac, sizeof(ac), "%02d/%02d/%02d %d:%02d:%02d", (int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year - 100, (int)t->tm_hour, (int)t->tm_min, (int)t->tm_sec );
#  endif
#endif

#ifdef LL_DEBUG
	printf( "%s-%s\tSending: %s\n", Module, ac, pcTmp );
#endif
#ifdef LL_DEBUG_FILE

	PLUTO_SAFETY_LOCK_ERRORSONLY( ll, (*m_LL_DEBUG_Mutex) );

	FILE *file = fopen( m_pcSockLogFile, "a" );
	if( !file ) // check
	{
		file = fopen( m_pcSockLogErrorFile, "a" );
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf( file, "Cannot write to regular sock log\n" );
		fprintf( file, "%d-%s-%s\tSending Binary (%d bytes): %s\n\n", m_Socket, Module, ac, iSize, pcTmp );
		fclose( file );
	}
	else
	{
		fprintf( file, "%d-%s-%s\tSending Binary (%d bytes): %s\n\n", m_Socket, Module, ac, iSize, pcTmp );
		fclose( file );
	}
	ll.Release();

#endif
#endif
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	delete[] pcTmp;
#endif

	int iBytesLeft = iSize;
	while( iBytesLeft > 0 )
	{
		fd_set wrfds;
		struct timeval tv;

		if ( m_Socket == INVALID_SOCKET )
		{
#ifdef LL_DEBUG_FILE
			PLUTO_SAFETY_LOCK_ERRORSONLY(ll2,(*m_LL_DEBUG_Mutex));
			FILE *file = fopen( m_pcSockLogFile, "a" );
			if( !file )
			{
				file = fopen( m_pcSockLogErrorFile, "a" );
				// Don't check -- if this still fails just throw an exception something is very wrong!
				fprintf( file, "Cannot write to regular sock log\n" );
				fprintf( file, "Socket became invalid before FD_SET" );
				fclose( file );
			}
			else
			{
				fprintf( file, "Socket became invalid before FD_SET" );
				fclose( file );
			}
			ll2.Release();
#endif
			return false;		
		}
		
		FD_ZERO( &wrfds );
		FD_SET( m_Socket, &wrfds );
		int iRet;
		do
		{
			tv.tv_sec = 30;
			tv.tv_usec = 0;
			/** @todo check comment */
			//time_t end, start = time(NULL);
			iRet = select( (int)(m_Socket+1), NULL, &wrfds, NULL, &tv );
			// without timeout
			iRet = select( (int)(m_Socket+1), NULL, &wrfds, NULL, NULL );
	
		} while( iRet != -1 && iRet != 1 );
		int iSendBytes = ( iBytesLeft > 16192 ) ? 16192 : iBytesLeft;
		iSendBytes = send( m_Socket, pcData+( iSize-iBytesLeft ), iSendBytes, 0 );
		
		if ( iSendBytes > 0 )
			iBytesLeft -= iSendBytes;
		else
		{
			closesocket( m_Socket );
			/** @todo check comment */
			// AB 1-25-2004 pthread_mutex_unlock(&m_DCESocketMutex);
			m_Socket = INVALID_SOCKET;
			return false;
		}
	}

/** @todo check comment */
// AB 1-25-2004	pthread_mutex_unlock(&m_DCESocketMutex);
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( iSize > 200 )
	{
		LACA_B4_2( "Sent big block: %d %s", pcData, m_sName.c_str() );
	}
	else
	{
		if( m_sName.substr(0,6) != "Logger" )
		{
			LACA_B4_4( "Sent: (%d) %s thread: %p %s", iSize, pcData, pthread_self(), m_sName.c_str() );
		}
	}
#endif
	return true; // success
}

bool Socket::ReceiveData( int iSize, char *pcData )
{
	if( m_Socket == INVALID_SOCKET )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData failed, m_socket %d (%p) %s", m_Socket, this, m_sName.c_str() );
		PlutoLock::DumpOutstandingLocks();
		return false;
	}

#ifdef DEBUG
	clock_t clk_start=clock();
	clock_t clk_select1=0, clk_select1b=0, clk_select2=0, clk_select2b=0;
#endif

	int iBytesLeft = iSize;
	time_t end=0, start = 0;
	
	while( iBytesLeft > 0 )
	{
		if ( m_pcCurInsockBuffer )
		{
			int iByteCpy = min( m_iSockBufBytesLeft, iBytesLeft );
			memcpy( pcData, m_pcCurInsockBuffer, iByteCpy );
			m_iSockBufBytesLeft -= iByteCpy;
			iBytesLeft -= iByteCpy;
			if ( m_iSockBufBytesLeft == 0 )
				m_pcCurInsockBuffer = NULL;
			else
				m_pcCurInsockBuffer += iByteCpy;
		     if ( iBytesLeft > 0 )
				pcData += iByteCpy;
		}
		else
		{
			fd_set rfds;
			struct timeval tv;

			FD_ZERO(&rfds);
			FD_SET(m_Socket, &rfds);
			int iRet;
#ifndef DISABLE_SOCKET_TIMEOUTS
			if( m_iReceiveTimeout > 0 )
#else
			if ( false )
#endif
			{
				tv.tv_sec = m_iReceiveTimeout;
				tv.tv_usec = 0;
				start = time(NULL);
				do	//select might return early because of a signal
				{
#ifdef DEBUG
					clk_select1 = clock();
#endif
					iRet = select( (int) (m_Socket+1), &rfds, NULL, NULL, &tv );
					end = time( NULL );
#ifdef DEBUG
					clk_select1b = clock();
#endif
				}while( iRet!=-1 && iRet != 1 && (end-start) < m_iReceiveTimeout );
			}
			else
			{
				while(1)
				{
#ifdef DEBUG
					clk_select2 = clock();
#endif
					/*
					//added by Chris : 2004.12.15
					timeval tout;
					tout.tv_sec = 5;
					tout.tv_usec = 0;
					iRet = select( (int)(m_Socket+1), &rfds, NULL, NULL, &tout );*/

					iRet = select( (int)(m_Socket+1), &rfds, NULL, NULL, NULL );
#ifdef DEBUG
					clk_select2b = clock();
#endif
					if( iRet == 1 ) // got it!
						break;
#ifdef UNDER_CE
					if( iRet == -1 )
#else
					if( iRet == -1 && errno != EINTR )
#endif
						break;
				}
			}
			
			if( iRet == 0 || iRet == -1 )
			{
				closesocket( m_Socket );
				m_Socket = INVALID_SOCKET;
#ifdef DEBUG
				g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData failed, ret %d start: %d 1: %d 1b: %d 2: %d 2b: %d, %d %s",
				iRet, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );
				PlutoLock::DumpOutstandingLocks();
#else
				g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData failed, ret %d socket %d %s", iRet, m_Socket, m_sName.c_str() );
				PlutoLock::DumpOutstandingLocks();
#endif
				return false;
			}
			
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		if( m_sName.substr(0,6) != "Logger" )
		{
			LACA_B4_1( "before recv %s", m_sName.c_str() );
		}
#endif			
			m_iSockBufBytesLeft = recv( m_Socket, m_pcInSockBuffer, INSOCKBUFFER_SIZE - 1, 0 ); 
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		if( m_sName.substr(0,6) != "Logger" )
		{
			LACA_B4_2( "after recv: %d %s", m_iSockBufBytesLeft, m_sName.c_str() );
		}
#endif			
			if ( m_iSockBufBytesLeft <= 0 )
			{
				closesocket( m_Socket );
				m_Socket = INVALID_SOCKET;
#ifdef DEBUG
				g_pPlutoLogger->Write( LV_WARNING, "Socket::ReceiveData failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d socket %d %s",
				m_iSockBufBytesLeft, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );
#else
	
				g_pPlutoLogger->Write( LV_STATUS, "Socket::ReceiveData failed, bytes left %d socket %d %s", m_iSockBufBytesLeft, m_Socket, m_sName.c_str() );
#endif
				return false;
			}
			m_pcCurInsockBuffer = m_pcInSockBuffer; // refreshing the current position

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		char *pcTmp2 = new char[m_iSockBufBytesLeft +1]; // freed after the if
		memcpy( pcTmp2, m_pcInSockBuffer, m_iSockBufBytesLeft );
		char *pc2 = pcTmp2;
		while( pc2 < pcTmp2 + m_iSockBufBytesLeft ) // replacing \b with |
		{
			if ( *pc2=='\b')
				*pc2='|';
			pc2++;
		}
		pcTmp2[m_iSockBufBytesLeft]=0; // end
		
		if( m_iSockBufBytesLeft < 200 )
		{
			LACA_B4_2( "(%s)Received: %s\n", m_sName.c_str(), pcTmp2 );
		}
		else
		{
			LACA_B4_1( "(%s)Received: big block\n", m_sName.c_str() );
		}
		
		delete[] pcTmp2;
#endif
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
			char *pcTmp = new char[m_iSockBufBytesLeft +1]; // freed after writing to the file
			memcpy( pcTmp, m_pcInSockBuffer, m_iSockBufBytesLeft );
			char *pc = pcTmp;
			while( pc < pcTmp + m_iSockBufBytesLeft ) // replacing \b with |
			{
				if (*pc=='\b')
					*pc='|';
				pc++;
			}
			pcTmp[m_iSockBufBytesLeft]=0;
#ifdef UNDER_CE
			char ac[50];
			SYSTEMTIME lt;
			::GetLocalTime(&lt);
			sprintf( ac, "%d/%d/%d %d:%d:%d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond );
#else  
		   	time_t ts;
			time (&ts );
			struct tm *t = localtime( &ts );
			char ac[50];
			snprintf( ac, sizeof(ac), "%02d/%02d/%02d %d:%02d:%02d", (int)t->tm_mon+1, (int)t->tm_mday, (int)t->tm_year-100, (int)t->tm_hour, (int)t->tm_min, (int)t->tm_sec );
#endif
#ifdef LL_DEBUG
			printf( "%s-%s\tReceived: %s\n", Module, ac, pcTmp );
#endif
#ifdef LL_DEBUG_FILE
			PLUTO_SAFETY_LOCK_ERRORSONLY( ll3, (*m_LL_DEBUG_Mutex) );
			FILE *file = fopen( m_pcSockLogFile, "a" );
			if( !file )
			{
				file = fopen( m_pcSockLogErrorFile, "a" );
				// Don't check -- if this still fails just throw an exception something is very wrong!
				fprintf( file, "Cannot write to regular sock log\n" );
				fprintf( file, "%d-%s-%s\tReceiving Binary (%d bytes): %s\n\n", m_Socket,Module, ac, m_iSockBufBytesLeft , pcTmp );
				fclose( file );
			}
			else
			{
				fprintf( file, "%d-%s-%s\tReceiving Binary (%d bytes): %s\n\n", m_Socket,Module, ac,m_iSockBufBytesLeft , pcTmp );
				fclose( file );
			}
			ll3.Release();
#endif
			delete[] pcTmp;
#endif
		}
	}
	return true;
}

bool Socket::ReceiveString( string &sRefString )
{
	char acBuf[4096], *pcBuf;
	int	iLen = sizeof( acBuf ) - 1;
	if(( m_Socket == INVALID_SOCKET ) || ( acBuf == NULL ))
	{
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveString failed, m_socket %d buf %p %s", m_Socket, acBuf, m_sName.c_str() );
			PlutoLock::DumpOutstandingLocks();
		}
		return false;
	}

	pcBuf = acBuf;

	do
	{
		if ( !ReceiveData( 1, pcBuf ) ) // uses ReceiveData to get the string char by char
		{
			sRefString = "ReceiveData failed";
			g_pPlutoLogger->Write( LV_STATUS, "Socket::ReceiveString ReceiveData failed %d %s", m_Socket, m_sName.c_str() );
			return false;
		}
		++pcBuf;
		--iLen;
	} while( *(pcBuf-1) != '\n' && *(pcBuf-1) != 0 && iLen ); // while within iLen and not reached the string's end
	
	if ( !iLen ) // didn't get all that was expected or more @todo ask
	{
#ifdef LL_DEBUG_FILE
		PLUTO_SAFETY_LOCK_ERRORSONLY( ll4, (*m_LL_DEBUG_Mutex) );
		FILE *file = fopen( m_pcSockLogFile, "a" );
		if( !file )
		{
			file = fopen( m_pcSockLogErrorFile, "a" );
			// Don't check -- if this still fails just throw an exception something is very wrong!
			fprintf( file, "Cannot write to regular sock log\n" );
			fprintf( file, "%d-%s !!!!!! OVERFLOW !!!!!!!!!\n\n", m_Socket, Module );
			fclose( file );
		}
		else
		{
			fprintf( file, "%d-%s !!!!!! OVERFLOW !!!!!!!!!\n\n", m_Socket, Module );
			fclose( file );
		}
		ll4.Release();
#else
		// Yikes, we received more than sizeof(acBuf) characters.  Must be spewing.  Drop the connection.
		closesocket( m_Socket );
#endif
		sRefString = "Not a string, or excessive length";

		g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveString failed len: %d socket: %d %s", iLen, m_Socket, m_sName.c_str() );
		PlutoLock::DumpOutstandingLocks();

		return false;
	}
	
	*pcBuf = 0;
	pcBuf--;
	while( *pcBuf == '\r' || *pcBuf == '\n' && pcBuf > acBuf ) // replacing cariage return and \n with \0
		*pcBuf-- = '\0';
	sRefString = acBuf;
	
#ifdef LL_DEBUG
	printf( "%s-Received String: %s\n", Module, acBuf );
#endif
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK_ERRORSONLY( ll4, (*m_LL_DEBUG_Mutex) ); 
	FILE *file = fopen( m_pcSockLogFile, "a" );
	if( !file )
	{
		file = fopen( m_pcSockLogErrorFile, "a" );
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf( file, "Cannot write to regular sock log\n" );
		fprintf( file, "%d-%s-Received Text: %s\n\n", m_Socket, Module, acBuf );
		fclose( file );
	}
	else
	{
		fprintf( file, "%d-%s-Received Text: %s\n\n", m_Socket, Module, acBuf );
		fclose( file );
	}
	ll4.Release(); 
#endif

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( m_sName.substr(0,6) != "Logger" )
	{
		LACA_B4_2( "Received string: %s %s", m_sName.c_str(), sRefString.c_str() );
	}
#endif
	return true;
cout << "temp: received string in dce socket" << endl;		
}

bool Socket::SendString( string sLine )
{
#ifdef LL_DEBUG
	printf( "%s-Send strng: %s\n", Module, sLine.c_str() );
#endif
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK_ERRORSONLY( ll, (*m_LL_DEBUG_Mutex) ); 
	FILE *file = fopen( m_pcSockLogFile, "a" );
	if( !file )
	{
		file = fopen( m_pcSockLogErrorFile, "a" );
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf( file, "Cannot write to regular sock log\n" );
		fprintf( file, "%d-%s-Send Text: %s\n\n", m_Socket,Module, sLine.c_str() );
		fclose( file );
	}
	else
	{
		fprintf( file, "%d-%s-Send Text: %s\n\n", m_Socket, Module, sLine.c_str() );
		fclose( file );
	}
	ll.Release();
#endif
	sLine += "\n"; // add the newline
	return SendData( (int)sLine.length(), sLine.c_str() ); // sending the string as a char array
}

string Socket::SendReceiveString( string sLine )
{
	// Protect the whole operation
	PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, m_SocketMutex );  // Don't log anything but failures
	SendString( sLine );

	string sResponse;
	if( ReceiveString( sResponse ) )
		return sResponse;
	return "";
}
