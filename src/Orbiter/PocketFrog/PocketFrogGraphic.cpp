#include "PocketFrogGraphic.h"
#include "Logger.h"
#include "PFGHelper.h"
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement,
					   Orbiter *pOrbiter)
					   : PlutoGraphic(Filename, GraphicManagement, pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(Orbiter *pOrbiter)
: PlutoGraphic(pOrbiter)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(Surface *pSurface)
{
	Initialize();

	m_pSurface = pSurface;
}
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::~PocketFrogGraphic()
{
	Clear();
}
//-------------------------------------------------------------------------------------------------------
void PocketFrogGraphic::Initialize()
{
	m_pSurface = NULL;
}
//-------------------------------------------------------------------------------------------------------
bool PocketFrogGraphic::LoadGraphic(char *pData, size_t iSize)
{
	if(m_GraphicFormat == GR_OCG)
	{
		m_pSurface = PocketFrog_LoadPFG(pData, iSize);
	}
	else
	{
		//temp hack
		FileUtils::WriteBufferIntoFile("\\Storage Card\\Image.tmp", pData, iSize);
		m_pSurface = LoadImage( Orbiter_PocketFrog::GetInstance()->GetOrbiterDisplay(), 
			TEXT("\\Storage Card\\Image.tmp"));
	}

	if( !m_pSurface )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read graphic from data %p with size %d", pData, iSize);
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
void PocketFrogGraphic::Clear()
{
	if (m_pSurface)
	{
		delete m_pSurface;
		m_pSurface = NULL;
	}
}
//-------------------------------------------------------------------------------------------------------
