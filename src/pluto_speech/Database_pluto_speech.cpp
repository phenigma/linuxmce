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

#include "Database_pluto_speech.h"


#include "DCEConfig.h"

Database_pluto_speech::Database_pluto_speech()
{
CreateTable_Alias();
CreateTable_Keyword();
}

Database_pluto_speech::~Database_pluto_speech()
{
	DeleteAllTables();
}

void Database_pluto_speech::DeleteAllTables()
{
DeleteTable_Alias();
DeleteTable_Keyword();
}

bool Database_pluto_speech::Connect(string host, string user, string pass, string sDBName, int port)
{
db_handle = mysql_init(NULL);
if (mysql_real_connect(db_handle, host.c_str(), user.c_str(), pass.c_str(), sDBName.c_str(), port, NULL, 0) == NULL)
{return false;}
else
{return true;}
}

bool Database_pluto_speech::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
