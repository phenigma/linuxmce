/**
 *
 * @file MediaAttributes.h
 * @brief header file for the MediaAttribute, MediaPicture, MediaAttributes class
 *
 */

#ifndef MEDIAATTRIBUTES_H
#define MEDIAATTRIBUTES_H

#include <deque>
using namespace std;

#include "PlutoUtils/MySQLHelper.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_PlaylistEntry.h"

/**
 * @brief Contains a media file
 */

class MediaFile;  // Class declared at the end of the file since it references MediaAttributes

/**
 * @brief the attributes of the media
 * @todo ask
 */

class MediaAttribute
{
public:
    int PK_Attribute;
    int PK_AttributeType;
    string Name;
    string FirstName;
};

/**
 * @brief documenatation
 * @todo ask
 */

class MediaPicture
{
public:
    int PK_Picture;
    int PK_File;
    int PK_Attribute;
    string Path;
    string Extension;
};

typedef list<MediaAttribute *> listMediaAttribute;
typedef list<MediaPicture *> listMediaPicture;

/**
 * @brief documentation
 * @todo ask
 */

class MediaAttributes : public MySqlHelper
{
private:
    Database_pluto_media *m_pDatabase_pluto_media;

public:
    /** @brief constructor */
    MediaAttributes(string host, string user, string pass, string db_name, int port);
    int CreatedMedia(int PK_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture);
    void UpdatedMedia(int PK_File,int PK_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture);
    int AddAttribute(int PK_File,int PK_Attribute,int PK_AttributeType,string Name,string FirstName);
    void ChangeAttribute(int OldPK_AttributeType,int NewPK_AttributeType,string OldName,string OldFirstName,string NewName,string NewFirstName);
    void ChangeAttribute(int PK_Attribute,string NewName,string NewFirstName);
    void AddPicture(int PK_File,int PK_Attribute,string Path);
    int SetPicture(int PK_Picture,string Path);
    void UpdateSearchTokens(int PK_Attribute);
    void ScanDirectory(string Path);

    // Utility functions
    string GetFilePathFromFileID(int PK_File);
    string GetFilePathsFromAttributeID(int PK_Atribute);
    int GetFileIDFromFilePath(string File);
    string GetAnyPictureUnderDirectory(string File,int *PK_Picture,int MaxDepthToSearch);
    string GetPictureFromFilePath(string File,int *PK_Picture);
    string GetPictureFromFileID(int PK_File,int *PK_Picture);
    string GetPictureFromAttributeID(int PK_Attribute,int *PK_Picture);
    int GetAttributeFromFileID(int PK_File);
    int GetAttributeFromFilePath(string File);
    int GetFileIDFromAttributeID(int PK_Attribute);
	void TransformFilenameToDeque(string sFilename,deque<MediaFile *> &dequeMediaFile);

    void MarkAsMissing(int iKey, string fileName);

	bool SavePlaylist(deque<MediaFile *> &dequeMediaFile, int iPK_Users, int &iPK_Playlist, string sPlaylistName );
	int LoadPlaylist(int iPK_Playlist, deque<MediaFile *> &dequeMediaFile, string &sPlaylistName);
	static void PurgeDequeMediaFile(deque<MediaFile *> &dequeMediaFile);


	static listMediaAttribute *AttributesFromString(string Input);
    static string AttributesToString(listMediaAttribute *plistMediaAttribute);

    static listMediaPicture *PicturesFromString(string Input);
    static string PicturesToString(listMediaPicture *plistMediaPicture);
};

class MediaFile
{
public:
	MediaFile(int dwPK_File,string sFullyQualifiedFile)	{
		m_dwPK_File=dwPK_File; m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
	}

	MediaFile(MediaAttributes *pMediaAttributes, string sFullyQualifiedFile)	{
		m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_File=pMediaAttributes->GetFileIDFromFilePath(sFullyQualifiedFile);
	}

	MediaFile(Row_PlaylistEntry *pRow_PlaylistEntry) {
		m_dwPK_File=pRow_PlaylistEntry->FK_File_get(); 
		m_sPath=pRow_PlaylistEntry->Path_get();
		m_sFilename=pRow_PlaylistEntry->Filename_get();
	}

	~MediaFile() {
	}

	int m_dwPK_File;
	string m_sPath,m_sFilename;
	string FullyQualifiedFile() {
		return m_sPath + "/" + m_sFilename; 
	}
};

#endif
