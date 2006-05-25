#include "OrbiterSDL_Win32.h"
#include "PlutoUtils/StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"
#include "PopupMessage.h"
#include "ProgressDialog.h"
#include "VirtualKeysTranslator.h"

#ifdef ENABLE_MOUSE_BEHAVIOR
#include "Win32/MouseBehavior_Win32.h"
#endif

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "SelfUpdate.h"

const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK SDLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OrbiterSDL_Win32 *pRenderer = dynamic_cast<OrbiterSDL_Win32 *>(Orbiter::GetInstance()->Renderer());

	if(pRenderer == NULL)
		return 0L;

	LRESULT Result = pRenderer->OldSDLWindowProc(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			pRenderer->HandleKeyEvents(uMsg, wParam, lParam);
			break;
	}

	return Result;
}
//-----------------------------------------------------------------------------------------------------
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32::OrbiterSDL_Win32() : OrbiterSDL()

{
	hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

#ifdef ENABLE_MOUSE_BEHAVIOR
	m_pMouseBehavior = new MouseBehavior_Win32(this);
#endif

	if(NULL != hSDLWindow)
	{
#pragma warning(disable:4244)
#pragma warning(disable:4312)

		OldSDLWindowProc = reinterpret_cast<WNDPROC>(
			::SetWindowLongPtr(
				hSDLWindow, 
				GWL_WNDPROC, 
				reinterpret_cast<LONG_PTR>(SDLWindowProc)
			)
		);

#pragma warning(default:4312)
#pragma warning(default:4244)
	}

	m_bConnectionLost = false;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32::~OrbiterSDL_Win32()
{
#pragma warning(disable:4244)

	//restore old sdl windowproc
	::SetWindowLongPtr(hSDLWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(OldSDLWindowProc));

#pragma warning(default:4244)
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::RenderScreen( bool bRenderGraphicsOnly )
{
#ifdef ENABLE_OPENGL
	OrbiterSDLGL::RenderScreen( bRenderGraphicsOnly );
#else
	OrbiterSDL::RenderScreen( bRenderGraphicsOnly );
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Orbiter::Event orbiterEvent;
    orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
    orbiterEvent.data.button.m_iPK_Button = 0;

    if(uMsg == WM_KEYDOWN)
        orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
    else if(uMsg == WM_KEYUP)
        orbiterEvent.type = Orbiter::Event::BUTTON_UP;

    if(!TranslateVirtualKeys2PlutoKeys(uMsg, wParam, lParam, orbiterEvent.data.button.m_iPK_Button, 
		Orbiter::GetInstance()->m_bShiftDown, Orbiter::GetInstance()->m_bAltDown, 
		Orbiter::GetInstance()->m_bControlDown, Orbiter::GetInstance()->m_bCapsLock)
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
    else if( wParam == VK_A && Orbiter::GetInstance()->m_bControlDown)
#endif
    {
        ShowMainDialog();
        Orbiter::GetInstance()->m_bControlDown = false;
    }

    if(wParam == VK_SHIFT)
        Orbiter::GetInstance()->m_bShiftDownOnScreenKeyboard = false;

    Orbiter::GetInstance()->ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::WriteStatusOutput(const char* pMessage)
{
	RECT rect = { 0, 0, Orbiter::GetInstance()->m_iImageWidth, Orbiter::GetInstance()->m_iImageHeight };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, pMessage, MAX_STRING_LEN);
	#define MY_TEXT wTextBuffer
#else
	#define MY_TEXT pMessage
#endif

	HDC hDC = ::GetDC(hSDLWindow);

	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));

	::SetBkColor(hDC, RGB(0, 0, 0));
	::SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	::SetBkMode(hDC, OPAQUE);	

	::DrawText(hDC, MY_TEXT, int(string(pMessage).length()), &rect, 
		DT_NOPREFIX | DT_VCENTER | DT_CENTER | DT_SINGLELINE); 

	::ReleaseDC(hSDLWindow, hDC);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::OnQuit()
{
	Orbiter::GetInstance()->m_bQuit = true;
	Orbiter::GetInstance()->m_bConnectionLost = true;
	pthread_cond_broadcast( &Orbiter::GetInstance()->m_listMessageQueueCond );

	//atexit(SDL_Quit);
	//::PostMessage(hSDLWindow, WM_QUIT, 0L, 0L);
	//::ShowWindow(hSDLWindow, SW_HIDE);

	SDL_Event event;
	event.type = SDL_MOUSEMOTION;
	SDL_PushEvent(&event); 
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSDL_Win32::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(Orbiter::GetInstance());

	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
int OrbiterSDL_Win32::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
    return PromptUserEx(sPrompt, p_mapPrompts);
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSDL_Win32::DisplayProgress(string sMessage, int nProgress)
{
    return DialogProgressEx(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------