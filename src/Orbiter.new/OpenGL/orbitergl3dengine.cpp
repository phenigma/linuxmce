//
// C++ Implementation: orbitergl3dengine 
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "orbitergl3dengine.h"
#include "../Orbiter.h"
#ifdef POCKETFROG
#include "../PocketFrog/OrbiterRenderer_PocketFrog.h"
#include "../PocketFrog/OpenGLProxy.h"
#else
#include "SDL.h"
#include "../SDL/OrbiterSDL.h"
#include "../SDL/SDLGraphic.h"
#endif

#include <GL/gl.h>

#include "DCE/Logger.h"

using namespace DCE;

#include "Orbiter3DCommons.h"

#ifndef WIN32 // linux
#include "../Linux/OrbiterLinux.h"
#endif // linux

OrbiterGL3D* OrbiterGL3D::Instance = NULL;

OrbiterGL3D::OrbiterGL3D ()
	: Screen3D(NULL),
	  HighLighArea(NULL),
	  SelectedArea(NULL),
	  Widgets(NULL),
	  EffectBuilder(NULL)
{
	Instance = this;
}

void OrbiterGL3D::Paint()
{
	if(Screen3D == NULL)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"NULL pointer in declaration of 3D widget... something is really wrong here!");
		exit(-1);
	}
	
	EffectBuilder->Widgets->SetUpNextDisplay();
	EffectBuilder->UpdateEffects();

	Screen3D->SetBackgroundColor(0.8f, 0.8f, 0.8f, 1.0f);
	Screen3D->SetTextureWraping(Widgets->UVRect.Left, Widgets->UVRect.Top, 
		Widgets->UVRect.Width, Widgets->UVRect.Height);

	Screen3D->SetVisible(true);
	HighLighArea->SetVisible(true);
	HighLighArea->SetBackgroundColor(1.0f, 1.0f, 0.0f, 0.2f);

	SelectedArea->SetVisible(true);
	SelectedArea->SetBackgroundColor(1.0f, 0.0f, 0.0f, 0.2f);

	MouseCursor->SetVisible(false);
	//MouseCursor->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
	MouseCursor->SetTextureWraping(0, 0, 1, 1);

	EffectBuilder->Paint();
	Flip();
}
	
OrbiterGL3D::~OrbiterGL3D()
{
	delete Widgets;
	delete EffectBuilder;

	ReleaseOpenGL();
}

void OrbiterGL3D::BeginAnimation()
{
#ifdef POCKETFROG

#endif
}

void OrbiterGL3D::EndAnimation()
{
#ifdef POCKETFROG
	/*
	OrbiterRenderer_PocketFrog *pOrbiterRenderer_PocketFrog = dynamic_cast<OrbiterRenderer_PocketFrog *>(Renderer->GetOrbiter()->);
	ReleaseDC(pOrbiterRenderer_PocketFrog->m_hWnd, hdc);
	OpenGLProxy::GetInstance()->Detach();
	*/
#endif
}

void OrbiterGL3D::Flip()
{
#ifndef POCKETFROG

    if (Renderer)
        Renderer->X_LockDisplay();
    SDL_GL_SwapBuffers();
    if (Renderer)
        Renderer->X_UnlockDisplay();

#else // ndef POCKETFROG
	SwapBuffers(hdc);
#endif // ndef POCKETFROG
}

int OrbiterGL3D::InitOpenGL()
{
#ifndef POCKETFROG
	//initializing the engine...
	Uint32 uSDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

	if (SDL_Init(uSDLInitFlags) == -1)
	{
#ifndef WINCE
		cerr << "Failed initializing SDL: " << SDL_GetError() << endl;
#else
		printf("Failed to initialize SDL %s\n", SDL_GetError());
#endif //WINCE

    // TODO: move in StartOrbiterSDL.cpp : init_system()
#ifndef WIN32 //linux
        string sCmd = "/usr/pluto/bin/Start_X.sh";//; /usr/pluto/bin/Start_WM.sh";
        g_pPlutoLogger->Write(LV_CRITICAL, "X is not running! Starting X and the window manager: %s", sCmd.c_str());
        system(sCmd.c_str());
#endif //linux

		exit(1);
	}

	SDL_WM_SetCaption("OrbiterSDL", "OrbiterSDL");

	atexit(SDL_Quit);
	g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	Uint32 uVideoModeFlags = SDL_OPENGL | SDL_RESIZABLE;

	OrbiterSDL * pRenderer = (OrbiterSDL *)Renderer;
#if !defined(WIN32) || defined(WINCE)
	if(Renderer->m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
#endif
	
	if ((pRenderer->Screen = SDL_SetVideoMode(Renderer->GetOrbiter()->m_iImageWidth, Renderer->GetOrbiter()->m_iImageHeight, 
		0, uVideoModeFlags)) == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, 
			"Failed to set video mode (%d x %d): %s", 
			Renderer->GetOrbiter()->m_iImageWidth, Renderer->GetOrbiter()->m_iImageHeight,
			SDL_GetError());
		exit(1);
	}
	g_pPlutoLogger->Write(LV_STATUS, "Set video mode to %d x %d Window.", 
		Renderer->GetOrbiter()->m_iImageWidth, Renderer->GetOrbiter()->m_iImageHeight);

	g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");

#else //#ifdef POCKETFROG
		OrbiterRenderer_PocketFrog *pRenderer = dynamic_cast<OrbiterRenderer_PocketFrog *>(Renderer); 
		// remember the window handle (HWND)
		HWND mhWnd = pRenderer->m_hWnd;

		// get the device context (DC)
		this->hdc = GetDC( mhWnd );

		// set the pixel format for the DC
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory( &pfd, sizeof( pfd ) );
		pfd.nSize = sizeof( pfd );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int format = ChoosePixelFormat( hdc, &pfd );
		SetPixelFormat( hdc, format, &pfd );

		// create the render context (RC)
		HGLRC mhRC = wglCreateContext( hdc );

		// make it the current render context
		wglMakeCurrent( hdc, mhRC );

#endif //#ifndef POCKETFROG

	/**
	OpenGL starting code 
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);  // Change Matrix Mode to Projection
	glLoadIdentity();             // Reset View
	glOrtho(0, Renderer->GetOrbiter()->m_iImageWidth, 0, 
		Renderer->GetOrbiter()->m_iImageHeight, 0, Renderer->GetOrbiter()->m_iImageWidth);
	glMatrixMode(GL_MODELVIEW);   // Change Projection to Matrix Mode
	glLoadIdentity();

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();

	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);

	glScalef(1, -1.0f, 1);
	glTranslatef(0, -(GLfloat)Renderer->GetOrbiter()->m_iImageHeight, 0);


    return 1;
}

void OrbiterGL3D::ReleaseOpenGL()
{
#ifndef POCKETFROG
	SDL_Quit();
#endif
}

OrbiterGL3D* OrbiterGL3D::GetInstance()
{
	if(Instance)
		return Instance;
	return new OrbiterGL3D();
}

int OrbiterGL3D::BuildOrbiterGL(OrbiterRenderer *pRenderer)
{
	Renderer = pRenderer;
	FloatRect FullScreenSize(0.0f, 0.0f, float(Renderer->GetOrbiter()->m_iImageWidth), float(Renderer->GetOrbiter()->m_iImageHeight));

	if (!InitOpenGL())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL cannot start! Do you have OpenGL drivers installed on your system? %s", 
			"<<Fails on OrbiterGL3D::InitOpenGL()>>");
		return 0;
	}

	Widgets = new DrawingWidgetsEngine(
		Renderer->GetOrbiter()->m_iImageWidth, 
		Renderer->GetOrbiter()->m_iImageHeight
		);

	if(!Widgets)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation Widgets component>>");
		return 0;
	}

	EffectBuilder = new GL2DEffectFactory(Widgets);

	if(!EffectBuilder)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation EffectBuilder component>>");
		return 0;
	}

	Screen3D = dynamic_cast<TBasicWindow*>(Widgets->CreateWidget(
		BASICWINDOW, 
		0, 0, 
		(int)FullScreenSize.Width,
		(int)FullScreenSize.Height, 
		"None"));

	if(!Screen3D)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation Screen3D component>>");
		return 0;
	}

	
	this->HighLighArea = dynamic_cast<TBasicWindow*>(Widgets->CreateWidget(
		BASICWINDOW, 
		0, 0, 
		(int)0,
		(int)0, 
		"None"));

	if(!HighLighArea)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation HighLighArea component>>");
		return 0;
	}

	this->SelectedArea= dynamic_cast<TBasicWindow*>(Widgets->CreateWidget(
		BASICWINDOW, 
		0, 0, 
		(int)0,
		(int)0, 
		"None"));
	if(!SelectedArea)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation SelectedArea component>>");
		return 0;
	}

	this->MouseCursor= dynamic_cast<TBasicWindow*>(Widgets->CreateWidget(
		BASICWINDOW, 
		0, 0, 
		(int)64,
		(int)64, 
		"None"));
	if(!MouseCursor)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
			"Alert: OPENGL started but there is no memory to create attached environment for it! %s", 
			"<<Fails on creation SelectedArea component>>");
		return 0;
	}

	Commons3D::Instance().BuildCommons(EffectBuilder, Screen3D, HighLighArea, SelectedArea, MouseCursor);
	return 1;
}
