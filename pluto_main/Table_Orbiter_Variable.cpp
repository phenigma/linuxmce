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
#include <list>

#include <mysql.h>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_Orbiter_Variable.h"
#include "Table_Orbiter.h"
#include "Table_Variable.h"



void Database_pluto_main::CreateTable_Orbiter_Variable()
{
	tblOrbiter_Variable = new Table_Orbiter_Variable(this);
}

void Database_pluto_main::DeleteTable_Orbiter_Variable()
{
	delete tblOrbiter_Variable;
}

Table_Orbiter_Variable::~Table_Orbiter_Variable()
{
	map<Table_Orbiter_Variable::Key, class Row_Orbiter_Variable*, Table_Orbiter_Variable::Key_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		delete (*it).second;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		delete (*it).second;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Orbiter_Variable::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Orbiter_Variable*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Orbiter_Variable::Key key(this);					
			map<Table_Orbiter_Variable::Key, Row_Orbiter_Variable*, Table_Orbiter_Variable::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Orbiter_Variable::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Orbiter_Variable::Key key(this);		
		Row_Orbiter_Variable *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Orbiter_Variable::Row_Orbiter_Variable(Table_Orbiter_Variable *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Orbiter_Variable::SetDefaultValues()
{
	m_FK_Orbiter = 0;
is_null[0] = false;
m_FK_Variable = 0;
is_null[1] = false;
is_null[2] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[3] = false;
m_IsNew_RecordInfo = 1;
is_null[4] = false;
m_IsDeleted_RecordInfo = 0;
is_null[5] = false;
is_null[6] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Orbiter_Variable::FK_Orbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Orbiter;}
long int Row_Orbiter_Variable::FK_Variable_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Variable;}
string Row_Orbiter_Variable::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}
string Row_Orbiter_Variable::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Orbiter_Variable::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Orbiter_Variable::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Orbiter_Variable::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Orbiter_Variable::FK_Orbiter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Orbiter = val; is_modified=true; is_null[0]=false;}
void Row_Orbiter_Variable::FK_Variable_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Variable = val; is_modified=true; is_null[1]=false;}
void Row_Orbiter_Variable::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[2]=false;}
void Row_Orbiter_Variable::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[3]=false;}
void Row_Orbiter_Variable::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Orbiter_Variable::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Orbiter_Variable::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[6]=false;}

		
bool Row_Orbiter_Variable::Value_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Orbiter_Variable::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Orbiter_Variable::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Orbiter_Variable::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_Orbiter_Variable::Value_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Orbiter_Variable::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Orbiter_Variable::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Orbiter_Variable::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_Orbiter_Variable::FK_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Orbiter);

return buf;
}

string Row_Orbiter_Variable::FK_Variable_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Variable);

return buf;
}

string Row_Orbiter_Variable::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
return string()+"\""+buf+"\"";
}

string Row_Orbiter_Variable::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Orbiter_Variable::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Orbiter_Variable::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Orbiter_Variable::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Orbiter_Variable::Key::Key(long int in_FK_Orbiter, long int in_FK_Variable)
{
			pk_FK_Orbiter = in_FK_Orbiter;
pk_FK_Variable = in_FK_Variable;
	
}

Table_Orbiter_Variable::Key::Key(Row_Orbiter_Variable *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Orbiter = pRow->m_FK_Orbiter;
pk_FK_Variable = pRow->m_FK_Variable;
	
}		

bool Table_Orbiter_Variable::Key_Less::operator()(const Table_Orbiter_Variable::Key &key1, const Table_Orbiter_Variable::Key &key2) const
{
			if (key1.pk_FK_Orbiter!=key2.pk_FK_Orbiter)
return key1.pk_FK_Orbiter<key2.pk_FK_Orbiter;
else
if (key1.pk_FK_Variable!=key2.pk_FK_Variable)
return key1.pk_FK_Variable<key2.pk_FK_Variable;
else
return false;	
}	

void Table_Orbiter_Variable::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Orbiter_Variable*>::iterator i = addedRows.begin();
	
		Row_Orbiter_Variable *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Orbiter_asSQL()+", "+pRow->FK_Variable_asSQL()+", "+pRow->Value_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Orbiter_Variable (FK_Orbiter, FK_Variable, Value, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Orbiter_Variable*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Orbiter_Variable* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Variable[32];
sprintf(tmp_FK_Variable, "%li", key.pk_FK_Variable);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Variable=" + tmp_FK_Variable;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Orbiter="+pRow->FK_Orbiter_asSQL()+", FK_Variable="+pRow->FK_Variable_asSQL()+", Value="+pRow->Value_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Orbiter_Variable set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Orbiter_Variable*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Orbiter_Variable*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Variable[32];
sprintf(tmp_FK_Variable, "%li", key.pk_FK_Variable);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Variable=" + tmp_FK_Variable;

	
		string query = "delete from Orbiter_Variable where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Orbiter_Variable::GetRows(string where_statement,vector<class Row_Orbiter_Variable*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Orbiter_Variable where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query" << endl;
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_Orbiter_Variable *pRow = new Row_Orbiter_Variable(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Variable = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Variable));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Value = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Modification_RecordInfo = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Orbiter_Variable::Key, Row_Orbiter_Variable*, Table_Orbiter_Variable::Key_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Orbiter_Variable* Table_Orbiter_Variable::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Orbiter_Variable *pRow = new Row_Orbiter_Variable(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Orbiter_Variable* Table_Orbiter_Variable::GetRow(long int in_FK_Orbiter, long int in_FK_Variable)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Orbiter, in_FK_Variable);

	map<Key, Row_Orbiter_Variable*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Orbiter_Variable* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Orbiter_Variable* Table_Orbiter_Variable::FetchRow(Table_Orbiter_Variable::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Variable[32];
sprintf(tmp_FK_Variable, "%li", key.pk_FK_Variable);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Variable=" + tmp_FK_Variable;


	string query = "select * from Orbiter_Variable where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query" << endl;
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
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

	Row_Orbiter_Variable *pRow = new Row_Orbiter_Variable(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Variable = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Variable));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Value = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Modification_RecordInfo = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Orbiter* Row_Orbiter_Variable::FK_Orbiter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter *pTable = table->database->Orbiter_get();
return pTable->GetRow(m_FK_Orbiter);
}
class Row_Variable* Row_Orbiter_Variable::FK_Variable_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Variable *pTable = table->database->Variable_get();
return pTable->GetRow(m_FK_Variable);
}





