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

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Attribute.h"
#include "MediaSection.h"

/**
 * @brief Contains a media file
 */

namespace DCE
{
	class MediaStream;
};

class MediaFile;
class Row_Attribute;
class Row_Disc;

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

class MediaAttributes
{
private:
    Database_pluto_media *m_pDatabase_pluto_media;

public:
    /** @brief constructor */
    MediaAttributes(string host, string user, string pass, string db_name, int port);
    ~MediaAttributes();
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
	// First name can either be a separate parameter, or part of Name delimited by a tab
    Row_Attribute *GetAttributeFromDescription(int PK_AttributeType,string sName,string sFirstName=""); 
	void TransformFilenameToDeque(string sFilename,deque<MediaFile *> &dequeMediaFile);

	string GetTabbedName(Row_Attribute *pRow_Attribute)
	{
		if( pRow_Attribute->FirstName_get().size()==0 )
			return pRow_Attribute->Name_get();
		else
			return pRow_Attribute->Name_get() + "\t" + pRow_Attribute->FirstName_get();
	}

	string GetPrintableName(Row_Attribute *pRow_Attribute)
	{
		if( pRow_Attribute->FirstName_get().size()==0 )
			return pRow_Attribute->Name_get();
		else
			return pRow_Attribute->Name_get() + ", " + pRow_Attribute->FirstName_get();
	}

	string GetPrintableName(int PK_Attribute)
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
		if( !pRow_Attribute )
			return "";
		return GetPrintableName(pRow_Attribute);
	}

	MediaSection *GetMediaSection(deque<MediaSection *> *p_dequeMediaSection,int Section) 
	{
		while( Section>p_dequeMediaSection->size() )
			p_dequeMediaSection->push_back(new MediaSection());
		return (*p_dequeMediaSection)[Section-1];
	}
    void MarkAsMissing(int iKey, string fileName);

	bool SavePlaylist(deque<MediaFile *> &dequeMediaFile, int iPK_Users, int &iPK_Playlist, string sPlaylistName );
	int LoadPlaylist(int iPK_Playlist, deque<MediaFile *> &dequeMediaFile, string &sPlaylistName);
	static void PurgeDequeMediaFile(deque<MediaFile *> &dequeMediaFile);


	static listMediaAttribute *AttributesFromString(string Input);
    static string AttributesToString(listMediaAttribute *plistMediaAttribute);

    static listMediaPicture *PicturesFromString(string Input);
    static string PicturesToString(listMediaPicture *plistMediaPicture);

	int AddIdentifiedDiscToDB(string sIdentifiedDisc,MediaStream *pMediaStream);
	void AddAttributesToDisc(Row_Disc *pRow_Disc,int iFileOrTrack,int iSection,map<int,int> *p_mapPK_Attribute);
	bool IsDiscAlreadyIdentified(string sIdentifiedDisc,MediaStream *pMediaStream);
	void AddAttributeToStream(MediaStream *pMediaStream,Row_Attribute *pRow_Attribute,int File,int Track,int Section);
	void LoadStreamAttributes(MediaStream *pMediaStream);
};

#endif
