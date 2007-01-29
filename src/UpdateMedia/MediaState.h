#ifndef __MEDIA_STATE_H__
#define __MEDIA_STATE_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
#include "PlutoMediaFile.h"
//-----------------------------------------------------------------------------------------------------
struct MediaItemState
{
	int m_nFileID;
	string m_sPath;
	string m_sFilename;
	string m_sCurrentDbAttrDate;
	int m_sCurrentDbAttrCount;
	string m_sOldDbAttrDate;
	int m_sOldDbAttrCount;
	string m_sOldFileDate;
	bool m_bHasAttributes;

	MediaItemState(int nFileID, string sPath, string sFilename, string sCurrentDbAttrDate,
		int sCurrentDbAttrCount, string sOldDbAttrDate, int sOldDbAttrCount, string sOldFileDate,
		bool bHasAttributes
		) :
		m_nFileID(nFileID),
		m_sPath(sPath),
		m_sFilename(sFilename),
		m_sCurrentDbAttrDate(sCurrentDbAttrDate),
		m_sCurrentDbAttrCount(sCurrentDbAttrCount),
		m_sOldDbAttrDate(sOldDbAttrDate),
		m_sOldDbAttrCount(sOldDbAttrCount),
		m_sOldFileDate(sOldFileDate),
		m_bHasAttributes(bHasAttributes)
	{
	}

	MediaItemState() : m_nFileID(0), m_sCurrentDbAttrCount(0), m_sOldDbAttrCount(0), m_bHasAttributes(false)
	{
	}
};
//-----------------------------------------------------------------------------------------------------
typedef map<pair<string /*path*/, string /*filename*/>, MediaItemState > MapMediaState;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
//-----------------------------------------------------------------------------------------------------
class MediaState
{
	MapMediaState m_mapMediaState;

	static MediaState m_instance;

	//helpers
	string ReadMediaFileInfo(string sFilePath);
	MediaItemState LoadDbInfoForFile(Database_pluto_media *pDatabase_pluto_media, int nFileID);

public:

	static MediaState& Instance() { return m_instance; }

	void LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory);
	MediaSyncMode SyncModeNeeded(string sDirectory, string sFile);

	void FileSynchronized(Database_pluto_media *pDatabase_pluto_media, string sDirectory, string sFile, int nFileID);
};
//-----------------------------------------------------------------------------------------------------
#endif
