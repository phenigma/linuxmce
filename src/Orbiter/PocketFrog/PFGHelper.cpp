#include "PFGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "../PocketFrog/Orbiter_PocketFrog.h"
#include "src/internal/graphicbuffer.h"

#include "PlutoUtils/Profiler.h"

//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(char *pOCGData, size_t iOCGDataSize)
{
g_PlutoProfiler->Start("PocketFrog_LoadPFG prep 1");
	RendererOCG *pRendererOCG = new RendererOCG();
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep 1");
g_PlutoProfiler->Start("PocketFrog_LoadPFG prep 2");
	pRendererOCG->SetOCGData(pOCGData, iOCGDataSize);
	Surface *pSurface = NULL;

	char *pPixelsData;
	size_t iPixelsDataSize;
	char *pPixelFormatData;	
	size_t iPixelFormatDataSize;
	int iWidth;
	int iHeigth;
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep 2");

g_PlutoProfiler->Start("PocketFrog_LoadPFG pRendererOCG->GetSurface");
	if(pRendererOCG->GetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeigth))
	{
g_PlutoProfiler->Stop("PocketFrog_LoadPFG pRendererOCG->GetSurface");
		if(iPixelsDataSize)
		{
g_PlutoProfiler->Start("PocketFrog_LoadPFG pRendererOCG->CreateSurface");
			pSurface = Orbiter_PocketFrog::GetInstance()->GetOrbiterDisplay()->CreateSurface(iWidth, iHeigth);
g_PlutoProfiler->Stop("PocketFrog_LoadPFG pRendererOCG->CreateSurface");

g_PlutoProfiler->Start("PocketFrog_LoadPFG GetPixels");
			Pixel * pPixels = pSurface->m_buffer->GetPixels();
g_PlutoProfiler->Stop("PocketFrog_LoadPFG GetPixels");
g_PlutoProfiler->Start("PocketFrog_LoadPFG memcpy");
			memcpy((char *)pPixels, pPixelsData, iPixelsDataSize);
g_PlutoProfiler->Stop("PocketFrog_LoadPFG memcpy");
		}
	}
else
g_PlutoProfiler->Stop("PocketFrog_LoadPFG pRendererOCG->GetSurface");


g_PlutoProfiler->Start("PocketFrog_LoadPFG del");
	PLUTO_SAFE_DELETE(pRendererOCG);
g_PlutoProfiler->Stop("PocketFrog_LoadPFG del");
	return pSurface; 
}
//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(string sFilename)
{
	size_t iSize;
	char *pData = FileUtils::ReadFileIntoBuffer(sFilename, iSize);

	if(!pData)
		return NULL;

	Surface *pSurface = PocketFrog_LoadPFG(pData, iSize);

	PLUTO_SAFE_DELETE_ARRAY(pData);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
bool PocketFrog_SavePFG(Surface *pSurface, string sFilename)
{
	int iPixelsDataSize = pSurface->m_height * pSurface->m_buffer->GetPitch();//pSurface.pitch;
	char *pPixelsData = new char[iPixelsDataSize];
	char *pPixels = (char *)pSurface->m_buffer->GetPixels();
	memcpy(pPixelsData, pPixels, iPixelsDataSize);

	int iPixelFormatDataSize = 4;
	char dummy[] = {0, 0, 0, 0};
	char *pPixelFormatData = new char[iPixelFormatDataSize];
	memcpy(pPixelFormatData, dummy, iPixelFormatDataSize);

	RendererOCG *pRendererOCG = new RendererOCG(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, 
		pSurface->m_width, pSurface->m_height);

	bool bResult = pRendererOCG->SaveOCG(sFilename);
	PLUTO_SAFE_DELETE(pRendererOCG);	

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------