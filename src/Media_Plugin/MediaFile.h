#ifndef MediaFile_h
#define MediaFile_h

#include "MediaTitle.h"
#include "MediaSection.h"
#include "MediaAttributes_LowLevel.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File.h"

class MediaFile
{
public:
	MediaFile(int dwPK_File,string sFullyQualifiedFile)	{
		m_dwPK_File=dwPK_File; m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
	}

	MediaFile(string sMRL)	{
		m_sFilename=sMRL;
		m_dwPK_File=0;
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
	}

	MediaFile(MediaAttributes_LowLevel *pMediaAttributes_LowLevel, string sFullyQualifiedFile) {
		m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_File=pMediaAttributes_LowLevel->GetFileIDFromFilePath(sFullyQualifiedFile);
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
	}

	MediaFile(MediaFile *pMediaFile_Copy) {
		m_dwPK_File=pMediaFile_Copy->m_dwPK_File;
		m_sPath=pMediaFile_Copy->m_sPath;
		m_sFilename=pMediaFile_Copy->m_sFilename;
		m_sDescription=pMediaFile_Copy->m_sDescription;
		m_sStartPosition=pMediaFile_Copy->m_sStartPosition;
		m_dwPK_CommandGroup_Start=pMediaFile_Copy->m_dwPK_CommandGroup_Start;
		m_dwPK_CommandGroup_Stop=pMediaFile_Copy->m_dwPK_CommandGroup_Stop;
		m_dwPK_Bookmark=pMediaFile_Copy->m_dwPK_Bookmark;
		m_dwDuration=pMediaFile_Copy->m_dwDuration;
		m_tTimeout=pMediaFile_Copy->m_tTimeout;
	}


	MediaFile(Row_PlaylistEntry *pRow_PlaylistEntry) {
		m_dwPK_File=pRow_PlaylistEntry->FK_File_get();
		m_dwPK_Bookmark=pRow_PlaylistEntry->FK_Bookmark_get();
		m_dwDuration=pRow_PlaylistEntry->Duration_get();
		m_tTimeout=0;
		m_sPath=pRow_PlaylistEntry->Path_get();
		m_sFilename=pRow_PlaylistEntry->Filename_get();
		m_dwPK_CommandGroup_Start=pRow_PlaylistEntry->EK_CommandGroup_Start_get();
		m_dwPK_CommandGroup_Stop=pRow_PlaylistEntry->EK_CommandGroup_Stop_get();
		if( m_dwPK_Bookmark )
		{
			Row_Bookmark *pRow_Bookmark = pRow_PlaylistEntry->Table_PlaylistEntry_get()->Database_pluto_media_get()->Bookmark_get()->GetRow(m_dwPK_Bookmark);
			if( pRow_Bookmark )
			{
				Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
				if( pRow_File )
				{
					m_dwPK_File = pRow_File->PK_File_get();
					m_sPath = pRow_File->Path_get();
					m_sFilename = pRow_File->Filename_get();
					m_sStartPosition = pRow_Bookmark->Position_get();
				}
			}
		}	

	}

	~MediaFile() {
		for(size_t s=0;s<m_dequeMediaSection.size();++s)
			if( m_dequeMediaSection[s] )
				delete m_dequeMediaSection[s];
	}

	deque<MediaSection *> m_dequeMediaSection;
	deque<MediaTitle *> m_dequeMediaTitle;
	map< int,int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    int m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,int>::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : (*it).second; }
	int m_dwPK_File;
	unsigned long m_dwPK_Bookmark,m_dwDuration,m_dwPK_CommandGroup_Start,m_dwPK_CommandGroup_Stop;
	time_t m_tTimeout;
	string m_sPath,m_sFilename,m_sDescription;
	string m_sStartPosition; /** Where to start the media the first time.  As soon as the media has begun MediaPlugin will reset this */

	string FullyQualifiedFile() {
		if( m_sPath.size() )
			return m_sPath + "/" + m_sFilename; 
		else
			return m_sFilename; 
	}
};

void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional);

#endif
