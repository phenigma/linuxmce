#ifndef __GRAPHIC_BUILDER_H__
#define __GRAPHIC_BUILDER_H__

#include "PlutoGraphic.h"
#include "DesignObj_Orbiter.h"
//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, 
						 string Filename, eGraphicManagement GraphicManagement, class OrbiterRenderer *pOrbiterRenderer);
//-------------------------------------------------------------------------------------------------------
#endif //__GRAPHIC_BUILDER_H__

