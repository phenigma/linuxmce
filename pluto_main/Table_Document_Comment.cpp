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
#include "Table_Document_Comment.h"
#include "Table_Document.h"



void Database_pluto_main::CreateTable_Document_Comment()
{
	tblDocument_Comment = new Table_Document_Comment(this);
}

void Database_pluto_main::DeleteTable_Document_Comment()
{
	delete tblDocument_Comment;
}

Table_Document_Comment::~Table_Document_Comment()
{
	map<Table_Document_Comment::Key, class Row_Document_Comment*, Table_Document_Comment::Key_Less>::iterator it;
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


void Row_Document_Comment::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Document_Comment*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Document_Comment::Key key(this);					
			map<Table_Document_Comment::Key, Row_Document_Comment*, Table_Document_Comment::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Document_Comment::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Document_Comment::Key key(this);		
		Row_Document_Comment *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Document_Comment::Row_Document_Comment(Table_Document_Comment *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Document_Comment::SetDefaultValues()
{
	m_PK_Document_Comment = 0;
is_null[0] = false;
m_FK_Document = 0;
is_null[1] = false;
m_FK_MasterUsers = 0;
is_null[2] = false;
m_Comment = "";
is_null[3] = false;
m_Date = "000000000000";
is_null[4] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Document_Comment::PK_Document_Comment_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Document_Comment;}
long int Row_Document_Comment::FK_Document_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Document;}
long int Row_Document_Comment::FK_MasterUsers_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_MasterUsers;}
string Row_Document_Comment::Comment_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Comment;}
string Row_Document_Comment::Date_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Date;}

		
void Row_Document_Comment::PK_Document_Comment_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Document_Comment = val; is_modified=true; is_null[0]=false;}
void Row_Document_Comment::FK_Document_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Document = val; is_modified=true; is_null[1]=false;}
void Row_Document_Comment::FK_MasterUsers_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_MasterUsers = val; is_modified=true; is_null[2]=false;}
void Row_Document_Comment::Comment_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Comment = val; is_modified=true; is_null[3]=false;}
void Row_Document_Comment::Date_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Date = val; is_modified=true; is_null[4]=false;}

		

			
	

string Row_Document_Comment::PK_Document_Comment_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Document_Comment);

return buf;
}

string Row_Document_Comment::FK_Document_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Document);

return buf;
}

string Row_Document_Comment::FK_MasterUsers_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_MasterUsers);

return buf;
}

string Row_Document_Comment::Comment_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Comment.c_str(), (unsigned long) m_Comment.size());
return string()+"\""+buf+"\"";
}

string Row_Document_Comment::Date_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[25];
mysql_real_escape_string(table->database->db_handle, buf, m_Date.c_str(), (unsigned long) m_Date.size());
return string()+"\""+buf+"\"";
}




Table_Document_Comment::Key::Key(long int in_PK_Document_Comment)
{
			pk_PK_Document_Comment = in_PK_Document_Comment;
	
}

Table_Document_Comment::Key::Key(Row_Document_Comment *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Document_Comment = pRow->m_PK_Document_Comment;
	
}		

bool Table_Document_Comment::Key_Less::operator()(const Table_Document_Comment::Key &key1, const Table_Document_Comment::Key &key2) const
{
			if (key1.pk_PK_Document_Comment!=key2.pk_PK_Document_Comment)
return key1.pk_PK_Document_Comment<key2.pk_PK_Document_Comment;
else
return false;	
}	

void Table_Document_Comment::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Document_Comment*>::iterator i = addedRows.begin();
	
		Row_Document_Comment *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Document_Comment_asSQL()+", "+pRow->FK_Document_asSQL()+", "+pRow->FK_MasterUsers_asSQL()+", "+pRow->Comment_asSQL()+", "+pRow->Date_asSQL();

	
		string query = "insert into Document_Comment (PK_Document_Comment, FK_Document, FK_MasterUsers, Comment, Date) values ("+
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
	

	for (map<Key, Row_Document_Comment*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Document_Comment* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Document_Comment[32];
sprintf(tmp_PK_Document_Comment, "%li", key.pk_PK_Document_Comment);


string condition;
condition = condition + "PK_Document_Comment=" + tmp_PK_Document_Comment;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Document_Comment="+pRow->PK_Document_Comment_asSQL()+", FK_Document="+pRow->FK_Document_asSQL()+", FK_MasterUsers="+pRow->FK_MasterUsers_asSQL()+", Comment="+pRow->Comment_asSQL()+", Date="+pRow->Date_asSQL();

	
		string query = "update Document_Comment set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Document_Comment*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Document_Comment*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Document_Comment[32];
sprintf(tmp_PK_Document_Comment, "%li", key.pk_PK_Document_Comment);


string condition;
condition = condition + "PK_Document_Comment=" + tmp_PK_Document_Comment;

	
		string query = "delete from Document_Comment where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Document_Comment::GetRows(string where_statement,vector<class Row_Document_Comment*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Document_Comment " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Document_Comment where " + where_statement;
		
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

		Row_Document_Comment *pRow = new Row_Document_Comment(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Document_Comment = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Document_Comment));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Document));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_MasterUsers = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_MasterUsers));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Comment = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Comment = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Date = string(row[4],lengths[4]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Document_Comment::Key, Row_Document_Comment*, Table_Document_Comment::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Document_Comment* Table_Document_Comment::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Document_Comment *pRow = new Row_Document_Comment(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Document_Comment* Table_Document_Comment::GetRow(long int in_PK_Document_Comment)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Document_Comment);

	map<Key, Row_Document_Comment*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Document_Comment* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Document_Comment* Table_Document_Comment::FetchRow(Table_Document_Comment::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Document_Comment[32];
sprintf(tmp_PK_Document_Comment, "%li", key.pk_PK_Document_Comment);


string condition;
condition = condition + "PK_Document_Comment=" + tmp_PK_Document_Comment;


	string query = "select * from Document_Comment where " + condition;		

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

	Row_Document_Comment *pRow = new Row_Document_Comment(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Document_Comment = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Document_Comment));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Document));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_MasterUsers = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_MasterUsers));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Comment = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Comment = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Date = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Document* Row_Document_Comment::FK_Document_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document);
}





