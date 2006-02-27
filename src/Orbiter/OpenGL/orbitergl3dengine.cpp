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
#include "../PocketFrog/Orbiter_PocketFrog.h"
#include "../PocketFrog/OpenGLProxy.h"
#else
#include "SDL.h"
#include "../SDL/OrbiterSDLGL.h"
#include "../SDL/SDLGraphic.h"
#endif

#include <GL/gl.h>

#include "DCE/Logger.h"

using namespace DCE;

OrbiterGL3D::OrbiterGL3D (Orbiter * pOrbiterGL)
	: Screen3D(NULL),
	  Widgets(NULL),
	  EffectBuilder(NULL)  
{
	this->pOrbiterGL = pOrbiterGL;
	FloatRect FullScreenSize(0.0f, 0.0f, float(pOrbiterGL->m_iImageWidth), float(pOrbiterGL->m_iImageHeight));
	
	Widgets = new DrawingWidgetsEngine(
		pOrbiterGL->m_iImageWidth, 
		pOrbiterGL->m_iImageHeight
		);

	InitOpenGL();
	
	EffectBuilder = new GL2DEffectFactory(Widgets);
	
	Screen3D = (TBasicWindow *)/*dynamic_cast<TBasicWindow*>*/(Widgets->CreateWidget(
		BASICWINDOW, 
		0, 0, 
		(int)FullScreenSize.Width,
		(int)FullScreenSize.Height, 
		"None"));
}

void OrbiterGL3D::Paint()
{
	srand((unsigned int)time(NULL));

	if(Screen3D == NULL)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"NULL pointer in declaration of 3D widget... something is really wrong here!");
		exit(234);
	}
	
	EffectBuilder->UpdateEffects();
	if (EffectBuilder->HasEffects())
		Screen3D->SetVisible(false);
	else
	{
		auto_ptr<PlutoGraphic> spPlutoGraphic;
#ifdef POCKETFROG
		spPlutoGraphic.reset(
			new PocketFrogGraphic(
				dynamic_cast<Orbiter_PocketFrog *>(pOrbiterGL)->GetOrbiterDisplay()->GetBackBuffer()
			)
		);
#else
		spPlutoGraphic.reset(
			new SDLGraphic(
				dynamic_cast<OrbiterSDLGL *>(pOrbiterGL)->m_pScreenImage
			)
		);
#endif
		Screen3D->SetUpTextureFromSurface(spPlutoGraphic.get());
		Screen3D->SetVisible(true);

		spPlutoGraphic->Initialize();
	}
		
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
	Orbiter_PocketFrog *pOrbiter_PocketFrog = dynamic_cast<Orbiter_PocketFrog *>(pOrbiterGL);
	OpenGLProxy::GetInstance()->Attach(pOrbiter_PocketFrog->m_hWnd);
	hdc = GetDC(pOrbiter_PocketFrog->m_hWnd);	

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);  // Change Matrix Mode to Projection
	glLoadIdentity();             // Reset View
	glOrtho(0, pOrbiterGL->m_iImageWidth, 0, 
		pOrbiterGL->m_iImageHeight, 0, pOrbiterGL->m_iImageWidth);
	glMatrixMode(GL_MODELVIEW);   // Change Projection to Matrix Mode
	glLoadIdentity();

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();

	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
#endif
}

void OrbiterGL3D::EndAnimation()
{
#ifdef POCKETFROG
	Orbiter_PocketFrog *pOrbiter_PocketFrog = dynamic_cast<Orbiter_PocketFrog *>(pOrbiterGL);
	ReleaseDC(pOrbiter_PocketFrog->m_hWnd, hdc);
	OpenGLProxy::GetInstance()->Detach();
#endif
}

void OrbiterGL3D::Flip()
{
#ifndef POCKETFROG
	SDL_GL_SwapBuffers();
#else
	SwapBuffers(hdc);
#endif
}

void OrbiterGL3D::InitOpenGL()
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

#ifndef WIN32 //linux
		string sCmd = "/usr/pluto/bin/Start_X.sh; /usr/pluto/bin/Start_ratpoison.sh";
		g_pPlutoLogger->Write(LV_CRITICAL, "X is not running! Starting X and ratpoison: %s", sCmd.c_str());
		system(sCmd.c_str());
#endif //linux

		exit(1);
	}

	SDL_WM_SetCaption("OrbiterSDL", "OrbiterSDL");

	atexit(SDL_Quit);
	g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

	Uint32 uVideoModeFlags = SDL_OPENGL;
	uVideoModeFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */

	bool m_bFullScreen = false;
#if !defined(WIN32) || defined(WINCE)
	if(m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
#endif

	OrbiterSDLGL * pOrbiterSDLGL = (OrbiterSDLGL *)this->pOrbiterGL;
	if ((pOrbiterSDLGL->Screen = SDL_SetVideoMode(pOrbiterSDLGL->m_iImageWidth, pOrbiterSDLGL->m_iImageHeight, 
		0, uVideoModeFlags)) == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, 
			"Failed to set video mode (%d x %d): %s", 
			pOrbiterSDLGL->m_iImageWidth, pOrbiterSDLGL->m_iImageHeight,
			SDL_GetError());
		exit(1);
	}
	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	g_pPlutoLogger->Write(LV_STATUS, "Set video mode to %d x %d Window.", 
		pOrbiterSDLGL->m_iImageWidth, pOrbiterSDLGL->m_iImageHeight);

	g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");

	/**
	OpenGL starting code 
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);  // Change Matrix Mode to Projection
	glLoadIdentity();             // Reset View
	glOrtho(0, pOrbiterSDLGL->m_iImageWidth, 0, 
		pOrbiterSDLGL->m_iImageHeight, 0, pOrbiterSDLGL->m_iImageWidth);
	glMatrixMode(GL_MODELVIEW);   // Change Projection to Matrix Mode
	glLoadIdentity();

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();

	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
#endif //#ifndef POCKETFROG
}

void OrbiterGL3D::ReleaseOpenGL()
{
#ifndef POCKETFROG
	SDL_Quit();
#endif
}


