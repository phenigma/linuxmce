#include "Win32Popup.h"
#include "Logger.h"
#include "CallBackTypes.h"
#include "Task.h"
using namespace DCE;
//--------------------------------------------------------------------------------------------------------------
void *Win32PopupWorkerThread(void *p);
//--------------------------------------------------------------------------------------------------------------
Win32Popup::Win32Popup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData) :
	OrbiterPopup(aDialogType, pCallBackData), m_WorkerThreadID(0), m_bAbortWorkerThread(false)
{
}
//--------------------------------------------------------------------------------------------------------------
Win32Popup::~Win32Popup(void)
{
	m_bAbortWorkerThread = true;

	if(m_WorkerThreadID > 0)
		pthread_join(m_WorkerThreadID, NULL);

	m_WorkerThreadID = 0;
}
//--------------------------------------------------------------------------------------------------------------
void Win32Popup::Initialize()
{
	pthread_create(&m_WorkerThreadID, NULL, Win32PopupWorkerThread, this);
}
//--------------------------------------------------------------------------------------------------------------
bool Win32Popup::InitWindow()
{
	RECT r = {m_rect.X, m_rect.Y, m_rect.X + m_rect.Width, m_rect.Y + m_rect.Height};
	DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW /*| WS_EX_TRANSPARENT*/;
	DWORD style = (WS_VISIBLE | WS_POPUP) & ~WS_CAPTION;

	if (!(m_hWnd = Create( 0, r, "win32 popup", style, exStyle )))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Window creation failed, id: %ul", m_ulPopupID);
		return false;
	}

	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
	ShowWindow(SW_SHOWNORMAL);
	return true;
}
//--------------------------------------------------------------------------------------------------------------
void Win32Popup::Run()
{
	if(!InitWindow())
		return;

	MSG msg;
	for(;;)
	{
		if (::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			if (msg.message == WM_QUIT)
				break;

			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		else
		{
			Task *pTask = GetTask();
			if(NULL != pTask)
			{
				if(pTask->TaskType == cbOnDialogDelete)
				{
					::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
					break;
				}
				else
				{
					//process the refresh here
				}
			}
			else
				Sleep(50);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------
LRESULT Win32Popup::OnClose( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	return 0L;
}
//--------------------------------------------------------------------------------------------------------------
LRESULT Win32Popup::OnCreate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	return 0L;
}
//--------------------------------------------------------------------------------------------------------------
LRESULT Win32Popup::OnDestroy( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	delete this;
	return 0L;
}
//--------------------------------------------------------------------------------------------------------------
LRESULT Win32Popup::OnPaint( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	PAINTSTRUCT ps;

	BeginPaint( &ps );
	HDC hdc = GetDC();
	RECT rect = {0, 0, m_rect.Width, m_rect.Height};
	::FillRect(hdc, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	ReleaseDC(hdc);
	EndPaint( &ps );

	return 0L;
}
//--------------------------------------------------------------------------------------------------------------
void *Win32PopupWorkerThread(void *p)
{
	Win32Popup *pWin32Popup = static_cast<Win32Popup *>(p);
	if(NULL == pWin32Popup)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Win32PopupWorkerThread: got invalid Win32Popup!");
		return NULL;
	}

	pWin32Popup->Run();
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------

