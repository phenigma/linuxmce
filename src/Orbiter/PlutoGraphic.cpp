#include "PlutoGraphic.h"
#include "../PlutoUtils/FileUtils.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"
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
	delete[] m_pGraphicData;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void PlutoGraphic::Initialize() 
{ 
	m_GraphicManagement = GR_KEEPCOMPRESSED; 
	m_GraphicFormat = GR_UNKNOWN; 
	m_Filename = "";
	m_pOrbiterRenderer = NULL;
	m_pGraphicData = NULL;
	m_GraphicLength = 0;
}
//-------------------------------------------------------------------------------------------------------

bool PlutoGraphic::LoadGraphicFile(const char *ImageFilename, int iRotation)
{
	if (m_pOrbiterRenderer->m_pOrbiter->m_bIsOSD)
	{
		m_pGraphicData = FileUtils::ReadFileIntoBuffer(ImageFilename, m_GraphicLength);
	}
	else
	{
		int Length=0;

		DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_pOrbiterRenderer->m_pOrbiter->m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false,  BL_SameHouse, ImageFilename,
			&m_pGraphicData, &Length );

		m_pOrbiterRenderer->m_pOrbiter->SendCommand( CMD_Request_File_Cat );
		m_GraphicLength = (size_t)Length;
	}
	if (m_GraphicLength == 0)
		return false;
	
	m_GraphicFormat = GR_JPG; // todo: get format by filename

	return LoadGraphic(m_pGraphicData, m_GraphicLength, iRotation);
}
