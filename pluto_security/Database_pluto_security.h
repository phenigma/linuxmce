#ifndef __Database_pluto_security_H_
#define __Database_pluto_security_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
class DLL_EXPORT Database_pluto_security
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
public:
class Table_Alert* Alert_get() { return tblAlert; }
class Table_AlertType* AlertType_get() { return tblAlertType; }
class Table_ModeChange* ModeChange_get() { return tblModeChange; }
class Table_Notification* Notification_get() { return tblNotification; }
class Table_Picture* Picture_get() { return tblPicture; }
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Alert();
void CreateTable_AlertType();
void CreateTable_ModeChange();
void CreateTable_Notification();
void CreateTable_Picture();
void DeleteTable_Alert();
void DeleteTable_AlertType();
void DeleteTable_ModeChange();
void DeleteTable_Notification();
void DeleteTable_Picture();
};
#endif
