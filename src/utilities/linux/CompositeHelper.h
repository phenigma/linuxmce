#ifndef __COMPOSITE_HELPER_H__
#define __COMPOSITE_HELPER_H__
//---------------------------------------------------------------------------------------------------------------
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>
//---------------------------------------------------------------------------------------------------------------
XVisualInfo *GetVisualForComposite(Display *dpy);
bool RegisterReplacementWindowForSDL(Display *dpy, XVisualInfo *visinfo, int nWidth, int nHeight);
//---------------------------------------------------------------------------------------------------------------
#endif //__COMPOSITE_HELPER_H__
