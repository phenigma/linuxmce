//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "swp_xdga.h"
#endif

#include "swp_xdga.h"

#ifdef USE_DEBUG_CODE
#include "define_all.h"
#else
#include "../../defines/define_all.h"
#endif // USE_DEBUG_CODE

XDGADevice *g_pXDGADevice = NULL;
Display *g_pDisplay = NULL;

#define HWND Window
HWND DGAhwnd;
//static XDGAMode* modes;
//static XDGADevice *dga_dev;

void X11DRV_DGAMotionEvent( HWND UNUSED_ARG(hwnd), XEvent *xev )
{
    XDGAMotionEvent *event = (XDGAMotionEvent *)xev;
    _LOG_DBG("dx==%d, dy==%d", event->dx, event->dy);
    //X11DRV_send_mouse_input( DGAhwnd, MOUSEEVENTF_MOVE, event->dx, event->dy, 0, event->time, 0, 0 );
}

//void X11DRV_DGAButtonPressEvent( HWND hwnd, XEvent *xev )
//{
//    XDGAButtonEvent *event = (XDGAButtonEvent *)xev;
//    int buttonNum = event->button - 1;
//    if (buttonNum >= NB_BUTTONS) return;
//    update_key_state( event->state );
//    X11DRV_send_mouse_input( DGAhwnd, button_down_flags[buttonNum], 0, 0,
//                             0, EVENT_x11_time_to_win32_time(event->time), 0, 0 );
//}

//void X11DRV_DGAButtonReleaseEvent( HWND hwnd, XEvent *xev )
//{
//    XDGAButtonEvent *event = (XDGAButtonEvent *)xev;
//    int buttonNum = event->button - 1;
//    if (buttonNum >= NB_BUTTONS) return;
//    update_key_state( event->state );
//    X11DRV_send_mouse_input( DGAhwnd, button_up_flags[buttonNum], 0, 0,
//                             0, EVENT_x11_time_to_win32_time(event->time), 0, 0 );
//}

bool XDGA_Mouse_GetPosition(int &nPosX, int &nPosY)
{
    int major, minor, event_base, error_base;
    Bool ok;
    ok = XDGAQueryExtension(g_pDisplay, &event_base, &error_base);
    _LOG_DBG("ok==%d, event_base==%d, error_base==%d", ok, event_base, error_base);
    ok = XDGAQueryVersion(g_pDisplay, &major, &minor);
    _LOG_DBG("ok==%d, major==%d, minor==%d", ok, major, minor);
    //XDGAMotionEvent xdgamotionevent;
    //XDGAEvent xdgaevent;
    XDGASelectInput(g_pDisplay, XDefaultScreen(g_pDisplay), PointerMotionMask | PointerMotionHintMask);
    XDGAMotionEvent *event = new XDGAMotionEvent;
    nPosX = event->dx;
    nPosY = event->dy;
    _LOG_DBG("dx==%d, dy==%d", event->dx, event->dy);
    delete event;
    return true;
}

bool XDGA_Init(Display *pDisplay)
{
    g_pDisplay = pDisplay;
    int nRetCode = XDGAOpenFramebuffer(g_pDisplay, XDefaultScreen(g_pDisplay));
    _COND_RET(nRetCode==0, false);
    int nmodes = 0;
    _LOG_NFO("XDGAQueryModes()");
    XDGAMode* modes = XDGAQueryModes(g_pDisplay, XDefaultScreen(g_pDisplay), &nmodes);
    _LOG_DBG("nmodes==%d", nmodes);
    _COND_RET(modes, false);
    _LOG_NFO("XDGASetMode()");
    for (int i=0; i<nmodes; i++)
    {
        if (i <= 3)
            continue;
        g_pXDGADevice = XDGASetMode(g_pDisplay, XDefaultScreen(g_pDisplay), modes[i].num);
        if (g_pXDGADevice)
            _LOG_NFO("init mode %d/%d : ok", i, nmodes);
        else
            _LOG_WRN("init mode %d/%d : ok", i, nmodes);
    }
    if (g_pXDGADevice)
    {
        _LOG_ERR("Cannot set XDGA mode");
        return false;
    }
    XDGASetViewport(g_pDisplay, XDefaultScreen(g_pDisplay), 0, 0, XDGAFlipImmediate);
    _LOG_NFO("done");
    return true;
}

bool XDGA_Exit()
{
    g_pXDGADevice = XDGASetMode(g_pDisplay, XDefaultScreen(g_pDisplay), 0);
    _COND(g_pXDGADevice);
    XDGACloseFramebuffer(g_pDisplay, XDefaultScreen(g_pDisplay));
    g_pXDGADevice = NULL;
    _LOG_NFO("done");
    return true;
}
