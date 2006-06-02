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

	LRESULT Result = pOrbiterRenderer_SDL_Win32->m_pfOldSDLWindowProc(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		pOrbiterRenderer_SDL_Win32->HandleKeyEvents(uMsg, wParam, lParam);
		break;
	}

	return Result;
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL_Win32::OrbiterRenderer_SDL_Win32(Orbiter *pOrbiter) : OrbiterRenderer_SDL(pOrbiter)
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
OrbiterRenderer_SDL_Win32::~OrbiterRenderer_SDL_Win32()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Win32::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Orbiter::Event orbiterEvent;
	orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
	orbiterEvent.data.button.m_iPK_Button = 0;

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