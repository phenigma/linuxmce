//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "thread.h"
#include "DCE/Logger.h"

namespace CM11ADEV {

Thread::Thread()
	: isrunning(false)
{
}


Thread::~Thread() {
g_pPlutoLogger->Write(LV_STATUS,"Destroying Thread - before Wait");
	Wait();
g_pPlutoLogger->Write(LV_STATUS,"Destroying Thread - After Wait");
}

bool
Thread::isRunning() {
	return isrunning;
}

int 
Thread::Run(bool wait) {
	isrunning = true;
	
	handleStartup();
	
	int ret = pthread_create(&threadid, NULL, _threadproc, (void*)this);
	if(ret == 0) {
		DCE::g_pPlutoLogger->Write(LV_STATUS, "Thread %d created", threadid);
	} else {
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "Error creating thread");
		isrunning = false;
	}
	
	if(wait) {
		Wait();
	}
	return ret;
}

void 
Thread::Wait() {
	pthread_join(threadid, 0);
}

void* 
Thread::_Run() {
	return 0;
}

void* Thread::_threadproc(void *arg) {
	Thread* pme = (Thread*)arg;

	void* pret = pme->_Run();
	pme->isrunning = false;
	pme->handleTerminate();
	pthread_exit(pret);
	return 0;
}

};
