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
#include "Table_Package_Compat.h"
#include "Table_Package.h"
#include "Table_Distro.h"
#include "Table_OperatingSystem.h"



void Database_pluto_main::CreateTable_Package_Compat()
{
	tblPackage_Compat = new Table_Package_Compat(this);
}

void Database_pluto_main::DeleteTable_Package_Compat()
{
	delete tblPackage_Compat;
}

Table_Package_Compat::~Table_Package_Compat()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Package_Compat *pRow = (Row_Package_Compat *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Package_Compat *pRow = (Row_Package_Compat *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Package_Compat::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Package_Compat *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Package_Compat *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Package_Compat);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Package_Compat::Reload()
{
	Row_Package_Compat *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Package_Compat);
		Row_Package_Compat *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Compat::Row_Package_Compat(Table_Package_Compat *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Compat::SetDefaultValues()
{
	m_PK_Package_Compat = 0;
is_null[0] = false;
m_FK_Package = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Compat::PK_Package_Compat_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Package_Compat;}
long int Row_Package_Compat::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
long int Row_Package_Compat::FK_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Distro;}
long int Row_Package_Compat::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_OperatingSystem;}

		
void Row_Package_Compat::PK_Package_Compat_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Package_Compat = val; is_modified=true; is_null[0]=false;}
void Row_Package_Compat::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[1]=false;}
void Row_Package_Compat::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Distro = val; is_modified=true; is_null[2]=false;}
void Row_Package_Compat::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[3]=false;}

		
bool Row_Package_Compat::FK_Distro_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Package_Compat::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}

			
void Row_Package_Compat::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Package_Compat::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
	

string Row_Package_Compat::PK_Package_Compat_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Package_Compat);

return buf;
}

string Row_Package_Compat::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Package_Compat::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_Package_Compat::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}




Table_Package_Compat::Key::Key(long int in_PK_Package_Compat)
{
			pk_PK_Package_Compat = in_PK_Package_Compat;
	
}

Table_Package_Compat::Key::Key(Row_Package_Compat *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Package_Compat = pRow->m_PK_Package_Compat;
	
}		

bool Table_Package_Compat::Key_Less::operator()(const Table_Package_Compat::Key &key1, const Table_Package_Compat::Key &key2) const
{
			if (key1.pk_PK_Package_Compat!=key2.pk_PK_Package_Compat)
return key1.pk_PK_Package_Compat<key2.pk_PK_Package_Compat;
else
return false;	
}	

void Table_Package_Compat::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Package_Compat *pRow = (Row_Package_Compat *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Package_Compat_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->FK_OperatingSystem_asSQL();

	
		string query = "insert into Package_Compat (PK_Package_Compat, FK_Package, FK_Distro, FK_OperatingSystem) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Package_Compat=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Package_Compat);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Package_Compat* pRow = (Row_Package_Compat*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Package_Compat);

		char tmp_PK_Package_Compat[32];
sprintf(tmp_PK_Package_Compat, "%li", key.pk);


string condition;
condition = condition + "PK_Package_Compat=" + tmp_PK_Package_Compat;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Package_Compat="+pRow->PK_Package_Compat_asSQL()+", FK_Package="+pRow->FK_Package_asSQL()+", FK_Distro="+pRow->FK_Distro_asSQL()+", FK_OperatingSystem="+pRow->FK_OperatingSystem_asSQL();

	
		string query = "update Package_Compat set " + update_values_list + " where " + condition;
			
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
		Row_Package_Compat* pRow = (Row_Package_Compat*) (*i).second;	

		char tmp_PK_Package_Compat[32];
sprintf(tmp_PK_Package_Compat, "%li", key.pk);


string condition;
condition = condition + "PK_Package_Compat=" + tmp_PK_Package_Compat;

	
		string query = "delete from Package_Compat where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Package_Compat::GetRows(string where_statement,vector<class Row_Package_Compat*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Package_Compat " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Package_Compat where " + where_statement;
		
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

		Row_Package_Compat *pRow = new Row_Package_Compat(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package_Compat = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Package_Compat));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Package_Compat);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Package_Compat *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Package_Compat* Table_Package_Compat::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Package_Compat *pRow = new Row_Package_Compat(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Compat* Table_Package_Compat::GetRow(long int in_PK_Package_Compat)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Package_Compat);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Package_Compat*) (*i).second;
	//we have to fetch row
	Row_Package_Compat* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Compat* Table_Package_Compat::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Package_Compat[32];
sprintf(tmp_PK_Package_Compat, "%li", key.pk);


string condition;
condition = condition + "PK_Package_Compat=" + tmp_PK_Package_Compat;


	string query = "select * from Package_Compat where " + condition;		

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

	Row_Package_Compat *pRow = new Row_Package_Compat(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package_Compat = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Package_Compat));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_Package_Compat::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Distro* Row_Package_Compat::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}
class Row_OperatingSystem* Row_Package_Compat::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}





