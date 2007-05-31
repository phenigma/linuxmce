#ifndef __SDL_FRONT_END_H__
#define __SDL_FRONT_END_H__
//--------------------------------------------------------------------------------------------------------
#include <SDL.h>
//--------------------------------------------------------------------------------------------------------
#include "../../LCDLogic/IRenderer.h"
//--------------------------------------------------------------------------------------------------------
class SDLFrontEnd : public IRenderer
{
private:

	SDL_Surface *m_pSurface;
	int m_nWidth, m_nHeight;

public:

	SDLFrontEnd(int w, int h);
	~SDLFrontEnd();

	bool Init();
	bool EventLoop();

	void Render(DisplayState *pDisplayState = NULL);

private:

	void RenderText(int x, int y, string sText, int width);
};
//--------------------------------------------------------------------------------------------------------
#endif //__SDL_FRONT_END_H__
