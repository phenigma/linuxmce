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
	MouseBehavior::SetMousePosition(X,Y);
	SetCursorPos(X,Y);
}

void MouseBehavior_Win32::ShowMouse(bool bShow)
{
	ShowCursor(bShow);
}

bool MouseBehavior_Win32::ConstrainMouse(const PlutoRectangle &rect)
{
	m_bMouseConstrained = rect.X!=0 || rect.Y!=0 || rect.Width!=0 || rect.Height!=0;
	if( rect.Width==0 || rect.Height==0 )
	{
		ClipCursor(NULL);
		return true;
	}

	RECT rcClip;           // new area for ClipCursor
	rcClip.left=rect.X;
	rcClip.right=rect.X + rect.Width;
	rcClip.top=rect.Y;
	rcClip.bottom=rect.Y + rect.Height;
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

