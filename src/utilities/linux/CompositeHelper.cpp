#include "CompositeHelper.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------------------------------------------
XVisualInfo *GetVisualForComposite(Display *dpy)
{
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
	if(!XRenderQueryExtension(dpy, &render_event_base, &render_error_base)) {
		printf("No RENDER extension found\n");
		return NULL;
	}

	/* Get the list of FBConfigs that match our criteria */
	fbconfigs = glXChooseFBConfig(dpy, DefaultScreen(dpy), attrib, &numfbconfigs);
	if (!fbconfigs) 
	{
		printf("None matched\n");
		/* None matched */
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
			printf("Found visual 0x%x \n", visinfo->visualid);
			printf("visual %p \n", visinfo->visual);
			printf("screen %d \n", visinfo->screen);
			printf("depth %d \n", visinfo->depth);
			printf("red_mask 0x%x \n", visinfo->red_mask);
			printf("green_mask 0x%x \n", visinfo->green_mask);
			printf("blue_mask 0x%x \n", visinfo->blue_mask);
			printf("colormap_size %d \n", visinfo->colormap_size);
			printf("bits_per_rgb %d \n", visinfo->bits_per_rgb);
			printf("================================================\n");

			char buff[24];
			sprintf(buff, "0x%x", visinfo->visualid);
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

	return visinfo;
}
//---------------------------------------------------------------------------------------------------------------
bool RegisterReplacementWindowForSDL(Display *dpy, XVisualInfo *visinfo, int nWidth, int nHeight)
{
	if(NULL != visinfo)
	{
		XSetWindowAttributes xattr;
		XWMHints *hints;
		XTextProperty titleprop, iconprop;
		xattr.override_redirect = True;
		xattr.background_pixel = BlackPixel(dpy, XDefaultScreen(dpy));
		xattr.border_pixel = 0;
		xattr.colormap = XCreateColormap(dpy, RootWindow(dpy, XDefaultScreen(dpy)) , visinfo->visual, AllocNone); ;

		printf("About to create the window ...\n");

		Window FSwindow = XCreateWindow(dpy, RootWindow(dpy, XDefaultScreen(dpy)),
			0, 0, nWidth, nHeight, 1,
			visinfo->depth, InputOutput, visinfo->visual,
			CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWColormap,
			&xattr); 

        // We want to get MapNotify events
        XSelectInput(dpy, FSwindow, StructureNotifyMask);

		XMapWindow(dpy, FSwindow);

		char buff[24];
		sprintf(buff, "0x%x", (int)FSwindow);
		printf("Window ID to use 0x%x\n", (int)FSwindow);
		setenv("SDL_WINDOWID", buff, 1);
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------------------------
