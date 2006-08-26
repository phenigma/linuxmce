#pragma once

#include "FadeBase.h"

class FadeBlending : public FadeBase
{
public:
	FadeBlending(MeshFrame* AfterFrame, MeshFrame*BeforeFrame, int StartTime, int TimeToFade);
	~FadeBlending();

	bool Update(int Time);
};
