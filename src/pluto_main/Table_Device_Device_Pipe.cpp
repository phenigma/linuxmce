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
#include "Table_Device_Device_Pipe.h"
#include "Table_Device.h"
#include "Table_Device.h"
#include "Table_Pipe.h"
#include "Table_Command.h"
#include "Table_Command.h"



void Database_pluto_main::CreateTable_Device_Device_Pipe()
{
	tblDevice_Device_Pipe = new Table_Device_Device_Pipe(this);
}

void Database_pluto_main::DeleteTable_Device_Device_Pipe()
{
	delete tblDevice_Device_Pipe;
}

Table_Device_Device_Pipe::~Table_Device_Device_Pipe()
{
	map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Device_Device_Pipe *pRow = (Row_Device_Device_Pipe *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Device_Device_Pipe *pRow = (Row_Device_Device_Pipe *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Device_Device_Pipe::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Device_Device_Pipe *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Device_Device_Pipe *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			TripleLongKey key(pRow->m_FK_Device_From,pRow->m_FK_Device_To,pRow->m_FK_Pipe);
			map<TripleLongKey, TableRow*, TripleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device_Device_Pipe::Reload()
{
	Row_Device_Device_Pipe *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		TripleLongKey key(pRow->m_FK_Device_From,pRow->m_FK_Device_To,pRow->m_FK_Pipe);
		Row_Device_Device_Pipe *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Device_Device_Pipe::Row_Device_Device_Pipe(Table_Device_Device_Pipe *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Device_Device_Pipe::SetDefaultValues()
{
	m_FK_Device_From = 0;
is_null[0] = false;
m_FK_Device_To = 0;
is_null[1] = false;
m_FK_Pipe = 0;
is_null[2] = false;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_psc_frozen = 0;
is_null[8] = false;
m_psc_mod = "00000000000000";
is_null[9] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Device_Device_Pipe::FK_Device_From_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_From;}
long int Row_Device_Device_Pipe::FK_Device_To_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_To;}
long int Row_Device_Device_Pipe::FK_Pipe_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Pipe;}
long int Row_Device_Device_Pipe::FK_Command_Input_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command_Input;}
long int Row_Device_Device_Pipe::FK_Command_Output_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command_Output;}
long int Row_Device_Device_Pipe::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Device_Device_Pipe::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Device_Device_Pipe::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Device_Device_Pipe::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Device_Device_Pipe::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Device_Device_Pipe::FK_Device_From_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_From = val; is_modified=true; is_null[0]=false;}
void Row_Device_Device_Pipe::FK_Device_To_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_To = val; is_modified=true; is_null[1]=false;}
void Row_Device_Device_Pipe::FK_Pipe_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Pipe = val; is_modified=true; is_null[2]=false;}
void Row_Device_Device_Pipe::FK_Command_Input_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command_Input = val; is_modified=true; is_null[3]=false;}
void Row_Device_Device_Pipe::FK_Command_Output_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command_Output = val; is_modified=true; is_null[4]=false;}
void Row_Device_Device_Pipe::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[5]=false;}
void Row_Device_Device_Pipe::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[6]=false;}
void Row_Device_Device_Pipe::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[7]=false;}
void Row_Device_Device_Pipe::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[8]=false;}
void Row_Device_Device_Pipe::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[9]=false;}

		
bool Row_Device_Device_Pipe::FK_Command_Input_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Device_Device_Pipe::FK_Command_Output_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Device_Device_Pipe::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Device_Device_Pipe::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Device_Device_Pipe::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Device_Device_Pipe::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_Device_Device_Pipe::FK_Command_Input_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Device_Device_Pipe::FK_Command_Output_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Device_Device_Pipe::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Device_Device_Pipe::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Device_Device_Pipe::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Device_Device_Pipe::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_Device_Device_Pipe::FK_Device_From_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_From);

return buf;
}

string Row_Device_Device_Pipe::FK_Device_To_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_To);

return buf;
}

string Row_Device_Device_Pipe::FK_Pipe_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Pipe);

return buf;
}

string Row_Device_Device_Pipe::FK_Command_Input_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command_Input);

return buf;
}

string Row_Device_Device_Pipe::FK_Command_Output_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command_Output);

return buf;
}

string Row_Device_Device_Pipe::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Device_Device_Pipe::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Device_Device_Pipe::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Device_Device_Pipe::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Device_Device_Pipe::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Device_Device_Pipe::Key::Key(long int in_FK_Device_From, long int in_FK_Device_To, long int in_FK_Pipe)
{
			pk_FK_Device_From = in_FK_Device_From;
pk_FK_Device_To = in_FK_Device_To;
pk_FK_Pipe = in_FK_Pipe;
	
}

Table_Device_Device_Pipe::Key::Key(Row_Device_Device_Pipe *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Device_From = pRow->m_FK_Device_From;
pk_FK_Device_To = pRow->m_FK_Device_To;
pk_FK_Pipe = pRow->m_FK_Pipe;
	
}		

bool Table_Device_Device_Pipe::Key_Less::operator()(const Table_Device_Device_Pipe::Key &key1, const Table_Device_Device_Pipe::Key &key2) const
{
			if (key1.pk_FK_Device_From!=key2.pk_FK_Device_From)
return key1.pk_FK_Device_From<key2.pk_FK_Device_From;
else
if (key1.pk_FK_Device_To!=key2.pk_FK_Device_To)
return key1.pk_FK_Device_To<key2.pk_FK_Device_To;
else
if (key1.pk_FK_Pipe!=key2.pk_FK_Pipe)
return key1.pk_FK_Pipe<key2.pk_FK_Pipe;
else
return false;	
}	

bool Table_Device_Device_Pipe::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Device_Device_Pipe *pRow = (Row_Device_Device_Pipe *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Device_From_asSQL()+", "+pRow->FK_Device_To_asSQL()+", "+pRow->FK_Pipe_asSQL()+", "+pRow->FK_Command_Input_asSQL()+", "+pRow->FK_Command_Output_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Device_Device_Pipe (`FK_Device_From`, `FK_Device_To`, `FK_Pipe`, `FK_Command_Input`, `FK_Command_Output`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
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
		
				
			
			addedRows.erase(i);
			TripleLongKey key(pRow->m_FK_Device_From,pRow->m_FK_Device_To,pRow->m_FK_Pipe);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Device_Device_Pipe* pRow = (Row_Device_Device_Pipe*) (*i).second;	
		TripleLongKey key(pRow->m_FK_Device_From,pRow->m_FK_Device_To,pRow->m_FK_Pipe);

		char tmp_FK_Device_From[32];
sprintf(tmp_FK_Device_From, "%li", key.pk1);

char tmp_FK_Device_To[32];
sprintf(tmp_FK_Device_To, "%li", key.pk2);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk3);


string condition;
condition = condition + "`FK_Device_From`=" + tmp_FK_Device_From+" AND "+"`FK_Device_To`=" + tmp_FK_Device_To+" AND "+"`FK_Pipe`=" + tmp_FK_Pipe;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_Device_From`="+pRow->FK_Device_From_asSQL()+", `FK_Device_To`="+pRow->FK_Device_To_asSQL()+", `FK_Pipe`="+pRow->FK_Pipe_asSQL()+", `FK_Command_Input`="+pRow->FK_Command_Input_asSQL()+", `FK_Command_Output`="+pRow->FK_Command_Output_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Device_Device_Pipe set " + update_values_list + " where " + condition;
			
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
		Row_Device_Device_Pipe* pRow = (Row_Device_Device_Pipe*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		TripleLongKey key = (*i).first;
		Row_Device_Device_Pipe* pRow = (Row_Device_Device_Pipe*) (*i).second;	

		char tmp_FK_Device_From[32];
sprintf(tmp_FK_Device_From, "%li", key.pk1);

char tmp_FK_Device_To[32];
sprintf(tmp_FK_Device_To, "%li", key.pk2);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk3);


string condition;
condition = condition + "`FK_Device_From`=" + tmp_FK_Device_From+" AND "+"`FK_Device_To`=" + tmp_FK_Device_To+" AND "+"`FK_Pipe`=" + tmp_FK_Pipe;

	
		string query = "delete from Device_Device_Pipe where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_Device_Device_Pipe*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Device_Device_Pipe::GetRows(string where_statement,vector<class Row_Device_Device_Pipe*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Device_Device_Pipe`.* from Device_Device_Pipe " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Device_Device_Pipe`.* from Device_Device_Pipe where " + where_statement;
		
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

		Row_Device_Device_Pipe *pRow = new Row_Device_Device_Pipe(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device_From = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device_From));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Device_To = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Device_To));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Pipe = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Pipe));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Command_Input = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Command_Input));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Command_Output = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Command_Output));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
}



		//checking for duplicates

		TripleLongKey key(pRow->m_FK_Device_From,pRow->m_FK_Device_To,pRow->m_FK_Pipe);
		
		map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Device_Device_Pipe *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Device_Device_Pipe* Table_Device_Device_Pipe::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Device_Device_Pipe *pRow = new Row_Device_Device_Pipe(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Device_Device_Pipe* Table_Device_Device_Pipe::GetRow(long int in_FK_Device_From, long int in_FK_Device_To, long int in_FK_Pipe)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	TripleLongKey row_key(in_FK_Device_From, in_FK_Device_To, in_FK_Pipe);

	map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Device_Device_Pipe*) (*i).second;
	//we have to fetch row
	Row_Device_Device_Pipe* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device_Device_Pipe* Table_Device_Device_Pipe::FetchRow(TripleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Device_From[32];
sprintf(tmp_FK_Device_From, "%li", key.pk1);

char tmp_FK_Device_To[32];
sprintf(tmp_FK_Device_To, "%li", key.pk2);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk3);


string condition;
condition = condition + "`FK_Device_From`=" + tmp_FK_Device_From+" AND "+"`FK_Device_To`=" + tmp_FK_Device_To+" AND "+"`FK_Pipe`=" + tmp_FK_Pipe;


	string query = "select * from Device_Device_Pipe where " + condition;		

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

	Row_Device_Device_Pipe *pRow = new Row_Device_Device_Pipe(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device_From = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device_From));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Device_To = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Device_To));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Pipe = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Pipe));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Command_Input = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Command_Input));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Command_Output = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Command_Output));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Device* Row_Device_Device_Pipe::FK_Device_From_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_From);
}
class Row_Device* Row_Device_Device_Pipe::FK_Device_To_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_To);
}
class Row_Pipe* Row_Device_Device_Pipe::FK_Pipe_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Pipe *pTable = table->database->Pipe_get();
return pTable->GetRow(m_FK_Pipe);
}
class Row_Command* Row_Device_Device_Pipe::FK_Command_Input_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command_Input);
}
class Row_Command* Row_Device_Device_Pipe::FK_Command_Output_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command_Output);
}





