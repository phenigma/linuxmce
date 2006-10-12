#ifndef GalleryScenario_H_
#define GalleryScenario_H_

#include "GaleryStateMachine.h"
#include "FileBrowser.h"
#include "FadeFactory.h"
#include "ZoomFactory.h"
#include "ZoomBase.h"
#include "AnimatedPicture.h"


class GalleryScenario
{
	AnimatedPicture* AfterPicture;
	AnimatedPicture* BeforePicture;

	GaleryStateMachine* StateMachine;
	int StartTime;
	int StartFrame;
	int FaddingTime;
	int ZoomTime;
	int nLastTimeUpdated;

	int NewImageStartTime;
	bool m_bUseAnimation;

	FileBrowser* Browser;
	FadeBase* Fades;
	int Width;
	int Height;

	ZoomBase* ZoomEffect;
public:
	GalleryScenario(int Width, int Height, int FaddingTime, int ZoomTime, 
		string SearchImageFolder, bool bUseAnimation);
	~GalleryScenario();
	bool Update(void);
};

#endif
