/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

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
