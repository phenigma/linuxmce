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
#include "Table_Type.h"

#include "Table_File.h"
#include "Table_Type_AttributeType.h"
#include "Table_Type_Extension.h"


void Database_pluto_media::CreateTable_Type()
{
	tblType = new Table_Type(this);
}

void Database_pluto_media::DeleteTable_Type()
{
	delete tblType;
}

Table_Type::~Table_Type()
{
	map<Table_Type::Key, class Row_Type*, Table_Type::Key_Less>::iterator it;
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


void Row_Type::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Type*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Type::Key key(this);					
			map<Table_Type::Key, Row_Type*, Table_Type::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Type::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Type::Key key(this);		
		Row_Type *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Type::Row_Type(Table_Type *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Type::SetDefaultValues()
{
	m_PK_Type = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Type::PK_Type_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Type;}
string Row_Type::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Type::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}

		
void Row_Type::PK_Type_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Type = val; is_modified=true; is_null[0]=false;}
void Row_Type::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Type::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}

		
bool Row_Type::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_Type::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_Type::PK_Type_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Type);

return buf;
}

string Row_Type::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Type::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}




Table_Type::Key::Key(long int in_PK_Type)
{
			pk_PK_Type = in_PK_Type;
	
}

Table_Type::Key::Key(Row_Type *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Type = pRow->m_PK_Type;
	
}		

bool Table_Type::Key_Less::operator()(const Table_Type::Key &key1, const Table_Type::Key &key2) const
{
			if (key1.pk_PK_Type!=key2.pk_PK_Type)
return key1.pk_PK_Type<key2.pk_PK_Type;
else
return false;	
}	

void Table_Type::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Type*>::iterator i = addedRows.begin();
	
		Row_Type *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Type_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL();

	
		string query = "insert into Type (PK_Type, Description, Define) values ("+
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
	

	for (map<Key, Row_Type*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Type* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Type[32];
sprintf(tmp_PK_Type, "%li", key.pk_PK_Type);


string condition;
condition = condition + "PK_Type=" + tmp_PK_Type;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Type="+pRow->PK_Type_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL();

	
		string query = "update Type set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Type*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Type*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Type[32];
sprintf(tmp_PK_Type, "%li", key.pk_PK_Type);


string condition;
condition = condition + "PK_Type=" + tmp_PK_Type;

	
		string query = "delete from Type where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Type::GetRows(string where_statement,vector<class Row_Type*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Type " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Type where " + where_statement;
		
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

		Row_Type *pRow = new Row_Type(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Type));
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



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Type::Key, Row_Type*, Table_Type::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Type* Table_Type::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Type *pRow = new Row_Type(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Type* Table_Type::GetRow(long int in_PK_Type)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Type);

	map<Key, Row_Type*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Type* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Type* Table_Type::FetchRow(Table_Type::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Type[32];
sprintf(tmp_PK_Type, "%li", key.pk_PK_Type);


string condition;
condition = condition + "PK_Type=" + tmp_PK_Type;


	string query = "select * from Type where " + condition;		

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

	Row_Type *pRow = new Row_Type(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Type));
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



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_Type::File_FK_Type_getrows(vector <class Row_File*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_File *pTable = table->database->File_get();
pTable->GetRows("FK_Type=" + StringUtils::itos(m_PK_Type),rows);
}
void Row_Type::Type_AttributeType_FK_Type_getrows(vector <class Row_Type_AttributeType*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Type_AttributeType *pTable = table->database->Type_AttributeType_get();
pTable->GetRows("FK_Type=" + StringUtils::itos(m_PK_Type),rows);
}
void Row_Type::Type_Extension_FK_Type_getrows(vector <class Row_Type_Extension*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Type_Extension *pTable = table->database->Type_Extension_get();
pTable->GetRows("FK_Type=" + StringUtils::itos(m_PK_Type),rows);
}



