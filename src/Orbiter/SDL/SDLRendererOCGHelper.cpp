#include "SDLRendererOCGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoSDLDefs.h"
#include "../SerializeClass/ShapesColors.h"

#include <SDL.h>
//----------------------------------------------------------------------------------------------------------------
#define RED_MASK_16		(0x1F << 11)
#define GREEN_MASK_16	(0x3F << 5)
#define BLUE_MASK_16	0x1F
//----------------------------------------------------------------------------------------------------------------
inline Uint16 make16bcolor( unsigned char red, unsigned char green, unsigned char blue )
{
	return (Uint16) (((red << 8) & RED_MASK_16) | ((green << 3) & GREEN_MASK_16) | (blue >> 3));
}
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(char *pOCGData, size_t iOCGDataSize)
{
	RendererOCG *pRendererOCG = new RendererOCG();
	pRendererOCG->SetOCGData(pOCGData, iOCGDataSize);
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
SDL_Surface* SDL_LoadOCG(string sFilename)
{
	size_t iSize;
	char *pData = FileUtils::ReadFileIntoBuffer(sFilename, iSize);

	if(!pData)
		return NULL;

	SDL_Surface *pSurface = SDL_LoadOCG(pData, iSize);

	PLUTO_SAFE_DELETE_ARRAY(pData);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
bool SDL_SaveOCG(SDL_Surface *pSurface, string sFilename, bool bPocket)
{
	int bpp = pSurface->format->BytesPerPixel;

	int iPixelFormatDataSize = sizeof(SDL_PixelFormat);
	char *pPixelFormatData = new char[iPixelFormatDataSize];
	memcpy(pPixelFormatData, pSurface->format, iPixelFormatDataSize);

	int iPixelsDataSize = pSurface->h * pSurface->w * pSurface->format->BytesPerPixel;

	if(bpp == 4 && bPocket)
		iPixelsDataSize /= 2;

	char *pPixelsData = new char[iPixelsDataSize];
	if(!bPocket || (bPocket && bpp == 2) )
	{
		memcpy(pPixelsData, pSurface->pixels, iPixelsDataSize);
	}
	else
	{
		char *pPixelsDataTemp = pPixelsData;
		SDL_PixelFormat * PF = pSurface->format;

		for(int y = 0; y < pSurface->h; y++)
			for(int x = 0; x < pSurface->w; x++)
			{
				Uint8 * pixel = (Uint8 *) pSurface->pixels + y * pSurface->pitch + x * bpp;
				Uint32 color;
				Uint16 _16bit_color;

				if(bpp == 4)
				{
					color = * (Uint32 *) pixel;
					Uint8 *pColor = (Uint8 *)(&color);
					_16bit_color = make16bcolor(pColor[0], pColor[1], pColor[2]);
				}
				else if(bpp == 3)
				{
					if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
						color = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
					else
						color = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

					Uint8 *pColor = (Uint8 *)(&color);
					_16bit_color = make16bcolor(pColor[2], pColor[1], pColor[0]);
				}


				* (Uint16 *)pPixelsDataTemp = _16bit_color;
				pPixelsDataTemp += sizeof(Uint16);
			}
	}

	RendererOCG *pRendererOCG = new RendererOCG(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, 
		pSurface->w, pSurface->h);

	bool bResult = pRendererOCG->SaveOCG(sFilename);
	PLUTO_SAFE_DELETE(pRendererOCG);	

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------