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
#ifndef CM11AMUTEX_H
#define CM11AMUTEX_H

#include <pthread.h>

namespace DCE {

/**
@author 
*/
class IOMutex {
public:
    IOMutex();
    ~IOMutex();

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
