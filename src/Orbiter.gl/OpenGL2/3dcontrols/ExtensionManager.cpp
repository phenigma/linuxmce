#include "ExtensionManager.h"

#include <SDL.h>
#include <GL/gl.h>

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
	glClearColor(CleanRed, CleanGreen, CleanBlue, 1.0f);
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

