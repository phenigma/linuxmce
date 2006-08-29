#ifndef Gallery_H_
#define Gallery_H_

#include "SDLFrontEnd.h"
#include "GalleryScenario.h"
#include "ExtensionManager.h"

#include <string>
using namespace std;

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
public:
	Gallery(void);
	~Gallery(void);
	void MainLoop(void);
	static Gallery* Instance(void);
private:
public:
	bool Setup(int Width, int Height, string FolderName);
	void CleanUp(void);
	void EvaluateEvent(WM_Event Event);
	void _Sleep(int Miliseconds);
};

#endif
