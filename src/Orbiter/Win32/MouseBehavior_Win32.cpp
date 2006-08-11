#include "MouseBehavior_Win32.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
using namespace DCE;


//-----------------------------------------------------------------------------------------------------
MouseBehavior_Win32::MouseBehavior_Win32(Orbiter *pOrbiter)
	: MouseBehavior(pOrbiter)
{
}

void MouseBehavior_Win32::SetMousePosition(int X,int Y)
{
	int nWindowX = 0, nWindowY = 0;
	GetWindowPosition(nWindowX, nWindowY);

	MouseBehavior::SetMousePosition(nWindowX + X, nWindowY + Y);
	
	SetCursorPos(nWindowY + X, nWindowY + Y);
}

void MouseBehavior_Win32::ShowMouse(bool bShow)
{
	ShowCursor(bShow);
}

bool MouseBehavior_Win32::ConstrainMouse(const PlutoRectangle &rect)
{
	int nWindowX = 0, nWindowY = 0;
	GetWindowPosition(nWindowX, nWindowY);

	m_bMouseConstrained = rect.X!=0 || rect.Y!=0 || rect.Width!=0 || rect.Height!=0;
	if( rect.Width==0 || rect.Height==0 )
	{
		ClipCursor(NULL);
		return true;
	}

	RECT rcClip;           // new area for ClipCursor
	rcClip.left=nWindowX + rect.X;
	rcClip.right=nWindowX + rect.X + rect.Width;
	rcClip.top=nWindowY + rect.Y;
	rcClip.bottom=nWindowY + rect.Y + rect.Height;
	ClipCursor(&rcClip); 
	return true;
}

void MouseBehavior_Win32::SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle)
{
	LPCTSTR lpCursor;

	switch(mouseCursorStyle)
	{
	case mcs_Normal:
		lpCursor =	IDC_ARROW;
		break;
	case mcs_LeftRight:
		lpCursor =	IDC_SIZEWE;
		break;
	case mcs_UpDown:
		lpCursor =	IDC_SIZENS;
		break;
	case mcs_AnyDirection:
		lpCursor =	IDC_SIZEALL;
		break;
	case mcs_LeftRightUpDown:
		lpCursor =	IDC_CROSS;
		break;
	};
	HCURSOR hCursor = LoadCursor(NULL, IDC_WAIT); 
	SetCursor(hCursor);
}

void MouseBehavior_Win32::GetWindowPosition(int &x, int &y)
{
	//we'll need to window's position in order to apply the constrain relatively to the window's position
	HWND hWnd = ::FindWindow("SDL_app", "OrbiterGL");
	RECT rectWin;
	::GetWindowRect(hWnd, &rectWin);

	x = rectWin.left;
	y = rectWin.top;
}
