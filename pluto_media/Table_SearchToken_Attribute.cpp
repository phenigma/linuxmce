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
#include "Table_SearchToken_Attribute.h"
#include "Table_SearchToken.h"
#include "Table_Attribute.h"



void Database_pluto_media::CreateTable_SearchToken_Attribute()
{
	tblSearchToken_Attribute = new Table_SearchToken_Attribute(this);
}

void Database_pluto_media::DeleteTable_SearchToken_Attribute()
{
	delete tblSearchToken_Attribute;
}

Table_SearchToken_Attribute::~Table_SearchToken_Attribute()
{
	map<Table_SearchToken_Attribute::Key, class Row_SearchToken_Attribute*, Table_SearchToken_Attribute::Key_Less>::iterator it;
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


void Row_SearchToken_Attribute::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_SearchToken_Attribute*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_SearchToken_Attribute::Key key(this);					
			map<Table_SearchToken_Attribute::Key, Row_SearchToken_Attribute*, Table_SearchToken_Attribute::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_SearchToken_Attribute::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_SearchToken_Attribute::Key key(this);		
		Row_SearchToken_Attribute *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_SearchToken_Attribute::Row_SearchToken_Attribute(Table_SearchToken_Attribute *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_SearchToken_Attribute::SetDefaultValues()
{
	m_FK_SearchToken = 0;
is_null[0] = false;
m_FK_Attribute = 0;
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_SearchToken_Attribute::FK_SearchToken_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_SearchToken;}
long int Row_SearchToken_Attribute::FK_Attribute_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Attribute;}

		
void Row_SearchToken_Attribute::FK_SearchToken_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_SearchToken = val; is_modified=true; is_null[0]=false;}
void Row_SearchToken_Attribute::FK_Attribute_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Attribute = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_SearchToken_Attribute::FK_SearchToken_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_SearchToken);

return buf;
}

string Row_SearchToken_Attribute::FK_Attribute_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Attribute);

return buf;
}




Table_SearchToken_Attribute::Key::Key(long int in_FK_SearchToken, long int in_FK_Attribute)
{
			pk_FK_SearchToken = in_FK_SearchToken;
pk_FK_Attribute = in_FK_Attribute;
	
}

Table_SearchToken_Attribute::Key::Key(Row_SearchToken_Attribute *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_SearchToken = pRow->m_FK_SearchToken;
pk_FK_Attribute = pRow->m_FK_Attribute;
	
}		

bool Table_SearchToken_Attribute::Key_Less::operator()(const Table_SearchToken_Attribute::Key &key1, const Table_SearchToken_Attribute::Key &key2) const
{
			if (key1.pk_FK_SearchToken!=key2.pk_FK_SearchToken)
return key1.pk_FK_SearchToken<key2.pk_FK_SearchToken;
else
if (key1.pk_FK_Attribute!=key2.pk_FK_Attribute)
return key1.pk_FK_Attribute<key2.pk_FK_Attribute;
else
return false;	
}	

void Table_SearchToken_Attribute::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_SearchToken_Attribute*>::iterator i = addedRows.begin();
	
		Row_SearchToken_Attribute *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_SearchToken_asSQL()+", "+pRow->FK_Attribute_asSQL();

	
		string query = "insert into SearchToken_Attribute (FK_SearchToken, FK_Attribute) values ("+
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
	

	for (map<Key, Row_SearchToken_Attribute*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_SearchToken_Attribute* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_SearchToken[32];
sprintf(tmp_FK_SearchToken, "%li", key.pk_FK_SearchToken);

char tmp_FK_Attribute[32];
sprintf(tmp_FK_Attribute, "%li", key.pk_FK_Attribute);


string condition;
condition = condition + "FK_SearchToken=" + tmp_FK_SearchToken+" AND "+"FK_Attribute=" + tmp_FK_Attribute;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_SearchToken="+pRow->FK_SearchToken_asSQL()+", FK_Attribute="+pRow->FK_Attribute_asSQL();

	
		string query = "update SearchToken_Attribute set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_SearchToken_Attribute*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_SearchToken_Attribute*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_SearchToken[32];
sprintf(tmp_FK_SearchToken, "%li", key.pk_FK_SearchToken);

char tmp_FK_Attribute[32];
sprintf(tmp_FK_Attribute, "%li", key.pk_FK_Attribute);


string condition;
condition = condition + "FK_SearchToken=" + tmp_FK_SearchToken+" AND "+"FK_Attribute=" + tmp_FK_Attribute;

	
		string query = "delete from SearchToken_Attribute where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_SearchToken_Attribute::GetRows(string where_statement,vector<class Row_SearchToken_Attribute*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from SearchToken_Attribute " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from SearchToken_Attribute where " + where_statement;
		
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

		Row_SearchToken_Attribute *pRow = new Row_SearchToken_Attribute(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_SearchToken = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_SearchToken));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Attribute = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Attribute));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_SearchToken_Attribute::Key, Row_SearchToken_Attribute*, Table_SearchToken_Attribute::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_SearchToken_Attribute* Table_SearchToken_Attribute::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_SearchToken_Attribute *pRow = new Row_SearchToken_Attribute(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_SearchToken_Attribute* Table_SearchToken_Attribute::GetRow(long int in_FK_SearchToken, long int in_FK_Attribute)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_SearchToken, in_FK_Attribute);

	map<Key, Row_SearchToken_Attribute*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_SearchToken_Attribute* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_SearchToken_Attribute* Table_SearchToken_Attribute::FetchRow(Table_SearchToken_Attribute::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_SearchToken[32];
sprintf(tmp_FK_SearchToken, "%li", key.pk_FK_SearchToken);

char tmp_FK_Attribute[32];
sprintf(tmp_FK_Attribute, "%li", key.pk_FK_Attribute);


string condition;
condition = condition + "FK_SearchToken=" + tmp_FK_SearchToken+" AND "+"FK_Attribute=" + tmp_FK_Attribute;


	string query = "select * from SearchToken_Attribute where " + condition;		

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

	Row_SearchToken_Attribute *pRow = new Row_SearchToken_Attribute(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_SearchToken = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_SearchToken));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Attribute = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Attribute));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_SearchToken* Row_SearchToken_Attribute::FK_SearchToken_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SearchToken *pTable = table->database->SearchToken_get();
return pTable->GetRow(m_FK_SearchToken);
}
class Row_Attribute* Row_SearchToken_Attribute::FK_Attribute_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Attribute *pTable = table->database->Attribute_get();
return pTable->GetRow(m_FK_Attribute);
}





