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
#include "Table_EntertainArea.h"
#include "Table_Room.h"

#include "Table_CommandGroup_EntertainArea.h"
#include "Table_Device_EntertainArea.h"
#include "Table_Orbiter.h"


void Database_pluto_main::CreateTable_EntertainArea()
{
	tblEntertainArea = new Table_EntertainArea(this);
}

void Database_pluto_main::DeleteTable_EntertainArea()
{
	delete tblEntertainArea;
}

Table_EntertainArea::~Table_EntertainArea()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_EntertainArea *pRow = (Row_EntertainArea *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_EntertainArea *pRow = (Row_EntertainArea *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_EntertainArea::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_EntertainArea *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_EntertainArea *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_EntertainArea);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_EntertainArea::Reload()
{
	Row_EntertainArea *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_EntertainArea);
		Row_EntertainArea *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_EntertainArea::Row_EntertainArea(Table_EntertainArea *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_EntertainArea::SetDefaultValues()
{
	m_PK_EntertainArea = 0;
is_null[0] = false;
m_FK_Room = 0;
is_null[1] = false;
m_Only1Stream = 0;
is_null[2] = false;
m_Description = "";
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

long int Row_EntertainArea::PK_EntertainArea_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_EntertainArea;}
long int Row_EntertainArea::FK_Room_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Room;}
short int Row_EntertainArea::Only1Stream_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Only1Stream;}
string Row_EntertainArea::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_EntertainArea::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_EntertainArea::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_EntertainArea::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_EntertainArea::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_EntertainArea::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_EntertainArea::PK_EntertainArea_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_EntertainArea = val; is_modified=true; is_null[0]=false;}
void Row_EntertainArea::FK_Room_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Room = val; is_modified=true; is_null[1]=false;}
void Row_EntertainArea::Only1Stream_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Only1Stream = val; is_modified=true; is_null[2]=false;}
void Row_EntertainArea::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_EntertainArea::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_EntertainArea::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_EntertainArea::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_EntertainArea::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_EntertainArea::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}

		
bool Row_EntertainArea::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_EntertainArea::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_EntertainArea::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_EntertainArea::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_EntertainArea::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_EntertainArea::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_EntertainArea::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_EntertainArea::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_EntertainArea::PK_EntertainArea_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_EntertainArea);

return buf;
}

string Row_EntertainArea::FK_Room_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Room);

return buf;
}

string Row_EntertainArea::Only1Stream_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Only1Stream);

return buf;
}

string Row_EntertainArea::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_EntertainArea::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_EntertainArea::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_EntertainArea::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_EntertainArea::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_EntertainArea::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_EntertainArea::Key::Key(long int in_PK_EntertainArea)
{
			pk_PK_EntertainArea = in_PK_EntertainArea;
	
}

Table_EntertainArea::Key::Key(Row_EntertainArea *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_EntertainArea = pRow->m_PK_EntertainArea;
	
}		

bool Table_EntertainArea::Key_Less::operator()(const Table_EntertainArea::Key &key1, const Table_EntertainArea::Key &key2) const
{
			if (key1.pk_PK_EntertainArea!=key2.pk_PK_EntertainArea)
return key1.pk_PK_EntertainArea<key2.pk_PK_EntertainArea;
else
return false;	
}	

void Table_EntertainArea::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_EntertainArea *pRow = (Row_EntertainArea *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_EntertainArea_asSQL()+", "+pRow->FK_Room_asSQL()+", "+pRow->Only1Stream_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into EntertainArea (PK_EntertainArea, FK_Room, Only1Stream, Description, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_EntertainArea=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_EntertainArea);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_EntertainArea* pRow = (Row_EntertainArea*) (*i).second;	
		SingleLongKey key(pRow->m_PK_EntertainArea);

		char tmp_PK_EntertainArea[32];
sprintf(tmp_PK_EntertainArea, "%li", key.pk);


string condition;
condition = condition + "PK_EntertainArea=" + tmp_PK_EntertainArea;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_EntertainArea="+pRow->PK_EntertainArea_asSQL()+", FK_Room="+pRow->FK_Room_asSQL()+", Only1Stream="+pRow->Only1Stream_asSQL()+", Description="+pRow->Description_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update EntertainArea set " + update_values_list + " where " + condition;
			
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
		Row_EntertainArea* pRow = (Row_EntertainArea*) (*i).second;	

		char tmp_PK_EntertainArea[32];
sprintf(tmp_PK_EntertainArea, "%li", key.pk);


string condition;
condition = condition + "PK_EntertainArea=" + tmp_PK_EntertainArea;

	
		string query = "delete from EntertainArea where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_EntertainArea::GetRows(string where_statement,vector<class Row_EntertainArea*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from EntertainArea " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from EntertainArea where " + where_statement;
		
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

		Row_EntertainArea *pRow = new Row_EntertainArea(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_EntertainArea = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_EntertainArea));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Room = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Room));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Only1Stream = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Only1Stream));
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

		SingleLongKey key(pRow->m_PK_EntertainArea);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_EntertainArea *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_EntertainArea* Table_EntertainArea::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_EntertainArea *pRow = new Row_EntertainArea(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_EntertainArea* Table_EntertainArea::GetRow(long int in_PK_EntertainArea)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_EntertainArea);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_EntertainArea*) (*i).second;
	//we have to fetch row
	Row_EntertainArea* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_EntertainArea* Table_EntertainArea::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_EntertainArea[32];
sprintf(tmp_PK_EntertainArea, "%li", key.pk);


string condition;
condition = condition + "PK_EntertainArea=" + tmp_PK_EntertainArea;


	string query = "select * from EntertainArea where " + condition;		

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

	Row_EntertainArea *pRow = new Row_EntertainArea(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_EntertainArea = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_EntertainArea));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Room = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Room));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Only1Stream = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Only1Stream));
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


class Row_Room* Row_EntertainArea::FK_Room_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Room *pTable = table->database->Room_get();
return pTable->GetRow(m_FK_Room);
}


void Row_EntertainArea::CommandGroup_EntertainArea_FK_EntertainArea_getrows(vector <class Row_CommandGroup_EntertainArea*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_EntertainArea *pTable = table->database->CommandGroup_EntertainArea_get();
pTable->GetRows("FK_EntertainArea=" + StringUtils::itos(m_PK_EntertainArea),rows);
}
void Row_EntertainArea::Device_EntertainArea_FK_EntertainArea_getrows(vector <class Row_Device_EntertainArea*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_EntertainArea *pTable = table->database->Device_EntertainArea_get();
pTable->GetRows("FK_EntertainArea=" + StringUtils::itos(m_PK_EntertainArea),rows);
}
void Row_EntertainArea::Orbiter_FK_EntertainArea_getrows(vector <class Row_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter *pTable = table->database->Orbiter_get();
pTable->GetRows("FK_EntertainArea=" + StringUtils::itos(m_PK_EntertainArea),rows);
}



