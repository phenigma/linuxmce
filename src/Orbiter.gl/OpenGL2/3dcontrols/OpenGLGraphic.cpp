#include "OpenGLGraphic.h"

#include <SDL_image.h>

OpenGLGraphic::OpenGLGraphic()
{
}

OpenGLGraphic::~OpenGLGraphic()
{
}

bool OpenGLGraphic::SetupFromImage(std::string FileName)
{
	SDL_Surface *Surface = IMG_Load(FileName.c_str());

	if(Surface == NULL)
		return false;
	/* Create The Texture */
	
	glGenTextures( 1, &Texture);
	
	/* Typical Texture Generation Using Data From The Bitmap */
	glBindTexture( GL_TEXTURE_2D, Texture );
	
	/* Generate The Texture */
	glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			Surface->w, Surface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			Surface->pixels);

	SDL_FreeSurface(Surface);
	
	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	return true;
}
