#ifndef __ServerManagement_h_
#define __ServerManagement_h_

#include "DCE/PlainClientSocket.h"
#include "MapManagement.h"
#include <map>

/** Singelton class that populate the maps for the server*/
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
	map<string,string> maptmpProgramRecord;
	map<int,string> maptmpStation;
	map<u_int64_t,string> maptmpSchedule;
	map<string,string> maptmpActor;
	map<string,string> maptmpGenre;
	map<string,string> maptmpRole;

};

#endif
