//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mutex.h"

namespace EIBBUS {

Mutex::Mutex() {
	pthread_mutex_init(&mutexid, 0);
}


Mutex::~Mutex(){
	pthread_mutex_destroy(&mutexid);
}

int 
Mutex::Lock() {
	return pthread_mutex_lock(&mutexid);
}

void 
Mutex::Unlock() {
	pthread_mutex_unlock(&mutexid);
}

};
