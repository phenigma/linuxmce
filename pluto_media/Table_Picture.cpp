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
#include "Table_Picture.h"

#include "Table_Picture_Attribute.h"
#include "Table_Picture_File.h"


void Database_pluto_media::CreateTable_Picture()
{
	tblPicture = new Table_Picture(this);
}

void Database_pluto_media::DeleteTable_Picture()
{
	delete tblPicture;
}

Table_Picture::~Table_Picture()
{
	map<Table_Picture::Key, class Row_Picture*, Table_Picture::Key_Less>::iterator it;
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


void Row_Picture::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Picture*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Picture::Key key(this);					
			map<Table_Picture::Key, Row_Picture*, Table_Picture::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Picture::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Picture::Key key(this);		
		Row_Picture *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Picture::Row_Picture(Table_Picture *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Picture::SetDefaultValues()
{
	m_PK_Picture = 0;
is_null[0] = false;
m_Extension = "";
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Picture::PK_Picture_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Picture;}
string Row_Picture::Extension_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Extension;}

		
void Row_Picture::PK_Picture_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Picture = val; is_modified=true; is_null[0]=false;}
void Row_Picture::Extension_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Extension = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_Picture::PK_Picture_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Picture);

return buf;
}

string Row_Picture::Extension_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[15];
mysql_real_escape_string(table->database->db_handle, buf, m_Extension.c_str(), (unsigned long) m_Extension.size());
return string()+"\""+buf+"\"";
}




Table_Picture::Key::Key(long int in_PK_Picture)
{
			pk_PK_Picture = in_PK_Picture;
	
}

Table_Picture::Key::Key(Row_Picture *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Picture = pRow->m_PK_Picture;
	
}		

bool Table_Picture::Key_Less::operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const
{
			if (key1.pk_PK_Picture!=key2.pk_PK_Picture)
return key1.pk_PK_Picture<key2.pk_PK_Picture;
else
return false;	
}	

void Table_Picture::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Picture*>::iterator i = addedRows.begin();
	
		Row_Picture *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Picture_asSQL()+", "+pRow->Extension_asSQL();

	
		string query = "insert into Picture (PK_Picture, Extension) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Picture=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Picture*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Picture* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk_PK_Picture);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Picture="+pRow->PK_Picture_asSQL()+", Extension="+pRow->Extension_asSQL();

	
		string query = "update Picture set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Picture*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Picture*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk_PK_Picture);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;

	
		string query = "delete from Picture where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Picture::GetRows(string where_statement,vector<class Row_Picture*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Picture " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Picture where " + where_statement;
		
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

		Row_Picture *pRow = new Row_Picture(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Picture));
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
		
                map<Table_Picture::Key, Row_Picture*, Table_Picture::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Picture* Table_Picture::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Picture *pRow = new Row_Picture(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Picture* Table_Picture::GetRow(long int in_PK_Picture)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Picture);

	map<Key, Row_Picture*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Picture* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Picture* Table_Picture::FetchRow(Table_Picture::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk_PK_Picture);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;


	string query = "select * from Picture where " + condition;		

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

	Row_Picture *pRow = new Row_Picture(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Picture));
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




void Row_Picture::Picture_Attribute_FK_Picture_getrows(vector <class Row_Picture_Attribute*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Picture_Attribute *pTable = table->database->Picture_Attribute_get();
pTable->GetRows("FK_Picture=" + StringUtils::itos(m_PK_Picture),rows);
}
void Row_Picture::Picture_File_FK_Picture_getrows(vector <class Row_Picture_File*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Picture_File *pTable = table->database->Picture_File_get();
pTable->GetRows("FK_Picture=" + StringUtils::itos(m_PK_Picture),rows);
}



