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
#include "IOThread.h"
//#include "DCE/Logger.h"

namespace DCE {

IOThread::IOThread(bool usemain)
	: usemain_(usemain), threadid_(0), requeststop_(false)
{
}


IOThread::~IOThread() {
}

bool
IOThread::isRunning() {
	return (threadid_ != 0);
}

int 
IOThread::Run(bool wait) {
	int ret = 0;
	if(!usemain_) {
		ret = pthread_create(&threadid_, NULL, _threadproc, (void*)this);
	} else {
		_threadproc(this);
	}
	
	if(ret == 0) {
//		DCE::g_pPlutoLogger->Write(LV_STATUS, "IOThread %d created", threadid);
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
IOThread::Wait(bool requeststop) {
	if(usemain_) {
		return;
	}
	if(threadid_ != 0) {
		if(requeststop) {
			SignalStop();
		}
		pthread_join(threadid_, 0);
		threadid_ = 0;
	}
}

void 
IOThread::SignalStop() {
	requeststop_ = true;
}

void* 
IOThread::_Run() {
	return 0;
}

void* IOThread::_threadproc(void *arg) {
	IOThread* pme = (IOThread*)arg;
	
	if(pme->handleStartup()) {
		void* pret = pme->_Run();
		pme->threadid_ = 0;
		pme->requeststop_ = false;
		pme->handleTerminate();
		if(pme->threadid_ != 0) {
			pthread_exit(pret);
		}
	}
	return 0;
}

};
