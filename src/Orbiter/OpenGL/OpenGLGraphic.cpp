#include "OpenGLGraphic.h"
#include "Logger.h"

#include "SDL_rotozoom.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#include "SDLRendererOCGHelper.h"
#include "PlutoSDLDefs.h"

#include "../Orbiter.h"

//-------------------------------------------------------------------------------------------------------
OpenGLGraphic::OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement,
					   Orbiter *pOrbiter)
					   : PlutoGraphic(Filename, GraphicManagement, pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
OpenGLGraphic::OpenGLGraphic(Orbiter *pOrbiter)
: PlutoGraphic(pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
OpenGLGraphic::OpenGLGraphic(struct SDL_Surface *pSDL_Surface)
{
	Initialize();

	m_pSDL_Surface = pSDL_Surface;
    if (m_pSDL_Surface == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"OpenGLGraphic::OpenGLGraphic() : NULL m_pSDL_Surface");
        return;
    }
	Width = m_pSDL_Surface->w;
	Height = m_pSDL_Surface->h;
}
//-------------------------------------------------------------------------------------------------------
OpenGLGraphic::~OpenGLGraphic()
{
	Clear();
}
//-------------------------------------------------------------------------------------------------------
void OpenGLGraphic::Initialize()
{
	m_pSDL_Surface = NULL;
}
//-------------------------------------------------------------------------------------------------------
bool OpenGLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	if(m_GraphicFormat == GR_OCG)
	{
		string sErrorMessage = 
			"Cannot load OCG files in Orbiter SDL. "
			"Please uncheck 'Use OCG' device data for this device (" + 
				StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + ").";

        g_pPlutoLogger->Write(LV_CRITICAL, sErrorMessage.c_str());
		m_pOrbiter->PromptUser(sErrorMessage.c_str(), 100);
        exit(1);
		return false;
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

 /*   if(iRotation)
    {
        SDL_Surface *pSourceSurface = m_pSDL_Surface;
        m_pSDL_Surface = rotozoomSurface(pSourceSurface, iRotation, 1, SMOOTHING_ON);
        SDL_FreeSurface(pSourceSurface);
    }*/

	Width = m_pSDL_Surface->w;
	Height = m_pSDL_Surface->h;

	return true;
}
//-------------------------------------------------------------------------------------------------------
void OpenGLGraphic::Clear()
{
	if (m_pSDL_Surface)
	{
		SDL_FreeSurface(m_pSDL_Surface);
		m_pSDL_Surface = NULL;
	}
}
//-------------------------------------------------------------------------------------------------------

PlutoGraphic *OpenGLGraphic::GetHighlightedVersion()
{
    if (m_pSDL_Surface == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"OpenGLGraphic::GetHighlightedVersion() : NULL m_pSDL_Surface");
        return NULL;
    }
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

	return new OpenGLGraphic(pSDL_Surface);
}

//-----------------------------------------------------------------------------------------------------
Uint32 OpenGLGraphic::getpixel(SDL_Surface *pSDL_Surface,int x, int y)
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
void OpenGLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
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
        * (Uint32 *) pixel = pixel_color | 0xFF000000; //opaque
        break;
    }
}

PlutoGraphic* OpenGLGraphic::Clone()
{
	SDL_Surface* Surface = SDL_DisplayFormat(this->m_pSDL_Surface);
	return new OpenGLGraphic(Surface);
}

bool OpenGLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{	
	//TODO : implement me!
    pRawBitmapData = NULL;	
	ulSize = 0;
	return false;
}
