// Orbiter.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <commctrl.h>

#include "main.h"
#include "StartOrbiterCE.h"
#include "DCE/Logger.h"

#define  VERSION "<=version=>"

namespace DCE
{
	Logger *g_pPlutoLogger;
}

using namespace DCE; 

#define MAX_LOADSTRING 100


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

    string sRouter_IP="dce_router";
    int PK_Device=0;
    string sLogger="orbiter.txt";
    int Width=800,Height=600;
    bool bLocalMode=false; // If true, it will not connect to PlutoServer but will look for it's files in the local directory
	bool bFullScreen=false;
    string sLocalDirectory="";
    string sNestedDisplay = "";

    bool bError=false; // An error parsing the command line
    char c;

	//parse command line
	string::size_type pos = 0;

	char pCmdLine[256];
	wcstombs(pCmdLine, lpCmdLine, 256);

	//sprintf(pCmdLine, "%S", lpCmdLine);
	string command_line = pCmdLine;
	string token;
	do
	{
		token = StringUtils::Tokenize(command_line, " ", pos);

		if(token != "" && token.size() >= 2)
			switch(token[1])
			{	
				case 'r':
					sRouter_IP = StringUtils::Tokenize(command_line, " ", pos);
					break;
				case 'd':
					PK_Device = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					break;
				case 'l':
					sLogger = StringUtils::Tokenize(command_line, " ", pos);
					break;
				case 'D':
					sLocalDirectory = StringUtils::Tokenize(command_line, " ", pos);
					break;
				case 'L':
					bLocalMode = true;
					break;
				case 'W':
					Width = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					break;
				case 'H':
					Height = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					break;
				case 'F':
					bFullScreen = true;
					break;
				default:
					bError=true;
					break;
		}
	}
	while(token != "");

    try
    {
        if( sLogger=="null" )
            g_pPlutoLogger = new NullLogger();
        else if( sLogger=="stdout" )
            g_pPlutoLogger = new FileLogger(stdout);
        else
            g_pPlutoLogger = new FileLogger(sLogger.c_str());
    }
    catch(...)
    {
        printf("Unable to create logger\n");
		return -1;
    }

    if (bError)
    {
		printf("Orbiter, v. %s\n", VERSION);
		printf("A Pluto DCE Device.  See www.plutohome.com/dce for details.\n");
        printf("Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dce_router|stdout|null|filename]\n"); 
	    printf("[-D Directory] [-L] [-W Width] [-H Height]\n");
        printf("-r router's IP	-- the IP address of the DCE Router  Defaults to 'dce_router'.\n");
        printf("-d my device ID	-- This device's ID number.  If not specified, it will be requested\n");
	    printf("	               from the router based on our IP address.\n");
        printf("-l dce_router	-- Where to save the log files.  Specify 'dce_router' to have \n"); 
	    printf("	               the messages logged to the DCE Router.  Defaults to stdout.\n");
        printf("-D directory	-- If a directory is specified, it will look for it's image \n");
	    printf("	               and config files here rather than requesting from the server.\n");
        printf("-L              -- Local mode only.  Do not connect to the server.  All messages will just loop back.\n");
        printf("-F              -- Full screen.\n");
		printf("-W/H            -- Width/Height default to full screen.\n");

    }
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "Starting logger...");
		g_pPlutoLogger->Write(LV_STATUS, "Device: %d starting",PK_Device);

		if( sLocalDirectory.length()>0 && sLocalDirectory[ sLocalDirectory.length()-1 ]!='/' )
			sLocalDirectory += "/";

		WORD    wVersion;
		WSADATA wsaData;
		wVersion = MAKEWORD( 1, 1 );
		if (WSAStartup(wVersion, &wsaData)!=0)
		{
			int ec = WSAGetLastError();
			char s[91];
			printf("WSAStartup err %d", ec);
			return 0;
		}

		try
		{
			StartOrbiterCE(PK_Device,sRouter_IP,sLocalDirectory,bLocalMode,Width,Height,bFullScreen);
		}
		catch(string s)
		{
			printf("Caught exception: %s\n", s.c_str());
		}
		catch(const char *s)
		{
			printf("Caught exception: %s\n", s);
		}
	}


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    WSACleanup();

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
