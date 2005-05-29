#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_media.h"


#include "DCEConfig.h"


namespace DCE
{
	Logger *g_pPlutoLogger; //dummy
}
using namespace DCE;
Database_pluto_media::Database_pluto_media()
{
tblAttribute=NULL;
tblAttributeType=NULL;
tblDisc=NULL;
tblDisc_Attribute=NULL;
tblFile=NULL;
tblFile_Attribute=NULL;
tblPicture=NULL;
tblPicture_Attribute=NULL;
tblPicture_Disc=NULL;
tblPicture_File=NULL;
tblPlaylist=NULL;
tblPlaylistEntry=NULL;
tblSearchToken=NULL;
tblSearchToken_Attribute=NULL;
tblType=NULL;
tblType_AttributeType=NULL;
tblType_Extension=NULL;
tblpsc_media_batdet=NULL;
tblpsc_media_bathdr=NULL;
tblpsc_media_batuser=NULL;
tblpsc_media_repset=NULL;
tblpsc_media_schema=NULL;
tblpsc_media_tables=NULL;
}

Database_pluto_media::~Database_pluto_media()
{
	DeleteAllTables();
}

void Database_pluto_media::DeleteAllTables()
{
DeleteTable_Attribute();
DeleteTable_AttributeType();
DeleteTable_Disc();
DeleteTable_Disc_Attribute();
DeleteTable_File();
DeleteTable_File_Attribute();
DeleteTable_Picture();
DeleteTable_Picture_Attribute();
DeleteTable_Picture_Disc();
DeleteTable_Picture_File();
DeleteTable_Playlist();
DeleteTable_PlaylistEntry();
DeleteTable_SearchToken();
DeleteTable_SearchToken_Attribute();
DeleteTable_Type();
DeleteTable_Type_AttributeType();
DeleteTable_Type_Extension();
DeleteTable_psc_media_batdet();
DeleteTable_psc_media_bathdr();
DeleteTable_psc_media_batuser();
DeleteTable_psc_media_repset();
DeleteTable_psc_media_schema();
DeleteTable_psc_media_tables();
}

bool Database_pluto_media::Connect(string host, string user, string pass, string sDBName, int port)
{
return MySQLConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_media::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
