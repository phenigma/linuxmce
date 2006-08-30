#ifndef FadeFactory_H_
#define FadeFactory_H_

#include "FadeBase.h"

class FadeFactory
{
	FadeFactory(void);
	static FadeFactory* Instance_;

public:
	static FadeFactory* Instance();

	virtual ~FadeFactory(void);

	FadeBase* CreateFadeMode(int Mode, MeshFrame* AfterFrame, MeshFrame*BeforeFrame, 
		int StartTime, int TimeToFade);
};

#endif
