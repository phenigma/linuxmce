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
#include "Table_Type_AttributeType.h"
#include "Table_Type.h"
#include "Table_AttributeType.h"



void Database_pluto_media::CreateTable_Type_AttributeType()
{
	tblType_AttributeType = new Table_Type_AttributeType(this);
}

void Database_pluto_media::DeleteTable_Type_AttributeType()
{
	delete tblType_AttributeType;
}

Table_Type_AttributeType::~Table_Type_AttributeType()
{
	map<Table_Type_AttributeType::Key, class Row_Type_AttributeType*, Table_Type_AttributeType::Key_Less>::iterator it;
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


void Row_Type_AttributeType::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Type_AttributeType*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Type_AttributeType::Key key(this);					
			map<Table_Type_AttributeType::Key, Row_Type_AttributeType*, Table_Type_AttributeType::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Type_AttributeType::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Type_AttributeType::Key key(this);		
		Row_Type_AttributeType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Type_AttributeType::Row_Type_AttributeType(Table_Type_AttributeType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Type_AttributeType::SetDefaultValues()
{
	m_FK_Type = 0;
is_null[0] = false;
m_FK_AttributeType = 0;
is_null[1] = false;
m_Identifier = 0;
is_null[2] = false;
m_CombineAsOne = 0;
is_null[3] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Type_AttributeType::FK_Type_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Type;}
long int Row_Type_AttributeType::FK_AttributeType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_AttributeType;}
short int Row_Type_AttributeType::Identifier_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Identifier;}
short int Row_Type_AttributeType::CombineAsOne_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_CombineAsOne;}

		
void Row_Type_AttributeType::FK_Type_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Type = val; is_modified=true; is_null[0]=false;}
void Row_Type_AttributeType::FK_AttributeType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_AttributeType = val; is_modified=true; is_null[1]=false;}
void Row_Type_AttributeType::Identifier_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Identifier = val; is_modified=true; is_null[2]=false;}
void Row_Type_AttributeType::CombineAsOne_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_CombineAsOne = val; is_modified=true; is_null[3]=false;}

		
bool Row_Type_AttributeType::CombineAsOne_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}

			
void Row_Type_AttributeType::CombineAsOne_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
	

string Row_Type_AttributeType::FK_Type_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Type);

return buf;
}

string Row_Type_AttributeType::FK_AttributeType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_AttributeType);

return buf;
}

string Row_Type_AttributeType::Identifier_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Identifier);

return buf;
}

string Row_Type_AttributeType::CombineAsOne_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CombineAsOne);

return buf;
}




Table_Type_AttributeType::Key::Key(long int in_FK_Type, long int in_FK_AttributeType)
{
			pk_FK_Type = in_FK_Type;
pk_FK_AttributeType = in_FK_AttributeType;
	
}

Table_Type_AttributeType::Key::Key(Row_Type_AttributeType *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Type = pRow->m_FK_Type;
pk_FK_AttributeType = pRow->m_FK_AttributeType;
	
}		

bool Table_Type_AttributeType::Key_Less::operator()(const Table_Type_AttributeType::Key &key1, const Table_Type_AttributeType::Key &key2) const
{
			if (key1.pk_FK_Type!=key2.pk_FK_Type)
return key1.pk_FK_Type<key2.pk_FK_Type;
else
if (key1.pk_FK_AttributeType!=key2.pk_FK_AttributeType)
return key1.pk_FK_AttributeType<key2.pk_FK_AttributeType;
else
return false;	
}	

void Table_Type_AttributeType::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Type_AttributeType*>::iterator i = addedRows.begin();
	
		Row_Type_AttributeType *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Type_asSQL()+", "+pRow->FK_AttributeType_asSQL()+", "+pRow->Identifier_asSQL()+", "+pRow->CombineAsOne_asSQL();

	
		string query = "insert into Type_AttributeType (FK_Type, FK_AttributeType, Identifier, CombineAsOne) values ("+
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
	

	for (map<Key, Row_Type_AttributeType*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Type_AttributeType* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk_FK_AttributeType);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type+" AND "+"FK_AttributeType=" + tmp_FK_AttributeType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Type="+pRow->FK_Type_asSQL()+", FK_AttributeType="+pRow->FK_AttributeType_asSQL()+", Identifier="+pRow->Identifier_asSQL()+", CombineAsOne="+pRow->CombineAsOne_asSQL();

	
		string query = "update Type_AttributeType set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Type_AttributeType*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Type_AttributeType*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk_FK_AttributeType);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type+" AND "+"FK_AttributeType=" + tmp_FK_AttributeType;

	
		string query = "delete from Type_AttributeType where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Type_AttributeType::GetRows(string where_statement,vector<class Row_Type_AttributeType*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Type_AttributeType " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Type_AttributeType where " + where_statement;
		
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

		Row_Type_AttributeType *pRow = new Row_Type_AttributeType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Type));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AttributeType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Identifier = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Identifier));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_CombineAsOne = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_CombineAsOne));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Type_AttributeType::Key, Row_Type_AttributeType*, Table_Type_AttributeType::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Type_AttributeType* Table_Type_AttributeType::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Type_AttributeType *pRow = new Row_Type_AttributeType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Type_AttributeType* Table_Type_AttributeType::GetRow(long int in_FK_Type, long int in_FK_AttributeType)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Type, in_FK_AttributeType);

	map<Key, Row_Type_AttributeType*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Type_AttributeType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Type_AttributeType* Table_Type_AttributeType::FetchRow(Table_Type_AttributeType::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Type[32];
sprintf(tmp_FK_Type, "%li", key.pk_FK_Type);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk_FK_AttributeType);


string condition;
condition = condition + "FK_Type=" + tmp_FK_Type+" AND "+"FK_AttributeType=" + tmp_FK_AttributeType;


	string query = "select * from Type_AttributeType where " + condition;		

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

	Row_Type_AttributeType *pRow = new Row_Type_AttributeType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Type = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Type));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AttributeType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Identifier = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Identifier));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_CombineAsOne = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_CombineAsOne));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Type* Row_Type_AttributeType::FK_Type_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Type *pTable = table->database->Type_get();
return pTable->GetRow(m_FK_Type);
}
class Row_AttributeType* Row_Type_AttributeType::FK_AttributeType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_AttributeType *pTable = table->database->AttributeType_get();
return pTable->GetRow(m_FK_AttributeType);
}





