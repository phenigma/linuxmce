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
#include "Table_RepositorySource_URL.h"
#include "Table_RepositorySource.h"
#include "Table_Country.h"

#include "Table_Installation_RepositorySource_URL.h"


void Database_pluto_main::CreateTable_RepositorySource_URL()
{
	tblRepositorySource_URL = new Table_RepositorySource_URL(this);
}

void Database_pluto_main::DeleteTable_RepositorySource_URL()
{
	delete tblRepositorySource_URL;
}

Table_RepositorySource_URL::~Table_RepositorySource_URL()
{
	map<Table_RepositorySource_URL::Key, class Row_RepositorySource_URL*, Table_RepositorySource_URL::Key_Less>::iterator it;
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


void Row_RepositorySource_URL::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_RepositorySource_URL*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_RepositorySource_URL::Key key(this);					
			map<Table_RepositorySource_URL::Key, Row_RepositorySource_URL*, Table_RepositorySource_URL::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_RepositorySource_URL::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_RepositorySource_URL::Key key(this);		
		Row_RepositorySource_URL *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_RepositorySource_URL::Row_RepositorySource_URL(Table_RepositorySource_URL *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_RepositorySource_URL::SetDefaultValues()
{
	m_PK_RepositorySource_URL = 0;
is_null[0] = false;
m_FK_RepositorySource = 0;
is_null[1] = false;
m_URL = "";
is_null[2] = false;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_RepositorySource_URL::PK_RepositorySource_URL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_RepositorySource_URL;}
long int Row_RepositorySource_URL::FK_RepositorySource_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_RepositorySource;}
string Row_RepositorySource_URL::URL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_URL;}
long int Row_RepositorySource_URL::FK_Country_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Country;}

		
void Row_RepositorySource_URL::PK_RepositorySource_URL_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_RepositorySource_URL = val; is_modified=true; is_null[0]=false;}
void Row_RepositorySource_URL::FK_RepositorySource_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_RepositorySource = val; is_modified=true; is_null[1]=false;}
void Row_RepositorySource_URL::URL_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_URL = val; is_modified=true; is_null[2]=false;}
void Row_RepositorySource_URL::FK_Country_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Country = val; is_modified=true; is_null[3]=false;}

		
bool Row_RepositorySource_URL::FK_Country_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}

			
void Row_RepositorySource_URL::FK_Country_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
	

string Row_RepositorySource_URL::PK_RepositorySource_URL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_RepositorySource_URL);

return buf;
}

string Row_RepositorySource_URL::FK_RepositorySource_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositorySource);

return buf;
}

string Row_RepositorySource_URL::URL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[511];
mysql_real_escape_string(table->database->db_handle, buf, m_URL.c_str(), (unsigned long) m_URL.size());
return string()+"\""+buf+"\"";
}

string Row_RepositorySource_URL::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}




Table_RepositorySource_URL::Key::Key(long int in_PK_RepositorySource_URL)
{
			pk_PK_RepositorySource_URL = in_PK_RepositorySource_URL;
	
}

Table_RepositorySource_URL::Key::Key(Row_RepositorySource_URL *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_RepositorySource_URL = pRow->m_PK_RepositorySource_URL;
	
}		

bool Table_RepositorySource_URL::Key_Less::operator()(const Table_RepositorySource_URL::Key &key1, const Table_RepositorySource_URL::Key &key2) const
{
			if (key1.pk_PK_RepositorySource_URL!=key2.pk_PK_RepositorySource_URL)
return key1.pk_PK_RepositorySource_URL<key2.pk_PK_RepositorySource_URL;
else
return false;	
}	

void Table_RepositorySource_URL::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_RepositorySource_URL*>::iterator i = addedRows.begin();
	
		Row_RepositorySource_URL *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_RepositorySource_URL_asSQL()+", "+pRow->FK_RepositorySource_asSQL()+", "+pRow->URL_asSQL()+", "+pRow->FK_Country_asSQL();

	
		string query = "insert into RepositorySource_URL (PK_RepositorySource_URL, FK_RepositorySource, URL, FK_Country) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_RepositorySource_URL=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_RepositorySource_URL*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_RepositorySource_URL* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk_PK_RepositorySource_URL);


string condition;
condition = condition + "PK_RepositorySource_URL=" + tmp_PK_RepositorySource_URL;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_RepositorySource_URL="+pRow->PK_RepositorySource_URL_asSQL()+", FK_RepositorySource="+pRow->FK_RepositorySource_asSQL()+", URL="+pRow->URL_asSQL()+", FK_Country="+pRow->FK_Country_asSQL();

	
		string query = "update RepositorySource_URL set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_RepositorySource_URL*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_RepositorySource_URL*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk_PK_RepositorySource_URL);


string condition;
condition = condition + "PK_RepositorySource_URL=" + tmp_PK_RepositorySource_URL;

	
		string query = "delete from RepositorySource_URL where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_RepositorySource_URL::GetRows(string where_statement,vector<class Row_RepositorySource_URL*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from RepositorySource_URL where " + where_statement;
		
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

		Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositorySource_URL));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_URL = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Country));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_RepositorySource_URL::Key, Row_RepositorySource_URL*, Table_RepositorySource_URL::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_RepositorySource_URL* Table_RepositorySource_URL::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_RepositorySource_URL* Table_RepositorySource_URL::GetRow(long int in_PK_RepositorySource_URL)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_RepositorySource_URL);

	map<Key, Row_RepositorySource_URL*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_RepositorySource_URL* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_RepositorySource_URL* Table_RepositorySource_URL::FetchRow(Table_RepositorySource_URL::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk_PK_RepositorySource_URL);


string condition;
condition = condition + "PK_RepositorySource_URL=" + tmp_PK_RepositorySource_URL;


	string query = "select * from RepositorySource_URL where " + condition;		

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

	Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositorySource_URL));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_URL = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Country));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_RepositorySource* Row_RepositorySource_URL::FK_RepositorySource_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
return pTable->GetRow(m_FK_RepositorySource);
}
class Row_Country* Row_RepositorySource_URL::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}


void Row_RepositorySource_URL::Installation_RepositorySource_URL_FK_RepositorySource_URL_getrows(vector <class Row_Installation_RepositorySource_URL*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation_RepositorySource_URL *pTable = table->database->Installation_RepositorySource_URL_get();
pTable->GetRows("FK_RepositorySource_URL=" + StringUtils::itos(m_PK_RepositorySource_URL),rows);
}



