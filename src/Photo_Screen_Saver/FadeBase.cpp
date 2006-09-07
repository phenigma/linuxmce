#include "FadeBase.h"

FadeBase::FadeBase(Frame* AfterFrame, Frame*BeforeFrame, int StartTime, int TimeToFade)
{
	this->StartTime = StartTime;
	this->TimeToFade = TimeToFade;
	this->AfterFrame = AfterFrame;
	this->BeforeFrame = BeforeFrame;
}

FadeBase::~FadeBase(void)
{
}
