#include "SDLFrontEnd.h"
#include "DCE/Logger.h"
using namespace DCE;

#include <iostream>

#include <SDL_rotozoom.h>

SDLFrontEnd::SDLFrontEnd()
	: IsEventWaiting (false)
{
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return;
	} 

	CurrentFont = NULL;

	Display = NULL;
}

SDLFrontEnd::~SDLFrontEnd()
{
	if(CurrentFont!= NULL)
		TTF_CloseFont(CurrentFont);
	TTF_Quit();
	SDL_Quit();
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
	if(Display)
		SDL_Quit();
	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)== -1)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"SDL library cannot init properly!");
		return -1;
	}
		
	if (FullScreen)
		Flags = SDL_FULLSCREEN;

	Flags |=  SDL_OPENGL;

	Display = SDL_SetVideoMode(Width, Height, 0, Flags);
	g_pPlutoLogger->Write(LV_STATUS,"Setting video to w: %d h: %d",Width,Height);
	if(!Display)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot init the video mode!");
		return -1;
	}
	
	SDL_WM_SetCaption("PlutoGalleryViewer", "GalleryViewer");
	
	return 0;
}

void SDLFrontEnd::Flip()
{
	SDL_GL_SwapBuffers();
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
		g_pPlutoLogger->Write(LV_CRITICAL,"SDLFrontEnd::TextOutWidth : TTF_RenderText_Blended crashed!");
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
	if(CurrentFont == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Warning: there is no font setup for rendering!");
		return -1;
	}
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
		g_pPlutoLogger->Write(LV_CRITICAL,"Renderer::RealRenderText : TTF_RenderText_Blended crashed!");
	}


	if (RenderedText == NULL)
	{	
		TTF_Quit();
		g_pPlutoLogger->Write(LV_CRITICAL,"Renderer::RealRenderText: Can't render text: %s", Text);
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
	
	//SDL_BlitSurface(RenderedText, NULL, Screen, &SDL_rect);
	try
	{
		SDL_FreeSurface(RenderedText);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Renderer::RealRenderText : SDL_FreeSurface crashed!");
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

int SDLFrontEnd::GetScreenWidth()
{
	return Display->w;
}

int SDLFrontEnd::GetScreenHeight()
{
	return Display->h;
}

