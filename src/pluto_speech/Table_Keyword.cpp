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
#include "Table_Keyword.h"



void Database_pluto_speech::CreateTable_Keyword()
{
	tblKeyword = new Table_Keyword(this);
}

void Database_pluto_speech::DeleteTable_Keyword()
{
	delete tblKeyword;
}

Table_Keyword::~Table_Keyword()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Keyword *pRow = (Row_Keyword *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Keyword *pRow = (Row_Keyword *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Keyword::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Keyword *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Keyword *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Keyword);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Keyword::Reload()
{
	Row_Keyword *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Keyword);
		Row_Keyword *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Keyword::Row_Keyword(Table_Keyword *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Keyword::SetDefaultValues()
{
	m_PK_Keyword = 0;
is_null[0] = false;
m_SimpleAdvanced = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Keyword::PK_Keyword_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Keyword;}
short int Row_Keyword::SimpleAdvanced_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SimpleAdvanced;}
long int Row_Keyword::EK_Room_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_Room;}
string Row_Keyword::Keyword_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Keyword;}
long int Row_Keyword::EK_CommandGroup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_CommandGroup;}

		
void Row_Keyword::PK_Keyword_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Keyword = val; is_modified=true; is_null[0]=false;}
void Row_Keyword::SimpleAdvanced_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SimpleAdvanced = val; is_modified=true; is_null[1]=false;}
void Row_Keyword::EK_Room_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_Room = val; is_modified=true; is_null[2]=false;}
void Row_Keyword::Keyword_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Keyword = val; is_modified=true; is_null[3]=false;}
void Row_Keyword::EK_CommandGroup_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_CommandGroup = val; is_modified=true; is_null[4]=false;}

		
bool Row_Keyword::SimpleAdvanced_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Keyword::EK_Room_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Keyword::Keyword_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Keyword::EK_CommandGroup_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}

			
void Row_Keyword::SimpleAdvanced_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Keyword::EK_Room_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Keyword::Keyword_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Keyword::EK_CommandGroup_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
	

string Row_Keyword::PK_Keyword_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Keyword);

return buf;
}

string Row_Keyword::SimpleAdvanced_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_SimpleAdvanced);

return buf;
}

string Row_Keyword::EK_Room_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Room);

return buf;
}

string Row_Keyword::Keyword_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Keyword.c_str(), (unsigned long) m_Keyword.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Keyword::EK_CommandGroup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_CommandGroup);

return buf;
}




Table_Keyword::Key::Key(long int in_PK_Keyword)
{
			pk_PK_Keyword = in_PK_Keyword;
	
}

Table_Keyword::Key::Key(Row_Keyword *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Keyword = pRow->m_PK_Keyword;
	
}		

bool Table_Keyword::Key_Less::operator()(const Table_Keyword::Key &key1, const Table_Keyword::Key &key2) const
{
			if (key1.pk_PK_Keyword!=key2.pk_PK_Keyword)
return key1.pk_PK_Keyword<key2.pk_PK_Keyword;
else
return false;	
}	

bool Table_Keyword::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Keyword *pRow = (Row_Keyword *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Keyword_asSQL()+", "+pRow->SimpleAdvanced_asSQL()+", "+pRow->EK_Room_asSQL()+", "+pRow->Keyword_asSQL()+", "+pRow->EK_CommandGroup_asSQL();

	
		string query = "insert into Keyword (PK_Keyword, SimpleAdvanced, EK_Room, Keyword, EK_CommandGroup) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Keyword);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Keyword* pRow = (Row_Keyword*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Keyword);

		char tmp_PK_Keyword[32];
sprintf(tmp_PK_Keyword, "%li", key.pk);


string condition;
condition = condition + "PK_Keyword=" + tmp_PK_Keyword;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Keyword="+pRow->PK_Keyword_asSQL()+", SimpleAdvanced="+pRow->SimpleAdvanced_asSQL()+", EK_Room="+pRow->EK_Room_asSQL()+", Keyword="+pRow->Keyword_asSQL()+", EK_CommandGroup="+pRow->EK_CommandGroup_asSQL();

	
		string query = "update Keyword set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Keyword* pRow = (Row_Keyword*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Keyword* pRow = (Row_Keyword*) (*i).second;	

		char tmp_PK_Keyword[32];
sprintf(tmp_PK_Keyword, "%li", key.pk);


string condition;
condition = condition + "PK_Keyword=" + tmp_PK_Keyword;

	
		string query = "delete from Keyword where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_Keyword*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Keyword::GetRows(string where_statement,vector<class Row_Keyword*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Keyword " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Keyword where " + where_statement;
		
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

		Row_Keyword *pRow = new Row_Keyword(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Keyword = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Keyword));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_SimpleAdvanced = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_SimpleAdvanced));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Room = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Room));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Keyword = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Keyword = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_CommandGroup = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_CommandGroup));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Keyword);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Keyword *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Keyword* Table_Keyword::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Keyword *pRow = new Row_Keyword(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Keyword* Table_Keyword::GetRow(long int in_PK_Keyword)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Keyword);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Keyword*) (*i).second;
	//we have to fetch row
	Row_Keyword* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Keyword* Table_Keyword::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Keyword[32];
sprintf(tmp_PK_Keyword, "%li", key.pk);


string condition;
condition = condition + "PK_Keyword=" + tmp_PK_Keyword;


	string query = "select * from Keyword where " + condition;		

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

	Row_Keyword *pRow = new Row_Keyword(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Keyword = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Keyword));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_SimpleAdvanced = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_SimpleAdvanced));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Room = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Room));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Keyword = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Keyword = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_CommandGroup = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_CommandGroup));
}



	mysql_free_result(res);			
	
	return pRow;						
}







