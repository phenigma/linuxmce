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
void MediaState::LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory)
{
	string sSql = 
		"SELECT PK_File, Path, Filename, "
		"GREATEST(MAX(Bookmark.psc_mod), MAX(File_Attribute.psc_mod), MAX(Attribute.psc_mod), MAX(LongAttribute.psc_mod), MAX(Picture_File.psc_mod)) AS CurrentDbAttrDate, "
		"(COUNT(Bookmark.PK_Bookmark) + COUNT(File_Attribute.FK_Attribute) + COUNT(Attribute.PK_Attribute) + COUNT(LongAttribute.PK_LongAttribute) + COUNT(Picture_File.FK_Picture)) AS CurrentDbAttrCount, "
		"AttrDate AS OldDbAttrDate, AttrCount AS OldDbAttrCount, ModificationDate AS OldFileDate "
		"FROM File "
		"LEFT JOIN Bookmark ON Bookmark.FK_File = PK_File "
		"LEFT JOIN File_Attribute ON File_Attribute.FK_File = PK_File "
		"LEFT JOIN Attribute ON File_Attribute.FK_Attribute = PK_Attribute "
		"LEFT JOIN LongAttribute ON LongAttribute.FK_File = PK_File "
		"LEFT JOIN Picture_File ON Picture_File.FK_File = PK_File "
		"WHERE Path LIKE '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sRootDirectory), true) + "%' "
		"OR (Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(FileUtils::BasePath(sRootDirectory))) + "' AND "
		"Filename = '" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sRootDirectory)) + "' ) "
		"GROUP BY PK_File, Path, Filename";

	enum SqlFields
	{
		sfFileID,
		sfPath,
		sfFilename,
		sfCurrentDbAttrDate,
		sfCurrentDbAttrCount,
		sfOldDbAttrDate,
		sfOldDbAttrCount,
		sfOldFileDate
	};

	MYSQL_ROW row;
	PlutoSqlResult allresult;
	if(NULL != (allresult.r = pDatabase_pluto_media->mysql_query_result(sSql)))
	{
		while((row = mysql_fetch_row(allresult.r)))
		{
			if(NULL != row && NULL != row[sfFileID] && NULL != row[sfPath] && NULL != row[sfFilename])
			{
				int nFileID = atoi(row[sfFileID]);
				string sPath = row[sfPath];
				string sFilename = row[sfFilename];

				string sCurrentDbAttrDate = NULL != row[sfCurrentDbAttrDate] ? row[sfCurrentDbAttrDate] : string();
				int sCurrentDbAttrCount = NULL != row[sfCurrentDbAttrCount] ? atoi(row[sfCurrentDbAttrCount]) : 0;
				string sOldDbAttrDate = NULL != row[sfOldDbAttrDate] ? row[sfOldDbAttrDate] : string();
				int sOldDbAttrCount = NULL != row[sfOldDbAttrCount] ? atoi(row[sfOldDbAttrCount]) : 0;
				string sOldFileDate = NULL != row[sfOldFileDate] ? row[sfOldFileDate] : string();

				m_mapMediaState[make_pair(sPath, sFilename)] = MediaItemState(nFileID, sPath, sFilename,
					sCurrentDbAttrDate, sCurrentDbAttrCount, sOldDbAttrDate, sOldDbAttrCount, sOldFileDate);
			}
		}
	}	
}
//-----------------------------------------------------------------------------------------------------
MediaSyncMode MediaState::SyncModeNeeded(string sDirectory, string sFile)
{
	MediaSyncMode sync_mode = modeNone;
	bool bNeedToUpdateDb = false;
	bool bNeedtoUpdateFile = false;

	string sCurrentFileDate = ReadFileInfo(sDirectory + "/" + sFile);

	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	if(it != m_mapMediaState.end())
	{
		MediaItemState item = it->second;
		if(StringUtils::SQLDateTime(item.m_sOldFileDate) != StringUtils::SQLDateTime(sCurrentFileDate))
			bNeedToUpdateDb = true;

		if(
			item.m_sCurrentDbAttrCount != item.m_sOldDbAttrCount || 
			StringUtils::SQLDateTime(item.m_sCurrentDbAttrDate) != StringUtils::SQLDateTime(item.m_sOldDbAttrDate)
		)
			bNeedtoUpdateFile = true;

		if(bNeedToUpdateDb)
			if(bNeedtoUpdateFile)
				sync_mode = modeBoth;
			else 
				sync_mode = modeFileToDb;
		else
			if(bNeedtoUpdateFile)
				sync_mode = modeDbToFile;
			else
				sync_mode = modeNone;
	}
	else
	{
		sync_mode = modeBoth;
	}

	return sync_mode;
}
//-----------------------------------------------------------------------------------------------------
void MediaState::FileSynchronized(Database_pluto_media *pDatabase_pluto_media, string sDirectory, string sFile, int nFileID)
{
	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	MediaItemState item;
	if(it != m_mapMediaState.end())
		item = it->second;
	else
		item = LoadDbInfoForFile(pDatabase_pluto_media, nFileID);

	string sUpdateSql = "UPDATE File SET ";

	string sFileTimestamp = ReadFileInfo(sDirectory + "/" + sFile);
	sUpdateSql += "ModificationDate = '" + sFileTimestamp + "', ";
	item.m_sOldFileDate = sFileTimestamp;

	sUpdateSql += "AttrCount = " + StringUtils::ltos(item.m_sCurrentDbAttrCount) + ", ";
	sUpdateSql += "AttrDate = '" + item.m_sCurrentDbAttrDate + "' ";
	item.m_sOldDbAttrCount = item.m_sCurrentDbAttrCount;
	item.m_sOldDbAttrDate = item.m_sCurrentDbAttrDate;

	sUpdateSql += "WHERE PK_File = " + StringUtils::ltos(nFileID);

	pDatabase_pluto_media->threaded_mysql_query(sUpdateSql);
	m_mapMediaState[make_pair(sDirectory, sFile)] = item;
}
//-----------------------------------------------------------------------------------------------------
string MediaState::ReadFileInfo(string sFilePath)
{
#ifdef WIN32
	struct __stat64 buf;
#else
	struct stat64 buf;
#endif

#ifdef WIN32
	if(!_stat64(sFilePath.c_str(), &buf))
#else
	if(!stat64(sFilePath.c_str(), &buf))
#endif
	{
		return StringUtils::SQLDateTime((time_t)buf.st_mtime);
	}

	return "";
}
//-----------------------------------------------------------------------------------------------------
MediaItemState MediaState::LoadDbInfoForFile(Database_pluto_media *pDatabase_pluto_media, int nFileID)
{
	string sSql = 
		"SELECT PK_File, Path, Filename, "
		"GREATEST(MAX(Bookmark.psc_mod), MAX(File_Attribute.psc_mod), MAX(Attribute.psc_mod), MAX(LongAttribute.psc_mod), MAX(Picture_File.psc_mod)) AS CurrentDbAttrDate, "
		"(COUNT(Bookmark.PK_Bookmark) + COUNT(File_Attribute.FK_Attribute) + COUNT(Attribute.PK_Attribute) + COUNT(LongAttribute.PK_LongAttribute) + COUNT(Picture_File.FK_Picture)) AS CurrentDbAttrCount, "
		"AttrDate AS OldDbAttrDate, AttrCount AS OldDbAttrCount, ModificationDate AS OldFileDate "
		"FROM File "
		"LEFT JOIN Bookmark ON Bookmark.FK_File = PK_File "
		"LEFT JOIN File_Attribute ON File_Attribute.FK_File = PK_File "
		"LEFT JOIN Attribute ON File_Attribute.FK_Attribute = PK_Attribute "
		"LEFT JOIN LongAttribute ON LongAttribute.FK_File = PK_File "
		"LEFT JOIN Picture_File ON Picture_File.FK_File = PK_File "
		"WHERE PK_File = " + StringUtils::ltos(nFileID) + " "
		"GROUP BY PK_File, Path, Filename";

	enum SqlFields
	{
		sfFileID,
		sfPath,
		sfFilename,
		sfCurrentDbAttrDate,
		sfCurrentDbAttrCount,
		sfOldDbAttrDate,
		sfOldDbAttrCount,
		sfOldFileDate
	};

	MYSQL_ROW row;
	PlutoSqlResult allresult;
	if(NULL != (allresult.r = pDatabase_pluto_media->mysql_query_result(sSql)))
	{
		if(NULL != (row = mysql_fetch_row(allresult.r)))
		{
			if(NULL != row[sfFileID] && NULL != row[sfPath] && NULL != row[sfFilename])
			{
				int nFileID = atoi(row[sfFileID]);
				string sPath = row[sfPath];
				string sFilename = row[sfFilename];

				string sCurrentDbAttrDate = NULL != row[sfCurrentDbAttrDate] ? row[sfCurrentDbAttrDate] : string();
				int sCurrentDbAttrCount = NULL != row[sfCurrentDbAttrCount] ? atoi(row[sfCurrentDbAttrCount]) : 0;
				string sOldDbAttrDate = NULL != row[sfOldDbAttrDate] ? row[sfOldDbAttrDate] : string();
				int sOldDbAttrCount = NULL != row[sfOldDbAttrCount] ? atoi(row[sfOldDbAttrCount]) : 0;
				string sOldFileDate = NULL != row[sfOldFileDate] ? row[sfOldFileDate] : string();

				return MediaItemState(nFileID, sPath, sFilename,
					sCurrentDbAttrDate, sCurrentDbAttrCount, sOldDbAttrDate, sOldDbAttrCount, sOldFileDate);
			}
		}
	}

	return MediaItemState();
}
//-----------------------------------------------------------------------------------------------------
