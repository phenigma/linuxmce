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
//#include "DCE/Logger.h"

namespace MYTHTV {

Thread::Thread()
	: threadid_(0), requeststop_(false)
{
}


Thread::~Thread() {
	Wait(true);
}

bool
Thread::isRunning() {
	return (threadid_ != 0);
}

int 
Thread::Run(bool wait) {
	if(!handleStartup()) {
		return false;	
	}
	
	int ret = pthread_create(&threadid_, NULL, _threadproc, (void*)this);
	if(ret == 0) {
//		DCE::g_pPlutoLogger->Write(LV_STATUS, "Thread %d created", threadid);
	} else {
//		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "Error creating thread");
		threadid_ = 0;
	}
	
	if(wait) {
		Wait();
	}
	return ret;
}

void 
Thread::Wait(bool requeststop) {
	if(threadid_ != 0) {
		requeststop_ = requeststop;
		pthread_join(threadid_, 0);
		threadid_ = 0;
	}
}

void* 
Thread::_Run() {
	return 0;
}

void* Thread::_threadproc(void *arg) {
	Thread* pme = (Thread*)arg;

	void* pret = pme->_Run();
	pme->threadid_ = 0;
	pme->requeststop_ = false;
	pme->handleTerminate();
	pthread_exit(pret);
	return 0;
}

};
