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
#include "Table_DeviceCommandGroup_Command.h"
#include "Table_DeviceCommandGroup.h"
#include "Table_Command.h"



void Database_pluto_main::CreateTable_DeviceCommandGroup_Command()
{
	tblDeviceCommandGroup_Command = new Table_DeviceCommandGroup_Command(this);
}

void Database_pluto_main::DeleteTable_DeviceCommandGroup_Command()
{
	delete tblDeviceCommandGroup_Command;
}

Table_DeviceCommandGroup_Command::~Table_DeviceCommandGroup_Command()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DeviceCommandGroup_Command *pRow = (Row_DeviceCommandGroup_Command *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DeviceCommandGroup_Command *pRow = (Row_DeviceCommandGroup_Command *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DeviceCommandGroup_Command::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_DeviceCommandGroup_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DeviceCommandGroup_Command *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_DeviceCommandGroup,pRow->m_FK_Command);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceCommandGroup_Command::Reload()
{
	Row_DeviceCommandGroup_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_DeviceCommandGroup,pRow->m_FK_Command);
		Row_DeviceCommandGroup_Command *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceCommandGroup_Command::Row_DeviceCommandGroup_Command(Table_DeviceCommandGroup_Command *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceCommandGroup_Command::SetDefaultValues()
{
	m_FK_DeviceCommandGroup = 0;
is_null[0] = false;
m_FK_Command = 0;
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

long int Row_DeviceCommandGroup_Command::FK_DeviceCommandGroup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceCommandGroup;}
long int Row_DeviceCommandGroup_Command::FK_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command;}
string Row_DeviceCommandGroup_Command::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_DeviceCommandGroup_Command::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_DeviceCommandGroup_Command::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_DeviceCommandGroup_Command::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_DeviceCommandGroup_Command::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_DeviceCommandGroup_Command::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_DeviceCommandGroup_Command::FK_DeviceCommandGroup_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceCommandGroup = val; is_modified=true; is_null[0]=false;}
void Row_DeviceCommandGroup_Command::FK_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command = val; is_modified=true; is_null[1]=false;}
void Row_DeviceCommandGroup_Command::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_DeviceCommandGroup_Command::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_DeviceCommandGroup_Command::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_DeviceCommandGroup_Command::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_DeviceCommandGroup_Command::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_DeviceCommandGroup_Command::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}

		
bool Row_DeviceCommandGroup_Command::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_DeviceCommandGroup_Command::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_DeviceCommandGroup_Command::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_DeviceCommandGroup_Command::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_DeviceCommandGroup_Command::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_DeviceCommandGroup_Command::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_DeviceCommandGroup_Command::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_DeviceCommandGroup_Command::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_DeviceCommandGroup_Command::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_DeviceCommandGroup_Command::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_DeviceCommandGroup_Command::FK_DeviceCommandGroup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCommandGroup);

return buf;
}

string Row_DeviceCommandGroup_Command::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_DeviceCommandGroup_Command::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[511];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_DeviceCommandGroup_Command::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceCommandGroup_Command::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceCommandGroup_Command::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceCommandGroup_Command::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceCommandGroup_Command::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_DeviceCommandGroup_Command::Key::Key(long int in_FK_DeviceCommandGroup, long int in_FK_Command)
{
			pk_FK_DeviceCommandGroup = in_FK_DeviceCommandGroup;
pk_FK_Command = in_FK_Command;
	
}

Table_DeviceCommandGroup_Command::Key::Key(Row_DeviceCommandGroup_Command *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_DeviceCommandGroup = pRow->m_FK_DeviceCommandGroup;
pk_FK_Command = pRow->m_FK_Command;
	
}		

bool Table_DeviceCommandGroup_Command::Key_Less::operator()(const Table_DeviceCommandGroup_Command::Key &key1, const Table_DeviceCommandGroup_Command::Key &key2) const
{
			if (key1.pk_FK_DeviceCommandGroup!=key2.pk_FK_DeviceCommandGroup)
return key1.pk_FK_DeviceCommandGroup<key2.pk_FK_DeviceCommandGroup;
else
if (key1.pk_FK_Command!=key2.pk_FK_Command)
return key1.pk_FK_Command<key2.pk_FK_Command;
else
return false;	
}	

bool Table_DeviceCommandGroup_Command::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DeviceCommandGroup_Command *pRow = (Row_DeviceCommandGroup_Command *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DeviceCommandGroup_asSQL()+", "+pRow->FK_Command_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into DeviceCommandGroup_Command (FK_DeviceCommandGroup, FK_Command, Description, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_DeviceCommandGroup,pRow->m_FK_Command);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DeviceCommandGroup_Command* pRow = (Row_DeviceCommandGroup_Command*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_DeviceCommandGroup,pRow->m_FK_Command);

		char tmp_FK_DeviceCommandGroup[32];
sprintf(tmp_FK_DeviceCommandGroup, "%li", key.pk1);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk2);


string condition;
condition = condition + "FK_DeviceCommandGroup=" + tmp_FK_DeviceCommandGroup+" AND "+"FK_Command=" + tmp_FK_Command;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_DeviceCommandGroup="+pRow->FK_DeviceCommandGroup_asSQL()+", FK_Command="+pRow->FK_Command_asSQL()+", Description="+pRow->Description_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update DeviceCommandGroup_Command set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_DeviceCommandGroup_Command *pRow = (Row_DeviceCommandGroup_Command *)(*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DeviceCommandGroup_Command* pRow = (Row_DeviceCommandGroup_Command*) (*i).second;

		char tmp_FK_DeviceCommandGroup[32];
sprintf(tmp_FK_DeviceCommandGroup, "%li", key.pk1);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk2);


string condition;
condition = condition + "FK_DeviceCommandGroup=" + tmp_FK_DeviceCommandGroup+" AND "+"FK_Command=" + tmp_FK_Command;

	
		string query = "delete from DeviceCommandGroup_Command where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_DeviceCommandGroup_Command::GetRows(string where_statement,vector<class Row_DeviceCommandGroup_Command*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from DeviceCommandGroup_Command " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from DeviceCommandGroup_Command where " + where_statement;
		
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

		Row_DeviceCommandGroup_Command *pRow = new Row_DeviceCommandGroup_Command(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceCommandGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceCommandGroup));
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

		DoubleLongKey key(pRow->m_FK_DeviceCommandGroup,pRow->m_FK_Command);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DeviceCommandGroup_Command *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_DeviceCommandGroup_Command* Table_DeviceCommandGroup_Command::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceCommandGroup_Command *pRow = new Row_DeviceCommandGroup_Command(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceCommandGroup_Command* Table_DeviceCommandGroup_Command::GetRow(long int in_FK_DeviceCommandGroup, long int in_FK_Command)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_DeviceCommandGroup, in_FK_Command);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DeviceCommandGroup_Command*) (*i).second;
	//we have to fetch row
	Row_DeviceCommandGroup_Command* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceCommandGroup_Command* Table_DeviceCommandGroup_Command::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_DeviceCommandGroup[32];
sprintf(tmp_FK_DeviceCommandGroup, "%li", key.pk1);

char tmp_FK_Command[32];
sprintf(tmp_FK_Command, "%li", key.pk2);


string condition;
condition = condition + "FK_DeviceCommandGroup=" + tmp_FK_DeviceCommandGroup+" AND "+"FK_Command=" + tmp_FK_Command;


	string query = "select * from DeviceCommandGroup_Command where " + condition;		

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

	Row_DeviceCommandGroup_Command *pRow = new Row_DeviceCommandGroup_Command(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceCommandGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceCommandGroup));
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


class Row_DeviceCommandGroup* Row_DeviceCommandGroup_Command::FK_DeviceCommandGroup_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceCommandGroup *pTable = table->database->DeviceCommandGroup_get();
return pTable->GetRow(m_FK_DeviceCommandGroup);
}
class Row_Command* Row_DeviceCommandGroup_Command::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}





