#ifndef __GRAPHIC_BUILDER_H__
#define __GRAPHIC_BUILDER_H__

#include "DesignObj_Orbiter.h"
//-------------------------------------------------------------------------------------------------------
PlutoGraphic *CreateGraphic(GraphicType Type, string Filename, eGraphicManagement GraphicManagement, 
	class Orbiter *pOrbiter); 
//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, string Filename, 
	eGraphicManagement GraphicManagement, class Orbiter *pOrbiter);
//-------------------------------------------------------------------------------------------------------
#endif //__GRAPHIC_BUILDER_H__

