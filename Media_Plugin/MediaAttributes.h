/**
 *
 * @file MediaAttributes.h
 * @brief header file for the MediaAttribute, MediaPicture, MediaAttributes class
 *
 */

#ifndef MEDIAATTRIBUTES_H
#define MEDIAATTRIBUTES_H

#include "PlutoUtils/MySQLHelper.h"
#include "pluto_media/Database_pluto_media.h"

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


	static listMediaAttribute *AttributesFromString(string Input);
	static string AttributesToString(listMediaAttribute *plistMediaAttribute);

	static listMediaPicture *PicturesFromString(string Input);
	static string PicturesToString(listMediaPicture *plistMediaPicture);
};

#endif
