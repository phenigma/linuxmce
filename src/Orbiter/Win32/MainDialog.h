#if !defined(__MAIN_DIALOG_H__)
#define __MAIN_DIALOG_H__
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
	#include "stdafx.h"
	#include "StartOrbiterCE.h"
	#include <commctrl.h>
	#include <aygshell.h>
	#include <sipapi.h>

	#include "StartOrbiterCE.h"

#else
	#include "windows.h"
	#include <string>
	using namespace std;

	#include "StartOrbiter_Win32.h"
#endif
//-----------------------------------------------------------------------------------------------------
WORD				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);

void				WriteStatusOutput(const char* pMessage);
void				ShowMainDialog();
void				ShowSDLWindow();

void				RecordMouseAction(int x, int y);
void				RecordKeyboardAction(long key);
//-----------------------------------------------------------------------------------------------------
void				StartOrbiterThread();
void				LoadUI_From_ConfigurationData();
//-----------------------------------------------------------------------------------------------------
struct CommandLineParams
{
	string sRouter_IP;
	int PK_Device;
	string sLogger;
	int Width, Height;
	bool bLocalMode; 
	bool bFullScreen;
	string sLocalDirectory;
	string sNestedDisplay;
};
//-----------------------------------------------------------------------------------------------------
extern CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
#endif //__MAIN_DIALOG_H__