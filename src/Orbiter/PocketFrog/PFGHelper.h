#ifndef __SDL_RENDERER_OCG_HELPER_H__
#define __SDL_RENDERER_OCG_HELPER_H__

#include <PocketFrog.h>
using namespace Frog;

#include <string>
using namespace std;
//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(string sFilename);
Surface* PocketFrog_LoadPFG(char *pOCGData, size_t iOCGDataSize);
//----------------------------------------------------------------------------------------------------------------
bool PocketFrog_SavePFG(Surface *pSurface, string sFilename);
//----------------------------------------------------------------------------------------------------------------
#endif