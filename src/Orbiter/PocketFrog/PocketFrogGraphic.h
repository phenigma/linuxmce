#ifndef __POCKETFROG_GRAPHIC_H__
#define __POCKETFROG_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "../DesignObj_Orbiter.h"
#include "Orbiter_PocketFrog.h"
#include <PocketFrog.h>
using namespace Frog;
//-------------------------------------------------------------------------------------------------------
class PocketFrogGraphic : public PlutoGraphic
{
public:
	PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pCI);
	PocketFrogGraphic(Surface *pSurface);
	PocketFrogGraphic(Orbiter *pCI);
	~PocketFrogGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtPocketFrogGraphic; }
	bool IsEmpty() { return NULL == m_pSurface; }
	bool LoadGraphic(char *pData, size_t iSize);
	void Clear(); 

	class Surface *m_pSurface;
};
//-------------------------------------------------------------------------------------------------------
#endif //__POCKETFROG_GRAPHIC_H__

