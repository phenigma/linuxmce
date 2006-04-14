#ifndef SDLFRONTEND_H_
#define SDLFRONTEND_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include "ColorDesc.h"

#include "WM_Event.h"

class SDLFrontEnd
{
	SDL_Event Event;
	bool IsEventWaiting;
	
	SDL_Surface* Screen;
	char FontName[1024];

	SDL_Surface* Background;

	/**
	 *	Mode = 0 -> defalt align to left
	 *	Mode = 1 -> center align 
	 *	Mode = 2 -> right align 
	 */

	int PaintFont(TTF_Font *Font, char* Text, int Top, int Left, 
		int nPixelHeight, TColorDesc Color, int Mode);
public:

	SDLFrontEnd();
	virtual ~SDLFrontEnd();
	
	bool HasEventPending();
	
	void TranslateEvent(WM_Event& WMEvent);
	
	int StartVideoMode(int Width, int Height, bool FullScreen);

	int TextOutput(char* Text, int Left, int Top, float FontSize, 
			TColorDesc Color, TColorDesc BackColor, int Mode);

	void Flip();

	void PaintBackground();
};

#endif /*SDLFRONTEND_H_*/
