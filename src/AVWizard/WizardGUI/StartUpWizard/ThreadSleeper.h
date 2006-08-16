#ifndef ThreadSleeper_H_
#define ThreadSleeper_H_

#include <pthread.h>

class WizardWidgetLabel;
class ThreadSleeper
{
	int TickRemaining;
	WizardWidgetLabel* Label;
	static ThreadSleeper* Instance_;
	pthread_t tid;
	pthread_mutex_t LockMutex;
	int LastTime;
	ThreadSleeper();
public:
	bool bQuit;
	static ThreadSleeper* Instance();

	int GetSecondRemaining();
	void Init(int NoSeconds);
	void Quit();
	void SecondTick();
	void SetLabel(WizardWidgetLabel* Label);
};

#endif //ThreadSleeper_H_
