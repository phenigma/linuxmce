/*
 * File: TableInfo.h
 * Defining: class TableInfo
 * Purpose: Storing table parameters and column parameters
 * 
 * Members:
 * Name         Type                Description                  Extra info
 *
 * table_name   string              Table name
 * fields       vector<FieldInfo*>  Vector of column parameters
 * 
 * loadFromDB   bool (*)(MYSQL * db, string table)  Load table definition from database
 * HasPrimaryKeys  bool (*) ()      If table has Primay Keys
 */

#ifndef TableInfo_h
#define TableInfo_h

#include "mysql.h"
#include "FieldInfo.h"

#include <string>
#include <vector>
using namespace std;

class TableInfo
{
protected:
	string table_name;
	vector<FieldInfo*> fields;
	MYSQL *db;

	vector<FieldInfo *> m_vectFieldInfoDependencies;

public:
	TableInfo(MYSQL * use_db) : db(use_db) {}
	// TODO: add destructor

	bool loadFromDB(string table);
	bool HasPrimaryKeys();

	string get_table_name();
};
#endif
