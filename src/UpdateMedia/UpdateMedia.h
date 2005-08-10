#ifndef UpdateMedia_h
#define UpdateMedia_h

#include "pluto_media/Database_pluto_media.h"
#include "pluto_main/Database_pluto_main.h"

class UpdateMedia 
{
	string m_sDirectory,m_sExtensions;
	map<string,int> m_mapExtensions;

public:
    Database_pluto_media *m_pDatabase_pluto_media ;
    Database_pluto_main *m_pDatabase_pluto_main ;
	UpdateMedia(string host, string user, string pass, int port,string sDirectory);

	void DoIt();

	int ReadDirectory(string Directory); // Returns the picture for the directory
	void SetPicAttribute(string sDirectory,string sFile,int PK_Picture);
	void SetFileAttribute(string sDirectory,string sFile,int PK_File);
	int GetFileAttribute(string sDirectory,string sFile);
	int AddFileToDatabase(int PK_MediaType,string sDirectory,string sFile);
	int GetPicForFileOrDirectory(string sFile,int PK_File); // if PK_File is 0, we will find it
	int HandleFileNotInDatabase(string sDirectory,string sFile,int PK_MediaType=0);
	void UpdateSearchTokens();
	void UpdateThumbnails();
};
#endif
