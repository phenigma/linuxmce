#ifndef __ORBITER_WIN32_DEFS__
#define __ORBITER_WIN32_DEFS__

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
