#include "PopupMessage.h"
#include "Windows.h"
#include "Resource.h"
#include "MainDialog.h"

static bool g_bStopPopupMessage = false;
static bool g_bPopupMessageIsRunning = false;

HWND	g_hwndPopupDialog;		// The main dialog window handle
HWND	g_hWnd_CancelButton;

#define WIN_WIDTH 400
#define WIN_HEIGHT 150
const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------

void OnCancel()
{
	exit(1); //kill everything
}

//-----------------------------------------------------------------------------------------------------


static HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width = 50)
{
	RECT rt_cb = { x, y, width, 20 };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
#define CAPTION wTextBuffer
#else
#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), CAPTION, 
		WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP, 
		rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
		hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}

//-----------------------------------------------------------------------------------------------------

static HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption, int height = 20)
{
	RECT rt_label = { x, y, width, height };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
#define CAPTION wTextBuffer
#else
#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("STATIC"), CAPTION, 
		WS_CHILD | WS_VISIBLE, 
		rt_label.left, rt_label.top, rt_label.right, rt_label.bottom, 
		hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}

//-----------------------------------------------------------------------------------------------------
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
LRESULT CALLBACK WndProcPopup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:

			switch(wmEvent)
			{
			case BN_CLICKED:
				{
					if(lParam == (LPARAM)g_hWnd_CancelButton)
						OnCancel();

					break;
				}
			}
			break;

		case WM_CREATE:
			{
				CreateLabel(hWnd, 20, 30, 400, "Failed to connect to DCERouter. Retrying, please wait...");
				g_hWnd_CancelButton = CreateButton(hWnd, WIN_WIDTH / 2 - 30, WIN_HEIGHT - 60, "&Cancel", 60);
			}
			break;

		case WM_PAINT:
			{
				RECT rt;
				hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rt);
				COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
				HBRUSH hBrush = CreateSolidBrush(crBkColor);
				SelectObject(hdc, hBrush);
				FillRect(hdc, &rt, hBrush);//(HBRUSH)::GetStockObject(GRAY_BRUSH));
				DeleteObject(hBrush);
				EndPaint(hWnd, &ps);
			}
			break; 

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

//-----------------------------------------------------------------------------------------------------
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
WORD MyRegisterClassPopup(HINSTANCE hInstance, LPTSTR szWindowClass)
{
#ifdef WINCE
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC) WndProcPopup;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ORBITER));
	wc.hCursor			= 0;
	wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);

#else
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProcPopup;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ORBITER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //(LPCTSTR)IDC_ORBITER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
#endif
}

//-----------------------------------------------------------------------------------------------------
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
BOOL InitInstancePopup(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	g_hInst = hInstance;		// Store instance handle in our global variable

	LPTSTR szWindowClass = TEXT("PlutoPopupMessage");
	LPTSTR szTitle = TEXT("Pluto Orbiter");

	MyRegisterClassPopup(hInstance, szWindowClass);

#ifdef WINCE
	hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME | WS_CAPTION | WS_BORDER,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
#endif

	if (!hWnd)
	{	
		return FALSE;
	}

	RECT rc;
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &rc);

	int iWidth = rc.right - rc.left;
	int iHeight = rc.bottom - rc.top;

	rc.left = (iWidth - WIN_WIDTH) / 2;
	rc.right = rc.left + WIN_WIDTH;
	rc.top = (iHeight - WIN_HEIGHT) / 2;
	rc.bottom = rc.top + WIN_HEIGHT;

	MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);

	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hwndPopupDialog = hWnd;

	return TRUE;
}

DWORD WINAPI PopupMessageThread( LPVOID lpParameter)
{
	InitInstancePopup(g_hInst, 1);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) && !g_bStopPopupMessage) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	g_bPopupMessageIsRunning = false;

	return 0L;
}

bool ShowPopup(const char* pTitle, const char *pCaption)
{
	if(g_bPopupMessageIsRunning)
		return false;

	g_bPopupMessageIsRunning = true;

	g_bStopPopupMessage = false;
	::CreateThread(NULL, 0, PopupMessageThread, 0, 0, NULL);	
	return true;
}

bool ChangePopupSettings(const char* pTitle, const char *pCaption)
{
	return true;
}

void ClosePopup()
{
	g_bStopPopupMessage = true;
	PostMessage(g_hwndPopupDialog, WM_CLOSE, 0, 0);
}