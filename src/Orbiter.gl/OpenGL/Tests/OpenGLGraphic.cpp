#include "OpenGLGraphic.h"

#include <SDL_image.h>

#include "GLMathUtils.h"

#include <iostream>

OpenGLGraphic::OpenGLGraphic()
{
	MaxU = 1.0f;
	MaxV = 1.0f;
	Texture = 0;
}

OpenGLGraphic::~OpenGLGraphic()
{
}

bool OpenGLGraphic::SetupFromImage(std::string FileName)
{
	SDL_Surface *Surface = IMG_Load(FileName.c_str());

	Convert(Surface);
	
	return true;
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	static Uint32 rmask = 0xff000000;
	static Uint32 gmask = 0x00ff0000;
	static Uint32 bmask = 0x0000ff00;
	static Uint32 amask = 0x000000ff;
#else
	static Uint32 rmask = 0x000000ff;
	static Uint32 gmask = 0x0000ff00;
	static Uint32 bmask = 0x00ff0000;
	static Uint32 amask = 0xff000000;
#endif


void OpenGLGraphic::Convert(SDL_Surface* Surface)
{
	
	Width = GLMathUtils::MinPowerOf2(Surface->w);
	Height = GLMathUtils::MinPowerOf2(Surface->h);
	
	MaxU = ((float)Surface->w)/Width;
	MaxV = ((float)Surface->h)/Height;
	
	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */
	SDL_Surface *Result = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, Surface->format->BitsPerPixel, 
		rmask, gmask, bmask, amask);
		
	if(Result == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return;
	}
	
	SDL_BlitSurface(Surface, NULL, Result, NULL);

	Width = Surface->w;
	Height = Surface->h;
		
	/* Typical Texture Generation Using Data From The Bitmap */
	OpenGLTexture FinalTexture;
	glGenTextures( 1, &FinalTexture);
	glBindTexture(GL_TEXTURE_2D, FinalTexture);
	if(Result->format->BytesPerPixel == 4)
		/* Generate The Texture */
		glTexImage2D( GL_TEXTURE_2D, 
		0, 3, 
		Result->w, Result->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		Result->pixels );
	else
	/* Generate The Texture */
	glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			Result->w, Result->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			Result->pixels );

	SDL_FreeSurface(Result);
	
	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	Texture = FinalTexture;
}

void OpenGLGraphic::ReleaseTexture()
{
	glDeleteTextures(1, &Texture);
	Texture = 0;
	Width = 0;
	Height = 0;
	MaxU = 0.0f;
	MaxV = 0.0f;
}
