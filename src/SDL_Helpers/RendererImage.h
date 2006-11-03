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
	SDL_Surface *m_pSurface;
#else
	Surface *m_pSurface;
#endif

		bool NewSurface;
		
		RendererImage() : m_sFilename(""), m_pSurface(NULL), NewSurface(true) {}
		~RendererImage() 
		{ 
			if (m_pSurface) 
			{
#ifndef POCKETFROG
				SDL_FreeSurface(m_pSurface);
#else
				delete m_pSurface;
#endif
			}
		}
};


#endif //__RENDERER_IMAGE_H__
