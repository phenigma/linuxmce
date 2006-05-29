//
// C++ Interface: SafetyLock
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SAFETYLOCK_H
#define SAFETYLOCK_H

/**
Safety lock via mutexes class

	@author Ciprian Mustiata <ciprian.m@plutohome.com>
*/
#include <pthread.h>

class SafetyLock
{
	pthread_mutex_t* Mutex;
public:
	SafetyLock(pthread_mutex_t* Mutex);
	virtual ~SafetyLock();
};

#endif
