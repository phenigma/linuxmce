#ifndef __PLUTO_SDL_DEFS_H__
#define __PLUTO_SDL_DEFS_H__
//----------------------------------------------------------------------------------------------------------------
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	static Uint32 rmask = 0xff000000;
	static Uint32 gmask = 0x00ff0000;
	static Uint32 bmask = 0x0000ff00;
	static Uint32 amask = 0x000000ff;
#else
	static Uint32 rmask = 0x000000ff;
	static Uint32 gmask = 0x0000ff00;
	static Uint32 bmask = 0x00ff0000;
	static Uint32 amask = 0xff000000;
#endif
//----------------------------------------------------------------------------------------------------------------
#endif