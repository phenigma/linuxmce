#include "SDLFrontEnd.h"


#include <iostream>

#include "SDL_image.h"

SDLFrontEnd::SDLFrontEnd()
	: IsEventWaiting (false)
{
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	Background = IMG_Load("back.png");
}

SDLFrontEnd::~SDLFrontEnd()
{
	TTF_Quit();
}

bool SDLFrontEnd::HasEventPending()
{
	if (IsEventWaiting)
		return true;

	IsEventWaiting = SDL_PollEvent(&Event) != 0;
	
	return IsEventWaiting;
}

void SDLFrontEnd::TranslateEvent(WM_Event& WMEvent)
{
	if(IsEventWaiting)
	{
		WMEvent.ConvertFromSDLEvent(Event);
		IsEventWaiting = false;
	}
	else
		WMEvent.Type = 0;
}

int SDLFrontEnd::StartVideoMode(int Width, int Height, bool FullScreen)
{
	int Flags = 0;
	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO)== -1)
	{
		std::cout<<"SDL library cannot init properly!"<<std::endl;
		exit(-1);
	}
		
	if (FullScreen)
		Flags = SDL_FULLSCREEN;
		
	Screen = SDL_SetVideoMode(Width, Height, 0, Flags);
	if(!Screen)
	{
		std::cout<<"Cannot init the video mode!"<<std::endl;
		exit(1);
	}
	
	return 0;
}

void SDLFrontEnd::Flip()
{
	//SDL_FillRect(Screen, NULL, 0xffffff);
	SDL_Flip(Screen);
}

void SDLFrontEnd::PaintBackground()
{
	SDL_UpperBlit(Background, NULL, Screen, NULL);
}

int SDLFrontEnd::PaintFont(TTF_Font *Font, 
						   char* Text, 
						   int Top, int Left, int nPixelHeight, 
						   TColorDesc Color,
						   int Mode)
{
	SDL_Color SDL_color;

	SDL_color.r = Color.GetRed();
	SDL_color.g = Color.GetGreen();
	SDL_color.b = Color.GetBlue();
	SDL_color.unused = 255;

	/* Underline combined with anything crashes */
	int style = TTF_STYLE_BOLD;
	
	TTF_SetFontStyle(Font, style);

	SDL_Surface * RenderedText = NULL;
	try
	{
		RenderedText = TTF_RenderText_Blended(Font, Text, SDL_color);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"Renderer::RealRenderText : TTF_RenderText_Blended crashed!"<<std::endl;
	}


	if (RenderedText == NULL)
	{	
		TTF_Quit();
		std::cout<<"Renderer::RealRenderText: Can't render text: "<<Text<<std::endl;
		return -1;
	}

	SDL_Rect SDL_rect;
	SDL_rect.x = Left;
	SDL_rect.y = Top;

	int Result = RenderedText->w;
	switch(Mode) {
	case 0:
		//Nothing to do, default aling is left one
		break;
	case 1:
		SDL_rect.x -= Result / 2;
		break;
	case 2:
		SDL_rect.x -= Result;
		break;
	}
	
	SDL_BlitSurface(RenderedText, NULL, Screen, &SDL_rect);
	try
	{
		SDL_FreeSurface(RenderedText);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"Renderer::RealRenderText : SDL_FreeSurface crashed!"<<std::endl;
	}
	return Result;
}

int SDLFrontEnd::TextOutput(char* Text, int Left, int Top, float FontSize, TColorDesc Color, 
						 TColorDesc BackColor, int Mode)
{
	int nPixelHeight = (int)FontSize;

#ifdef WIN32
	strcpy(FontName, "C:\\WINDOWS\\Fonts\\verdana.ttf");
#else
	strcpy(FontName, "/usr/share/fonts/truetype/msttcorefonts/verdana.ttf");
#endif
	//set up the font
	TTF_Font *Font = TTF_OpenFont(FontName, nPixelHeight);

	int Result = 0;
		Result = PaintFont(Font, Text, Top+2, Left+2, nPixelHeight, BackColor, Mode);
	if (Result!= -1)
		Result = PaintFont(Font, Text, Top, Left, nPixelHeight, Color, Mode);
	TTF_CloseFont(Font);
	
	return Result;
}

