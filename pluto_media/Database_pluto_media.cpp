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

Database_pluto_media::Database_pluto_media()
{
CreateTable_Attribute();
CreateTable_AttributeType();
CreateTable_File();
CreateTable_File_Attribute();
CreateTable_Picture();
CreateTable_Picture_Attribute();
CreateTable_Picture_File();
CreateTable_SearchToken();
CreateTable_SearchToken_Attribute();
CreateTable_Type();
CreateTable_Type_AttributeType();
CreateTable_Type_Extension();
CreateTable_psc_media_batdet();
CreateTable_psc_media_bathdr();
CreateTable_psc_media_repset();
CreateTable_psc_media_schema();
CreateTable_psc_media_tables();
}

Database_pluto_media::~Database_pluto_media()
{
	DeleteAllTables();
}

void Database_pluto_media::DeleteAllTables()
{
DeleteTable_Attribute();
DeleteTable_AttributeType();
DeleteTable_File();
DeleteTable_File_Attribute();
DeleteTable_Picture();
DeleteTable_Picture_Attribute();
DeleteTable_Picture_File();
DeleteTable_SearchToken();
DeleteTable_SearchToken_Attribute();
DeleteTable_Type();
DeleteTable_Type_AttributeType();
DeleteTable_Type_Extension();
DeleteTable_psc_media_batdet();
DeleteTable_psc_media_bathdr();
DeleteTable_psc_media_repset();
DeleteTable_psc_media_schema();
DeleteTable_psc_media_tables();
}

bool Database_pluto_media::Connect(string host, string user, string pass, string sDBName, int port)
{
db_handle = mysql_init(NULL);
if (mysql_real_connect(db_handle, host.c_str(), user.c_str(), pass.c_str(), sDBName.c_str(), port, NULL, 0) == NULL)
{return false;}
else
{return true;}
}

bool Database_pluto_media::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
