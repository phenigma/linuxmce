#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include "Socket.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace std;

namespace DCE
{
	class SocketListener;

	class ServerSocket : public Socket
	{
	public:
		int m_DeviceID, m_bFlaggedForClosure;
		pluto_pthread_mutex_t m_ConnectionMutex;
		pthread_t m_ClientThreadID;



		SocketListener *m_pListener;
		class Command_Impl *m_pCommand;

		ServerSocket(SocketListener *pListener, SOCKET Sock,string Name);
		virtual ~ServerSocket();

		void Run();
	};
}
#endif
