#include "OrbiterSDL_Win32.h"
#include "StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "SelfUpdate.h"

#define VK_A 0x41
#define VK_C 0x43
#define VK_E 0x45
#define VK_T 0x54
#define VK_P 0x50
#define VK_L 0x4C
#define VK_R 0x52
#define VK_Z 0x5A

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}

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
OrbiterSDL_Win32::OrbiterSDL_Win32(int DeviceID, string ServerAddress, string sLocalDirectory, 
								   bool bLocalMode, int nImageWidth, int nImageHeight, 
								   bool bFullScreen) :
	OrbiterSDL(DeviceID, ServerAddress, 
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

    m_bShiftDown = false;
	m_bControlDown = false;
	m_bAltDown = false;
	m_bRepeat = false;
	m_bCapsLock = false;
    m_cKeyDown=0;
	m_bConnectionLost = false;

	/*
	if(m_bFullScreen)
	{
		DEVMODE dmSettings;                          // Device Mode variable - Needed to change modes
		memset(&dmSettings,0,sizeof(dmSettings));    // Makes Sure Memory's Cleared

		// Get current settings -- This function fills our the settings
		// This makes sure NT and Win98 machines change correctly
		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
		{
			return;
		}

		dmSettings.dmPelsWidth = 800;                        // Set the desired Screen Width
		dmSettings.dmPelsHeight = 600;                      // Set the desired Screen Height
		dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;    // Set the flags saying we're changing the Screen Width and Height

		// This function actually changes the screen to full screen
		// CDS_FULLSCREEN Gets Rid Of Start Bar.
		// We always want to get a result from this function to check if we failed
		int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN); 
		// Check if we didn't recieved a good return message From the function
		if(result != DISP_CHANGE_SUCCESSFUL)
		{
			// Display the error message and quit the program
			PostQuitMessage(0);
		}

		SetWindowPos(hSDLWindow, HWND_TOPMOST, 0, 0, 800, 600, SWP_SHOWWINDOW);
		SetForegroundWindow(hSDLWindow);
	}
	*/
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32::~OrbiterSDL_Win32()
{
	//restore old sdl windowproc
	::SetWindowLong(hSDLWindow, GWL_WNDPROC, reinterpret_cast<long>(OldSDLWindowProc));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_Win32::BuildOrbiterSDL_Win32(
	int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen
)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32 constructor.");
		m_pInstance = new OrbiterSDL_Win32(DeviceID, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen);
	}
	else
	{
		throw "OrbiterSDL_Win32 already created!";
	}
	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler;
	g_pCommand_Impl=m_pInstance;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_Win32::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "OrbiterSDL_Win32: need to cleanup orbiter...");
	if(NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;

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
	static WPARAM wOldParam = 0;
	static bool bLastEvent_KeyDown = false;

	if(uMsg == WM_KEYDOWN && bLastEvent_KeyDown && wOldParam == wParam) //this is a repeated key
		return;

	wOldParam = wParam;
	bLastEvent_KeyDown = uMsg == WM_KEYDOWN; //false if WM_KEYUP

    if (uMsg == WM_KEYDOWN)
    {
		if ( wParam == 81)
		{
			OnQuit();
			return;
		}

        g_pPlutoLogger->Write(LV_STATUS, "Key pressed event");
		switch (wParam)
        {
			case VK_SHIFT:
				m_bShiftDown=true;
				break;
			case VK_CONTROL:
				m_bControlDown=true;
				break;
			case VK_MENU:
				m_bAltDown=true;
				break;
			case VK_CAPITAL:
				m_bCapsLock = !m_bCapsLock;
				break;
			default:
				m_cKeyDown = clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
				break;
        }
    }
    else if (uMsg == WM_KEYUP)
    {
		RecordKeyboardAction(long(wParam));

        bool bHandled=false;
clock_t ccc=clock();
        m_bRepeat = m_cKeyDown && clock() - m_cKeyDown > CLOCKS_PER_SEC/2;
        m_cKeyDown=0;

        g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) wParam, (int) m_bRepeat, (int) m_bShiftDown);

#ifndef PHONEKEYS
        if(wParam >= VK_A && wParam <= VK_Z) // A-Z keys
        {
            if((!m_bCapsLock && !m_bShiftDown) || (m_bCapsLock && m_bShiftDown))
                bHandled = ButtonDown(BUTTON_a_CONST + wParam - VK_A);
            else
                bHandled = ButtonDown(BUTTON_A_CONST + wParam - VK_Z);
        }
        else
#endif 
        if( wParam == VK_SHIFT)
            m_bShiftDown=false;
        else if( wParam == VK_CONTROL)
            m_bControlDown=false;
        else if( wParam == VK_MENU )
            m_bAltDown=false;
		else if( wParam == VK_ESCAPE && m_bControlDown && m_bAltDown)
		{
			OnQuit();
		}
		else if( wParam == VK_F10)
		{
			ShowMainDialog();
		}
        else if( !m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=ButtonDown(BUTTON_0_CONST);
					break;
				case '1':
					bHandled=ButtonDown(BUTTON_1_CONST);
					break;
				case '2':
					bHandled=ButtonDown(BUTTON_2_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_3_CONST);
					break;
				case '4':
					bHandled=ButtonDown(BUTTON_4_CONST);
					break;
				case '5':
					bHandled=ButtonDown(BUTTON_5_CONST);
					break;
				case '6':
					bHandled=ButtonDown(BUTTON_6_CONST);
					break;
				case '7':
					bHandled=ButtonDown(BUTTON_7_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_8_CONST);
					break;
				case '9':
					bHandled=ButtonDown(BUTTON_9_CONST);
					break;

#ifdef PHONEKEYS
                case VK_C:
                    bHandled = ButtonDown(BUTTON_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = ButtonDown(BUTTON_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = ButtonDown(BUTTON_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = ButtonDown(BUTTON_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = ButtonDown(BUTTON_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = ButtonDown(BUTTON_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = ButtonDown(BUTTON_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = ButtonDown(BUTTON_Pound_CONST);
                    break;
#endif 

				case VK_F1:
					bHandled=ButtonDown(BUTTON_F1_CONST);
					break;
				case VK_F2:
					bHandled=ButtonDown(BUTTON_F2_CONST);
					break;
				case VK_F3:
					bHandled=ButtonDown(BUTTON_F3_CONST);
					break;
				case VK_F4:
					bHandled=ButtonDown(BUTTON_F4_CONST);
					break;
				case VK_F5:
					bHandled=ButtonDown(BUTTON_F5_CONST);
					break;
				case VK_UP:
					bHandled=ButtonDown(BUTTON_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=ButtonDown(BUTTON_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=ButtonDown(BUTTON_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=ButtonDown(BUTTON_Right_Arrow_CONST);
					break;
				case VK_RETURN:
					bHandled=ButtonDown(BUTTON_Enter_CONST);
					break;
				case VK_BACK:
					bHandled=ButtonDown(BUTTON_Back_CONST);
					break;
				default:
					g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) wParam);
            };
        } 
        else if( m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case VK_UP:
					bHandled=ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_Pound_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_Asterisk_CONST);
					break;
            };
        }
		else if( m_bShiftDown && m_bControlDown && wParam==VK_F1 )
			GotoScreen(m_sMainMenu);
        else if( m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=ButtonDown(BUTTON_Rept_0_CONST);
					break;
				case '1':
					bHandled=ButtonDown(BUTTON_Rept_1_CONST);
					break;
				case '2':
					bHandled=ButtonDown(BUTTON_Rept_2_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_Rept_3_CONST);
					break;
				case '4':
					bHandled=ButtonDown(BUTTON_Rept_4_CONST);
					break;
				case '5':
					bHandled=ButtonDown(BUTTON_Rept_5_CONST);
					break;
				case '6':
					bHandled=ButtonDown(BUTTON_Rept_6_CONST);
					break;
				case '7':
					bHandled=ButtonDown(BUTTON_Rept_7_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_Rept_8_CONST);
					break;
				case '9':
					bHandled=ButtonDown(BUTTON_Rept_9_CONST);
					break;
#ifdef PHONEKEYS
                case VK_C:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = ButtonDown(BUTTON_Rept_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = ButtonDown(BUTTON_Rept_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = ButtonDown(BUTTON_Rept_Pound_CONST);
                    break;
#endif 
			}
		}
	}
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
