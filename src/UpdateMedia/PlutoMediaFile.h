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

	MediaSyncMode m_MediaSyncMode;
	PlutoMediaAttributes *m_pPlutoMediaAttributes;

    string m_sDirectory;
    string m_sFile;
	int m_nOurInstallationID;
	int m_nPK_MediaType;
	bool m_bIsDir;

	Database_pluto_media *m_pDatabase_pluto_media;
	static MediaSyncMode m_DefaultMediaSyncMode;

	//id3 operations - load:
	void LoadPlutoAttributes(string sFullFileName);
	void LoadLegacyAttributes(string sFullFileName);
	//id3 operations - save:
	void SavePlutoAttributes(string sFullFileName);

	//db operations - load:
	void LoadStartPosition();
	void LoadEverythingFromDb();
	void LoadShortAttributes();
	void LoadLongAttributes();
	void LoadBookmarkPictures();
	//db operations - save:
	void SaveStartPosition();
	void SaveShortAttributesInDb(bool bAddAllToDb);
	void SaveLongAttributesInDb(bool bAddAllToDb);
	void SaveBookmarkPictures();
	void SaveEveryThingToDb();

	//misc helpers
	int AddFileToDatabase(int PK_MediaType);
	void AssignPlutoDevice();
	string FileWithAttributes(bool bCreateId3File = true);
	int GetOwnerForPath(string sPath);
	int GetFileIDFromDB();
	string AdjustLongAttributeForDisplay(string sText);

	//pictures
	pair<unsigned long, char *> LoadPicture(int nPictureId, bool bThumb = false);
	void SavePicture(pair<unsigned long, char *> pairPicture, int nPictureId, bool bThumb/* = false*/);

public:
    PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation,
        string sDirectory, string sFile);
    ~PlutoMediaFile();

	//synchronization mode:
	void SetSyncMode(MediaSyncMode mode) { m_MediaSyncMode = mode; }
	static void SetDefaultSyncMode(MediaSyncMode mode) { m_DefaultMediaSyncMode = mode; }
	static MediaSyncMode GetDefaultSyncMode() { return m_DefaultMediaSyncMode; }

	void SetFileAttribute(int PK_File);
    int GetFileAttribute();
    void SetPicAttribute(int PK_Picture, string sPictureUrl);
    int GetPicAttribute(int PK_File);

	int HandleFileNotInDatabase(int PK_MediaType = 0);
	void RenameAttribute(int Attribute_Type, string sOldValue, string sNewValue);

	static bool IsSupported(string sFileName);
	static bool IsDirectory(string sFilePath);
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
	static string GetExtensions(); //tab delimited
};
//-----------------------------------------------------------------------------------------------------

#endif
