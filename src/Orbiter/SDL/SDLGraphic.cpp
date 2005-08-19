#include "SDLGraphic.h"
#include "Logger.h"

#include "SDL_rotozoom.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#include "SDLRendererOCGHelper.h"
#include "PlutoSDLDefs.h"

//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(string Filename, eGraphicManagement GraphicManagement,
					   Orbiter *pOrbiter)
					   : PlutoGraphic(Filename, GraphicManagement, pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(Orbiter *pOrbiter)
: PlutoGraphic(pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(struct SDL_Surface *pSDL_Surface)
{
	Initialize();

	m_pSDL_Surface = pSDL_Surface;
	Width = m_pSDL_Surface->w;
	Height = m_pSDL_Surface->h;
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::~SDLGraphic()
{
	Clear();
}
//-------------------------------------------------------------------------------------------------------
void SDLGraphic::Initialize()
{
	m_pSDL_Surface = NULL;
}
//-------------------------------------------------------------------------------------------------------
bool SDLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	if(m_GraphicFormat == GR_OCG)
	{
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot load ocg files with PocketFrog surfaces in Orbiter SDL. Please check 'Use_OCG' device data for this device. Exiting...");
        exit(1);
	}
	else
	{
		SDL_RWops * rw = SDL_RWFromMem(pData, int(iSize));
		m_pSDL_Surface = IMG_Load_RW(rw, 1); // rw is freed here
	}

	if( !m_pSDL_Surface )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read graphic from data %p with size %d", pData, iSize);
		return false;
	}

    if(iRotation)
    {
        SDL_Surface *pSourceSurface = m_pSDL_Surface;
        m_pSDL_Surface = rotozoomSurface(pSourceSurface, iRotation, 1, SMOOTHING_ON);
        SDL_FreeSurface(pSourceSurface);
    }

	Width = m_pSDL_Surface->w;
	Height = m_pSDL_Surface->h;

	return true;
}
//-------------------------------------------------------------------------------------------------------
void SDLGraphic::Clear()
{
	if (m_pSDL_Surface)
	{
		SDL_FreeSurface(m_pSDL_Surface);
		m_pSDL_Surface = NULL;
	}
}
//-------------------------------------------------------------------------------------------------------

PlutoGraphic *SDLGraphic::GetHighlightedVersion()
{
	if( m_pSDL_Surface->format->BytesPerPixel!=4 )
		return NULL;

	SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
		Width, Height, 32, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = 0; SourceRect.y = 0;
	SourceRect.w = Width; SourceRect.h = Height;

	SDL_SetAlpha(m_pSDL_Surface, 0, 0);

	for(int w=0;w<Width;w++)
	{
		for(int h=0;h<Height;h++)
		{
		    unsigned char *pS = (unsigned char *) m_pSDL_Surface->pixels + h * m_pSDL_Surface->pitch + w * 4;
		    unsigned char *pD = (unsigned char *) pSDL_Surface->pixels + h * pSDL_Surface->pitch + w * 4;
			pD[0] = min(255,pS[0] + 30);
			pD[1] = min(255,pS[1] + 30);
			pD[2] = min(255,pS[2] + 30);
			pD[3] = 255;
		}
	}

	return new SDLGraphic(pSDL_Surface);
}

//-----------------------------------------------------------------------------------------------------
Uint32 SDLGraphic::getpixel(SDL_Surface *pSDL_Surface,int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

//-----------------------------------------------------------------------------------------------------
void SDLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color;
        break;
    }
}
