//
// Author : Remus C.
//
// Changed by : ...
//

// notes:
//        use Lock() and Unlock()
//        use a single instance
//        initialize it from the start

#ifndef _SWP_XDGA_H_
#define _SWP_XDGA_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "swp_xdga.h"
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86dga.h>

#define USE_XDGA

bool XDGA_Mouse_GetPosition(int &nPosX, int &nPosY, bool bRelative=false);

bool XDGA_Init(Display *pDisplay);
bool XDGA_Exit();
extern XDGADevice *g_pXDGADevice;

#endif
// _SWP_XDGA_H_
