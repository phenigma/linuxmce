#ifndef __JPEG_WRAPPER_H__
#define __JPEG_WRAPPER_H__

bool SDL_SaveJPG(struct SDL_Surface *pSurface, const char* sFileName, int iQuality = 70);

#endif