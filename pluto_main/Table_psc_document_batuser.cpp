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
#include "Table_psc_document_batuser.h"
#include "Table_psc_document_bathdr.h"



void Database_pluto_main::CreateTable_psc_document_batuser()
{
	tblpsc_document_batuser = new Table_psc_document_batuser(this);
}

void Database_pluto_main::DeleteTable_psc_document_batuser()
{
	delete tblpsc_document_batuser;
}

Table_psc_document_batuser::~Table_psc_document_batuser()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_document_batuser *pRow = (Row_psc_document_batuser *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_document_batuser *pRow = (Row_psc_document_batuser *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_document_batuser::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_psc_document_batuser *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_document_batuser *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_document_batuser);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_document_batuser::Reload()
{
	Row_psc_document_batuser *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_document_batuser);
		Row_psc_document_batuser *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_document_batuser::Row_psc_document_batuser(Table_psc_document_batuser *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_document_batuser::SetDefaultValues()
{
	m_PK_psc_document_batuser = 0;
is_null[0] = false;
m_FK_psc_document_bathdr = 0;
is_null[1] = false;
m_psc_user = 0;
is_null[2] = false;
m_is_sup = 0;
is_null[3] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_document_batuser::PK_psc_document_batuser_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_psc_document_batuser;}
long int Row_psc_document_batuser::FK_psc_document_bathdr_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_psc_document_bathdr;}
long int Row_psc_document_batuser::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_psc_document_batuser::is_sup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_is_sup;}

		
void Row_psc_document_batuser::PK_psc_document_batuser_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_psc_document_batuser = val; is_modified=true; is_null[0]=false;}
void Row_psc_document_batuser::FK_psc_document_bathdr_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_psc_document_bathdr = val; is_modified=true; is_null[1]=false;}
void Row_psc_document_batuser::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[2]=false;}
void Row_psc_document_batuser::is_sup_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_is_sup = val; is_modified=true; is_null[3]=false;}

		

			
	

string Row_psc_document_batuser::PK_psc_document_batuser_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_document_batuser);

return buf;
}

string Row_psc_document_batuser::FK_psc_document_bathdr_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_document_bathdr);

return buf;
}

string Row_psc_document_batuser::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_psc_document_batuser::is_sup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_is_sup);

return buf;
}




Table_psc_document_batuser::Key::Key(long int in_PK_psc_document_batuser)
{
			pk_PK_psc_document_batuser = in_PK_psc_document_batuser;
	
}

Table_psc_document_batuser::Key::Key(Row_psc_document_batuser *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_psc_document_batuser = pRow->m_PK_psc_document_batuser;
	
}		

bool Table_psc_document_batuser::Key_Less::operator()(const Table_psc_document_batuser::Key &key1, const Table_psc_document_batuser::Key &key2) const
{
			if (key1.pk_PK_psc_document_batuser!=key2.pk_PK_psc_document_batuser)
return key1.pk_PK_psc_document_batuser<key2.pk_PK_psc_document_batuser;
else
return false;	
}	

bool Table_psc_document_batuser::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_document_batuser *pRow = (Row_psc_document_batuser *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_document_batuser_asSQL()+", "+pRow->FK_psc_document_bathdr_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->is_sup_asSQL();

	
		string query = "insert into psc_document_batuser (PK_psc_document_batuser, FK_psc_document_bathdr, psc_user, is_sup) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_psc_document_batuser=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_document_batuser);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_document_batuser* pRow = (Row_psc_document_batuser*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_document_batuser);

		char tmp_PK_psc_document_batuser[32];
sprintf(tmp_PK_psc_document_batuser, "%li", key.pk);


string condition;
condition = condition + "PK_psc_document_batuser=" + tmp_PK_psc_document_batuser;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_psc_document_batuser="+pRow->PK_psc_document_batuser_asSQL()+", FK_psc_document_bathdr="+pRow->FK_psc_document_bathdr_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", is_sup="+pRow->is_sup_asSQL();

	
		string query = "update psc_document_batuser set " + update_values_list + " where " + condition;
			
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
		Row_psc_document_batuser* pRow = (Row_psc_document_batuser*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_document_batuser* pRow = (Row_psc_document_batuser*) (*i).second;	

		char tmp_PK_psc_document_batuser[32];
sprintf(tmp_PK_psc_document_batuser, "%li", key.pk);


string condition;
condition = condition + "PK_psc_document_batuser=" + tmp_PK_psc_document_batuser;

	
		string query = "delete from psc_document_batuser where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_psc_document_batuser*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_psc_document_batuser::GetRows(string where_statement,vector<class Row_psc_document_batuser*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from psc_document_batuser " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from psc_document_batuser where " + where_statement;
		
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

		Row_psc_document_batuser *pRow = new Row_psc_document_batuser(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_document_batuser = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_document_batuser));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_document_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_document_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_user));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_is_sup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_is_sup));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_psc_document_batuser);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_document_batuser *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_psc_document_batuser* Table_psc_document_batuser::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_psc_document_batuser *pRow = new Row_psc_document_batuser(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_document_batuser* Table_psc_document_batuser::GetRow(long int in_PK_psc_document_batuser)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_psc_document_batuser);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_document_batuser*) (*i).second;
	//we have to fetch row
	Row_psc_document_batuser* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_document_batuser* Table_psc_document_batuser::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_psc_document_batuser[32];
sprintf(tmp_PK_psc_document_batuser, "%li", key.pk);


string condition;
condition = condition + "PK_psc_document_batuser=" + tmp_PK_psc_document_batuser;


	string query = "select * from psc_document_batuser where " + condition;		

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

	Row_psc_document_batuser *pRow = new Row_psc_document_batuser(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_document_batuser = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_document_batuser));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_document_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_document_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_user));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_is_sup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_is_sup));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_psc_document_bathdr* Row_psc_document_batuser::FK_psc_document_bathdr_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_psc_document_bathdr *pTable = table->database->psc_document_bathdr_get();
return pTable->GetRow(m_FK_psc_document_bathdr);
}





