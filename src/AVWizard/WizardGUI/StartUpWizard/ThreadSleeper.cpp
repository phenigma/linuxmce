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

#include <iostream>

ThreadSleeper::ThreadSleeper(int NoSeconds)
{
	std::cout<<"ThreadSleeper::ThreadSleeper"<<std::endl;
	bQuit = false;

	TickRemaining = NoSeconds*1000;
}

ThreadSleeper::~ThreadSleeper()
{
	Quit();
	pthread_join(tid, NULL);
}

void ThreadSleeper::Init()
{
	std::cout<<"ThreadSleeper::Init"<<std::endl;
	pthread_mutex_init(&lockmutex, NULL);
	pthread_create(&tid, NULL, SleeperThreadFunc,this);

	pthread_mutexattr_t MutexAttr;
	pthread_mutexattr_init(&MutexAttr);
	pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&lockmutex, &MutexAttr);
	pthread_mutexattr_destroy(&MutexAttr);
}

int ThreadSleeper::GetSecondRemaining()
{
	std::cout<<"ThreadSleeper::GetSecondRemaining()"<<std::endl;
	SafetyLock Lock(&lockmutex);
	return (TickRemaining + 999) / 1000;
}

bool ThreadSleeper::Quit()
{
	SafetyLock Lock(&lockmutex);
	bQuit = true;
	
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
		std::cout<<"ThreadSleeper::SecondTick()"<<std::endl;
		SafetyLock Lock(&ThreadPtr->lockmutex);
		if (Seconds2 != Seconds1)
		{
			ThreadPtr->SecondTick();
		}
	}
	return NULL;
}
