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
#include "Table_SearchToken.h"

#include "Table_SearchToken_Attribute.h"


void Database_pluto_media::CreateTable_SearchToken()
{
	tblSearchToken = new Table_SearchToken(this);
}

void Database_pluto_media::DeleteTable_SearchToken()
{
	delete tblSearchToken;
}

Table_SearchToken::~Table_SearchToken()
{
	map<Table_SearchToken::Key, class Row_SearchToken*, Table_SearchToken::Key_Less>::iterator it;
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


void Row_SearchToken::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_SearchToken*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_SearchToken::Key key(this);					
			map<Table_SearchToken::Key, Row_SearchToken*, Table_SearchToken::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_SearchToken::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_SearchToken::Key key(this);		
		Row_SearchToken *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_SearchToken::Row_SearchToken(Table_SearchToken *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_SearchToken::SetDefaultValues()
{
	m_PK_SearchToken = 0;
is_null[0] = false;
m_Token = "";
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_SearchToken::PK_SearchToken_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_SearchToken;}
string Row_SearchToken::Token_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Token;}

		
void Row_SearchToken::PK_SearchToken_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_SearchToken = val; is_modified=true; is_null[0]=false;}
void Row_SearchToken::Token_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Token = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_SearchToken::PK_SearchToken_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_SearchToken);

return buf;
}

string Row_SearchToken::Token_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Token.c_str(), (unsigned long) m_Token.size());
return string()+"\""+buf+"\"";
}




Table_SearchToken::Key::Key(long int in_PK_SearchToken)
{
			pk_PK_SearchToken = in_PK_SearchToken;
	
}

Table_SearchToken::Key::Key(Row_SearchToken *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_SearchToken = pRow->m_PK_SearchToken;
	
}		

bool Table_SearchToken::Key_Less::operator()(const Table_SearchToken::Key &key1, const Table_SearchToken::Key &key2) const
{
			if (key1.pk_PK_SearchToken!=key2.pk_PK_SearchToken)
return key1.pk_PK_SearchToken<key2.pk_PK_SearchToken;
else
return false;	
}	

void Table_SearchToken::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_SearchToken*>::iterator i = addedRows.begin();
	
		Row_SearchToken *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_SearchToken_asSQL()+", "+pRow->Token_asSQL();

	
		string query = "insert into SearchToken (PK_SearchToken, Token) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_SearchToken=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_SearchToken*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_SearchToken* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_SearchToken[32];
sprintf(tmp_PK_SearchToken, "%li", key.pk_PK_SearchToken);


string condition;
condition = condition + "PK_SearchToken=" + tmp_PK_SearchToken;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_SearchToken="+pRow->PK_SearchToken_asSQL()+", Token="+pRow->Token_asSQL();

	
		string query = "update SearchToken set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_SearchToken*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_SearchToken*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_SearchToken[32];
sprintf(tmp_PK_SearchToken, "%li", key.pk_PK_SearchToken);


string condition;
condition = condition + "PK_SearchToken=" + tmp_PK_SearchToken;

	
		string query = "delete from SearchToken where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_SearchToken::GetRows(string where_statement,vector<class Row_SearchToken*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from SearchToken " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from SearchToken where " + where_statement;
		
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

		Row_SearchToken *pRow = new Row_SearchToken(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_SearchToken = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_SearchToken));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Token = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Token = string(row[1],lengths[1]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_SearchToken::Key, Row_SearchToken*, Table_SearchToken::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_SearchToken* Table_SearchToken::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_SearchToken *pRow = new Row_SearchToken(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_SearchToken* Table_SearchToken::GetRow(long int in_PK_SearchToken)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_SearchToken);

	map<Key, Row_SearchToken*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_SearchToken* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_SearchToken* Table_SearchToken::FetchRow(Table_SearchToken::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_SearchToken[32];
sprintf(tmp_PK_SearchToken, "%li", key.pk_PK_SearchToken);


string condition;
condition = condition + "PK_SearchToken=" + tmp_PK_SearchToken;


	string query = "select * from SearchToken where " + condition;		

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

	Row_SearchToken *pRow = new Row_SearchToken(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_SearchToken = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_SearchToken));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Token = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Token = string(row[1],lengths[1]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_SearchToken::SearchToken_Attribute_FK_SearchToken_getrows(vector <class Row_SearchToken_Attribute*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SearchToken_Attribute *pTable = table->database->SearchToken_Attribute_get();
pTable->GetRows("FK_SearchToken=" + StringUtils::itos(m_PK_SearchToken),rows);
}



