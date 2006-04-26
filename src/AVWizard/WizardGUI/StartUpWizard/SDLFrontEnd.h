
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef SDLFrontEnd_H_
#define SDLFrontEnd_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include "ColorDesc.h"

#include "WM_Event.h"
#include <string>

class SDLFrontEnd
{
	SDL_Event Event;
	bool IsEventWaiting;
	/**
	 *	Offscreen surface that does the drawing. At the end you should 
	 *	show the results using Flip() method
	 */
	SDL_Surface* Screen;

	/**
	 *	Current used font for rendering text	
	 */
	TTF_Font *CurrentFont;
	/**
	 *	Mode = 0 -> defalt align to left
	 *	Mode = 1 -> center align 
	 *	Mode = 2 -> right align 
	 */
	int PaintFont(char* Text, int Top, int Left, 
		TColorDesc Color, int Mode);
public:

	/**
	 *	Standard constructor for SDL frontend	
	 */
	SDLFrontEnd();
	virtual ~SDLFrontEnd();
	
	bool HasEventPending();
	
	void TranslateEvent(WM_Event& WMEvent);
	
	int StartVideoMode(int Width, int Height, bool FullScreen);

	int TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode);

	/**
	 *	Says the width in pixels of rendering text
	 */
	int TextOutWidth(std::string Text);

	/**
	 *	Display the final result 
	 */
	void Flip();

	void PaintBackground();

	bool SetCurrentFont(std::string FontName, int FontHeight, int Style);

	void Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect);
};

#endif /*SDLFRONTEND_H_*/
