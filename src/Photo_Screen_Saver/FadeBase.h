#ifndef FadeBase_H_
#define FadeBase_H_

#include "Frame.h"

class FadeBase
{
protected:
	int StartTime;
	int TimeToFade;
	Frame* AfterFrame;
	Frame*BeforeFrame;
public:
	FadeBase(Frame* AfterFrame, Frame*BeforeFrame, int StartTime, int TimeToFade);
	virtual bool Update(int Time) = 0;
	virtual ~FadeBase(void);
};

#endif

