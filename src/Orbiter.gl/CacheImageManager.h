#ifndef __CACHE_IMAGE_MANAGER_H__
#define __CACHE_IMAGE_MANAGER_H__

#include <string>
#include <vector>
using namespace std;
//------------------------------------------------------------------------
struct CacheImageData
{
	CacheImageData(string sFileName, int iImageSize, int iPriority)
	{
		m_sFileName = sFileName;
		m_iImageSize = iImageSize;
		m_iPriority = iPriority;
	}

	string m_sFileName;
	int m_iImageSize;
	int m_iPriority;
};
//------------------------------------------------------------------------
class CacheImageManager
{
	string m_sTimeStamp;
	string m_sCacheFolder;
	int m_iCacheSize;

	vector<CacheImageData *> m_vectCacheInfo;

public:
	CacheImageManager(string sCacheFolder, int iCacheSize);
	~CacheImageManager();

	bool VerifyCache(string sTimeStamp);
	void CacheImage(char *pData, int iSize, string sFileName, int iPriority);
	bool IsImageInCache(string sFileName, int iPriority = -1);
	string GetCacheImageFileName(string sFileName);

private:
	bool ClearCache();
	int GetCacheActualSize();
	void AdjustCacheSize();
};
//------------------------------------------------------------------------
#endif
