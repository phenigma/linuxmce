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

#include <semaphore.h>

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
	void Post();
	bool Wait(int milisecs = -1);
	bool TryWait();

private:
	sem_t event_;
};

};

#endif
