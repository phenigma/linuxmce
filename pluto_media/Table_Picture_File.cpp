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
#include "Table_Picture_File.h"
#include "Table_Picture.h"
#include "Table_File.h"



void Database_pluto_media::CreateTable_Picture_File()
{
	tblPicture_File = new Table_Picture_File(this);
}

void Database_pluto_media::DeleteTable_Picture_File()
{
	delete tblPicture_File;
}

Table_Picture_File::~Table_Picture_File()
{
	map<Table_Picture_File::Key, class Row_Picture_File*, Table_Picture_File::Key_Less>::iterator it;
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


void Row_Picture_File::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Picture_File*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Picture_File::Key key(this);					
			map<Table_Picture_File::Key, Row_Picture_File*, Table_Picture_File::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Picture_File::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Picture_File::Key key(this);		
		Row_Picture_File *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Picture_File::Row_Picture_File(Table_Picture_File *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Picture_File::SetDefaultValues()
{
	m_FK_Picture = 0;
is_null[0] = false;
m_FK_File = 0;
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Picture_File::FK_Picture_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Picture;}
long int Row_Picture_File::FK_File_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_File;}

		
void Row_Picture_File::FK_Picture_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Picture = val; is_modified=true; is_null[0]=false;}
void Row_Picture_File::FK_File_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_File = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_Picture_File::FK_Picture_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Picture);

return buf;
}

string Row_Picture_File::FK_File_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_File);

return buf;
}




Table_Picture_File::Key::Key(long int in_FK_Picture, long int in_FK_File)
{
			pk_FK_Picture = in_FK_Picture;
pk_FK_File = in_FK_File;
	
}

Table_Picture_File::Key::Key(Row_Picture_File *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Picture = pRow->m_FK_Picture;
pk_FK_File = pRow->m_FK_File;
	
}		

bool Table_Picture_File::Key_Less::operator()(const Table_Picture_File::Key &key1, const Table_Picture_File::Key &key2) const
{
			if (key1.pk_FK_Picture!=key2.pk_FK_Picture)
return key1.pk_FK_Picture<key2.pk_FK_Picture;
else
if (key1.pk_FK_File!=key2.pk_FK_File)
return key1.pk_FK_File<key2.pk_FK_File;
else
return false;	
}	

void Table_Picture_File::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Picture_File*>::iterator i = addedRows.begin();
	
		Row_Picture_File *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Picture_asSQL()+", "+pRow->FK_File_asSQL();

	
		string query = "insert into Picture_File (FK_Picture, FK_File) values ("+
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
	

	for (map<Key, Row_Picture_File*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Picture_File* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Picture[32];
sprintf(tmp_FK_Picture, "%li", key.pk_FK_Picture);

char tmp_FK_File[32];
sprintf(tmp_FK_File, "%li", key.pk_FK_File);


string condition;
condition = condition + "FK_Picture=" + tmp_FK_Picture+" AND "+"FK_File=" + tmp_FK_File;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Picture="+pRow->FK_Picture_asSQL()+", FK_File="+pRow->FK_File_asSQL();

	
		string query = "update Picture_File set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Picture_File*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Picture_File*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Picture[32];
sprintf(tmp_FK_Picture, "%li", key.pk_FK_Picture);

char tmp_FK_File[32];
sprintf(tmp_FK_File, "%li", key.pk_FK_File);


string condition;
condition = condition + "FK_Picture=" + tmp_FK_Picture+" AND "+"FK_File=" + tmp_FK_File;

	
		string query = "delete from Picture_File where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Picture_File::GetRows(string where_statement,vector<class Row_Picture_File*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Picture_File " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Picture_File where " + where_statement;
		
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

		Row_Picture_File *pRow = new Row_Picture_File(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Picture));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_File));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Picture_File::Key, Row_Picture_File*, Table_Picture_File::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Picture_File* Table_Picture_File::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Picture_File *pRow = new Row_Picture_File(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Picture_File* Table_Picture_File::GetRow(long int in_FK_Picture, long int in_FK_File)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Picture, in_FK_File);

	map<Key, Row_Picture_File*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Picture_File* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Picture_File* Table_Picture_File::FetchRow(Table_Picture_File::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Picture[32];
sprintf(tmp_FK_Picture, "%li", key.pk_FK_Picture);

char tmp_FK_File[32];
sprintf(tmp_FK_File, "%li", key.pk_FK_File);


string condition;
condition = condition + "FK_Picture=" + tmp_FK_Picture+" AND "+"FK_File=" + tmp_FK_File;


	string query = "select * from Picture_File where " + condition;		

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

	Row_Picture_File *pRow = new Row_Picture_File(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Picture));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_File));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Picture* Row_Picture_File::FK_Picture_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Picture *pTable = table->database->Picture_get();
return pTable->GetRow(m_FK_Picture);
}
class Row_File* Row_Picture_File::FK_File_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_File *pTable = table->database->File_get();
return pTable->GetRow(m_FK_File);
}





