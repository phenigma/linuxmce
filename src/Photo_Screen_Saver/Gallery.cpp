#include "Gallery.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "ExtensionManager.h"
#include "Painter.h"

#include "DCE/Logger.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <fcntl.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <unistd.h>
	#include <sys/wait.h>
#endif


unsigned long ProcessUtils::g_SecondsReset=0;

void ProcessUtils::ResetMsTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	g_SecondsReset = ts.tv_sec;
}

unsigned long ProcessUtils::GetMicroTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	return (ts.tv_sec-g_SecondsReset)*1000000 + (ts.tv_nsec/1000);
}



Gallery* Gallery::Instance_ = NULL;

Gallery::Gallery():
	Quit(false)
{
}

Gallery::~Gallery(void)
{
	delete Scenario;
}

void Gallery::MainLoop(bool * m_bQuit)
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
				_Sleep(35);
			}
			else
			{
				_Sleep(80);
			}
		}
		Quit = Quit || (* m_bQuit);
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
	Utils.ResetMsTime();
#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_WARNING, "StartFlip %d", Utils.GetMicroTime());
#endif
	glClear(GL_COLOR_BUFFER_BIT);

	Scenario->Update();

	FrontEnd->Flip();
#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_WARNING, "EndFlip %d", Utils.GetMicroTime());
#endif
}

bool Gallery::Setup(int Width, int Height, int FaddingTime, int ZoomTime, string FolderName)
{
//	Width = 800; Height = 500;
	Quit = false;
	Event.Type = 0;
	FrontEnd = new SDLFrontEnd();
	bool Result = FrontEnd->StartVideoMode(Width, Height, false) != 0;
	Painter::Instance()->Setup(&Extensions);
	Extensions.Resize(Width, Height);
	Scenario = new GalleryScenario(Width, Height, FaddingTime, ZoomTime, FolderName);
	return Result;
}

void Gallery::CleanUp()
{
	delete FrontEnd;
	FrontEnd = NULL;
	Event.Type = 0;
}

void Gallery::EvaluateEvent(WM_Event Event)
{
	if(Event.Type != 0)
		Quit = true;
}

void Gallery::_Sleep(int Miliseconds)
{
#ifdef WIN32
	::Sleep(Miliseconds);
#else
	usleep(Miliseconds*1000);
#endif
}
