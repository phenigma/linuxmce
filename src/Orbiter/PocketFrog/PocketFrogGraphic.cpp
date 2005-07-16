#include "PocketFrogGraphic.h"
#include "Logger.h"
#include "PFGHelper.h"

#ifndef WINCE
#include "PocketFrog/PNGWrapper.h"
#endif

#define M_PI 3.1415592654

#include "src/internal/graphicbuffer.h"
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
bool PocketFrogGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
    DisplayDevice *pDisplayDevice = Orbiter_PocketFrog::GetInstance()->GetOrbiterDisplay();

	if(m_GraphicFormat == GR_OCG)
	{
		m_pSurface = PocketFrog_LoadPFG(pData, iSize);
	}
	else
	{
		m_pSurface = LoadImage(pDisplayDevice, (uint8_t*)pData, 
			(uint8_t*)(pData + iSize));
	}

#ifndef WINCE
	if( !m_pSurface ) //maybe it's a png ?
	{
		FileUtils::WriteBufferIntoFile("tmp.img", pData, iSize); //hack :(
		m_pSurface = PocketFrog_LoadPNG(pDisplayDevice, "tmp.img");
	}
#endif

	if( !m_pSurface )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read graphic from data %p with size %d", pData, iSize);
		return false;
	}

    if(iRotation)
    {
		iRotation = 360-iRotation;  // We use cw, they ccw
        Surface *pSourceSurface = m_pSurface;

        m_pSurface = pDisplayDevice->CreateSurface(m_pSurface->m_width, m_pSurface->m_height);
        Rasterizer *pRasterizer = pDisplayDevice->CreateRasterizer(m_pSurface);
        pRasterizer->BlitRotated( m_pSurface->m_width/2, m_pSurface->m_height/2, float(iRotation * M_PI / 180.), pSourceSurface); 

        delete pSourceSurface;
    }

	return true;
}
//-------------------------------------------------------------------------------------------------------
void PocketFrogGraphic::Clear()
{
	if(NULL != m_pSurface)
	{
		if(
			NULL != m_pSurface->m_buffer.get() && 
			!::IsBadReadPtr(m_pSurface->m_buffer->GetPixels(), m_pSurface->GetWidth() * m_pSurface->GetHeight() * 2)
		)
		{
			delete m_pSurface;
		}
	}

	m_pSurface = NULL;
}
//-------------------------------------------------------------------------------------------------------
