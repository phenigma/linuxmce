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
#include "PlutoMediaFile.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_Installation.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_LongAttribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_main/Define_MediaType.h"

#include "Media_Plugin/MediaAttributes_LowLevel.h"
#include "UpdateMedia/PlutoMediaAttributes.h"

#include "id3info/id3info.h"
#include "MediaIdentifier.h"
#include "FileUtils/file_utils.h"

#ifdef UPDATE_MEDIA
#include "MediaState.h"
#endif

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

using namespace UpdateMediaFileUtils;
using namespace std;
using namespace DCE;

#define MAX_PICTURES 20

#include "pluto_media/Database_pluto_media.h"
//-----------------------------------------------------------------------------------------------------
#ifdef UPDATE_MEDIA
namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};
#endif
//-----------------------------------------------------------------------------------------------------
char *MediaSyncModeStr[] =
{
	"modeNone",
	"modeDbToFile",
	"modeFileToDb",
	"modeBoth"
};
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
MediaSyncMode PlutoMediaFile::m_DefaultMediaSyncMode = modeNone;
bool PlutoMediaFile::m_bNewFilesAdded = false;
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation, 
	string sDirectory, string sFile) : m_MediaSyncMode(modeNone), m_pPlutoMediaAttributes(NULL)
{
	//initializations
    m_pDatabase_pluto_media = pDatabase_pluto_media;
    m_sDirectory = sDirectory;
    m_sFile = sFile;
	m_nOurInstallationID = PK_Installation;
	m_nPK_MediaType = 0;
	m_bNewFileToDb = false;

	string sFilePath = sDirectory + "/" + sFile;
	m_bIsDir = IsDirectory(sFilePath);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PlutoMediaFile STARTED: dir %s file %s", 
		m_sDirectory.c_str(), m_sFile.c_str());

	if(m_bIsDir)
	{
		if(NULL == m_pPlutoMediaAttributes)
			m_pPlutoMediaAttributes = new PlutoMediaAttributes();
		return;
	}

	//get the path to id3 file
	string sAttributeFile = FileWithAttributes(false);
	string sAttributeFullFilePath = m_sDirectory + "/" + sAttributeFile;
	LoadPlutoAttributes(sAttributeFullFilePath);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing path %s, file %s. "
		"Found %d attributes, %d long attributes in id3 file", 
		m_sDirectory.c_str(), m_sFile.c_str(), m_pPlutoMediaAttributes->m_mapAttributes.size(),
		m_pPlutoMediaAttributes->m_mapLongAttributes.size());
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::~PlutoMediaFile()
{
	if(m_MediaSyncMode == modeDbToFile || m_MediaSyncMode == modeBoth)
	{
		//Save everything in id3 file
		SavePlutoAttributes(m_sDirectory + "/" + FileWithAttributes());
	}

	if(m_MediaSyncMode == modeFileToDb || m_MediaSyncMode == modeBoth)
	{
		//Save everything in db
		SaveEveryThingToDb();
	}

#ifdef UPDATE_MEDIA
	if(NULL != m_pPlutoMediaAttributes)
	{
		if(!m_pPlutoMediaAttributes->m_nFileID)
			m_pPlutoMediaAttributes->m_nFileID = GetFileIDFromDB();

		MediaState::Instance().FileSynchronized(m_pDatabase_pluto_media, m_sDirectory, m_sFile, 
			m_pPlutoMediaAttributes->m_nFileID);
	}
#endif

	delete m_pPlutoMediaAttributes;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# PlutoMediaFile ENDED: dir %s file %s, sync mode %s", 
		m_sDirectory.c_str(), m_sFile.c_str(), MediaSyncModeStr[m_MediaSyncMode]);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool PlutoMediaFile::IsSupported(string sFileName)
{
	const string csSupportedExtensions("mp3:ogg:aac:flac");
	string sExtension = StringUtils::ToLower(FileUtils::FindExtension(sFileName));

	if(sExtension.empty())
		return false;

    return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool PlutoMediaFile::IsDirectory(string sFilePath)
{
	bool bIsDir = false;

#ifdef WIN32
	struct __stat64 buf;
	if(!_stat64(sFilePath.c_str(), &buf))
		bIsDir = (0 != (buf.st_mode & _S_IFDIR)); 
#else
	struct stat64 buf;
	if(!stat64(sFilePath.c_str(), &buf))
		bIsDir = S_ISDIR(buf.st_mode);
#endif

	return bIsDir;
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaFile::AdjustLongAttributeForDisplay(string sText)
{	
	const size_t cnMaxSize = 30;
	return sText.length() > cnMaxSize ? (sText.substr(cnMaxSize) + "...").c_str() : sText.c_str();
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::HandleFileNotInDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;

	int PK_File=0;
	// See if the same INode is already in the database
	int INode = FileUtils::GetInode(m_sDirectory + "/" + m_sFile);
	if( INode>0 )
	{
		vector<Row_File *> vectRow_File;
		m_pDatabase_pluto_media->File_get()->GetRows("INode=" + StringUtils::itos(INode),&vectRow_File);
		if( vectRow_File.size() )  // Should only be 1
		{
			Row_File *pRow_File = vectRow_File[0];

			string sMidMD5File = FileUtils::GetMidFileChecksum(m_sDirectory + "/" + m_sFile);

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Reusing record PK_File %d for %s/%s ? Md5 db %s, md5 file %s", pRow_File->PK_File_get(), m_sDirectory.c_str(), m_sFile.c_str(), pRow_File->MD5_get().c_str(), sMidMD5File.c_str());

			//reuse the file only if it's the same file (midmd5 is the same)
			if(pRow_File->MD5_get() == sMidMD5File || pRow_File->MD5_isNull())
			{
				PK_File=pRow_File->PK_File_get();
				pRow_File->Ignore_set(0);  // This could be a re-used INode
				pRow_File->IsNew_set(1); // This could be a re-used INode
				pRow_File->Path_set(m_sDirectory);
				pRow_File->Filename_set(m_sFile);
				pRow_File->Table_File_get()->Commit();

#ifdef UPDATE_MEDIA
				if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
					pRow_File->Source_set("U");
				else
					pRow_File->Source_set("F");
#endif

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::HandleFileNotInDatabase %s/%s N db-attr: %d Inode: %d size %d mt %d/%d, md5 %s", 
					m_sDirectory.c_str(), m_sFile.c_str(), pRow_File->PK_File_get(), INode, (int) vectRow_File.size(), PK_MediaType, pRow_File->EK_MediaType_get(),
					pRow_File->MD5_get().c_str());
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Won't reuse file record %d from db because md5 is different", pRow_File->PK_File_get());
			}
		}
	}

    // Nope.  It's either a new file, or it was moved here from some other directory.  If so,
    // then the the attribute should be set.
	if( !PK_File )
		PK_File = GetFileAttribute();
	
	//make sure it's not already in the database
	if(!PK_File)
		PK_File = GetFileIDFromDB();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s/%s not IN db-attr: %d INode: %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File, INode);

    // Is it a media file?
    if(!PK_MediaType)
	{
        PK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
		m_nPK_MediaType = PK_MediaType;
	}

	if(PK_MediaType == 0)
	{
		if(m_bIsDir)
		{
			string sFullPath = FileUtils::IncludeTrailingSlash(m_sDirectory) + m_sFile;
			string sBasePath = "/home/public/data/";
			if(sFullPath.find(sBasePath + "videos") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredVideo_CONST;
			else if(sFullPath.find(sBasePath + "audio") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredAudio_CONST;
			else if(sFullPath.find(sBasePath + "pictures") == 0)
				PK_MediaType = MEDIATYPE_pluto_Pictures_CONST;
			else if(sFullPath.find(sBasePath + "documents") == 0)
				PK_MediaType = MEDIATYPE_misc_DocViewer_CONST;
			else
				PK_MediaType = MEDIATYPE_misc_DocViewer_CONST;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media Type is: %d, is folder %d", PK_MediaType, m_bIsDir);

	if(!PK_File)
    {
		if(PK_MediaType || m_bIsDir)
            return AddFileToDatabase(PK_MediaType);
        else
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "not a media file");
            return 0;
        }
    }
    else
    {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reusing file, updating media type to %d", PK_MediaType);

        Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
        if( !pRow_File )
        {
            SetFileAttribute(0);  // The file doesn't exist.  Shouldn't really happen
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Huh??  not in db now");
            return 0;
        }
        // it was in the database, but it's been moved to a different directory or renamed
        pRow_File->Path_set(m_sDirectory);
        pRow_File->Filename_set(m_sFile);
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->IsDirectory_set(m_bIsDir);

#ifdef UPDATE_MEDIA
		if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
			pRow_File->Source_set("U");
		else
			pRow_File->Source_set("F");
#endif

		int nEK_Users_Private = GetOwnerForPath(m_sDirectory);
		if(nEK_Users_Private != 0)
			pRow_File->EK_Users_Private_set(nEK_Users_Private);
		else
			pRow_File->EK_Users_Private_setNull(true);

        pRow_File->Table_File_get()->Commit();

		m_pPlutoMediaAttributes->m_nFileID = PK_File;
		AssignPlutoDevice();
    }

	return PK_File;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveEveryThingToDb()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: ready to sync db with attributes found in id3 file");

	// Is it a media file?
	if(!m_nPK_MediaType)
		m_nPK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
	
	//not a media file
	if(!m_nPK_MediaType)
		return;

	SaveStartPosition();
	SaveShortAttributesInDb(false);
	SaveLongAttributesInDb(false);
	SaveCoverarts();
	SaveBookmarkPictures();
	SaveMiscInfo();
	AssignPlutoDevice();

	UpdateMd5Field();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveStartPosition()
{
	//save/update start position in the database, if needed
	if(m_pPlutoMediaAttributes->m_sStartPosition != "")
	{
		vector<Row_Bookmark *> vectBookmarks;
		m_pDatabase_pluto_media->Bookmark_get()->GetRows("Description = 'START' AND IsAutoResume = 1 "
			"AND FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID), &vectBookmarks);

		if(vectBookmarks.size() > 0)
		{
			Row_Bookmark *pRow_Bookmark = vectBookmarks[0];
			if(pRow_Bookmark->Position_get() != m_pPlutoMediaAttributes->m_sStartPosition)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: got different start position. Updating to %s...",
					m_pPlutoMediaAttributes->m_sStartPosition.c_str());

				pRow_Bookmark->Position_set(m_pPlutoMediaAttributes->m_sStartPosition);
				pRow_Bookmark->Table_Bookmark_get()->Commit();
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: already got same start position. Ignoring...");
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: No start position in the database for this file. "
				"Adding %s start position.", m_pPlutoMediaAttributes->m_sStartPosition.c_str());

			Row_Bookmark *pRow_Bookmark_New = m_pDatabase_pluto_media->Bookmark_get()->AddRow();
			pRow_Bookmark_New->Position_set(m_pPlutoMediaAttributes->m_sStartPosition);
			pRow_Bookmark_New->Description_set("START");
			pRow_Bookmark_New->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);
			pRow_Bookmark_New->EK_MediaType_set(m_nPK_MediaType);
			pRow_Bookmark_New->IsAutoResume_set(1);
			m_pDatabase_pluto_media->Bookmark_get()->Commit();
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveShortAttributesInDb(bool bAddAllToDb)
{
	int PK_File = m_pPlutoMediaAttributes->m_nFileID;

	MapPlutoMediaAttributes mapPlutoMediaAttributes;

	if(!bAddAllToDb)
	{
		string SQL = 
			"SELECT PK_Attribute,FK_AttributeType, Name, Track, Section "
			"FROM Attribute JOIN File_Attribute ON PK_Attribute = FK_Attribute "
			"WHERE FK_File = " + StringUtils::ltos(PK_File);

		PlutoSqlResult result;
		DB_ROW row;
		if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
		{
			while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
			{
				int nPK_Attribute = atoi(row[0]);
				int nFK_AttributeType = atoi(row[1]);
				string sName = row[2];
				int nTrack = NULL != row[3] ? atoi(row[3]) : 0;
				int nSection = NULL != row[4] ? atoi(row[4]) : 0;

				mapPlutoMediaAttributes.insert(
					std::make_pair(
					nFK_AttributeType, 
					new PlutoMediaAttribute(nPK_Attribute, nFK_AttributeType, sName, nTrack, nSection)
					)
					);
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: attributes is db %d, total %d",
		mapPlutoMediaAttributes.size(), m_pPlutoMediaAttributes->m_mapAttributes.size());

	// For albums we don't want to add a new one for each media file.  All media from the same album
	// should be grouped into a single album attribute.  However since different performers could have
	// albums of the same name we need to maek a pass first to find the performer and skip the album, and then another pass
	// to add the album
	int PK_Attribute_Performer=0;
	for(int iPass=0;iPass<2;++iPass)
	{
		//Save any new attributes in the database
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

			bool bAttributeAlreadyAdded = false;

			if(!bAddAllToDb)
			{
				for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(),
					enddb = mapPlutoMediaAttributes.end(); itdb != enddb; ++itdb)
				{
					PlutoMediaAttribute *pDBPlutoMediaAttribute = itdb->second;

					if(
						pPlutoMediaAttribute->m_nType == pDBPlutoMediaAttribute->m_nType		&&
						StringUtils::ToLower(pPlutoMediaAttribute->m_sName) == StringUtils::ToLower(pDBPlutoMediaAttribute->m_sName)		&&
						pPlutoMediaAttribute->m_nTrack == pDBPlutoMediaAttribute->m_nTrack		&&
						pPlutoMediaAttribute->m_nSection == pDBPlutoMediaAttribute->m_nSection
						)
					{
						if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Performer_CONST )
							PK_Attribute_Performer = pDBPlutoMediaAttribute->m_nPK_Attribute;
						bAttributeAlreadyAdded = true;
						break;
					}
				}
			}

			if(!bAttributeAlreadyAdded)
			{
				if(pPlutoMediaAttribute->m_nType > 0 && !StringUtils::OnlyWhiteSpace(pPlutoMediaAttribute->m_sName))
				{
					if( (iPass==0 && pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Album_CONST) || (iPass==1 && pPlutoMediaAttribute->m_nType!=ATTRIBUTETYPE_Album_CONST) )
						continue; // See notes above

					MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
					Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(m_nPK_MediaType,
						pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName, PK_Attribute_Performer);

					if( pRow_Attribute==NULL )
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"PlutoMediaFile::SyncDbAttributes attribute type %d/%s is empty",
							pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName.c_str());
						continue;
					}

					pPlutoMediaAttribute->m_nPK_Attribute = pRow_Attribute->PK_Attribute_get();
					if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Performer_CONST )
						PK_Attribute_Performer = pPlutoMediaAttribute->m_nPK_Attribute;

					//already in the database?
					if(NULL == m_pDatabase_pluto_media->File_Attribute_get()->GetRow(
						PK_File, pRow_Attribute->PK_Attribute_get(),
						pPlutoMediaAttribute->m_nTrack, pPlutoMediaAttribute->m_nSection)
						)
					{
						Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
						pRow_File_Attribute->FK_File_set(PK_File);
						pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
						pRow_File_Attribute->Section_set(pPlutoMediaAttribute->m_nSection);
						pRow_File_Attribute->Track_set(pPlutoMediaAttribute->m_nTrack);
						pRow_File_Attribute->Table_File_Attribute_get()->Commit();

						LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: Adding attribute to database: "
							"for PK_File %d, AttrID %d, AttrType = %d with value %s, section %d, track %d", 
							PK_File, pRow_Attribute->PK_Attribute_get(), pPlutoMediaAttribute->m_nType,
							pPlutoMediaAttribute->m_sName.c_str(), pPlutoMediaAttribute->m_nSection, pPlutoMediaAttribute->m_nTrack); 
					}
				}
			}
		}
	}

	for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(); itdb != mapPlutoMediaAttributes.end(); ++itdb)
		delete itdb->second;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveLongAttributesInDb(bool bAddAllToDb)
{
	int PK_File = m_pPlutoMediaAttributes->m_nFileID;
	MapPlutoMediaAttributes mapPlutoMediaAttributes;

	if(!bAddAllToDb)
	{
		string SQL = 
			"SELECT FK_AttributeType, Text "
			"FROM LongAttribute "
			"WHERE FK_File = " + StringUtils::ltos(PK_File);

		PlutoSqlResult result;
		DB_ROW row;
		if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
		{
			while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
			{
				int nFK_AttributeType = atoi(row[0]);
				string sName = row[1];

				mapPlutoMediaAttributes.insert(
					std::make_pair(
						nFK_AttributeType, 
						new PlutoMediaAttribute(0, nFK_AttributeType, sName)
					)
				);
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: long attributes is db %d, total %d",
		mapPlutoMediaAttributes.size(), m_pPlutoMediaAttributes->m_mapLongAttributes.size());

	//Save any new long attributes in the database
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
		end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end; ++it)
	{
		PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

		bool bAttributeAlreadyAdded = false;

		if(!bAddAllToDb)
		{
			for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(),
				enddb = mapPlutoMediaAttributes.end(); itdb != enddb; ++itdb)
			{
				PlutoMediaAttribute *pDBPlutoMediaAttribute = itdb->second;

				if(
					pPlutoMediaAttribute->m_nType == pDBPlutoMediaAttribute->m_nType &&
					StringUtils::ToLower(pPlutoMediaAttribute->m_sName) == StringUtils::ToLower(pDBPlutoMediaAttribute->m_sName)
					)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}
		}

		if(!bAttributeAlreadyAdded)
		{
			if(pPlutoMediaAttribute->m_nType > 0 && !StringUtils::OnlyWhiteSpace(pPlutoMediaAttribute->m_sName))
			{
				Row_LongAttribute *pRow_LongAttribute = m_pDatabase_pluto_media->LongAttribute_get()->AddRow();
				pRow_LongAttribute->FK_AttributeType_set(pPlutoMediaAttribute->m_nType);
				pRow_LongAttribute->FK_File_set(PK_File);
				pRow_LongAttribute->Text_set(pPlutoMediaAttribute->m_sName);
				pRow_LongAttribute->Table_LongAttribute_get()->Commit();

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: Adding long attribute to database: "
					"for PK_File %d, long AttrID %d, AttrType = %d with value %s", 
					PK_File, pRow_LongAttribute->PK_LongAttribute_get(), pPlutoMediaAttribute->m_nType,
					AdjustLongAttributeForDisplay(pPlutoMediaAttribute->m_sName).c_str()
				); 
			}
		}
	}

	for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(); itdb != mapPlutoMediaAttributes.end(); ++itdb)
		delete itdb->second;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveBookmarkPictures()
{
	//got bookmarks in id3 file, but none in the database?
	if(!m_pPlutoMediaAttributes->m_listBookmarks.empty() && !NumberOfBookmarksFromDB())
	{
		int nCounter = 0;
		for(ListBookmarks::iterator it = m_pPlutoMediaAttributes->m_listBookmarks.begin(), 
			end = m_pPlutoMediaAttributes->m_listBookmarks.end(); it != end; ++it)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SaveBookmarkPictures: saving bookmark...");

			PlutoMediaBookmark *pBookmark = *it;

			int PK_Picture = 0;
			if(pBookmark->m_dataPicture.m_dwSize != 0 && pBookmark->m_dataPictureThumb.m_dwSize != 0)
			{
				Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
				pRow_Picture->Extension_set("jpg");
				pRow_Picture->Table_Picture_get()->Commit();

				SavePicture(make_pair(pBookmark->m_dataPicture.m_dwSize, pBookmark->m_dataPicture.m_pBlock), pRow_Picture->PK_Picture_get());
				SavePicture(make_pair(pBookmark->m_dataPictureThumb.m_dwSize, pBookmark->m_dataPictureThumb.m_pBlock), pRow_Picture->PK_Picture_get(), true);

				PK_Picture = pRow_Picture->PK_Picture_get();
			}

			Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->AddRow();
			pRow_Bookmark->Description_set(pBookmark->m_sDescription);
			pRow_Bookmark->Position_set(pBookmark->m_sPosition);
			pRow_Bookmark->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);

			if(PK_Picture != 0)
				pRow_Bookmark->FK_Picture_set(PK_Picture);

			pRow_Bookmark->Table_Bookmark_get()->Commit();

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SaveBookmarkPictures: too many bookmarks...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveMiscInfo()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SaveMiscInfo: FK_FileFormat %d, FK_MediaSubType %d", 
		m_pPlutoMediaAttributes->m_nFileFormat, m_pPlutoMediaAttributes->m_nMediaSubType);

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
	if(NULL != pRow_File)
	{
		if(m_pPlutoMediaAttributes->m_nFileFormat != 0)
			pRow_File->FK_FileFormat_set(m_pPlutoMediaAttributes->m_nFileFormat);
		else
			pRow_File->FK_FileFormat_setNull(true);

		if(m_pPlutoMediaAttributes->m_nMediaSubType != 0)
			pRow_File->FK_MediaSubType_set(m_pPlutoMediaAttributes->m_nMediaSubType);
		else
			pRow_File->FK_MediaSubType_setNull(true);

		pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveCoverarts()
{
	//got coverarts in id3 file, but none in the database?
	if(!m_pPlutoMediaAttributes->m_mapCoverarts.empty() && !NumberOfCoverartsFromDB())
	{
		MapPictures::iterator it = m_pPlutoMediaAttributes->m_mapCoverarts.begin();
		MapPictures::iterator end = m_pPlutoMediaAttributes->m_mapCoverarts.end();
		MapPictures::iterator it_thumb = m_pPlutoMediaAttributes->m_mapCoverartsThumbs.begin();
		MapPictures::iterator end_thumb = m_pPlutoMediaAttributes->m_mapCoverartsThumbs.end();

		int nCounter = 0;
		for( ; it != end && it_thumb != end_thumb; ++it, ++it_thumb)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SaveCoverarts : saving coverart...");

			Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
			pRow_Picture->Extension_set("jpg");
			pRow_Picture->Table_Picture_get()->Commit();

			Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
			pRow_Picture_File->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);
			pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
			pRow_Picture_File->Table_Picture_File_get()->Commit();

            SavePicture(make_pair(it->first, it->second), pRow_Picture->PK_Picture_get());
			SavePicture(make_pair(it_thumb->first, it_thumb->second), pRow_Picture->PK_Picture_get(), true);

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SaveCoverarts : too many coverarts...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::AddFileToDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;
	Row_File *pRow_File = NULL;

	//We'll have to be sure first that we won't create duplicates; check first is there are any
	//rows we can reuse, marked as 'Missing' (deleted before because the file was removed/moved)
	//However, this should never happen. Added this code temporary to trace a possible logic flaw.
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows(
		"Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(m_sDirectory)) + 
		"' AND Filename = '" + StringUtils::SQLEscape(m_sFile) + "' AND Missing = 1", &vectRow_File);

	int nEK_Users_Private = GetOwnerForPath(m_sDirectory);

	//Any luck to reuse a missing file record?
	if(vectRow_File.size())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> there is already "
			"a record in the database marked as missing for %s - %s file", m_sDirectory.c_str(), m_sFile.c_str());

		//Get the first record and reuse it
		pRow_File = vectRow_File[0];
		pRow_File->Missing_set(0);
		pRow_File->EK_MediaType_set(PK_MediaType);

		if(nEK_Users_Private != 0)
			pRow_File->EK_Users_Private_set(nEK_Users_Private);
		else
			pRow_File->EK_Users_Private_setNull(true);

		pRow_File->Table_File_get()->Commit();

		//This will get a vector with desired File_Attribute's rows
		vector<Row_File_Attribute *> vectRow_File_Attribute;
		m_pDatabase_pluto_media->File_Attribute_get()->GetRows("FK_File = " + StringUtils::ltos(pRow_File->PK_File_get()),
			&vectRow_File_Attribute);
		
        //Iterate through the vector and mark all the rows as deleted		
		for(vector<Row_File_Attribute *>::iterator it = vectRow_File_Attribute.begin();
			it != vectRow_File_Attribute.end(); ++it
		)
		{
			Row_File_Attribute *pRow_File_Attribute = *it;
			pRow_File_Attribute->Delete();
		}

		//If there are indeed rows to be deleted, commit the changes
		if(vectRow_File_Attribute.size())
			m_pDatabase_pluto_media->File_Attribute_get()->Commit();

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> reusing PK_File %d. "
			"Deleted children records.", pRow_File->PK_File_get());
	}
	else
	{
		//No record to reuse; we'll create a new one
		pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
		pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(m_sDirectory));
		pRow_File->Filename_set(FileUtils::ExcludeTrailingSlash(m_sFile));
		pRow_File->IsDirectory_set(m_bIsDir);
		pRow_File->EK_MediaType_set(PK_MediaType);

#ifdef UPDATE_MEDIA
		if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
			pRow_File->Source_set("U");
		else
			pRow_File->Source_set("F");
#endif

		if(nEK_Users_Private != 0)
			pRow_File->EK_Users_Private_set(nEK_Users_Private);
		else
			pRow_File->EK_Users_Private_setNull(true);

		pRow_File->INode_set( FileUtils::GetInode( pRow_File->Path_get() + "/" + pRow_File->Filename_get() ) );
		pRow_File->Table_File_get()->Commit();

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "PlutoMediaFile::AddFileToDatabase new %s PK_File %d Inode %d",
			(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );
	}

	//These are our installation and our file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = pRow_File->PK_File_get();

	SaveShortAttributesInDb(true);
	SaveLongAttributesInDb(true);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase picture url %s",
		m_pPlutoMediaAttributes->m_sPictureUrl.c_str());

	// Got a picture url? Let's download it if we don't have already coverarts saved
	if(m_pPlutoMediaAttributes->m_mapCoverarts.empty())
	{
		if(m_pPlutoMediaAttributes->m_sPictureUrl != "")
		{
			//It's a "new" file, but we know the picture url
			//we'll download the picture and record in Picture table
			MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
			Row_Picture *pRow_Picture = mediaAttributes_LowLevel.AddPicture(NULL, 0, 
				FileUtils::FindExtension(m_pPlutoMediaAttributes->m_sPictureUrl), 
				m_pPlutoMediaAttributes->m_sPictureUrl
			);

			if(pRow_Picture)
			{
				m_pPlutoMediaAttributes->m_nPictureID = pRow_Picture->PK_Picture_get();

				Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
				pRow_Picture_File->FK_File_set(pRow_File->PK_File_get());
				pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
				pRow_Picture_File->Table_Picture_File_get()->Commit();

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase Added picture to file: PK_File %d, PK_Picture %d",
					pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_nPictureID);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase Failed to add picture to file: PK_File %d, picture url: %s",
					pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %s/%s to db with PK_File = %d", m_sDirectory.c_str(), m_sFile.c_str(),
		pRow_File->PK_File_get());

	m_bNewFileToDb = true;
	m_bNewFilesAdded = true;

	if( PK_MediaType==MEDIATYPE_pluto_Pictures_CONST && !m_bIsDir)
	{
		string Output = m_sDirectory + "/" + m_sFile;
		StringUtils::Replace(&Output,"\"","\\\"");
		string sCmd = "convert \"" + Output + "\" -scale 256x256 -antialias \"jpeg:" + Output + ".tnj\"";
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Creating thumbnail %s",sCmd.c_str());
		system(sCmd.c_str());
	}

	if(m_pPlutoMediaAttributes->m_mapCoverarts.empty())
	{
		for(list<pair<char *, size_t> >::iterator itp = m_listPicturesForID3Tags.begin();
			itp != m_listPicturesForID3Tags.end(); ++itp)
		{
			char *pPictureData = itp->first;
			size_t nPictureSize = itp->second;

			if(FileUtils::WriteBufferIntoFile("UpdateMedia_lastimage.jpg", pPictureData, nPictureSize))
			{
				string Cmd = "convert -scale 256x256 -antialias UpdateMedia_lastimage.jpg UpdateMedia_lastimage_tn.jpg";
				int result;
				if((result = system(Cmd.c_str())) != 0)
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str(), result);
				else
				{
					size_t nPictureThumbSize = 0;
					char *PictureThumbData = FileUtils::ReadFileIntoBuffer("UpdateMedia_lastimage_tn.jpg", nPictureThumbSize);

					if(NULL != PictureThumbData && nPictureThumbSize > 0)
					{
						m_pPlutoMediaAttributes->m_mapCoverarts.insert(make_pair(static_cast<unsigned long>(nPictureSize), pPictureData));
						m_pPlutoMediaAttributes->m_mapCoverartsThumbs.insert(make_pair(static_cast<unsigned long>(nPictureThumbSize), PictureThumbData));
					}
					else
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot read image from disk: UpdateMedia_lastimage_tn.jpg");
					}				
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot save image to disk: UpdateMedia_lastimage.jpg");
			}
		}
	}

    return pRow_File->PK_File_get();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::AssignPlutoDevice()
{
#ifdef UPDATE_MEDIA
	map<int, int> mapMountedDevices;
	list<string> listFiles;
	FileUtils::FindDirectories(listFiles, "/mnt/device", false, true);
	MapMountedDevicesToPlutoDevices(listFiles, mapMountedDevices);

	int nEK_Device = PlutoDeviceForFile(m_sDirectory + "/" + m_sFile, mapMountedDevices);

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
	if(NULL != pRow_File)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s/%s on pluto device %d", m_sDirectory.c_str(), m_sFile.c_str(), nEK_Device);

		if(nEK_Device != 0)
			pRow_File->EK_Device_set(nEK_Device);
		else
			pRow_File->EK_Device_setNull(true);

		pRow_File->Table_File_get()->Commit();
	}

#endif //UPDATE_MEDIA
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::UpdateMd5Field()
{
	string sMidMd5 = FileUtils::GetMidFileChecksum(m_sDirectory + "/" + m_sFile);

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
	if(NULL != pRow_File)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s/%s, PK_File %d - midmd5 is %s", 
			m_sDirectory.c_str(), m_sFile.c_str(), m_pPlutoMediaAttributes->m_nFileID, sMidMd5.c_str());
		pRow_File->MD5_set(sMidMd5);
		pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetFileAttribute(int PK_File)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetFileAttribute %s/%s %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File);

	//make sure it's our installation and file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = PK_File;

	LoadEverythingFromDb();
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileAttribute()
{
	if(m_pPlutoMediaAttributes->m_nInstallationID == m_nOurInstallationID && m_pPlutoMediaAttributes->m_nFileID != 0)
	{
		//same installation, same file; however, it's a good record?
		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
		if(NULL != pRow_File && pRow_File->Filename_get() == m_sFile)
			return pRow_File->PK_File_get();
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "GetFileAttribute %s/%s installation %d/%d id %d doesn't match database %s", 
				m_sDirectory.c_str(), m_sFile.c_str(),
				m_pPlutoMediaAttributes->m_nInstallationID, m_nOurInstallationID, m_pPlutoMediaAttributes->m_nFileID,
				pRow_File ? pRow_File->Filename_get().c_str() : "*NULL*");
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "GetFileAttribute %s/%s not found installation %d/%d id %d", 
		m_sDirectory.c_str(), m_sFile.c_str(),
		m_pPlutoMediaAttributes->m_nInstallationID, m_nOurInstallationID, m_pPlutoMediaAttributes->m_nFileID);
    return 0;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetPicAttribute(int PK_Picture, string sPictureUrl)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetPicAttribute %s/%s PK_Picture %d", m_sDirectory.c_str(), m_sFile.c_str(),
		PK_Picture);

	//set the right picture id and picture url
	m_pPlutoMediaAttributes->m_nPictureID = PK_Picture;
	m_pPlutoMediaAttributes->m_sPictureUrl = sPictureUrl;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileIDFromDB()
{
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(m_sDirectory) + 
		"' AND Filename='" + StringUtils::SQLEscape(m_sFile) + "' AND Missing = 0", &vectRow_File);
	if( vectRow_File.size() )
		return vectRow_File[0]->PK_File_get();
	else
		return 0;  // Can do nothing.  This isn't in the database
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetPicAttribute(int PK_File)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: file %d", PK_File);

	//got the file in the database?
    if(!PK_File)
	{
		PK_File = GetFileIDFromDB();
		m_pPlutoMediaAttributes->m_nFileID = PK_File;
	}

	//got any picture associated with the file?
    vector<Row_Picture_File *> vectPicture_File;
    m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found %d pics for file", vectPicture_File.size());
    if( vectPicture_File.size() )
    {
		long PK_Picture = vectPicture_File[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got picture %d associated to file %d", PK_Picture, PK_File);

        SetPicAttribute(PK_Picture, sPictureUrl);
        return PK_Picture;  // The first pic for this directory
    }

    //Does one of the attributes have a picture?
    vector<Row_Picture_Attribute *> vectPicture_Attribute;
    m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
        "JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "
        " JOIN Attribute ON Picture_Attribute.FK_Attribute=Attribute.PK_Attribute "
        " JOIN AttributeType ON Attribute.FK_AttributeType=AttributeType.PK_AttributeType "
        " WHERE FK_File=" + 
        StringUtils::itos(PK_File) +
        " ORDER BY `PicPriority`",&vectPicture_Attribute);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found %d pics for attribute", (int) vectPicture_Attribute.size());
    if( vectPicture_Attribute.size())
    {
		long PK_Picture = vectPicture_Attribute[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got picture %d associated to attribute %d", PK_Picture, 
			vectPicture_Attribute[0]->FK_Attribute_get());

        SetPicAttribute(PK_Picture, sPictureUrl);
        return PK_Picture;  // The first pic for this directory
    }

	if(m_pPlutoMediaAttributes->m_mapCoverarts.empty())
	{
		if(m_pPlutoMediaAttributes->m_sPictureUrl != "")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got no picture in the db, but got picture URL %s",
				m_pPlutoMediaAttributes->m_sPictureUrl.c_str());

			//It's a "new" file, but we know the picture url
			//we'll download the picture and record in Picture table
			MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
			Row_Picture *pRow_Picture = mediaAttributes_LowLevel.AddPicture(NULL, 0, 
				FileUtils::FindExtension(m_pPlutoMediaAttributes->m_sPictureUrl), 
				m_pPlutoMediaAttributes->m_sPictureUrl
			);

			if(NULL != pRow_Picture)
			{
				m_pPlutoMediaAttributes->m_nPictureID = pRow_Picture->PK_Picture_get();

				Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
				pRow_Picture_File->FK_File_set(PK_File);
				pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
				pRow_Picture_File->Table_Picture_File_get()->Commit();

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::GetPicAttribute Added picture to file: PK_File %d, PK_Picture %d",
					PK_File, m_pPlutoMediaAttributes->m_nPictureID);

				return m_pPlutoMediaAttributes->m_nPictureID;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to add picture to file: PK_File %d, picture url: %s",
					PK_File, m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got no picture for file %d", PK_File);

    return 0;
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaFile::FileWithAttributes(bool bCreateId3File)
{
	//no id3 files for directories
	if(m_bIsDir)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# No id3 file for folders");
		return "";
	}

	//no id3 file if the media file doesn't exist anymore.
	if(!FileUtils::FileExists(m_sDirectory + "/" + m_sFile))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# No id3 file. The media file doesn't exist anymore.");
		return "";
	}

	string sFileWithAttributes = m_sFile;
	if(!IsSupported(m_sFile))
	{
		sFileWithAttributes = m_sFile + ".id3";
		if(FileUtils::FileExists(m_sDirectory + "/" + sFileWithAttributes))
			return sFileWithAttributes;

		if(!bCreateId3File)
			return "";

		if(
			m_pPlutoMediaAttributes->m_mapAttributes.size() == 0 && 
			m_pPlutoMediaAttributes->m_mapLongAttributes.size() == 0 && 
			m_pPlutoMediaAttributes->m_mapCoverarts.size() == 0 &&
			m_pPlutoMediaAttributes->m_listBookmarks.size() == 0 &&
			m_pPlutoMediaAttributes->m_sStartPosition == ""
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# Won't create id3 file (the media file doesn't have attributes)");
			return "";
		}

		if(!FileUtils::DirExists(m_sDirectory + "/" + sFileWithAttributes))
			FileUtils::WriteTextFile(m_sDirectory + "/" + sFileWithAttributes, ""); //touch it
	}

	return sFileWithAttributes;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SavePlutoAttributes(string sFullFileName)
{
	if(m_MediaSyncMode == modeFileToDb)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# NOT saving attributes in id3 files. Read only mode!");
		return;
	}

	GetPicAttribute(m_pPlutoMediaAttributes->m_nFileID);

	//Temporary map with attributes for common tags
	map<int, string> mapAttributes;
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(), 
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		mapAttributes[it->first] = it->second->m_sName;
	}

	list<pair<char *, size_t> > listPictures;
	for(MapPictures::iterator itc = m_pPlutoMediaAttributes->m_mapCoverarts.begin();
		itc != m_pPlutoMediaAttributes->m_mapCoverarts.end(); ++itc)
	{
		listPictures.push_back(make_pair(itc->second, itc->first));
	}

	//Save common id3 tags
	SetId3Info(sFullFileName, mapAttributes, listPictures);
	listPictures.clear();

	//Save user defined text
	char *pDataCurrentPosition = NULL;
	char *pDataStartPosition = NULL;
	unsigned long ulSize = 0;

	m_pPlutoMediaAttributes->SerializeWrite();
	ulSize = m_pPlutoMediaAttributes->CurrentSize();
	pDataCurrentPosition = pDataStartPosition = m_pPlutoMediaAttributes->m_pcDataBlock;

	size_t Size = ulSize;
	SetUserDefinedInformation(sFullFileName, pDataStartPosition, Size);
	m_pPlutoMediaAttributes->FreeSerializeMemory();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SavePlutoAttributes: saving %d attributes in the id3 file %s",
		m_pPlutoMediaAttributes->m_mapAttributes.size(), sFullFileName.c_str());
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadPlutoAttributes(string sFullFileName)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes (from id3 file): %s", sFullFileName.c_str());

	m_pPlutoMediaAttributes = new PlutoMediaAttributes();

	//deserialize data from user defined tag
	char *pData = NULL;
	size_t Size = 0;
	GetUserDefinedInformation(sFullFileName, pData, Size);

	if(NULL != pData)
	{
		m_pPlutoMediaAttributes->SerializeRead((unsigned long)Size, pData);
		delete []pData;
		pData = NULL;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto attributes loaded (from id3 file - general object tag) %d", 
		m_pPlutoMediaAttributes->m_mapAttributes.size());

	//get common id3 attributes
	map<int, string> mapAttributes;
	m_listPicturesForID3Tags.clear();
	GetId3Info(sFullFileName, mapAttributes, m_listPicturesForID3Tags);	

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: id3 attributes loaded (from id3 file - common tags) %d", 
		mapAttributes.size());

	//merge attributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;

		MapPlutoMediaAttributes::iterator itm = m_pPlutoMediaAttributes->m_mapAttributes.find(nType);
		if(itm == m_pPlutoMediaAttributes->m_mapAttributes.end())
			m_pPlutoMediaAttributes->m_mapAttributes.insert(
				std::make_pair(
					nType, 
					new PlutoMediaAttribute(0,nType, sValue)
				)
			);
		else
			itm->second->m_sName = sValue;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto attributes merged (for id3 file) %d", 
		m_pPlutoMediaAttributes->m_mapAttributes.size());
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadEverythingFromDb()
{
	LoadStartPosition();
	LoadShortAttributes();
	LoadLongAttributes();
	LoadCoverarts();
	LoadBookmarkPictures();
	LoadMiscInfo();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadStartPosition()
{
	//load start position from the database
	vector<Row_Bookmark *> vectBookmarks;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows("Description = 'START' AND IsAutoResume = 1 "
		"AND FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID), &vectBookmarks);

	if(vectBookmarks.size() > 0)
	{
		Row_Bookmark *pRow_Bookmark = vectBookmarks[0];	
		m_pPlutoMediaAttributes->m_sStartPosition = pRow_Bookmark->Position_get();

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: read start position from db: %s",
			m_pPlutoMediaAttributes->m_sStartPosition.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadShortAttributes()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT PK_Attribute,FK_AttributeType, Name, Track, Section "
		"FROM Attribute JOIN File_Attribute ON PK_Attribute = FK_Attribute "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	MapPlutoMediaAttributes mapDBAttributes;

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			int nPK_Attribute = atoi(row[0]);
			int nFK_AttributeType = atoi(row[1]);
			string sName = row[2];
			int nTrack = NULL != row[3] ? atoi(row[3]) : 0;
			int nSection = NULL != row[4] ? atoi(row[4]) : 0;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Attr in db : type %d\t\tvalue %s", nFK_AttributeType, sName.c_str());

			bool bAttributeAlreadyAdded = false;

			if(m_MediaSyncMode == modeDbToFile) 
			{
				mapDBAttributes.insert(std::make_pair(nFK_AttributeType, 
					new PlutoMediaAttribute(nPK_Attribute,nFK_AttributeType, sName, nTrack, nSection)));
			}

			for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
				end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
				if(
					pPlutoMediaAttribute->m_nType == nFK_AttributeType	&&
					pPlutoMediaAttribute->m_sName == sName				&&
					pPlutoMediaAttribute->m_nTrack == nTrack			&&
					pPlutoMediaAttribute->m_nSection == nSection
					)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}

			if(!bAttributeAlreadyAdded)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: (from db) new attributes to add in id3 file -  type %d, values %s",
					nFK_AttributeType, sName.c_str());

				m_pPlutoMediaAttributes->m_mapAttributes.insert(
					std::make_pair(
						nFK_AttributeType, 
						new PlutoMediaAttribute(nPK_Attribute, nFK_AttributeType, sName, nTrack, nSection)
					)
				);
			}
		}
	}

	if(m_MediaSyncMode == modeDbToFile)
	{
		//we need to check here if any attribute was deleted from website in order to delete it from 
		//file's attribute too.
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end;)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute_File = it->second;
			bool bAttributeFoundInDB = false;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Attr in file : type %d\t\tvalue %s", 
				pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName.c_str());

			for(MapPlutoMediaAttributes::iterator it_DbAttr = mapDBAttributes.begin(), end_DbAttr = mapDBAttributes.end(); 
				it_DbAttr != end_DbAttr; ++it_DbAttr)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute_DB = it_DbAttr->second;

				if(*pPlutoMediaAttribute_File == *pPlutoMediaAttribute_DB)
				{
					bAttributeFoundInDB = true;
					break;
				}
			}

			if(!bAttributeFoundInDB)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Removing attribute from file %d, value %s, since it was remove from the site.",
					pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName.c_str());

				RemoveId3Tag(m_sDirectory + "/" + FileWithAttributes(), pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName);
				m_pPlutoMediaAttributes->m_mapAttributes.erase(it++);
			}
			else
				++it;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "After sync'ing file, attr in db %d, attr in file %d", 
			mapDBAttributes.size(), m_pPlutoMediaAttributes->m_mapAttributes.size());

		//cleanup
		for(MapPlutoMediaAttributes::iterator it = mapDBAttributes.begin(), end = mapDBAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
			delete pPlutoMediaAttribute;
		}
		mapDBAttributes.clear();
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto attributes merged with those from database %d", 
		m_pPlutoMediaAttributes->m_mapAttributes.size());
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadLongAttributes()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_AttributeType, Text "
		"FROM LongAttribute "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	MapPlutoMediaAttributes mapDbLongAttributes;

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			int nFK_AttributeType = atoi(row[0]);
			string sName = row[1];

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Long attribute in db : type %d\t\tvalue %s", nFK_AttributeType, AdjustLongAttributeForDisplay(sName).c_str());

			bool bAttributeAlreadyAdded = false;
			if(m_MediaSyncMode == modeDbToFile) 
				mapDbLongAttributes.insert(std::make_pair(nFK_AttributeType, new PlutoMediaAttribute(0, nFK_AttributeType, sName)));

			for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
				end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end; ++it)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
				if(pPlutoMediaAttribute->m_nType == nFK_AttributeType && pPlutoMediaAttribute->m_sName == sName)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}

			if(!bAttributeAlreadyAdded)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: (from db) new long attributes to add in id3 file -  type %d, values %s",
					nFK_AttributeType, AdjustLongAttributeForDisplay(sName).c_str());

				m_pPlutoMediaAttributes->m_mapLongAttributes.insert(
					std::make_pair(nFK_AttributeType, new PlutoMediaAttribute(0, nFK_AttributeType, sName))
				);
			}
		}
	}

	if(m_MediaSyncMode == modeDbToFile)
	{
		//we need to check here if any long attribute was deleted from website in order to delete it from 
		//file's attribute too.
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end;)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute_File = it->second;
			bool bAttributeFoundInDB = false;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Long attr in file : type %d\t\tvalue %s", 
				pPlutoMediaAttribute_File->m_nType, AdjustLongAttributeForDisplay(pPlutoMediaAttribute_File->m_sName).c_str());

			for(MapPlutoMediaAttributes::iterator it_DbAttr = mapDbLongAttributes.begin(), end_DbAttr = mapDbLongAttributes.end(); 
				it_DbAttr != end_DbAttr; ++it_DbAttr)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute_DB = it_DbAttr->second;

				if(*pPlutoMediaAttribute_File == *pPlutoMediaAttribute_DB)
				{
					bAttributeFoundInDB = true;
					break;
				}
			}

			if(!bAttributeFoundInDB)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Removing attribute from file %d, value %s, since it was remove from the site.",
					pPlutoMediaAttribute_File->m_nType, AdjustLongAttributeForDisplay(pPlutoMediaAttribute_File->m_sName).c_str());

				RemoveId3Tag(m_sDirectory + "/" + FileWithAttributes(), pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName);
				m_pPlutoMediaAttributes->m_mapLongAttributes.erase(it++);
			}
			else
				++it;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "After sync'ing file, long attr in db %d, long attr in file %d", 
			mapDbLongAttributes.size(), m_pPlutoMediaAttributes->m_mapLongAttributes.size());

		//cleanup
		for(MapPlutoMediaAttributes::iterator it = mapDbLongAttributes.begin(), end = mapDbLongAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
			delete pPlutoMediaAttribute;
		}
		mapDbLongAttributes.clear();
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto long attributes merged with those from database %d", 
		m_pPlutoMediaAttributes->m_mapLongAttributes.size());

}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadBookmarkPictures()
{
	//if the file was added before, but we already have bookmarks in db ignore those from id3
	if(!m_bNewFileToDb && NumberOfBookmarksFromDB())
		m_pPlutoMediaAttributes->ClearBookmarks();

	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_Picture, Description, Position FROM Bookmark "
		"WHERE FK_Picture IS NOT NULL AND "
		"FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		int nCounter = 0;
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1] && NULL != row[2])
		{
			int nFK_Picture = atoi(row[0]);
			string sDescription(row[1]);
			string sPosition(row[2]);

			//skip start position
			if(sDescription == "START")
				continue;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadBookmarkPictures: Adding bookmark desc %s, pos %s", sDescription.c_str(),
				sPosition.c_str());

			pair<unsigned long, char *> picture_data = LoadPicture(nFK_Picture);
			pair<unsigned long, char *> thumb_picture_data = LoadPicture(nFK_Picture, true);

			PlutoMediaBookmark *pBookmark = new PlutoMediaBookmark(sDescription, sPosition);
			pBookmark->m_dataPicture.m_dwSize = picture_data.first;
			pBookmark->m_dataPicture.m_pBlock = picture_data.second;
			pBookmark->m_dataPictureThumb.m_dwSize = thumb_picture_data.first;
			pBookmark->m_dataPictureThumb.m_pBlock = thumb_picture_data.second;

			m_pPlutoMediaAttributes->m_listBookmarks.push_back(pBookmark);

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LoadBookmarkPictures : too many bookmarks...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadMiscInfo()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_MediaSubType, FK_FileFormat FROM File WHERE "
		"PK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			m_pPlutoMediaAttributes->m_nMediaSubType = atoi(row[0]);
			m_pPlutoMediaAttributes->m_nFileFormat = atoi(row[1]);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SaveMiscInfo: FK_FileFormat %d, FK_MediaSubType %d", 
		m_pPlutoMediaAttributes->m_nFileFormat, m_pPlutoMediaAttributes->m_nMediaSubType);
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadCoverarts()
{
	//if the file was added before, but we already have coverarts in db ignore those from id3
	if(!m_bNewFileToDb && NumberOfCoverartsFromDB())
		m_pPlutoMediaAttributes->ClearCoverarts();

	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_Picture FROM Picture_File "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		int nCounter = 0;
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0])
		{
			int nFK_Picture = atoi(row[0]);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadCoverarts: Adding coverarts, picture id %d...", nFK_Picture);
			m_pPlutoMediaAttributes->m_mapCoverarts.insert(LoadPicture(nFK_Picture));
			m_pPlutoMediaAttributes->m_mapCoverartsThumbs.insert(LoadPicture(nFK_Picture, true));

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LoadCoverarts : too many coverarts...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
pair<unsigned long, char *> PlutoMediaFile::LoadPicture(int nPictureId, bool bThumb/* = false*/)
{
	string sPictureFile = "/home/mediapics/" + StringUtils::ltos(nPictureId) + (bThumb ? "_tn" : "") + ".jpg";

	size_t nSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sPictureFile, nSize);
	return make_pair(static_cast<unsigned long>(nSize), pData);
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SavePicture(pair<unsigned long, char *> pairPicture, int nPictureId, bool bThumb/* = false*/)
{
	string sPictureFile = "/home/mediapics/" + StringUtils::ltos(nPictureId) + (bThumb ? "_tn" : "") + ".jpg";
	FileUtils::WriteBufferIntoFile(sPictureFile, pairPicture.second, pairPicture.first);
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::RenameAttribute(int Attribute_Type, string sOldValue, string sNewValue)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# RenameAttribute : need to rename attribute type %d, value %s with %s", 
		Attribute_Type, sOldValue.c_str(), sNewValue.c_str());

	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(), 
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		if(it->first == Attribute_Type && it->second->m_sName == sOldValue)
		{
			it->second->m_sName = sOldValue;
			break;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetOwnerForPath(string sPath)
{
	// like this: /home/user_xxx/data

	int nEK_Users_Private = 0;
	const string csHomeUserPath = "/home/user_";

	if(sPath.find(csHomeUserPath) == 0)
	{
		string sRemainingPath = sPath.substr(csHomeUserPath.length());
		string sUserId = sRemainingPath.substr(0, sRemainingPath.find("/"));
		nEK_Users_Private = atoi(sUserId.c_str());
	}

	return nEK_Users_Private;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::NumberOfCoverartsFromDB()
{
	PlutoSqlResult result_count;
	DB_ROW row_count;
	string SQLCount = 
		"SELECT Count(*) FROM Picture_File "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	int nNumberOfCoverartsFromDB = 0;
    if((result_count.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQLCount)) && (row_count = db_wrapper_fetch_row(result_count.r)) && NULL != row_count[0])
		nNumberOfCoverartsFromDB = atoi(row_count[0]);

	return nNumberOfCoverartsFromDB;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::NumberOfBookmarksFromDB()
{
	PlutoSqlResult result_count;
	DB_ROW row_count;
	string SQLCount = 
		"SELECT Count(*) FROM Bookmark "
		"WHERE FK_Picture IS NOT NULL AND "
		"FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	int nNumberOfBookmarksFromDB = 0;
    if((result_count.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQLCount)) && (row_count = db_wrapper_fetch_row(result_count.r)) && NULL != row_count[0])
		nNumberOfBookmarksFromDB = atoi(row_count[0]);

	return nNumberOfBookmarksFromDB;
}
//-----------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaIdentifier class
//
//-----------------------------------------------------------------------------------------------------
map<string,int> PlutoMediaIdentifier::m_mapExtensions;
//-----------------------------------------------------------------------------------------------------
/*static*/ void PlutoMediaIdentifier::Activate(Database_pluto_main *pDatabase_pluto_main)
{
	if(m_mapExtensions.size()) //activate once
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Activating Pluto Media Identifier...");
	
    vector<Row_MediaType *> vectRow_MediaType;
    pDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
    for(size_t s=0;s<vectRow_MediaType.size();++s)
    {
        Row_MediaType *pRow_MediaType = vectRow_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
			m_mapExtensions[StringUtils::ToLower(StringUtils::Tokenize(sExtensions,",",pos))] = pRow_MediaType->PK_MediaType_get();
    }

    vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
    pDatabase_pluto_main->DeviceTemplate_MediaType_get()->GetRows("1=1",&vectRow_DeviceTemplate_MediaType);
    for(size_t s=0;s<vectRow_DeviceTemplate_MediaType.size();++s)
    {
        Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_DeviceTemplate_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
            m_mapExtensions[StringUtils::ToLower(StringUtils::Tokenize(sExtensions,",",pos))] = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pluto Media Identifier activated. Extensions %d", m_mapExtensions.size());
}
//-----------------------------------------------------------------------------------------------------
/*static*/ string PlutoMediaIdentifier::GetExtensions()
{
	string sResult;
	for(map<string,int>::iterator it = m_mapExtensions.begin(); it != m_mapExtensions.end(); ++it)
		sResult += "*." + it->first + "\t";

	return sResult;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ int PlutoMediaIdentifier::Identify(string sFilename)
{
    //we'll try first to identify the media based on the extension
    //if we don't find any extention to match it, we'll use MediaIdentifier which uses the magic from files
	string sExtension = FileUtils::FindExtension(sFilename);

	if(sExtension.empty())
		return 0;

    map<string, int>::iterator it = m_mapExtensions.find(StringUtils::ToLower(sExtension));
    return it != m_mapExtensions.end() ? it->second : 0 /*MediaIdentifier::GetFileMediaType(sFilename)*/;
}
//-----------------------------------------------------------------------------------------------------
