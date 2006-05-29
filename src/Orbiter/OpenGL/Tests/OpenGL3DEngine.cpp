#include "OpenGL3DEngine.h"

#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL_ttf.h>

OpenGL3DEngine::OpenGL3DEngine()
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}
	
}

OpenGL3DEngine::~OpenGL3DEngine()
{
}

bool OpenGL3DEngine::InitVideoMode(int Width, int Height, int Bpp, bool FullScreen)
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)<0)
		return false;
		
	SDL_WM_SetCaption("OrbiterSDL", "OrbiterSDL");

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	Uint32 uVideoModeFlags = SDL_OPENGL | SDL_RESIZABLE;

	#if !defined(WIN32) || defined(WINCE)
		if(FullScreen)
			uVideoModeFlags |= SDL_FULLSCREEN;
	#endif
	
	Screen = SDL_SetVideoMode(Width, Height, Bpp, uVideoModeFlags);
	
	if(Height == 0)
		Height = 1;
	// Set the viewport for the OpenGL window
	glViewport(0, 0, Width, Height);
	// Change Matrix Mode to Projection
	glMatrixMode(GL_PROJECTION);
	// Do the perspective calculations. Last value = max clipping depth
	gluPerspective(90.0, (float)Width/Height, 0.001, 1000);
	// Return to the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	// Reset View
	glLoadIdentity();      
	// Enables Smooth Color Shading
	glShadeModel(GL_SMOOTH);
	//Realy Nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.0);                       // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                 // Enable Depth Buffer
    glDepthFunc(GL_LESS);		           // The Type Of Depth Test To Do

	glEnable(GL_TEXTURE_2D);

	
	return Screen != NULL;
}

void OpenGL3DEngine::Finalize(void)
{
}

