/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <winsock.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <mysql.h>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_Region.h"
#include "Table_Country.h"

#include "Table_City.h"


void Database_pluto_main::CreateTable_Region()
{
	tblRegion = new Table_Region(this);
}

void Database_pluto_main::DeleteTable_Region()
{
	if( tblRegion )
		delete tblRegion;
}

bool Database_pluto_main::Commit_Region(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRegion->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Region::~Table_Region()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Region *pRow = (Row_Region *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Region *pRow = (Row_Region *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Region::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Region *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Region *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Region);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Region::Reload()
{
	Row_Region *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Region);
		Row_Region *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Region::Row_Region(Table_Region *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Region::SetDefaultValues()
{
	m_PK_Region = 0;
is_null[0] = false;
m_FK_Country = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Region::PK_Region_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PK_Region;}
long int Row_Region::FK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Country;}
string Row_Region::Region_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Region;}
string Row_Region::Code_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Code;}
string Row_Region::ADM1Code_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_ADM1Code;}

		
void Row_Region::PK_Region_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PK_Region = val; is_modified=true; is_null[0]=false;}
void Row_Region::FK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Country = val; is_modified=true; is_null[1]=false;}
void Row_Region::Region_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Region = val; is_modified=true; is_null[2]=false;}
void Row_Region::Code_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Code = val; is_modified=true; is_null[3]=false;}
void Row_Region::ADM1Code_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_ADM1Code = val; is_modified=true; is_null[4]=false;}

		
bool Row_Region::Region_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[2];}
bool Row_Region::Code_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_Region::ADM1Code_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}

			
void Row_Region::Region_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Region::Code_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Region::ADM1Code_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
	

string Row_Region::PK_Region_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Region);

return buf;
}

string Row_Region::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}

string Row_Region::Region_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char *buf = new char[91];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Region.c_str(), (unsigned long) min((size_t)45,m_Region.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Region::Code_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char *buf = new char[17];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Code.c_str(), (unsigned long) min((size_t)8,m_Code.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Region::ADM1Code_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char *buf = new char[9];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_ADM1Code.c_str(), (unsigned long) min((size_t)4,m_ADM1Code.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_Region::Key::Key(long int in_PK_Region)
{
			pk_PK_Region = in_PK_Region;
	
}

Table_Region::Key::Key(Row_Region *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_PK_Region = pRow->m_PK_Region;
	
}		

bool Table_Region::Key_Less::operator()(const Table_Region::Key &key1, const Table_Region::Key &key2) const
{
			if (key1.pk_PK_Region!=key2.pk_PK_Region)
return key1.pk_PK_Region<key2.pk_PK_Region;
else
return false;	
}	

bool Table_Region::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Region *pRow = (Row_Region *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Region_asSQL()+", "+pRow->FK_Country_asSQL()+", "+pRow->Region_asSQL()+", "+pRow->Code_asSQL()+", "+pRow->ADM1Code_asSQL();

	
		string query = "insert into Region (`PK_Region`, `FK_Country`, `Region`, `Code`, `ADM1Code`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			bool bResult=database->MySQLConnect(true);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::Commit Cannot perform query [%s] %s reconnect: %d",query.c_str(),database->m_sLastMySqlError.c_str(),(int) bResult);
			if( bDeleteFailedInsertRow )
			{
				addedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id = (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
		pRow->m_PK_Region=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Region is auto increment but has no value %s",database->m_sLastMySqlError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Region);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Region* pRow = (Row_Region*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Region);

		char tmp_PK_Region[32];
sprintf(tmp_PK_Region, "%li", key.pk);


string condition;
condition = condition + "`PK_Region`=" + tmp_PK_Region;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Region`="+pRow->PK_Region_asSQL()+", `FK_Country`="+pRow->FK_Country_asSQL()+", `Region`="+pRow->Region_asSQL()+", `Code`="+pRow->Code_asSQL()+", `ADM1Code`="+pRow->ADM1Code_asSQL();

	
		string query = "update Region set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			bool bResult=database->MySQLConnect(true);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::Commit Cannot perform update query [%s] %s reconnect: %d",query.c_str(),database->m_sLastMySqlError.c_str(),(int) bResult);
			if( bDeleteFailedModifiedRow )
			{
				cachedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Region* pRow = (Row_Region*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Region* pRow = (Row_Region*) (*i).second;	

		char tmp_PK_Region[32];
sprintf(tmp_PK_Region, "%li", key.pk);


string condition;
condition = condition + "`PK_Region`=" + tmp_PK_Region;

	
		string query = "delete from Region where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			bool bResult=database->MySQLConnect(true);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::Commit Cannot perform delete query [%s] %s reconnect: %d",query.c_str(),database->m_sLastMySqlError.c_str(),(int) bResult);
			return false;
		}	
		
		pRow = (Row_Region*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Region::GetRows(string where_statement,vector<class Row_Region*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Region`.* from Region " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Region`.* from Region where " + where_statement;
	else
		query = "select `Region`.* from Region";
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		bool bResult=database->MySQLConnect(true);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::GetRows Cannot perform query [%s] %s reconnect: %d",query.c_str(),database->m_sLastMySqlError.c_str(),(int) bResult);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::GetRows mysql_store_result returned NULL handler");
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_Region *pRow = new Row_Region(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Region = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Region));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Country));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Region = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Region = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Code = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Code = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ADM1Code = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ADM1Code = string(row[4],lengths[4]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Region);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Region *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Region* Table_Region::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_Region *pRow = new Row_Region(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Region* Table_Region::GetRow(long int in_PK_Region)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	SingleLongKey row_key(in_PK_Region);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Region*) (*i).second;
	//we have to fetch row
	Row_Region* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Region* Table_Region::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_PK_Region[32];
sprintf(tmp_PK_Region, "%li", key.pk);


string condition;
condition = condition + "`PK_Region`=" + tmp_PK_Region;


	string query = "select * from Region where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		bool bResult=database->MySQLConnect(true);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::FetchRow Cannot perform query [%s] %s reconnect: %d",query.c_str(),database->m_sLastMySqlError.c_str(),(int) bResult);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Region::FetchRow mysql_store_result returned NULL handler");
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	
	
	MYSQL_ROW row = mysql_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		mysql_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = mysql_fetch_lengths(res);

	Row_Region *pRow = new Row_Region(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Region = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Region));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Country));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Region = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Region = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Code = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Code = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ADM1Code = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ADM1Code = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Country* Row_Region::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}


void Row_Region::City_FK_Region_getrows(vector <class Row_City*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_City *pTable = table->database->City_get();
pTable->GetRows("`FK_Region`=" + StringUtils::itos(m_PK_Region),rows);
}



