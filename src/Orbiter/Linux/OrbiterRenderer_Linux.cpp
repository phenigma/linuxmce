#include "OrbiterRenderer_Linux.h"

#include "../Orbiter.h"
#include "OrbiterLinux.h"
using namespace DCE;

#include <stdlib.h>

#include "DCE/Logger.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>
#include "../CallBackTypes.h"
#include "../dialog_types.h"

#ifdef ORBITER_OPENGL
	#define BASE_CLASS OrbiterRenderer_OpenGL
#else
	#define BASE_CLASS OrbiterRenderer_SDL
#endif


OrbiterRenderer_Linux::OrbiterRenderer_Linux(Orbiter *pOrbiter) : BASE_CLASS(pOrbiter)
{

}

OrbiterRenderer_Linux::~OrbiterRenderer_Linux()
{

}

void OrbiterRenderer_Linux::RenderScreen( bool bRenderGraphicsOnly )
{
	if( bRenderGraphicsOnly )
	{
		BASE_CLASS::RenderScreen( bRenderGraphicsOnly );
        RenderScreen_ApplyMask();
		return;
	}

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		pOrbiterLinux->m_pWinListManager->HideAllWindows();
		pOrbiterLinux->m_bIsExclusiveMode = true;

		{
			if(NULL == pOrbiterLinux->GetDisplay())
			{
				g_pPlutoLogger->Write(LV_WARNING, "The display is not available");
				return;
			}

			X11_Locker lock(pOrbiterLinux->GetDisplay());
			BASE_CLASS::RenderScreen(bRenderGraphicsOnly);
			XFlush(pOrbiterLinux->GetDisplay()); // TODO: test and remove this
            RenderScreen_ApplyMask();
		}

		if(pOrbiterLinux->m_bOrbiterReady)
			pOrbiterLinux->m_pWinListManager->ShowSdlWindow(pOrbiterLinux->m_bIsExclusiveMode);

	}
}

bool OrbiterRenderer_Linux::RenderScreen_ApplyMask()
{
    if (true) // TODO: ask about the condition // OrbiterLogic()->DATA_??
        return false;
    //SDL_Surface *pImage = Screen;
    OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "MouseBehavior_Linux::ptrOrbiterLinux() : NULL dynamic_cast<OrbiterLinux *>(%p)", m_pOrbiter);
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ptrOrbiterLinux() : start");
    // remembered values
    Pixmap bitmap_mask = 0;
    GC gc;
    Display *pDisplay = NULL;
    // initialize
    Window window = pOrbiterLinux->m_pX11->GetMainWindow();
    unsigned int width = 0;
    unsigned int height = 0;
    pOrbiterLinux->m_pX11->Object_GetGeometry(window, NULL, NULL, &width, &height);
    bool bResult = pOrbiterLinux->m_pX11->Shape_Context_Enter(window, width, height, bitmap_mask, gc, pDisplay);
    if (! bResult)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "MouseBehavior_Linux::ptrOrbiterLinux() : cannot use the shape extension");
        return false;
    }
    // creating the desired shape
    // TODO : replace with the real algorithm, expecting a pre-calculated shape here
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ptrOrbiterLinux() : computing the shape");
    for (unsigned int x=0; x<width; ++x)
        for (unsigned int y=0; y<height; ++y)
        {
            if (x % 100 < y % 100)
                XDrawPoint(pDisplay, bitmap_mask, gc, x, y);
        }
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ptrOrbiterLinux() : computing the shape : done");
    // done
    bResult = pOrbiterLinux->m_pX11->Shape_Context_Leave(window, bitmap_mask, gc, pDisplay);
    if (! bResult)
    {
        g_pPlutoLogger->Write(LV_WARNING, "MouseBehavior_Linux::ptrOrbiterLinux() : cannot set the window shape");
    }
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ptrOrbiterLinux() : done");
    return true;
}

void OrbiterRenderer_Linux::InitializeAfterSetVideoMode()
{
	g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterSetVideoMode() START");

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterSetVideoMode()");
		pOrbiterLinux->X11_Init();
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : HideOtherWindows");
		pOrbiterLinux->HideOtherWindows();
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : done");
	}
}

void OrbiterRenderer_Linux::InitializeAfterRelatives()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		// allow initial "extern" dialogs to receive clicks until this point
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives()");
		pOrbiterLinux->GrabPointer(true);
		pOrbiterLinux->GrabKeyboard(true);
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives() : done");
	}
}

bool OrbiterRenderer_Linux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

    sMessage += ": ";
    for(map<string, bool>::const_iterator it = mapChildDevices.begin(); it != mapChildDevices.end(); ++it)
        if(!it->second)
            sMessage += StringUtils::Replace(it->first, "|", "# ") + ", ";
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;
    if (pOrbiterLinux->m_pProgressWnd && pOrbiterLinux->m_pProgressWnd->IsCancelled())
    {
        delete pOrbiterLinux->m_pProgressWnd;
        pOrbiterLinux->m_pProgressWnd = NULL;
        return true;
    }
    if (nProgress != -1 && !pOrbiterLinux->m_pProgressWnd)
    {
        // Create the progress window ...
        pOrbiterLinux->m_pProgressWnd = new XProgressWnd();
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->Run();
        pOrbiterLinux->m_pWinListManager->MaximizeWindow(pOrbiterLinux->m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(pOrbiterLinux->m_pProgressWnd)
    {
        // We are done here ...
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
        return false;
    }

    return false;
}

bool OrbiterRenderer_Linux::DisplayProgress(string sMessage, int nProgress)
{
    std::cout << "== DisplayProgress waitlist( " << sMessage << ", " << nProgress << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

    if (pOrbiterLinux->m_pProgressWnd && pOrbiterLinux->m_pProgressWnd->IsCancelled())
    {
        delete pOrbiterLinux->m_pProgressWnd;
        pOrbiterLinux->m_pProgressWnd = NULL;
        return true;
    }
	if (nProgress != -1 && !pOrbiterLinux->m_pProgressWnd)
    {
        // Create the progress window ...
        pOrbiterLinux->m_pProgressWnd = new XProgressWnd();
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->Run();
        pOrbiterLinux->m_pWinListManager->MaximizeWindow(pOrbiterLinux->m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(pOrbiterLinux->m_pProgressWnd)
    {
        // We are done here ...
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
        return false;
    }

    return false;
}

int OrbiterRenderer_Linux::PromptUser(string sPrompt, int iTimeoutSeconds, map<int,string> *p_mapPrompts)
{
    map<int,string> mapPrompts;
    mapPrompts[PROMPT_CANCEL]    = "Ok";
    if (p_mapPrompts == NULL) {
        p_mapPrompts = &mapPrompts;
    }
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
    pOrbiterLinux->m_pWinListManager->MaximizeWindow(promptDlg.m_wndName);
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
    return nUserAnswer;
}

void OrbiterRenderer_Linux::LockDisplay()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_LockDisplay();
}

void OrbiterRenderer_Linux::UnlockDisplay()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_UnlockDisplay();
}

void OrbiterRenderer_Linux::EventLoop()
{
	int SDL_Event_Pending = 0;

	SDL_Event Event;

	// For now I'll assume that shift + arrows scrolls a grid
	while (!OrbiterLogic()->m_bQuit && !OrbiterLogic()->m_bReload)
	{
		LockDisplay();
		SDL_Event_Pending = SDL_PollEvent(&Event);
		UnlockDisplay();

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			}
		}
		else
		{
			OnIdle();
		}
	}  // while
}
