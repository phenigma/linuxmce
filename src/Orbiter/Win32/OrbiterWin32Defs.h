#ifndef __ORBITER_WIN32_DEFS__
#define __ORBITER_WIN32_DEFS__

#ifdef POCKETFROG
	#include "Orbiter_PocketFrog.h"
	#define ORBITER_CLASS Orbiter_PocketFrog
#elif ORBITER_OPENGL
	#include "Orbiter_OpenGL.h"
	#define ORBITER_CLASS Orbiter_OpenGL
#else
	#ifdef WINCE
		#include "OrbiterSDL_WinCE.h"
		#define ORBITER_CLASS OrbiterSDL_WinCE
	#else
		#include "OrbiterSDL_Win32.h"
		#define ORBITER_CLASS OrbiterSDL_Win32
	#endif //WINCE
#endif //POCKETFROG

#ifdef WINCE
	#ifdef WINCE_x86
		const string csUpdateBinaryName("UpdateBinaryCE_x86.exe");
		const string csOrbiter_Update("/usr/pluto/bin/Orbiter_CeNet4_x86.dat");
	#else
		const string csUpdateBinaryName("UpdateBinaryCE.exe");
		const string csOrbiter_Update("/usr/pluto/bin/Orbiter_CeNet4_XScale.dat");
	#endif
#else
		const string csUpdateBinaryName("UpdateBinary.exe");
	#ifdef ORBITER_OPENGL
		const string csOrbiter_Update("/usr/pluto/bin/Orbiter_OpenGL.dat");
	#else
		const string csOrbiter_Update("/usr/pluto/bin/Orbiter_Win32.dat");
	#endif
#endif

#endif //__ORBITER_WIN32_DEFS__
