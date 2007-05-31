#include "SDLFrontEnd.h"
//--------------------------------------------------------------------------------------------------------
#include <SDL_image.h>
#include <SDL_ttf.h>
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::SDLFrontEnd(int w, int h) : m_nWidth(w), m_nHeight(h)
{
}
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::~SDLFrontEnd()
{
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

	Render();

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
void SDLFrontEnd::Render(DisplayState *pDisplayState)
{
	SDL_Rect Rectangle;
	Rectangle.x = 0; Rectangle.y = 0; Rectangle.w = m_nWidth; Rectangle.h = m_nHeight;
	SDL_FillRect(m_pSurface, &Rectangle, SDL_MapRGBA(m_pSurface->format, 0xe0, 0xff, 0xe0, 0xff));
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::RenderText(int x, int y, string sText, int width)\
{
	
}
//--------------------------------------------------------------------------------------------------------