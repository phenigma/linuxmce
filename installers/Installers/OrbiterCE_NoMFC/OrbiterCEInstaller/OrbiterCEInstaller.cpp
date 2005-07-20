// OrbiterCEInstaller.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OrbiterCEInstaller.h"
#include <commctrl.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND				hwndCB;			// The command bar handle
HWND				g_wndEdit;
HWND				g_wndStatus;
HWND				g_wndInstall;
HWND				g_wndServerEdit;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);

#define MAX_STRING_LEN 1024
#define WND_WIDTH 240
#define WND_HEIGHT 150

#include "Wininet.h"
#pragma comment(lib, "Wininet.lib")

HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width=50)
{
	RECT rt_cb = { x, y, width, 20 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption)
{
	RECT rt_label = { x, y, width, 18 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("STATIC"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE, 
			rt_label.left, rt_label.top, rt_label.right, rt_label.bottom, 
			hParentWnd, NULL, hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateEdit(HWND hParentWnd, int x, int y,  int width, char* caption, bool bNumber, bool bAlignRight)
{
	RECT rt_edit = { x, y, width, 18 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(
			TEXT("EDIT"), wTextBuffer, 
			ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER |
				(bAlignRight ? ES_RIGHT : ES_LEFT) |
				(bNumber ? ES_NUMBER : 0), 
			rt_edit.left, rt_edit.top, rt_edit.right, rt_edit.bottom, 
			hParentWnd, NULL, hInst, NULL
		);

	return hWndList;
}


bool DownloadFile(wchar_t *pUrl, wchar_t *pDestFile)
{
	bool bResult = false;
	HINTERNET hInternet = InternetOpen(TEXT("Pluto OrbiterCE Installer"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if(hInternet)
	{
		HINTERNET hFile = InternetOpenUrl(hInternet, pUrl,
			NULL, 0, 0, 0);
			
		if(hFile)
		{
			CHAR buffer[1024];
			::ZeroMemory(buffer, sizeof(buffer));
			DWORD dwRead;

			FILE *file = _wfopen(pDestFile, TEXT("wb"));
			while(InternetReadFile(hFile, buffer, 1023, &dwRead))
			{
				if(!dwRead)
					break;

				buffer[dwRead] = 0;

				fwrite(buffer, sizeof(CHAR), dwRead, file);
			}
			fclose(file);
			bResult = true;
		}

		InternetCloseHandle(hFile);
	}
	
	InternetCloseHandle(hInternet);
	return bResult;
}

bool DownloadFileHelper(wchar_t *wcPath, wchar_t *wcServername, wchar_t *wcUrl, wchar_t *wcFile)
{
	wchar_t DestFileW[MAX_STRING_LEN];
	wchar_t StatusW[MAX_STRING_LEN];
	wchar_t FullUrlW[MAX_STRING_LEN];

	wcscpy(FullUrlW, TEXT("http://"));
	wcscat(FullUrlW, wcServername);
	wcscat(FullUrlW, wcUrl);

	wcscpy(DestFileW, wcPath);
	wcscat(DestFileW, TEXT("/"));
	wcscat(DestFileW, wcFile);

	wcscpy(StatusW, TEXT("Downloading file: "));
	wcscat(StatusW, DestFileW);

	::SetWindowText(g_wndStatus, StatusW);
	::UpdateWindow(g_wndStatus);
	if(!DownloadFile(FullUrlW, DestFileW))
	{
		wcscpy(StatusW, TEXT("Failed to download file: "));
		wcscat(StatusW, DestFileW);

		::SetWindowText(g_wndStatus, StatusW);
		::UpdateWindow(g_wndStatus);
		return false;
	}

	return true;
}

void OnInstall()
{
	wchar_t wcPath[MAX_STRING_LEN];
	GetWindowText(g_wndEdit, wcPath, MAX_STRING_LEN);

	wchar_t wcServername[MAX_STRING_LEN];
	GetWindowText(g_wndServerEdit, wcServername, MAX_STRING_LEN);

	bool bResult = true;

	if(bResult)
		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=orbiter/binaries/AYGSHELL.DLL"), TEXT("AYGSHELL.DLL"));

	if(bResult)
		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=orbiter/binaries/PthreadsCE.dll"), TEXT("PthreadsCE.dll"));

//	if(bResult)
//		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=orbiter/binaries/SDL.dll"), TEXT("SDL.dll"));

	if(bResult)
		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=orbiter/binaries/Orbiter.MD5"), TEXT("Orbiter.MD5"));

	if(bResult)
		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=bin/OrbiterCE.exe"), TEXT("OrbiterCE.exe"));

	if(bResult)
		bResult = DownloadFileHelper(wcPath, wcServername, TEXT("/pluto-admin/fdownload.php?filepath=orbiter/binaries/logo.gif"), TEXT("logo.gif"));

	if(bResult)
	{
		::SetWindowText(g_wndStatus, TEXT("Installation completed successfully!"));
		::UpdateWindow(g_wndStatus);
	}
}


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

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ORBITERCEINSTALLER);

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
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ORBITERCEINSTALLER));
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
	LoadString(hInstance, IDC_ORBITERCEINSTALLER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);

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

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch(wmEvent)
			{
			case BN_CLICKED:
				{
					if(lParam == (LPARAM)g_wndInstall)
						OnInstall();

					break;
				}
			}


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

			RECT rt;
			GetClientRect(hWnd, &rt);

			CreateLabel(hWnd, 10, 30, WND_WIDTH - 80, "Core IP address or name:");	
			g_wndServerEdit = CreateEdit(hWnd, 10, 50,  WND_WIDTH - 80, "dcerouter", false, false);

			CreateLabel(hWnd, 10, 75, WND_WIDTH - 80, "Destination folder:");	
			g_wndEdit = CreateEdit(hWnd, 10, 95,  WND_WIDTH - 80, "/Storage Card", false, false);

			g_wndStatus = CreateLabel(hWnd, 0, WND_HEIGHT - 18, WND_WIDTH, "Please enter the path where you would like to install Orbiter.");	
			g_wndInstall = CreateButton(hWnd, WND_WIDTH - 65, 95, "Install");

			break;
		case WM_PAINT:
			{
				RECT rt;
				GetClientRect(hWnd, &rt);
				hdc = BeginPaint(hWnd, &ps);
				FillRect(hdc, &rt, (HBRUSH)::GetStockObject(GRAY_BRUSH));
				EndPaint(hWnd, &ps);
			}
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
