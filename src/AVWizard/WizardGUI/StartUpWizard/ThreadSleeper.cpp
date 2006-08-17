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

#include "GUIWizardUtils.h"

#include "WizardWidgetLabel.h"
#include "Wizard.h"
#include "SafetyLock.h"

#include <SDL.h>
#include <iostream>

void* SleeperThreadFunc(void* Instance);

ThreadSleeper::ThreadSleeper()
{
	tid = 0;
	pthread_mutex_init(&LockMutex, NULL);
}

void ThreadSleeper::Init(int NoSeconds)
{
	std::cout << "1 tid: " << tid << std::endl;

	if(tid != 0)
	{
		std::cout<<"ThreadSleeper::Init-> Race condition,should not happend"<<std::endl;
		return;
	}
	this->TickRemaining = NoSeconds*1000;
	std::cout<<"ThreadSleeper::Init"<<std::endl;
	LastTime = SDL_GetTicks();
	bQuit = false;
	pthread_create(&tid, NULL, SleeperThreadFunc,this);

	std::cout << "2 tid: " << tid << std::endl;
}

int ThreadSleeper::GetSecondRemaining()
{
	std::cout<<"ThreadSleeper::GetSecondRemaining()"<<std::endl;
	SafetyLock Lock(&LockMutex);
	std::cout<<"ThreadSleeper::GetSecondRemaining() got lock"<<std::endl;
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

void ThreadSleeper::Quit()
{
	{
	//SafetyLock Lock(&LockMutex);
	std::cout<<"ThreadSleeper::Quit"<<std::endl;
	bQuit = true;
	}

	//if(tid)
//		pthread_join(tid, NULL);
	tid = 0;
	std::cout<<"ThreadSleeper::Quit: tid is 0"<<std::endl;
}

void ThreadSleeper::SecondTick()
{
#ifdef DEBUG
	std::cout<< "VideoResolutionSecondSleeper::SecondTick()" << std::endl;
#endif
	if(!Label)
	{
		std::cout<<"VideoResolutionSecondSleeper::SecondTick Warning! No label = nothing to draw";
		return;
	}
	int Seconds = GetSecondRemaining();

	SafetyLock Lock(&LockMutex);
	std::string LabelCaption = Utils::Int32ToString(Seconds);
	Label->SetCaption(LabelCaption);
	WM_Event Event;
	std::cout<<"Seconds: "<<Seconds<<std::endl;
	if(Seconds)
		Event.DownKey();
	else
		Event.EscapeKey();
	Wizard::GetInstance()->GenerateCustomEvent(Event);
}

/*virtual*/ void ThreadSleeper::SetLabel(WizardWidgetLabel* Label)
{
	SafetyLock Lock(&LockMutex);
	this->Label = Label;
}

ThreadSleeper* ThreadSleeper::Instance_ = NULL;
ThreadSleeper* ThreadSleeper::Instance()
{
	if(!Instance_)
		Instance_ = new ThreadSleeper();
	return Instance_;
}

void* SleeperThreadFunc(void* Instance)
{
	ThreadSleeper* ThreadPtr= (ThreadSleeper*) Instance;
	ThreadPtr->SecondTick();
	int Seconds1 = ThreadPtr->GetSecondRemaining();
	std::cout<<"SleeperThreadFunc..."<<std::endl;

	while(!ThreadPtr->bQuit && ThreadPtr->GetSecondRemaining())
	{
		std::cout<<"sleeper..."<<std::endl;
		Sleep(50);
		int Seconds2 = ThreadPtr->GetSecondRemaining();

		if (Seconds2 != Seconds1)
		{
			Seconds1 = Seconds2;
			std::cout<<"ThreadSleeper::SecondTick()"<<std::endl;
			ThreadPtr->SecondTick();
		}
		else
			std::cout<<"else-ul"<<std::endl;
	}

	std::cout<<"TreadQuit"<<std::endl;
	ThreadPtr->Quit();
	return NULL;
}

