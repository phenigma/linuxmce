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
#include "Table_License.h"

#include "Table_Package.h"


void Database_pluto_main::CreateTable_License()
{
	tblLicense = new Table_License(this);
}

void Database_pluto_main::DeleteTable_License()
{
	delete tblLicense;
}

Table_License::~Table_License()
{
	map<Table_License::Key, class Row_License*, Table_License::Key_Less>::iterator it;
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


void Row_License::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_License*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_License::Key key(this);					
			map<Table_License::Key, Row_License*, Table_License::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_License::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_License::Key key(this);		
		Row_License *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_License::Row_License(Table_License *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_License::SetDefaultValues()
{
	m_PK_License = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
m_Define = "";
is_null[2] = false;
is_null[3] = true;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_License::PK_License_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_License;}
string Row_License::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_License::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_License::Summary_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Summary;}
string Row_License::URL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_URL;}

		
void Row_License::PK_License_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_License = val; is_modified=true; is_null[0]=false;}
void Row_License::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_License::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_License::Summary_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Summary = val; is_modified=true; is_null[3]=false;}
void Row_License::URL_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_URL = val; is_modified=true; is_null[4]=false;}

		
bool Row_License::Summary_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_License::URL_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}

			
void Row_License::Summary_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_License::URL_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
	

string Row_License::PK_License_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_License);

return buf;
}

string Row_License::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_License::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_License::Summary_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Summary.c_str(), (unsigned long) m_Summary.size());
return string()+"\""+buf+"\"";
}

string Row_License::URL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_URL.c_str(), (unsigned long) m_URL.size());
return string()+"\""+buf+"\"";
}




Table_License::Key::Key(long int in_PK_License)
{
			pk_PK_License = in_PK_License;
	
}

Table_License::Key::Key(Row_License *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_License = pRow->m_PK_License;
	
}		

bool Table_License::Key_Less::operator()(const Table_License::Key &key1, const Table_License::Key &key2) const
{
			if (key1.pk_PK_License!=key2.pk_PK_License)
return key1.pk_PK_License<key2.pk_PK_License;
else
return false;	
}	

void Table_License::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_License*>::iterator i = addedRows.begin();
	
		Row_License *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_License_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Summary_asSQL()+", "+pRow->URL_asSQL();

	
		string query = "insert into License (PK_License, Description, Define, Summary, URL) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_License=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_License*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_License* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk_PK_License);


string condition;
condition = condition + "PK_License=" + tmp_PK_License;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_License="+pRow->PK_License_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Summary="+pRow->Summary_asSQL()+", URL="+pRow->URL_asSQL();

	
		string query = "update License set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_License*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_License*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk_PK_License);


string condition;
condition = condition + "PK_License=" + tmp_PK_License;

	
		string query = "delete from License where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_License::GetRows(string where_statement,vector<class Row_License*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from License " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from License where " + where_statement;
		
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

		Row_License *pRow = new Row_License(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_License = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_License));
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

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Summary = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_URL = string(row[4],lengths[4]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_License::Key, Row_License*, Table_License::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_License* Table_License::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_License *pRow = new Row_License(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_License* Table_License::GetRow(long int in_PK_License)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_License);

	map<Key, Row_License*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_License* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_License* Table_License::FetchRow(Table_License::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk_PK_License);


string condition;
condition = condition + "PK_License=" + tmp_PK_License;


	string query = "select * from License where " + condition;		

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

	Row_License *pRow = new Row_License(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_License = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_License));
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

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Summary = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_URL = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_License::Package_FK_License_getrows(vector <class Row_Package*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
pTable->GetRows("FK_License=" + StringUtils::itos(m_PK_License),rows);
}



