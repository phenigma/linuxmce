#include "SDLFrontEnd.h"
//--------------------------------------------------------------------------------------------------------
#include <SDL_image.h>
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::SDLFrontEnd(int w, int h) : m_pSurface(NULL), m_pFont(NULL), m_nWidth(w), m_nHeight(h)
{
}
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::~SDLFrontEnd()
{
	TTF_Quit();
	SDL_Quit();
}
//--------------------------------------------------------------------------------------------------------
bool SDLFrontEnd::Init()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		return false;
	}	

	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

	if (!videoInfo)
	{
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		return false;
	}

	int videoFlags = SDL_HWPALETTE;

	if(videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	if(videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	m_pSurface = SDL_SetVideoMode(m_nWidth, m_nHeight, 32, videoFlags);

	if (!m_pSurface)
	{
		fprintf(stderr,  "Video mode set failed: %s\n", SDL_GetError());
		return false;
	}

	Clear();

	if(TTF_Init() != -1)
	{
		m_pFont =  TTF_OpenFont("/windows/fonts/arial.ttf", 12);
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool SDLFrontEnd::EventLoop()
{
	SDL_Event event;

	while(true)
	{
		while (SDL_PollEvent(&event))
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					//handle key press : event.key.keysym
					break;

				case SDL_QUIT:
					printf("about to quit!\n");
					return true;
					break;

				default:
					break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::Render(const DisplayState &display_state)
{
	Clear();

	RenderText(10, 10, display_state.m_sHeader);
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::Clear()
{
	SDL_Rect Rectangle;
	Rectangle.x = 0; Rectangle.y = 0; Rectangle.w = m_nWidth; Rectangle.h = m_nHeight;
	SDL_FillRect(m_pSurface, &Rectangle, SDL_MapRGBA(m_pSurface->format, 0xe0, 0xff, 0xe0, 0xff));
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::RenderText(int x, int y, string sText)
{
	SDL_Color color;
	color.r = 0xff;
	color.g = 0;
	color.b = 0;
	color.unused = 0;

	SDL_Surface *pTextSurface = TTF_RenderUTF8_Blended(m_pFont, sText.c_str(), color);

	if(NULL != pTextSurface)
	{
		SDL_Rect dest_rect;
		dest_rect.x = x;
		dest_rect.y = y;
		dest_rect.w = 0;
		dest_rect.h = 0;

		SDL_BlitSurface(pTextSurface, NULL, m_pSurface, &dest_rect);
		SDL_FreeSurface(pTextSurface);
	}
}
//--------------------------------------------------------------------------------------------------------