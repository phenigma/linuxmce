#ifndef __ServerManagement_h_
#define __ServerManagement_h_

#include "DCE/PlainClientSocket.h"
#include "MapManagement.h"


class ServerManagement{
	
public:
	static ServerManagement* getInstance();
	static void* ServerManagement_Thread(void *p);
	
	~ServerManagement();

private:

	ServerManagement();
	void Run();
	
	static ServerManagement* instance;
	pthread_mutexattr_t m_MutexAttr;
	pluto_pthread_mutex_t TribuneMutex;
};

#endif
