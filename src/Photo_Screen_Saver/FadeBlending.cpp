#include "FadeBlending.h"

#include "SDL.h"

FadeBlending::FadeBlending(MeshFrame* AfterFrame, MeshFrame*BeforeFrame, int StartTime, int TimeToFade)
: FadeBase(AfterFrame, BeforeFrame, StartTime, TimeToFade)
{
}

FadeBlending::~FadeBlending(void)
{
}

bool FadeBlending::Update(int Time)
{
	bool Result = false;
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int ProgressTime = Time - StartTime;
	float Progress = (float) ProgressTime / TimeToFade;
	if (Progress > 1.0f)
	{
		Result = true;
		Progress = 1.0f;
	}

	AfterFrame->SetAlpha(Progress);
	BeforeFrame->SetAlpha(1-Progress);
	return Result;
}
