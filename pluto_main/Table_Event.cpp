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
#include "Table_Event.h"
#include "Table_EventCategory.h"

#include "Table_CannedEvents.h"
#include "Table_DeviceCategory_Event.h"
#include "Table_DeviceTemplate_Event.h"
#include "Table_Device_HouseMode.h"
#include "Table_EventHandler.h"
#include "Table_Event_EventParameter.h"


void Database_pluto_main::CreateTable_Event()
{
	tblEvent = new Table_Event(this);
}

void Database_pluto_main::DeleteTable_Event()
{
	delete tblEvent;
}

Table_Event::~Table_Event()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Event *pRow = (Row_Event *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Event *pRow = (Row_Event *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Event::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Event *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Event *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Event);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Event::Reload()
{
	Row_Event *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Event);
		Row_Event *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Event::Row_Event(Table_Event *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Event::SetDefaultValues()
{
	m_PK_Event = 0;
is_null[0] = false;
m_FK_EventCategory = 0;
is_null[1] = false;
is_null[2] = true;
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

long int Row_Event::PK_Event_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Event;}
long int Row_Event::FK_EventCategory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_EventCategory;}
string Row_Event::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Event::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_Event::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Event::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Event::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Event::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Event::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Event::PK_Event_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Event = val; is_modified=true; is_null[0]=false;}
void Row_Event::FK_EventCategory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_EventCategory = val; is_modified=true; is_null[1]=false;}
void Row_Event::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_Event::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[3]=false;}
void Row_Event::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_Event::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_Event::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_Event::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_Event::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}

		
bool Row_Event::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Event::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Event::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Event::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Event::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Event::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Event::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Event::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Event::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Event::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Event::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Event::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Event::PK_Event_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Event);

return buf;
}

string Row_Event::FK_EventCategory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_EventCategory);

return buf;
}

string Row_Event::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Event::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Event::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Event::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Event::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Event::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Event::psc_mod_asSQL()
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




Table_Event::Key::Key(long int in_PK_Event)
{
			pk_PK_Event = in_PK_Event;
	
}

Table_Event::Key::Key(Row_Event *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Event = pRow->m_PK_Event;
	
}		

bool Table_Event::Key_Less::operator()(const Table_Event::Key &key1, const Table_Event::Key &key2) const
{
			if (key1.pk_PK_Event!=key2.pk_PK_Event)
return key1.pk_PK_Event<key2.pk_PK_Event;
else
return false;	
}	

void Table_Event::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Event *pRow = (Row_Event *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Event_asSQL()+", "+pRow->FK_EventCategory_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into Event (PK_Event, FK_EventCategory, Description, Define, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Event=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Event);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Event* pRow = (Row_Event*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Event);

		char tmp_PK_Event[32];
sprintf(tmp_PK_Event, "%li", key.pk);


string condition;
condition = condition + "PK_Event=" + tmp_PK_Event;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Event="+pRow->PK_Event_asSQL()+", FK_EventCategory="+pRow->FK_EventCategory_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update Event set " + update_values_list + " where " + condition;
			
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
		Row_Event* pRow = (Row_Event*) (*i).second;	

		char tmp_PK_Event[32];
sprintf(tmp_PK_Event, "%li", key.pk);


string condition;
condition = condition + "PK_Event=" + tmp_PK_Event;

	
		string query = "delete from Event where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Event::GetRows(string where_statement,vector<class Row_Event*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Event " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Event where " + where_statement;
		
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

		Row_Event *pRow = new Row_Event(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Event = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Event));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_EventCategory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_EventCategory));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
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

		SingleLongKey key(pRow->m_PK_Event);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Event *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Event* Table_Event::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Event *pRow = new Row_Event(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Event* Table_Event::GetRow(long int in_PK_Event)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Event);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Event*) (*i).second;
	//we have to fetch row
	Row_Event* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Event* Table_Event::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Event[32];
sprintf(tmp_PK_Event, "%li", key.pk);


string condition;
condition = condition + "PK_Event=" + tmp_PK_Event;


	string query = "select * from Event where " + condition;		

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

	Row_Event *pRow = new Row_Event(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Event = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Event));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_EventCategory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_EventCategory));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
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


class Row_EventCategory* Row_Event::FK_EventCategory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EventCategory *pTable = table->database->EventCategory_get();
return pTable->GetRow(m_FK_EventCategory);
}


void Row_Event::CannedEvents_FK_Event_getrows(vector <class Row_CannedEvents*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CannedEvents *pTable = table->database->CannedEvents_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}
void Row_Event::DeviceCategory_Event_FK_Event_getrows(vector <class Row_DeviceCategory_Event*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceCategory_Event *pTable = table->database->DeviceCategory_Event_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}
void Row_Event::DeviceTemplate_Event_FK_Event_getrows(vector <class Row_DeviceTemplate_Event*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Event *pTable = table->database->DeviceTemplate_Event_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}
void Row_Event::Device_HouseMode_FK_Event_getrows(vector <class Row_Device_HouseMode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_HouseMode *pTable = table->database->Device_HouseMode_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}
void Row_Event::EventHandler_FK_Event_getrows(vector <class Row_EventHandler*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EventHandler *pTable = table->database->EventHandler_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}
void Row_Event::Event_EventParameter_FK_Event_getrows(vector <class Row_Event_EventParameter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Event_EventParameter *pTable = table->database->Event_EventParameter_get();
pTable->GetRows("FK_Event=" + StringUtils::itos(m_PK_Event),rows);
}



