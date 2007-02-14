#ifndef ServerManagement_h
#define ServerManagement_h

#include "DCE/PlainClientSocket.h"
#include "MapManagement.h"


class ServerManagement{
	
public:
	static ServerManagement* getInstance();
	static void ServerManagement_Thread(void *p);

private:
	pthread_mutexattr_t m_MutexAttr;
	pluto_pthread_mutex_t TribuneMutex;
	static ServerManagement* instance;
	static MapManagement* mapManag;
	ServerManagement();
	void Run();
}

#endif