#include "SDLFrontEnd.h"

#include <iostream>

#include "Wizard.h"

#include <SDL_rotozoom.h>

SDLFrontEnd::SDLFrontEnd()
	: IsEventWaiting (false)
{
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		Wizard::GetInstance()->SetExitWithCode(-1);
		return;
	}

	CurrentFont = NULL;
	Display = NULL;

	Screen = NULL;
	BackSurface = NULL;
	ScaledScreen = NULL;
	ScaledBack = NULL;
}

SDLFrontEnd::~SDLFrontEnd()
{
	if (Screen)
		SDL_FreeSurface(Screen);
	if (BackSurface)
		SDL_FreeSurface(BackSurface);
	if (ScaledScreen)
		SDL_FreeSurface(ScaledScreen);
	if (ScaledBack)
		SDL_FreeSurface(ScaledBack);

	if(CurrentFont!= NULL)
		TTF_CloseFont(CurrentFont);
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
	SDL_Quit();
	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)== -1)
	{
		std::cout<<"SDL library cannot init properly!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}
		
	if (FullScreen)
		Flags = SDL_FULLSCREEN;
		
	Display = SDL_SetVideoMode(Width, Height, 0, Flags);
	if(!Display)
	{
		std::cout<<"Cannot init the video mode!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}
	
	SDL_WM_SetCaption("AVWizard", "AVWizard");

	BackSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 24, 0, 0, 0, 0);
	Screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 24, 0, 0, 0, 0);

	return 0;
}

void SDLFrontEnd::Arrows()
{

	int Width = Screen->w;
	int Height = Screen->h;


	SDL_Rect RectCorner;
	RectCorner.w = 8;
	RectCorner.h = 8;

	RectCorner.x = 0;
	RectCorner.y = 0;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 0, 0, 255));
	RectCorner.x = Width - 8;
	RectCorner.y = Height - 8;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 0, 0, 255));
	RectCorner.x = 0;
	RectCorner.y = Height - 8;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 0, 0, 255));
	RectCorner.x = Width - 8;
	RectCorner.y = 0;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 0, 0, 255));


	RectCorner.x = 8;
	RectCorner.y = 8;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 255, 0, 255));
	RectCorner.x = Width - 16;
	RectCorner.y = Height - 16;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 255, 0, 255));
	RectCorner.x = Width - 16;
	RectCorner.y = 8;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 255, 0, 255));
	RectCorner.x = 8;
	RectCorner.y = Height - 16;
	SDL_FillRect(Screen, &RectCorner, SDL_MapRGBA(Screen->format, 255, 255, 0, 255));
}

void SDLFrontEnd::Flip(int LeftBorder, int TopBorder, int Border)
{
	if (BackColor)
		SDL_FillRect(Display, NULL, SDL_MapRGBA(Screen->format, BackColor->GetRed(), 
			BackColor->GetGreen(), 
			BackColor->GetBlue(), 
			255));

	double ZoomX = 1;
	double ZoomY = 1;
	double ZoomX2 = 1;
	double ZoomY2 = 1;


	int Width = Screen->w;
	int Height = Screen->h;

	SDL_Rect Rect, RectZoom;

	ZoomX = (Width-2*Border) / 640.0f;
	ZoomY = (Height-2*Border) / 480.0f;
	RectZoom.x = LeftBorder;
	RectZoom.y = TopBorder;
	RectZoom.w = Width - 2 * Border;
	RectZoom.h = Height - 2 * Border;

	if(NeedUpdateBack)
	{
		ScaledBack = zoomSurface(BackSurface, ZoomX, ZoomY, SMOOTHING_ON);
		NeedUpdateBack = false;
	}
	if(Wizard::GetInstance()->CurrentPage == 5)
	{
		int Border2 = Border + ARROWS_BORDER;
		ZoomX2 = (Width-2*Border2) / 640.0f;
		ZoomY2 = (Height-2*Border2) / 480.0f;
		Rect.x = LeftBorder + ARROWS_BORDER;
		Rect.y = TopBorder + ARROWS_BORDER;
		Rect.w = Width - 2 * Border2;
		Rect.h = Height - 2 * Border2;
	}
	else
	{
		int Border2 = Border;
		ZoomX2 = (Width-2*Border2) / 640.0f;
		ZoomY2 = (Height-2*Border2) / 480.0f;
		Rect.x = LeftBorder;
		Rect.y = TopBorder;
		Rect.w = Width - 2 * Border2;
		Rect.h = Height - 2 * Border2;
	}


	SDL_BlitSurface(ScaledBack, NULL, Display, &RectZoom);

	
	if(NeedUpdateScreen)
	{
		ScaledScreen = zoomSurface(Screen, ZoomX2, ZoomY2, SMOOTHING_ON);
		NeedUpdateScreen = true;
	}

	SDL_BlitSurface(ScaledScreen, NULL, Display, &Rect);
	//SDL_SaveBMP(Display, "/home/ciplogic/Desktop/screen2.bmp");

	//SDL_SaveBMP(BackSurface, "/home/ciplogic/Desktop/screen.bmp");
	SDL_Flip(Display);	
}

void SDLFrontEnd::PaintBackground()
{
	SDL_FillRect(Display, NULL, 0xffffff);
}

int SDLFrontEnd::TextOutWidth(std::string Text)
{
	SDL_Color SDL_color;
	SDL_color.r = 0;
	SDL_color.g = 0;
	SDL_color.b = 0;
	SDL_color.unused = 0;

	SDL_Surface * RenderedText = NULL;
	try
	{
		RenderedText = TTF_RenderText_Blended(CurrentFont, Text.c_str(), SDL_color);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"SDLFrontEnd::TextOutWidth : TTF_RenderText_Blended crashed!"<<std::endl;
		return -1;
	}
	int Result = 0;
	if (RenderedText != NULL)
		Result = RenderedText->w;

	return Result;
}

int SDLFrontEnd::PaintFont(char* Text, 
						   int Top, int Left, 
						   TColorDesc Color,
						   int Mode)
{
	SDL_Color SDL_color;

	SDL_color.r = Color.GetRed();
	SDL_color.g = Color.GetGreen();
	SDL_color.b = Color.GetBlue();
	SDL_color.unused = 0;


	SDL_Surface * RenderedText = NULL;
	try
	{
		RenderedText = TTF_RenderText_Blended(CurrentFont, Text, SDL_color);
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

int SDLFrontEnd::TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode)
{
	return PaintFont(Text, Top, Left, Color, Mode);
}

bool SDLFrontEnd::SetCurrentFont(std::string FontName, int FontHeight, int Style)
{
	TTF_Font *Font;
	//set up the font
	Font = TTF_OpenFont(FontName.c_str(), FontHeight);

	if(Font!= NULL)
	{
		if(CurrentFont!= NULL)
			TTF_CloseFont(CurrentFont);
		CurrentFont = Font;
	}
	else
		return false;

	int style = Style;

	TTF_SetFontStyle(CurrentFont, style);
	return true;
}

void SDLFrontEnd::Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{
	NeedUpdateScreen = true;

	if(ScaledScreen)
	{
		SDL_FreeSurface(ScaledScreen);
		ScaledScreen = NULL;
	}

	SDL_BlitSurface(Surface, &SrcRect, Screen, &DestRect);
}

void SDLFrontEnd::BackBlit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{
	if (Surface == NULL)
		return;
	NeedUpdateBack = true;
	if(ScaledBack)
	{
		SDL_FreeSurface(ScaledBack);
		ScaledBack = NULL;
	}

	SDL_BlitSurface(Surface, &SrcRect, BackSurface, &DestRect);	
}

void SDLFrontEnd::BackFillColor(TColorDesc* Color)
{
	NeedUpdateBack = true;
	if(BackColor == NULL)
		delete BackColor;
	this->BackColor = Color;
	if(ScaledBack)
	{
		SDL_FreeSurface(ScaledBack);
		ScaledBack = NULL;
	}	
	SDL_FillRect(BackSurface, NULL, SDL_MapRGBA(Screen->format, 
		Color->GetRed(), 
		Color->GetGreen(), 
		Color->GetBlue(), 
		255));
}

int SDLFrontEnd::GetScreenWidth()
{
	return Display->w;
}

int SDLFrontEnd::GetScreenHeight()
{
	return Display->h;
}
