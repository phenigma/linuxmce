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
#include "Table_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "Table_DeviceTemplate.h"
#include "Table_DeviceCategory.h"
#include "Table_Input.h"
#include "Table_Input.h"

#include "Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe.h"


void Database_pluto_main::CreateTable_DeviceTemplate_DeviceCategory_ControlledVia()
{
	tblDeviceTemplate_DeviceCategory_ControlledVia = new Table_DeviceTemplate_DeviceCategory_ControlledVia(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia()
{
	delete tblDeviceTemplate_DeviceCategory_ControlledVia;
}

Table_DeviceTemplate_DeviceCategory_ControlledVia::~Table_DeviceTemplate_DeviceCategory_ControlledVia()
{
	map<Table_DeviceTemplate_DeviceCategory_ControlledVia::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia*, Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less>::iterator it;
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


void Row_DeviceTemplate_DeviceCategory_ControlledVia::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DeviceTemplate_DeviceCategory_ControlledVia*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DeviceTemplate_DeviceCategory_ControlledVia::Key key(this);					
			map<Table_DeviceTemplate_DeviceCategory_ControlledVia::Key, Row_DeviceTemplate_DeviceCategory_ControlledVia*, Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate_DeviceCategory_ControlledVia::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DeviceTemplate_DeviceCategory_ControlledVia::Key key(this);		
		Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_DeviceCategory_ControlledVia::Row_DeviceTemplate_DeviceCategory_ControlledVia(Table_DeviceTemplate_DeviceCategory_ControlledVia *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_DeviceCategory_ControlledVia::SetDefaultValues()
{
	m_PK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
is_null[0] = false;
m_FK_DeviceTemplate = 0;
is_null[1] = false;
m_FK_DeviceCategory = 0;
is_null[2] = false;
is_null[3] = true;
m_RerouteMessagesToParent = 0;
is_null[4] = false;
m_AutoCreateChildren = 0;
is_null[5] = false;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
m_psc_frozen = 0;
is_null[11] = false;
m_psc_mod = "00000000000000";
is_null[12] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate_DeviceCategory_ControlledVia::PK_DeviceTemplate_DeviceCategory_ControlledVia_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_DeviceTemplate_DeviceCategory_ControlledVia;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceCategory;}
string Row_DeviceTemplate_DeviceCategory_ControlledVia::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia::RerouteMessagesToParent_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_RerouteMessagesToParent;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia::AutoCreateChildren_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_AutoCreateChildren;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Input_Audio;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Input_Video;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_DeviceTemplate_DeviceCategory_ControlledVia::PK_DeviceTemplate_DeviceCategory_ControlledVia_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_DeviceTemplate_DeviceCategory_ControlledVia = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceCategory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::RerouteMessagesToParent_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_RerouteMessagesToParent = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::AutoCreateChildren_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_AutoCreateChildren = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Input_Audio = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Input_Video = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[11]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[12]=false;}

		
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_DeviceTemplate_DeviceCategory_ControlledVia::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_DeviceTemplate_DeviceCategory_ControlledVia::PK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DeviceTemplate_DeviceCategory_ControlledVia);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCategory);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::RerouteMessagesToParent_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RerouteMessagesToParent);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::AutoCreateChildren_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AutoCreateChildren);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Input_Audio);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Input_Video);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_DeviceTemplate_DeviceCategory_ControlledVia::Key::Key(long int in_PK_DeviceTemplate_DeviceCategory_ControlledVia)
{
			pk_PK_DeviceTemplate_DeviceCategory_ControlledVia = in_PK_DeviceTemplate_DeviceCategory_ControlledVia;
	
}

Table_DeviceTemplate_DeviceCategory_ControlledVia::Key::Key(Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_DeviceTemplate_DeviceCategory_ControlledVia = pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia;
	
}		

bool Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less::operator()(const Table_DeviceTemplate_DeviceCategory_ControlledVia::Key &key1, const Table_DeviceTemplate_DeviceCategory_ControlledVia::Key &key2) const
{
			if (key1.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia!=key2.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia)
return key1.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia<key2.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia;
else
return false;	
}	

void Table_DeviceTemplate_DeviceCategory_ControlledVia::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DeviceTemplate_DeviceCategory_ControlledVia*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->RerouteMessagesToParent_asSQL()+", "+pRow->AutoCreateChildren_asSQL()+", "+pRow->FK_Input_Audio_asSQL()+", "+pRow->FK_Input_Video_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into DeviceTemplate_DeviceCategory_ControlledVia (PK_DeviceTemplate_DeviceCategory_ControlledVia, FK_DeviceTemplate, FK_DeviceCategory, Description, RerouteMessagesToParent, AutoCreateChildren, FK_Input_Audio, FK_Input_Video, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DeviceTemplate_DeviceCategory_ControlledVia* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia);


string condition;
condition = condition + "PK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_DeviceTemplate_DeviceCategory_ControlledVia="+pRow->PK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()+", FK_DeviceTemplate="+pRow->FK_DeviceTemplate_asSQL()+", FK_DeviceCategory="+pRow->FK_DeviceCategory_asSQL()+", Description="+pRow->Description_asSQL()+", RerouteMessagesToParent="+pRow->RerouteMessagesToParent_asSQL()+", AutoCreateChildren="+pRow->AutoCreateChildren_asSQL()+", FK_Input_Audio="+pRow->FK_Input_Audio_asSQL()+", FK_Input_Video="+pRow->FK_Input_Video_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update DeviceTemplate_DeviceCategory_ControlledVia set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DeviceTemplate_DeviceCategory_ControlledVia*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia);


string condition;
condition = condition + "PK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia;

	
		string query = "delete from DeviceTemplate_DeviceCategory_ControlledVia where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DeviceTemplate_DeviceCategory_ControlledVia::GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DeviceTemplate_DeviceCategory_ControlledVia where " + where_statement;
		
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

		Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_RerouteMessagesToParent = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_RerouteMessagesToParent));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AutoCreateChildren = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AutoCreateChildren));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Input_Audio = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Input_Audio));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Input_Video = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Input_Video));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_DeviceTemplate_DeviceCategory_ControlledVia::Key, Row_DeviceTemplate_DeviceCategory_ControlledVia*, Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DeviceTemplate_DeviceCategory_ControlledVia* Table_DeviceTemplate_DeviceCategory_ControlledVia::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_DeviceCategory_ControlledVia* Table_DeviceTemplate_DeviceCategory_ControlledVia::GetRow(long int in_PK_DeviceTemplate_DeviceCategory_ControlledVia)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_DeviceTemplate_DeviceCategory_ControlledVia);

	map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_DeviceCategory_ControlledVia* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_DeviceCategory_ControlledVia* Table_DeviceTemplate_DeviceCategory_ControlledVia::FetchRow(Table_DeviceTemplate_DeviceCategory_ControlledVia::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_PK_DeviceTemplate_DeviceCategory_ControlledVia);


string condition;
condition = condition + "PK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_PK_DeviceTemplate_DeviceCategory_ControlledVia;


	string query = "select * from DeviceTemplate_DeviceCategory_ControlledVia where " + condition;		

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

	Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate_DeviceCategory_ControlledVia));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_RerouteMessagesToParent = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_RerouteMessagesToParent));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AutoCreateChildren = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AutoCreateChildren));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Input_Audio = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Input_Audio));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Input_Video = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Input_Video));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DeviceTemplate* Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_DeviceCategory* Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}
class Row_Input* Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Audio_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Input *pTable = table->database->Input_get();
return pTable->GetRow(m_FK_Input_Audio);
}
class Row_Input* Row_DeviceTemplate_DeviceCategory_ControlledVia::FK_Input_Video_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Input *pTable = table->database->Input_get();
return pTable->GetRow(m_FK_Input_Video);
}


void Row_DeviceTemplate_DeviceCategory_ControlledVia::DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_DeviceTemplate_DeviceCategory_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pTable = table->database->DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get();
pTable->GetRows("FK_DeviceTemplate_DeviceCategory_ControlledVia=" + StringUtils::itos(m_PK_DeviceTemplate_DeviceCategory_ControlledVia),rows);
}



