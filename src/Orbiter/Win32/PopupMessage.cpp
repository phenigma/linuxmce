#include "PopupMessage.h"
#include "Windows.h"
#include "Resource.h"
#include "MainDialog.h"
#include "PlutoUtils/StringUtils.h"

HWND	g_hwndPopupDialog;		// The main dialog window handle
static int g_nWindowWidth, g_nWindowHeight;
const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
static int g_nResponse;
static string g_sPrompt;
static map<int, string> *g_p_mapPrompts;
static map<int, HWND> g_mapButtons;

#define MAX_NUMBER_OF_BUTTONS 20
#define WINDOW_TITLE          30
#define BUTTON_WIDTH          100
#define LABEL_HEIGHT          40

#ifdef WINCE
    #define BUTTON_HEIGHT    15
    #define BUTTON_SEPARATOR 5
#else
    #define BUTTON_HEIGHT    20
    #define BUTTON_SEPARATOR 10
#endif
//-----------------------------------------------------------------------------------------------------
static HWND CreateButton(HWND hParentWnd, int x, int y, int width, int height, const char* caption)
{
	RECT rt_cb = { x, y, width, height };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
#define CAPTION wTextBuffer
#else
#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), CAPTION, 
		WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP | BS_MULTILINE | BS_TEXT, 
		rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
		hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
static void AdjustWindowSize()
{
    RECT rc;
    HWND hWndDesktop = ::GetDesktopWindow();
    ::GetWindowRect(hWndDesktop, &rc);

    int iDesktopWidth = rc.right - rc.left;
    int iDesktopHeight = rc.bottom - rc.top;

	if(rc.right < rc.bottom) //this is a PDA for sure
	{
		MoveWindow(g_hwndPopupDialog, 0, 0, g_nWindowWidth, g_nWindowHeight, TRUE);
	}
	else
	{
		rc.left = (iDesktopWidth - g_nWindowWidth) / 2;
		rc.right = rc.left + g_nWindowWidth;
		rc.top = (iDesktopHeight - g_nWindowHeight) / 2;
		rc.bottom = rc.top + g_nWindowHeight;
		MoveWindow(g_hwndPopupDialog, rc.left, rc.top, g_nWindowWidth, g_nWindowHeight, TRUE);
	}
}
//-----------------------------------------------------------------------------------------------------
static HWND CreateLabel(HWND hParentWnd, int x, int y, int width, int height, char* caption)
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
                    map<int, HWND>::iterator it;
                    for(it = g_mapButtons.begin(); it != g_mapButtons.end(); it++)
                    {
                        if((LPARAM)it->second == lParam)
                        {
                            g_nResponse = it->first;
                            PostMessage(g_hwndPopupDialog, WM_CLOSE, 0, 0);
                        }
                    }
					break;
				}
			}
			break;

		case WM_CREATE:
			{
				CreateLabel(hWnd, BUTTON_SEPARATOR, BUTTON_SEPARATOR, 
                    g_nWindowWidth - 2 * BUTTON_SEPARATOR, BUTTON_SEPARATOR + LABEL_HEIGHT, 
                    const_cast<char *>(g_sPrompt.c_str()));

				int nScaleX=0,nScaleY=0;
				if( g_p_mapPrompts )
				{
					if( g_p_mapPrompts->size() )
					{
						nScaleX = 10 / g_p_mapPrompts->size();
						nScaleY = 10 / g_p_mapPrompts->size();
						if( nScaleX>3 )
							nScaleX=3;
						if( nScaleY>2 )
							nScaleY=2;
						if( nScaleX<1 )
							nScaleX=1;
						if( nScaleY<1 )
							nScaleY=1;
					}
					else
						nScaleX=nScaleY=1;
				}

                int nButtonWidth  = nScaleX * BUTTON_WIDTH;
                int nButtonHeight = nScaleY * BUTTON_HEIGHT;

                int nButtonsPerRow = (g_nWindowWidth - BUTTON_SEPARATOR) / (nButtonWidth + BUTTON_SEPARATOR);
                int nButtonIndex = 0;
                int nButtonOnRowIndex = 0;
                int nButtonColumnIndex = 0;
                int nAdjustment = (g_nWindowWidth - (nButtonsPerRow * (nButtonWidth + BUTTON_SEPARATOR) + BUTTON_SEPARATOR)) / 2;

				if( g_p_mapPrompts )
				{
					map<int, string>::iterator it;
					for(it = g_p_mapPrompts->begin(); it != g_p_mapPrompts->end(); it++)
					{
						string sCaption = StringUtils::Replace(it->second, "&", "&&");
						nButtonOnRowIndex = nButtonIndex % nButtonsPerRow;
						nButtonColumnIndex = nButtonIndex / nButtonsPerRow; 

						HWND hWndButton = CreateButton(hWnd, nAdjustment + BUTTON_SEPARATOR + nButtonOnRowIndex * (nButtonWidth + BUTTON_SEPARATOR), 
							BUTTON_SEPARATOR + LABEL_HEIGHT + BUTTON_SEPARATOR + nButtonColumnIndex * (nButtonHeight + BUTTON_SEPARATOR),
							nButtonWidth, nButtonHeight, const_cast<char *>(sCaption.c_str()));

						g_mapButtons[it->first] = hWndButton;
						nButtonIndex++;

						if(nButtonIndex > MAX_NUMBER_OF_BUTTONS)
							break;
					}
				}

                g_nWindowHeight = min(g_nWindowHeight, 
                    WINDOW_TITLE + 
                    BUTTON_SEPARATOR + LABEL_HEIGHT + BUTTON_SEPARATOR +
                    (nButtonColumnIndex + 1) * (nButtonHeight + BUTTON_SEPARATOR));
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
#ifdef WINCE    
    g_nWindowWidth = 240;
    g_nWindowHeight = 280;
#else
    g_nWindowWidth = 400;
    g_nWindowHeight = 400;
#endif

	HWND hWnd = NULL;
	g_hInst = hInstance;		// Store instance handle in our global variable

	LPTSTR szWindowClass = TEXT("PlutoPopupMessage");
	LPTSTR szTitle = TEXT("Pluto Orbiter");

	MyRegisterClassPopup(hInstance, szWindowClass);

#ifdef WINCE
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
#endif

	if (!hWnd)
		return FALSE;

	g_hwndPopupDialog = hWnd;
    AdjustWindowSize();
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int PromptUserEx(string sPrompt,map<int,string> *p_mapPrompts)
{
    g_nResponse = -1;
    g_sPrompt = sPrompt;
    g_p_mapPrompts = p_mapPrompts;

    InitInstancePopup(g_hInst, 1);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_mapButtons.clear();
    return g_nResponse;
}