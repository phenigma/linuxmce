#ifndef HANDLEREQUEST_SOCKET_H
#define HANDLEREQUEST_SOCKET_H

#include "ClientSocket.h"
namespace DCE
{
	class HandleRequestSocket : public ClientSocket
	{
	public:
		pthread_t m_RequestHandlerThread;

		bool m_bRunning, m_bUnexpected, m_bTerminate;

		HandleRequestSocket(int DeviceID, string IPAddress, string Name);
		virtual ~HandleRequestSocket();

		virtual void Disconnect(); 
		virtual void DisconnectAndWait();

		virtual bool OnConnect(string ExtraInfo="");
		virtual void OnUnexpectedDisconnect();

		virtual void ReceivedString(string Line)=0;
		virtual bool ReceivedMessage(Message *pMessage)=0;

		virtual void RunThread();
	};
}
#endif
