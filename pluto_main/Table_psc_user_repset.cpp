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
#include "Table_psc_user_repset.h"



void Database_pluto_main::CreateTable_psc_user_repset()
{
	tblpsc_user_repset = new Table_psc_user_repset(this);
}

void Database_pluto_main::DeleteTable_psc_user_repset()
{
	delete tblpsc_user_repset;
}

Table_psc_user_repset::~Table_psc_user_repset()
{
	map<Table_psc_user_repset::Key, class Row_psc_user_repset*, Table_psc_user_repset::Key_Less>::iterator it;
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


void Row_psc_user_repset::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_psc_user_repset*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_psc_user_repset::Key key(this);					
			map<Table_psc_user_repset::Key, Row_psc_user_repset*, Table_psc_user_repset::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_user_repset::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_psc_user_repset::Key key(this);		
		Row_psc_user_repset *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_user_repset::Row_psc_user_repset(Table_psc_user_repset *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_user_repset::SetDefaultValues()
{
	m_PK_psc_user_repset = 0;
is_null[0] = false;
m_Value = "";
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_user_repset::PK_psc_user_repset_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_psc_user_repset;}
string Row_psc_user_repset::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}

		
void Row_psc_user_repset::PK_psc_user_repset_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_psc_user_repset = val; is_modified=true; is_null[0]=false;}
void Row_psc_user_repset::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_psc_user_repset::PK_psc_user_repset_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_user_repset);

return buf;
}

string Row_psc_user_repset::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
return string()+"\""+buf+"\"";
}




Table_psc_user_repset::Key::Key(long int in_PK_psc_user_repset)
{
			pk_PK_psc_user_repset = in_PK_psc_user_repset;
	
}

Table_psc_user_repset::Key::Key(Row_psc_user_repset *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_psc_user_repset = pRow->m_PK_psc_user_repset;
	
}		

bool Table_psc_user_repset::Key_Less::operator()(const Table_psc_user_repset::Key &key1, const Table_psc_user_repset::Key &key2) const
{
			if (key1.pk_PK_psc_user_repset!=key2.pk_PK_psc_user_repset)
return key1.pk_PK_psc_user_repset<key2.pk_PK_psc_user_repset;
else
return false;	
}	

void Table_psc_user_repset::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_psc_user_repset*>::iterator i = addedRows.begin();
	
		Row_psc_user_repset *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_user_repset_asSQL()+", "+pRow->Value_asSQL();

	
		string query = "insert into psc_user_repset (PK_psc_user_repset, Value) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_psc_user_repset=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_psc_user_repset*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_psc_user_repset* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_psc_user_repset[32];
sprintf(tmp_PK_psc_user_repset, "%li", key.pk_PK_psc_user_repset);


string condition;
condition = condition + "PK_psc_user_repset=" + tmp_PK_psc_user_repset;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_psc_user_repset="+pRow->PK_psc_user_repset_asSQL()+", Value="+pRow->Value_asSQL();

	
		string query = "update psc_user_repset set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_psc_user_repset*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_psc_user_repset*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_psc_user_repset[32];
sprintf(tmp_PK_psc_user_repset, "%li", key.pk_PK_psc_user_repset);


string condition;
condition = condition + "PK_psc_user_repset=" + tmp_PK_psc_user_repset;

	
		string query = "delete from psc_user_repset where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_psc_user_repset::GetRows(string where_statement,vector<class Row_psc_user_repset*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from psc_user_repset " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from psc_user_repset where " + where_statement;
		
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

		Row_psc_user_repset *pRow = new Row_psc_user_repset(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_user_repset = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_user_repset));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Value = string(row[1],lengths[1]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_psc_user_repset::Key, Row_psc_user_repset*, Table_psc_user_repset::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_psc_user_repset* Table_psc_user_repset::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_psc_user_repset *pRow = new Row_psc_user_repset(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_user_repset* Table_psc_user_repset::GetRow(long int in_PK_psc_user_repset)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_psc_user_repset);

	map<Key, Row_psc_user_repset*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_psc_user_repset* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_user_repset* Table_psc_user_repset::FetchRow(Table_psc_user_repset::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_psc_user_repset[32];
sprintf(tmp_PK_psc_user_repset, "%li", key.pk_PK_psc_user_repset);


string condition;
condition = condition + "PK_psc_user_repset=" + tmp_PK_psc_user_repset;


	string query = "select * from psc_user_repset where " + condition;		

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

	Row_psc_user_repset *pRow = new Row_psc_user_repset(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_user_repset = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_user_repset));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Value = string(row[1],lengths[1]);
}



	mysql_free_result(res);			
	
	return pRow;						
}







