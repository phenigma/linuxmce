#include "CompositeHelper.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
//---------------------------------------------------------------------------------------------------------------
XVisualInfo *GetVisualForComposite(Display *dpy)
{
	XLockDisplay(dpy); 

	int attrib[] = {
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_ALPHA_SIZE, 1,
		GLX_BUFFER_SIZE, 1,
		GLX_DOUBLEBUFFER, True,
		GLX_DEPTH_SIZE, 1,
		None };

	GLXFBConfig *fbconfigs, fbconfig;
	int numfbconfigs, render_event_base, render_error_base;
	XVisualInfo *visinfo;
	XRenderPictFormat *pictFormat;

	printf("Trying to use composite...\n");

	/* Make sure we have the RENDER extension */
	if(!XRenderQueryExtension(dpy, &render_event_base, &render_error_base)) 
	{
		printf("No RENDER extension found\n");
		XUnlockDisplay(dpy);
		return NULL;
	}

	/* Get the list of FBConfigs that match our criteria */
	fbconfigs = glXChooseFBConfig(dpy, DefaultScreen(dpy), attrib, &numfbconfigs);
	if (!fbconfigs) 
	{
		printf("None matched\n");
		/* None matched */
		XUnlockDisplay(dpy);
		return NULL;
	}

	int i = 0;

	printf("Number of FB configs found %d\n", numfbconfigs);

	/* Find an FBConfig with a visual that has a RENDER picture format that
	* has alpha */
	for (i = 0; i < numfbconfigs; i++) 
	{
		visinfo = glXGetVisualFromFBConfig(dpy, fbconfigs[i]);
		if (!visinfo) continue;

		pictFormat = XRenderFindVisualFormat(dpy, visinfo->visual);
		if (!pictFormat) continue;

		if(pictFormat->direct.alphaMask > 0) 
		{
			fbconfig = fbconfigs[i];

			printf("================================================\n");
			printf("Found visual with alpha and render:\n");
			printf("Found visual 0x%x \n", (int)visinfo->visualid);
			printf("visual %p \n", visinfo->visual);
			printf("screen %d \n", (int)visinfo->screen);
			printf("depth %d \n", (int)visinfo->depth);
			printf("red_mask 0x%x \n", (int)visinfo->red_mask);
			printf("green_mask 0x%x \n", (int)visinfo->green_mask);
			printf("blue_mask 0x%x \n", (int)visinfo->blue_mask);
			printf("colormap_size %d \n", (int)visinfo->colormap_size);
			printf("bits_per_rgb %d \n", (int)visinfo->bits_per_rgb);
			printf("================================================\n");

			char buff[24];
			sprintf(buff, "0x%x", (int)visinfo->visualid);
			setenv("SDL_VIDEO_X11_VISUALID", buff, 1);
			break;
		}

		XFree(visinfo);
	}

	if (i == numfbconfigs) 
	{
		/* None of the FBConfigs have alpha.  Use a normal (opaque) FBConfig instead */
		printf("None of the FBConfigs have alpha.  Use a normal (opaque) FBConfig instead\n");

		fbconfig = fbconfigs[0];
		visinfo = glXGetVisualFromFBConfig(dpy, fbconfig);
		pictFormat = XRenderFindVisualFormat(dpy, visinfo->visual);
	}

	XFree(fbconfigs);
	XUnlockDisplay(dpy);
	return visinfo;
}
//---------------------------------------------------------------------------------------------------------------
void FillTextProperty(XTextProperty* p, const char* text)
{
  char *list[2];

  list[0] = (char*)text;
  list[1] = NULL;
  XStringListToTextProperty(list,1,p);         /* convert string */
}
//---------------------------------------------------------------------------------------------------------------
bool RegisterReplacementWindowForSDL(Display *dpy, XVisualInfo *visinfo, int nWidth, int nHeight)
{
	XLockDisplay(dpy); 

	if(NULL != visinfo)
	{
		XSetWindowAttributes xattr;
		xattr.override_redirect = True;
		xattr.background_pixel = BlackPixel(dpy, XDefaultScreen(dpy));
		xattr.border_pixel = 0;
		xattr.colormap = XCreateColormap(dpy, RootWindow(dpy, XDefaultScreen(dpy)) , visinfo->visual, AllocNone); ;

		printf("About to create the window ...\n");

		Window FSwindow = XCreateWindow(
			dpy, RootWindow(dpy, XDefaultScreen(dpy)), 
			0, 0, nWidth, nHeight, 
			InputOnly, 
			visinfo->depth, InputOutput, visinfo->visual,
			/*CWOverrideRedirect |*/ CWBackPixel | CWBorderPixel | CWColormap,
			&xattr);

		//display, parent, 
		//x, y, width, height, 
		//border_width, 
		//depth, class, visual, 
		//valuemask, 
		//attributes

		// Tell KDE to keep the fullscreen window on top 
		{
			XEvent ev;
			long mask;

			memset(&ev, 0, sizeof(ev));
			ev.xclient.type = ClientMessage;
			ev.xclient.window = RootWindow(dpy, XDefaultScreen(dpy));
			ev.xclient.message_type = XInternAtom(dpy, "KWM_KEEP_ON_TOP", False);
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = FSwindow;
			ev.xclient.data.l[1] = CurrentTime;
			mask = SubstructureRedirectMask;
			XSendEvent(dpy, RootWindow(dpy, XDefaultScreen(dpy)), False, mask, &ev);
		}

		XWMHints *hints = XAllocWMHints();
		hints->input = True;
		hints->flags = InputHint;
		hints->initial_state = NormalState;
		hints->flags |= StateHint;
		XSetWMHints(dpy, FSwindow, hints);
		XFree(hints); 

		XTextProperty titleprop, iconprop;
		FillTextProperty(&titleprop, "OrbiterGL");
		XSetWMName(dpy, FSwindow, &titleprop);
		XFree(titleprop.value);
		FillTextProperty(&iconprop, "OrbiterGL");
		XSetWMIconName(dpy, FSwindow, &iconprop);
		XFree(iconprop.value);

		XSelectInput(dpy, FSwindow, FocusChangeMask | KeyPressMask | KeyReleaseMask | PropertyChangeMask | StructureNotifyMask | KeymapStateMask);

		// Set the class hints so we can get an icon (AfterStep) 
		{
		XClassHint *classhints;
		classhints = XAllocClassHint();
			if(classhints != NULL) 
			{
				classhints->res_name = "Orbiter";
				classhints->res_class = "Orbiter";
				XSetClassHint(dpy, FSwindow, classhints);
				XFree(classhints);
			}
		}

		Atom atom = None;

		atom = XInternAtom (dpy, "_NET_WM_ACTION_CLOSE", False);
		XChangeProperty (dpy, FSwindow, XInternAtom (dpy,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (dpy, "_NET_WM_ACTION_CHANGE_DESKTOP", False);
		XChangeProperty (dpy, FSwindow, XInternAtom (dpy,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (dpy, "_NET_WM_ACTION_STICK", False);
		XChangeProperty (dpy, FSwindow, XInternAtom (dpy,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (dpy, "_NET_WM_ACTION_SHADE", False);
		XChangeProperty (dpy, FSwindow, XInternAtom (dpy,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 

		atom = XInternAtom (dpy, "_WIN_HINTS", False);
		XChangeProperty (dpy, FSwindow, XInternAtom (dpy,"_WIN_HINTS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 

		Atom WM_HINTS = XInternAtom(dpy, "_WIN_HINTS", True);
		if(WM_HINTS != None) 
		{
			long lWM_Hints = 0;
			XChangeProperty(dpy, FSwindow, WM_HINTS, WM_HINTS, 32, PropModeReplace, (unsigned char *)&lWM_Hints, sizeof(lWM_Hints)/4);
		}

		Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(dpy, FSwindow, &WM_DELETE_WINDOW, 1); 

		Atom wm_state = XInternAtom(dpy, "WM_STATE", True);
		unsigned long data_wm_state[2];
		data_wm_state[0] = (unsigned long) 1;
		data_wm_state[1] = (unsigned long) None;
		XChangeProperty(dpy, FSwindow, wm_state, wm_state, 32, PropModeReplace, (unsigned char *)data_wm_state, 2);

		XMapWindow(dpy, FSwindow);

		char buff[24];
		sprintf(buff, "0x%x", (int)FSwindow);
		printf("Window ID to use 0x%x\n", (int)FSwindow);
		setenv("SDL_WINDOWID", buff, 1);

		XUnlockDisplay(dpy);
		return true;
	}

	XUnlockDisplay(dpy);
	return false;
}
//---------------------------------------------------------------------------------------------------------------
