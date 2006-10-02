#ifndef SDL_DEFS_H
#define SDL_DEFS_H

#include <SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 rmask = 0xff000000;
	const Uint32 gmask = 0x00ff0000;
	const Uint32 bmask = 0x0000ff00;
	const Uint32 amask = 0x000000ff;
	
	const int rshift = 24, gshift = 16, bshift = 8, ashift = 0;
#else
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;

	const int rshift = 0, gshift = 8, bshift = 16, ashift = 24;
#endif
//----------------------------------------------------------------------------------------------------------------
#endif /*SDL_DEFS_H*/
