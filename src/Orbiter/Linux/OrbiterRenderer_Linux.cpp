/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "OrbiterLinux.h"

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

#include "OrbiterRenderer_Linux.h"
#include "../ScreenHistory.h"

#ifdef ORBITER_OPENGL
	#define BASE_CLASS OrbiterRenderer_OpenGL
	#include "../OpenGL/OpenGL3DEngine.h"
#else
	#define BASE_CLASS OrbiterRenderer_SDL
#endif

#ifndef USE_SDL_GL_PATCH
	#if !defined(VIA_OVERLAY)
		#include "utilities/linux/CompositeHelper.h"
	#endif
#endif

using namespace DCE;

OrbiterRenderer_Linux::OrbiterRenderer_Linux(Orbiter *pOrbiter) : BASE_CLASS(pOrbiter), 
	m_screenMaskObjects(None), m_screenMaskPopups(None), m_screenMaskCurrent(None), 
	m_bHasPopups(false), m_bScreenRendered(false)
{
}

OrbiterRenderer_Linux::~OrbiterRenderer_Linux()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL)
	{
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskObjects);
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskPopups);
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskCurrent);
	}
}

bool OrbiterRenderer_Linux::HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID)
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		m_bHasPopups = true;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d opaque",
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height);

		//modify mask
		pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
			m_screenMaskPopups,
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height,
			true );
		
		ApplyMasks();
	}

	if (pOrbiterLinux != NULL && pOrbiterLinux->IsYieldInput() && Popup->m_sName == "popup_alert")
	{
		PlutoRectangle rectTotal(0, 0, Popup->m_pObj->m_rPosition.Width, Popup->m_pObj->m_rPosition.Height);
		pOrbiterLinux->ApplyMask(rectTotal, Popup->m_Position, mtShowPopupMask);
	}

	return BASE_CLASS::HandleShowPopup(Popup, Position, EffectID);
}

bool OrbiterRenderer_Linux::HandleHidePopup(PlutoPopup* Popup)
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		m_bHasPopups = true;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d transparent",
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height);

		//modify mask
		pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
			m_screenMaskPopups,
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height,
			false );
		
		ApplyMasks();
	}

	if (pOrbiterLinux != NULL && pOrbiterLinux->IsYieldInput() && Popup->m_sName == "popup_alert")
	{
		PlutoRectangle rectTotal(0, 0, Popup->m_pObj->m_rPosition.Width, Popup->m_pObj->m_rPosition.Height);
		pOrbiterLinux->ApplyMask(rectTotal, Popup->m_Position, mtHidePopupMask);
	}

	return BASE_CLASS::HandleHidePopup(Popup);
}

void OrbiterRenderer_Linux::ObjectRendered(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	BASE_CLASS::ObjectRendered(pObj, point);

	if(point.X != 0 || point.Y != 0)
	{
		//don't handle the objects from popups
		return;
	}

	if(m_bScreenRendered)
	{
		//the screen was rendered; we might want to refresh an object, but we don't need to 
		//modify the mask; only the popups can modify the mask after the screen was rendered
		return;
	}

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		if(pObj->m_vectGraphic.size() > 0 && NULL != pObj->m_vectGraphic[0])
		{
			string sMaskPath = OrbiterLogic()->GetLocalDirectory() + pObj->m_vectGraphic[0]->m_Filename + ".mask.xbm";

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Copy object %s -  position %d,%d,%d,%d / popup offset %d,%d",
				pObj->m_ObjectID.c_str(), pObj->m_rPosition.X, pObj->m_rPosition.Y,
				pObj->m_rPosition.Width, pObj->m_rPosition.Height, point.X, point.Y);

			if(!pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(
				pOrbiterLinux->m_pX11->GetWmWindow(), sMaskPath, m_screenMaskObjects,
				pObj->m_rPosition.X, pObj->m_rPosition.Y))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_PixmapMask_Copy ERROR!");
			}
		}
		else
		{
			if(pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d opaque",
					pObj->m_rPosition.X, pObj->m_rPosition.Y, pObj->m_rPosition.Width, pObj->m_rPosition.Height);

				//modify mask
				pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
					m_screenMaskObjects, pObj->m_rPosition.X, pObj->m_rPosition.Y,
					pObj->m_rPosition.Width, pObj->m_rPosition.Height, true);
			}
		}
	}
}

void OrbiterRenderer_Linux::RenderScreen( bool bRenderGraphicsOnly )
{
	m_bScreenRendered = false;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		if(pOrbiterLinux->m_bUseMask)
		{
			if(m_screenMaskCurrent == None)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Pixmap_Create %d,%d,%d,%d transparent (reset), wmwindow %p",
					0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, pOrbiterLinux->m_pX11->GetWmWindow());

				m_screenMaskObjects = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);
				m_screenMaskPopups = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);	
				m_screenMaskCurrent = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);
			}

			m_bHasPopups = false;

			//reset masks
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d transparent (reset), wmwindow %p",
				0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, pOrbiterLinux->m_pX11->GetWmWindow());
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
				m_screenMaskObjects, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, false);
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
				m_screenMaskPopups, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, false);
		}

		if( bRenderGraphicsOnly )
		{
			BASE_CLASS::RenderScreen( bRenderGraphicsOnly );
	
                        if(pOrbiterLinux->m_bUseMask)
                                ApplyMasks();

	                m_bScreenRendered = true;
			return;
		}		
		
		pOrbiterLinux->StopActivateExternalWindowTask();
		pOrbiterLinux->m_pWinListManager->HideAllWindows();
		pOrbiterLinux->m_bIsExclusiveMode = true; // This is set to false if there's an application desktop

		{
			if(NULL == pOrbiterLinux->GetDisplay())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "The display is not available");
				return;
			}

			X11_Locker lock(pOrbiterLinux->GetDisplay());
			BASE_CLASS::RenderScreen(bRenderGraphicsOnly);

	                if(pOrbiterLinux->m_bUseMask)
        	                ApplyMasks();
		}

		if(pOrbiterLinux->m_bOrbiterReady)
			pOrbiterLinux->m_pWinListManager->ShowSdlWindow(pOrbiterLinux->m_bIsExclusiveMode, pOrbiterLinux->m_bYieldScreen);

		m_bScreenRendered = true;
		pOrbiterLinux->m_pWinListManager->ApplyContext();
	}
}

void OrbiterRenderer_Linux::ApplyMasks()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_Window_Apply!");

		if(m_bHasPopups)
		{
			//mirror mask objects with mask current
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(m_screenMaskObjects, m_screenMaskCurrent, 0, 0, 
				pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 0, 0);
			//merge mask popup with mask current
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(m_screenMaskPopups, m_screenMaskCurrent, 0, 0, 
				pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 0, 0, GXor);

			if( !pOrbiterLinux->m_pX11->Shape_Window_Apply(pOrbiterLinux->m_pX11->GetWmWindow(), m_screenMaskCurrent))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_Window_Apply ERROR!");
			}
		}
		else
		{
			if( !pOrbiterLinux->m_pX11->Shape_Window_Apply(pOrbiterLinux->m_pX11->GetWmWindow(), m_screenMaskObjects) )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_Window_Apply ERROR!");
			}
		}
	}
}

void OrbiterRenderer_Linux::InitializeAfterSetVideoMode()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() START");

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode()");
		pOrbiterLinux->X11_Init();
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : HideOtherWindows");
		pOrbiterLinux->HideOtherWindows();
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : done");
	}
}

void OrbiterRenderer_Linux::InitializeAfterRelatives()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		// allow initial "extern" dialogs to receive clicks until this point
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterRelatives()");
		pOrbiterLinux->GrabPointer(true);
		pOrbiterLinux->GrabKeyboard(true);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterRelatives() : done");
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
        pOrbiterLinux->m_pWinListManager->PositionWindow(pOrbiterLinux->m_pProgressWnd->m_wndName, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight);
		pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(false);
		pOrbiterLinux->m_pWinListManager->ApplyContext();
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
		pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(true);
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
		pOrbiterLinux->m_pWinListManager->ApplyContext();
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
		pOrbiterLinux->m_pWinListManager->PositionWindow(pOrbiterLinux->m_pProgressWnd->m_wndName, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight);
		pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(false);
		pOrbiterLinux->m_pWinListManager->ApplyContext();
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
		pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(true);
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
		pOrbiterLinux->m_pWinListManager->ApplyContext();
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

	pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(false);
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
	pOrbiterLinux->m_pWinListManager->PositionWindow(promptDlg.m_wndName, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight);
	pOrbiterLinux->m_pWinListManager->ApplyContext();
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
	pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(true);
	pOrbiterLinux->m_pWinListManager->ApplyContext();
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

#ifndef MAEMO_NOKIA770
void OrbiterRenderer_Linux::EventLoop()
{
#ifdef USE_SDL_PATCH

	int SDL_Event_Pending = 0;

	SDL_Event Event;
	SDL_EventState(SDL_VIDEOEXPOSE, SDL_ENABLE);
	SDL_EventState(SDL_VIDEORESIZE, SDL_ENABLE);

	// For now I'll assume that shift + arrows scrolls a grid
	while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
	{
		LockDisplay();
		SDL_Event_Pending = SDL_PollEvent(&Event);
		UnlockDisplay();

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Event type %d", Event.type);

			if (Event.type == SDL_QUIT)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			}
#ifdef ORBITER_OPENGL
			else if(Event.type == SDL_VIDEOEXPOSE && Event.type == SDL_VIDEORESIZE)
			{
				Engine->RefreshScreen();
			}
#endif //ORBITER_OPENGL
                        else if(Event.type == SDL_ACTIVEEVENT)
                        {
                            /* See what happened */
                            string sAppState = string("Orbiter app ") + (Event.active.gain ? "gained" : "lost");
                            if ( Event.active.state & SDL_APPACTIVE )
                            {
                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s active state", sAppState.c_str() );
                                if(Event.active.gain)
                                        OrbiterLogic()->CMD_Activate_PC_Desktop(0);
                            }
                        }
		}
		else
		{
			OnIdle();
		}
	}  // while

#else

	Display *dpy = CompositeHelper::GetInstance().GetDisplay();
    if (!dpy) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unable to open display!");
		return;
    }

	XSetWindowAttributes attr;
    XWindowAttributes wattr;

    /* select for all events */
    attr.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask |
			   ButtonReleaseMask | EnterWindowMask |
			   LeaveWindowMask | PointerMotionMask | 
			   Button1MotionMask |
			   Button2MotionMask | Button3MotionMask |
			   Button4MotionMask | Button5MotionMask |
			   ButtonMotionMask | KeymapStateMask |
			   ExposureMask | VisibilityChangeMask | 
			   StructureNotifyMask | /* ResizeRedirectMask | */
			   SubstructureNotifyMask | SubstructureRedirectMask |
			   FocusChangeMask | PropertyChangeMask |
			   ColormapChangeMask | OwnerGrabButtonMask;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux->m_pX11)
	{
		XGetWindowAttributes(dpy, pOrbiterLinux->m_pX11->GetMainWindow(), &wattr);
		if (wattr.all_event_masks & ButtonPressMask)
			attr.event_mask &= ~ButtonPressMask;
		attr.event_mask &= ~SubstructureRedirectMask;
		XSelectInput(dpy, pOrbiterLinux->m_pX11->GetMainWindow(), attr.event_mask);
	}
	
	XEvent report;   // event union reference
	SDL_Event Event;
	int SDL_Event_Pending = 0;

	while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
	{	
		SDL_Event_Pending = SDL_PollEvent(&Event);
		if (SDL_Event_Pending && Event.type == SDL_QUIT)
		{
			//it's all over now...
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
			break;
		}

		// event loop
		XNextEvent(dpy, &report);
		switch (report.type)
		{
			case EnterNotify: 
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "EnterNotify!");
				OrbiterLogic()->CMD_Activate_PC_Desktop(0);
				break;

			case LeaveNotify: 
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "LeaveNotify!");
				break;

			case VisibilityNotify:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "VisibilityNotify!");
				break;

			case FocusIn:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "FocusIn!");
				break;

			case FocusOut:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "FocusOut!");
				break;

			default:
				//LoggerWrapper::GetInstance()->Write(LV_WARNING, "got an event %d -- ignore it!", report.type);
			break;
		}            
	}       

#endif //USE_SDL_PATCH
}
#endif //MAEMO_NOKIA770
