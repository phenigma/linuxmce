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
    m_pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_LockDisplay();
	MouseBehavior::SetMousePosition(X,Y);
    Display *dpy = XOpenDisplay (NULL);
    Window rootwindow = DefaultRootWindow (dpy);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",X,Y);

    XWarpPointer(dpy, rootwindow,rootwindow , 0, 0, 0, 0, X,Y);
    XCloseDisplay(dpy);

    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_UnlockDisplay();
}

void MouseBehavior_Linux::ShowMouse(bool bShow)
{
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_LockDisplay();
	SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_UnlockDisplay();
}
