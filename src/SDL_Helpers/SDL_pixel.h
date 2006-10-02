#ifndef SDL_PIXEL_H
#define SDL_PIXEL_H

#include <SDL.h>

Uint8 * getpixelpointer(SDL_Surface *pSurface, int x, int y);
Uint32 getpixel(SDL_Surface *pSurface, int x, int y);
void putpixel(SDL_Surface *pSurface, int x, int y, Uint32 pixel_color);
#endif /*SDL_PIXEL_H*/
