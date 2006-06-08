#include "ExtensionManager.h"

#ifdef WIN32
#define LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

ExtensionManager::ExtensionManager(void)
{
}

ExtensionManager::~ExtensionManager(void)
{
}

/*virtual*/ void ExtensionManager::SetClearColor(float Red, float Green, float Blue)
{
	this->CleanRed = Red;
	this->CleanGreen = Green;
	this->CleanBlue = Blue;
}

/*virtual*/ void ExtensionManager::ClearScreen(bool CleanColor, bool CleanZBuffer)
{
	if(!CleanColor && !CleanZBuffer)
		return;
	glClearColor(CleanRed, CleanGreen, CleanBlue, 0.0f);
	unsigned int uFlags = 0;
	if(CleanColor)
		uFlags |= GL_COLOR_BUFFER_BIT;
	if(CleanZBuffer)
		uFlags |= GL_DEPTH_BUFFER_BIT;
		
	glClear(uFlags);
	
}

/*virtual*/ void ExtensionManager::EnableZBuffer(bool Enable)
{
	if(Enable)
		glEnable(GL_DEPTH_TEST);
	else	
		glDisable(GL_DEPTH_TEST);
}

/*virtual*/ void ExtensionManager::Flip()
{
	SDL_GL_SwapBuffers();
}

void ExtensionManager::Resize(int Width, int Height)
{
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
}

bool ExtensionManager::InitVideoMode(int Width, int Height, int Bpp, bool FullScreen)
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

	Resize(Width, Height);	

	glEnable(GL_TEXTURE_2D);

	return Screen != NULL;
}

