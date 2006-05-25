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

/*static*/ OrbiterRenderer *OrbiterRendererFactory::CreateRenderer()
{
#ifdef WIN32
	#ifdef POCKETFROG
		return new OrbiterRenderer_PocketFrog();
	#else
		return new OrbiterSDL_Win32();
	#endif
#else
		return new OrbiterSDL();
#endif
}

