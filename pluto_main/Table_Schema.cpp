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
#include "Table_Schema.h"
#include "Table_Version.h"



void Database_pluto_main::CreateTable_Schema()
{
	tblSchema = new Table_Schema(this);
}

void Database_pluto_main::DeleteTable_Schema()
{
	delete tblSchema;
}

Table_Schema::~Table_Schema()
{
	map<Table_Schema::Key, class Row_Schema*, Table_Schema::Key_Less>::iterator it;
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


void Row_Schema::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Schema*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Schema::Key key(this);					
			map<Table_Schema::Key, Row_Schema*, Table_Schema::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Schema::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Schema::Key key(this);		
		Row_Schema *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Schema::Row_Schema(Table_Schema *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Schema::SetDefaultValues()
{
	m_PK_Schema = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Schema::PK_Schema_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Schema;}
long int Row_Schema::FK_Version_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Version;}
string Row_Schema::SQLCommands_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SQLCommands;}

		
void Row_Schema::PK_Schema_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Schema = val; is_modified=true; is_null[0]=false;}
void Row_Schema::FK_Version_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Version = val; is_modified=true; is_null[1]=false;}
void Row_Schema::SQLCommands_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SQLCommands = val; is_modified=true; is_null[2]=false;}

		
bool Row_Schema::FK_Version_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Schema::SQLCommands_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_Schema::FK_Version_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Schema::SQLCommands_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_Schema::PK_Schema_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Schema);

return buf;
}

string Row_Schema::FK_Version_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Version);

return buf;
}

string Row_Schema::SQLCommands_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_SQLCommands.c_str(), (unsigned long) m_SQLCommands.size());
return string()+"\""+buf+"\"";
}




Table_Schema::Key::Key(long int in_PK_Schema)
{
			pk_PK_Schema = in_PK_Schema;
	
}

Table_Schema::Key::Key(Row_Schema *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Schema = pRow->m_PK_Schema;
	
}		

bool Table_Schema::Key_Less::operator()(const Table_Schema::Key &key1, const Table_Schema::Key &key2) const
{
			if (key1.pk_PK_Schema!=key2.pk_PK_Schema)
return key1.pk_PK_Schema<key2.pk_PK_Schema;
else
return false;	
}	

void Table_Schema::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Schema*>::iterator i = addedRows.begin();
	
		Row_Schema *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Schema_asSQL()+", "+pRow->FK_Version_asSQL()+", "+pRow->SQLCommands_asSQL();

	
		string query = "insert into Schema (PK_Schema, FK_Version, SQLCommands) values ("+
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
	

	for (map<Key, Row_Schema*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Schema* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Schema[32];
sprintf(tmp_PK_Schema, "%li", key.pk_PK_Schema);


string condition;
condition = condition + "PK_Schema=" + tmp_PK_Schema;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Schema="+pRow->PK_Schema_asSQL()+", FK_Version="+pRow->FK_Version_asSQL()+", SQLCommands="+pRow->SQLCommands_asSQL();

	
		string query = "update Schema set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Schema*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Schema*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Schema[32];
sprintf(tmp_PK_Schema, "%li", key.pk_PK_Schema);


string condition;
condition = condition + "PK_Schema=" + tmp_PK_Schema;

	
		string query = "delete from Schema where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Schema::GetRows(string where_statement,vector<class Row_Schema*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Schema " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Schema where " + where_statement;
		
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

		Row_Schema *pRow = new Row_Schema(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Schema = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Schema));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Version));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_SQLCommands = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_SQLCommands = string(row[2],lengths[2]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Schema::Key, Row_Schema*, Table_Schema::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Schema* Table_Schema::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Schema *pRow = new Row_Schema(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Schema* Table_Schema::GetRow(long int in_PK_Schema)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Schema);

	map<Key, Row_Schema*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Schema* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Schema* Table_Schema::FetchRow(Table_Schema::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Schema[32];
sprintf(tmp_PK_Schema, "%li", key.pk_PK_Schema);


string condition;
condition = condition + "PK_Schema=" + tmp_PK_Schema;


	string query = "select * from Schema where " + condition;		

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

	Row_Schema *pRow = new Row_Schema(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Schema = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Schema));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Version));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_SQLCommands = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_SQLCommands = string(row[2],lengths[2]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Version* Row_Schema::FK_Version_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Version *pTable = table->database->Version_get();
return pTable->GetRow(m_FK_Version);
}





