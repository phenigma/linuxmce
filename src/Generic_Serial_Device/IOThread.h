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

namespace DCE {

/**
@author 
*/
class IOThread {
public:
    IOThread(bool usemain = false); // set true to use main thread
    virtual ~IOThread();

	int Run(bool wait = true);
	void Wait(bool requeststop = false);
	bool isRunning();
	
protected:
	virtual void* _Run();
	
	bool isStopRequested() {
		return requeststop_;
	};
	
protected:
	virtual bool handleBeforeStartup() { return true; };
	virtual void handleAfterTerminate() {};
	virtual bool handleStartup() { return true; };
	virtual void handleTerminate() {};
	
private:
	bool usemain_;
	pthread_t threadid_;
	bool requeststop_;
	static void* _threadproc(void *arg);
};

};

#endif
