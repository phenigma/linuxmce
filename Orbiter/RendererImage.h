#ifndef __RENDERER_IMAGE_H__
#define __RENDERER_IMAGE_H__

#include <string>
using namespace std;

#include <SDL.h>

class RendererImage
{
	public:
		string m_sFilename;
		SDL_Surface * m_pSDL_Surface;
		bool NewSurface;
		
		RendererImage() : m_sFilename(""), m_pSDL_Surface(NULL), NewSurface(true) {}
		~RendererImage() { if (m_pSDL_Surface) SDL_FreeSurface(m_pSDL_Surface); }
};

#endif //__RENDERER_IMAGE_H__
