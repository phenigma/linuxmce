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
#ifndef ASTERISKCONDITION_H
#define ASTERISKCONDITION_H

#include "mutex.h"

namespace ASTERISK {

/**
@author 
*/
class Condition {
public:
    Condition();
    virtual ~Condition();

    void Wait();
    void Signal();
    
    pthread_cond_t getHandle() {
	return cond;
    }
	
private:
	pthread_cond_t cond;
	Mutex m;
};

};

#endif
