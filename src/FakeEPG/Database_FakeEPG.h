#ifndef __Database_FakeEPG_H_
#define __Database_FakeEPG_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
class DLL_EXPORT Database_FakeEPG
{
public:
MYSQL *db_handle;
Database_FakeEPG();
~Database_FakeEPG();
void DeleteAllTables();
private:
class Table_Listing* tblListing;
public:
class Table_Listing* Listing_get() { return tblListing; }
bool Connect(string host, string user, string pass, string DBName, int port);
void Disconnect();
private:
void CreateTable_Listing();
void DeleteTable_Listing();
};
#endif
