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

#include "Database_pluto_security.h"


#include "DCEConfig.h"

Database_pluto_security::Database_pluto_security()
{
tblAlert=NULL;
tblAlertType=NULL;
tblAlert_Device=NULL;
tblModeChange=NULL;
tblNotification=NULL;
tblpsc_security_batdet=NULL;
tblpsc_security_bathdr=NULL;
tblpsc_security_batuser=NULL;
tblpsc_security_repset=NULL;
tblpsc_security_schema=NULL;
tblpsc_security_tables=NULL;
}

Database_pluto_security::~Database_pluto_security()
{
	DeleteAllTables();
}

void Database_pluto_security::DeleteAllTables()
{
DeleteTable_Alert();
DeleteTable_AlertType();
DeleteTable_Alert_Device();
DeleteTable_ModeChange();
DeleteTable_Notification();
DeleteTable_psc_security_batdet();
DeleteTable_psc_security_bathdr();
DeleteTable_psc_security_batuser();
DeleteTable_psc_security_repset();
DeleteTable_psc_security_schema();
DeleteTable_psc_security_tables();
}

bool Database_pluto_security::Connect(string host, string user, string pass, string sDBName, int port)
{
return MySQLConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_security::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
