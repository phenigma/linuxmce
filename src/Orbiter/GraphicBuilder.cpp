#include "GraphicBuilder.h"
#include "RendererMNG.h"
#include "OrbiterGen/Renderer.h"
#include "DCE/Logger.h"

#ifdef POCKETFROG
	#include "PocketFrog/PocketFrogGraphic.h"
#else
	#include "SDL/SDLGraphic.h"
#endif
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
bool IsOCG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "OCG" == sExtension || "ocg" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
bool IsPFG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "PFG" == sExtension || "pfg" == sExtension;
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
	else if(IsOCG(Filename))
		eGF = GR_OCG;
	//else if(IsPFG(Filename))
	//	eGF = GR_PFG;


#ifdef POCKETFROG
	PocketFrogGraphic *pGraphic = new PocketFrogGraphic(pOrbiter);
#else	
	SDLGraphic *pGraphic = new SDLGraphic(pOrbiter); //we won't load the graphic yet
#endif

	pGraphic->m_GraphicFormat = eGF;
	pGraphic->m_GraphicManagement = GraphicManagement;
	pGraphic->m_Filename = Filename;
	vectPlutoGraphic.push_back(pGraphic);
}
//-------------------------------------------------------------------------------------------------------
