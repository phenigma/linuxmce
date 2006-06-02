#ifndef __SDL_RENDERER_OCG_HELPER_H__
#define __SDL_RENDERER_OCG_HELPER_H__

#include <SDL.h>
#include <string>
using namespace std;
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(string sFilename);
SDL_Surface* SDL_LoadOCG(char *pOCGData, size_t iOCGDataSize);
//----------------------------------------------------------------------------------------------------------------
bool SDL_SaveOCG(SDL_Surface *pSurface, string sFilename, bool bPocket = true);
//----------------------------------------------------------------------------------------------------------------
#endif
