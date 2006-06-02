#ifndef __POCKETFROG_GRAPHIC_H__
#define __POCKETFROG_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//-------------------------------------------------------------------------------------------------------
#include "../PlutoGraphic.h"
#include <PocketFrog.h>
using namespace Frog;
//-------------------------------------------------------------------------------------------------------
class OrbiterRenderer;
//-------------------------------------------------------------------------------------------------------
class PocketFrogGraphic : public PlutoGraphic
{
public:
	PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	PocketFrogGraphic(Surface *pSurface);
	PocketFrogGraphic(OrbiterRenderer *pOrbiterRenderer);
	~PocketFrogGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtPocketFrogGraphic; }
	bool IsEmpty() { return NULL == m_pSurface; }
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0);
	void Clear(); 
    PlutoGraphic *GetHighlightedVersion();
	PlutoGraphic *Clone();
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	class Surface *m_pSurface;
};
//-------------------------------------------------------------------------------------------------------
#endif //__POCKETFROG_GRAPHIC_H__

