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
#include "Table_Orbiter.h"

#include "Table_CachedScreens.h"
#include "Table_Device_Orbiter.h"
#include "Table_Orbiter_Users_PasswordReq.h"
#include "Table_Orbiter_Variable.h"


void Database_pluto_main::CreateTable_Orbiter()
{
	tblOrbiter = new Table_Orbiter(this);
}

void Database_pluto_main::DeleteTable_Orbiter()
{
	delete tblOrbiter;
}

Table_Orbiter::~Table_Orbiter()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Orbiter *pRow = (Row_Orbiter *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Orbiter *pRow = (Row_Orbiter *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Orbiter::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Orbiter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Orbiter *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Orbiter);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Orbiter::Reload()
{
	Row_Orbiter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Orbiter);
		Row_Orbiter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Orbiter::Row_Orbiter(Table_Orbiter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Orbiter::SetDefaultValues()
{
	m_PK_Orbiter = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_Regen = 1;
is_null[3] = false;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
m_psc_frozen = 0;
is_null[7] = false;
m_psc_mod = "00000000000000";
is_null[8] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Orbiter::PK_Orbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Orbiter;}
string Row_Orbiter::FloorplanInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FloorplanInfo;}
string Row_Orbiter::Modification_LastGen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_LastGen;}
short int Row_Orbiter::Regen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Regen;}
long int Row_Orbiter::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Orbiter::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Orbiter::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Orbiter::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Orbiter::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Orbiter::PK_Orbiter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Orbiter = val; is_modified=true; is_null[0]=false;}
void Row_Orbiter::FloorplanInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FloorplanInfo = val; is_modified=true; is_null[1]=false;}
void Row_Orbiter::Modification_LastGen_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_LastGen = val; is_modified=true; is_null[2]=false;}
void Row_Orbiter::Regen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Regen = val; is_modified=true; is_null[3]=false;}
void Row_Orbiter::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_Orbiter::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_Orbiter::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_Orbiter::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_Orbiter::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}

		
bool Row_Orbiter::FloorplanInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Orbiter::Modification_LastGen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Orbiter::Regen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Orbiter::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Orbiter::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Orbiter::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Orbiter::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Orbiter::FloorplanInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Orbiter::Modification_LastGen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Orbiter::Regen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Orbiter::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Orbiter::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Orbiter::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Orbiter::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Orbiter::PK_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Orbiter);

return buf;
}

string Row_Orbiter::FloorplanInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[5000000];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_FloorplanInfo.c_str(), (unsigned long) min(16777215,m_FloorplanInfo.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Orbiter::Modification_LastGen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[39];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Modification_LastGen.c_str(), (unsigned long) min(19,m_Modification_LastGen.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Orbiter::Regen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Regen);

return buf;
}

string Row_Orbiter::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Orbiter::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Orbiter::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Orbiter::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Orbiter::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Orbiter::Key::Key(long int in_PK_Orbiter)
{
			pk_PK_Orbiter = in_PK_Orbiter;
	
}

Table_Orbiter::Key::Key(Row_Orbiter *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Orbiter = pRow->m_PK_Orbiter;
	
}		

bool Table_Orbiter::Key_Less::operator()(const Table_Orbiter::Key &key1, const Table_Orbiter::Key &key2) const
{
			if (key1.pk_PK_Orbiter!=key2.pk_PK_Orbiter)
return key1.pk_PK_Orbiter<key2.pk_PK_Orbiter;
else
return false;	
}	

bool Table_Orbiter::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Orbiter *pRow = (Row_Orbiter *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Orbiter_asSQL()+", "+pRow->FloorplanInfo_asSQL()+", "+pRow->Modification_LastGen_asSQL()+", "+pRow->Regen_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Orbiter (`PK_Orbiter`, `FloorplanInfo`, `Modification_LastGen`, `Regen`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_Orbiter=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Orbiter);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Orbiter* pRow = (Row_Orbiter*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Orbiter);

		char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk);


string condition;
condition = condition + "`PK_Orbiter`=" + tmp_PK_Orbiter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Orbiter`="+pRow->PK_Orbiter_asSQL()+", `FloorplanInfo`="+pRow->FloorplanInfo_asSQL()+", `Modification_LastGen`="+pRow->Modification_LastGen_asSQL()+", `Regen`="+pRow->Regen_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Orbiter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Orbiter* pRow = (Row_Orbiter*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Orbiter* pRow = (Row_Orbiter*) (*i).second;	

		char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk);


string condition;
condition = condition + "`PK_Orbiter`=" + tmp_PK_Orbiter;

	
		string query = "delete from Orbiter where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Orbiter*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Orbiter::GetRows(string where_statement,vector<class Row_Orbiter*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Orbiter`.* from Orbiter " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Orbiter`.* from Orbiter where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_Orbiter *pRow = new Row_Orbiter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FloorplanInfo = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_FloorplanInfo = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Modification_LastGen = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Modification_LastGen = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Regen = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Regen));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Orbiter);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Orbiter *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Orbiter* Table_Orbiter::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Orbiter *pRow = new Row_Orbiter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Orbiter* Table_Orbiter::GetRow(long int in_PK_Orbiter)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Orbiter);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Orbiter*) (*i).second;
	//we have to fetch row
	Row_Orbiter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Orbiter* Table_Orbiter::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk);


string condition;
condition = condition + "`PK_Orbiter`=" + tmp_PK_Orbiter;


	string query = "select * from Orbiter where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
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

	Row_Orbiter *pRow = new Row_Orbiter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FloorplanInfo = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_FloorplanInfo = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Modification_LastGen = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Modification_LastGen = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Regen = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Regen));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_Orbiter::CachedScreens_FK_Orbiter_getrows(vector <class Row_CachedScreens*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CachedScreens *pTable = table->database->CachedScreens_get();
pTable->GetRows("`FK_Orbiter`=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Device_Orbiter_FK_Orbiter_getrows(vector <class Row_Device_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Orbiter *pTable = table->database->Device_Orbiter_get();
pTable->GetRows("`FK_Orbiter`=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Orbiter_Users_PasswordReq_FK_Orbiter_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter_Users_PasswordReq *pTable = table->database->Orbiter_Users_PasswordReq_get();
pTable->GetRows("`FK_Orbiter`=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Orbiter_Variable_FK_Orbiter_getrows(vector <class Row_Orbiter_Variable*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter_Variable *pTable = table->database->Orbiter_Variable_get();
pTable->GetRows("`FK_Orbiter`=" + StringUtils::itos(m_PK_Orbiter),rows);
}



