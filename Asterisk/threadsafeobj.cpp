//
// C++ Implementation: threadsafeobj
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "threadsafeobj.h"

namespace ASTERISK {

ThreadSafeObj::ThreadSafeObj() {
}
ThreadSafeObj::~ThreadSafeObj() {
}

void ThreadSafeObj::Lock() {
	m.Lock();
}

void ThreadSafeObj::Unlock() {
	m.Unlock();
}


};
