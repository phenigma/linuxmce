#ifndef __RENDERER_IMAGE_H__
#define __RENDERER_IMAGE_H__

#include <string>
using namespace std;

//hack: this should be a generic class!

#ifndef POCKETFROG
	#include <SDL.h>
#else
	#include <PocketFrog.h>
	using namespace Frog;
#endif


class RendererImage
{
	public:
		string m_sFilename;

#ifndef POCKETFROG
	SDL_Surface *m_pSDL_Surface;
#else
	Surface *m_pSDL_Surface;
#endif

		bool NewSurface;
		
		RendererImage() : m_sFilename(""), m_pSDL_Surface(NULL), NewSurface(true) {}
		~RendererImage() 
		{ 
			if (m_pSDL_Surface) 
			{
#ifndef POCKETFROG
				SDL_FreeSurface(m_pSDL_Surface);
#else
				delete m_pSDL_Surface;
#endif
			}
		}
};


#endif //__RENDERER_IMAGE_H__
