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
#include "Table_Output.h"
#include "Table_Command.h"

#include "Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe.h"
#include "Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe.h"
#include "Table_DeviceTemplate_Output.h"
#include "Table_Device_Device_Pipe.h"


void Database_pluto_main::CreateTable_Output()
{
	tblOutput = new Table_Output(this);
}

void Database_pluto_main::DeleteTable_Output()
{
	delete tblOutput;
}

Table_Output::~Table_Output()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Output *pRow = (Row_Output *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Output *pRow = (Row_Output *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Output::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Output *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Output *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Output);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Output::Reload()
{
	Row_Output *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Output);
		Row_Output *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Output::Row_Output(Table_Output *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Output::SetDefaultValues()
{
	m_PK_Output = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
m_Define = "";
is_null[2] = false;
is_null[3] = true;
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

long int Row_Output::PK_Output_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Output;}
string Row_Output::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Output::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_Output::FK_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command;}
long int Row_Output::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Output::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Output::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Output::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Output::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Output::PK_Output_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Output = val; is_modified=true; is_null[0]=false;}
void Row_Output::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Output::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_Output::FK_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command = val; is_modified=true; is_null[3]=false;}
void Row_Output::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_Output::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_Output::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_Output::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_Output::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}

		
bool Row_Output::FK_Command_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Output::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Output::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Output::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Output::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Output::FK_Command_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Output::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Output::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Output::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Output::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Output::PK_Output_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Output);

return buf;
}

string Row_Output::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Output::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Output::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_Output::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Output::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Output::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Output::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Output::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Output::Key::Key(long int in_PK_Output)
{
			pk_PK_Output = in_PK_Output;
	
}

Table_Output::Key::Key(Row_Output *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Output = pRow->m_PK_Output;
	
}		

bool Table_Output::Key_Less::operator()(const Table_Output::Key &key1, const Table_Output::Key &key2) const
{
			if (key1.pk_PK_Output!=key2.pk_PK_Output)
return key1.pk_PK_Output<key2.pk_PK_Output;
else
return false;	
}	

void Table_Output::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Output *pRow = (Row_Output *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Output_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->FK_Command_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into Output (PK_Output, Description, Define, FK_Command, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Output=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Output);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Output* pRow = (Row_Output*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Output);

		char tmp_PK_Output[32];
sprintf(tmp_PK_Output, "%li", key.pk);


string condition;
condition = condition + "PK_Output=" + tmp_PK_Output;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Output="+pRow->PK_Output_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", FK_Command="+pRow->FK_Command_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update Output set " + update_values_list + " where " + condition;
			
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
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Output* pRow = (Row_Output*) (*i).second;	

		char tmp_PK_Output[32];
sprintf(tmp_PK_Output, "%li", key.pk);


string condition;
condition = condition + "PK_Output=" + tmp_PK_Output;

	
		string query = "delete from Output where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Output::GetRows(string where_statement,vector<class Row_Output*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Output " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Output where " + where_statement;
		
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

		Row_Output *pRow = new Row_Output(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Output = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Output));
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
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Command));
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

		SingleLongKey key(pRow->m_PK_Output);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Output *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Output* Table_Output::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Output *pRow = new Row_Output(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Output* Table_Output::GetRow(long int in_PK_Output)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Output);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Output*) (*i).second;
	//we have to fetch row
	Row_Output* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Output* Table_Output::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Output[32];
sprintf(tmp_PK_Output, "%li", key.pk);


string condition;
condition = condition + "PK_Output=" + tmp_PK_Output;


	string query = "select * from Output where " + condition;		

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

	Row_Output *pRow = new Row_Output(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Output = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Output));
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
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Command));
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


class Row_Command* Row_Output::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}


void Row_Output::DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Output_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pTable = table->database->DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get();
pTable->GetRows("FK_Output=" + StringUtils::itos(m_PK_Output),rows);
}
void Row_Output::DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Output_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe *pTable = table->database->DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_get();
pTable->GetRows("FK_Output=" + StringUtils::itos(m_PK_Output),rows);
}
void Row_Output::DeviceTemplate_Output_FK_Output_getrows(vector <class Row_DeviceTemplate_Output*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Output *pTable = table->database->DeviceTemplate_Output_get();
pTable->GetRows("FK_Output=" + StringUtils::itos(m_PK_Output),rows);
}
void Row_Output::Device_Device_Pipe_FK_Output_getrows(vector <class Row_Device_Device_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Pipe *pTable = table->database->Device_Device_Pipe_get();
pTable->GetRows("FK_Output=" + StringUtils::itos(m_PK_Output),rows);
}



