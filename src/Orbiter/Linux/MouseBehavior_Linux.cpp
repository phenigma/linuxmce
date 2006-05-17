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
        , m_bIsActive_Mouse_Constrain(false)
        , m_window_Mouse_Constrain(0)
{
}

MouseBehavior_Linux::~MouseBehavior_Linux()
{
    if (m_bIsActive_Mouse_Constrain)
    {
        // deactivate
        X11_Mouse_Constrain(ptrOrbiterLinux()->getDisplay(), 0, 0, 0, 0);
    }
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
    m_pOrbiter->X_LockDisplay();
	MouseBehavior::SetMousePosition(X,Y);
    Display *dpy = ptrOrbiterLinux()->getDisplay();
    Window rootwindow = DefaultRootWindow (dpy);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",X,Y);

    XWarpPointer(dpy, rootwindow, rootwindow, 0, 0, 0, 0, X, Y);

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
    g_pPlutoLogger->Write(LV_STATUS, "X11NewUI : MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d)",
                          rect.X, rect.Y, rect.Width, rect.Height
                          );

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
    // call the real function
    std::string sErrorMessage;
    bool bSuccess = X11_Mouse_Constrain(ptrOrbiterLinux()->getDisplay(), rect.X, rect.Y, rect.Width, rect.Height, &sErrorMessage);
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
    g_pPlutoLogger->Write(LV_STATUS, "X11NewUI : MouseBehavior_Linux::SetMousePointerStyle(%d)");
    // go to the right object
    OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
    if (pOrbiterLinux == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : dynamic_cast error",
            mouseCursorStyle
            );
        return;
    }
    // convert enum values
    std::string sErr;
    std::string sDir = "/usr/pluto/orbiter/skins/Basic/cursors/pointers_bw/";
    std::string sName;
    int nShape = 0;
    switch (mouseCursorStyle)
    {
        case mcs_Normal:
            nShape = XC_top_left_arrow;
            if (! X11_Window_SetCursor_Font(
                    ptrOrbiterLinux()->getDisplay(), ptrOrbiterLinux()->getWindow(),
                    nShape, &sErr
                    ))
            {
                g_pPlutoLogger->Write(
                    LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : X11_Window_SetCursor_Font(%d) : %s",
                    mouseCursorStyle, nShape, sErr.c_str()
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
    if (! X11_Window_SetCursor_Image(
            ptrOrbiterLinux()->getDisplay(), ptrOrbiterLinux()->getWindow(),
            sPath, sPathMask, &sErr
            ))
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : X11_Window_SetCursor_Image(%s, %s) : %s",
            mouseCursorStyle, sPath, sPathMask, sErr.c_str()
            );
        return;
    }
}

std::string X11_ErrorText(Display * pDisplay, int error_code)
{
    if (error_code == 0)
        return "";
    static const int size_buffer = 10000;
    char buffer[size_buffer];
    buffer[0] = '\0';
    buffer[0] = 0;
    XGetErrorText(pDisplay, error_code, buffer, size_buffer);
    return buffer;
}

bool X11_Pixmap_ReadFile(Display *pDisplay, Window window, const std::string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return, std::string *pStringError/*=NULL*/)
{
    // dummy error message string, used when no such string is required
    std::string string_error_dummy;
    if (pStringError == NULL)
        pStringError = &string_error_dummy;
    std::string &sErr = *pStringError;
    sErr = "";

    // X11 result or status
    int code = 0;

    // start
    XLockDisplay(pDisplay);

    do
    {
        // read x-bitmap from file
        code = XReadBitmapFile(pDisplay, window, sPath.c_str(), &width_return, &height_return, &pixmap_return, &x_hot_return, &y_hot_return);
        if (code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        if (x_hot_return < 0)
        {
            x_hot_return = 0;
        }
        if (y_hot_return < 0)
        {
            y_hot_return = 0;
        }
    } while (0);

    // end
    XSync(pDisplay, false);
    XUnlockDisplay(pDisplay);
    if (sErr.size())
        return false;
    return true;
}

bool MouseBehavior_Linux::X11_Mouse_Constrain(Display *pDisplay, int nPosX, int nPosY, unsigned int nWidth, int unsigned nHeight, std::string *pStringError/*=NULL*/)
{
    // dummy error message string, used when no such string is required
    std::string string_error_dummy;
    if (pStringError == NULL)
        pStringError = &string_error_dummy;
    std::string &sErr = *pStringError;
    sErr = "";

    // X11 result or status
    int code = 0;

    // start
    XLockDisplay(pDisplay);

    do
    {
        Window &window = m_window_Mouse_Constrain;

        // hide window case
        if ( (nWidth == 0) && (nHeight == 0) )
        {
            if (! m_bIsActive_Mouse_Constrain)
            {
                sErr = "window not created";
                break;
            }
            m_bIsActive_Mouse_Constrain = false;
            code = XUngrabPointer(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
            code = XUnmapWindow(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
            code = XDestroyWindow(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
            break;
        }

        // check values
        // Size (not including the border) must be nonzero (or a Value error results)
        // Note: The Xlib manual doesn't mention this restriction ?
        if ( (nWidth <= 0) || (nHeight <= 0) )
        {
            sErr = "bad arguments, size must be greater than 0";
            break;
        }

        // show window
        if (m_bIsActive_Mouse_Constrain)
        {
            sErr = "window is already created";
            break;
        }

        if (window > 0)
        {
            code = XDestroyWindow(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
        }

        Window parent_window = XDefaultRootWindow(pDisplay);

        window = XCreateSimpleWindow(
            pDisplay,
            parent_window,
            nPosX, nPosY, nWidth, nHeight,
            0, // BorderWidth,
            0, // BorderColor
            0 // BgColor
            );
        XSync(pDisplay, false);

        if (window <= 0)
        {
            sErr = "cannot create window";
            break;
        }
        XClassHint classHint;
        classHint.res_name = "pointer_constrain_name";
        classHint.res_class = "pointer_constrain_class";
        code = XSetClassHint(pDisplay, window, &classHint);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        XSync(pDisplay, false);
        code = XStoreName(pDisplay, window, "pointer_constrain_window");
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        XSync(pDisplay, false);
        code = XMapWindow(pDisplay, window);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        XSync(pDisplay, false);
        if (1)
        {
            code = XLowerWindow(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
        }
        if (1)
        {
            code = XGrabPointer(
                pDisplay, window,
                false,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask,
                GrabModeAsync, // pointer_mode
                GrabModeAsync, // keyboard_mode
                window, // confine_to_window
                None, // cursor // TODO: This may need to be set to the cursor of this window
                CurrentTime );
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            XSync(pDisplay, false);
        }

        // correct the window position (wm may change it after create or show)
        code = XMoveResizeWindow(pDisplay, window, nPosX, nPosY, nWidth, nHeight);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        XSync(pDisplay, false);

        m_bIsActive_Mouse_Constrain = true;

    } while (0);

    // end
    XSync(pDisplay, false);
    XUnlockDisplay(pDisplay);
    if (sErr.size())
        return false;
    return true;
}

bool MouseBehavior_Linux::X11_Window_SetCursor_Font(Display *pDisplay, Window window, int nShape, std::string *pStringError/*=NULL*/)
    {
    // dummy error message string, used when no such string is required
    std::string string_error_dummy;
    if (pStringError == NULL)
        pStringError = &string_error_dummy;
    std::string &sErr = *pStringError;
    sErr = "";

    // X11 result or status
    int code = 0;

    // start
    XLockDisplay(pDisplay);

    do
    {
        // revert to the default cursor
        if (nShape < 0)
        {
            code = XUndefineCursor(pDisplay, window);
            if (! code)
            {
                sErr = X11_ErrorText(pDisplay, code);
                break;
            }
            break;
        }
        // create the cursor
        Cursor cursor = XCreateFontCursor(pDisplay, nShape);
        if (! cursor)
        {
            sErr = "cannot create the font cursor";
            break;
        }
        // change the cursor
        code = XDefineCursor(pDisplay, window, cursor);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
    } while (0);

    // end
    XSync(pDisplay, false);
    XUnlockDisplay(pDisplay);
    if (sErr.size())
        return false;
    return true;
}

bool MouseBehavior_Linux::X11_Window_SetCursor_Image(Display *pDisplay, Window window, const std::string &sPath, const std::string &sPathMask, std::string *pStringError/*=NULL*/)
{
    // dummy error message string, used when no such string is required
    std::string string_error_dummy;
    if (pStringError == NULL)
        pStringError = &string_error_dummy;
    std::string &sErr = *pStringError;
    sErr = "";

    // X11 result or status
    int code = 0;

    // start
    XLockDisplay(pDisplay);

    Pixmap pixmap = None;
    Pixmap pixmap_mask = None;
    do
    {
        unsigned int width_return = 0;
        unsigned int height_return = 0;
        int x_hot_return = 0;
        int y_hot_return = 0;
        // read pixmap
        if (! X11_Pixmap_ReadFile(pDisplay, window, sPath.c_str(), pixmap, width_return, height_return, x_hot_return, y_hot_return, &sErr))
        {
            break;
        }
        int x_hot_return_mask = 0;
        int y_hot_return_mask = 0;
        unsigned int width_return_mask = 0;
        unsigned int height_return_mask = 0;
        // read pixmap mask
        if (! X11_Pixmap_ReadFile(pDisplay, window, sPathMask.c_str(), pixmap_mask, width_return_mask, height_return_mask, x_hot_return_mask, y_hot_return_mask, &sErr))
        {
            break;
        }
        // check values
        if ( ( width_return != width_return_mask ) || ( height_return != height_return_mask ) )
        {
            sErr = "size of the x-bitmap and his mask differ";
            break;
        }
        // prepare extra data
        int nScreen = XDefaultScreen(pDisplay);
        Colormap colormap = XDefaultColormap(pDisplay, nScreen);
        XColor color_fg;
        color_fg.pixel = BlackPixel(pDisplay, nScreen);
        code = XQueryColor(pDisplay, colormap, &color_fg);
        XSync(pDisplay, false);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        XColor color_bg;
        color_bg.pixel = WhitePixel(pDisplay, nScreen);
        code = XQueryColor(pDisplay, colormap, &color_bg);
        XSync(pDisplay, false);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
        // create the cursor
        Cursor cursor = XCreatePixmapCursor(pDisplay, pixmap, pixmap_mask, &color_fg, &color_bg, x_hot_return, y_hot_return);
        if (! cursor)
        {
            sErr = "Cannot create pixmap cursor";
            break;
        }
        // // recolor the cursor
        //color_bg.red = 255;
        //color_bg.green = 0;
        //color_bg.blue = 0;
        //color_fg.red = 0;
        //color_fg.green = 255;
        //color_fg.blue = 0;
        //code = XRecolorCursor(pDisplay, cursor, &color_bg, &color_fg);
        //if (! code)
        //{
        //    sErr = X11_ErrorText(pDisplay, code);
        //    break;
        //}
        // change the cursor
        code = XDefineCursor(pDisplay, window, cursor);
        if (! code)
        {
            sErr = X11_ErrorText(pDisplay, code);
            break;
        }
    } while (0);

    // cleanup
    if (pixmap != None)
        XFreePixmap(pDisplay, pixmap);
    if (pixmap_mask != None)
        XFreePixmap(pDisplay, pixmap_mask);

    // end
    XSync(pDisplay, false);
    XUnlockDisplay(pDisplay);
    if (sErr.size())
        return false;
    return true;
}
