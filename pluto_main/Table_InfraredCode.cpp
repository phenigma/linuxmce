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
#include "Table_InfraredCode.h"
#include "Table_InfraredGroup.h"
#include "Table_Command.h"
#include "Table_DeviceTemplate.h"
#include "Table_Device.h"



void Database_pluto_main::CreateTable_InfraredCode()
{
	tblInfraredCode = new Table_InfraredCode(this);
}

void Database_pluto_main::DeleteTable_InfraredCode()
{
	delete tblInfraredCode;
}

Table_InfraredCode::~Table_InfraredCode()
{
	map<Table_InfraredCode::Key, class Row_InfraredCode*, Table_InfraredCode::Key_Less>::iterator it;
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


void Row_InfraredCode::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_InfraredCode*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_InfraredCode::Key key(this);					
			map<Table_InfraredCode::Key, Row_InfraredCode*, Table_InfraredCode::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_InfraredCode::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_InfraredCode::Key key(this);		
		Row_InfraredCode *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_InfraredCode::Row_InfraredCode(Table_InfraredCode *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_InfraredCode::SetDefaultValues()
{
	m_FK_InfraredGroup = 0;
is_null[0] = false;
m_FK_Command = 0;
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

long int Row_InfraredCode::FK_InfraredGroup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_InfraredGroup;}
long int Row_InfraredCode::FK_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command;}
long int Row_InfraredCode::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_InfraredCode::FK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device;}
string Row_InfraredCode::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_InfraredCode::IRData_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IRData;}
long int Row_InfraredCode::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_InfraredCode::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_InfraredCode::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_InfraredCode::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_InfraredCode::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_InfraredCode::FK_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_InfraredGroup = val; is_modified=true; is_null[0]=false;}
void Row_InfraredCode::FK_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command = val; is_modified=true; is_null[1]=false;}
void Row_InfraredCode::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[2]=false;}
void Row_InfraredCode::FK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device = val; is_modified=true; is_null[3]=false;}
void Row_InfraredCode::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[4]=false;}
void Row_InfraredCode::IRData_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IRData = val; is_modified=true; is_null[5]=false;}
void Row_InfraredCode::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_InfraredCode::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_InfraredCode::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_InfraredCode::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_InfraredCode::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}

		
bool Row_InfraredCode::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_InfraredCode::FK_Device_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_InfraredCode::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_InfraredCode::IRData_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_InfraredCode::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_InfraredCode::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_InfraredCode::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_InfraredCode::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_InfraredCode::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_InfraredCode::FK_Device_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_InfraredCode::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_InfraredCode::IRData_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_InfraredCode::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_InfraredCode::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_InfraredCode::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_InfraredCode::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_InfraredCode::FK_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_InfraredGroup);

return buf;
}

string Row_InfraredCode::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_InfraredCode::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_InfraredCode::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_InfraredCode::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredCode::IRData_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_IRData.c_str(), (unsigned long) m_IRData.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredCode::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_InfraredCode::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_InfraredCode::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_InfraredCode::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_InfraredCode::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_InfraredCode::Key::Key(long int in_FK_InfraredGroup, long int in_FK_Command)
{
			pk_FK_InfraredGroup = in_FK_InfraredGroup;
pk_FK_Command = in_FK_Command;
	
}

Table_InfraredCode::Key::Key(Row_InfraredCode *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_InfraredGroup = pRow->m_FK_InfraredGroup;
pk_FK_Command = pRow->m_FK_Command;
	
}		

bool Table_InfraredCode::Key_Less::operator()(const Table_InfraredCode::Key &key1, const Table_InfraredCode::Key &key2) const
{
			if (key1.pk_FK_InfraredGroup!=key2.pk_FK_InfraredGroup)
return key1.pk_FK_InfraredGroup<key2.pk_FK_InfraredGroup;
else
if (key1.pk_FK_Command!=key2.pk_FK_Command)
return key1.pk_FK_Command<key2.pk_FK_Command;
else
return false;	
}	

void Table_InfraredCode::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_InfraredCode*>::iterator i = addedRows.begin();
	
		Row_InfraredCode *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_InfraredGroup_asSQL()+", "+pRow->FK_Command_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Device_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->IRData_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into InfraredCode (FK_InfraredGroup, FK_Command, FK_DeviceTemplate, FK_Device, Description, IRData, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
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
	

	for (map<Key, Row_InfraredCode*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_InfraredCode* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_InfraredGroup[32];
sprintf(tmp_FK_InfraredGroup, "%li", key.pk_FK_InfraredGroup);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk_FK_Command);


string condition;
condition = condition + "FK_InfraredGroup=" + tmp_FK_InfraredGroup+" AND "+"FK_Command=" + tmp_FK_Command;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_InfraredGroup="+pRow->FK_InfraredGroup_asSQL()+", FK_Command="+pRow->FK_Command_asSQL()+", FK_DeviceTemplate="+pRow->FK_DeviceTemplate_asSQL()+", FK_Device="+pRow->FK_Device_asSQL()+", Description="+pRow->Description_asSQL()+", IRData="+pRow->IRData_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update InfraredCode set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_InfraredCode*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_InfraredCode*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_InfraredGroup[32];
sprintf(tmp_FK_InfraredGroup, "%li", key.pk_FK_InfraredGroup);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk_FK_Command);


string condition;
condition = condition + "FK_InfraredGroup=" + tmp_FK_InfraredGroup+" AND "+"FK_Command=" + tmp_FK_Command;

	
		string query = "delete from InfraredCode where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_InfraredCode::GetRows(string where_statement,vector<class Row_InfraredCode*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from InfraredCode " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from InfraredCode where " + where_statement;
		
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

		Row_InfraredCode *pRow = new Row_InfraredCode(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Command));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Device));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Description = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IRData = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_IRData = string(row[5],lengths[5]);
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

		Key key(pRow);
		
                map<Table_InfraredCode::Key, Row_InfraredCode*, Table_InfraredCode::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_InfraredCode* Table_InfraredCode::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_InfraredCode *pRow = new Row_InfraredCode(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_InfraredCode* Table_InfraredCode::GetRow(long int in_FK_InfraredGroup, long int in_FK_Command)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_InfraredGroup, in_FK_Command);

	map<Key, Row_InfraredCode*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_InfraredCode* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_InfraredCode* Table_InfraredCode::FetchRow(Table_InfraredCode::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_InfraredGroup[32];
sprintf(tmp_FK_InfraredGroup, "%li", key.pk_FK_InfraredGroup);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk_FK_Command);


string condition;
condition = condition + "FK_InfraredGroup=" + tmp_FK_InfraredGroup+" AND "+"FK_Command=" + tmp_FK_Command;


	string query = "select * from InfraredCode where " + condition;		

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

	Row_InfraredCode *pRow = new Row_InfraredCode(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Command));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Device));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Description = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IRData = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_IRData = string(row[5],lengths[5]);
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


class Row_InfraredGroup* Row_InfraredCode::FK_InfraredGroup_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_InfraredGroup *pTable = table->database->InfraredGroup_get();
return pTable->GetRow(m_FK_InfraredGroup);
}
class Row_Command* Row_InfraredCode::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}
class Row_DeviceTemplate* Row_InfraredCode::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_Device* Row_InfraredCode::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}





