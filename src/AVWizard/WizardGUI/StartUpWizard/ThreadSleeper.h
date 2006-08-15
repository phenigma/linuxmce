#ifndef ThreadSleeper_H_
#define ThreadSleeper_H_

#include <pthread.h>

class ThreadSleeper
{
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

void* SleeperThreadFunc(void* Instance);

#endif //ThreadSleeper_H_
