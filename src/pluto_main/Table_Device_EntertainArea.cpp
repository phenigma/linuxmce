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
#include "Table_Device_EntertainArea.h"
#include "Table_Device.h"
#include "Table_EntertainArea.h"



void Database_pluto_main::CreateTable_Device_EntertainArea()
{
	tblDevice_EntertainArea = new Table_Device_EntertainArea(this);
}

void Database_pluto_main::DeleteTable_Device_EntertainArea()
{
	delete tblDevice_EntertainArea;
}

Table_Device_EntertainArea::~Table_Device_EntertainArea()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Device_EntertainArea *pRow = (Row_Device_EntertainArea *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Device_EntertainArea *pRow = (Row_Device_EntertainArea *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Device_EntertainArea::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Device_EntertainArea *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Device_EntertainArea *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_EntertainArea);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device_EntertainArea::Reload()
{
	Row_Device_EntertainArea *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_EntertainArea);
		Row_Device_EntertainArea *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Device_EntertainArea::Row_Device_EntertainArea(Table_Device_EntertainArea *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Device_EntertainArea::SetDefaultValues()
{
	m_FK_Device = 0;
is_null[0] = false;
m_FK_EntertainArea = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_psc_frozen = 0;
is_null[5] = false;
m_psc_mod = "00000000000000";
is_null[6] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Device_EntertainArea::FK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device;}
long int Row_Device_EntertainArea::FK_EntertainArea_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_EntertainArea;}
long int Row_Device_EntertainArea::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Device_EntertainArea::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Device_EntertainArea::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Device_EntertainArea::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Device_EntertainArea::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Device_EntertainArea::FK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device = val; is_modified=true; is_null[0]=false;}
void Row_Device_EntertainArea::FK_EntertainArea_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_EntertainArea = val; is_modified=true; is_null[1]=false;}
void Row_Device_EntertainArea::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[2]=false;}
void Row_Device_EntertainArea::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[3]=false;}
void Row_Device_EntertainArea::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[4]=false;}
void Row_Device_EntertainArea::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[5]=false;}
void Row_Device_EntertainArea::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[6]=false;}

		
bool Row_Device_EntertainArea::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Device_EntertainArea::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Device_EntertainArea::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Device_EntertainArea::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_Device_EntertainArea::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Device_EntertainArea::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Device_EntertainArea::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Device_EntertainArea::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_Device_EntertainArea::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_Device_EntertainArea::FK_EntertainArea_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_EntertainArea);

return buf;
}

string Row_Device_EntertainArea::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Device_EntertainArea::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Device_EntertainArea::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Device_EntertainArea::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Device_EntertainArea::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Device_EntertainArea::Key::Key(long int in_FK_Device, long int in_FK_EntertainArea)
{
			pk_FK_Device = in_FK_Device;
pk_FK_EntertainArea = in_FK_EntertainArea;
	
}

Table_Device_EntertainArea::Key::Key(Row_Device_EntertainArea *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Device = pRow->m_FK_Device;
pk_FK_EntertainArea = pRow->m_FK_EntertainArea;
	
}		

bool Table_Device_EntertainArea::Key_Less::operator()(const Table_Device_EntertainArea::Key &key1, const Table_Device_EntertainArea::Key &key2) const
{
			if (key1.pk_FK_Device!=key2.pk_FK_Device)
return key1.pk_FK_Device<key2.pk_FK_Device;
else
if (key1.pk_FK_EntertainArea!=key2.pk_FK_EntertainArea)
return key1.pk_FK_EntertainArea<key2.pk_FK_EntertainArea;
else
return false;	
}	

bool Table_Device_EntertainArea::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Device_EntertainArea *pRow = (Row_Device_EntertainArea *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Device_asSQL()+", "+pRow->FK_EntertainArea_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Device_EntertainArea (`FK_Device`, `FK_EntertainArea`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
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
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_EntertainArea);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Device_EntertainArea* pRow = (Row_Device_EntertainArea*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_EntertainArea);

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_EntertainArea[32];
sprintf(tmp_FK_EntertainArea, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_EntertainArea`=" + tmp_FK_EntertainArea;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_Device`="+pRow->FK_Device_asSQL()+", `FK_EntertainArea`="+pRow->FK_EntertainArea_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Device_EntertainArea set " + update_values_list + " where " + condition;
			
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
		Row_Device_EntertainArea* pRow = (Row_Device_EntertainArea*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Device_EntertainArea* pRow = (Row_Device_EntertainArea*) (*i).second;	

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_EntertainArea[32];
sprintf(tmp_FK_EntertainArea, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_EntertainArea`=" + tmp_FK_EntertainArea;

	
		string query = "delete from Device_EntertainArea where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_Device_EntertainArea*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Device_EntertainArea::GetRows(string where_statement,vector<class Row_Device_EntertainArea*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Device_EntertainArea " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Device_EntertainArea where " + where_statement;
		
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

		Row_Device_EntertainArea *pRow = new Row_Device_EntertainArea(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_EntertainArea = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_EntertainArea));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_EntertainArea);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Device_EntertainArea *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Device_EntertainArea* Table_Device_EntertainArea::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Device_EntertainArea *pRow = new Row_Device_EntertainArea(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Device_EntertainArea* Table_Device_EntertainArea::GetRow(long int in_FK_Device, long int in_FK_EntertainArea)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_Device, in_FK_EntertainArea);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Device_EntertainArea*) (*i).second;
	//we have to fetch row
	Row_Device_EntertainArea* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device_EntertainArea* Table_Device_EntertainArea::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_EntertainArea[32];
sprintf(tmp_FK_EntertainArea, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_EntertainArea`=" + tmp_FK_EntertainArea;


	string query = "select * from Device_EntertainArea where " + condition;		

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

	Row_Device_EntertainArea *pRow = new Row_Device_EntertainArea(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_EntertainArea = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_EntertainArea));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Device* Row_Device_EntertainArea::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}
class Row_EntertainArea* Row_Device_EntertainArea::FK_EntertainArea_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EntertainArea *pTable = table->database->EntertainArea_get();
return pTable->GetRow(m_FK_EntertainArea);
}





