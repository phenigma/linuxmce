#ifndef __RENDERER_OCG_H__
#define __RENDERER_OCG_H__

#include <string>
using namespace std;
//----------------------------------------------------------------------------------------------------------------
class RendererOCG
{
	string m_sFilename;
	
	char *m_pOCGData;
	size_t m_iOCGSize;

	char *m_pPixelsData;
	size_t m_iPixelsDataSize;
	char *m_pPixelFormatData;
	size_t m_iPixelFormatDataSize;
	int m_iWidth;
	int m_iHeight;

	void Initialize();
    
public:
	RendererOCG();
	RendererOCG(string sOCGFilename);
	RendererOCG(
		char *pPixelsData, size_t iPixelsDataSize, 
		char *pPixelFormatData, size_t iPixelFormatData, 
		int iWidth, int iHeight
	); 
	~RendererOCG();

    bool SaveOCG(string sOCGFilename);
	bool LoadOCG(string sOCGFilename);

	bool SetSurface(
		char *pPixelsData, size_t iPixelsDataSize, 
		char *pPixelFormatData, size_t iPixelFormatData, 
		int iWidth, int iHeight
		); 
	bool SetOCGData(char *pOCGData, size_t iOCGSize);

	bool GetSurface(
		char *& pPixelsData, size_t& iPixelsDataSize, 
		char *& pPixelFormatData, size_t& iPixelFormatData,
		int& iWidth, int& iHeight
	);
	bool GetOCGData(char *& pOCGData, size_t& iOCGSize);

	void Cleanup();
};
//----------------------------------------------------------------------------------------------------------------
#endif
