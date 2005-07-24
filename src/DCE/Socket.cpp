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
	#include <netdb.h>
#endif

#ifdef WINCE
	#include "wince.h"
#endif

#include "Message.h"
#include "Socket.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"


pluto_pthread_mutex_t *m_LL_DEBUG_Mutex=NULL;

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

// An application can create another handler that gets called instead in the event of a deadlock
void (*g_pSocketCrashHandler)(Socket *pSocket)=NULL;

void hexmemcpy( char *pDest, const char *pSource, int NumBytes )
{
	for(int i=0;i<NumBytes;++i)
	{
		char c = pSource[i];
		if( (c>='0' && c<='9') || (c>='A' && c<='z') )
			pDest[i]=c;
		else
			pDest[i]=' ';
	}
	pDest[NumBytes]=0;
}

/**
 * @brief entry point for the MessageQueue thread
 */
void* PingLoop( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	Socket *pSocket = (Socket *) param;
	timespec ts_NextPing;
	ts_NextPing.tv_nsec=0;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,pSocket->m_SocketMutex);  // lock this first

	while(true)
	{
		ts_NextPing.tv_sec=time(NULL)+5;
		sSM.TimedCondWait(ts_NextPing);

		if( !pSocket->m_bUsePingToKeepAlive || pSocket->m_Socket == INVALID_SOCKET )
		{
			sSM.Release();
			pSocket->m_bUsePingToKeepAlive=false;
			return NULL; // Don't try anymore
		}
			
		string sResponse=pSocket->SendReceiveString("PING");
		
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "Sent PING on %p and got %s",pSocket,sResponse.c_str());
#endif
		if( sResponse!="PONG" )
		{
			sSM.Release();
			pSocket->PingFailed();
		}
	}
}

Socket::Socket(string Name,string sIPAddress) : m_SocketMutex("socket mutex " + Name)
{
g_pPlutoLogger->Write(LV_STATUS,"Socket const");
#ifndef WINCE
#ifdef THREAD_LOG
	printf("start const %p\n",this);
#endif
#endif
	m_pcSockLogFile=m_pcSockLogErrorFile=NULL;
	m_sHostName = sIPAddress;

	/*
	// code unusable; there's a memory leak in libc6 2.3.2
	struct addrinfo hints;
	struct addrinfo * res;
			
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = PF_INET;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;

	getaddrinfo(m_sIPAddress.c_str(), NULL, &hints, &res);

	freeaddrinfo(res);
	*/

    struct hostent * res;
#ifdef WIN32
    res = gethostbyname(sIPAddress.c_str());
#else
    res = gethostbyname2(sIPAddress.c_str(), AF_INET);
#endif

    if (res != NULL)
    {
        m_sIPAddress = 
            StringUtils::ltos(res->h_addr[0] & 0xFF) + "." +
            StringUtils::ltos(res->h_addr[1] & 0xFF) + "." +
            StringUtils::ltos(res->h_addr[2] & 0xFF) + "." +
            StringUtils::ltos(res->h_addr[3] & 0xFF);
    }
    else
        m_sIPAddress = sIPAddress;

	m_iSocketCounter = SocketCounter++;
	m_sName = Name;
	m_bQuit = false;
	m_bUsePingToKeepAlive = false;
	m_pthread_pingloop_id = 0;
	m_pSocket_PingFailure=NULL;
	m_eSocketType = st_Unknown;


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
	char c[256];
	snprintf( c, sizeof(c), "%02d-%02d-%02d %d-%02d-%02d %d", (int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year-100, (int)t->tm_hour, (int)t->tm_min, (int)t->tm_sec, m_iSocketCounter );
#  endif



#ifdef WIN32
	sprintf( m_pcSockLogFile, "\\socklog-%s-%s-%s-%s.txt", g_sBinary.c_str(), c, Module, m_sName.c_str() );
	sprintf( m_pcSockLogErrorFile, "\\socklog_error-%s-%s-%s-%s.txt", g_sBinary.c_str(), c, Module, m_sName.c_str() );
#else
	sprintf( m_pcSockLogFile, "/tmp/socklog-%s-%p-%d-%s-%s-%s", g_sBinary.c_str(), this, m_iSocketCounter, c, Module, m_sName.c_str() );
	sprintf( m_pcSockLogErrorFile, "/tmp/socklog_error-%s-%p-%d-%s-%s-%s", g_sBinary.c_str(), this, m_iSocketCounter, c, Module, m_sName.c_str() );
#endif

	FILE *f = fopen( m_pcSockLogFile, "a" );
	if( f )
	{
		fprintf( f, "Socket opened address: %p counter: %d name: %s\n",this,m_iSocketCounter,m_sName.c_str() );
		fclose( f );
	}
	else
		system( (string("lsof >> /var/log/pluto/lsof1_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );

#endif

	m_pcInSockBuffer = new char[INSOCKBUFFER_SIZE];
	m_pcCurInsockBuffer = NULL;
	m_Socket = INVALID_SOCKET;
	m_iReceiveTimeout = 0;
	pthread_mutexattr_init( &m_SocketMutexAttr );
	pthread_mutexattr_settype( &m_SocketMutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_cond_init( &m_PingLoopCond, NULL );
	m_SocketMutex.Init( &m_SocketMutexAttr,&m_PingLoopCond );
#ifdef LL_DEBUG_FILE

	if( m_LL_DEBUG_Mutex == NULL )
	{
		m_LL_DEBUG_Mutex = new pluto_pthread_mutex_t( "debug" );
		m_LL_DEBUG_Mutex->Init( NULL );
	}
#endif

#ifndef WINCE
#ifdef THREAD_LOG
	printf("stop const %p\n",this);
#endif
#endif
}

Socket::~Socket()
{
#ifndef WINCE
#ifdef THREAD_LOG
	printf("start dest %p\n",this);
#endif
#endif


#ifdef DEBUG
	g_pPlutoLogger->Write( LV_SOCKET, "Socket::~Socket(): deleting socket @%p %s (socket id in destructor: %d)", this, m_sName.c_str(), m_Socket );
#endif

	Close();

	if( m_bUsePingToKeepAlive )
	{
		pthread_cond_broadcast( &m_PingLoopCond );
		while( m_bUsePingToKeepAlive )
			Sleep(10);
	}

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
	else
		system( (string("lsof >> /var/log/pluto/lsof2_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );

	delete[] m_pcSockLogFile;
	delete[] m_pcSockLogErrorFile;
#endif

#ifndef WINCE
#ifdef THREAD_LOG
	printf("stop dest %p\n",this);
#endif
#endif
}

bool Socket::SendMessage( Message *pMessage, bool bDeleteMessage )
{
	char *pcData = NULL;
	unsigned long dwSize = 0;
	pMessage->ToData( dwSize, pcData, true ); // converts the message to data
	bool bReturnValue = SendData( dwSize, pcData ); // and sends it

	if(NULL != pcData)
	{
		// delete[] pcData; // free heap
		delete[] pcData;
		pcData = NULL;
	}

	if(bDeleteMessage && NULL != pMessage)
	{
		delete pMessage;
		pMessage = NULL;
	}

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
			if( m_bQuit )
				return NULL;
			g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - failed to allocate buffer - socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
			if( g_pSocketCrashHandler )
				(*g_pSocketCrashHandler)(this);
		}
		else
		{
			if ( ReceiveData( iLength, pcBuffer) ) // we got something back
			{
				pcBuffer[iLength]=0;
				Message *pMessage = new Message( iLength, pcBuffer[0] ? pcBuffer : pcBuffer + 1 ); // making a message from the data

#ifdef LL_DEBUG_FILE
			PLUTO_SAFETY_LOCK_ERRORSONLY(ll2,(*m_LL_DEBUG_Mutex));
			FILE *file = fopen( m_pcSockLogFile, "a" );
			if( !file )
			{
				cout << "cannot open file1: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				cerr << "cannot open file1: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				file = fopen( m_pcSockLogErrorFile, "a" );

				if( !file )
				{
					cout << "cannot open file2: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					cerr << "cannot open file2: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					system( (string("lsof >> /var/log/pluto/lsof3_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
				}

				// Don't check -- if this still fails just throw an exception something is very wrong!
				fprintf( file, "Received message type %d id %d expecting reply %d\n",pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) pMessage->m_eExpectedResponse );
				fclose( file );
			}
			else
			{
				fprintf( file, "Received message type %d id %d expecting reply %d\n",pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) pMessage->m_eExpectedResponse );
				fclose( file );
			}
			ll2.Release();
#endif
				delete[] pcBuffer; // freeing the buffer we no longer need
			#ifdef LOG_ALL_CONTROLLER_ACTIVITY
				LACA_B4_5( "Received Message from: %d to: %d type: %d id: %d %s",
					pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To, pMessage->m_dwMessage_Type, pMessage->m_dwID, m_sName.c_str() );
			#endif
				return pMessage;
			}
			delete[] pcBuffer;
			if( m_bQuit )
				return NULL;
			g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - failed ReceiveData - socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
			PlutoLock::DumpOutstandingLocks();
			if( g_pSocketCrashHandler )
				(*g_pSocketCrashHandler)(this);
		}
	}
#ifdef UNDER_CE
	__except(EXCEPTION_EXECUTE_HANDLER)
#else
	catch( ... ) // an exception was thrown
#endif
	{
		if( m_bQuit )
			return NULL;
		g_pPlutoLogger->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - out of memory? socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
		PlutoLock::DumpOutstandingLocks();
		if( g_pSocketCrashHandler )
			(*g_pSocketCrashHandler)(this);
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

#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	char *pcTmp = new char[iSize+1]; // freeing it after writing data to the file
	hexmemcpy( pcTmp, pcData, min(iSize,200) );
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
	char ac[256];
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
		cout << "cannot open file5: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		cerr << "cannot open file5: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		file = fopen( m_pcSockLogErrorFile, "a" );

		if( !file )
		{
			cout << "cannot open file6: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file6: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			system( (string("lsof >> /var/log/pluto/lsof4_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
		}

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
		struct timeval tv_total, tv;
        struct timeval tv_select_1; 
        struct timeval tv_select_2; 
        struct timeval tv_select;

		if ( m_Socket == INVALID_SOCKET )
		{
#ifdef LL_DEBUG_FILE
			PLUTO_SAFETY_LOCK_ERRORSONLY(ll2,(*m_LL_DEBUG_Mutex));
			FILE *file = fopen( m_pcSockLogFile, "a" );
			if( !file )
			{
				cout << "cannot open file7: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				cerr << "cannot open file7: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				file = fopen( m_pcSockLogErrorFile, "a" );

				if( !file )
				{
					cout << "cannot open file8: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					cerr << "cannot open file8: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					system( (string("lsof >> /var/log/pluto/lsof5_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
				}

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

		int iRet;
		tv_total.tv_sec = SOCKET_TIMEOUT;
		tv_total.tv_usec = 0;
		do
		{
			if( m_Socket == INVALID_SOCKET || m_bQuit )
				return false;

			FD_ZERO( &wrfds );
			FD_SET( m_Socket, &wrfds );

			tv.tv_sec = 1;
			tv.tv_usec = 0;

            //before select
            gettimeofday(&tv_select_1, NULL);

			iRet = select((int) (m_Socket+1), NULL, &wrfds, NULL, &tv);

            //after select
            gettimeofday(&tv_select_2, NULL);
            //the select took 'tv_select' time
            tv_select = tv_select_2 - tv_select_1;


#ifndef WINCE
			if (errno == EINTR)
			{
				errno = 0;
				iRet = 0;
			}
#endif

			tv_total -= tv_select;

		} while (iRet != -1 && iRet != 1 && tv_total.tv_sec > 0);

		if (iRet == 1)
		{
			int iSendBytes = ( iBytesLeft > 16192 ) ? 16192 : iBytesLeft;
			iSendBytes = send( m_Socket, pcData+( iSize-iBytesLeft ), iSendBytes, 0 );

			if ( iSendBytes > 0 )
				iBytesLeft -= iSendBytes;
			else
			{
				Close();
#ifdef LL_DEBUG_FILE

	PLUTO_SAFETY_LOCK_ERRORSONLY( ll, (*m_LL_DEBUG_Mutex) );

	FILE *file = fopen( m_pcSockLogFile, "a" );
	if( !file ) // check
	{
		cout << "cannot open file9: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		cerr << "cannot open file9: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		file = fopen( m_pcSockLogErrorFile, "a" );

		if( !file )
		{
			cout << "cannot open file10: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file10: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			system( (string("lsof >> /var/log/pluto/lsof6_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
		}

		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf( file, "Cannot write to regular sock log\n" );
		fprintf( file, "%d-%s-%s\tClosing socket at line 402\n\n", m_Socket, Module, ac );
		fclose( file );
	}
	else
	{
		fprintf( file, "%d-%s-%s\tClosing socket at line 402\n\n", m_Socket, Module, ac);
		fclose( file );
	}
	ll.Release();

#endif
				return false;
			}
		}
		else
		{
			Close();
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
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	sSM.m_bIgnoreDeadlock=true;  // This socket can block a long time on receive.  Don't treat that as a deadlock
	if( m_Socket == INVALID_SOCKET )
	{
		if( m_bQuit )
			return false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, m_socket %d (%p) %s ch: %p", this, m_Socket, this, m_sName.c_str(), g_pSocketCrashHandler );
		PlutoLock::DumpOutstandingLocks();
		if( g_pSocketCrashHandler )
			(*g_pSocketCrashHandler)(this);
		return false;
	}

#ifdef DEBUG
	clock_t clk_start=clock();
	clock_t clk_select1=0, clk_select1b=0, clk_select2=0, clk_select2b=0;
#endif

	int iBytesLeft = iSize;

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
			struct timeval tv, tv_total;
            struct timeval tv_select_1; 
            struct timeval tv_select_2; 
            struct timeval tv_select;

			int iRet;

			tv_total.tv_sec = m_iReceiveTimeout;
			tv_total.tv_usec = 0;
			//g_pPlutoLogger->Write(LV_STATUS, "Socket::ReceiveData timeout %d socket %d", m_iReceiveTimeout, m_Socket);
			do
			{
				if( m_Socket == INVALID_SOCKET || m_bQuit )
					return false;

				FD_ZERO(&rfds);
				FD_SET(m_Socket, &rfds);

				tv.tv_sec = 1;
				tv.tv_usec = 0;
                
                //before select
                gettimeofday(&tv_select_1, NULL);

				iRet = select((int) (m_Socket+1), &rfds, NULL, NULL, &tv);

                //after select
                gettimeofday(&tv_select_2, NULL);
                //the select took 'tv_select' time
                tv_select = tv_select_2 - tv_select_1;

#ifndef WINCE
				if (errno == EINTR)
				{
					errno = 0;
					iRet = 0;
				}
#endif
				tv_total -= tv_select;
#ifndef DISABLE_SOCKET_TIMEOUTS
			} while (iRet != -1 && iRet != 1 && (m_iReceiveTimeout > 0 ? tv_total.tv_sec > 0 : true));
#else
			} while (iRet != -1 && iRet != 1);
#endif
			//g_pPlutoLogger->Write(LV_STATUS, "Socket::ReceiveData timeout %d socket %d ret %d errno %d %s", tv_total.tv_sec, m_Socket, iRet, errno, strerror(errno));

			if( iRet == 0 || iRet == -1 )
			{
				Close();
				if( m_bQuit )
					return false;
#ifdef DEBUG
				g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, ret %d start: %d 1: %d 1b: %d 2: %d 2b: %d, %d %s ch: %p",
				this, iRet, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
				PlutoLock::DumpOutstandingLocks();


#ifdef LL_DEBUG_FILE

	PLUTO_SAFETY_LOCK_ERRORSONLY( ll, (*m_LL_DEBUG_Mutex) );

	FILE *file = fopen( m_pcSockLogFile, "a" );
	if( !file ) // check
	{
		cout << "cannot open file11: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		cerr << "cannot open file11: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		file = fopen( m_pcSockLogErrorFile, "a" );

		if( !file )
		{
			cout << "cannot open file12: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file12: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			system( (string("lsof >> /var/log/pluto/lsof7_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
		}
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf( file, "Cannot write to regular sock log\n" );
		fprintf( file, "%d-%s\tClosing socket at line 540\n\n", m_Socket, Module);
		fclose( file );
	}
	else
	{
		fprintf( file, "%d-%s\tClosing socket at line 540\n\n", m_Socket, Module);
		fclose( file );
	}
	ll.Release();

#endif

#else
				g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, ret %d socket %d %s ch: %p", this, iRet, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
				PlutoLock::DumpOutstandingLocks();
#endif
				if( g_pSocketCrashHandler )
					(*g_pSocketCrashHandler)(this);
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
#ifdef WIN32
				g_pPlutoLogger->Write(LV_STATUS,"Socket closure error code: %d",WSAGetLastError());
#endif
#ifdef DEBUG
				g_pPlutoLogger->Write( LV_WARNING, "Socket::ReceiveData %p failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d socket %d %s",
				this, m_iSockBufBytesLeft, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );

	#ifdef LL_DEBUG_FILE
				PLUTO_SAFETY_LOCK_ERRORSONLY( ll, (*m_LL_DEBUG_Mutex) );

				FILE *file = fopen( m_pcSockLogFile, "a" );
				if( !file ) // check
				{
					cout << "cannot open file13: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
					cerr << "cannot open file13: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
					file = fopen( m_pcSockLogErrorFile, "a" );

					if( !file )
					{
						cout << "cannot open file14: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
						cerr << "cannot open file14: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
						system( (string("lsof >> /var/log/pluto/lsof8_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
					}
					// Don't check -- if this still fails just throw an exception something is very wrong!
					fprintf( file, "Cannot write to regular sock log\n" );
					fprintf( file, "%d-%s\tClosing socket at line 592\n\n", m_Socket, Module );
					fprintf( file, "Socket::ReceiveData failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d socket %d %s",
							m_iSockBufBytesLeft, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );
					fclose( file );
				}
				else
				{
					fprintf( file, "%d-%s\tClosing socket at line 592\n\n", m_Socket, Module);
					fprintf( file, "Socket::ReceiveData failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d socket %d %s",
							m_iSockBufBytesLeft, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );
					fclose( file );
				}
				ll.Release();
	#endif

#else
				g_pPlutoLogger->Write( LV_STATUS, "Socket::ReceiveData failed, bytes left %d socket %d %s", m_iSockBufBytesLeft, m_Socket, m_sName.c_str() );
#endif
				Close();
				return false;
			}
			m_pcCurInsockBuffer = m_pcInSockBuffer; // refreshing the current position

#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
			char *pcTmp = new char[m_iSockBufBytesLeft +1]; // freed after writing to the file
			hexmemcpy( pcTmp, m_pcInSockBuffer, min(m_iSockBufBytesLeft,200) );
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
				cout << "cannot open file15: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				cerr << "cannot open file15: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
				file = fopen( m_pcSockLogErrorFile, "a" );

				if( !file )
				{
					cout << "cannot open file16: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					cerr << "cannot open file16: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
					system( (string("lsof >> /var/log/pluto/lsof9_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
				}
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
		if( m_bQuit )
			return false;
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveString %p failed, m_socket %d buf %p %s ch: %p", this, m_Socket, acBuf, m_sName.c_str(), g_pSocketCrashHandler );
			PlutoLock::DumpOutstandingLocks();
			if( g_pSocketCrashHandler )
				(*g_pSocketCrashHandler)(this);
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
		// If it's just a ping, respond with pong
		if( *(pcBuf-1) == '\n' && pcBuf-acBuf==5 && strncmp(acBuf,"PING",4)==0 )
		{
			SendString("PONG");
			pcBuf = acBuf;
			iLen = sizeof( acBuf ) - 1;
		}
	} while( *(pcBuf-1) != '\n' && *(pcBuf-1) != 0 && iLen ); // while within iLen and not reached the string's end

	if ( !iLen ) // didn't get all that was expected or more @todo ask
	{
		if( m_bQuit )
			return false;
#ifdef LL_DEBUG_FILE
		PLUTO_SAFETY_LOCK_ERRORSONLY( ll4, (*m_LL_DEBUG_Mutex) );
		FILE *file = fopen( m_pcSockLogFile, "a" );
		if( !file )
		{
			cout << "cannot open file18: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file18: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
			file = fopen( m_pcSockLogErrorFile, "a" );

			if( !file )
			{
				cout << "cannot open file19: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
				cerr << "cannot open file19: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
				system( (string("lsof >> /var/log/pluto/lsof10_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
			}
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
		Close();
#endif
		sRefString = "Not a string, or excessive length";

		g_pPlutoLogger->Write( LV_CRITICAL, "Socket::ReceiveString %p failed len: %d socket: %d %s ch: %p", this, iLen, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
		PlutoLock::DumpOutstandingLocks();
		if( g_pSocketCrashHandler )
			(*g_pSocketCrashHandler)(this);

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
		cout << "cannot open file20: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		cerr << "cannot open file20: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		file = fopen( m_pcSockLogErrorFile, "a" );

		if( !file )
		{
			cout << "cannot open file21: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file21: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			system( (string("lsof >> /var/log/pluto/lsof11_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
		}
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
		cout << "cannot open file24: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		cerr << "cannot open file24: " << m_pcSockLogFile << ": " << strerror(errno) << endl;
		file = fopen( m_pcSockLogErrorFile, "a" );

		if( !file )
		{
			cout << "cannot open file25: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			cerr << "cannot open file25: " << m_pcSockLogErrorFile << ": " << strerror(errno) << endl;
			system( (string("lsof >> /var/log/pluto/lsof12_") + StringUtils::itos((int) time(NULL)) + ".newlog").c_str() );
		}
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

void Socket::StartPingLoop()
{
	m_bUsePingToKeepAlive=true;

	if(pthread_create( &m_pthread_pingloop_id, NULL, PingLoop, (void*)this) )
	{
		m_bUsePingToKeepAlive=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create ping loop %p ch: %p", this, g_pSocketCrashHandler );
	}
}

void Socket::PingFailed()
{
	g_pPlutoLogger->Write( LV_CRITICAL, "Socket::PingFailed %p %s (socket id in destructor: %d %p, ch: %p)", 
		this, m_sName.c_str(), m_Socket, m_pSocket_PingFailure, g_pSocketCrashHandler );
	if( m_pSocket_PingFailure )
	{
		m_pSocket_PingFailure->PingFailed();
		return;
	}
	int k=2;
}

void Socket::Close()
{
	if ( m_Socket != INVALID_SOCKET )
 	{
		shutdown(m_Socket, 0x02 /*SD_BOTH*/); //no recv or send 

		closesocket( m_Socket );
#ifndef WINCE
		close(m_Socket);
#endif
	}
	m_Socket = INVALID_SOCKET;
}
