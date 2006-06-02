#ifndef __JPEG_WRAPPER_H__
#define __JPEG_WRAPPER_H__

#include <SDL_image.h>

bool SDL_SaveJPG(SDL_Surface *pSurface, const char* sFileName, int iQuality = 70);

#endif
