#include "OrbiterRenderer_SDL_Win32.h"
#include "../../OrbiterRenderer.h"
#include "../../Orbiter.h"
#include "../../Win32/VirtualKeysTranslator.h"
#include "../../Win32/MainDialog.h"
//-----------------------------------------------------------------------------------------------------
#include "../Win32/MainDialog.h"
#include "../Win32/PopupMessage.h"
#include "../Win32/ProgressDialog.h"
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK SDLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Orbiter *pOrbiter = Orbiter::Instance();

	if(pOrbiter == NULL)
		return 0L;

	OrbiterRenderer_SDL_Win32 *pOrbiterRenderer_SDL_Win32 = dynamic_cast<OrbiterRenderer_SDL_Win32 *>(pOrbiter->Renderer());

	if(pOrbiterRenderer_SDL_Win32 == NULL)
		return 0L;

#ifdef ORBITER_OPENGL
	LRESULT Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
#else
	LRESULT Result = pOrbiterRenderer_SDL_Win32->m_pfOldSDLWindowProc(hWnd, uMsg, wParam, lParam);
#endif

	switch(uMsg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		pOrbiterRenderer_SDL_Win32->HandleKeyEvents(uMsg, wParam, lParam);
		break;

#ifdef ORBITER_OPENGL
	case WM_CLOSE:
		{
			pOrbiterRenderer_SDL_Win32->OrbiterLogic()->OnQuit();
		}
	break;

	case WM_PAINT:
	case WM_MOVE:
		{
			pOrbiterRenderer_SDL_Win32->BeginPaint();
			pOrbiterRenderer_SDL_Win32->EndPaint();
		}
	break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = 
				uMsg == WM_LBUTTONDOWN ? 
					Orbiter::Event::REGION_DOWN :
					uMsg == WM_LBUTTONUP ? 
						Orbiter::Event::REGION_UP : 
						Orbiter::Event::MOUSE_MOVE;

			orbiterEvent.data.region.m_iButton = 1;
			orbiterEvent.data.region.m_iX = (int)(short)LOWORD(lParam);
			orbiterEvent.data.region.m_iY = (int)(short)HIWORD(lParam);
			pOrbiterRenderer_SDL_Win32->OrbiterLogic()->ProcessEvent(orbiterEvent);
		}
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = 
				uMsg == WM_RBUTTONDOWN ? 
					Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;

			orbiterEvent.data.region.m_iButton = 3;
			orbiterEvent.data.region.m_iX = (int)(short)LOWORD(lParam);
			orbiterEvent.data.region.m_iY = (int)(short)HIWORD(lParam);
			pOrbiterRenderer_SDL_Win32->OrbiterLogic()->ProcessEvent(orbiterEvent);
		}
		break;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = 
				uMsg == WM_MBUTTONDOWN ? 
					Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;

			orbiterEvent.data.region.m_iButton = 2;
			orbiterEvent.data.region.m_iX = (int)(short)LOWORD(lParam);
			orbiterEvent.data.region.m_iY = (int)(short)HIWORD(lParam);
			pOrbiterRenderer_SDL_Win32->OrbiterLogic()->ProcessEvent(orbiterEvent);
		}
		break;
#endif

	default:
		break;
	}

	return Result;
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL_Win32::OrbiterRenderer_SDL_Win32(Orbiter *pOrbiter) : 
#ifdef ORBITER_OPENGL
	OrbiterRenderer_OpenGL(pOrbiter)
#else
	OrbiterRenderer_SDL(pOrbiter)
#endif
	, m_pfOldSDLWindowProc(NULL)
{
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL_Win32::~OrbiterRenderer_SDL_Win32()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Win32::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Orbiter::Event orbiterEvent;
	orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
	orbiterEvent.data.button.m_iPK_Button = 0;
	orbiterEvent.data.button.m_bSimulated = false;

	if(uMsg == WM_KEYDOWN)
	{
		orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Key down %d bControlDown %d bAltDown %d bCapsLock %d bShiftDown %d",
			wParam,(int) OrbiterLogic()->m_bControlDown,(int) OrbiterLogic()->m_bAltDown,(int) OrbiterLogic()->m_bCapsLock,(int) OrbiterLogic()->m_bShiftDown);
#endif
	}
	else if(uMsg == WM_KEYUP)
		orbiterEvent.type = Orbiter::Event::BUTTON_UP;

	if(!TranslateVirtualKeys2PlutoKeys(uMsg, wParam, lParam, orbiterEvent.data.button.m_iPK_Button, 
		OrbiterLogic()->m_bShiftDown, OrbiterLogic()->m_bAltDown, OrbiterLogic()->m_bControlDown, OrbiterLogic()->m_bCapsLock)
		)
		return; //prevent auto-repeted

	//orbiter win xp/ce specifics
	if( wParam == VK_F12 && orbiterEvent.type == Orbiter::Event::BUTTON_UP) 
	{
		OnQuit();
	}
#ifdef WINCE
	else if( wParam == VK_F10) 
#else
	else if( wParam == VK_A && OrbiterLogic()->m_bControlDown)
#endif
	{
		ShowMainDialog();
		OrbiterLogic()->m_bControlDown = false;
	}

	if(wParam == VK_SHIFT)
		OrbiterLogic()->m_bShiftDownOnScreenKeyboard = false;

	RecordKeyboardAction(orbiterEvent.data.button.m_iPK_Button);

	OrbiterLogic()->ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_SDL_Win32::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
	return PromptUserEx(sPrompt, p_mapPrompts);
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_SDL_Win32::DisplayProgress(string sMessage, int nProgress)
{
	return DialogProgressEx(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Win32::SetupWindow()
{
	m_hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);
#pragma warning(disable:4312)
#pragma warning(disable:4244)

	if(NULL != m_hSDLWindow)
	{
		m_pfOldSDLWindowProc = reinterpret_cast<WNDPROC>(
			::SetWindowLong(
				m_hSDLWindow, 
				GWL_WNDPROC, 
				reinterpret_cast<LONG_PTR>(SDLWindowProc)
			)
		);
	}
#pragma warning(default:4312)
#pragma warning(default:4244)
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_SDL_Win32::Configure()
{
#ifdef ORBITER_OPENGL
	OrbiterRenderer_OpenGL::Configure();
#else
	OrbiterRenderer_SDL::Configure();
#endif

#ifndef WINCE	
	if(OrbiterLogic()->m_bFullScreen)
	{
		m_hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);
		if(NULL != m_hSDLWindow)
		{
			int style = GetWindowLong(m_hSDLWindow, GWL_STYLE);
			SetWindowLong(m_hSDLWindow, GWL_STYLE, (style & ~WS_CAPTION));

			HWND hWnd_TaskBar = ::FindWindow("Shell_TrayWnd", NULL);
			if(hWnd_TaskBar)
				::SetWindowPos(hWnd_TaskBar, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); 

			RECT rc;
			HWND hWndDesktop = ::GetDesktopWindow();
			::GetWindowRect(hWndDesktop, &rc);
			::MoveWindow(m_hSDLWindow, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);

			::BringWindowToTop(m_hSDLWindow);
			SetForegroundWindow(m_hSDLWindow);

			RECT r;
			::GetClientRect( m_hSDLWindow, &r );
			g_pPlutoLogger->Write(LV_CRITICAL, "After fullscreen GetClientRect reports: %d, %d, %d, %d",
				r.left, r.top, r.right, r.bottom);
		}
	}
#endif
}
//-----------------------------------------------------------------------------------------------------