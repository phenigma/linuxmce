#ifndef __MEDIA_STATE_H__
#define __MEDIA_STATE_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
typedef map<int /*pk_file*/, pair<int /*attr count*/, string /*max timestamp*/> > MapDbState;
typedef map<pair<string /*path*/, string /*filename*/>, pair<int /*attr count*/, string /*max timestamp*/> > MapDbStateEx;
typedef map<pair<string /*path*/, string /*filename*/>, pair<int /*pk_file*/, string /*timestamp*/> > MapFileState;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
//-----------------------------------------------------------------------------------------------------
class MediaState
{
	MapDbState m_mapDbTimestamps;
	MapFileState m_mapFilesState;

	static MediaState m_instance;

public:

	static MediaState& Instance() { return m_instance; }

	void UpdateDbStateForFile(int nPK_File, int nAttributeCount, string sDbTimestamp);
	void UpdateFileSystemStateForFile(pair<string, string> pairFilePath, int nPK_File, string sTimestamp);
	bool NeedsSync(string sDirectory, string sFile, string sCurrentDbTimestamp, 
		int nCurrentAttributesCount, string sCurrentFileTimestamp);

	//helpers
	void ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, int nPK_File,
		string& sDbTimestamp, int& nAttributesCount);
	void ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, pair<string, string> pairFilePath,
		string& sDbTimestamp, int& nAttributesCount);
	void ReadDbInfoForAllFiles(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory, 
		MapDbStateEx& mapCurrentDbState);

	void ReadFileInfo(pair<string, string> pairFilePath, string& sFileTimestamp);
};
//-----------------------------------------------------------------------------------------------------
#endif
