//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYTHTVTHREAD_H
#define MYTHTVTHREAD_H

#include <pthread.h>

namespace EIBBUS {

/**
@author 
*/
class Thread {
public:
    Thread();
    virtual ~Thread();

	virtual int Run(bool wait = true);
	void Wait(bool requeststop = false);
	bool isRunning();
	
protected:
	virtual void* _Run();
	
	bool isStopRequested() {
		return requeststop_;
	};
	
protected:
	virtual bool handleStartup() {
		return true;
	};
	virtual void handleTerminate() {};
	
private:
	bool requeststop_;
	pthread_t threadid_;
	static void* _threadproc(void *arg);
};

};

#endif
