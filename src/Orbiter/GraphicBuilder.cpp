#include "GraphicBuilder.h"
#include "RendererMNG.h"
#include "OrbiterGen/Renderer.h"
#include "DCE/Logger.h"

#if defined(POCKETFROG)
	#include "PocketFrog/PocketFrogGraphic.h"
#elif defined(ORBITER_OPENGL)	
	#include "OpenGL/OpenGLGraphic.h"
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
	eGraphicFormat eGF = GR_UNKNOWN;

	if(IsMNG(Filename))
		eGF = GR_MNG;
	else if(IsPNG(Filename))
		eGF = GR_PNG;
	else if(IsOCG(Filename))
		eGF = GR_OCG;

	PlutoGraphic *pGraphic
#if defined(POCKETFROG)
		= new PocketFrogGraphic(pOrbiter);
#elif defined(ORBITER_OPENGL)
		= new OpenGLGraphic(pOrbiter);
#else
		= new SDLGraphic(pOrbiter); //we won't load the graphic yet
#endif

	if(NULL != pGraphic)
	{
		pGraphic->m_GraphicFormat = eGF;
		pGraphic->m_GraphicManagement = GraphicManagement;
		pGraphic->m_Filename = Filename;
		vectPlutoGraphic.push_back(pGraphic);
	}
}
//-------------------------------------------------------------------------------------------------------
