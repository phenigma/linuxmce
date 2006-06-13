#include "OpenGLGraphic.h"

#include <SDL_image.h>

#include "GLMathUtils.h"
#include "DCE/Logger.h"

#include <iostream>

OpenGLGraphic::OpenGLGraphic() : PlutoGraphic()
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer) : PlutoGraphic(pOrbiterRenderer)
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer)
	: PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer)
{
	Initialize();
}

OpenGLGraphic::~OpenGLGraphic()
{
	if(NULL != LocalSurface)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

}

void OpenGLGraphic::Initialize()
{
	MaxU = 1.0f;
	MaxV = 1.0f;
	LocalSurface = NULL;
	Texture = 0;
	Width = 0;
	Height = 0;
}

bool OpenGLGraphic::SetupFromImage(std::string FileName)
{
	SDL_Surface *Surface = IMG_Load(FileName.c_str());

	Prepare(Surface);
	
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


void OpenGLGraphic::Prepare(SDL_Surface* Surface)
{    
	if(Surface == NULL)
	{
		if (LocalSurface != NULL)
		{
			SDL_FreeSurface(LocalSurface);
			LocalSurface = NULL;
		}
		LocalSurface = Surface;
		return;
	}
	Width = GLMathUtils::MinPowerOf2(Surface->w);
	Height = GLMathUtils::MinPowerOf2(Surface->h);
	
	MaxU = ((float)Surface->w)/Width;
	MaxV = ((float)Surface->h)/Height;
	
	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */
	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, Width, Height, 
		Surface->format->BitsPerPixel, 
		rmask, gmask, bmask, amask);
		
	if(LocalSurface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return;
	}
	
	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);

	Width = Surface->w;
	Height = Surface->h;
		
}

void OpenGLGraphic::Convert()
{
	if( NULL == LocalSurface)
		return;

	if(Texture)
	{
		glDeleteTextures(1, &Texture);
		Texture = 0;
	}

	/* Typical Texture Generation Using Data From The Bitmap */
	OpenGLTexture FinalTexture;
	glGenTextures( 1, &FinalTexture);
	glBindTexture(GL_TEXTURE_2D, FinalTexture);

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

	DCE::g_pPlutoLogger->Write(LV_STATUS, "Freeing surface %p" , LocalSurface);

	SDL_FreeSurface(LocalSurface);
	LocalSurface = NULL;
		
	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	Texture = FinalTexture;
}

PlutoGraphic* OpenGLGraphic::Clone()
{
	return NULL;
}

GraphicType OpenGLGraphic::GraphicType_get()
{
	return gtOpenGLGraphic;
}

bool OpenGLGraphic::IsEmpty()
{
	return LocalSurface == NULL;
}

bool OpenGLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	SDL_RWops * rw = SDL_RWFromMem(pData, int(iSize));
	LocalSurface = IMG_Load_RW(rw, 1); // rw is freed here

	if(LocalSurface)
	{
		Width = LocalSurface->w;
		Height = LocalSurface->h;
	}

	return LocalSurface != NULL;
}

void OpenGLGraphic::Clear()
{

}

bool OpenGLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{
	pRawBitmapData = NULL;
	ulSize = 0;
	return false;
}
