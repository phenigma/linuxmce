#include "PlutoGraphic.h"
#include "OrbiterRenderer.h"
//=======================================================================================================
//Generic PlutoGraphic class methods
//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic()
{
	Initialize();

	Width = 0;
	Height = 0;
}
//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(OrbiterRenderer *pOrbiterRenderer)
{
	Initialize();

	m_pOrbiterRenderer = pOrbiterRenderer;
	Width = 0;
	Height = 0;
}

//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer)
{
	Initialize();

	m_pOrbiterRenderer = pOrbiterRenderer;
	m_Filename = Filename;
	m_GraphicManagement = GraphicManagement;
	Width = 0;
	Height = 0;
};
//-------------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic::~PlutoGraphic()
{
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void PlutoGraphic::Initialize() 
{ 
	m_GraphicManagement = GR_KEEPCOMPRESSED; 
	m_GraphicFormat = GR_UNKNOWN; 
	m_Filename = "";
	m_pOrbiterRenderer = NULL;
}
//-------------------------------------------------------------------------------------------------------
