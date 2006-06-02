#include "OrbiterRendererFactory.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"

#if defined(PROXY_ORBITER) 
	#include "../Proxy_Orbiter/Proxy_OrbiterRenderer_SDL.h"
#elif defined(BLUETOOTH_DONGLE)
	#include "../Bluetooth_Dongle/OrbiterRenderer_SDL_Bluetooth.h"	
#else
	#ifdef WIN32
		#ifdef POCKETFROG
			#include "PocketFrog/OrbiterRenderer_PocketFrog.h"
		#else
			#include "SDL/Win32/OrbiterRenderer_SDL_Win32.h"
		#endif
	#else
		#include "Linux/OrbiterRenderer_SDL_Linux.h"
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
		#ifdef POCKETFROG
			return new OrbiterRenderer_PocketFrog(pOrbiter);
		#else
			return new OrbiterRenderer_SDL_Win32(pOrbiter);
		#endif
	#else
			return new OrbiterRenderer_SDL_Linux(pOrbiter);
	#endif
#endif
}

