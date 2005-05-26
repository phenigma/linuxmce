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


namespace DCE
{
	Logger *g_pPlutoLogger; //dummy
}
using namespace DCE;
Database_pluto_speech::Database_pluto_speech()
{
tblAlias=NULL;
tblKeyword=NULL;
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
return MySQLConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_speech::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
