// Orbiter.cpp : Defines the entry point for the application.
//

#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "Orbiter/SDL/OrbiterSDL.h"

#include "stdafx.h"
#include "OrbiterCENet.h"
#include <commctrl.h>
#include <string>

using namespace DCE;
using namespace std;

#include "Orbiter.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#define  VERSION "<=version=>"

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE; 

#define MAX_LOADSTRING 100

void StartOrbiter(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,int Width,int Height)
{
	OrbiterSDL *pCLinux =
        new OrbiterSDL(
        PK_Device, sRouter_IP,
        sLocalDirectory, bLocalMode, Width, Height);

    if (bLocalMode || pCLinux->Connect())
    {
        g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
        pCLinux->Initialize(gtSDLGraphic);

        if (!bLocalMode)
            pCLinux->CreateChildren();

        //pCLinux->Initialize_Display();

        g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");
        SDL_Event Event;

        // temporary hack --
        // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
        // For now I'll assume that shift + arrows scrolls a grid
        bool bShiftDown=false,bControlDown=false,bAltDown=false,bRepeat=false,bCapsLock=false;
        clock_t cKeyDown=0;
        while (true)
        {
//g_pPlutoLogger->Write(LV_STATUS,"Before wait for event");
            SDL_WaitEvent(&Event);
//g_pPlutoLogger->Write(LV_STATUS,"wait for event returned %d",Event.type);

            if (Event.type == SDL_QUIT)
                break;
#ifdef AUDIDEMO
            if (Event.type == SDL_MOUSEBUTTONDOWN)
            {
                g_pPlutoLogger->Write(LV_WARNING, "================================= Mouse button pressed %d", Event.button.button);
                if( Event.button.button==4 )
                    pCLinux->ButtonDown(BUTTON_Up_Arrow_CONST);
                else if( Event.button.button==5 )
                    pCLinux->ButtonDown(BUTTON_Down_Arrow_CONST);
                else if( Event.button.button==2 )
                    pCLinux->ButtonDown(BUTTON_Enter_CONST);
                else if( Event.button.button==1 )
                    pCLinux->ButtonDown(BUTTON_4_CONST);
                else if( Event.button.button==3 )
                    pCLinux->ButtonDown(BUTTON_5_CONST);
                else if( Event.button.button==7 )
                    pCLinux->ButtonDown(BUTTON_1_CONST);
                else if( Event.button.button==6 )
                    pCLinux->ButtonDown(BUTTON_2_CONST);
                else
                    g_pPlutoLogger->Write(LV_WARNING, "========================================== Mouse button not handled!");
            }
#else
            if (Event.type == SDL_MOUSEBUTTONDOWN)
                pCLinux->RegionDown(Event.button.x, Event.button.y);
#endif
            else if (Event.type == SDL_MOUSEMOTION)
                int k=2; //pCLinux->RegionDown(Event.button.x,Event.button.y);
            else if (Event.type == SDL_KEYDOWN)
            {
                g_pPlutoLogger->Write(LV_STATUS, "Key pressed event");
				switch (Event.key.keysym.sym)
                {
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    bShiftDown=true;
                    break;
                case SDLK_LCTRL:
                case SDLK_RCTRL:
                    bControlDown=true;
                    break;
                case SDLK_LALT:
                case SDLK_RALT:
                    bAltDown=true;
                    break;
                case SDLK_CAPSLOCK:
                    bCapsLock = !bCapsLock;
                    break;
                default:
                    cKeyDown=clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
                    break;
                }
            }
            else if (Event.type == SDL_KEYUP)
            {
                bool bHandled=false;
                bRepeat = cKeyDown && clock()-cKeyDown > CLOCKS_PER_SEC/2;
                cKeyDown=0;

                g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) Event.key.keysym.sym, (int) bRepeat, (int) bShiftDown);
#ifndef PHONEKEYS
                if(Event.key.keysym.sym >= SDLK_a && Event.key.keysym.sym <= SDLK_z)
                {
                    if((!bCapsLock && !bShiftDown) || (bCapsLock && bShiftDown))
                        bHandled = pCLinux->ButtonDown(BUTTON_a_CONST + Event.key.keysym.sym - SDLK_a);
                    else
                        bHandled = pCLinux->ButtonDown(BUTTON_A_CONST + Event.key.keysym.sym - SDLK_a);
                }
                else
#endif
                if( Event.key.keysym.sym==SDLK_LSHIFT || Event.key.keysym.sym==SDLK_RSHIFT )
                    bShiftDown=false;

                else if( Event.key.keysym.sym==SDLK_LCTRL || Event.key.keysym.sym==SDLK_RCTRL )
                    bControlDown=false;
                else if( Event.key.keysym.sym==SDLK_LALT || Event.key.keysym.sym==SDLK_RALT )
                    bAltDown=false;
                else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_0:
                    case SDLK_KP0:
                        bHandled=pCLinux->ButtonDown(BUTTON_0_CONST);
                        break;
                    case SDLK_1:
                    case SDLK_KP1:
                        bHandled=pCLinux->ButtonDown(BUTTON_1_CONST);
                        break;
                    case SDLK_2:
                    case SDLK_KP2:
                        bHandled=pCLinux->ButtonDown(BUTTON_2_CONST);
                        break;
                    case SDLK_3:
                    case SDLK_KP3:
                        bHandled=pCLinux->ButtonDown(BUTTON_3_CONST);
                        break;
                    case SDLK_4:
                    case SDLK_KP4:
                        bHandled=pCLinux->ButtonDown(BUTTON_4_CONST);
                        break;
                    case SDLK_5:
                    case SDLK_KP5:
                        bHandled=pCLinux->ButtonDown(BUTTON_5_CONST);
                        break;
                    case SDLK_6:
                    case SDLK_KP6:
                        bHandled=pCLinux->ButtonDown(BUTTON_6_CONST);
                        break;
                    case SDLK_7:
                    case SDLK_KP7:
                        bHandled=pCLinux->ButtonDown(BUTTON_7_CONST);
                        break;
                    case SDLK_8:
                    case SDLK_KP8:
                        bHandled=pCLinux->ButtonDown(BUTTON_8_CONST);
                        break;
                    case SDLK_9:
                    case SDLK_KP9:
                        bHandled=pCLinux->ButtonDown(BUTTON_9_CONST);
                        break;

#ifdef PHONEKEYS
                    case SDLK_c:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_C_CONST);
                        break;
                    case SDLK_p:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Pencil_CONST);
                        break;
                    case SDLK_t:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Talk_CONST);
                        break;
                    case SDLK_e:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_End_CONST);
                        break;
                    case SDLK_l:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Soft_left_CONST);
                        break;
                    case SDLK_r:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Soft_right_CONST);
                        break;
                    case SDLK_ASTERISK:
                        bHandled=pCLinux->ButtonDown(BUTTON_Asterisk_CONST);
                        break;
                    case SDLK_HASH:
                        bHandled=pCLinux->ButtonDown(BUTTON_Pound_CONST);
                        break;
#endif
                    case SDLK_F1:
                        bHandled=pCLinux->ButtonDown(BUTTON_F1_CONST);
                        break;
                    case SDLK_F2:
                        bHandled=pCLinux->ButtonDown(BUTTON_F2_CONST);
                        break;
                    case SDLK_F3:
                        bHandled=pCLinux->ButtonDown(BUTTON_F3_CONST);
                        break;
                    case SDLK_F4:
                        bHandled=pCLinux->ButtonDown(BUTTON_F4_CONST);
                        break;
                    case SDLK_F5:
                        bHandled=pCLinux->ButtonDown(BUTTON_F5_CONST);
                        break;

                    case SDLK_UP:
                        bHandled=pCLinux->ButtonDown(BUTTON_Up_Arrow_CONST);
                        break;
                    case SDLK_DOWN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Down_Arrow_CONST);
                        break;
                    case SDLK_LEFT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Left_Arrow_CONST);
                        break;
                    case SDLK_RIGHT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Right_Arrow_CONST);
                        break;
                    case SDLK_KP_ENTER:
                    case SDLK_RETURN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Enter_CONST);
                        break;
                    case SDLK_BACKSPACE:
                        bHandled=pCLinux->ButtonDown(BUTTON_Back_CONST);
                        break;
                    default:
                        g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) Event.key.keysym.sym);
                        continue;
                    };
                } // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                else if( bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
                        break;
                    case SDLK_DOWN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
                        break;
                    case SDLK_LEFT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
                        break;
                    case SDLK_RIGHT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
                        break;
/*
                    case SDLK_0:
                        bHandled=pCLinux->ButtonDown(BUTTON_0_CONST);
                        break;
                    case SDLK_1:
                        bHandled=pCLinux->ButtonDown(BUTTON_1_CONST);
                        break;
                    case SDLK_2:
                        bHandled=pCLinux->ButtonDown(BUTTON_2_CONST);
                        break;
*/
                    case SDLK_3:
                        bHandled=pCLinux->ButtonDown(BUTTON_Pound_CONST);
                        break;
/*
                    case SDLK_4:
                        bHandled=pCLinux->ButtonDown(BUTTON_4_CONST);
                        break;
                    case SDLK_5:
                        bHandled=pCLinux->ButtonDown(BUTTON_5_CONST);
                        break;
                    case SDLK_6:
                        bHandled=pCLinux->ButtonDown(BUTTON_6_CONST);
                        break;
                    case SDLK_7:
                        bHandled=pCLinux->ButtonDown(BUTTON_7_CONST);
                        break;
*/
                    case SDLK_8:
                        bHandled=pCLinux->ButtonDown(BUTTON_Asterisk_CONST);
                        break;
/*
                    case SDLK_9:
                        bHandled=pCLinux->ButtonDown(BUTTON_9_CONST);
                        break;
*/
                    };
                }
                else if( bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_0:
                    case SDLK_KP0:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_0_CONST);
                        break;
                    case SDLK_1:
                    case SDLK_KP1:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_1_CONST);
                        break;
                    case SDLK_2:
                    case SDLK_KP2:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_2_CONST);
                        break;
                    case SDLK_3:
                    case SDLK_KP3:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_3_CONST);
                        break;
                    case SDLK_4:
                    case SDLK_KP4:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_4_CONST);
                        break;
                    case SDLK_5:
                    case SDLK_KP5:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_5_CONST);
                        break;
                    case SDLK_6:
                    case SDLK_KP6:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_6_CONST);
                        break;
                    case SDLK_7:
                    case SDLK_KP7:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_7_CONST);
                        break;
                    case SDLK_8:
                    case SDLK_KP8:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_8_CONST);
                        break;
                    case SDLK_9:
                    case SDLK_KP9:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_9_CONST);
                        break;

#ifdef PHONEKEYS
                    case SDLK_c:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_C_CONST);
                        break;
                    case SDLK_p:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Pencil_CONST);
                        break;
                    case SDLK_t:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Talk_CONST);
                        break;
                    case SDLK_e:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_End_CONST);
                        break;
                    case SDLK_l:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Soft_left_CONST);
                        break;
                    case SDLK_r:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Soft_right_CONST);
                        break;
                    case SDLK_ASTERISK:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Asterisk_CONST);
                        break;
                    case SDLK_HASH:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Pound_CONST);
                        break;
#endif
                    }
                }
            }
        }  // while
    } // if connect
    delete pCLinux; 
}






// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND				hwndCB;			// The command bar handle

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ORBITER);

	g_pPlutoLogger = new FileLogger("output.txt"); 
	g_pPlutoLogger->Write(LV_STATUS, "Starting logger...");

	StartOrbiter(138, "10.0.0.79", "/", false, 800, 600);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ORBITER));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_ORBITER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{	
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	if (hwndCB)
		CommandBar_Show(hwndCB, TRUE);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_HELP_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_FILE_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			hwndCB = CommandBar_Create(hInst, hWnd, 1);			
			CommandBar_InsertMenubar(hwndCB, hInst, IDM_MENU, 0);
			CommandBar_AddAdornments(hwndCB, 0, 0);
			break;
		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, 
				DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			CommandBar_Destroy(hwndCB);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;

	switch (message)
	{
		case WM_INITDIALOG:
			// trying to center the About dialog
			if (GetWindowRect(hDlg, &rt1)) {
				GetClientRect(GetParent(hDlg), &rt);
				DlgWidth	= rt1.right - rt1.left;
				DlgHeight	= rt1.bottom - rt1.top ;
				NewPosX		= (rt.right - rt.left - DlgWidth)/2;
				NewPosY		= (rt.bottom - rt.top - DlgHeight)/2;
				
				// if the About box is larger than the physical screen 
				if (NewPosX < 0) NewPosX = 0;
				if (NewPosY < 0) NewPosY = 0;
				SetWindowPos(hDlg, 0, NewPosX, NewPosY,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			return TRUE;

		case WM_COMMAND:
			if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
