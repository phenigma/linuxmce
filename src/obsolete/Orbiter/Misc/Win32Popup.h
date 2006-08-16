#pragma once

#include "../CallBackData.h"
#include "../OrbiterPopup.h"
#include "../PlutoUtils/MultiThreadIncludes.h"
//--------------------------------------------------------------------------------------------------------------
#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>
//--------------------------------------------------------------------------------------------------------------
class Win32Popup : public OrbiterPopup, public CWindowImpl<Win32Popup>
{
	BEGIN_MSG_MAP( Win32Popup )
		MESSAGE_HANDLER( WM_CLOSE,       OnClose )
		MESSAGE_HANDLER( WM_CREATE,      OnCreate )
		MESSAGE_HANDLER( WM_DESTROY,     OnDestroy )
		MESSAGE_HANDLER( WM_PAINT,		 OnPaint )
	END_MSG_MAP()

	DECLARE_WND_CLASS(TEXT("Win32Popup"));

public:
	Win32Popup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData);
	void Initialize();
	void Run();
	void RefreshPopup(CallBackData *pCallBackData);

private:
	pthread_t m_WorkerThreadID;
	bool m_bAbortWorkerThread;

	bool InitWindow();
	~Win32Popup(void);

    LRESULT OnClose( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnCreate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnDestroy( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnPaint( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
};
//--------------------------------------------------------------------------------------------------------------