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
CompositeHelper CompositeHelper::m_Instance;
//---------------------------------------------------------------------------------------------------------------
CompositeHelper::CompositeHelper()
{
	m_display = XOpenDisplay(":0.0");
}
//---------------------------------------------------------------------------------------------------------------
CompositeHelper::~CompositeHelper()
{
	XCloseDisplay(m_display);
	m_display = NULL;
}
//---------------------------------------------------------------------------------------------------------------
XVisualInfo *CompositeHelper::GetVisualForComposite()
{
	XLockDisplay(m_display); 

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
	if(!XRenderQueryExtension(m_display, &render_event_base, &render_error_base)) 
	{
		printf("No RENDER extension found\n");
		XUnlockDisplay(m_display);
		return NULL;
	}

	/* Get the list of FBConfigs that match our criteria */
	fbconfigs = glXChooseFBConfig(m_display, DefaultScreen(m_display), attrib, &numfbconfigs);
	if (!fbconfigs) 
	{
		printf("None matched\n");
		/* None matched */
		XUnlockDisplay(m_display);
		return NULL;
	}

	int i = 0;

	printf("Number of FB configs found %d\n", numfbconfigs);

	/* Find an FBConfig with a visual that has a RENDER picture format that
	* has alpha */
	for (i = 0; i < numfbconfigs; i++) 
	{
		visinfo = glXGetVisualFromFBConfig(m_display, fbconfigs[i]);
		if (!visinfo) continue;

		pictFormat = XRenderFindVisualFormat(m_display, visinfo->visual);
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
		visinfo = glXGetVisualFromFBConfig(m_display, fbconfig);
		pictFormat = XRenderFindVisualFormat(m_display, visinfo->visual);
	}

	XFree(fbconfigs);
	XUnlockDisplay(m_display);
	return visinfo;
}
//---------------------------------------------------------------------------------------------------------------
void CompositeHelper::FillTextProperty(XTextProperty* p, const char* text)
{
  char *list[2];

  list[0] = (char*)text;
  list[1] = NULL;
  XStringListToTextProperty(list,1,p);         /* convert string */
}
//---------------------------------------------------------------------------------------------------------------
bool CompositeHelper::RegisterReplacementWindowForSDL(XVisualInfo *visinfo, int nWidth, int nHeight)
{
	XLockDisplay(m_display); 

	if(NULL != visinfo)
	{
		XSetWindowAttributes xattr;
		xattr.override_redirect = True;
		xattr.background_pixel = BlackPixel(m_display, XDefaultScreen(m_display));
		xattr.border_pixel = 0;
		xattr.colormap = XCreateColormap(m_display, RootWindow(m_display, XDefaultScreen(m_display)) , visinfo->visual, AllocNone); ;

		printf("About to create the window ...\n");

		Window FSwindow = XCreateWindow(
			m_display, RootWindow(m_display, XDefaultScreen(m_display)), 
			0, 0, nWidth, nHeight, 
			InputOnly, 
			visinfo->depth, InputOutput, visinfo->visual,
			CWBackPixel | CWBorderPixel | CWColormap,
			&xattr);

		XSelectInput(m_display, FSwindow, 
			EnterWindowMask | LeaveWindowMask | FocusChangeMask | 
			KeyPressMask | KeyReleaseMask | PropertyChangeMask | 
			StructureNotifyMask | KeymapStateMask | ExposureMask | VisibilityChangeMask);

		// Tell KDE to keep the fullscreen window on top 
		{
			XEvent ev;
			long mask;

			memset(&ev, 0, sizeof(ev));
			ev.xclient.type = ClientMessage;
			ev.xclient.window = RootWindow(m_display, XDefaultScreen(m_display));
			ev.xclient.message_type = XInternAtom(m_display, "KWM_KEEP_ON_TOP", False);
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = FSwindow;
			ev.xclient.data.l[1] = CurrentTime;
			mask = SubstructureRedirectMask;
			XSendEvent(m_display, RootWindow(m_display, XDefaultScreen(m_display)), False, mask, &ev);
		}

		XWMHints *hints = XAllocWMHints();
		hints->input = True;
		hints->flags = InputHint;
		hints->initial_state = NormalState;
		hints->flags |= StateHint;
		XSetWMHints(m_display, FSwindow, hints);
		XFree(hints); 

		XTextProperty titleprop, iconprop;
		FillTextProperty(&titleprop, "OrbiterGL");
		XSetWMName(m_display, FSwindow, &titleprop);
		XFree(titleprop.value);
		FillTextProperty(&iconprop, "OrbiterGL");
		XSetWMIconName(m_display, FSwindow, &iconprop);
		XFree(iconprop.value);

		// Set the class hints so we can get an icon (AfterStep) 
		{
		XClassHint *classhints;
		classhints = XAllocClassHint();
			if(classhints != NULL) 
			{
				classhints->res_name = "Orbiter";
				classhints->res_class = "Orbiter";
				XSetClassHint(m_display, FSwindow, classhints);
				XFree(classhints);
			}
		}

		Atom atom = None;

		atom = XInternAtom (m_display, "_NET_WM_ACTION_CLOSE", False);
		XChangeProperty (m_display, FSwindow, XInternAtom (m_display,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (m_display, "_NET_WM_ACTION_CHANGE_DESKTOP", False);
		XChangeProperty (m_display, FSwindow, XInternAtom (m_display,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (m_display, "_NET_WM_ACTION_STICK", False);
		XChangeProperty (m_display, FSwindow, XInternAtom (m_display,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 
		atom = XInternAtom (m_display, "_NET_WM_ACTION_SHADE", False);
		XChangeProperty (m_display, FSwindow, XInternAtom (m_display,"_NET_WM_ALLOWED_ACTIONS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 

		atom = XInternAtom (m_display, "_WIN_HINTS", False);
		XChangeProperty (m_display, FSwindow, XInternAtom (m_display,"_WIN_HINTS",False), XA_ATOM, 32, PropModeAppend, (unsigned char *) &atom, 1); 

		Atom WM_HINTS = XInternAtom(m_display, "_WIN_HINTS", True);
		if(WM_HINTS != None) 
		{
			long lWM_Hints = 0;
			XChangeProperty(m_display, FSwindow, WM_HINTS, WM_HINTS, 32, PropModeReplace, (unsigned char *)&lWM_Hints, sizeof(lWM_Hints)/4);
		}

		Atom WM_DELETE_WINDOW = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(m_display, FSwindow, &WM_DELETE_WINDOW, 1); 

		Atom wm_state = XInternAtom(m_display, "WM_STATE", True);
		unsigned long data_wm_state[2];
		data_wm_state[0] = (unsigned long) 1;
		data_wm_state[1] = (unsigned long) None;
		XChangeProperty(m_display, FSwindow, wm_state, wm_state, 32, PropModeReplace, (unsigned char *)data_wm_state, 2);

		XMapWindow(m_display, FSwindow);

		char buff[24];
		sprintf(buff, "0x%x", (int)FSwindow);
		printf("Window ID to use 0x%x\n", (int)FSwindow);
		setenv("SDL_WINDOWID", buff, 1);

		XUnlockDisplay(m_display);
		return true;
	}

	XUnlockDisplay(m_display);
	return false;
}
//---------------------------------------------------------------------------------------------------------------
bool CompositeHelper::UnregisterReplacementWindowForSDL()
{
	setenv("SDL_WINDOWID", "", 1);
	return true;
}
//---------------------------------------------------------------------------------------------------------------
