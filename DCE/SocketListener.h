#ifndef SocketLISTENER_H
#define SocketLISTENER_H

#include <map>
#include <list>
#include <string>
#include "ServerSocket.h"
#include "DCE/Logger.h"

namespace DCE
{
	class ServerSocket;
	class Message; 

	typedef ::std::map<int, ServerSocket *> DeviceClientMap;

	class SocketListener 
	{
	protected:
		pthread_t m_ListenerThreadID;
		int m_ListenPort;
		SOCKET m_Socket;
	public:
		string m_sName;
		pluto_pthread_mutex_t m_ListenerMutex;

		virtual bool ReceivedString(Socket *pSocket, string Line)=0;
		virtual void ReceivedMessage(Socket *pSocket, Message* pMessage)=0;

		void FlagForRemoval(int DeviceID);
		bool SendString(int DeviceID, string Line);
		bool SendMessage(int DeviceID, Message *pMessage);
		bool SendData(int DeviceID, int Length, const char *Data);

		bool m_bTerminate, m_bRunning, m_bClosed;
		::std::list<Socket *> m_Clients;
		DeviceClientMap m_mapCommandHandlers;

	#ifdef TEST_DISCONNECT

		Socket *m_pTestDisconnectCmdHandler;
		Socket *m_pTestDisconnectEvent;

	#endif

		void StartListening(int PortNumber);
	//	void StopListening();
		virtual void OnDisconnected(int DeviceID)
		{}

		void RegisterCommandHandler(ServerSocket *Socket, int DeviceID);
		virtual void RegisteredCommandHandler(int DeviceID) = 0;
		
		virtual Socket *CreateSocket(SOCKET newsock,string Name);
		void RemoveSocket(Socket *Socket);

		::std::string GetIPAddress();
		::std::string GetIPAddress(int DeviceID);

		SocketListener(string Name);
		virtual ~SocketListener();

		void Run();
	};
}

#endif
