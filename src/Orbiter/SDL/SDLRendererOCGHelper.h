#ifndef __SDL_RENDERER_OCG_HELPER_H__
#define __SDL_RENDERER_OCG_HELPER_H__

#include <sdl.h>
#include <string>
using namespace std;
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(string sFilename);
//----------------------------------------------------------------------------------------------------------------
bool SDL_SaveOCG(SDL_Surface *pSurface, string sFilename);
//----------------------------------------------------------------------------------------------------------------
#endif