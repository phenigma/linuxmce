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
#include "Table_Package_Package.h"
#include "Table_Package.h"
#include "Table_Package.h"



void Database_pluto_main::CreateTable_Package_Package()
{
	tblPackage_Package = new Table_Package_Package(this);
}

void Database_pluto_main::DeleteTable_Package_Package()
{
	delete tblPackage_Package;
}

Table_Package_Package::~Table_Package_Package()
{
	map<Table_Package_Package::Key, class Row_Package_Package*, Table_Package_Package::Key_Less>::iterator it;
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


void Row_Package_Package::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Package_Package*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Package_Package::Key key(this);					
			map<Table_Package_Package::Key, Row_Package_Package*, Table_Package_Package::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Package_Package::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Package_Package::Key key(this);		
		Row_Package_Package *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Package::Row_Package_Package(Table_Package_Package *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Package::SetDefaultValues()
{
	m_FK_Package = 0;
is_null[0] = false;
m_FK_Package_DependsOn = 0;
is_null[1] = false;
m_OnlyToBuild = 0;
is_null[2] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Package::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
long int Row_Package_Package::FK_Package_DependsOn_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package_DependsOn;}
short int Row_Package_Package::OnlyToBuild_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_OnlyToBuild;}

		
void Row_Package_Package::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[0]=false;}
void Row_Package_Package::FK_Package_DependsOn_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package_DependsOn = val; is_modified=true; is_null[1]=false;}
void Row_Package_Package::OnlyToBuild_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_OnlyToBuild = val; is_modified=true; is_null[2]=false;}

		

			
	

string Row_Package_Package::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Package_Package::FK_Package_DependsOn_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package_DependsOn);

return buf;
}

string Row_Package_Package::OnlyToBuild_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_OnlyToBuild);

return buf;
}




Table_Package_Package::Key::Key(long int in_FK_Package, long int in_FK_Package_DependsOn)
{
			pk_FK_Package = in_FK_Package;
pk_FK_Package_DependsOn = in_FK_Package_DependsOn;
	
}

Table_Package_Package::Key::Key(Row_Package_Package *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Package = pRow->m_FK_Package;
pk_FK_Package_DependsOn = pRow->m_FK_Package_DependsOn;
	
}		

bool Table_Package_Package::Key_Less::operator()(const Table_Package_Package::Key &key1, const Table_Package_Package::Key &key2) const
{
			if (key1.pk_FK_Package!=key2.pk_FK_Package)
return key1.pk_FK_Package<key2.pk_FK_Package;
else
if (key1.pk_FK_Package_DependsOn!=key2.pk_FK_Package_DependsOn)
return key1.pk_FK_Package_DependsOn<key2.pk_FK_Package_DependsOn;
else
return false;	
}	

void Table_Package_Package::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Package_Package*>::iterator i = addedRows.begin();
	
		Row_Package_Package *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Package_asSQL()+", "+pRow->FK_Package_DependsOn_asSQL()+", "+pRow->OnlyToBuild_asSQL();

	
		string query = "insert into Package_Package (FK_Package, FK_Package_DependsOn, OnlyToBuild) values ("+
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
	

	for (map<Key, Row_Package_Package*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Package_Package* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Package_DependsOn[32];
sprintf(tmp_FK_Package_DependsOn, "%li", key.pk_FK_Package_DependsOn);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Package_DependsOn=" + tmp_FK_Package_DependsOn;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Package="+pRow->FK_Package_asSQL()+", FK_Package_DependsOn="+pRow->FK_Package_DependsOn_asSQL()+", OnlyToBuild="+pRow->OnlyToBuild_asSQL();

	
		string query = "update Package_Package set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Package_Package*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Package_Package*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Package_DependsOn[32];
sprintf(tmp_FK_Package_DependsOn, "%li", key.pk_FK_Package_DependsOn);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Package_DependsOn=" + tmp_FK_Package_DependsOn;

	
		string query = "delete from Package_Package where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Package_Package::GetRows(string where_statement,vector<class Row_Package_Package*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Package_Package where " + where_statement;
		
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

		Row_Package_Package *pRow = new Row_Package_Package(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package_DependsOn = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package_DependsOn));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_OnlyToBuild = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_OnlyToBuild));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Package_Package::Key, Row_Package_Package*, Table_Package_Package::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Package_Package* Table_Package_Package::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Package_Package *pRow = new Row_Package_Package(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Package* Table_Package_Package::GetRow(long int in_FK_Package, long int in_FK_Package_DependsOn)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Package, in_FK_Package_DependsOn);

	map<Key, Row_Package_Package*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Package_Package* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Package* Table_Package_Package::FetchRow(Table_Package_Package::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Package_DependsOn[32];
sprintf(tmp_FK_Package_DependsOn, "%li", key.pk_FK_Package_DependsOn);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Package_DependsOn=" + tmp_FK_Package_DependsOn;


	string query = "select * from Package_Package where " + condition;		

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

	Row_Package_Package *pRow = new Row_Package_Package(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package_DependsOn = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package_DependsOn));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_OnlyToBuild = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_OnlyToBuild));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_Package_Package::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Package* Row_Package_Package::FK_Package_DependsOn_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package_DependsOn);
}





