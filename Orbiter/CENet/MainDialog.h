#if !defined(__MAIN_DIALOG_H__)
#define __MAIN_DIALOG_H__
//-----------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>

#include "StartOrbiterCE.h"
//-----------------------------------------------------------------------------------------------------
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);

void				WriteStatusOutput(const char* pMessage);
void				ShowMainDialog();
void				ShowSDLWindow();

void				RecordMouseAction(int x, int y);
void				RecordKeyboardAction(long key);
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