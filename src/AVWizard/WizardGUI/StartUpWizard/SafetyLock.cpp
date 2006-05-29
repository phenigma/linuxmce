//
// C++ Implementation: SafetyLock
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SafetyLock.h"

SafetyLock::SafetyLock(pthread_mutex_t* Mutex)
{
	this->Mutex = Mutex;
	pthread_mutex_lock(Mutex);
}


SafetyLock::~SafetyLock()
{
	pthread_mutex_unlock(Mutex);
}


