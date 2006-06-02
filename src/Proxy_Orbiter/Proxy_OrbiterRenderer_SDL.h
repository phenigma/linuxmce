#ifndef __PROXY_ORBITER_RENDERER_SDL_H__
#define __PROXY_ORBITER_RENDERER_SDL_H__

#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

namespace DCE
{
	class Orbiter;

	class Proxy_OrbiterRenderer_SDL : public OrbiterRenderer_SDL
	{
	private:
		friend class OrbiterRendererFactory;
		Proxy_OrbiterRenderer_SDL(Orbiter *pOrbiter);

	public:

		~Proxy_OrbiterRenderer_SDL();

		void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
		void BeginPaint();
		void EndPaint();
	};
}

#endif // __PROXY_ORBITER_RENDERER_SDL_H__