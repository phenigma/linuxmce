#ifndef MediaFile_h
#define MediaFile_h

#include "MediaTitle.h"
#include "MediaSection.h"

class MediaFile
{
public:
	MediaFile(int dwPK_File,string sFullyQualifiedFile)	{
		m_dwPK_File=dwPK_File; m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
	}

	MediaFile(string sMRL)	{
		m_sFilename=sMRL;
		m_dwPK_File=0;
	}

	MediaFile(MediaAttributes *pMediaAttributes, string sFullyQualifiedFile) {
		m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_File=pMediaAttributes->GetFileIDFromFilePath(sFullyQualifiedFile);
	}

	MediaFile(MediaFile *pMediaFile_Copy) {
		m_dwPK_File=pMediaFile_Copy->m_dwPK_File;
		m_sPath=pMediaFile_Copy->m_sPath;
		m_sFilename=pMediaFile_Copy->m_sFilename;
		m_sDescription=pMediaFile_Copy->m_sDescription;
		m_sStartPosition=pMediaFile_Copy->m_sStartPosition;
	}


	MediaFile(Row_PlaylistEntry *pRow_PlaylistEntry) {
		m_dwPK_File=pRow_PlaylistEntry->FK_File_get(); 
		m_sPath=pRow_PlaylistEntry->Path_get();
		m_sFilename=pRow_PlaylistEntry->Filename_get();
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
