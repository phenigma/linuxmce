#ifndef __ORBITER_RENDERER_SDL_RENDERER_H__
#define __ORBITER_RENDERER_SDL_RENDERER_H__

#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_SDL_Bluetooth : public OrbiterRenderer_SDL
	{
	private:
		friend class OrbiterRendererFactory;
		OrbiterRenderer_SDL_Bluetooth(Orbiter *pOrbiter);

	public:

		~OrbiterRenderer_SDL_Bluetooth();

		void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
		void BeginPaint();
		void EndPaint();
	};
}

#endif // __ORBITER_RENDERER_SDL_RENDERER_H__