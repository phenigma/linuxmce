#include "MainDialog.h"
#include "Resource.h"

#define MAX_LOADSTRING 100
#define WIN_WIDTH	   400
#define WIN_HEIGHT	   300
#define MENU_HEIGHT	   25

const MAX_STRING_LEN = 4096;

// Global Variables:
HINSTANCE			g_hInst;				// The current instance
HWND				g_hwndMainDialog;		// The main dialog window handle
HWND				g_hwndCB;				// The command bar handle
HWND				g_hWndList;

static SHACTIVATEINFO s_sai;

//-----------------------------------------------------------------------------------------------------
/*extern*/ CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
DWORD WINAPI OrbiterThread( LPVOID lpParameter)
{
	try
	{
		StartOrbiterCE(
			CmdLineParams.PK_Device, 
			CmdLineParams.sRouter_IP,
			CmdLineParams.sLocalDirectory,
			CmdLineParams.bLocalMode,
			CmdLineParams.Width,
			CmdLineParams.Height,
			CmdLineParams.bFullScreen
		);
	}
	catch(string s)
	{
		WriteStatusOutput(s.c_str());
	}
	catch(const char *s)
	{
		WriteStatusOutput(s);
	}

	return 0L;
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	g_hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_ORBITER, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//If it is already running, then focus on the window
	hWnd = FindWindow(szWindowClass, szTitle);	
	if (hWnd) 
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return 0;
	} 

	MyRegisterClass(hInstance, szWindowClass);
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{	
		return FALSE;
	}

	//When the main window is created using CW_USEDEFAULT the height of the menubar (if one
	// is created is not taken into account). So we resize the window after creating it
	// if a menubar is present
	if (g_hwndCB)
    {
		RECT rc;
        RECT rcMenuBar;

		GetWindowRect(hWnd, &rc);

		int iWidth = rc.right - rc.left;
		int iHeight = rc.bottom - rc.top;
		
		rc.left = (iWidth - WIN_WIDTH) / 2;
		rc.right = rc.left + WIN_WIDTH;
		rc.top = (iHeight - WIN_HEIGHT) / 2;
		rc.bottom = rc.top + WIN_HEIGHT;

        GetWindowRect(g_hwndCB, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
		MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hwndMainDialog = hWnd;

	return TRUE;
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
				case IDM_MAIN_FILE_QUIT:
				case IDOK:
					SendMessage (hWnd, WM_CLOSE, 0, 0);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			{
				g_hwndCB = CreateRpCommandBar(hWnd);
				// Initialize the shell activate info structure
				memset (&s_sai, 0, sizeof (s_sai));
				s_sai.cbSize = sizeof (s_sai);

				// Create a list box
				RECT rt = {2, MENU_HEIGHT + 1, WIN_WIDTH - 4, WIN_HEIGHT - 2 * MENU_HEIGHT - 12 };

				g_hWndList = CreateWindow(TEXT("LISTBOX"), NULL, (WS_CHILD | WS_VISIBLE | LBS_STANDARD) & ~LBS_SORT, 
					rt.left, rt.top, rt.right, rt.bottom, hWnd, NULL, g_hInst, NULL);

				::CreateThread(NULL, 0, OrbiterThread, 0, 0, NULL);
			}
			break;

		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			FillRect(hdc, &rt, (HBRUSH)::GetStockObject(GRAY_BRUSH));
			LoadString(g_hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, 
				DT_SINGLELINE | DT_BOTTOM | DT_RIGHT);
			EndPaint(hWnd, &ps);
			break; 
		case WM_DESTROY:
			CommandBar_Destroy(g_hwndCB);
			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
            // Notify shell of our activate message
			SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
     		break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
     		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateRpCommandBar(HWND hwnd)
{
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	mbi.nToolBarId = IDM_MENU;
	mbi.hInstRes   = g_hInst;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;

	if (!SHCreateMenuBar(&mbi)) 
		return NULL;

	return mbi.hwndMB;
}
//-----------------------------------------------------------------------------------------------------
void WriteStatusOutput(const char* pMessage)
{
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, pMessage, MAX_STRING_LEN);

	::SendMessage(g_hWndList, LB_ADDSTRING, 0L, (LPARAM)wTextBuffer);

	//LB_GETCOUNT
}
//-----------------------------------------------------------------------------------------------------
void ShowMainDialog()
{
	//::SetForegroundWindow(g_hwndMainDialog );
	::BringWindowToTop(g_hwndMainDialog );
}
//-----------------------------------------------------------------------------------------------------