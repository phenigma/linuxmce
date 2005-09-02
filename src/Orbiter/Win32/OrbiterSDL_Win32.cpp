#include "OrbiterSDL_Win32.h"
#include "StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"
#include "PopupMessage.h"
#include "ProgressDialog.h"
#include "VirtualKeysTranslator.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "SelfUpdate.h"

const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK SDLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OrbiterSDL_Win32 *pOrbiter = OrbiterSDL_Win32::GetInstance();

	if(pOrbiter == NULL)
		return 0L;

	LRESULT Result = pOrbiter->OldSDLWindowProc(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			pOrbiter->HandleKeyEvents(uMsg, wParam, lParam);
			break;
	}

	return Result;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32 *OrbiterSDL_Win32::m_pInstance = NULL; //the one and only
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32::OrbiterSDL_Win32(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, 
								   bool bLocalMode, int nImageWidth, int nImageHeight, 
								   bool bFullScreen) :
	OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen)
{
	//todo
	hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

	if(NULL != hSDLWindow)
	{
		OldSDLWindowProc = reinterpret_cast<WNDPROC>(::SetWindowLong(hSDLWindow, 
			GWL_WNDPROC, reinterpret_cast<LONG>(SDLWindowProc)));
	}

	m_bConnectionLost = false;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32::~OrbiterSDL_Win32()
{
	//restore old sdl windowproc
	::SetWindowLong(hSDLWindow, GWL_WNDPROC, reinterpret_cast<long>(OldSDLWindowProc));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_Win32::BuildOrbiter(
	int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen
)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32 constructor.");
		m_pInstance = new OrbiterSDL_Win32(DeviceID, PK_DeviceTemplate, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen);
	}
	else
	{
		throw "OrbiterSDL_Win32 already created!";
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_Win32::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32: need to cleanup orbiter...");
	if(NULL != m_pInstance)
	{	
		OrbiterSDL_Win32 *pCopy = m_pInstance;
		m_pInstance = NULL;

		delete pCopy;
		pCopy = NULL;

		g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32: orbiter deleted.");
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ OrbiterSDL_Win32 *OrbiterSDL_Win32::GetInstance()
{
	if(!m_pInstance)
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32:GetInstance The instance to orbiter is NULL");

	return m_pInstance;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::RenderScreen()
{
	OrbiterSDL::RenderScreen();
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
        m_bShiftDown, m_bAltDown, m_bControlDown, m_bCapsLock)
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
    else if( wParam == VK_A && m_bControlDown)
#endif
    {
        ShowMainDialog();
        m_bControlDown = false;
    }

    Orbiter::ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::WriteStatusOutput(const char* pMessage)
{
	RECT rect = { 0, 0, m_iImageWidth, m_iImageHeight };

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

	::DrawText(hDC, MY_TEXT, string(pMessage).length(), &rect, 
		DT_NOPREFIX | DT_VCENTER | DT_CENTER | DT_SINGLELINE); 

	::ReleaseDC(hSDLWindow, hDC);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_Win32::OnQuit()
{
	m_bQuit = true;
	m_bConnectionLost = true;

	//atexit(SDL_Quit);
	//::PostMessage(hSDLWindow, WM_QUIT, 0L, 0L);
	::ShowWindow(hSDLWindow, SW_HIDE);

	SDL_Event event;
	event.type = SDL_MOUSEMOTION;
	SDL_PushEvent(&event); 
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSDL_Win32::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);

	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
int OrbiterSDL_Win32::PromptUser(string sPrompt,map<int,string> *p_mapPrompts)
{
    return PromptUserEx(sPrompt, p_mapPrompts);
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSDL_Win32::DisplayProgress(string sMessage, int nProgress)
{
    return DialogProgressEx(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------