#include "Orbiter.h"
#include "PlutoGraphic.h"

//=======================================================================================================
//Generic PlutoGraphic class methods
//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic()
{
	Initialize();

	Width = 0;
	Height = 0;
}

PlutoGraphic::PlutoGraphic(Orbiter *pOrbiter)
{
	Initialize();

	m_pOrbiter = pOrbiter;
	Width = 0;
	Height = 0;
}

//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pOrbiter)
{
	Initialize();

	m_pOrbiter = pOrbiter;
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
}



