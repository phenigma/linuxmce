#include "OrbiterRendererFactory.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"

#if defined(PROXY_ORBITER) 
	#include "../Proxy_Orbiter/Proxy_OrbiterRenderer_SDL.h"
#elif defined(BLUETOOTH_DONGLE)
	#include "../Bluetooth_Dongle/OrbiterRenderer_SDL_Bluetooth.h"	
#else
	#ifdef WIN32
		#if defined(POCKETFROG)
			#include "PocketFrog/OrbiterRenderer_PocketFrog.h"
		#elif defined(ORBITER_OPENGL)
			#include "OpenGL/OrbiterRenderer_OpenGL.h"
		#else
			#include "SDL/Win32/OrbiterRenderer_SDL_Win32.h"
		#endif
	#else
		#if defined(ORBITER_OPENGL)
			#include "OpenGL/OrbiterRenderer_OpenGL.h"
		#else
			#include "Linux/OrbiterRenderer_SDL_Linux.h"
		#endif
	#endif
#endif

/*static*/ OrbiterRenderer *OrbiterRendererFactory::CreateRenderer(Orbiter *pOrbiter)
{
#if defined(PROXY_ORBITER)
	return new Proxy_OrbiterRenderer_SDL(pOrbiter);
#elif defined(BLUETOOTH_DONGLE)
	return new OrbiterRenderer_SDL_Bluetooth(pOrbiter);
#else
	#ifdef WIN32
		#if defined(POCKETFROG)
			return new OrbiterRenderer_PocketFrog(pOrbiter);
		#elif defined(ORBITER_OPENGL)
			return new OrbiterRenderer_OpenGL(pOrbiter);
		#else
			return new OrbiterRenderer_SDL_Win32(pOrbiter);
		#endif
	#else
		#if defined(ORBITER_OPENGL)
			return new OrbiterRenderer_OpenGL(pOrbiter);
		#else
			return new OrbiterRenderer_SDL_Linux(pOrbiter);
		#endif
	#endif
#endif
}

