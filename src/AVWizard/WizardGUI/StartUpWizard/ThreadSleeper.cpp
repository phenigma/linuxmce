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

#include <SDL.h>
#include <iostream>

void* SleeperThreadFunc(void* Instance);

ThreadSleeper::ThreadSleeper(int NoSeconds) : tid(0)
{
	std::cout<<"ThreadSleeper::ThreadSleeper"<<std::endl;
	bQuit = false;


	TickRemaining = NoSeconds*1000;
	LastTime = SDL_GetTicks();
}

ThreadSleeper::~ThreadSleeper()
{	
}

void ThreadSleeper::Init()
{
	std::cout<<"ThreadSleeper::Init"<<std::endl;
	pthread_mutexattr_t MutexAttr;
	pthread_mutexattr_init(&MutexAttr);
	pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&lockmutex, &MutexAttr);
	pthread_mutexattr_destroy(&MutexAttr);

	pthread_create(&tid, NULL, SleeperThreadFunc,this);

}

int ThreadSleeper::GetSecondRemaining()
{
	std::cout<<"ThreadSleeper::GetSecondRemaining()"<<std::endl;
	int CurrentTime = SDL_GetTicks();
	TickRemaining = TickRemaining - (CurrentTime - LastTime);
	LastTime = CurrentTime;
	int Result = 0;
	if ((TickRemaining + 999) / 1000 > 0)
		Result = (TickRemaining + 999) / 1000;
	std::cout<<"remaining: " <<TickRemaining<<std::endl;
	std::cout<<"Seconds: " <<Result<<std::endl;
	return Result;
}

bool ThreadSleeper::Quit()
{
	std::cout<<"ThreadSleeper::Quit"<<std::endl;
	bQuit = true;
	pthread_join(tid, NULL);
	return true;
}

void* SleeperThreadFunc(void* Instance)
{
	ThreadSleeper* ThreadPtr= (ThreadSleeper*) Instance;
	ThreadPtr->SecondTick();
	int Seconds1 = ThreadPtr->GetSecondRemaining();
	while(!ThreadPtr->bQuit && ThreadPtr->GetSecondRemaining())
	{
		Sleep(100);
		int Seconds2 = ThreadPtr->GetSecondRemaining();

		if (Seconds2 != Seconds1)
		{
			Seconds1 = Seconds2;
			std::cout<<"ThreadSleeper::SecondTick()"<<std::endl;
			ThreadPtr->SecondTick();
		}
	}

	std::cout<<"TreadQuit"<<std::endl;
	return NULL;
}
