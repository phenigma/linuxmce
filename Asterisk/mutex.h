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
#ifndef ASTERISKMUTEX_H
#define ASTERISKMUTEX_H

#include <pthread.h>

namespace ASTERISK {

/**
@author 
*/
class Mutex{
public:
    Mutex();
    ~Mutex();

public:	
    int Lock();
    void Unlock();
	
    pthread_mutex_t getHandle() const {
	return mutexid;
    }

public:
	pthread_mutex_t mutexid;
};

};

#endif
