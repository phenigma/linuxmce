#ifndef SDL_SAVEPNG_H
#define SDL_SAVEPNG_H

#include <stdio.h>
#include "SDL.h"

#include <string>
using namespace std;

bool IMG_SavePNG(SDL_Surface *pSurface, FILE * FileHandle, bool bPrependSignature = true);
bool IMG_SavePNG(SDL_Surface *pSurface, const string & sFileName);

#endif /*SDL_SAVEPNG_H*/
