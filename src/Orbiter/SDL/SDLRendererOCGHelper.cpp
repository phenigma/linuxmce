#include "SDLRendererOCGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoSDLDefs.h"
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(string sFilename)
{
	RendererOCG *pRendererOCG = new RendererOCG(sFilename);
	SDL_Surface *pSurface = NULL;

	char *pPixelsData;
	size_t iPixelsDataSize;
	char *pPixelFormatData;
	size_t iPixelFormatDataSize;
	int iWidth;
	int iHeigth;

	if(pRendererOCG->GetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeigth))
	{
		pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, iWidth, iHeigth, 32, rmask, gmask, bmask, amask);

		delete pSurface->pixels;
		pSurface->pixels = new char[iPixelsDataSize];
		memcpy((char *)pSurface->pixels, pPixelsData, iPixelsDataSize);

		delete (SDL_PixelFormat *)pSurface->format; 
		pSurface->format = (SDL_PixelFormat *)(new char[iPixelFormatDataSize]);
		memcpy((char *)pSurface->format, pPixelFormatData, iPixelFormatDataSize);
	}

	PLUTO_SAFE_DELETE(pRendererOCG);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
bool SDL_SaveOCG(SDL_Surface *pSurface, string sFilename)
{
	int iPixelsDataSize = pSurface->h * pSurface->w * pSurface->format->BytesPerPixel;
	char *pPixelsData = new char[iPixelsDataSize];
	memcpy(pPixelsData, pSurface->pixels, iPixelsDataSize);

	int iPixelFormatDataSize = sizeof(SDL_PixelFormat);
	char *pPixelFormatData = new char[iPixelFormatDataSize];
	memcpy(pPixelFormatData, pSurface->format, iPixelFormatDataSize);

	RendererOCG *pRendererOCG = new RendererOCG(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, 
		pSurface->w, pSurface->h);

	bool bResult = pRendererOCG->SaveOCG(sFilename);
	PLUTO_SAFE_DELETE(pRendererOCG);	

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------