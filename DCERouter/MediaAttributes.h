#ifndef MEDIAATTRIBUTES_H
#define MEDIAATTRIBUTES_H

class MediaAttribute
{
public:
	int PK_MED_Attribute,PK_MED_C_AttributeType;
	string Name,FirstName;
};

class MediaPicture
{
public:
	int PK_MED_Picture,PK_MED_File,PK_MED_Attribute;
	string Path,Extension;
};

typedef list<MediaAttribute *> listMediaAttribute;
typedef list<MediaPicture *> listMediaPicture;

class MediaAttributes
{
public:
	class DCEMI_PS_Media *m_pDCEMI_PS_Media;
	class DCEMI_PS_Main *m_pDCEMI_PS_Main;

	MediaAttributes(class DCEMI_PS_Media *pDCEMI_PS_Media);

	int CreatedMedia(int PK_MED_C_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture);
	void UpdatedMedia(int PK_MED_File,int PK_MED_C_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture);
	int AddAttribute(int PK_MED_File,int PK_MED_Attribute,int PK_MED_C_AttributeType,string Name,string FirstName);
	void ChangeAttribute(int OldPK_MED_C_AttributeType,int NewPK_MED_C_AttributeType,string OldName,string OldFirstName,string NewName,string NewFirstName);
	void ChangeAttribute(int PK_MED_Attribute,string NewName,string NewFirstName);
	void AddPicture(int PK_MED_File,int PK_MED_Attribute,string Path);
	int SetPicture(int PK_MED_Picture,string Path);
	void UpdateSearchTokens(int PK_MED_Attribute);
	void ScanDirectory(string Path);

	// Utility functions
	string GetFilePathFromFileID(int PK_MED_File);
	string GetFilePathsFromAttributeID(int PK_MED_Atribute);
	int GetFileIDFromFilePath(string File);
	string GetAnyPictureUnderDirectory(string File,int *PK_MED_Picture,int MaxDepthToSearch);
	string GetPictureFromFilePath(string File,int *PK_MED_Picture);
	string GetPictureFromFileID(int PK_MED_File,int *PK_MED_Picture);
	string GetPictureFromAttributeID(int PK_MED_Attribute,int *PK_MED_Picture);
	int GetAttributeFromFileID(int PK_MED_File);
	int GetAttributeFromFilePath(string File);
	int GetFileIDFromAttributeID(int PK_MED_Attribute);


	static listMediaAttribute *AttributesFromString(string Input);
	static string AttributesToString(listMediaAttribute *plistMediaAttribute);

	static listMediaPicture *PicturesFromString(string Input);
	static string PicturesToString(listMediaPicture *plistMediaPicture);
};

#endif
