#include "GraphicBuilder.h"
#include "SDL/SDLGraphic.h"
#include "RendererMNG.h"
#include "Renderer.h"
#include "DCE/Logger.h"
//-------------------------------------------------------------------------------------------------------
bool IsMNG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "MNG" == sExtension || "mng" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
bool IsPNG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "PNG" == sExtension || "png" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, string Filename, 
						 eGraphicManagement GraphicManagement, Orbiter *pOrbiter)
{
	size_t iFileSize = 0;
	char *pFileData = NULL;

	//TODO: set the right graphic format, instead of GR_UNKNOWN for non-MNG graphic files
	eGraphicFormat eGF = GR_UNKNOWN;

	if(IsMNG(Filename))
		eGF = GR_MNG;
	else if(IsPNG(Filename))
		eGF = GR_PNG;

	switch(Type) 
	{
		case gtSDLGraphic: 
		{
			SDLGraphic *pSDL_Graphic = new SDLGraphic(pOrbiter); //we won't load the graphic yet
			pSDL_Graphic->m_GraphicFormat = eGF;
			pSDL_Graphic->m_GraphicManagement = GraphicManagement;
			pSDL_Graphic->m_Filename = Filename;
			vectPlutoGraphic.push_back(pSDL_Graphic);
		}
		break;
	}
}
//-------------------------------------------------------------------------------------------------------
