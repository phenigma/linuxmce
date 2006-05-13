#include "MouseBehavior_Linux.h"

#include "SDL.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#define USE_X11_LIB (! USE_WX_LIB)
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

using namespace DCE;

//-----------------------------------------------------------------------------------------------------

MouseBehavior_Linux::MouseBehavior_Linux(Orbiter *pOrbiter)
        : MouseBehavior(pOrbiter)
{
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
    m_pOrbiter->X_LockDisplay();
	MouseBehavior::SetMousePosition(X,Y);
    Display *dpy = XOpenDisplay (NULL);
    Window rootwindow = DefaultRootWindow (dpy);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",X,Y);

    XWarpPointer(dpy, rootwindow,rootwindow , 0, 0, 0, 0, X,Y);
    XCloseDisplay(dpy);

    m_pOrbiter->X_UnlockDisplay();
}

void MouseBehavior_Linux::ShowMouse(bool bShow)
{
    m_pOrbiter->X_LockDisplay();
	SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
    m_pOrbiter->X_UnlockDisplay();
}

bool MouseBehavior_Linux::ConstrainMouse(const PlutoRectangle &rect)
{
    // go to the right object
    OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
    if (pOrbiterLinux == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : dynamic_cast error",
            rect.X, rect.Y, rect.Width, rect.Height
            );
        return false;
    }
    XRecordExtensionHandler *pXRecordExtensionHandler = pOrbiterLinux->GetXRecordExtensionHandler();
    if (pXRecordExtensionHandler == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : NULL m_pRecordHandler",
            rect.X, rect.Y, rect.Width, rect.Height
            );
        return false;
    }
    // call the real function
    std::string sErrorMessage;
    bool bSuccess = pXRecordExtensionHandler->X11_ConstrainMouse(rect.X, rect.Y, rect.Width, rect.Height, &sErrorMessage);
    // log the status
    if (bSuccess)
    {
        g_pPlutoLogger->Write(
            LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : ok",
            rect.X, rect.Y, rect.Width, rect.Height
            );
    }
    else
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : %s",
            rect.X, rect.Y, rect.Width, rect.Height, sErrorMessage.c_str()
            );
    }
    return true;
}

void MouseBehavior_Linux::SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle)
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

