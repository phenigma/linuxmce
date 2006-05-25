/*
  OrbiterLinux

  Copyright (C) 2004 Pluto, Inc., a Florida Corporation

  www.plutohome.com

  Phone: +1 (877) 758-8648

  This program is distributed according to the terms of the Pluto Public License, available at:
  http://plutohome.com/index.php?section=public_license

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include <stdlib.h>
#define USE_X11_LIB (! USE_WX_LIB)

#if (USE_WX_LIB)
#include "../wxAppMain/wx_all_include.cpp"
#include "../wxAppMain/wx_safe_dialog.h"
#include "../wxAppMain/wxdialog_waitgrid.h"
#include "../wxAppMain/wxdialog_waitlist.h"
#include "../wxAppMain/wxdialog_waituser.h"
#define USE_TASK_MANAGER true
#include "../Task.h"
#include "../TaskManager.h"
#endif // (USE_WX_LIB)

#include "DCE/Logger.h"
#include "OrbiterLinux.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>

#include "pluto_main/Define_Button.h"
#include "PlutoUtils/PlutoButtonsToX.h"
#include "OSDScreenHandler.h"
#include "MouseBehavior_Linux.h"

#include "../CallBackTypes.h"

#include "../dialog_types.h"

using namespace std;

// new calls to X(Un)LockDisplay activated by default
//
// to deactivate new calls to X(Un)LockDisplay
// copy this in LaunchOrbiter.sh
// export PLUTO_DISABLE_X11LOCK=

OrbiterLinux::OrbiterLinux(int DeviceID, int PK_DeviceTemplate,
                           string ServerAddress, string sLocalDirectory,
                           bool bLocalMode,
                           int nImageWidth, int nImageHeight)
        : Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode,
                     nImageWidth, nImageHeight)
          // defaults
          /**
           * @hack to make it work for the short term. We need to find a way to set the class name properly or use the window ID if we can find it.
           * The reason this is hack is because there could be potentially multiple SDL_Applications running at the same time which could break the controlling code.
           */

{
}


OrbiterLinux::~OrbiterLinux()
{
}


OrbiterRendererSDLLinux * OrbiterLinux::ptrRendererSDL()
{
    OrbiterRendererSDLLinux * pOrbiterSDL = dynamic_cast<OrbiterRendererSDLLinux *>(Renderer());
    if (pOrbiterSDL == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "OrbiterLinux::ptrOrbiterSDL() : NULL dynamic_cast<OrbiterSDL *>(%p)",
            this
            );
        return NULL;
    }
    return pOrbiterSDL;
}


void OrbiterLinux::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
{
	if( sOnOff!="X" )
		return;
    Display *dpy = ptrRendererSDL()->getDisplay();
    Window win = DefaultRootWindow (dpy);

    SDL_SysWMinfo sdlinfo;
    SDL_VERSION(&sdlinfo.version);
    int r2=SDL_GetWMInfo(&sdlinfo);
    Window w2 = sdlinfo.info.x11.wmwindow;
    Window w3 = sdlinfo.info.x11.window;
    Window w4 = sdlinfo.info.x11.fswindow;
    g_pPlutoLogger->Write(LV_CRITICAL, "GRABBING MOUSE window %d, r2: %d, w2: %d w3: %d w5: %d",(int) win,r2,(int) w2,(int) w3,(int) w4);

    win = w3;
    Pixmap blank;
    XColor dummy;
    char data[1] = {0};
    Cursor cursor;

    /* make a blank cursor */
    ptrRendererSDL()->X_LockDisplay();
    blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
    if(blank == None) fprintf(stderr, "error: out of memory.\n");
    cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
    XFreePixmap (dpy, blank);
    ptrRendererSDL()->X_UnlockDisplay();

    //GrabPointer();
    //GrabKeyboard();
}

void OrbiterLinux::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_WARNING, "CMD off not implemented on the orbiter yet");
}

void OrbiterLinux::CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::CMD_Activate_Window(%s)", sWindowName.c_str());
	ptrRendererSDL()->m_WinListManager.SetExternApplicationName(sWindowName);
}

void OrbiterLinux::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
{
    if( m_bYieldInput )
    {
        pair<bool,int> XKeySym = PlutoButtonsToX(atoi(sPK_Button.c_str()));
        g_pPlutoLogger->Write(LV_WARNING, "Need to forward pluto key %s to X key %d (shift %d)", sPK_Button.c_str(),XKeySym.second,XKeySym.first);

        Display *dpy = ptrRendererSDL()->getDisplay();
        ptrRendererSDL()->X_LockDisplay();

		if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0 );

        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), True, 0 );
        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), False, 0 );

        if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0 );

		ptrRendererSDL()->X_UnlockDisplay();
    }
    Orbiter::CMD_Simulate_Keypress(sPK_Button,sName,sCMD_Result,pMessage);
}

void OrbiterLinux::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
{
    ptrRendererSDL()->X_LockDisplay();
    Display *dpy = ptrRendererSDL()->getDisplay();
    Window rootwindow = DefaultRootWindow (dpy);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",iPosition_X,iPosition_Y);

    XWarpPointer(dpy, rootwindow,0 , 0, 0, 0, 0, iPosition_X, iPosition_Y);
    ptrRendererSDL()->X_UnlockDisplay();
}

void OrbiterLinux::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_STATUS, "Clicking mouse %s",sType.c_str());

    ptrRendererSDL()->X_LockDisplay();
    XTestFakeButtonEvent(ptrRendererSDL()->XServerDisplay, 1, true, 0);
    XTestFakeButtonEvent(ptrRendererSDL()->XServerDisplay, 1, false, 0);
    ptrRendererSDL()->X_UnlockDisplay();

    // again
    Display *dpy = XOpenDisplay(NULL);
    XTestFakeButtonEvent(dpy, 1, true, 0);
    XTestFakeButtonEvent(dpy, 1, false, 0);
    XCloseDisplay(dpy);
}

void OrbiterLinux::CMD_Surrender_to_OS(string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::CMD_Surrender_to_OS(%s, %d)", sOnOff.c_str(), bFully_release_keyboard);
    ptrRendererSDL()->GrabPointer(sOnOff != "1");
    ptrRendererSDL()->GrabKeyboard(sOnOff != "1");
    Orbiter::CMD_Surrender_to_OS(sOnOff, bFully_release_keyboard, sCMD_Result, pMessage);
}



/*virtual*/ ScreenHandler *OrbiterLinux::CreateScreenHandler()
{
	return new OSDScreenHandler(this, &m_mapDesignObj);
}

