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
#include "condition.h"

namespace ASTERISK {

Condition::Condition()
{
	pthread_cond_init(&cond, 0);
}


Condition::~Condition() {
	pthread_cond_destroy(&cond);
}

void Condition::Wait() {
	pthread_cond_wait(&cond, &m.getHandle());
}

void Condition::Signal() {
	pthread_cond_signal(&cond);
}

};
