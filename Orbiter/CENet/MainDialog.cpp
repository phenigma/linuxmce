#include "MainDialog.h"
#include "Resource.h"
#include "StringUtils.h"
#include "FileUtils.h"
#include "Commdlg.h"
#include "MultiThreadIncludes.h"

#define MAX_LOADSTRING 100
#define WIN_WIDTH	   600
#define WIN_HEIGHT	   400

#define SDL_WIDTH	   800
#define SDL_HEIGHT	   600

#define MENU_HEIGHT	   25
//-----------------------------------------------------------------------------------------------------
const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
enum PageIndex
{
	piLogger,
	piRecorder,
	piRandom
};
//-----------------------------------------------------------------------------------------------------
DWORD OrbiterThreadId;
DWORD PlayerThreadId;
DWORD GeneratorThreadId;
//-----------------------------------------------------------------------------------------------------
// Global Variables:
HINSTANCE			g_hInst;				// The current instance
HWND				g_hwndMainDialog;		// The main dialog window handle
HWND				g_hwndCB;				// The command bar handle
//-----------------------------------------------------------------------------------------------------
HWND				g_hWndTab;
HWND				g_hWndPage1;
HWND				g_hWndPage2;
HWND				g_hWndPage3;
//-----------------------------------------------------------------------------------------------------
//page 1
HWND				g_hWndList;
//-----------------------------------------------------------------------------------------------------
//page 2
HWND				g_hWndRecord_MouseCheckBox; 
HWND				g_hWndRecord_KeyboardCheckBox;
HWND				g_hWndRecord_List;
HWND				g_hWndRecord_SaveButton;
HWND				g_hWndRecord_LoadButton;
HWND				g_hWndRecord_ClearButton;
HWND				g_hWndRecord_GoButton;
HWND				g_hWndRecord_RepeatEdit;
//-----------------------------------------------------------------------------------------------------
//page 3
HWND				g_hWndRandom_DelayMin;
HWND				g_hWndRandom_DelayMax;

HWND				g_hWndRandom_ButtonsPerClick;

HWND				g_hWndRandom_MouseCheckBox;
HWND				g_hWndRandom_KeyboardCheckBox;

HWND				g_hWndRandom_KeyOption1RadioBox;
HWND				g_hWndRandom_KeyOption2RadioBox;
HWND				g_hWndRandom_KeyOption3RadioBox;

HWND				g_hWndRandom_GenerateButton;
//-----------------------------------------------------------------------------------------------------
static SHACTIVATEINFO s_sai;
//-----------------------------------------------------------------------------------------------------
/*extern*/ CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK PagesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//-----------------------------------------------------------------------------------------------------
HWND CreateRpCommandBar(HWND);
HWND CreateTabControl(HWND hWnd);
HWND CreatePage(HWND hWnd);
HWND CreateListBox(HWND hWnd, RECT rt);			
HWND CreateCheckBox(HWND hParentWnd, int x, int y, const char* caption, int width = 130);
HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width = 50);
HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption);
HWND CreateEdit(HWND hParentWnd, int x, int y,  int width, char* caption, bool bNumber, bool bAlignRight);
HWND CreateRadioBox(HWND hParentWnd, int x, int y, const char* caption, int width = 130);

void SetActivePage(int TabIndex);
void RecordAction(const char* sAction);
void RecordDelay();

void PerformActionDelay(long delay);
void PerformActionKeyPress(long key);
void PerformActionMouseClick(int x, int y);
//-----------------------------------------------------------------------------------------------------
void OnRecord_Load();  
void OnRecord_Save();  
void OnRecord_Clear(); 
void OnRecord_Go();
void OnRandom_Generate();

void GetEditText(HWND hWndEdit, string& Text);
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
DWORD WINAPI PlayerThread( LPVOID lpParameter)
{
	Sleep(1000);

	string sTimesText;
	GetEditText(g_hWndRecord_RepeatEdit, sTimesText);

	int Times = atoi(sTimesText.c_str());

	int Count = ::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);

	while(Times--)
	for(int i = 0; i < Count; i++)
	{
		wchar_t lpszBuffer[256];
		::SendMessage(g_hWndRecord_List, LB_GETTEXT, i, (LPARAM)(LPCTSTR)lpszBuffer);

		char pItemBuffer[MAX_STRING_LEN];
		wcstombs(pItemBuffer, lpszBuffer, MAX_STRING_LEN);

		string sItemBuffer = pItemBuffer;

		string::size_type CurPos = 0;
		string sAction = StringUtils::Tokenize(sItemBuffer, " ", CurPos);

		if(sAction == "delay")
		{
			string sDelay = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
			long delay = atoi(sDelay.c_str());

			PerformActionDelay(delay);
		}
		else
			if(sAction == "button")
			{
				string sKey = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
				long key = atoi(sKey.c_str());

				PerformActionKeyPress(key);
			}
			else
			{
				string sClickX = StringUtils::Tokenize(sItemBuffer, ",", CurPos);
				int x = atoi(sClickX.c_str());

				CurPos++; //skip space too

				string sClickY = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
				int y = atoi(sClickY.c_str());

				PerformActionMouseClick(x, y);
			}
	}

	return 0L;
}
//-----------------------------------------------------------------------------------------------------
DWORD WINAPI GeneratorThread( LPVOID lpParameter)
{
	bool bGenerateMouseClicks = BST_CHECKED == ::SendMessage(g_hWndRandom_MouseCheckBox, BM_GETCHECK, 0, 0);
	bool bGenerateKeyboardEvents = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyboardCheckBox, BM_GETCHECK, 0, 0);

	bool bOption1 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption1RadioBox, BM_GETCHECK, 0, 0);
	bool bOption2 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption2RadioBox, BM_GETCHECK, 0, 0);
	bool bOption3 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption3RadioBox, BM_GETCHECK, 0, 0);

	string DelayMin;
	GetEditText(g_hWndRandom_DelayMin, DelayMin);

	string DelayMax;
	GetEditText(g_hWndRandom_DelayMax, DelayMax);

	string ButtonsPerClick;
	GetEditText(g_hWndRandom_ButtonsPerClick, ButtonsPerClick);

	int iDelayMin = atoi(DelayMin.c_str());
	int iDelayMax = atoi(DelayMax.c_str());
	int iButtonsPerClick = atoi(ButtonsPerClick.c_str());

	int x, y, delay;

	srand( (unsigned)::GetTickCount() );

	static Count = 0;

	while(true)
	{
		HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

		if(!hSDLWindow)
		{
			Sleep(10000); //Orbiter is closed. Let's wait.. :)
			continue;
		}
		
		delay = iDelayMin + rand() % (iDelayMax - iDelayMin); 
		PerformActionDelay(delay);

		if(Count == iButtonsPerClick)
		{
			Count = 0;

			x = rand() % SDL_WIDTH;
			y = rand() % SDL_HEIGHT;			

			PerformActionMouseClick(x, y);
		}
		else
		{
			Count++;
			//generate keys events

			if(bOption1)
			{
				//not implemented
			}
			else if(bOption2)
			{
				int list[] = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_RETURN };

				int index = rand() % sizeof(list)/sizeof(list[0]);

				PerformActionKeyPress(list[index]);
			}
			else if(bOption3)
			{
				//not implemented
			}
		}
	}
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
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION,
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
				case IDM_MAIN_SHOW_SDLWINDOW:
					ShowSDLWindow();
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}

			break;
		case WM_CREATE:
			{
				//make sure the that task bar is visible
				HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
				::ShowWindow(hTaskBarWindow, SW_SHOWNORMAL);
				
				g_hwndCB = CreateRpCommandBar(hWnd);

				// Initialize the shell activate info structure
				memset (&s_sai, 0, sizeof (s_sai));
				s_sai.cbSize = sizeof (s_sai);

				g_hWndTab = CreateTabControl(hWnd);
				g_hWndPage1 = CreatePage(g_hWndTab);
				g_hWndPage2 = CreatePage(g_hWndTab);
				g_hWndPage3 = CreatePage(g_hWndTab);

				//page 1
				RECT rt_list = { 1, 1, WIN_WIDTH - 16, WIN_HEIGHT - 4 * MENU_HEIGHT - 5 };
				g_hWndList = CreateListBox(g_hWndPage1, rt_list);

				//page 2
				g_hWndRecord_MouseCheckBox = CreateCheckBox(g_hWndPage2, 10, 10, "Record mouse clicks");
				::SendMessage(g_hWndRecord_MouseCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRecord_KeyboardCheckBox = CreateCheckBox(g_hWndPage2, 10, 30, "Record keyboard");
				::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_SETCHECK, BST_CHECKED, 0);
				
				RECT rt_record_list = { 170, 10, WIN_WIDTH - 190, WIN_HEIGHT - 150 };
				g_hWndRecord_List = CreateListBox(g_hWndPage2, rt_record_list);

				g_hWndRecord_SaveButton = CreateButton(g_hWndPage2, 120 + 170, WIN_HEIGHT - 140, "&Save");
				g_hWndRecord_LoadButton = CreateButton(g_hWndPage2, 120 + 230, WIN_HEIGHT - 140, "&Load");
				g_hWndRecord_ClearButton = CreateButton(g_hWndPage2, 120 + 290, WIN_HEIGHT - 140, "&Clear");
				g_hWndRecord_GoButton = CreateButton(g_hWndPage2, 110, 200, "&Go");

				CreateLabel(g_hWndPage2, 10, 200, 25, "Play");
				g_hWndRecord_RepeatEdit = CreateEdit(g_hWndPage2, 35, 200, 32, "1", true, true);
				CreateLabel(g_hWndPage2, 70, 200, 30, "times");

				reinterpret_cast<WNDPROC>(::SetWindowLong(g_hWndPage2, GWL_WNDPROC, 
					reinterpret_cast<long>(PagesWndProc)));

				//page 3
				CreateLabel(g_hWndPage3, 10, 10, 150, "Delay between events:");	
				g_hWndRandom_DelayMin = CreateEdit(g_hWndPage3, 160, 10, 50, "500", true, true);
				CreateLabel(g_hWndPage3, 215, 10, 5, "-");	
				g_hWndRandom_DelayMax = CreateEdit(g_hWndPage3, 225, 10, 50, "5000", true, true);

				CreateLabel(g_hWndPage3, 10, 40, 160, "Number of buttons per click: ");	
				g_hWndRandom_ButtonsPerClick = CreateEdit(g_hWndPage3, 175, 40, 50, "0", true, true);

				g_hWndRandom_MouseCheckBox = CreateCheckBox(g_hWndPage3, 10, 70, "Generate mouse clicks", 200);
				::SendMessage(g_hWndRandom_MouseCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRandom_KeyboardCheckBox = CreateCheckBox(g_hWndPage3, 10, 90, "Generate keyboard events", 200);
				::SendMessage(g_hWndRandom_KeyboardCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRandom_KeyOption1RadioBox = CreateRadioBox(g_hWndPage3, 10, 120, "A - Z, 0 - 9", 220);
				g_hWndRandom_KeyOption2RadioBox = CreateRadioBox(g_hWndPage3, 10, 140, "UP, DOWN, LEFT, RIGHT, ENTER", 220);
				g_hWndRandom_KeyOption3RadioBox = CreateRadioBox(g_hWndPage3, 10, 160, "Phone keys: 0 - 9, *, #, C, n", 220);
				::SendMessage(g_hWndRandom_KeyOption2RadioBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRandom_GenerateButton = CreateButton(g_hWndPage3, 10, 200, "&Generate", 100);

				reinterpret_cast<WNDPROC>(::SetWindowLong(g_hWndPage3, GWL_WNDPROC, 
					reinterpret_cast<long>(PagesWndProc)));

				SetActivePage(piLogger);

				//Create orbiter
				::CreateThread(NULL, 0, OrbiterThread, 0, 0, &OrbiterThreadId);
			}
			break;

		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			FillRect(hdc, &rt, (HBRUSH)::GetStockObject(GRAY_BRUSH));
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

		case WM_NOTIFY:
		{
			NMHDR *lpnmhdr = (LPNMHDR) lParam;

			switch(lpnmhdr->code)
			{
				case TCN_SELCHANGE:
					if(lpnmhdr->hwndFrom == g_hWndTab)
						SetActivePage(TabCtrl_GetCurSel(g_hWndTab));
					break;
			}

			break;
		}


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK PagesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	int wmId, wmEvent;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch(wmEvent)
			{
				case BN_CLICKED:
				{
					if(lParam == (LPARAM)g_hWndRecord_LoadButton)
						OnRecord_Load();
					else if(lParam == (LPARAM)g_hWndRecord_SaveButton)
						OnRecord_Save();  
					else if(lParam == (LPARAM)g_hWndRecord_ClearButton) 
						OnRecord_Clear(); 
					else if(lParam == (LPARAM)g_hWndRecord_GoButton)    
						OnRecord_Go(); 
					else if(lParam == (LPARAM)g_hWndRandom_GenerateButton)
						OnRandom_Generate();

					break;
				}
			}
		break;

		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);

			FillRect(hdc, &rt, (HBRUSH)::GetSysColorBrush(COLOR_3DFACE));
			EndPaint(hWnd, &ps);
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
HWND CreateTabControl(HWND hWnd)
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
	iccex.dwSize = ICC_TAB_CLASSES;
	InitCommonControlsEx (&iccex);

	RECT rt = {1, MENU_HEIGHT, WIN_WIDTH - 5, WIN_HEIGHT - 3 * MENU_HEIGHT };

	HWND hTabCtrl = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE ,
		rt.left, rt.top, rt.right, rt.bottom, hWnd,(HMENU)"", g_hInst, NULL );

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;

	tcItem.pszText = _T("Random");
	::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);
	tcItem.pszText = _T("Recorder");
	::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);
	tcItem.pszText = _T("Logger");
	::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);

	return hTabCtrl;
}
//-----------------------------------------------------------------------------------------------------
HWND CreatePage(HWND hWnd)
{
	RECT rt = {3, MENU_HEIGHT, WIN_WIDTH - 12, WIN_HEIGHT - 4 * MENU_HEIGHT - 4 };

	HWND hWndPage = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER,
		rt.left, rt.top, rt.right, rt.bottom, hWnd, (HMENU)"", g_hInst, NULL );

	return hWndPage;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateListBox(HWND hWnd, RECT rt_list)
{
	HWND hWndList = 
		CreateWindow(TEXT("LISTBOX"), NULL, 
			(WS_CHILD | WS_VISIBLE | LBS_STANDARD) & ~LBS_SORT, 
			rt_list.left, rt_list.top, rt_list.right, rt_list.bottom, 
			hWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateCheckBox(HWND hParentWnd, int x, int y, const char* caption, int width/* = 130*/)
{
	RECT rt_cb = { x, y, width, 18 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateRadioBox(HWND hParentWnd, int x, int y, const char* caption, int width/* = 130*/)
{
	RECT rt_cb = { x, y, width, 18 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width/*=50*/)
{
	RECT rt_cb = { x, y, width, 20 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption)
{
	RECT rt_label = { x, y, width, 20 };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);

	HWND hWndList = 
		CreateWindow(TEXT("STATIC"), wTextBuffer, 
			WS_CHILD | WS_VISIBLE | SS_CENTER, 
			rt_label.left, rt_label.top, rt_label.right, rt_label.bottom, 
			hParentWnd, NULL, g_hInst, NULL
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
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
void SetActivePage(int TabIndex)
{
	TabCtrl_SetCurSel(g_hWndTab, TabIndex);

	//hides all windows
	::ShowWindow(g_hWndPage1, SW_HIDE);
	::ShowWindow(g_hWndPage2, SW_HIDE);
	::ShowWindow(g_hWndPage3, SW_HIDE);

	switch(TabIndex)
	{
		case 0:	::ShowWindow(g_hWndPage1, SW_SHOW); break;
		case 1:	::ShowWindow(g_hWndPage2, SW_SHOW); break;
		case 2:	::ShowWindow(g_hWndPage3, SW_SHOW); break;
		default:
			WriteStatusOutput("ERROR: invalid page index");
	}
}
//-----------------------------------------------------------------------------------------------------
void WriteStatusOutput(const char* pMessage)
{
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, pMessage, MAX_STRING_LEN);

	::SendMessage(g_hWndList, LB_ADDSTRING, 0L, (LPARAM)wTextBuffer);

	int Count = ::SendMessage(g_hWndList, LB_GETCOUNT, 0L, 0L);
	::SendMessage(g_hWndList, LB_SETTOPINDEX, Count - 1, 0L);
}
//-----------------------------------------------------------------------------------------------------
void ShowMainDialog() //actually, hides the sdl window
{
	WriteStatusOutput("Minimizing SDL window...");

	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);
	::ShowWindow(hSDLWindow, SW_HIDE);

	HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
	::ShowWindow(hTaskBarWindow, SW_SHOWNORMAL);
}
//-----------------------------------------------------------------------------------------------------
void ShowSDLWindow() 
{
	WriteStatusOutput("Showing SDL window...");

	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

	if(hSDLWindow)
	{
		::ShowWindow(hSDLWindow, SW_SHOWNORMAL);

		HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
		::ShowWindow(hTaskBarWindow, SW_HIDE);
	}
	else
	{
		::MessageBox(NULL, TEXT("Orbiter is not running"), TEXT("Orbiter 2.0.0.0"), MB_ICONINFORMATION); 
	}
}
//-----------------------------------------------------------------------------------------------------
void RecordAction(const char* sAction)
{
	string s = string(sAction) + "\n";

	FILE* f = fopen("record.out", "a+");
	
	fseek(f, 0, SEEK_END);
	fwrite(f, s.length(), 1, (void *)s.c_str());

	fclose(f);

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, sAction, MAX_STRING_LEN);

	::SendMessage(g_hWndRecord_List, LB_ADDSTRING, 0L, (LPARAM)wTextBuffer);

	int Count = ::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);
	::SendMessage(g_hWndRecord_List, LB_SETTOPINDEX, Count - 1, 0L);
}
//-----------------------------------------------------------------------------------------------------
void RecordDelay()
{
	static long OldTick = ::GetTickCount();
	long Tick = ::GetTickCount();

	string sDelay = "delay " + StringUtils::ltos(Tick - OldTick) + " ";

	OldTick = Tick;

	RecordAction(sDelay.c_str());
}
//-----------------------------------------------------------------------------------------------------
void RecordMouseAction(int x, int y)
{
	if(BST_CHECKED == ::SendMessage(g_hWndRecord_MouseCheckBox, BM_GETCHECK, 0, 0))
	{
		RecordDelay();

		string sAction = "click " + StringUtils::ltos(x) + ", " + StringUtils::ltos(y) + " ";
		RecordAction(sAction.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void RecordKeyboardAction(long key)
{
	if(BST_CHECKED == ::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_GETCHECK, 0, 0))
	{
		if(key == 121) //don't record key F10
			return;

		RecordDelay();

		string sAction = "button " + StringUtils::ltos(key) + " (char: '" + char(key) + "')";
		RecordAction(sAction.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Load()
{
	WriteStatusOutput("RECORD: LOAD");

	OPENFILENAME ofn;

	wchar_t lpstrFile[256];
	ZeroMemory(lpstrFile, sizeof(lpstrFile));
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwndMainDialog;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	if(GetOpenFileName(&ofn))
	{
		char sTextBuffer[256];
		wcstombs(sTextBuffer, ofn.lpstrFile, 256);

		vector<string> vectString;
		FileUtils::ReadFileIntoVector( sTextBuffer, vectString ); /** < reads file into a vector of strings for each line */

		for(vector<string>::iterator it = vectString.begin(); it != vectString.end(); it++)
		{
			wchar_t wTextBuffer[MAX_STRING_LEN];
			mbstowcs(wTextBuffer, (*it).c_str(), MAX_STRING_LEN);

			::SendMessage(g_hWndRecord_List, LB_ADDSTRING, 0L, (LPARAM)wTextBuffer);
		}

		int Count = ::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);
		::SendMessage(g_hWndRecord_List, LB_SETTOPINDEX, Count - 1, 0L);

		vectString.clear();
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Save()
{
	WriteStatusOutput("RECORD: SAVE");

	OPENFILENAME ofn;

	wchar_t lpstrFile[256];
	ZeroMemory(lpstrFile, sizeof(lpstrFile));

	wcscpy(lpstrFile, TEXT("RecordOutput.txt"));
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwndMainDialog;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;

	if(GetSaveFileName(&ofn))
	{
		int Count = ::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);

		vector<string> vectString;
		for(int i = 0; i < Count; i++)
		{
			wchar_t lpszBuffer[256];
			::SendMessage(g_hWndRecord_List, LB_GETTEXT, i, (LPARAM)(LPCTSTR)lpszBuffer);

			char sItemBuffer[MAX_STRING_LEN];
			wcstombs(sItemBuffer, lpszBuffer, MAX_STRING_LEN);

			vectString.push_back(string(sItemBuffer));
		}


		char sTextBuffer[MAX_STRING_LEN];
		wcstombs(sTextBuffer, ofn.lpstrFile, MAX_STRING_LEN);

		FileUtils::WriteVectorToFile( sTextBuffer, vectString ); /** < writes a file where the vector of strings are the lines */

		vectString.clear();
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Clear()
{
	WriteStatusOutput("RECORD: CLEAR");

	while(::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L) > 0)
		::SendMessage(g_hWndRecord_List, LB_DELETESTRING, 0L, 0L);
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Go()
{
	WriteStatusOutput("RECORD: GO");

	::SendMessage(g_hWndRecord_MouseCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
	::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);

	ShowSDLWindow();

	::CreateThread(NULL, 0, PlayerThread, 0, 0, &PlayerThreadId);
}
//-----------------------------------------------------------------------------------------------------
void PerformActionDelay(long delay)
{
	Sleep(delay);
}
//-----------------------------------------------------------------------------------------------------
void PerformActionKeyPress(long key)
{
	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

	//should be send
	::SendMessage(hSDLWindow, WM_KEYDOWN, key, 0L);
	::SendMessage(hSDLWindow, WM_KEYUP, key, 0L);

	RECT rc = { 5, SDL_HEIGHT - 20, 200, SDL_HEIGHT - 1};

	HDC hDC = ::GetDC(hSDLWindow);

	string Str = "Key press: " + StringUtils::ltos(key);

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, Str.c_str(), MAX_STRING_LEN);

	::SetTextColor(hDC, RGB(255, 0, 0));
	::SetBkMode(hDC, TRANSPARENT);
	::DrawText(hDC, wTextBuffer, Str.length(), &rc, 0);
	::ReleaseDC(hSDLWindow, hDC); 
}
//-----------------------------------------------------------------------------------------------------
void PerformActionMouseClick(int x, int y)
{
	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

	//should be send
	::SendMessage(hSDLWindow, WM_LBUTTONDOWN, 0L, MAKELPARAM(x, y));
	::SendMessage(hSDLWindow, WM_LBUTTONUP, 0L, MAKELPARAM(x, y));

	RECT rc = { x - 5, y - 5, x + 5, y + 5 };

	HDC hDC = ::GetDC(hSDLWindow);
	::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	::ReleaseDC(hSDLWindow, hDC);
}
//-----------------------------------------------------------------------------------------------------
void OnRandom_Generate()
{
	WriteStatusOutput("RANDOM: GENERATE");

	//::SendMessage(g_hWndRecord_MouseCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
	//::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);

	ShowSDLWindow();

	::CreateThread(NULL, 0, GeneratorThread, 0, 0, &GeneratorThreadId);
}
//-----------------------------------------------------------------------------------------------------
void GetEditText(HWND hWndEdit, string& Text)
{
	wchar_t lpstrFile[256];
	ZeroMemory(lpstrFile, sizeof(lpstrFile));
	
	union
		{
			wchar_t str[4];
			long l;
		}
	temp;

	temp.l = 256;

	lpstrFile[0] = temp.str[0];
	lpstrFile[1] = temp.str[1];
	lpstrFile[2] = temp.str[2];
	lpstrFile[3] = temp.str[3];

	::SendMessage(hWndEdit, EM_GETLINE, 0, reinterpret_cast<long>(lpstrFile));

	char sItemBuffer[MAX_STRING_LEN];
	wcstombs(sItemBuffer, lpstrFile, MAX_STRING_LEN);

	Text = sItemBuffer;
}
//-----------------------------------------------------------------------------------------------------