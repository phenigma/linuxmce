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

MouseBehavior_Linux::~MouseBehavior_Linux()
{
    OrbiterLinux *pOrbiterLinux = ptrOrbiterLinux();
    // ptr should not be null
    // if it is, then this class was destroyed in the wrong place
    // now fixed, but leave the check anyway, only here
    if (pOrbiterLinux)
        if (pOrbiterLinux->m_pX11->Mouse_IsConstrainActive())
            pOrbiterLinux->m_pX11->Mouse_Constrain_Release();
}

OrbiterLinux * MouseBehavior_Linux::ptrOrbiterLinux()
{
    OrbiterLinux * pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
    if (pOrbiterLinux == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::ptrOrbiterLinux() : NULL dynamic_cast<OrbiterLinux *>(%p)",
            m_pOrbiter
            );
        return NULL;
    }
    return pOrbiterLinux;
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
	MouseBehavior::SetMousePosition(X,Y);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",X,Y);
    ptrOrbiterLinux()->m_pX11->Mouse_SetPosition(X, Y);
}

void MouseBehavior_Linux::ShowMouse(bool bShow)
{
    X11_Locker lock(ptrOrbiterLinux()->GetDisplay());
	SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
}

bool MouseBehavior_Linux::ConstrainMouse(const PlutoRectangle &rect)
{
    //return false;
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d)",
                          rect.X, rect.Y, rect.Width, rect.Height
                          );
    static const char sWindowClassName[] = "constrain_mouse.constrain_mouse";
    bool bResult = ptrOrbiterLinux()->m_pX11->Mouse_Constrain(rect.X, rect.Y, rect.Width, rect.Height, ptrOrbiterLinux()->GetMainWindow());
    if (bResult)
    {
        ptrOrbiterLinux()->m_pWMController->SetLayer(sWindowClassName, LayerBelow);
        ptrOrbiterLinux()->m_pWinListManager->ActivateSdlWindow();
        g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : done",
                              rect.X, rect.Y, rect.Width, rect.Height
                              );
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : error",
                              rect.X, rect.Y, rect.Width, rect.Height
                              );
    }
    return bResult;
}

void MouseBehavior_Linux::SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle)
{
    g_pPlutoLogger->Write(LV_STATUS, "MouseBehavior_Linux::SetMousePointerStyle(%d)");
    // convert enum values
    std::string sErr;
    std::string sDir = "/usr/pluto/orbiter/skins/Basic/cursors/pointers_bw/";
    std::string sName;
    int nShape = 0;
    switch (mouseCursorStyle)
    {
        case mcs_Normal:
            nShape = XC_top_left_arrow;
            if (! ptrOrbiterLinux()->m_pX11->Mouse_SetCursor_Font(ptrOrbiterLinux()->GetMainWindow(), nShape))
            {
                g_pPlutoLogger->Write(
                    LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : shape==%d",
                    mouseCursorStyle, nShape
                    );
                return;
            }
            return;
        case mcs_LeftRight:
            //nShape = XC_sb_h_double_arrow;
            sName = "left_right.xbm";
            break;
        case mcs_UpDown:
            //nShape = XC_sb_v_double_arrow;
            sName = "up_down.xbm";
            break;
        case mcs_AnyDirection:
            //nShape = XC_cross_reverse;
            sName = "horiz_vert.xbm";
            break;
        case mcs_LeftRightUpDown:
            //nShape = XC_fleur;
            sName = "horiz_vert.xbm";
            break;
        default:
            g_pPlutoLogger->Write(
                LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : bad value",
                mouseCursorStyle
                );
            return;
    }
    std::string sPath = sDir + sName;
    std::string sPathMask = sPath + ".msk";
    // try to change the cursor
    if (! ptrOrbiterLinux()->m_pX11->Mouse_SetCursor_Image(ptrOrbiterLinux()->GetMainWindow(), sPath, sPathMask))
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : path==%s, pathmask==%s",
            mouseCursorStyle, sPath.c_str(), sPathMask.c_str()
            );
        return;
    }
}
