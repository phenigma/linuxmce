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
#include "Table_Type_Extension.h"
#include "Table_Type.h"



void Database_pluto_media::CreateTable_Type_Extension()
{
	tblType_Extension = new Table_Type_Extension(this);
}

void Database_pluto_media::DeleteTable_Type_Extension()
{
	delete tblType_Extension;
}

Table_Type_Extension::~Table_Type_Extension()
{
	map<Table_Type_Extension::Key, class Row_Type_Extension*, Table_Type_Extension::Key_Less>::iterator it;
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


void Row_Type_Extension::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Type_Extension*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Type_Extension::Key key(this);					
			map<Table_Type_Extension::Key, Row_Type_Extension*, Table_Type_Extension::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Type_Extension::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Type_Extension::Key key(this);		
		Row_Type_Extension *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Type_Extension::Row_Type_Extension(Table_Type_Extension *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Type_Extension::SetDefaultValues()
{
	m_FK_Type = 0;
is_null[0] = false;
m_Extension = "";
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Type_Extension::FK_Type_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Type;}
string Row_Type_Extension::Extension_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Extension;}

		
void Row_Type_Extension::FK_Type_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Type = val; is_modified=true; is_null[0]=false;}
void Row_Type_Extension::Extension_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Extension = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_Type_Extension::FK_Type_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Type);

return buf;
}

string Row_Type_Extension::Extension_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[17];
mysql_real_escape_string(table->database->db_handle, buf, m_Extension.c_str(), (unsigned long) m_Extension.size());
return string()+"\""+buf+"\"";
}




Table_Type_Extension::Key::Key(long int in_FK_Type)
{
			pk_FK_Type = in_FK_Type;
	
}

Table_Type_Extension::Key::Key(Row_Type_Extension *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Type = pRow->m_FK_Type;
	
}		

bool Table_Type_Extension::Key_Less::operator()(const Table_Type_Extension::Key &key1, const Table_Type_Extension::Key &key2) const
{
			if (key1.pk_FK_Type!=key2.pk_FK_Type)
return key1.pk_FK_Type<key2.pk_FK_Type;
else
return false;	
}	

void Table_Type_Extension::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Type_Extension*>::iterator i = addedRows.begin();
	
		Row_Type_Extension *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Type_asSQL()+", "+pRow->Extension_asSQL();

	
		string query = "insert into Type_Extension (FK_Type, Extension) values ("+
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
	

	for (map<Key, Row_Type_Extension*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Type_Extension* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Type="+pRow->FK_Type_asSQL()+", Extension="+pRow->Extension_asSQL();

	
		string query = "update Type_Extension set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Type_Extension*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Type_Extension*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type;

	
		string query = "delete from Type_Extension where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Type_Extension::GetRows(string where_statement,vector<class Row_Type_Extension*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Type_Extension " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Type_Extension where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: " << query << endl;
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

		Row_Type_Extension *pRow = new Row_Type_Extension(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Type));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Extension = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Extension = string(row[1],lengths[1]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Type_Extension::Key, Row_Type_Extension*, Table_Type_Extension::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Type_Extension* Table_Type_Extension::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Type_Extension *pRow = new Row_Type_Extension(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Type_Extension* Table_Type_Extension::GetRow(long int in_FK_Type)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Type);

	map<Key, Row_Type_Extension*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Type_Extension* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Type_Extension* Table_Type_Extension::FetchRow(Table_Type_Extension::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type;


	string query = "select * from Type_Extension where " + condition;		

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

	Row_Type_Extension *pRow = new Row_Type_Extension(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Type));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Extension = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Extension = string(row[1],lengths[1]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Type* Row_Type_Extension::FK_Type_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Type *pTable = table->database->Type_get();
return pTable->GetRow(m_FK_Type);
}





