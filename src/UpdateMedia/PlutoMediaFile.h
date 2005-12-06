#ifndef __PLUTO_MEDIA_FILE_H__
#define __PLUTO_MEDIA_FILE_H__

#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
class Database_pluto_main;

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaFile
{
private:
    Database_pluto_media *m_pDatabase_pluto_media;
    Database_pluto_main *m_pDatabase_pluto_main;

    string m_sDirectory;
    string m_sFile;

    //internal helper functions
    int AddFileToDatabase(int PK_MediaType);

public:
    PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, Database_pluto_main *pDatabase_pluto_main,
        string sDirectory, string sFile);
    ~PlutoMediaFile();

    int HandleFileNotInDatabase(int PK_MediaType = 0);

    void SetFileAttribute(int PK_File);
    int GetFileAttribute();

    void SetPicAttribute(int PK_Picture);
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
