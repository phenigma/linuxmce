#ifndef __SDL_GRAPHIC_H__
#define __SDL_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "../DesignObj_Orbiter.h"
//-------------------------------------------------------------------------------------------------------
class SDLGraphic : public PlutoGraphic
{
public:
	SDLGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pCI);
	SDLGraphic(struct SDL_Surface *pSDL_Surface);
	SDLGraphic(Orbiter *pCI);
	~SDLGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtSDLGraphic; }
	bool IsEmpty() { return NULL == m_pSDL_Surface; }
	bool LoadGraphic(char *pData, size_t iSize);
	void Clear(); 

	struct SDL_Surface *m_pSDL_Surface;
};
//-------------------------------------------------------------------------------------------------------
#endif //__SDL_GRAPHIC_H__

