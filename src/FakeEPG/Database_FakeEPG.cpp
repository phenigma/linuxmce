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

#include "Database_FakeEPG.h"

Database_FakeEPG::Database_FakeEPG()
{
CreateTable_Listing();
}

Database_FakeEPG::~Database_FakeEPG()
{
	DeleteAllTables();
}

void Database_FakeEPG::DeleteAllTables()
{
DeleteTable_Listing();
}

bool Database_FakeEPG::Connect(string host, string user, string pass, string DBName, int port)
{
db_handle = mysql_init(NULL);
if (mysql_real_connect(db_handle, host.c_str(), user.c_str(), pass.c_str(), DBName.c_str(), port, NULL, 0) == NULL)
{return false;}
else
{return true;}
}

