#include "MediaState.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "../PlutoUtils/StringUtils.h"
#include "../pluto_media/Database_pluto_media.h"
//-----------------------------------------------------------------------------------------------------
MediaState MediaState::m_instance;
//-----------------------------------------------------------------------------------------------------
void MediaState::UpdateDbStateForFile(int nPK_File, int nAttributeCount, string sDbTimestamp)
{
	m_mapDbTimestamps[nPK_File].first = nAttributeCount;
	m_mapDbTimestamps[nPK_File].second = sDbTimestamp;
}
//-----------------------------------------------------------------------------------------------------
void MediaState::UpdateFileSystemStateForFile(pair<string, string> pairFilePath, int nPK_File, string sTimestamp)
{
	m_mapFilesState[pairFilePath].first = nPK_File;
	m_mapFilesState[pairFilePath].second = sTimestamp;
}
//-----------------------------------------------------------------------------------------------------
bool MediaState::NeedsSync(string sDirectory, string sFile, string sCurrentDbTimestamp, 
	int nCurrentAttributesCount, string sCurrentFileTimestamp)
{
	MapFileState::iterator it = m_mapFilesState.find(make_pair(sDirectory, sFile));
	if(it != m_mapFilesState.end())
	{
		int nPK_File = it->second.first;
		string sFileTimestamp = it->second.second;

		if(sFileTimestamp == sCurrentFileTimestamp)
		{
			MapDbState::iterator it_db = m_mapDbTimestamps.find(nPK_File);
			if(it_db != m_mapDbTimestamps.end())
			{
				int nAttributesCount = it_db->second.first;
				string sDbTimestamp = it_db->second.second;

				if(sDbTimestamp == sCurrentDbTimestamp && nAttributesCount == nCurrentAttributesCount)
					return false;
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
void MediaState::ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, int nPK_File,
	string& sDbTimestamp, int& nAttributesCount)
{
	nAttributesCount = 0;
	sDbTimestamp = "";

	string sSql = 
		"SELECT GREATEST(MAX(File.psc_mod), MAX(Attribute.psc_mod), MAX(File_Attribute.psc_mod)), COUNT(*) "
		"FROM File "
		"LEFT JOIN File_Attribute ON File_Attribute.FK_File = File.PK_File "
		"LEFT JOIN Attribute ON Attribute.PK_Attribute = File_Attribute.FK_Attribute "
		"WHERE PK_File = " + StringUtils::ltos(nPK_File);

	PlutoSqlResult allresult;
	if((allresult.r = pDatabase_pluto_media->mysql_query_result(sSql)))
	{
		MYSQL_ROW row = mysql_fetch_row(allresult.r);
		if(NULL != row && NULL != row[0] && NULL != row[1])
		{
			sDbTimestamp = row[0];
			nAttributesCount = atoi(row[1]);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaState::ReadDbInfo(Database_pluto_media *pDatabase_pluto_media, pair<string, string> pairFilePath,
	string& sDbTimestamp, int& nAttributesCount)
{
	nAttributesCount = 0;
	sDbTimestamp = "";

	string sSql = 
		"SELECT GREATEST(MAX(File.psc_mod), MAX(Attribute.psc_mod), MAX(File_Attribute.psc_mod)), COUNT(*) "
		"FROM File "
		"LEFT JOIN File_Attribute ON File_Attribute.FK_File = File.PK_File "
		"LEFT JOIN Attribute ON Attribute.PK_Attribute = File_Attribute.FK_Attribute "
		"WHERE Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(pairFilePath.first)) + 
		"' AND Filename = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(pairFilePath.second)) + "'";

	PlutoSqlResult allresult;
	if((allresult.r = pDatabase_pluto_media->mysql_query_result(sSql)))
	{
		MYSQL_ROW row = mysql_fetch_row(allresult.r);
		if(NULL != row && NULL != row[0] && NULL != row[1])
		{
			sDbTimestamp = row[0];
			nAttributesCount = atoi(row[1]);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaState::ReadDbInfoForAllFiles(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory, 
	MapDbStateEx& mapCurrentDbState)
{
	string sSql = 
		"SELECT Path, Filename, GREATEST(MAX(File.psc_mod), MAX(Attribute.psc_mod), MAX(File_Attribute.psc_mod)), COUNT(*) "
		"FROM File "
		"LEFT JOIN File_Attribute ON File_Attribute.FK_File = File.PK_File "
		"LEFT JOIN Attribute ON Attribute.PK_Attribute = File_Attribute.FK_Attribute "
		"WHERE Path LIKE '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sRootDirectory)) + "%' "
		"GROUP BY Path, Filename";

	enum SqlFields
	{
		sfPath,
		sfFilename,
		sfMaxTimestamp,
		sfAttrCount
	};

	MYSQL_ROW row;
	PlutoSqlResult allresult;
	if(NULL != (allresult.r = pDatabase_pluto_media->mysql_query_result(sSql)))
	{
		while((row = mysql_fetch_row(allresult.r)))
		{
			if(NULL != row && NULL != row[sfPath] && NULL != row[sfFilename] && NULL != row[sfMaxTimestamp] && NULL != row[sfAttrCount])
			{
				string sPath = row[sfPath];
				string sFilename = row[sfFilename];
				string sDbTimestamp = row[sfMaxTimestamp];
				int nAttributesCount = atoi(row[sfAttrCount]);

				mapCurrentDbState[make_pair(sPath, sFilename)] = make_pair(nAttributesCount, sDbTimestamp);
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaState::ReadFileInfo(pair<string, string> pairFilePath, string& sFileTimestamp)
{
	sFileTimestamp = "";

#ifdef WIN32
	struct __stat64 buf;
#else
	struct stat64 buf;
#endif

	string sFilePath = pairFilePath.first + "/" + pairFilePath.second;

#ifdef WIN32
	if(!_stat64(sFilePath.c_str(), &buf))
#else
	if(!stat64(sFilePath.c_str(), &buf))
#endif
	{
		sFileTimestamp = StringUtils::SQLDateTime((time_t)buf.st_mtime);
	}
}
//-----------------------------------------------------------------------------------------------------