//
// C++ Interface: threadsafeobj
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTHREADSAFEOBJ_H
#define ASTERISKTHREADSAFEOBJ_H

#include "mutex.h"

namespace ASTERISK {

/**
@author 
*/
class ThreadSafeObj {
public:
    ThreadSafeObj();
    ~ThreadSafeObj();

public:
	void Lock();
	void Unlock();

private:
	Mutex m; // safe guard
};

#define LOCKED_OP(a) Lock();a;Unlock();

#define LOCKED_OP_BEGIN Lock()
#define LOCKED_OP_END Unlock();

#define LOCK_OBJ(a) a.Lock();
#define UNLOCK_OBJ(a) a.Unlock();
};

#endif
