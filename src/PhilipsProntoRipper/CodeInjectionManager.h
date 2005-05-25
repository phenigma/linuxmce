#pragma once

#include <windows.h> 
#include <winuser.h> 
#include <winnt.h> 
#include <winable.h>
//-------------------------------------------------------------------------------------------------------------
int SetSelfPrivilege(LPSTR privlg, char doenable);
int InjectCode(HWND hWnd);
//-------------------------------------------------------------------------------------------------------------