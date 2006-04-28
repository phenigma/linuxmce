#ifndef __ORBITER_WIN32_DEFS__
#define __ORBITER_WIN32_DEFS__

#ifdef POCKETFROG

	#include "Orbiter_PocketFrog.h"
	#define ORBITER_CLASS Orbiter_PocketFrog

#else

	#ifdef WINCE
		#include "OrbiterSDL_WinCE.h"
		#define ORBITER_CLASS OrbiterSDL_WinCE
	#else
		#include "OrbiterSDL_Win32.h"
		#define ORBITER_CLASS OrbiterSDL_Win32
	#endif //WINCE

#endif //POCKETFROG

#endif //__ORBITER_WIN32_DEFS__
