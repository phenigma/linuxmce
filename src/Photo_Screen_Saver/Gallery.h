#ifndef Gallery_H_
#define Gallery_H_

#include "DCE/Logger.h"

#include "SDLFrontEnd.h"
#include "GalleryScenario.h"
#include "ExtensionManager.h"

#include <string>
using namespace std;

class ProcessUtils
{
public:
	static unsigned long g_SecondsReset;
	void ResetMsTime();
	unsigned long GetMicroTime();
};

class Gallery
{
	Gallery(void);
	void PaintScreen(void);

	ExtensionManager Extensions;
	static Gallery* Instance_;
	SDLFrontEnd* FrontEnd;
	bool Quit;
	pluto_pthread_mutex_t m_FrontEndMutex;

	/**
	* Last pending event definition
	*/
	WM_Event Event; 
	GalleryScenario* Scenario;
	ProcessUtils Utils;

public:
	~Gallery(void);
	void MainLoop(bool * m_bQuit_get());
	static Gallery* Instance(void);

    bool Setup(int Width, int Height, int FaddingTime, int ZoomTime, string FolderName, bool bUseAnimation);
	void CleanUp(void);
	void EvaluateEvent(WM_Event Event);
	void _Sleep(int Miliseconds);
};

#endif
