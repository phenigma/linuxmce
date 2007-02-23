/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
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
