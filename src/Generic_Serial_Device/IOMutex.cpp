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
#include "IOMutex.h"

namespace DCE {

IOMutex::IOMutex() {
	pthread_mutex_init(&mutexid, 0);
}


IOMutex::~IOMutex(){
	pthread_mutex_destroy(&mutexid);
}

int 
IOMutex::Lock() {
	return pthread_mutex_lock(&mutexid);
}

void 
IOMutex::Unlock() {
	pthread_mutex_unlock(&mutexid);
}

};
