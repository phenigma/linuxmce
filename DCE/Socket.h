#ifndef Socket_H
#define Socket_H

#include "PlutoUtils/MultiThreadIncludes.h"

#define INSOCKBUFFER_SIZE 8192

namespace DCE
{
	class Message;

	class Socket
	{
	public:
		pluto_pthread_mutex_t m_SocketMutex;			// AB 1-25-2004 - move these to public since Event_Impl needs to use them
		pthread_mutexattr_t m_SocketMutexAttr;

		char *m_InSockBuffer, *m_pCurInsockBuffer;
		int m_SockBufBytesLeft, m_ReceiveTimeout;
		SOCKET m_Socket;
		string m_sName;
		int m_SocketCounter;

		Socket(string Name);
		virtual ~Socket();

	//	void SocketLock();
	//	void SocketUnlock();

		// This issues both a MESSAGE <Length> string
		// and immediately sends the data.  It deletes pMsg when it's done.
		bool SendMessage(Message *pMsg, bool bDeleteMessage = true);

		void SetReceiveTimeout(int TimeoutSeconds) { m_ReceiveTimeout = TimeoutSeconds; };

		// This gets called in response to a MESSAGE <Length> string
		// It reads the data from the socket then parses it into a new Message.
		virtual Message *ReceiveMessage(int Length);

		// Just sends raw data to the socket.
		virtual bool SendData(int size, const char *data);
		virtual bool ReceiveData(int size, char *data);

		// String functions.
		virtual bool SendString(string s);
		virtual bool ReceiveString(string &s);

		virtual string SendReceiveString(string s);
		virtual Message *SendReceiveMessage(Message *pMessage);

		char *SockLogFile,*SockLogErrorFile;
	};
}

#endif

