#ifndef GLSAFETYLOCK_H_
#define GLSAFETYLOCK_H_

#include <pthread.h>

class GLSafetyLock
{
	pthread_mutex_t* Mutex;
public:
	GLSafetyLock(pthread_mutex_t* Mutex);
	virtual ~GLSafetyLock();
};

#endif /*GLSAFETYLOCK_H_*/
