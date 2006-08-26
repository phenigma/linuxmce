#include "Gallery.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "ExtensionManager.h"
#include "MeshPainter.h"

Gallery* Gallery::Instance_ = NULL;

Gallery::Gallery():
	Quit(false)
{
}

Gallery::~Gallery(void)
{
	delete Scenario;
}

void Gallery::MainLoop(void)
{
	while(!Quit)
	{
		while (!Quit && (FrontEnd->HasEventPending()))
		{
			FrontEnd->TranslateEvent(Event);
			if(Event.Type)
				EvaluateEvent(Event);
		}
		bool StatusChange = Event.Type != -1;

		if(!Quit)
		{
			if(StatusChange)
			{
				PaintScreen();
				Sleep(5);
			}
			else
			{
				Sleep(10);
			}
		}
	}
}

Gallery* Gallery::Instance(void)
{
	if(NULL == Instance_)
		Instance_ = new Gallery();
	return Instance_;
}

void Gallery::PaintScreen(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	Scenario->Update();

	FrontEnd->Flip();
}

bool Gallery::Setup(int Width, int Height, string FolderName)
{
	FrontEnd = new SDLFrontEnd();
	bool Result = FrontEnd->StartVideoMode(Width, Height, false) != 0;
	MeshPainter::Instance()->Setup(&Extensions);
	Extensions.Resize(Width, Height);
	Scenario = new GalleryScenario(Width, Height, 400, 1600);
	return Result;
}

void Gallery::CleanUp(void)
{
}

void Gallery::EvaluateEvent(WM_Event Event)
{
	if(Event.Type == WMET_ESCAPE_KEY)
		Quit = true;
}

void Gallery::Sleep(int Miliseconds)
{
#ifdef WIN32
	::Sleep(Miliseconds);
#else
	usleep(Miliseconds*1000);
#endif
}
