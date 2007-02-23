/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "GraphicImage.h"

#include "SDL_image.h"
#include "MathUtils.h"

GraphicImage::GraphicImage()
: LocalSurface(NULL), Texture(0)
{

}
GraphicImage::~GraphicImage()
{
	if(LocalSurface)
		SDL_FreeSurface(LocalSurface);
	ReleaseTexture();
}

bool GraphicImage::Load(string FileName)
{
	LocalSurface = IMG_Load(FileName.c_str()); 
	if (LocalSurface == NULL)
		return false;
	ScaleImage();
	Convert();

	return true;
}

void GraphicImage::Convert(void)
{
	if(LocalSurface == NULL)
		return;
	ReleaseTexture();

	/* Typical Texture Generation Using Data From The Bitmap */
	glGenTextures( 1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	if(LocalSurface->format->BytesPerPixel == 4)
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
		0, 4, 
		LocalSurface->w, LocalSurface->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		LocalSurface->pixels );
	else
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
		0, 3, 
		LocalSurface->w, LocalSurface->h, 
		0, GL_RGB,
		GL_UNSIGNED_BYTE, 
		LocalSurface->pixels );

	SDL_FreeSurface(LocalSurface);
	LocalSurface = NULL;

	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
}

void GraphicImage::ReleaseTexture(void)
{
	glDeleteTextures(1, &Texture);
	Texture = 0;
}

void GraphicImage::ScaleImage()
{
	if(LocalSurface == NULL)
		return;

	SDL_Surface* Surface = LocalSurface;
	Width = MinPowerOf2(Surface->w);
	Height = MinPowerOf2(Surface->h);

	if (Width == Surface->w && Height == Surface->h)
		return;

	MaxU = ((float)Surface->w)/Width;
	MaxV = ((float)Surface->h)/Height;

	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */

	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
		Surface->format->BitsPerPixel, 
		Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask,
		Surface->format->Amask);

	if(LocalSurface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return;
	}

	//TODO: check if LocalSurface is transparent after blit
	//SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL , SDL_ALPHA_TRANSPARENT);

	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);
	SDL_SaveBMP(Surface, "/Test.bmp");

	SDL_FreeSurface(Surface);  
}
