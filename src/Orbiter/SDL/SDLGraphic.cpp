#include "SDLGraphic.h"
#include "Logger.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#include "SDLRendererOCGHelper.h"
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
bool SDLGraphic::LoadGraphic(char *pData, size_t iSize)
{
	if(m_GraphicFormat == GR_OCG)
	{
		m_pSDL_Surface = SDL_LoadOCG(pData, iSize);
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
