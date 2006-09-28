#ifndef Gallery_H_
#define Gallery_H_

#include "SDLFrontEnd.h"
#include "GalleryScenario.h"
#include "ExtensionManager.h"

#include <string>
using namespace std;

class ProcessUtils
{
public:
	static unsigned long ProcessUtils::g_SecondsReset;
	void ResetMsTime();
	unsigned long GetMicroTime();
};

class Gallery
{
	ExtensionManager Extensions;
	static Gallery* Instance_;
	void PaintScreen(void);
	SDLFrontEnd* FrontEnd;
	bool Quit;
	/**
	* Last pending event definition
	*/
	WM_Event Event; 
	GalleryScenario* Scenario;
	ProcessUtils Utils;
public:
	Gallery(void);
	~Gallery(void);
	void MainLoop(bool * m_bQuit);
	static Gallery* Instance(void);
public:
	bool Setup(int Width, int Height, int FaddingTime, int ZoomTime, string FolderName);
	void CleanUp(void);
	void EvaluateEvent(WM_Event Event);
	void _Sleep(int Miliseconds);
};

#endif
