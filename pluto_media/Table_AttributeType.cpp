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
#include "Table_AttributeType.h"

#include "Table_Attribute.h"
#include "Table_Type_AttributeType.h"


void Database_pluto_media::CreateTable_AttributeType()
{
	tblAttributeType = new Table_AttributeType(this);
}

void Database_pluto_media::DeleteTable_AttributeType()
{
	delete tblAttributeType;
}

Table_AttributeType::~Table_AttributeType()
{
	map<Table_AttributeType::Key, class Row_AttributeType*, Table_AttributeType::Key_Less>::iterator it;
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


void Row_AttributeType::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_AttributeType*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_AttributeType::Key key(this);					
			map<Table_AttributeType::Key, Row_AttributeType*, Table_AttributeType::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_AttributeType::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_AttributeType::Key key(this);		
		Row_AttributeType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_AttributeType::Row_AttributeType(Table_AttributeType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_AttributeType::SetDefaultValues()
{
	m_PK_AttributeType = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_AttributeType::PK_AttributeType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_AttributeType;}
string Row_AttributeType::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_AttributeType::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}

		
void Row_AttributeType::PK_AttributeType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_AttributeType = val; is_modified=true; is_null[0]=false;}
void Row_AttributeType::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_AttributeType::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}

		
bool Row_AttributeType::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_AttributeType::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_AttributeType::PK_AttributeType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_AttributeType);

return buf;
}

string Row_AttributeType::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_AttributeType::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}




Table_AttributeType::Key::Key(long int in_PK_AttributeType)
{
			pk_PK_AttributeType = in_PK_AttributeType;
	
}

Table_AttributeType::Key::Key(Row_AttributeType *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_AttributeType = pRow->m_PK_AttributeType;
	
}		

bool Table_AttributeType::Key_Less::operator()(const Table_AttributeType::Key &key1, const Table_AttributeType::Key &key2) const
{
			if (key1.pk_PK_AttributeType!=key2.pk_PK_AttributeType)
return key1.pk_PK_AttributeType<key2.pk_PK_AttributeType;
else
return false;	
}	

void Table_AttributeType::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_AttributeType*>::iterator i = addedRows.begin();
	
		Row_AttributeType *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_AttributeType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL();

	
		string query = "insert into AttributeType (PK_AttributeType, Description, Define) values ("+
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
	

	for (map<Key, Row_AttributeType*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_AttributeType* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_AttributeType[32];
sprintf(tmp_PK_AttributeType, "%li", key.pk_PK_AttributeType);


string condition;
condition = condition + "PK_AttributeType=" + tmp_PK_AttributeType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_AttributeType="+pRow->PK_AttributeType_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL();

	
		string query = "update AttributeType set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_AttributeType*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_AttributeType*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_AttributeType[32];
sprintf(tmp_PK_AttributeType, "%li", key.pk_PK_AttributeType);


string condition;
condition = condition + "PK_AttributeType=" + tmp_PK_AttributeType;

	
		string query = "delete from AttributeType where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_AttributeType::GetRows(string where_statement,vector<class Row_AttributeType*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from AttributeType " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from AttributeType where " + where_statement;
		
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

		Row_AttributeType *pRow = new Row_AttributeType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_AttributeType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_AttributeType));
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
		
                map<Table_AttributeType::Key, Row_AttributeType*, Table_AttributeType::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_AttributeType* Table_AttributeType::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_AttributeType *pRow = new Row_AttributeType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_AttributeType* Table_AttributeType::GetRow(long int in_PK_AttributeType)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_AttributeType);

	map<Key, Row_AttributeType*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_AttributeType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_AttributeType* Table_AttributeType::FetchRow(Table_AttributeType::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_AttributeType[32];
sprintf(tmp_PK_AttributeType, "%li", key.pk_PK_AttributeType);


string condition;
condition = condition + "PK_AttributeType=" + tmp_PK_AttributeType;


	string query = "select * from AttributeType where " + condition;		

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

	Row_AttributeType *pRow = new Row_AttributeType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_AttributeType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_AttributeType));
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




void Row_AttributeType::Attribute_FK_AttributeType_getrows(vector <class Row_Attribute*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Attribute *pTable = table->database->Attribute_get();
pTable->GetRows("FK_AttributeType=" + StringUtils::itos(m_PK_AttributeType),rows);
}
void Row_AttributeType::Type_AttributeType_FK_AttributeType_getrows(vector <class Row_Type_AttributeType*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Type_AttributeType *pTable = table->database->Type_AttributeType_get();
pTable->GetRows("FK_AttributeType=" + StringUtils::itos(m_PK_AttributeType),rows);
}



