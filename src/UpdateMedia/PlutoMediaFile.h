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

enum PlutoCustomTag
{
	pctInstallation,
	pctFile,
	pctPicture,
	pctPictureUrl
};

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

    //internal helper functions
	//This will add a record in the File table and additional attributes too in related tables
    int AddFileToDatabase(int PK_MediaType);
	string FileWithAttributes(bool bCreateId3File = true);

	void SavePlutoAttributes(string sFullFileName);
	void LoadPlutoAttributes(string sFullFileName);

	void SyncDbAttributes();

public:
    PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation,
        string sDirectory, string sFile);
    ~PlutoMediaFile();

    int HandleFileNotInDatabase(int PK_MediaType = 0);

    void SetFileAttribute(int PK_File);
    int GetFileAttribute(bool bCreateId3File = true);

    void SetPicAttribute(int PK_Picture, string sPictureUrl);
    int GetPicAttribute(int PK_File);
	static bool IsSupported(string sFileName);
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
