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

	MediaItemState(int nFileID, string sPath, string sFilename, string sCurrentDbAttrDate,
		int sCurrentDbAttrCount, string sOldDbAttrDate, int sOldDbAttrCount, string sOldFileDate
		) :
		m_nFileID(nFileID),
		m_sPath(sPath),
		m_sFilename(sFilename),
		m_sCurrentDbAttrDate(sCurrentDbAttrDate),
		m_sCurrentDbAttrCount(sCurrentDbAttrCount),
		m_sOldDbAttrDate(sOldDbAttrDate),
		m_sOldDbAttrCount(sOldDbAttrCount),
		m_sOldFileDate(sOldFileDate)
	{
	}

	MediaItemState() : m_nFileID(0), m_sCurrentDbAttrCount(0), m_sOldDbAttrCount(0)
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
	string ReadFileInfo(string sFilePath);
	MediaItemState LoadDbInfoForFile(Database_pluto_media *pDatabase_pluto_media, int nFileID);

public:

	static MediaState& Instance() { return m_instance; }

	void LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory);
	MediaSyncMode SyncModeNeeded(string sDirectory, string sFile);

	void FileSynchronized(Database_pluto_media *pDatabase_pluto_media, string sDirectory, string sFile, int nFileID, MediaSyncMode sync_mode);

/*
	void UpdateDbStateForFile(int nPK_File, int nAttributeCount, string sDbTimestamp);
	void UpdateFileSystemStateForFile(pair<string, string> pairFilePath, int nPK_File, string sTimestamp);

	//helpers
	void ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, int nPK_File,
		string& sDbTimestamp, int& nAttributesCount);
	void ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, pair<string, string> pairFilePath,
		string& sDbTimestamp, int& nAttributesCount);
	void ReadDbInfoForAllFiles(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory, 
		MapDbStateEx& mapCurrentDbState);
	void ReadFileInfo(pair<string, string> pairFilePath, string& sFileTimestamp);
	void ReadDbInfoAboutFilesChanges(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory);

	int GetModifiedData(pair<string, string> pairFilePath, string& sModificationData);
*/
};
//-----------------------------------------------------------------------------------------------------
#endif
