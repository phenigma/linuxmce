#pragma once

#include "MeshFrame.h"

class FadeBase
{
protected:
	int StartTime;
	int TimeToFade;
	MeshFrame* AfterFrame;
	MeshFrame*BeforeFrame;
public:
	FadeBase(MeshFrame* AfterFrame, MeshFrame*BeforeFrame, int StartTime, int TimeToFade);
	virtual bool Update(int Time) = 0;
	~FadeBase(void);
};
