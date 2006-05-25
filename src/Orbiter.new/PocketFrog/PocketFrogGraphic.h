#ifndef __POCKETFROG_GRAPHIC_H__
#define __POCKETFROG_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "../DesignObj_Orbiter.h"
#include "OrbiterRenderer_PocketFrog.h"
#include <PocketFrog.h>
using namespace Frog;
//-------------------------------------------------------------------------------------------------------
class PocketFrogGraphic : public PlutoGraphic
{
public:
	PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement);
	PocketFrogGraphic(Surface *pSurface);
	PocketFrogGraphic();
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

