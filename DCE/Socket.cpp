/*
 * $Id: Socket.cpp,v 1.10 2004/10/28 05:11:28 mihaid Exp $
 */

#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#ifndef WIN32 
#include <fcntl.h>
#endif
#include "Message.h"
#include "Socket.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
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
	m_SocketCounter=SocketCounter++;
	m_sName=Name;

#ifdef LL_DEBUG_FILE
	SockLogFile = new char[200];
	SockLogErrorFile = new char[200];

#  ifdef UNDER_CE
	char c[50];
	SYSTEMTIME lt;
	::GetLocalTime(&lt);
	sprintf(c,"%d-%d-%d %d-%d-%d %d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, m_SocketCounter);
#  else
	time_t ts;
	time(&ts);
	struct tm *t = localtime(&ts);
	char c[50];
	snprintf(c,sizeof(c),"%02d-%02d-%02d %d-%02d-%02d %d",(int) t->tm_mon+1,(int) t->tm_mday,(int) t->tm_year-100,(int) t->tm_hour,(int) t->tm_min, (int)t->tm_sec, m_SocketCounter);
#  endif



#ifdef WIN32
	sprintf(SockLogFile,"\\socklog-%s-%s-%s",c,Module, m_sName.c_str());
	sprintf(SockLogErrorFile,"\\socklog_error-%s-%s-%s",c,Module, m_sName.c_str());
#else
	sprintf(SockLogFile,"/tmp/socklog-%s-%s-%s",c,Module, m_sName.c_str());
	sprintf(SockLogErrorFile,"/tmp/socklog_error-%s-%s-%s",c,Module, m_sName.c_str());
#endif

	FILE *f = fopen(SockLogFile, "a");
	if( f )
	{
		fprintf(f, "Socket opened\n");
		fclose(f);
	}

#endif

	m_InSockBuffer = new char[INSOCKBUFFER_SIZE];
	m_pCurInsockBuffer = NULL;
	m_Socket = INVALID_SOCKET;
	m_ReceiveTimeout = 0;
	pthread_mutexattr_init(&m_SocketMutexAttr);
	pthread_mutexattr_settype(&m_SocketMutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_SocketMutex.Init(&m_SocketMutexAttr);
#ifdef LL_DEBUG_FILE

	if( m_LL_DEBUG_Mutex==NULL )
	{
		m_LL_DEBUG_Mutex = new pluto_pthread_mutex_t("debug");
		m_LL_DEBUG_Mutex->Init(NULL);
	}
#endif
}

Socket::~Socket()
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_SOCKET,"deleting socket %p %s",this,m_sName.c_str());
#endif
	//jjhuff
	if (m_Socket!=INVALID_SOCKET)
		closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;

	delete[] m_InSockBuffer; 
	pthread_mutexattr_destroy(&m_SocketMutexAttr);
	pthread_mutex_destroy(&m_SocketMutex.mutex);
#ifdef LL_DEBUG_FILE
//	Don't do this since other sockets are using the mutex
//	pthread_mutex_destroy(m_LL_DEBUG_Mutex);
	FILE *f = fopen(SockLogFile, "a");
	if( f )
	{
		fprintf(f, "Socket closed\n");
		fclose(f);
	}
	delete[] SockLogFile;
	delete[] SockLogErrorFile;

#endif
}
/*  I'm removing this because we should always use the PLUTO_SAFETY_LOCK, which concept doesn't work with functions that do the locking
void Socket::SocketLock() 
{ 
	pthread_mutex_lock(&m_SocketMutex); 
}
	
void Socket::SocketUnlock() 
{ 
	pthread_mutex_unlock(&m_SocketMutex); 
}
*/
bool Socket::SendMessage(Message *pMsg, bool bDeleteMessage)
{
	char *Data;
	unsigned long Size;
	pMsg->ToData(Size, Data, true);
	bool ReturnValue = SendData(Size, Data);
	delete[] Data;

	if (bDeleteMessage)
		delete pMsg;

	return ReturnValue;
}

Message *Socket::SendReceiveMessage(Message *pMessage)
{
	pMessage->m_eExpectedResponse=ER_ReplyMessage;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // Don't log anything but failures
// hack	pMessage->m_eExpectedResponse=ER_Message;
	if( !SendMessage(pMessage) )
		return NULL;

	string sResult;
	 Message *pOutMessage;
	 if (ReceiveString(sResult) && sResult.substr(0,7)=="MESSAGE")
	 {
		 pOutMessage=ReceiveMessage(atoi(sResult.substr(8).c_str()));
		 return pOutMessage;
	 }
	 return NULL;
}

Message *Socket::ReceiveMessage(int Length)
{
#ifdef UNDER_CE
	__try
#else
	try
#endif
	{
		char *buffer = new char[Length+1];
		if( !buffer )
		{
			
				g_pPlutoLogger->Write(LV_CRITICAL,"Failed Socket::ReceiveMessage - failed to allocate buffer - socket: %d %s",m_Socket,m_sName.c_str());
		}
		else
		{
			if (ReceiveData(Length, buffer))
			{
				buffer[Length]=0;
				Message *pMessage = new Message(Length, buffer[0] ? buffer : buffer+1);
				delete[] buffer;
			#ifdef LOG_ALL_CONTROLLER_ACTIVITY
				LACA_B4_5("Received Message from: %d to: %d type: %d id: %d %s",
					pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,pMessage->m_dwMessage_Type,pMessage->m_dwID,m_sName.c_str());
			#endif
				return pMessage;
			}
			delete[] buffer;
			g_pPlutoLogger->Write(LV_CRITICAL,"Failed Socket::ReceiveMessage - failed ReceiveData - socket: %d %s",m_Socket,m_sName.c_str());
			PlutoLock::DumpOutstandingLocks();
		}
	}
#ifdef UNDER_CE
	__except(EXCEPTION_EXECUTE_HANDLER)
#else
	catch(...)
#endif
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed Socket::ReceiveMessage - out of memory? socket: %d %s",m_Socket,m_sName.c_str());
		PlutoLock::DumpOutstandingLocks();
	}
	return NULL;
}

bool Socket::SendData(int size, const char *data)
{
	if(m_Socket == INVALID_SOCKET)
	{
		return false;
	}

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( size>200 )
	{
		LACA_B4_3("Sending b4 lock big block: %d %d %s",size,data,m_sName.c_str());
	}
	else
	{
		if( m_sName.substr(0,6)!="Logger")
		{
			LACA_B4_4("Sending b4 lock: (%d) %s thread: %p %s",size,data,pthread_self(),m_sName.c_str());
		}
	}
#endif

	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // Don't log anything but failures
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( m_sName.substr(0,6)!="Logger")
	{
		LACA_B4_4("Sending after lock: (%d) %s %p %s",size,data,pthread_self(),m_sName.c_str());
	}
#endif

//	pthread_mutex_lock(&m_SocketMutex);  AB 1-25-2004 - use safety lock instead
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	char *tmp = new char[size+1];
	memcpy(tmp, data, size);
	tmp[size]=0;
	char *p=tmp;
	while((p < tmp+size) && (*p))
	{
		if (*p=='\b')
			*p='|';
		p++;
	}
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
#  ifdef UNDER_CE
	char c[50];
	SYSTEMTIME lt;
	::GetLocalTime(&lt);
	sprintf(c,"%d/%d/%d %d:%d:%d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond);
#  else
	time_t ts;
	time(&ts);
	struct tm *t = localtime(&ts);
	char c[50];
	snprintf(c,sizeof(c),"%02d/%02d/%02d %d:%02d:%02d",(int) t->tm_mon+1,(int) t->tm_mday,(int) t->tm_year-100,(int) t->tm_hour,(int) t->tm_min, (int)t->tm_sec);
#  endif
#endif

#ifdef LL_DEBUG
	printf("%s-%s\tSending: %s\n",Module,c,tmp);
#endif
#ifdef LL_DEBUG_FILE

	PLUTO_SAFETY_LOCK_ERRORSONLY(ll,(*m_LL_DEBUG_Mutex));

	FILE *f = fopen(SockLogFile, "a");
	if( !f )
	{
		f = fopen(SockLogErrorFile, "a");
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf(f,"Cannot write to regular sock log\n");
		fprintf(f, "%d-%s-%s\tSending Binary (%d bytes): %s\n\n", m_Socket, Module, c,size, tmp);
		fclose(f);
	}
	else
	{
		fprintf(f, "%d-%s-%s\tSending Binary (%d bytes): %s\n\n", m_Socket, Module, c,size, tmp);
		fclose(f);
	}
	ll.Release();

#endif
#endif
#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	delete[] tmp;
#endif

	int nBytesLeft = size;
	while(nBytesLeft > 0)
	{
		fd_set wrfds;
		struct timeval tv;

		if (m_Socket == INVALID_SOCKET)
		{
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK_ERRORSONLY(ll2,(*m_LL_DEBUG_Mutex));
	FILE *f = fopen(SockLogFile, "a");
	if( !f )
	{
		f = fopen(SockLogErrorFile, "a");
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf(f,"Cannot write to regular sock log\n");
		fprintf(f, "Socket became invalid before FD_SET");
		fclose(f);
	}
	else
	{
		fprintf(f, "Socket became invalid before FD_SET");
		fclose(f);
	}
	ll2.Release();
#endif
			return false;		
		}
		
		FD_ZERO(&wrfds);
		FD_SET(m_Socket, &wrfds);
		int ret;
		do
		{
			tv.tv_sec = 30;
			tv.tv_usec = 0;
//			time_t end, start = time(NULL);
			ret = select((int) (m_Socket+1), NULL, &wrfds, NULL, &tv);
			// without timeout
			ret = select((int) (m_Socket+1), NULL, &wrfds, NULL, NULL);
	
		} while(ret != -1 && ret != 1);
		int SendBytes = (nBytesLeft > 16192 ) ? 16192 : nBytesLeft;
		SendBytes = send(m_Socket, data+(size-nBytesLeft), SendBytes, 0);
		
		if (SendBytes > 0)
			nBytesLeft -= SendBytes;
		else
		{
			//jjhuff
			closesocket(m_Socket);
// AB 1-25-2004			pthread_mutex_unlock(&m_SocketMutex);
			m_Socket = INVALID_SOCKET;
			return false;
		}
	}
// AB 1-25-2004	pthread_mutex_unlock(&m_SocketMutex);
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( size>200 )
	{
		LACA_B4_2("Sent big block: %d %s",data,m_sName.c_str());
	}
	else
	{
		if( m_sName.substr(0,6)!="Logger")
		{
			LACA_B4_4("Sent: (%d) %s thread: %p %s",size,data,pthread_self(),m_sName.c_str());
		}
	}
#endif
	return true;
}

bool Socket::ReceiveData(int size, char *data)
{
	if(m_Socket == INVALID_SOCKET)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Socket::ReceiveData failed, m_socket %d (%p) %s",m_Socket,this,m_sName.c_str());
		PlutoLock::DumpOutstandingLocks();
		return false;
	}

#ifdef DEBUG
	clock_t clk_start=clock();
	clock_t clk_select1=0,clk_select1b=0,clk_select2=0,clk_select2b=0;
#endif

	int nBytesLeft = size;
	time_t end=0, start = 0;
	
	while(nBytesLeft > 0)
	{
		if (m_pCurInsockBuffer)
		{
			int ByteCpy = min(m_SockBufBytesLeft, nBytesLeft);
			memcpy(data, m_pCurInsockBuffer, ByteCpy);
			m_SockBufBytesLeft -= ByteCpy;
			nBytesLeft -= ByteCpy;
			if (m_SockBufBytesLeft == 0)
				m_pCurInsockBuffer = NULL;
			else
				m_pCurInsockBuffer += ByteCpy;
		     if (nBytesLeft > 0)
				data+=ByteCpy;
		}
		else
		{
			fd_set rfds;
			struct timeval tv;

			FD_ZERO(&rfds);
			FD_SET(m_Socket, &rfds);
			int ret;
#ifndef DISABLE_SOCKET_TIMEOUTS
			if(m_ReceiveTimeout > 0)
#else
			if (false)
#endif
			{
				tv.tv_sec = m_ReceiveTimeout;
				tv.tv_usec = 0;
				start = time(NULL);
				do	//select might return early because of a signal
				{
#ifdef DEBUG
					clk_select1 = clock();
#endif
					ret = select((int) (m_Socket+1), &rfds, NULL, NULL, &tv);
					end = time(NULL);
#ifdef DEBUG
					clk_select1b = clock();
#endif
				}while(ret!=-1 && ret != 1 && (end-start)<m_ReceiveTimeout);
			}
			else
			{
				while(1)
				{
#ifdef DEBUG
					clk_select2 = clock();
#endif
					ret = select((int) (m_Socket+1), &rfds, NULL, NULL, NULL);
#ifdef DEBUG
					clk_select2b = clock();
#endif
					if(ret == 1)
						break;
#ifdef UNDER_CE
					if(ret == -1)
#else
					if(ret == -1 && errno != EINTR)

#endif

						break;
				}
			}
			if(ret == 0 || ret == -1)
			{
				closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_CRITICAL,"Socket::ReceiveData failed, ret %d start: %d 1: %d 1b: %d 2: %d 2b: %d, %d %s",
			ret,(int) clk_start,(int) clk_select1,(int) clk_select1b,(int) clk_select2,(int) clk_select2b,m_Socket,m_sName.c_str());
		PlutoLock::DumpOutstandingLocks();
#else
		g_pPlutoLogger->Write(LV_CRITICAL,"Socket::ReceiveData failed, ret %d socket %d %s",ret,m_Socket,m_sName.c_str());
		PlutoLock::DumpOutstandingLocks();
#endif

				return false;
			}
			
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		if( m_sName.substr(0,6)!="Logger")
		{
			LACA_B4_1("before recv %s",m_sName.c_str());
		}
#endif			
			m_SockBufBytesLeft = recv(m_Socket, m_InSockBuffer, INSOCKBUFFER_SIZE - 1, 0); 
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		if( m_sName.substr(0,6)!="Logger")
		{
			LACA_B4_2("after recv: %d %s",m_SockBufBytesLeft,m_sName.c_str());
		}
#endif			
			if (m_SockBufBytesLeft<=0)
			{
				//jjhuff
				closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_WARNING,"Socket::ReceiveData failed, bytes left %d start: %d 1: %d 1b: %d 2: %d 2b: %d socket %d %s",
			m_SockBufBytesLeft,(int) clk_start,(int) clk_select1,(int) clk_select1b,(int) clk_select2,(int) clk_select2b,m_Socket,m_sName.c_str());
#else
	
		g_pPlutoLogger->Write(LV_STATUS,"Socket::ReceiveData failed, bytes left %d socket %d %s",m_SockBufBytesLeft,m_Socket,m_sName.c_str());
#endif
				return false;
			}
			m_pCurInsockBuffer = m_InSockBuffer;

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		char *tmp2 = new char[m_SockBufBytesLeft +1];
		memcpy(tmp2, m_InSockBuffer, m_SockBufBytesLeft );
		char *p2=tmp2;
		while(p2<tmp2+m_SockBufBytesLeft)
		{
			if (*p2=='\b')
				*p2='|';
			p2++;
		}
		tmp2[m_SockBufBytesLeft]=0;
if( m_SockBufBytesLeft<200 )
{
		LACA_B4_2("(%s)Received: %s\n",m_sName.c_str(),tmp2);
}
else
{
		LACA_B4_1("(%s)Received: big block\n",m_sName.c_str());
}
		delete[] tmp2;
#endif

			#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
				char *tmp = new char[m_SockBufBytesLeft +1];
				memcpy(tmp, m_InSockBuffer, m_SockBufBytesLeft );
				char *p=tmp;
				while(p<tmp+m_SockBufBytesLeft)
				{
					if (*p=='\b')
						*p='|';
					p++;
				}
				tmp[m_SockBufBytesLeft]=0;
			   #ifdef UNDER_CE
				char c[50];
				SYSTEMTIME lt;
				::GetLocalTime(&lt);
				sprintf(c,"%d/%d/%d %d:%d:%d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond);
			   #else  
			   	time_t ts;
				time(&ts);
				struct tm *t = localtime(&ts);
				char c[50];
				snprintf(c,sizeof(c),"%02d/%02d/%02d %d:%02d:%02d",(int) t->tm_mon+1,(int) t->tm_mday,(int) t->tm_year-100,(int) t->tm_hour,(int) t->tm_min, (int)t->tm_sec);
			   #endif
			#ifdef LL_DEBUG
				printf("%s-%s\tReceived: %s\n",Module, c,tmp);
			#endif
			#ifdef LL_DEBUG_FILE
				PLUTO_SAFETY_LOCK_ERRORSONLY(ll3,(*m_LL_DEBUG_Mutex));
				FILE *f = fopen(SockLogFile, "a");
				if( !f )
				{
					f = fopen(SockLogErrorFile, "a");
					// Don't check -- if this still fails just throw an exception something is very wrong!
					fprintf(f,"Cannot write to regular sock log\n");
					fprintf(f, "%d-%s-%s\tReceiving Binary (%d bytes): %s\n\n", m_Socket,Module, c,m_SockBufBytesLeft , tmp);
					fclose(f);
				}
				else
				{
					fprintf(f, "%d-%s-%s\tReceiving Binary (%d bytes): %s\n\n", m_Socket,Module, c,m_SockBufBytesLeft , tmp);
					fclose(f);
				}
				ll3.Release();
			#endif
				delete[] tmp;
			#endif
		}
	}
	return true;
}

bool Socket::ReceiveString(string &refstring)
{
	char    buf[4096], *pbuf;
	int		len=sizeof(buf)-1;
	if((m_Socket == INVALID_SOCKET) || (buf == NULL))
	{
		
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket::ReceiveString failed, m_socket %d buf %p %s",m_Socket,buf,m_sName.c_str());
			PlutoLock::DumpOutstandingLocks();
		}

		return false;
	}

	pbuf = buf;

	do
	{
		if (!ReceiveData(1, pbuf))
		{
			refstring = "ReceiveData failed";
			
			g_pPlutoLogger->Write(LV_STATUS,"Socket::ReceiveString ReceiveData failed %d %s",m_Socket,m_sName.c_str());
			return false;
		}
		++pbuf;
		--len;
	} while(*(pbuf-1)!='\n' && *(pbuf-1)!=0 && len);
	if (!len)
	{
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK_ERRORSONLY(ll4,(*m_LL_DEBUG_Mutex));
	FILE *f = fopen(SockLogFile, "a");
		if( !f )
		{
			f = fopen(SockLogErrorFile, "a");
			// Don't check -- if this still fails just throw an exception something is very wrong!
			fprintf(f,"Cannot write to regular sock log\n");
			fprintf(f, "%d-%s !!!!!! OVERFLOW !!!!!!!!!\n\n",m_Socket,Module);
			fclose(f);
		}
		else
		{
			fprintf(f, "%d-%s !!!!!! OVERFLOW !!!!!!!!!\n\n",m_Socket,Module);
			fclose(f);
		}
	ll4.Release();
#else
		// Yikes, we received more than sizeof(buf) characters.  Must be spewing.  Drop the connection.
		closesocket(m_Socket);
#endif
		refstring="Not a string, or excessive length";
			
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Socket::ReceiveString failed len: %d socket: %d %s",len,m_Socket,m_sName.c_str());
				PlutoLock::DumpOutstandingLocks();
			}
		return false;
	}
	*pbuf = 0;
	pbuf--;
	while(*pbuf == '\r' || *pbuf == '\n' && pbuf > buf)
	{
		*pbuf-- = '\0';
	}
	refstring=buf;
#ifdef LL_DEBUG
	printf("%s-Received String: %s\n",Module,buf);
#endif
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK(ll4,(*m_LL_DEBUG_Mutex)); 
	FILE *f = fopen(SockLogFile, "a");
	if( !f )
	{
		f = fopen(SockLogErrorFile, "a");
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf(f,"Cannot write to regular sock log\n");
		fprintf(f, "%d-%s-Received Text: %s\n\n", m_Socket,Module, buf);
		fclose(f);
	}
	else
	{
		fprintf(f, "%d-%s-Received Text: %s\n\n", m_Socket,Module, buf);
		fclose(f);
	}
	ll4.Release(); 
#endif

#ifdef LOG_ALL_CONTROLLER_ACTIVITY
	if( m_sName.substr(0,6)!="Logger")
	{
		LACA_B4_2("Received string: %s %s",m_sName.c_str(),refstring.c_str());
	}
#endif
	return true;
cout << "temp: received string in dce socket" << endl;		
}

bool Socket::SendString(string Line)
{
#ifdef LL_DEBUG
	printf("%s-Send strng: %s\n", Module, Line.c_str());
#endif
#ifdef LL_DEBUG_FILE
	PLUTO_SAFETY_LOCK(ll,(*m_LL_DEBUG_Mutex)); 
	FILE *f = fopen(SockLogFile, "a");
	if( !f )
	{
		f = fopen(SockLogErrorFile, "a");
		// Don't check -- if this still fails just throw an exception something is very wrong!
		fprintf(f,"Cannot write to regular sock log\n");
		fprintf(f, "%d-%s-Send Text: %s\n\n", m_Socket,Module, Line.c_str());
		fclose(f);
	}
	else
	{
		fprintf(f, "%d-%s-Send Text: %s\n\n", m_Socket,Module, Line.c_str());
		fclose(f);
	}
	ll.Release();
#endif
	Line+="\n";
	return SendData((int)Line.length(), Line.c_str());
}

string Socket::SendReceiveString(string s)
{
	// Protect the whole operation
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // Don't log anything but failures
	SendString(s);

	string Response;
	if( ReceiveString(Response) )
		return Response;
	return "";
}
