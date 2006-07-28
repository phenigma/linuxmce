//
// C++ Implementation: ThreadSleeper
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ThreadSleeper.h"

#include "SafetyLock.h"
#include "GUIWizardUtils.h"
ThreadSleeper::ThreadSleeper(int NoSeconds)
{
	bQuit = false;

	TickRemaining = NoSeconds*1000;
	pthread_mutex_init(&lockmutex, NULL);

	pthread_create(&tid, NULL, SleeperThreadFunc,this);
}

ThreadSleeper::~ThreadSleeper()
{
	Quit();
	pthread_join(tid, NULL);
}

int ThreadSleeper::GetSecondRemaining()
{
	SafetyLock Lock(&lockmutex);
	return (TickRemaining + 999) / 1000;
}

bool ThreadSleeper::Quit()
{
	bQuit = true;
	SafetyLock Lock(&lockmutex);
	
	return true;
}

void* SleeperThreadFunc(void* Instance)
{
	ThreadSleeper* ThreadPtr= (ThreadSleeper*) Instance;
	ThreadPtr->SecondTick();
	while(!ThreadPtr->bQuit && ThreadPtr->GetSecondRemaining())
	{
		Sleep(100);
		int Seconds1 = ThreadPtr->GetSecondRemaining();
		ThreadPtr->TickRemaining -= 100;
		int Seconds2 = ThreadPtr->GetSecondRemaining();
		if (Seconds2 != Seconds1)
			ThreadPtr->SecondTick();
	}
	return NULL;
}
