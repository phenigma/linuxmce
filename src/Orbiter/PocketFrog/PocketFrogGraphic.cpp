#include "PocketFrogGraphic.h"
#include "Logger.h"
#include "PFGHelper.h"
#include "OrbiterRenderer_PocketFrog.h"

#ifndef WINCE
#include "PocketFrog/PNGWrapper.h"
#endif

#define M_PI 3.1415592654

#include "src/internal/graphicbuffer.h"
#include "src/internal/raster2d.h"
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement,
					   OrbiterRenderer *pOrbiterRenderer)
					   : PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(OrbiterRenderer *pOrbiterRenderer)
: PlutoGraphic(pOrbiterRenderer)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
PocketFrogGraphic::PocketFrogGraphic(Surface *pSurface)
{
	Initialize();

	m_pSurface = pSurface;
    Width = pSurface->GetWidth();
    Height = pSurface->GetHeight();
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
	OrbiterRenderer_PocketFrog *pPockerFrogRenderer = 
		dynamic_cast<OrbiterRenderer_PocketFrog *>(m_pOrbiterRenderer);

	if(NULL == pPockerFrogRenderer)
		return false;

    DisplayDevice *pDisplayDevice = pPockerFrogRenderer->GetOrbiterDisplay();

	if(m_GraphicFormat == GR_OCG)
	{
		m_pSurface = PocketFrog_LoadPFG(pDisplayDevice, pData, iSize);
	}
	else
	{
        try
        {
            m_pSurface = LoadImage(pDisplayDevice, (uint8_t*)pData, 
                (uint8_t*)(pData + iSize));
        }
        catch(...)
        {
            //not working
            m_pSurface = NULL;
        }
	}

#ifndef WINCE 
	//windows CE has native support for png files
	//we only need this for windows xp version
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
        
        delete pRasterizer;
        delete pSourceSurface;
    }

	Width = m_pSurface->m_width;
	Height = m_pSurface->m_height;

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

PlutoGraphic *PocketFrogGraphic::GetHighlightedVersion()
{
	OrbiterRenderer_PocketFrog *pPockerFrogRenderer = 
		dynamic_cast<OrbiterRenderer_PocketFrog *>(m_pOrbiterRenderer);

	if(NULL == pPockerFrogRenderer)
		return NULL;

	DisplayDevice *pDisplayDevice = pPockerFrogRenderer->GetOrbiterDisplay();

    Rect srcRect;
    srcRect.Set(0, 0, Width, Height);

    Rasterizer *pSourceRasterizer = pDisplayDevice->CreateRasterizer(m_pSurface);

    Surface *pSurface = pDisplayDevice->CreateSurface(Width, Height);
    Rasterizer *pDestionationRasterizer = pDisplayDevice->CreateRasterizer(pSurface);
    pDestionationRasterizer->Blit(0, 0, m_pSurface, &srcRect);

    Pixel pixelCurrent;
    Pixel pixelHightlighted;
    for (int j = 0; j < Height; j++)
    {
        for (int i = 0; i < Width; i++)
        {
            pixelCurrent = pSourceRasterizer->GetPixel(i, j);

            int R = min(255, OrbiterRenderer_PocketFrog::GetRedColor(pixelCurrent) + 30);
            int G = min(255, OrbiterRenderer_PocketFrog::GetGreenColor(pixelCurrent) + 30);
            int B = min(255, OrbiterRenderer_PocketFrog::GetBlueColor(pixelCurrent) + 30);

            pixelHightlighted = OrbiterRenderer_PocketFrog::GetColor16(PlutoColor(R, G, B, 0));
            pDestionationRasterizer->SetPixel(i, j, pixelHightlighted);
        }
    }

    delete pSourceRasterizer;
    delete pDestionationRasterizer;

    return new PocketFrogGraphic(pSurface);
}

//-------------------------------------------------------------------------------------------------------
PlutoGraphic *PocketFrogGraphic::Clone()
{
	OrbiterRenderer_PocketFrog *pPockerFrogRenderer = 
		dynamic_cast<OrbiterRenderer_PocketFrog *>(m_pOrbiterRenderer);

	if(NULL == pPockerFrogRenderer)
		return NULL;

	DisplayDevice *pDisplayDevice = pPockerFrogRenderer->GetOrbiterDisplay();

	Rect srcRect;
	srcRect.Set(0, 0, Width, Height);

	Surface *pSurface = pDisplayDevice->CreateSurface(Width, Height);
	auto_ptr<Rasterizer> pDestionationRasterizer(pDisplayDevice->CreateRasterizer(pSurface));
	pDestionationRasterizer->Blit(0, 0, m_pSurface, &srcRect);

	return new PocketFrogGraphic(pSurface);
}
//-------------------------------------------------------------------------------------------------------
bool PocketFrogGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{
	return SaveImage(m_pSurface, pRawBitmapData, ulSize);
}
//-------------------------------------------------------------------------------------------------------