#include "OrbiterRendererFactory.h"

#ifdef WIN32
	#ifdef POCKETFROG
		#include "PocketFrog/OrbiterRenderer_PocketFrog.h"
	#else
		#include "OrbiterSDL_Win32.h"
	#endif
#else
	#include "SDL/OrbiterSDL.h"
#endif

/*static*/ OrbiterRenderer *OrbiterRendererFactory::CreateRenderer(Orbiter *pOrbiter)
{
#ifdef WIN32
	#ifdef POCKETFROG
		return new OrbiterRenderer_PocketFrog(pOrbiter);
	#else
		return new OrbiterSDL_Win32(pOrbiter);
	#endif
#else
		return new OrbiterSDL(pOrbiter);
#endif
}

