#include "GraphicBuilder.h"
#include "SDL/SDLGraphic.h"
//-------------------------------------------------------------------------------------------------------
PlutoGraphic *CreateGraphic(GraphicType Type, string Filename, eGraphicManagement GraphicManagement, 
							Orbiter *pOrbiter)
{
	class PlutoGraphic *pGraphic = NULL;

	switch(Type)
	{
	case gtSDLGraphic: 
		return new SDLGraphic(Filename, GraphicManagement, pOrbiter);
	}

	return pGraphic;
}
//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, string Filename, 
						 eGraphicManagement GraphicManagement, Orbiter *pOrbiter)
{
	//TODO
	switch(Type) //for now, handle only single frame graphic files 
	{
	case gtSDLGraphic: 
		vectPlutoGraphic.push_back(new SDLGraphic(Filename, GraphicManagement, pOrbiter));
	}
}
//-------------------------------------------------------------------------------------------------------
