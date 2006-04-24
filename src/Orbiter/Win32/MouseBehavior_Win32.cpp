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

