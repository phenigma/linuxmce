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

#ifdef PLUTOSERVER
	const char *Module="PlutoServer";
#else
  #ifdef APPDAEMON
	const char *Module="AppDaemon";
  #else
    const char *Module="Unknown";
  #endif
#endif

#define MAX_DELAY_FOR_RECEIVE_RESPONSE	20

using namespace DCE;

void (*g_pReceivePingHandler)(Socket *pSocket)=NULL;
bool (*g_pSendPingHandler)(Socket *pSocket)=NULL;

namespace DCE
{
	pluto_pthread_mutex_t *g_pSocketInfoMutex=NULL;
	map<int,SocketInfo *> g_mapSocketInfo;
}
SocketInfo *Socket::g_mapSocketInfo_Find(int iSocketCounter,string sName,Socket *pSocket)
{
	if( !g_pSocketInfoMutex )
	{
		g_pSocketInfoMutex = new pluto_pthread_mutex_t("socketinfo");
		g_pSocketInfoMutex->Init(NULL);
	}
	PLUTO_SAFETY_LOCK(sim,*g_pSocketInfoMutex);
	SocketInfo *pSocketInfo = g_mapSocketInfo[iSocketCounter];
	if( !pSocketInfo )
	{
		pSocketInfo = new SocketInfo(iSocketCounter,sName,pSocket);
		g_mapSocketInfo[iSocketCounter]=pSocketInfo;
	}
	return pSocketInfo;
}


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
#ifdef WINCE //give Orbiter CE time to start
	Sleep(40000); 
#endif

	Socket *pSocket = (Socket *) param;
	timespec ts_NextPing;
	ts_NextPing.tv_nsec=0;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,pSocket->m_SocketMutex);  // lock this first

	while(true)
	{
		ts_NextPing.tv_sec = long(time(NULL)) + 5;
		sSM.TimedCondWait(ts_NextPing);

		if( !pSocket->m_bUsePingToKeepAlive || pSocket->m_Socket == INVALID_SOCKET || pSocket->m_bQuit_get() )
		{
			sSM.Release();
			pSocket->m_bUsePingToKeepAlive=false;
			return NULL; // Don't try anymore
		}
		
		if( g_pSendPingHandler )
		{
			if( !(*g_pSendPingHandler)(pSocket) )
			{
				sSM.Release();
				pSocket->PingFailed();
				return NULL;
			}
		}
		else
		{
			string sResponse=pSocket->SendReceiveString("PING",PING_TIMEOUT);
			
	#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Sent PING on %p and got %s",pSocket,sResponse.c_str());
	#endif
			if( sResponse!="PONG" )
			{
				sSM.Release();
				pSocket->PingFailed();
				return NULL;
			}
		}
	}
}

Socket::Socket(string Name,string sIPAddress, string sMacAddress) : m_SocketMutex("socket mutex " + Name)
{
m_bExpectingAMessage=false;
    m_bCancelSocketOp = false;
	m_pcSockLogFile=m_pcSockLogErrorFile=NULL;
	m_sHostName = sIPAddress;
	m_sMacAddress = sMacAddress;
	m_DataFormat = dfBinary;

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

	getaddrinfo(m_sHostName.c_str(), NULL, &hints, &res);

	freeaddrinfo(res);
	*/

	if( sIPAddress.find('.')==string::npos && sIPAddress.size() )
	{
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
	m_pcInSockBuffer = new char[INSOCKBUFFER_SIZE];
	m_pcCurInsockBuffer = NULL;
	m_Socket = INVALID_SOCKET;
	m_iReceiveTimeout = 0;
	pthread_mutexattr_init( &m_SocketMutexAttr );
	pthread_mutexattr_settype( &m_SocketMutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_cond_init( &m_PingLoopCond, NULL );
	m_SocketMutex.Init( &m_SocketMutexAttr,&m_PingLoopCond );
}

Socket::~Socket()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "Socket::~Socket(): deleting socket @%p %s (socket id in destructor: m_Socket: %d)", this, m_sName.c_str(), m_Socket );
#endif

	if ( m_Socket != INVALID_SOCKET )
	{
		PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
		Close();
		sSM.Release();
	}

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
}

bool Socket::SendMessage( Message *pMessage, bool bDeleteMessage )
{
if( m_bExpectingAMessage )
int k=2;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Socket::SendMessage type %d id %d from %d to %d",
		pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To);
#endif
	bool bReturnValue;

	switch(m_DataFormat)
	{
		case dfBinary:
		{
			char *pcData = NULL;
			unsigned long dwSize = 0;
			pMessage->ToData( dwSize, pcData, true ); // converts the message to data
			bReturnValue = SendData( dwSize, pcData ); // and sends it
			if(NULL != pcData)
			{
				delete[] pcData;
				pcData = NULL;
			}
		}
		break;

		case dfPlainText:
		{
			bReturnValue = SendString(pMessage->ToString(true));
		}
		break;

		default:
		{
			char *pData = NULL;
			size_t nSize = 0;
			pMessage->SerializeToData(m_DataFormat, pData, nSize);

			bReturnValue = SendData(static_cast<int>(nSize), pData);

			if(NULL != pData)
			{
				delete[] pData;
				pData = NULL;
			}
		}
		break;
	}

	if(bDeleteMessage && NULL != pMessage)
	{
		delete pMessage;
		pMessage = NULL;
	}

	return bReturnValue; // the return of SendData
}

/*virtual*/ Message *Socket::ReceiveMessageRaw(string sData)
{
	Message *pOutMessage = NULL;

	const string csPlainTextKeywWord = "MESSAGET";
	const string csXmlKeyword = "MESSAGEXML";
	const string csBinaryKeyword = "MESSAGE";

	if(sData.find(csPlainTextKeywWord) == 0 && sData.size() > csPlainTextKeywWord.size() + 2)
		pOutMessage = ReceiveMessage(atoi(sData.substr(csPlainTextKeywWord.size()).c_str()), dfPlainText);
	else if(sData.find(csXmlKeyword) == 0 && sData.size() > csXmlKeyword.size() + 2)
		pOutMessage = ReceiveMessage(atoi(sData.substr(csXmlKeyword.size()).c_str()), dfXml);
	else if(sData.find(csBinaryKeyword) == 0 && sData.size() > csBinaryKeyword.size() + 2)
		pOutMessage = ReceiveMessage(atoi(sData.substr(csBinaryKeyword.size()).c_str()), dfBinary);
	
	return pOutMessage;
}

Message *Socket::SendReceiveMessage( Message *pMessage)
{
if( m_bExpectingAMessage )
int k=2;
	pMessage->m_eExpectedResponse=ER_ReplyMessage;
	PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, m_SocketMutex );  // Don't log anything but failures

	if( !SendMessage( pMessage ) ) // message couldn't be send
		return NULL;

	string sResult;
	Message *pOutMessage;

	bool bResult = ReceiveString( sResult, m_iReceiveTimeout > 0 ? m_iReceiveTimeout : MAX_DELAY_FOR_RECEIVE_RESPONSE );
	if (bResult && sResult.substr(0,7) == "MESSAGE" && sResult.size() > 7) // got the response we expected
	{
		pOutMessage = ReceiveMessageRaw(sResult);
		return pOutMessage; // return the response
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendReceiveMessage didn't get valid response %s", sResult.c_str());
	return NULL; // what we got wasn't what we expected it to be
}

Message *Socket::ReceiveMessage( int iLength, DataFormat format)
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
			if( m_bQuit_get()|| m_bCancelSocketOp)
				return NULL;
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - failed to allocate buffer - m_Socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
			if( g_pSocketCrashHandler )
				(*g_pSocketCrashHandler)(this);
		}
		else
		{
			if ( ReceiveData( iLength, pcBuffer) ) // we got something back
			{
				pcBuffer[iLength]=0;
				return DecodeMessage(pcBuffer, iLength, format);
			}
			delete[] pcBuffer;
			if( m_bQuit_get()|| m_bCancelSocketOp )
				return NULL;
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - failed ReceiveData - m_Socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
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
		if( m_bQuit_get()|| m_bCancelSocketOp )
			return NULL;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed Socket::ReceiveMessage %p - out of memory? m_Socket: %d %s ch: %p", this, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
		PlutoLock::DumpOutstandingLocks();
		if( g_pSocketCrashHandler )
			(*g_pSocketCrashHandler)(this);
	}

	return NULL; // errors, just returning NULL
}

bool Socket::SendData( int iSize, const char *pcData )
{
if( m_bExpectingAMessage )
int k=2;
	if( m_Socket == INVALID_SOCKET ) // m_Socket wasn't propperlly initializated
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendData socket is invalid");
		return false;
	}

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures

#ifdef LL_DEBUG
	char *pcTmp = new char[iSize+1]; // freeing it after writing data to the file
	hexmemcpy( pcTmp, pcData, min(iSize,200) );

#  ifdef UNDER_CE
	char ac[50];
	SYSTEMTIME lt;
	::GetLocalTime( &lt );
	sprintf( ac, "%d/%d/%d %d:%d:%d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond );
#  else
	time_t ts;
	time( &ts );
	struct tm t;
	localtime( &ts, &t );
	char ac[256];
	snprintf( ac, sizeof(ac), "%02d/%02d/%02d %d:%02d:%02d", (int)t.tm_mon + 1, (int)t.tm_mday, (int)t.tm_year - 100, (int)t.tm_hour, (int)t.tm_min, (int)t.tm_sec );
#  endif
	delete[] pcTmp;
#endif //LL_DEBUG

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
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendData Socket became invalid before FD_SET");
			return false;
		}

		int iRet;
		tv_total.tv_sec = SOCKET_TIMEOUT;
		tv_total.tv_usec = 0;
		do
		{
			if( m_Socket == INVALID_SOCKET || m_bQuit_get()|| m_bCancelSocketOp)
			{
				// This causes a recursive failure
//				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendData m_Socket %d quit %d",(int) m_Socket,(int) m_bQuit_get());
				return false;
			}

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
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendData sendbytes==0");
				Close();
				return false;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::SendData iRet %d",iRet);
			Close();
			return false;
		}
	}

	return true; // success
}

bool Socket::ReceiveData( int iSize, char *pcData, int nTimeout/* = -1*/ )
{
    int nInternalReceiveTimeout = nTimeout != -1 ? nTimeout : m_iReceiveTimeout;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	sSM.m_bIgnoreDeadlock=true;  // This socket can block a long time on receive.  Don't treat that as a deadlock
	if( m_Socket == INVALID_SOCKET )
	{
		if( m_bQuit_get()|| m_bCancelSocketOp )
			return false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, m_Socket: %d (%p) %s ch: %p", this, m_Socket, this, m_sName.c_str(), g_pSocketCrashHandler );
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

			tv_total.tv_sec = nInternalReceiveTimeout;
			tv_total.tv_usec = 0;
			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Socket::ReceiveData timeout %d socket %d", nInternalReceiveTimeout, m_Socket);
			do
			{
				if( m_Socket == INVALID_SOCKET || m_bQuit_get()|| m_bCancelSocketOp )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Socket::ReceiveData m_Socket %d m_bQuit_get()%d",(int) m_Socket,(int) m_bQuit_get());
					return false;
				}

				FD_ZERO(&rfds);
				FD_SET(m_Socket, &rfds);

				if( nInternalReceiveTimeout==-2 )
				{
					tv.tv_sec = 0;
					tv.tv_usec = 0;  
				}
				else
				{
					tv.tv_sec = 1;
					tv.tv_usec = 0;
				}
                
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
			} while (iRet != -1 && iRet != 1 && (nInternalReceiveTimeout > 0 || nInternalReceiveTimeout==-2 ? tv_total.tv_sec > 0 : true));
#else
			} while (iRet != -1 && iRet != 1);
#endif

			if( iRet == 0 || iRet == -1 )
			{
				if( nInternalReceiveTimeout==-2 )
					return false; // Special value means don't timeout
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveData-a %p failed ret %d",this, (int) iRet);
				Close();
				if( m_bQuit_get()|| m_bCancelSocketOp )
					return false;
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, ret %d start: %d 1: %d 1b: %d 2: %d 2b: %d, m_Socket: %d %s ch: %p",
					this, iRet, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
					PlutoLock::DumpOutstandingLocks();
#else
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveData %p failed, ret %d m_Socket: %d %s ch: %p", this, iRet, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
				PlutoLock::DumpOutstandingLocks();
#endif
				if( g_pSocketCrashHandler )
					(*g_pSocketCrashHandler)(this);
				return false;
			}

			m_iSockBufBytesLeft = recv( m_Socket, m_pcInSockBuffer, INSOCKBUFFER_SIZE - 1, 0 );


#ifndef PLATFORM_PR_MC1_CABLE
			if ( m_iSockBufBytesLeft <= 0 )
#else
			if ( m_iSockBufBytesLeft < 0 )
#endif
			{
#ifdef WIN32
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Socket closure error code: %d", WSAGetLastError());
#endif

#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Socket::ReceiveData %p failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d m_Socket: %d %s",
				this, m_iSockBufBytesLeft, (int) clk_start, (int) clk_select1, (int) clk_select1b, (int) clk_select2, (int) clk_select2b, m_Socket, m_sName.c_str() );
#else
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Socket::ReceiveData failed, bytes left %d m_Socket: %d %s", m_iSockBufBytesLeft, m_Socket, m_sName.c_str() );
#endif
				Close();
				return false;
			}

			m_pcCurInsockBuffer = m_pcInSockBuffer; // refreshing the current position

#ifdef LL_DEBUG
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
			struct tm t;
			localtime( &ts, &t );
			char ac[50];
			snprintf( ac, sizeof(ac), "%02d/%02d/%02d %d:%02d:%02d", (int)t.tm_mon+1, (int)t.tm_mday, (int)t.tm_year-100, (int)t.tm_hour, (int)t.tm_min, (int)t.tm_sec );
#endif
			delete[] pcTmp;
#endif
		}
	}
	return true;
}

bool Socket::ReceiveString( string &sRefString, int nTimeout/*= -1*/)
{
	char acBuf[4096], *pcBuf;
	int	iLen = sizeof( acBuf ) - 1;
	if(( m_Socket == INVALID_SOCKET ) || ( acBuf == NULL ))
	{
		if( m_bQuit_get()|| m_bCancelSocketOp )
			return false;
		{
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveString1 %p failed, m_Socket: %d buf %p %s ch: %p", this, m_Socket, acBuf, m_sName.c_str(), g_pSocketCrashHandler );
			PlutoLock::DumpOutstandingLocks();
			if( g_pSocketCrashHandler )
				(*g_pSocketCrashHandler)(this);
		}
		return false;
	}

	pcBuf = acBuf;

	do
	{
		if ( !ReceiveData( 1, pcBuf, nTimeout ) ) // uses ReceiveData to get the string char by char
		{
			if( nTimeout!=-2 )
			{
				sRefString = "ReceiveData failed";
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Socket::ReceiveString2 ReceiveData failed m_Socket: %d %s", m_Socket, m_sName.c_str() );
			}
			return false;
		}
		++pcBuf;
		--iLen;
		// If it's just a ping, respond with pong
		if( *(pcBuf-1) == '\n' )
		{
			if( pcBuf-acBuf==5 && strncmp(acBuf,"PING",4)==0 )
			{
				if( g_pReceivePingHandler )
					(*g_pReceivePingHandler)(this);
				else
					SendString("PONG");
				pcBuf = acBuf;
				iLen = sizeof( acBuf ) - 1;
			}
		}
	} while( *(pcBuf-1) != '\n' && *(pcBuf-1) != 0 && iLen ); // while within iLen and not reached the string's end

	if ( !iLen ) // didn't get all that was expected or more @todo ask
	{
		if( m_bQuit_get()|| m_bCancelSocketOp )
			return false;

		// Yikes, we received more than sizeof(acBuf) characters.  Must be spewing.  Drop the connection.
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::ReceiveString3 %p failed len: %d m_Socket: %d %s ch: %p", this, iLen, m_Socket, m_sName.c_str(), g_pSocketCrashHandler );
		Close();
		sRefString = "Not a string, or excessive length";

		PlutoLock::DumpOutstandingLocks();
		if( g_pSocketCrashHandler )
			(*g_pSocketCrashHandler)(this);

		return false;
	}

	*pcBuf = 0;
	pcBuf--;
	while( *pcBuf == '\r' || *pcBuf == '\n' && pcBuf >= acBuf ) // replacing cariage return and \n with \0
		*pcBuf-- = '\0';
	sRefString = acBuf;

	return true;
}

bool Socket::SendString( string sLine )
{
if( m_bExpectingAMessage )
int k=2;

	if( m_bExpectingAMessage && StringUtils::StartsWith(sLine,"MESSAGE",true)==false )
{
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::SendString send %s expecting=true", sLine.c_str());
#ifndef WIN32
		kill(getpid(), SIGSEGV);
#endif
		char *pFoo = NULL;
		strcpy(pFoo,"cause a crash");
}

	sLine += "\n"; // add the newline
	return SendData( (int)sLine.length(), sLine.c_str() ); // sending the string as a char array
}

string Socket::SendReceiveString( string sLine, int nTimeout/* = -1*/)
{
	// Protect the whole operation
	PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, m_SocketMutex );  // Don't log anything but failures
	SendString( sLine );

	string sResponse;
	if( ReceiveString( sResponse, nTimeout!=-1 || m_iReceiveTimeout ? nTimeout : MAX_DELAY_FOR_RECEIVE_RESPONSE) )
		return sResponse;
	return "";
}

void Socket::StartPingLoop()
{
	m_bUsePingToKeepAlive=true;

	if(pthread_create( &m_pthread_pingloop_id, NULL, PingLoop, (void*)this) )
	{
		m_bUsePingToKeepAlive=false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create ping loop %p ch: %p", this, g_pSocketCrashHandler );
	}

	//we have a design issue here: we can't join PingLoop thread because 
	//this thread is the one which detects network problems and deallocate the socket
	//we'll have to create it detached, to avoid resources leaks.
	pthread_detach(m_pthread_pingloop_id);
}

void Socket::PingFailed()
{
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Socket::PingFailed %p %s (socket id in destructor: %d %p, ch: %p)", 
		this, m_sName.c_str(), m_Socket, m_pSocket_PingFailure, g_pSocketCrashHandler );
	if( m_pSocket_PingFailure )
	{
		m_pSocket_PingFailure->PingFailed();
		return;
	}
}

void Socket::Close()
{
    m_bCancelSocketOp = true;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "Socket::Close() m_Socket %d", m_Socket );
#endif

	if ( m_Socket != INVALID_SOCKET )
 	{
#ifdef DEBUG
		int iResult2=closesocket( m_Socket );
		LoggerWrapper::GetInstance()->Write( LV_SOCKET, "Socket::Close() m_Socket %d closesocket: %d", m_Socket, iResult2 );
#else
		closesocket(m_Socket);
#endif
	}
	m_Socket = INVALID_SOCKET;
}

void Socket::SetReceiveTimeout( int TimeoutSeconds ) 
{
	m_iReceiveTimeout = TimeoutSeconds;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Setting timeout for socket %d to %d", m_Socket, m_iReceiveTimeout);
#endif
}

/*virtual*/ Message *Socket::DecodeMessage(char *pcBuffer, int nLength, DataFormat format)
{
	Message *pMessage;
	if(format == dfPlainText)
		pMessage = new Message( pcBuffer ); // making a message from the string
	else if(format == dfBinary)
		pMessage = new Message( nLength, pcBuffer[0] ? pcBuffer : pcBuffer + 1 ); // making a message from the data
	else 
	{
		pMessage = new Message();
		//todo: check return code:
		pMessage->DeserializeFromData(format, pcBuffer, nLength);
	}

	delete[] pcBuffer; // freeing the buffer we no longer need
	return pMessage;
}
