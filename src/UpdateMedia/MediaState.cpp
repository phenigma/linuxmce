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
#include "MediaState.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "../PlutoUtils/StringUtils.h"
#include "../pluto_media/Database_pluto_media.h"
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
MediaState MediaState::m_instance;
//-----------------------------------------------------------------------------------------------------
void MediaState::LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory)
{
	string sSql = 
		"SELECT PK_File, Path, Filename, "
		"GREATEST("
		"	IF(File.psc_mod IS NULL,CAST('0000-00-00 00:00:00' AS DATE),File.psc_mod), "
		"	IF(MAX(Bookmark.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Bookmark.psc_mod)), "
		"	IF(MAX(File_Attribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(File_Attribute.psc_mod)), "
		"	IF(MAX(Attribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Attribute.psc_mod)), "
		"	IF(MAX(LongAttribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(LongAttribute.psc_mod)), "
		"	IF(MAX(Picture_File.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Picture_File.psc_mod)) "
		") AS CurrentDbAttrDate, "
		"(COUNT(Bookmark.PK_Bookmark) + COUNT(File_Attribute.FK_Attribute) + COUNT(Attribute.PK_Attribute) + COUNT(LongAttribute.PK_LongAttribute) + COUNT(Picture_File.FK_Picture)) AS CurrentDbAttrCount, "
		"AttrDate AS OldDbAttrDate, AttrCount AS OldDbAttrCount, ModificationDate AS OldFileDate, "
		"(COUNT(File_Attribute.FK_Attribute) + COUNT(LongAttribute.PK_LongAttribute)) AS HasAttributes "
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
		sfOldFileDate,
		sfHasAttributes
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
				bool bHasAttributes = NULL != row[sfHasAttributes] ? atoi(row[sfHasAttributes]) > 0 : false;

				m_mapMediaState[make_pair(sPath, sFilename)] = MediaItemState(nFileID, sPath, sFilename,
					sCurrentDbAttrDate, sCurrentDbAttrCount, 
					sOldDbAttrDate, sOldDbAttrCount, sOldFileDate, bHasAttributes);
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

	string sCurrentFileDate = ReadMediaFileInfo(sDirectory + "/" + sFile);

	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	if(it != m_mapMediaState.end())
	{
		MediaItemState item = it->second;
		if(StringUtils::SQLDateTime(item.m_sOldFileDate) != StringUtils::SQLDateTime(sCurrentFileDate))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to update db for %s/%s: old file data %s, current file date %s", 
				sDirectory.c_str(), sFile.c_str(), 
				item.m_sOldFileDate.c_str(), sCurrentFileDate.c_str()); 
			bNeedToUpdateDb = true;
		}

		if(
			item.m_sCurrentDbAttrCount != item.m_sOldDbAttrCount || 
			StringUtils::SQLDateTime(item.m_sCurrentDbAttrDate) != StringUtils::SQLDateTime(item.m_sOldDbAttrDate)
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to update file for %s/%s: "
				"old attr count %d, current attr count %d, "
				"old attr date %s, current attr date %s",
				sDirectory.c_str(), sFile.c_str(), 
				item.m_sCurrentDbAttrCount, item.m_sOldDbAttrCount,
				item.m_sCurrentDbAttrDate.c_str(), item.m_sOldDbAttrDate.c_str()); 
			bNeedtoUpdateFile = true;
		}

		//if we have a media file with external id3 file missing
		//and it haas attributes in the database
		if(
			!PlutoMediaFile::IsSupported(sFile) && 
			!FileUtils::FileExists(sDirectory + "/" + sFile + ".id3") && 
			item.m_sCurrentDbAttrDate != "" && item.m_bHasAttributes
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to update file for %s/%s: "
				"current attr date %s, has attr %d",
				sDirectory.c_str(), sFile.c_str(), 
				item.m_sCurrentDbAttrDate.c_str(), item.m_bHasAttributes); 
			bNeedtoUpdateFile = true;
		}

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

	string sFileTimestamp = ReadMediaFileInfo(sDirectory + "/" + sFile);
	sUpdateSql += "ModificationDate = '" + sFileTimestamp + "', ";
	item.m_sOldFileDate = sFileTimestamp;

	sUpdateSql += "AttrCount = " + StringUtils::ltos(item.m_sCurrentDbAttrCount) + ", ";
	sUpdateSql += "AttrDate = NOW() ";
	item.m_sOldDbAttrCount = item.m_sCurrentDbAttrCount;
	item.m_sOldDbAttrDate = item.m_sCurrentDbAttrDate;

	sUpdateSql += "WHERE PK_File = " + StringUtils::ltos(nFileID);

	if(nFileID != 0)
		pDatabase_pluto_media->threaded_mysql_query(sUpdateSql);

	m_mapMediaState[make_pair(sDirectory, sFile)] = item;
}
//-----------------------------------------------------------------------------------------------------
string MediaState::ReadMediaFileInfo(string sFilePath)
{
#ifdef WIN32
	struct __stat64 buf;
#else
	struct stat64 buf;
#endif

	if(FileUtils::FileExists(sFilePath + ".id3"))
		sFilePath += ".id3";

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
		"GREATEST("
		"	IF(File.psc_mod IS NULL,CAST('0000-00-00 00:00:00' AS DATE),File.psc_mod), "
		"	IF(MAX(Bookmark.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Bookmark.psc_mod)), "
		"	IF(MAX(File_Attribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(File_Attribute.psc_mod)), "
		"	IF(MAX(Attribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Attribute.psc_mod)), "
		"	IF(MAX(LongAttribute.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(LongAttribute.psc_mod)), "
		"	IF(MAX(Picture_File.psc_mod) IS NULL,CAST('0000-00-00 00:00:00' AS DATE),MAX(Picture_File.psc_mod)) "
		") AS CurrentDbAttrDate, "
		"(COUNT(Bookmark.PK_Bookmark) + COUNT(File_Attribute.FK_Attribute) + COUNT(Attribute.PK_Attribute) + COUNT(LongAttribute.PK_LongAttribute) + COUNT(Picture_File.FK_Picture)) AS CurrentDbAttrCount, "
		"AttrDate AS OldDbAttrDate, AttrCount AS OldDbAttrCount, ModificationDate AS OldFileDate, "
		"(COUNT(File_Attribute.FK_Attribute) + COUNT(LongAttribute.PK_LongAttribute)) AS HasAttributes "
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
		sfOldFileDate,
		sfHasAttributes
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
				bool bHasAttributes = NULL != row[sfHasAttributes] ? atoi(row[sfHasAttributes]) > 0 : false;

				return MediaItemState(nFileID, sPath, sFilename,
					sCurrentDbAttrDate, sCurrentDbAttrCount, sOldDbAttrDate, 
					sOldDbAttrCount, sOldFileDate, bHasAttributes);
			}
		}
	}

	return MediaItemState();
}
//-----------------------------------------------------------------------------------------------------
bool MediaState::AlreadyInDatabase(string sDirectory, string sFile)
{
	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	return it != m_mapMediaState.end();
}
//-----------------------------------------------------------------------------------------------------
int MediaState::FileId(string sDirectory, string sFile)
{
	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	if(it != m_mapMediaState.end())
		return it->second.m_nFileID;

	return 0;
}
//-----------------------------------------------------------------------------------------------------