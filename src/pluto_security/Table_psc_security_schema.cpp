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
#include "Table_psc_security_schema.h"



void Database_pluto_security::CreateTable_psc_security_schema()
{
	tblpsc_security_schema = new Table_psc_security_schema(this);
}

void Database_pluto_security::DeleteTable_psc_security_schema()
{
	delete tblpsc_security_schema;
}

Table_psc_security_schema::~Table_psc_security_schema()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_security_schema *pRow = (Row_psc_security_schema *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_security_schema *pRow = (Row_psc_security_schema *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_security_schema::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_psc_security_schema *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_security_schema *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_security_schema);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_security_schema::Reload()
{
	Row_psc_security_schema *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_security_schema);
		Row_psc_security_schema *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_security_schema::Row_psc_security_schema(Table_psc_security_schema *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_security_schema::SetDefaultValues()
{
	m_PK_psc_security_schema = 0;
is_null[0] = false;
m_Value = "";
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_security_schema::PK_psc_security_schema_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_psc_security_schema;}
string Row_psc_security_schema::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}

		
void Row_psc_security_schema::PK_psc_security_schema_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_psc_security_schema = val; is_modified=true; is_null[0]=false;}
void Row_psc_security_schema::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_psc_security_schema::PK_psc_security_schema_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_security_schema);

return buf;
}

string Row_psc_security_schema::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_psc_security_schema::Key::Key(long int in_PK_psc_security_schema)
{
			pk_PK_psc_security_schema = in_PK_psc_security_schema;
	
}

Table_psc_security_schema::Key::Key(Row_psc_security_schema *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_psc_security_schema = pRow->m_PK_psc_security_schema;
	
}		

bool Table_psc_security_schema::Key_Less::operator()(const Table_psc_security_schema::Key &key1, const Table_psc_security_schema::Key &key2) const
{
			if (key1.pk_PK_psc_security_schema!=key2.pk_PK_psc_security_schema)
return key1.pk_PK_psc_security_schema<key2.pk_PK_psc_security_schema;
else
return false;	
}	

bool Table_psc_security_schema::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_security_schema *pRow = (Row_psc_security_schema *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_security_schema_asSQL()+", "+pRow->Value_asSQL();

	
		string query = "insert into psc_security_schema (`PK_psc_security_schema`, `Value`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_psc_security_schema=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_security_schema);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_security_schema* pRow = (Row_psc_security_schema*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_security_schema);

		char tmp_PK_psc_security_schema[32];
sprintf(tmp_PK_psc_security_schema, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_security_schema`=" + tmp_PK_psc_security_schema;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_psc_security_schema`="+pRow->PK_psc_security_schema_asSQL()+", `Value`="+pRow->Value_asSQL();

	
		string query = "update psc_security_schema set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_psc_security_schema* pRow = (Row_psc_security_schema*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_security_schema* pRow = (Row_psc_security_schema*) (*i).second;	

		char tmp_PK_psc_security_schema[32];
sprintf(tmp_PK_psc_security_schema, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_security_schema`=" + tmp_PK_psc_security_schema;

	
		string query = "delete from psc_security_schema where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_psc_security_schema*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_psc_security_schema::GetRows(string where_statement,vector<class Row_psc_security_schema*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from psc_security_schema " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from psc_security_schema where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_psc_security_schema *pRow = new Row_psc_security_schema(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_security_schema = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_security_schema));
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

		SingleLongKey key(pRow->m_PK_psc_security_schema);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_security_schema *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_psc_security_schema* Table_psc_security_schema::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_psc_security_schema *pRow = new Row_psc_security_schema(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_security_schema* Table_psc_security_schema::GetRow(long int in_PK_psc_security_schema)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_psc_security_schema);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_security_schema*) (*i).second;
	//we have to fetch row
	Row_psc_security_schema* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_security_schema* Table_psc_security_schema::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_psc_security_schema[32];
sprintf(tmp_PK_psc_security_schema, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_security_schema`=" + tmp_PK_psc_security_schema;


	string query = "select * from psc_security_schema where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

	Row_psc_security_schema *pRow = new Row_psc_security_schema(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_security_schema = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_security_schema));
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







