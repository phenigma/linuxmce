//
// C++ Implementation: OpenGLTextureConverter_SDL
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "OpenGLTextureConverter_SDL.h"
#include "SDLGraphic.h"
#include "PlutoSDLDefs.h"

#include "../OpenGL/math3dutils.h"

OpenGLTextureConverter_SDL::OpenGLTextureConverter_SDL(PlutoGraphic *pGraphic) 
	: OpenGLTextureConverter(pGraphic)
{
}

SDL_Surface* OpenGLTextureConverter_SDL::CreateSurfaceTexture(SDL_Surface* Surface)
{
	if (Surface == NULL)
		return NULL;
	int width = MathUtils::MinPowerOf2(Surface->w);
	int height = MathUtils::MinPowerOf2(Surface->h);
	
	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */
	SDL_Surface *Result = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, Surface->format->BitsPerPixel, rmask, gmask, bmask, amask);
					
	if(Result == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return NULL;
	}
	
	SDL_BlitSurface(Surface, NULL, Result, NULL);
	
	return Result;
}

OpenGLTexture OpenGLTextureConverter_SDL::Convert()
{
	OpenGLTexture Result;
	SDL_Surface* SurfaceTexture;
	SDL_Surface *Surface = dynamic_cast<SDLGraphic *>(m_spPlutoGraphic.get())->m_pSDL_Surface;

	if(Surface == NULL)
		return 0;
	/* Create The Texture */
	SurfaceTexture = CreateSurfaceTexture(Surface);
	
	glGenTextures( 1, &Result);
	
	/* Typical Texture Generation Using Data From The Bitmap */
	glBindTexture( GL_TEXTURE_2D, Result );
	if(SurfaceTexture->format->BytesPerPixel == 4)
		/* Generate The Texture */
		glTexImage2D( GL_TEXTURE_2D, 
		0, 3, 
		SurfaceTexture->w, SurfaceTexture->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		SurfaceTexture->pixels );
	else
	/* Generate The Texture */
	glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			SurfaceTexture->w, SurfaceTexture->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			SurfaceTexture->pixels );

	SDL_FreeSurface(SurfaceTexture);
	
	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	return Result;
}
