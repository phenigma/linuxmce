#ifndef __PLUTO_MEDIA_FILE_H__
#define __PLUTO_MEDIA_FILE_H__

#include <string>
#include <map>
#include <list>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
class Database_pluto_main;
class PlutoMediaAttributes;
class Row_File;
//-----------------------------------------------------------------------------------------------------
enum MediaSyncMode
{
	modeNone,
	modeDbToFile,
	modeFileToDb,
	modeBoth
};
//-----------------------------------------------------------------------------------------------------
extern char *MediaSyncModeStr[];
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaFile
{
private:

	Database_pluto_media *m_pDatabase_pluto_media;
	PlutoMediaAttributes *m_pPlutoMediaAttributes;

    string m_sDirectory;
    string m_sFile;
	int m_nOurInstallationID;
	int m_nPK_MediaType;
	bool m_bIsDir;

	static MediaSyncMode m_DefaultMediaSyncMode;
	MediaSyncMode m_MediaSyncMode;

    //internal helper functions
    int AddFileToDatabase(int PK_MediaType);
	void AssignPlutoDevice();
	string FileWithAttributes(bool bCreateId3File = true);

	void SavePlutoAttributes(string sFullFileName);
	void LoadPlutoAttributes(string sFullFileName);
	void LoadLegacyAttributes(string sFullFileName);
	void LoadAttributesFromDB(string sFullFileName, int PK_File);

	void SyncDbAttributes();
	int GetOwnerForPath(string sPath);
	int GetFileIDFromDB();

	static bool IsSupported(string sFileName);

public:
    PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation,
        string sDirectory, string sFile);
    ~PlutoMediaFile();

	void SetSyncMode(MediaSyncMode mode) { m_MediaSyncMode = mode; }
	static void SetDefaultSyncMode(MediaSyncMode mode) { m_DefaultMediaSyncMode = mode; }
	static MediaSyncMode GetDefaultSyncMode() { return m_DefaultMediaSyncMode; }

    int HandleFileNotInDatabase(int PK_MediaType = 0);
    
	void SetFileAttribute(int PK_File);
    int GetFileAttribute();
    void SetPicAttribute(int PK_Picture, string sPictureUrl);
    int GetPicAttribute(int PK_File);

	void RenameAttribute(int Attribute_Type, string sOldValue, string sNewValue);
};
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaIdentifier class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaIdentifier
{
private:
    static map<string,int> m_mapExtensions;

    virtual ~PlutoMediaIdentifier() = 0; //don't allow instances
public:
    static void Activate(Database_pluto_main *pDatabase_pluto_main);
    static int Identify(string sFilename);
};
//-----------------------------------------------------------------------------------------------------

#endif
