#include "CacheImageManager.h"

#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#include <algorithm>

using namespace DCE;
//----------------------------------------------------------------------------------------
bool CompareCacheImageData(const CacheImageData* c1, const CacheImageData* c2)
{
	return c1->m_iPriority < c2->m_iPriority;
}
//----------------------------------------------------------------------------------------
CacheImageManager::CacheImageManager(string sCacheFolder, int iCacheSize)
{
	m_sCacheFolder = sCacheFolder;
	m_iCacheSize = iCacheSize * 1024 * 1024; //size in bytes

	if(FileUtils::DirExists(m_sCacheFolder) && FileUtils::FileExists(m_sCacheFolder + "/CacheTimeStamp.txt"))
	{
		size_t iSize;
		char *pData = FileUtils::ReadFileIntoBuffer(m_sCacheFolder + "/CacheTimeStamp.txt", iSize);
		m_sTimeStamp = pData;
		delete pData;
		pData = NULL;
	}
	else
		m_sTimeStamp = "";
}
//----------------------------------------------------------------------------------------
CacheImageManager::~CacheImageManager()
{
	for(size_t i = 0; i < m_vectCacheInfo.size(); i++)
		delete m_vectCacheInfo[i];

	m_vectCacheInfo.clear();
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::VerifyCache(string sTimeStamp)
{
	g_pPlutoLogger->Write(LV_WARNING, "Server skins timestamp: %s. Cache skins timestamp: %s",
		sTimeStamp.c_str(), m_sTimeStamp.c_str());

	if(sTimeStamp != m_sTimeStamp)
	{
		g_pPlutoLogger->Write(LV_WARNING, "New skins are available. We'll purge the cache");

		if(!ClearCache())
			return false;
	}

	m_sTimeStamp = sTimeStamp;
	FileUtils::WriteBufferIntoFile(m_sCacheFolder + "/CacheTimeStamp.txt", 
		const_cast<char *>(m_sTimeStamp.c_str()), m_sTimeStamp.length());
	return true;
}
//----------------------------------------------------------------------------------------
void CacheImageManager::CacheImage(char *pData, int iSize, string sFileName, 
								   int iPriority)
{
	if(IsImageInCache(sFileName))
	{
		//g_pPlutoLogger->Write(LV_STATUS, "The image '%s' in already in cache.", sFileName.c_str());
		return;
	}

	CacheImageData* pCacheImageData = new CacheImageData(sFileName, iSize, iPriority);
	m_vectCacheInfo.push_back(pCacheImageData);
	sort(m_vectCacheInfo.begin(), m_vectCacheInfo.end(), CompareCacheImageData);

	AdjustCacheSize();

	if(IsImageInCache(sFileName))
	{
		if(!FileUtils::WriteBufferIntoFile(GetCacheImageFileName(sFileName), pData, iSize))
			g_pPlutoLogger->Write(LV_WARNING, "Unable to add image '%s' to cache", sFileName.c_str());
		else
			g_pPlutoLogger->Write(LV_STATUS, "Added image '%s' to cache", sFileName.c_str());
	}
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::IsImageInCache(string sFileName, int iPriority/*=0*/)
{
	for(size_t i = 0; i < m_vectCacheInfo.size(); i++)
	{
		CacheImageData* pCacheImageData = m_vectCacheInfo[i];

		if(pCacheImageData->m_sFileName == sFileName)
			return true;
	}

	if(iPriority == -1)
		return false;

	//file exists in cache folder 
	string sCacheImageFileName = GetCacheImageFileName(sFileName);
	if(FileUtils::FileExists(GetCacheImageFileName(sFileName))) 
	{
		int iSize = FileUtils::FileSize(sFileName);
		CacheImageData* pCacheImageData = new CacheImageData(sFileName, iSize, iPriority);
		m_vectCacheInfo.push_back(pCacheImageData);
		sort(m_vectCacheInfo.begin(), m_vectCacheInfo.end(), CompareCacheImageData);

		AdjustCacheSize();

		g_pPlutoLogger->Write(LV_STATUS, "Image '%s' found in cache folder. Added to our list.", sFileName.c_str());
		return IsImageInCache(sFileName);
	}

	return false;
}
//----------------------------------------------------------------------------------------
string CacheImageManager::GetCacheImageFileName(string sFileName)
{
	return m_sCacheFolder + "/" + sFileName;
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::ClearCache()
{
	if(FileUtils::DirExists(m_sCacheFolder))
		FileUtils::DelDir(m_sCacheFolder);

	FileUtils::MakeDir(m_sCacheFolder);
	if(!FileUtils::DirExists(m_sCacheFolder))
	{
		g_pPlutoLogger->Write(LV_WARNING, "Unable to create cache folder %s", m_sCacheFolder.c_str());
		return false;
	}

	return true;
}
//----------------------------------------------------------------------------------------
int CacheImageManager::GetCacheActualSize()
{
	int iCacheActualSize = 0;

	for(size_t i = 0; i < m_vectCacheInfo.size(); i++)
		iCacheActualSize += m_vectCacheInfo[i]->m_iImageSize;

	return iCacheActualSize;
}
//----------------------------------------------------------------------------------------
void CacheImageManager::AdjustCacheSize()
{
	while(GetCacheActualSize() > m_iCacheSize)
	{
		CacheImageData* pCacheImageData = m_vectCacheInfo.front();
        m_vectCacheInfo.erase(m_vectCacheInfo.begin());

		FileUtils::DelFile(GetCacheImageFileName(pCacheImageData->m_sFileName));
		if(NULL != pCacheImageData)
		{
			delete pCacheImageData;
			pCacheImageData = NULL;
		}
	}
}
//----------------------------------------------------------------------------------------