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
#include "Table_CommandGroup_Command_CommandParameter.h"
#include "Table_CommandGroup_Command.h"
#include "Table_CommandParameter.h"



void Database_pluto_main::CreateTable_CommandGroup_Command_CommandParameter()
{
	tblCommandGroup_Command_CommandParameter = new Table_CommandGroup_Command_CommandParameter(this);
}

void Database_pluto_main::DeleteTable_CommandGroup_Command_CommandParameter()
{
	delete tblCommandGroup_Command_CommandParameter;
}

Table_CommandGroup_Command_CommandParameter::~Table_CommandGroup_Command_CommandParameter()
{
	map<Table_CommandGroup_Command_CommandParameter::Key, class Row_CommandGroup_Command_CommandParameter*, Table_CommandGroup_Command_CommandParameter::Key_Less>::iterator it;
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


void Row_CommandGroup_Command_CommandParameter::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_CommandGroup_Command_CommandParameter*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_CommandGroup_Command_CommandParameter::Key key(this);					
			map<Table_CommandGroup_Command_CommandParameter::Key, Row_CommandGroup_Command_CommandParameter*, Table_CommandGroup_Command_CommandParameter::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CommandGroup_Command_CommandParameter::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_CommandGroup_Command_CommandParameter::Key key(this);		
		Row_CommandGroup_Command_CommandParameter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CommandGroup_Command_CommandParameter::Row_CommandGroup_Command_CommandParameter(Table_CommandGroup_Command_CommandParameter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CommandGroup_Command_CommandParameter::SetDefaultValues()
{
	m_FK_CommandGroup_Command = 0;
is_null[0] = false;
m_FK_CommandParameter = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
m_psc_frozen = 0;
is_null[6] = false;
m_psc_mod = "00000000000000";
is_null[7] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CommandGroup_Command_CommandParameter::FK_CommandGroup_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CommandGroup_Command;}
long int Row_CommandGroup_Command_CommandParameter::FK_CommandParameter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CommandParameter;}
string Row_CommandGroup_Command_CommandParameter::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}
long int Row_CommandGroup_Command_CommandParameter::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_CommandGroup_Command_CommandParameter::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_CommandGroup_Command_CommandParameter::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_CommandGroup_Command_CommandParameter::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_CommandGroup_Command_CommandParameter::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_CommandGroup_Command_CommandParameter::FK_CommandGroup_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CommandGroup_Command = val; is_modified=true; is_null[0]=false;}
void Row_CommandGroup_Command_CommandParameter::FK_CommandParameter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CommandParameter = val; is_modified=true; is_null[1]=false;}
void Row_CommandGroup_Command_CommandParameter::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[2]=false;}
void Row_CommandGroup_Command_CommandParameter::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_CommandGroup_Command_CommandParameter::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_CommandGroup_Command_CommandParameter::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_CommandGroup_Command_CommandParameter::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_CommandGroup_Command_CommandParameter::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}

		
bool Row_CommandGroup_Command_CommandParameter::Value_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_CommandGroup_Command_CommandParameter::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_CommandGroup_Command_CommandParameter::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_CommandGroup_Command_CommandParameter::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_CommandGroup_Command_CommandParameter::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_CommandGroup_Command_CommandParameter::Value_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_CommandGroup_Command_CommandParameter::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_CommandGroup_Command_CommandParameter::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_CommandGroup_Command_CommandParameter::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_CommandGroup_Command_CommandParameter::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_CommandGroup_Command_CommandParameter::FK_CommandGroup_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandGroup_Command);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::FK_CommandParameter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandParameter);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
return string()+"\""+buf+"\"";
}

string Row_CommandGroup_Command_CommandParameter::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_CommandGroup_Command_CommandParameter::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_CommandGroup_Command_CommandParameter::Key::Key(long int in_FK_CommandGroup_Command, long int in_FK_CommandParameter)
{
			pk_FK_CommandGroup_Command = in_FK_CommandGroup_Command;
pk_FK_CommandParameter = in_FK_CommandParameter;
	
}

Table_CommandGroup_Command_CommandParameter::Key::Key(Row_CommandGroup_Command_CommandParameter *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_CommandGroup_Command = pRow->m_FK_CommandGroup_Command;
pk_FK_CommandParameter = pRow->m_FK_CommandParameter;
	
}		

bool Table_CommandGroup_Command_CommandParameter::Key_Less::operator()(const Table_CommandGroup_Command_CommandParameter::Key &key1, const Table_CommandGroup_Command_CommandParameter::Key &key2) const
{
			if (key1.pk_FK_CommandGroup_Command!=key2.pk_FK_CommandGroup_Command)
return key1.pk_FK_CommandGroup_Command<key2.pk_FK_CommandGroup_Command;
else
if (key1.pk_FK_CommandParameter!=key2.pk_FK_CommandParameter)
return key1.pk_FK_CommandParameter<key2.pk_FK_CommandParameter;
else
return false;	
}	

void Table_CommandGroup_Command_CommandParameter::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_CommandGroup_Command_CommandParameter*>::iterator i = addedRows.begin();
	
		Row_CommandGroup_Command_CommandParameter *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_CommandGroup_Command_asSQL()+", "+pRow->FK_CommandParameter_asSQL()+", "+pRow->Value_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into CommandGroup_Command_CommandParameter (FK_CommandGroup_Command, FK_CommandParameter, Value, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
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
	

	for (map<Key, Row_CommandGroup_Command_CommandParameter*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_CommandGroup_Command_CommandParameter* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_CommandGroup_Command[32];
sprintf(tmp_FK_CommandGroup_Command, "%li", key.pk_FK_CommandGroup_Command);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk_FK_CommandParameter);


string condition;
condition = condition + "FK_CommandGroup_Command=" + tmp_FK_CommandGroup_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_CommandGroup_Command="+pRow->FK_CommandGroup_Command_asSQL()+", FK_CommandParameter="+pRow->FK_CommandParameter_asSQL()+", Value="+pRow->Value_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update CommandGroup_Command_CommandParameter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_CommandGroup_Command_CommandParameter*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_CommandGroup_Command_CommandParameter*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_CommandGroup_Command[32];
sprintf(tmp_FK_CommandGroup_Command, "%li", key.pk_FK_CommandGroup_Command);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk_FK_CommandParameter);


string condition;
condition = condition + "FK_CommandGroup_Command=" + tmp_FK_CommandGroup_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;

	
		string query = "delete from CommandGroup_Command_CommandParameter where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CommandGroup_Command_CommandParameter::GetRows(string where_statement,vector<class Row_CommandGroup_Command_CommandParameter*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from CommandGroup_Command_CommandParameter where " + where_statement;
		
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

		Row_CommandGroup_Command_CommandParameter *pRow = new Row_CommandGroup_Command_CommandParameter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_CommandGroup_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_CommandGroup_Command));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandParameter));
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_CommandGroup_Command_CommandParameter::Key, Row_CommandGroup_Command_CommandParameter*, Table_CommandGroup_Command_CommandParameter::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_CommandGroup_Command_CommandParameter* Table_CommandGroup_Command_CommandParameter::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CommandGroup_Command_CommandParameter *pRow = new Row_CommandGroup_Command_CommandParameter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CommandGroup_Command_CommandParameter* Table_CommandGroup_Command_CommandParameter::GetRow(long int in_FK_CommandGroup_Command, long int in_FK_CommandParameter)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_CommandGroup_Command, in_FK_CommandParameter);

	map<Key, Row_CommandGroup_Command_CommandParameter*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_CommandGroup_Command_CommandParameter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CommandGroup_Command_CommandParameter* Table_CommandGroup_Command_CommandParameter::FetchRow(Table_CommandGroup_Command_CommandParameter::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_CommandGroup_Command[32];
sprintf(tmp_FK_CommandGroup_Command, "%li", key.pk_FK_CommandGroup_Command);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk_FK_CommandParameter);


string condition;
condition = condition + "FK_CommandGroup_Command=" + tmp_FK_CommandGroup_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;


	string query = "select * from CommandGroup_Command_CommandParameter where " + condition;		

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

	Row_CommandGroup_Command_CommandParameter *pRow = new Row_CommandGroup_Command_CommandParameter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_CommandGroup_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_CommandGroup_Command));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandParameter));
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CommandGroup_Command* Row_CommandGroup_Command_CommandParameter::FK_CommandGroup_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_Command *pTable = table->database->CommandGroup_Command_get();
return pTable->GetRow(m_FK_CommandGroup_Command);
}
class Row_CommandParameter* Row_CommandGroup_Command_CommandParameter::FK_CommandParameter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandParameter *pTable = table->database->CommandParameter_get();
return pTable->GetRow(m_FK_CommandParameter);
}





