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
#include "Table_RepositoryType.h"

#include "Table_Installation.h"
#include "Table_Installation.h"
#include "Table_RepositorySource.h"


void Database_pluto_main::CreateTable_RepositoryType()
{
	tblRepositoryType = new Table_RepositoryType(this);
}

void Database_pluto_main::DeleteTable_RepositoryType()
{
	delete tblRepositoryType;
}

Table_RepositoryType::~Table_RepositoryType()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_RepositoryType *pRow = (Row_RepositoryType *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_RepositoryType *pRow = (Row_RepositoryType *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_RepositoryType::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_RepositoryType *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_RepositoryType *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_RepositoryType);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_RepositoryType::Reload()
{
	Row_RepositoryType *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_RepositoryType);
		Row_RepositoryType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_RepositoryType::Row_RepositoryType(Table_RepositoryType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_RepositoryType::SetDefaultValues()
{
	m_PK_RepositoryType = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
m_psc_frozen = 0;
is_null[9] = false;
m_psc_mod = "00000000000000";
is_null[10] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_RepositoryType::PK_RepositoryType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_RepositoryType;}
string Row_RepositoryType::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_RepositoryType::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
short int Row_RepositoryType::SourceOnly_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SourceOnly;}
string Row_RepositoryType::PathToFile_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PathToFile;}
string Row_RepositoryType::Instructions_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Instructions;}
long int Row_RepositoryType::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_RepositoryType::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_RepositoryType::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_RepositoryType::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_RepositoryType::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_RepositoryType::PK_RepositoryType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_RepositoryType = val; is_modified=true; is_null[0]=false;}
void Row_RepositoryType::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_RepositoryType::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_RepositoryType::SourceOnly_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SourceOnly = val; is_modified=true; is_null[3]=false;}
void Row_RepositoryType::PathToFile_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PathToFile = val; is_modified=true; is_null[4]=false;}
void Row_RepositoryType::Instructions_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Instructions = val; is_modified=true; is_null[5]=false;}
void Row_RepositoryType::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_RepositoryType::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_RepositoryType::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_RepositoryType::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_RepositoryType::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}

		
bool Row_RepositoryType::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_RepositoryType::SourceOnly_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_RepositoryType::PathToFile_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_RepositoryType::Instructions_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_RepositoryType::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_RepositoryType::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_RepositoryType::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_RepositoryType::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_RepositoryType::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_RepositoryType::SourceOnly_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_RepositoryType::PathToFile_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_RepositoryType::Instructions_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_RepositoryType::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_RepositoryType::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_RepositoryType::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_RepositoryType::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_RepositoryType::PK_RepositoryType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_RepositoryType);

return buf;
}

string Row_RepositoryType::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_RepositoryType::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_RepositoryType::SourceOnly_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_SourceOnly);

return buf;
}

string Row_RepositoryType::PathToFile_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_PathToFile.c_str(), (unsigned long) m_PathToFile.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_RepositoryType::Instructions_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char *buf = new char[511];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Instructions.c_str(), (unsigned long) m_Instructions.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_RepositoryType::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_RepositoryType::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_RepositoryType::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_RepositoryType::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_RepositoryType::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_RepositoryType::Key::Key(long int in_PK_RepositoryType)
{
			pk_PK_RepositoryType = in_PK_RepositoryType;
	
}

Table_RepositoryType::Key::Key(Row_RepositoryType *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_RepositoryType = pRow->m_PK_RepositoryType;
	
}		

bool Table_RepositoryType::Key_Less::operator()(const Table_RepositoryType::Key &key1, const Table_RepositoryType::Key &key2) const
{
			if (key1.pk_PK_RepositoryType!=key2.pk_PK_RepositoryType)
return key1.pk_PK_RepositoryType<key2.pk_PK_RepositoryType;
else
return false;	
}	

bool Table_RepositoryType::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_RepositoryType *pRow = (Row_RepositoryType *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_RepositoryType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->SourceOnly_asSQL()+", "+pRow->PathToFile_asSQL()+", "+pRow->Instructions_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into RepositoryType (`PK_RepositoryType`, `Description`, `Define`, `SourceOnly`, `PathToFile`, `Instructions`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_RepositoryType=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_RepositoryType);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_RepositoryType* pRow = (Row_RepositoryType*) (*i).second;	
		SingleLongKey key(pRow->m_PK_RepositoryType);

		char tmp_PK_RepositoryType[32];
sprintf(tmp_PK_RepositoryType, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositoryType`=" + tmp_PK_RepositoryType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_RepositoryType`="+pRow->PK_RepositoryType_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `SourceOnly`="+pRow->SourceOnly_asSQL()+", `PathToFile`="+pRow->PathToFile_asSQL()+", `Instructions`="+pRow->Instructions_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update RepositoryType set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_RepositoryType* pRow = (Row_RepositoryType*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_RepositoryType* pRow = (Row_RepositoryType*) (*i).second;	

		char tmp_PK_RepositoryType[32];
sprintf(tmp_PK_RepositoryType, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositoryType`=" + tmp_PK_RepositoryType;

	
		string query = "delete from RepositoryType where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_RepositoryType*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_RepositoryType::GetRows(string where_statement,vector<class Row_RepositoryType*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `RepositoryType`.* from RepositoryType " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `RepositoryType`.* from RepositoryType where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

		Row_RepositoryType *pRow = new Row_RepositoryType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositoryType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositoryType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_SourceOnly = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_SourceOnly));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_PathToFile = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_PathToFile = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Instructions = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Instructions = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_RepositoryType);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_RepositoryType *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_RepositoryType* Table_RepositoryType::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_RepositoryType *pRow = new Row_RepositoryType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_RepositoryType* Table_RepositoryType::GetRow(long int in_PK_RepositoryType)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_RepositoryType);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_RepositoryType*) (*i).second;
	//we have to fetch row
	Row_RepositoryType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_RepositoryType* Table_RepositoryType::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_RepositoryType[32];
sprintf(tmp_PK_RepositoryType, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositoryType`=" + tmp_PK_RepositoryType;


	string query = "select * from RepositoryType where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

	Row_RepositoryType *pRow = new Row_RepositoryType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositoryType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositoryType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_SourceOnly = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_SourceOnly));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_PathToFile = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_PathToFile = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Instructions = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Instructions = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_RepositoryType::Installation_FK_RepositoryType_Source_getrows(vector <class Row_Installation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
pTable->GetRows("`FK_RepositoryType_Source`=" + StringUtils::itos(m_PK_RepositoryType),rows);
}
void Row_RepositoryType::Installation_FK_RepositoryType_Binaries_getrows(vector <class Row_Installation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
pTable->GetRows("`FK_RepositoryType_Binaries`=" + StringUtils::itos(m_PK_RepositoryType),rows);
}
void Row_RepositoryType::RepositorySource_FK_RepositoryType_getrows(vector <class Row_RepositorySource*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
pTable->GetRows("`FK_RepositoryType`=" + StringUtils::itos(m_PK_RepositoryType),rows);
}



