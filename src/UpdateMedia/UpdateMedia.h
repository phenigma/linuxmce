#ifndef UpdateMedia_h
#define UpdateMedia_h

#include "pluto_media/Database_pluto_media.h"

class UpdateMedia 
{
	string m_sDirectory,m_sExtensions;

public:
    Database_pluto_media *m_pDatabase_pluto_media ;
	UpdateMedia(string host, string user, string pass, string db_name, int port,string sDirectory,string sExtensions);

	void DoIt();

	int ReadDirectory(string Directory); // Returns the picture for the directory
	void SetPicAttribute(string sDirectory,string sFile,int PK_Picture);
	void SetFileAttribute(string sDirectory,string sFile,int PK_File);
	int GetFileAttribute(string sDirectory,string sFile);
	int AddFileToDatabase(string sDirectory,string sFile);
};
#endif
