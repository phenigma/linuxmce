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
#include "Table_Command_CommandParameter.h"
#include "Table_Command.h"
#include "Table_CommandParameter.h"



void Database_pluto_main::CreateTable_Command_CommandParameter()
{
	tblCommand_CommandParameter = new Table_Command_CommandParameter(this);
}

void Database_pluto_main::DeleteTable_Command_CommandParameter()
{
	delete tblCommand_CommandParameter;
}

Table_Command_CommandParameter::~Table_Command_CommandParameter()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Command_CommandParameter *pRow = (Row_Command_CommandParameter *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Command_CommandParameter *pRow = (Row_Command_CommandParameter *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Command_CommandParameter::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Command_CommandParameter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Command_CommandParameter *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Command,pRow->m_FK_CommandParameter);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Command_CommandParameter::Reload()
{
	Row_Command_CommandParameter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Command,pRow->m_FK_CommandParameter);
		Row_Command_CommandParameter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Command_CommandParameter::Row_Command_CommandParameter(Table_Command_CommandParameter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Command_CommandParameter::SetDefaultValues()
{
	m_FK_Command = 0;
is_null[0] = false;
m_FK_CommandParameter = 0;
is_null[1] = false;
is_null[2] = true;
m_IsOut = 0;
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

long int Row_Command_CommandParameter::FK_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command;}
long int Row_Command_CommandParameter::FK_CommandParameter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CommandParameter;}
string Row_Command_CommandParameter::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
short int Row_Command_CommandParameter::IsOut_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsOut;}
long int Row_Command_CommandParameter::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Command_CommandParameter::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Command_CommandParameter::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Command_CommandParameter::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Command_CommandParameter::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Command_CommandParameter::FK_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command = val; is_modified=true; is_null[0]=false;}
void Row_Command_CommandParameter::FK_CommandParameter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CommandParameter = val; is_modified=true; is_null[1]=false;}
void Row_Command_CommandParameter::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_Command_CommandParameter::IsOut_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsOut = val; is_modified=true; is_null[3]=false;}
void Row_Command_CommandParameter::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_Command_CommandParameter::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_Command_CommandParameter::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_Command_CommandParameter::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_Command_CommandParameter::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}

		
bool Row_Command_CommandParameter::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Command_CommandParameter::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Command_CommandParameter::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Command_CommandParameter::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Command_CommandParameter::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Command_CommandParameter::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Command_CommandParameter::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Command_CommandParameter::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Command_CommandParameter::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Command_CommandParameter::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Command_CommandParameter::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_Command_CommandParameter::FK_CommandParameter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandParameter);

return buf;
}

string Row_Command_CommandParameter::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[511];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Command_CommandParameter::IsOut_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsOut);

return buf;
}

string Row_Command_CommandParameter::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Command_CommandParameter::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Command_CommandParameter::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Command_CommandParameter::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Command_CommandParameter::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_Command_CommandParameter::Key::Key(long int in_FK_Command, long int in_FK_CommandParameter)
{
			pk_FK_Command = in_FK_Command;
pk_FK_CommandParameter = in_FK_CommandParameter;
	
}

Table_Command_CommandParameter::Key::Key(Row_Command_CommandParameter *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Command = pRow->m_FK_Command;
pk_FK_CommandParameter = pRow->m_FK_CommandParameter;
	
}		

bool Table_Command_CommandParameter::Key_Less::operator()(const Table_Command_CommandParameter::Key &key1, const Table_Command_CommandParameter::Key &key2) const
{
			if (key1.pk_FK_Command!=key2.pk_FK_Command)
return key1.pk_FK_Command<key2.pk_FK_Command;
else
if (key1.pk_FK_CommandParameter!=key2.pk_FK_CommandParameter)
return key1.pk_FK_CommandParameter<key2.pk_FK_CommandParameter;
else
return false;	
}	

void Table_Command_CommandParameter::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Command_CommandParameter *pRow = (Row_Command_CommandParameter *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Command_asSQL()+", "+pRow->FK_CommandParameter_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->IsOut_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into Command_CommandParameter (FK_Command, FK_CommandParameter, Description, IsOut, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Command,pRow->m_FK_CommandParameter);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Command_CommandParameter* pRow = (Row_Command_CommandParameter*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Command,pRow->m_FK_CommandParameter);

		char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "FK_Command=" + tmp_FK_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Command="+pRow->FK_Command_asSQL()+", FK_CommandParameter="+pRow->FK_CommandParameter_asSQL()+", Description="+pRow->Description_asSQL()+", IsOut="+pRow->IsOut_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update Command_CommandParameter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Command_CommandParameter* pRow = (Row_Command_CommandParameter*) (*i).second;	

		char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "FK_Command=" + tmp_FK_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;

	
		string query = "delete from Command_CommandParameter where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Command_CommandParameter::GetRows(string where_statement,vector<class Row_Command_CommandParameter*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Command_CommandParameter " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Command_CommandParameter where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
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

		Row_Command_CommandParameter *pRow = new Row_Command_CommandParameter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Command));
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
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsOut = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsOut));
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

		DoubleLongKey key(pRow->m_FK_Command,pRow->m_FK_CommandParameter);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Command_CommandParameter *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Command_CommandParameter* Table_Command_CommandParameter::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Command_CommandParameter *pRow = new Row_Command_CommandParameter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Command_CommandParameter* Table_Command_CommandParameter::GetRow(long int in_FK_Command, long int in_FK_CommandParameter)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_Command, in_FK_CommandParameter);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Command_CommandParameter*) (*i).second;
	//we have to fetch row
	Row_Command_CommandParameter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Command_CommandParameter* Table_Command_CommandParameter::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "FK_Command=" + tmp_FK_Command+" AND "+"FK_CommandParameter=" + tmp_FK_CommandParameter;


	string query = "select * from Command_CommandParameter where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
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

	Row_Command_CommandParameter *pRow = new Row_Command_CommandParameter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Command));
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
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsOut = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsOut));
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


class Row_Command* Row_Command_CommandParameter::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}
class Row_CommandParameter* Row_Command_CommandParameter::FK_CommandParameter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandParameter *pTable = table->database->CommandParameter_get();
return pTable->GetRow(m_FK_CommandParameter);
}





