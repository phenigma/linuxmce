#ifndef RASERVER_SOCKET_H
#define RASERVER_SOCKET_H

#include "Socket.h"

using namespace std;

class RAServerSocket : public DCE::Socket
{
public:
	pthread_t m_ClientThreadID;
    
	class Logger *m_pLogger;

	class RAServer *m_pListener;

	RAServerSocket(class RAServer *pListener, SOCKET Sock);
	virtual ~RAServerSocket();

	void Run();
};

#endif

