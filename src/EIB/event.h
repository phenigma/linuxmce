//
// C++ Interface: event
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSEVENT_H
#define EIBBUSEVENT_H

#include <pthread.h>

namespace EIBBUS {

/**
@author 
*/
class Event
{
public:
    Event();
    ~Event();

public:
	void Signal();
	bool Wait(int milisecs = -1);

private:
	pthread_cond_t event_;
	pthread_mutex_t mutex_;
};

};

#endif
