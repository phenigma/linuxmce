#ifndef ThreadSleeper_H_
#define ThreadSleeper_H_

#include <pthread.h>

class ThreadSleeper
{
	int LastTime;
public:
	int TickRemaining;

	pthread_t tid;
	pthread_mutex_t lockmutex;
	bool bQuit;
	
	ThreadSleeper(int NoSeconds);
	virtual ~ThreadSleeper();
	int GetSecondRemaining();
	virtual void Init();
	bool Quit();
	virtual void SecondTick() = 0;
};

#endif //ThreadSleeper_H_
