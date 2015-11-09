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
#include <WinSock2.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_psc_local_batdet.h"
#include "Table_psc_local_bathdr.h"
#include "Table_psc_local_bathdr.h"
#include "Table_psc_local_bathdr.h"
#include "Table_psc_local_bathdr.h"



void Database_pluto_main::CreateTable_psc_local_batdet()
{
	tblpsc_local_batdet = new Table_psc_local_batdet(this);
}

void Database_pluto_main::DeleteTable_psc_local_batdet()
{
	if( tblpsc_local_batdet )
		delete tblpsc_local_batdet;
}

bool Database_pluto_main::Commit_psc_local_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblpsc_local_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_psc_local_batdet::~Table_psc_local_batdet()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_local_batdet *pRow = (Row_psc_local_batdet *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_local_batdet *pRow = (Row_psc_local_batdet *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_local_batdet::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_psc_local_batdet *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_local_batdet *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_local_batdet);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_psc_local_batdet::Reload()
{
	Row_psc_local_batdet *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_local_batdet);
		Row_psc_local_batdet *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_local_batdet::Row_psc_local_batdet(Table_psc_local_batdet *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_local_batdet::SetDefaultValues()
{
	m_PK_psc_local_batdet = 0;
is_null[0] = false;
m_FK_psc_local_bathdr = 0;
is_null[1] = false;
m_Tablename = "";
is_null[2] = false;
m_New = 0;
is_null[3] = false;
m_Deleted = 0;
is_null[4] = false;
m_Modified = 0;
is_null[5] = false;
m_FK_psc_local_bathdr_orig = 0;
is_null[6] = false;
m_FK_psc_local_bathdr_auth = 0;
is_null[7] = false;
m_FK_psc_local_bathdr_unauth = 0;
is_null[8] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_local_batdet::PK_psc_local_batdet_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_psc_local_batdet;}
long int Row_psc_local_batdet::FK_psc_local_bathdr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_psc_local_bathdr;}
string Row_psc_local_batdet::Tablename_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Tablename;}
long int Row_psc_local_batdet::New_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_New;}
long int Row_psc_local_batdet::Deleted_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Deleted;}
long int Row_psc_local_batdet::Modified_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Modified;}
long int Row_psc_local_batdet::FK_psc_local_bathdr_orig_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_psc_local_bathdr_orig;}
long int Row_psc_local_batdet::FK_psc_local_bathdr_auth_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_psc_local_bathdr_auth;}
long int Row_psc_local_batdet::FK_psc_local_bathdr_unauth_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_psc_local_bathdr_unauth;}

		
void Row_psc_local_batdet::PK_psc_local_batdet_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_psc_local_batdet = val; is_modified=true; is_null[0]=false;}
void Row_psc_local_batdet::FK_psc_local_bathdr_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_psc_local_bathdr = val; is_modified=true; is_null[1]=false;}
void Row_psc_local_batdet::Tablename_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Tablename = val; is_modified=true; is_null[2]=false;}
void Row_psc_local_batdet::New_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_New = val; is_modified=true; is_null[3]=false;}
void Row_psc_local_batdet::Deleted_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Deleted = val; is_modified=true; is_null[4]=false;}
void Row_psc_local_batdet::Modified_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Modified = val; is_modified=true; is_null[5]=false;}
void Row_psc_local_batdet::FK_psc_local_bathdr_orig_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_psc_local_bathdr_orig = val; is_modified=true; is_null[6]=false;}
void Row_psc_local_batdet::FK_psc_local_bathdr_auth_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_psc_local_bathdr_auth = val; is_modified=true; is_null[7]=false;}
void Row_psc_local_batdet::FK_psc_local_bathdr_unauth_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_psc_local_bathdr_unauth = val; is_modified=true; is_null[8]=false;}

		

			
	

string Row_psc_local_batdet::PK_psc_local_batdet_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_local_batdet);

return buf;
}

string Row_psc_local_batdet::FK_psc_local_bathdr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_local_bathdr);

return buf;
}

string Row_psc_local_batdet::Tablename_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Tablename.c_str(), (unsigned long) min((size_t)60,m_Tablename.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_psc_local_batdet::New_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_New);

return buf;
}

string Row_psc_local_batdet::Deleted_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Deleted);

return buf;
}

string Row_psc_local_batdet::Modified_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Modified);

return buf;
}

string Row_psc_local_batdet::FK_psc_local_bathdr_orig_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_local_bathdr_orig);

return buf;
}

string Row_psc_local_batdet::FK_psc_local_bathdr_auth_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_local_bathdr_auth);

return buf;
}

string Row_psc_local_batdet::FK_psc_local_bathdr_unauth_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_local_bathdr_unauth);

return buf;
}




Table_psc_local_batdet::Key::Key(long int in_PK_psc_local_batdet)
{
			pk_PK_psc_local_batdet = in_PK_psc_local_batdet;
	
}

Table_psc_local_batdet::Key::Key(Row_psc_local_batdet *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_psc_local_batdet = pRow->m_PK_psc_local_batdet;
	
}		

bool Table_psc_local_batdet::Key_Less::operator()(const Table_psc_local_batdet::Key &key1, const Table_psc_local_batdet::Key &key2) const
{
			if (key1.pk_PK_psc_local_batdet!=key2.pk_PK_psc_local_batdet)
return key1.pk_PK_psc_local_batdet<key2.pk_PK_psc_local_batdet;
else
return false;	
}	

bool Table_psc_local_batdet::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_local_batdet *pRow = (Row_psc_local_batdet *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_local_batdet_asSQL()+", "+pRow->FK_psc_local_bathdr_asSQL()+", "+pRow->Tablename_asSQL()+", "+pRow->New_asSQL()+", "+pRow->Deleted_asSQL()+", "+pRow->Modified_asSQL()+", "+pRow->FK_psc_local_bathdr_orig_asSQL()+", "+pRow->FK_psc_local_bathdr_auth_asSQL()+", "+pRow->FK_psc_local_bathdr_unauth_asSQL();

	
		string query = "insert into psc_local_batdet (`PK_psc_local_batdet`, `FK_psc_local_bathdr`, `Tablename`, `New`, `Deleted`, `Modified`, `FK_psc_local_bathdr_orig`, `FK_psc_local_bathdr_auth`, `FK_psc_local_bathdr_unauth`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedInsertRow )
				{
					addedRows.erase(i);
					delete pRow;
				}
				break;   // Go ahead and continue to do the updates
			}
		}
	
		if (db_wrapper_affected_rows(database->m_pDB)!=0)
		{
			
			
			long int id = (long int) db_wrapper_insert_id(database->m_pDB);
		
			if (id!=0)
		pRow->m_PK_psc_local_batdet=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_psc_local_batdet is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_local_batdet);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_local_batdet* pRow = (Row_psc_local_batdet*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_local_batdet);

		char tmp_PK_psc_local_batdet[32];
sprintf(tmp_PK_psc_local_batdet, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_local_batdet`=" + tmp_PK_psc_local_batdet;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_psc_local_batdet`="+pRow->PK_psc_local_batdet_asSQL()+", `FK_psc_local_bathdr`="+pRow->FK_psc_local_bathdr_asSQL()+", `Tablename`="+pRow->Tablename_asSQL()+", `New`="+pRow->New_asSQL()+", `Deleted`="+pRow->Deleted_asSQL()+", `Modified`="+pRow->Modified_asSQL()+", `FK_psc_local_bathdr_orig`="+pRow->FK_psc_local_bathdr_orig_asSQL()+", `FK_psc_local_bathdr_auth`="+pRow->FK_psc_local_bathdr_auth_asSQL()+", `FK_psc_local_bathdr_unauth`="+pRow->FK_psc_local_bathdr_unauth_asSQL();

	
		string query = "update psc_local_batdet set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedModifiedRow )
				{
					cachedRows.erase(i);
					delete pRow;
				}
				break;  // Go ahead and do the deletes
			}
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_psc_local_batdet* pRow = (Row_psc_local_batdet*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_local_batdet* pRow = (Row_psc_local_batdet*) (*i).second;	

		char tmp_PK_psc_local_batdet[32];
sprintf(tmp_PK_psc_local_batdet, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_local_batdet`=" + tmp_PK_psc_local_batdet;

	
		string query = "delete from psc_local_batdet where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_psc_local_batdet*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_psc_local_batdet::GetRows(string where_statement,vector<class Row_psc_local_batdet*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `psc_local_batdet`.* from psc_local_batdet " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `psc_local_batdet`.* from psc_local_batdet where " + where_statement;
	else
		query = "select `psc_local_batdet`.* from psc_local_batdet";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_psc_local_batdet *pRow = new Row_psc_local_batdet(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_local_batdet = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_local_batdet));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_local_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_local_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Tablename = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Tablename = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_New = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_New));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Deleted = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Deleted));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modified = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Modified));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_psc_local_bathdr_orig = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_psc_local_bathdr_orig));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_psc_local_bathdr_auth = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_psc_local_bathdr_auth));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_psc_local_bathdr_unauth = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_psc_local_bathdr_unauth));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_psc_local_batdet);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_local_batdet *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_psc_local_batdet* Table_psc_local_batdet::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_psc_local_batdet *pRow = new Row_psc_local_batdet(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_local_batdet* Table_psc_local_batdet::GetRow(long int in_PK_psc_local_batdet)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_psc_local_batdet);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_local_batdet*) (*i).second;
	//we have to fetch row
	Row_psc_local_batdet* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_local_batdet* Table_psc_local_batdet::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_psc_local_batdet[32];
sprintf(tmp_PK_psc_local_batdet, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_local_batdet`=" + tmp_PK_psc_local_batdet;


	string query = "select * from psc_local_batdet where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_local_batdet::FetchRow db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return NULL;
	}	
	
	DB_ROW row = db_wrapper_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		db_wrapper_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = db_wrapper_fetch_lengths(res);

	Row_psc_local_batdet *pRow = new Row_psc_local_batdet(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_local_batdet = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_local_batdet));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_local_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_local_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Tablename = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Tablename = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_New = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_New));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Deleted = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Deleted));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modified = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Modified));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_psc_local_bathdr_orig = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_psc_local_bathdr_orig));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_psc_local_bathdr_auth = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_psc_local_bathdr_auth));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_psc_local_bathdr_unauth = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_psc_local_bathdr_unauth));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_psc_local_bathdr* Row_psc_local_batdet::FK_psc_local_bathdr_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_local_bathdr *pTable = table->database->psc_local_bathdr_get();
return pTable->GetRow(m_FK_psc_local_bathdr);
}
class Row_psc_local_bathdr* Row_psc_local_batdet::FK_psc_local_bathdr_orig_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_local_bathdr *pTable = table->database->psc_local_bathdr_get();
return pTable->GetRow(m_FK_psc_local_bathdr_orig);
}
class Row_psc_local_bathdr* Row_psc_local_batdet::FK_psc_local_bathdr_auth_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_local_bathdr *pTable = table->database->psc_local_bathdr_get();
return pTable->GetRow(m_FK_psc_local_bathdr_auth);
}
class Row_psc_local_bathdr* Row_psc_local_batdet::FK_psc_local_bathdr_unauth_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_local_bathdr *pTable = table->database->psc_local_bathdr_get();
return pTable->GetRow(m_FK_psc_local_bathdr_unauth);
}





