//
// C++ Implementation: event
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "event.h"

#include <time.h>

namespace EIBBUS {

Event::Event() {
	sem_init(&event_, 0, 0);
}


Event::~Event() {
	sem_destroy(&event_);
}

void 
Event::Post() {
	sem_post(&event_);
}

bool 
Event::Wait(int milisecs) {
	if(milisecs < 0) {
		sem_wait(&event_);
		return true;
	} else {
		struct timespec timespec;
		clock_gettime(CLOCK_REALTIME, &timespec);
		
		timespec.tv_sec += milisecs / 1000;
		timespec.tv_nsec += milisecs % 1000;
		
		return (sem_timedwait(&event_, &timespec) == 0);
	}
}

bool 
Event::TryWait() {
	return (sem_trywait(&event_) == 0);
}

};
