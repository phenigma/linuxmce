#ifndef __Database_pluto_security_H_
#define __Database_pluto_security_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
#include "PlutoUtils/MySQLHelper.h"
class DLL_EXPORT Database_pluto_security: public MySqlHelper
{
public:
MYSQL *db_handle;
Database_pluto_security();
~Database_pluto_security();
void DeleteAllTables();
private:
class Table_Alert* tblAlert;
class Table_AlertType* tblAlertType;
class Table_ModeChange* tblModeChange;
class Table_Notification* tblNotification;
class Table_Picture* tblPicture;
class Table_psc_security_batdet* tblpsc_security_batdet;
class Table_psc_security_bathdr* tblpsc_security_bathdr;
class Table_psc_security_batuser* tblpsc_security_batuser;
class Table_psc_security_repset* tblpsc_security_repset;
class Table_psc_security_schema* tblpsc_security_schema;
class Table_psc_security_tables* tblpsc_security_tables;
public:
class Table_Alert* Alert_get() { return tblAlert; }
class Table_AlertType* AlertType_get() { return tblAlertType; }
class Table_ModeChange* ModeChange_get() { return tblModeChange; }
class Table_Notification* Notification_get() { return tblNotification; }
class Table_Picture* Picture_get() { return tblPicture; }
class Table_psc_security_batdet* psc_security_batdet_get() { return tblpsc_security_batdet; }
class Table_psc_security_bathdr* psc_security_bathdr_get() { return tblpsc_security_bathdr; }
class Table_psc_security_batuser* psc_security_batuser_get() { return tblpsc_security_batuser; }
class Table_psc_security_repset* psc_security_repset_get() { return tblpsc_security_repset; }
class Table_psc_security_schema* psc_security_schema_get() { return tblpsc_security_schema; }
class Table_psc_security_tables* psc_security_tables_get() { return tblpsc_security_tables; }
string m_sLastMySqlError;
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Alert();
void CreateTable_AlertType();
void CreateTable_ModeChange();
void CreateTable_Notification();
void CreateTable_Picture();
void CreateTable_psc_security_batdet();
void CreateTable_psc_security_bathdr();
void CreateTable_psc_security_batuser();
void CreateTable_psc_security_repset();
void CreateTable_psc_security_schema();
void CreateTable_psc_security_tables();
void DeleteTable_Alert();
void DeleteTable_AlertType();
void DeleteTable_ModeChange();
void DeleteTable_Notification();
void DeleteTable_Picture();
void DeleteTable_psc_security_batdet();
void DeleteTable_psc_security_bathdr();
void DeleteTable_psc_security_batuser();
void DeleteTable_psc_security_repset();
void DeleteTable_psc_security_schema();
void DeleteTable_psc_security_tables();
};
#endif
