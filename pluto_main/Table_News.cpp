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
#include "Table_News.h"
#include "Table_Package.h"



void Database_pluto_main::CreateTable_News()
{
	tblNews = new Table_News(this);
}

void Database_pluto_main::DeleteTable_News()
{
	delete tblNews;
}

Table_News::~Table_News()
{
	map<Table_News::Key, class Row_News*, Table_News::Key_Less>::iterator it;
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


void Row_News::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_News*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_News::Key key(this);					
			map<Table_News::Key, Row_News*, Table_News::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_News::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_News::Key key(this);		
		Row_News *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_News::Row_News(Table_News *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_News::SetDefaultValues()
{
	m_PK_News = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_News::PK_News_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_News;}
string Row_News::Date_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Date;}
long int Row_News::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
string Row_News::Title_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Title;}
string Row_News::ShortSummary_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ShortSummary;}
string Row_News::FullText_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FullText;}

		
void Row_News::PK_News_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_News = val; is_modified=true; is_null[0]=false;}
void Row_News::Date_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Date = val; is_modified=true; is_null[1]=false;}
void Row_News::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[2]=false;}
void Row_News::Title_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Title = val; is_modified=true; is_null[3]=false;}
void Row_News::ShortSummary_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ShortSummary = val; is_modified=true; is_null[4]=false;}
void Row_News::FullText_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FullText = val; is_modified=true; is_null[5]=false;}

		
bool Row_News::Date_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_News::FK_Package_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_News::Title_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_News::ShortSummary_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_News::FullText_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_News::Date_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_News::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_News::Title_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_News::ShortSummary_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_News::FullText_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_News::PK_News_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_News);

return buf;
}

string Row_News::Date_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[21];
mysql_real_escape_string(table->database->db_handle, buf, m_Date.c_str(), (unsigned long) m_Date.size());
return string()+"\""+buf+"\"";
}

string Row_News::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_News::Title_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Title.c_str(), (unsigned long) m_Title.size());
return string()+"\""+buf+"\"";
}

string Row_News::ShortSummary_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[511];
mysql_real_escape_string(table->database->db_handle, buf, m_ShortSummary.c_str(), (unsigned long) m_ShortSummary.size());
return string()+"\""+buf+"\"";
}

string Row_News::FullText_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_FullText.c_str(), (unsigned long) m_FullText.size());
return string()+"\""+buf+"\"";
}




Table_News::Key::Key(long int in_PK_News)
{
			pk_PK_News = in_PK_News;
	
}

Table_News::Key::Key(Row_News *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_News = pRow->m_PK_News;
	
}		

bool Table_News::Key_Less::operator()(const Table_News::Key &key1, const Table_News::Key &key2) const
{
			if (key1.pk_PK_News!=key2.pk_PK_News)
return key1.pk_PK_News<key2.pk_PK_News;
else
return false;	
}	

void Table_News::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_News*>::iterator i = addedRows.begin();
	
		Row_News *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_News_asSQL()+", "+pRow->Date_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->Title_asSQL()+", "+pRow->ShortSummary_asSQL()+", "+pRow->FullText_asSQL();

	
		string query = "insert into News (PK_News, Date, FK_Package, Title, ShortSummary, FullText) values ("+
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
	

	for (map<Key, Row_News*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_News* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_News[32];
sprintf(tmp_PK_News, "%li", key.pk_PK_News);


string condition;
condition = condition + "PK_News=" + tmp_PK_News;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_News="+pRow->PK_News_asSQL()+", Date="+pRow->Date_asSQL()+", FK_Package="+pRow->FK_Package_asSQL()+", Title="+pRow->Title_asSQL()+", ShortSummary="+pRow->ShortSummary_asSQL()+", FullText="+pRow->FullText_asSQL();

	
		string query = "update News set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_News*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_News*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_News[32];
sprintf(tmp_PK_News, "%li", key.pk_PK_News);


string condition;
condition = condition + "PK_News=" + tmp_PK_News;

	
		string query = "delete from News where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_News::GetRows(string where_statement,vector<class Row_News*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from News " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from News where " + where_statement;
		
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

		Row_News *pRow = new Row_News(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_News = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_News));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Date = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Package));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Title = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ShortSummary = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ShortSummary = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FullText = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_FullText = string(row[5],lengths[5]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_News::Key, Row_News*, Table_News::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_News* Table_News::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_News *pRow = new Row_News(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_News* Table_News::GetRow(long int in_PK_News)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_News);

	map<Key, Row_News*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_News* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_News* Table_News::FetchRow(Table_News::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_News[32];
sprintf(tmp_PK_News, "%li", key.pk_PK_News);


string condition;
condition = condition + "PK_News=" + tmp_PK_News;


	string query = "select * from News where " + condition;		

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

	Row_News *pRow = new Row_News(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_News = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_News));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Date = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Package));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Title = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ShortSummary = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ShortSummary = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FullText = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_FullText = string(row[5],lengths[5]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_News::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}





