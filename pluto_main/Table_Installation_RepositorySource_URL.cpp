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
#include "Table_Installation_RepositorySource_URL.h"
#include "Table_Installation.h"
#include "Table_RepositorySource_URL.h"



void Database_pluto_main::CreateTable_Installation_RepositorySource_URL()
{
	tblInstallation_RepositorySource_URL = new Table_Installation_RepositorySource_URL(this);
}

void Database_pluto_main::DeleteTable_Installation_RepositorySource_URL()
{
	delete tblInstallation_RepositorySource_URL;
}

Table_Installation_RepositorySource_URL::~Table_Installation_RepositorySource_URL()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Installation_RepositorySource_URL *pRow = (Row_Installation_RepositorySource_URL *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Installation_RepositorySource_URL *pRow = (Row_Installation_RepositorySource_URL *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Installation_RepositorySource_URL::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Installation_RepositorySource_URL *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Installation_RepositorySource_URL *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Installation,pRow->m_FK_RepositorySource_URL);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Installation_RepositorySource_URL::Reload()
{
	Row_Installation_RepositorySource_URL *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Installation,pRow->m_FK_RepositorySource_URL);
		Row_Installation_RepositorySource_URL *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Installation_RepositorySource_URL::Row_Installation_RepositorySource_URL(Table_Installation_RepositorySource_URL *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Installation_RepositorySource_URL::SetDefaultValues()
{
	m_FK_Installation = 0;
is_null[0] = false;
m_FK_RepositorySource_URL = 0;
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Installation_RepositorySource_URL::FK_Installation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Installation;}
long int Row_Installation_RepositorySource_URL::FK_RepositorySource_URL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_RepositorySource_URL;}

		
void Row_Installation_RepositorySource_URL::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Installation = val; is_modified=true; is_null[0]=false;}
void Row_Installation_RepositorySource_URL::FK_RepositorySource_URL_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_RepositorySource_URL = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_Installation_RepositorySource_URL::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_Installation_RepositorySource_URL::FK_RepositorySource_URL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositorySource_URL);

return buf;
}




Table_Installation_RepositorySource_URL::Key::Key(long int in_FK_Installation, long int in_FK_RepositorySource_URL)
{
			pk_FK_Installation = in_FK_Installation;
pk_FK_RepositorySource_URL = in_FK_RepositorySource_URL;
	
}

Table_Installation_RepositorySource_URL::Key::Key(Row_Installation_RepositorySource_URL *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Installation = pRow->m_FK_Installation;
pk_FK_RepositorySource_URL = pRow->m_FK_RepositorySource_URL;
	
}		

bool Table_Installation_RepositorySource_URL::Key_Less::operator()(const Table_Installation_RepositorySource_URL::Key &key1, const Table_Installation_RepositorySource_URL::Key &key2) const
{
			if (key1.pk_FK_Installation!=key2.pk_FK_Installation)
return key1.pk_FK_Installation<key2.pk_FK_Installation;
else
if (key1.pk_FK_RepositorySource_URL!=key2.pk_FK_RepositorySource_URL)
return key1.pk_FK_RepositorySource_URL<key2.pk_FK_RepositorySource_URL;
else
return false;	
}	

void Table_Installation_RepositorySource_URL::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Installation_RepositorySource_URL *pRow = (Row_Installation_RepositorySource_URL *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Installation_asSQL()+", "+pRow->FK_RepositorySource_URL_asSQL();

	
		string query = "insert into Installation_RepositorySource_URL (FK_Installation, FK_RepositorySource_URL) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Installation,pRow->m_FK_RepositorySource_URL);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Installation_RepositorySource_URL* pRow = (Row_Installation_RepositorySource_URL*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Installation,pRow->m_FK_RepositorySource_URL);

		char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk1);

char tmp_FK_RepositorySource_URL[32];
sprintf(tmp_FK_RepositorySource_URL, "%li", key.pk2);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"FK_RepositorySource_URL=" + tmp_FK_RepositorySource_URL;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Installation="+pRow->FK_Installation_asSQL()+", FK_RepositorySource_URL="+pRow->FK_RepositorySource_URL_asSQL();

	
		string query = "update Installation_RepositorySource_URL set " + update_values_list + " where " + condition;
			
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
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Installation_RepositorySource_URL* pRow = (Row_Installation_RepositorySource_URL*) (*i).second;	

		char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk1);

char tmp_FK_RepositorySource_URL[32];
sprintf(tmp_FK_RepositorySource_URL, "%li", key.pk2);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"FK_RepositorySource_URL=" + tmp_FK_RepositorySource_URL;

	
		string query = "delete from Installation_RepositorySource_URL where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Installation_RepositorySource_URL::GetRows(string where_statement,vector<class Row_Installation_RepositorySource_URL*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Installation_RepositorySource_URL " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Installation_RepositorySource_URL where " + where_statement;
		
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

		Row_Installation_RepositorySource_URL *pRow = new Row_Installation_RepositorySource_URL(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource_URL));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Installation,pRow->m_FK_RepositorySource_URL);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Installation_RepositorySource_URL *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Installation_RepositorySource_URL* Table_Installation_RepositorySource_URL::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Installation_RepositorySource_URL *pRow = new Row_Installation_RepositorySource_URL(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Installation_RepositorySource_URL* Table_Installation_RepositorySource_URL::GetRow(long int in_FK_Installation, long int in_FK_RepositorySource_URL)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_Installation, in_FK_RepositorySource_URL);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Installation_RepositorySource_URL*) (*i).second;
	//we have to fetch row
	Row_Installation_RepositorySource_URL* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Installation_RepositorySource_URL* Table_Installation_RepositorySource_URL::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk1);

char tmp_FK_RepositorySource_URL[32];
sprintf(tmp_FK_RepositorySource_URL, "%li", key.pk2);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"FK_RepositorySource_URL=" + tmp_FK_RepositorySource_URL;


	string query = "select * from Installation_RepositorySource_URL where " + condition;		

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

	Row_Installation_RepositorySource_URL *pRow = new Row_Installation_RepositorySource_URL(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource_URL));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Installation* Row_Installation_RepositorySource_URL::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}
class Row_RepositorySource_URL* Row_Installation_RepositorySource_URL::FK_RepositorySource_URL_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource_URL *pTable = table->database->RepositorySource_URL_get();
return pTable->GetRow(m_FK_RepositorySource_URL);
}





