#ifndef __SDL_GRAPHIC_H__
#define __SDL_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "SDL.h"
#include "../PlutoGraphic.h"
//-------------------------------------------------------------------------------------------------------
class OrbiterRenderer;
//-------------------------------------------------------------------------------------------------------
class SDLGraphic : public PlutoGraphic
{
public:
	SDLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	SDLGraphic(struct SDL_Surface *pSDL_Surface);
	SDLGraphic(OrbiterRenderer *pOrbiterRenderer);
	~SDLGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtSDLGraphic; }
	bool IsEmpty() { return NULL == m_pSDL_Surface; }
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0);
	void Clear(); 
	virtual PlutoGraphic *GetHighlightedVersion();
	PlutoGraphic* Clone();
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	struct SDL_Surface *m_pSDL_Surface;

	//helper methods
	static Uint32 getpixel(struct SDL_Surface *pSDL_Surface,int x, int y);
	static void putpixel(struct SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color);
};
//-------------------------------------------------------------------------------------------------------
#endif //__SDL_GRAPHIC_H__

