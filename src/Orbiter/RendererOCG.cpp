#include "RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/minilzo.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/Profiler.h"

//----------------------------------------------------------------------------------------------------------------
#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);
//----------------------------------------------------------------------------------------------------------------
static const long ciOCGMagicStart = 0x1234;
static const long ciOCGMagicEnd = 0x5678;
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG()
{
	Initialize();
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG(string sOCGFilename)
{
	Initialize();
	LoadOCG(sOCGFilename);
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG(char *pPixelsData, size_t iPixelsDataSize, 
						 char *pPixelFormatData, size_t iPixelFormatDataSize, 
						 int iWidth, int iHeight)
{
	Initialize();
	SetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeight);
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::~RendererOCG()
{
	Cleanup();
}
//----------------------------------------------------------------------------------------------------------------
void RendererOCG::Cleanup()
{
	m_sFilename = "";

	PLUTO_SAFE_DELETE_ARRAY(m_pOCGData);  
	m_iOCGSize = 0;
	PLUTO_SAFE_DELETE_ARRAY(m_pPixelsData);
	m_iPixelsDataSize = 0;
	PLUTO_SAFE_DELETE_ARRAY(m_pPixelFormatData);
	m_iPixelFormatDataSize = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}
//----------------------------------------------------------------------------------------------------------------
void RendererOCG::Initialize()
{
	m_sFilename = "";
	m_pOCGData = NULL;
	m_iOCGSize = 0;
	m_pPixelsData = NULL;
	m_iPixelsDataSize = 0;	
	m_pPixelFormatData = NULL;
	m_iPixelFormatDataSize = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SaveOCG(string sOCGFilename)
{
	return FileUtils::WriteBufferIntoFile(sOCGFilename, m_pOCGData, m_iOCGSize);
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::LoadOCG(string sOCGFilename)
{
	Cleanup();

	size_t iOCGSize;
	char *pOCGData = FileUtils::ReadFileIntoBuffer(sOCGFilename, iOCGSize);

	if(!pOCGData)
		return false;

	return SetOCGData(pOCGData, iOCGSize);

	PLUTO_SAFE_DELETE_ARRAY(pOCGData);
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SetSurface(char *pPixelsData, size_t iPixelsDataSize, 
							 char *pPixelFormatData, size_t iPixelFormatDataSize, 
							 int iWidth, int iHeight)
{
	Cleanup();

	m_pPixelsData = pPixelsData;
	m_iPixelsDataSize = iPixelsDataSize;
	m_pPixelFormatData = pPixelFormatData;
	m_iPixelFormatDataSize = iPixelFormatDataSize;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	//compress pixels data
	char *pCompressedData = new char[m_iPixelsDataSize + m_iPixelsDataSize / 64 + 16 + 3 + 4];
	size_t iCompressedDataSize;
	lzo1x_1_compress((lzo_byte *)m_pPixelsData, (lzo_uint)m_iPixelsDataSize, (lzo_byte *)pCompressedData, (lzo_uint *)(&iCompressedDataSize), wrkmem);

	//serialize data
	m_pOCGData = new char[4 + 4 + 4 + 4 + m_iPixelFormatDataSize + 4 + iCompressedDataSize + 4 + 4];
	m_iOCGSize = 0;

	//magic start
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&ciOCGMagicStart), sizeof(ciOCGMagicStart));
	m_iOCGSize += sizeof(ciOCGMagicStart);
	//width
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iWidth), sizeof(m_iWidth));
	m_iOCGSize += sizeof(m_iWidth);
	//heigth
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iHeight), sizeof(m_iHeight));
	m_iOCGSize += sizeof(m_iHeight);

	//pixels format size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iPixelFormatDataSize), sizeof(m_iPixelFormatDataSize));
	m_iOCGSize += sizeof(m_iPixelFormatDataSize);
	//pixels format data
	memcpy(m_pOCGData + m_iOCGSize, (char *)(m_pPixelFormatData), m_iPixelFormatDataSize);
	m_iOCGSize += m_iPixelFormatDataSize;

	//compressed data size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&iCompressedDataSize), sizeof(iCompressedDataSize));
	m_iOCGSize += sizeof(iCompressedDataSize);
	//compressed data
	memcpy(m_pOCGData + m_iOCGSize, (char *)(pCompressedData), iCompressedDataSize);
	m_iOCGSize += iCompressedDataSize;

	//uncompressed data size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iPixelsDataSize), sizeof(m_iPixelsDataSize));
	m_iOCGSize += sizeof(m_iPixelsDataSize);

	//magic end
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&ciOCGMagicEnd), sizeof(ciOCGMagicEnd));
	m_iOCGSize += sizeof(ciOCGMagicEnd);

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SetOCGData(char *pOCGData, size_t iOCGSize)//it's user responsability to delete pOCGData!!!
{
g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - Cleanup");
	Cleanup();
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - Cleanup");

g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - memcpy");
	m_iOCGSize = iOCGSize;
	m_pOCGData = new char[iOCGSize];
	memcpy(m_pOCGData, pOCGData, iOCGSize);
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - memcpy");

g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - deserialize");
	//deserialize OCG data
	size_t iOffset = 0;

	//magic start
	long iMagicStart = *(long *)(m_pOCGData + iOffset);
	iOffset += sizeof(iMagicStart);
	if(iMagicStart != ciOCGMagicStart)
		return false;

	//width
	m_iWidth = *(long *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iWidth);
	//height
	m_iHeight = *(long *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iHeight);

	//pixel format data size
	m_iPixelFormatDataSize = *(long *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iPixelFormatDataSize);
	//pixel format data
	m_pPixelFormatData = new char[m_iPixelFormatDataSize];
	memcpy(m_pPixelFormatData, m_pOCGData + iOffset, m_iPixelFormatDataSize);
	iOffset += m_iPixelFormatDataSize;

	//compressed data size
	size_t iCompressedDataSize = *(long *)(m_pOCGData + iOffset);
	iOffset += sizeof(iCompressedDataSize);
	//compressed data
	char *pCompressedData = new char[iCompressedDataSize];
	memcpy(pCompressedData, m_pOCGData + iOffset, iCompressedDataSize);
	iOffset += iCompressedDataSize;

	//uncompressed data size
	//int iUncompressedSize = *(long *)(m_pOCGData + iOffset);


		//WinCE issue :(
		unsigned char v1 = ((unsigned char *)(m_pOCGData + iOffset))[0];
		unsigned char v2 = ((unsigned char *)(m_pOCGData + iOffset))[1];
		unsigned char v3 = ((unsigned char *)(m_pOCGData + iOffset))[2];
		unsigned char v4 = ((unsigned char *)(m_pOCGData + iOffset))[3];

		int iUncompressedSize = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;



	iOffset += sizeof(iUncompressedSize);

	//magic end
	//long iMagicEnd = *(long *)(m_pOCGData + iOffset);

		//WinCE issue :(
		v1 = ((unsigned char *)(m_pOCGData + iOffset))[0];
		v2 = ((unsigned char *)(m_pOCGData + iOffset))[1];
		v3 = ((unsigned char *)(m_pOCGData + iOffset))[2];
		v4 = ((unsigned char *)(m_pOCGData + iOffset))[3];

		int iMagicEnd = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - deserialize");
g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - iMagicEnd");

	iOffset += sizeof(iMagicEnd);
	if(iMagicEnd != ciOCGMagicEnd || iOffset != m_iOCGSize)
	{
		PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
		return false;
	}

g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - iMagicEnd");

g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - m_pPixelsData");
	m_pPixelsData = new char[iUncompressedSize];
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - m_pPixelsData");

g_PlutoProfiler->Start("PocketFrog_LoadPFG prep - SetOCGData - lzo1x_decompress");
	lzo1x_decompress((lzo_byte *)pCompressedData, (lzo_uint)iCompressedDataSize, (lzo_byte *)m_pPixelsData, (lzo_uint *)(&m_iPixelsDataSize), wrkmem);
g_PlutoProfiler->Stop("PocketFrog_LoadPFG prep - SetOCGData - lzo1x_decompress");

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::GetSurface(char *& pPixelsData, size_t& iPixelsDataSize, 
							 char *& pPixelFormatData, size_t& iPixelFormatDataSize,
							 int& iWidth, int& iHeight)
{
	iWidth = m_iWidth;
	iHeight = m_iHeight;

	iPixelsDataSize = m_iPixelsDataSize;
	pPixelsData = m_pPixelsData;
	iPixelFormatDataSize = m_iPixelFormatDataSize;
	pPixelFormatData = m_pPixelFormatData;

	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::GetOCGData(char *& pOCGData, size_t& iOCGSize)
{
	//TODO: not implemented

	return true;
}
//----------------------------------------------------------------------------------------------------------------
