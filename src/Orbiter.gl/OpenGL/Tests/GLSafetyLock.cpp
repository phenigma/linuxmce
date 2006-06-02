#include "GLSafetyLock.h"

GLSafetyLock::GLSafetyLock(pthread_mutex_t* Mutex)
{
	this->Mutex = Mutex;
	pthread_mutex_lock(Mutex);
}

GLSafetyLock::~GLSafetyLock()
{
	pthread_mutex_unlock(Mutex);
}
