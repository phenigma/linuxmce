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
#include "Table_OperatingSystem.h"

#include "Table_Distro.h"
#include "Table_Package_Directory.h"
#include "Table_Package_Directory_File.h"
#include "Table_Package_Source_Compat.h"
#include "Table_RepositorySource.h"


void Database_pluto_main::CreateTable_OperatingSystem()
{
	tblOperatingSystem = new Table_OperatingSystem(this);
}

void Database_pluto_main::DeleteTable_OperatingSystem()
{
	delete tblOperatingSystem;
}

Table_OperatingSystem::~Table_OperatingSystem()
{
	map<Table_OperatingSystem::Key, class Row_OperatingSystem*, Table_OperatingSystem::Key_Less>::iterator it;
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


void Row_OperatingSystem::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_OperatingSystem*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_OperatingSystem::Key key(this);					
			map<Table_OperatingSystem::Key, Row_OperatingSystem*, Table_OperatingSystem::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_OperatingSystem::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_OperatingSystem::Key key(this);		
		Row_OperatingSystem *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_OperatingSystem::Row_OperatingSystem(Table_OperatingSystem *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_OperatingSystem::SetDefaultValues()
{
	m_PK_OperatingSystem = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_OperatingSystem::PK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_OperatingSystem;}
string Row_OperatingSystem::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_OperatingSystem::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}

		
void Row_OperatingSystem::PK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_OperatingSystem = val; is_modified=true; is_null[0]=false;}
void Row_OperatingSystem::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_OperatingSystem::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}

		
bool Row_OperatingSystem::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_OperatingSystem::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_OperatingSystem::PK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_OperatingSystem);

return buf;
}

string Row_OperatingSystem::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_OperatingSystem::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}




Table_OperatingSystem::Key::Key(long int in_PK_OperatingSystem)
{
			pk_PK_OperatingSystem = in_PK_OperatingSystem;
	
}

Table_OperatingSystem::Key::Key(Row_OperatingSystem *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_OperatingSystem = pRow->m_PK_OperatingSystem;
	
}		

bool Table_OperatingSystem::Key_Less::operator()(const Table_OperatingSystem::Key &key1, const Table_OperatingSystem::Key &key2) const
{
			if (key1.pk_PK_OperatingSystem!=key2.pk_PK_OperatingSystem)
return key1.pk_PK_OperatingSystem<key2.pk_PK_OperatingSystem;
else
return false;	
}	

void Table_OperatingSystem::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_OperatingSystem*>::iterator i = addedRows.begin();
	
		Row_OperatingSystem *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_OperatingSystem_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL();

	
		string query = "insert into OperatingSystem (PK_OperatingSystem, Description, Define) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_OperatingSystem=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_OperatingSystem*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_OperatingSystem* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk_PK_OperatingSystem);


string condition;
condition = condition + "PK_OperatingSystem=" + tmp_PK_OperatingSystem;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_OperatingSystem="+pRow->PK_OperatingSystem_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL();

	
		string query = "update OperatingSystem set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_OperatingSystem*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_OperatingSystem*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk_PK_OperatingSystem);


string condition;
condition = condition + "PK_OperatingSystem=" + tmp_PK_OperatingSystem;

	
		string query = "delete from OperatingSystem where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_OperatingSystem::GetRows(string where_statement,vector<class Row_OperatingSystem*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from OperatingSystem where " + where_statement;
		
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

		Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OperatingSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OperatingSystem));
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



		//checking for duplicates

		Key key(pRow);
		
                map<Table_OperatingSystem::Key, Row_OperatingSystem*, Table_OperatingSystem::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_OperatingSystem* Table_OperatingSystem::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_OperatingSystem* Table_OperatingSystem::GetRow(long int in_PK_OperatingSystem)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_OperatingSystem);

	map<Key, Row_OperatingSystem*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_OperatingSystem* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_OperatingSystem* Table_OperatingSystem::FetchRow(Table_OperatingSystem::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk_PK_OperatingSystem);


string condition;
condition = condition + "PK_OperatingSystem=" + tmp_PK_OperatingSystem;


	string query = "select * from OperatingSystem where " + condition;		

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

	Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OperatingSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OperatingSystem));
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



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_OperatingSystem::Distro_FK_OperatingSystem_getrows(vector <class Row_Distro*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
pTable->GetRows("FK_OperatingSystem=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Directory_FK_OperatingSystem_getrows(vector <class Row_Package_Directory*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory *pTable = table->database->Package_Directory_get();
pTable->GetRows("FK_OperatingSystem=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Directory_File_FK_OperatingSystem_getrows(vector <class Row_Package_Directory_File*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory_File *pTable = table->database->Package_Directory_File_get();
pTable->GetRows("FK_OperatingSystem=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Source_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Source_Compat*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Source_Compat *pTable = table->database->Package_Source_Compat_get();
pTable->GetRows("FK_OperatingSystem=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::RepositorySource_FK_OperatingSystem_getrows(vector <class Row_RepositorySource*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
pTable->GetRows("FK_OperatingSystem=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}



