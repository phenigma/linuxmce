#include "FadeBase.h"

FadeBase::FadeBase(MeshFrame* AfterFrame, MeshFrame*BeforeFrame, int StartTime, int TimeToFade)
{
	this->StartTime = StartTime;
	this->TimeToFade = TimeToFade;
	this->AfterFrame = AfterFrame;
	this->BeforeFrame = BeforeFrame;
}

FadeBase::~FadeBase(void)
{
}
