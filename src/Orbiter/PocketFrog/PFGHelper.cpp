#include "PFGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "../PocketFrog/Orbiter_PocketFrog.h"
#include "src/internal/graphicbuffer.h"

//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(char *pOCGData, size_t iOCGDataSize)
{
	RendererOCG *pRendererOCG = new RendererOCG();
	pRendererOCG->SetOCGData(pOCGData, iOCGDataSize);
	Surface *pSurface = NULL;

	char *pPixelsData;
	size_t iPixelsDataSize;
	char *pPixelFormatData;	
	size_t iPixelFormatDataSize;
	int iWidth;
	int iHeigth;

	if(pRendererOCG->GetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeigth))
	{
		if(iPixelsDataSize)
		{
			pSurface = Orbiter_PocketFrog::GetInstance()->GetOrbiterDisplay()->CreateSurface(iWidth, iHeigth);

			Pixel * pPixels = pSurface->m_buffer->GetPixels();
			memcpy((char *)pPixels, pPixelsData, iPixelsDataSize);
		}
	}
else

	PLUTO_SAFE_DELETE(pRendererOCG);

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