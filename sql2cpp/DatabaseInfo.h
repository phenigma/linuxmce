#ifndef DatabaseInfo_h
#define DatabaseInfo_h

#include "mysql.h"
#include <iostream>
using namespace std;

#ifdef GENERATOR
	#include "TableInfo_Generator.h"
	#define TABLEINFO_TYPE TableInfo_Generator
#else
	#include "TableInfo_SCVS.h"
	#define TABLEINFO_TYPE TableInfo_SCVS
#endif

class DatabaseInfo
{
	vector<class TABLEINFO_TYPE *> m_listTableInfo;
	MYSQL *db;
	map<string,TableInfo_Generator *> map_tables_info; // For faster lookups

public:
	DatabaseInfo(string db_host, string db_user, string db_pass, string db_name, int db_port)
	{
		// Establishing database connection			  	
		db = mysql_init(NULL);
			  
		if (mysql_real_connect(db, db_host.c_str(), db_user.c_str(), db_pass.c_str(), db_name.c_str(), db_port, NULL, 0) == NULL)
		{
			cout << "MySQL connection failed" << endl;
			throw string("MySQL connect failed");
		}
		else
			cout << "Connection established!" << endl;
	}

	~DatabaseInfo() { mysql_close(db); }

	void getDatabaseTablesList();
	vector<class TABLEINFO_TYPE *> *listTableInfo_get() { return &m_listTableInfo; }
	void ConvertTablesToStrings(vector<string> *vect_string);
};

#endif
