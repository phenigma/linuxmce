#include "SDLGraphic.h"
#include "Logger.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>
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
	m_pImage = NULL; 
}
//-------------------------------------------------------------------------------------------------------
bool SDLGraphic::LoadGraphic(string sFileName)
{
	m_pSDL_Surface = IMG_Load(sFileName.c_str());
	if( !m_pSDL_Surface )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read file %s", (sFileName).c_str());
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
bool SDLGraphic::LoadGraphic(char *pData, size_t iSize)
{
	SDL_RWops * rw = SDL_RWFromMem(pData, iSize);
	m_pSDL_Surface = IMG_Load_RW(rw, 1); // rw is freed here

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
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if (m_pSDL_Surface)
	{
		SDL_FreeSurface(m_pSDL_Surface);
		m_pSDL_Surface = NULL;
	}
}
//-------------------------------------------------------------------------------------------------------