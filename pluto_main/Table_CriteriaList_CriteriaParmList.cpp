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
#include "Table_CriteriaList_CriteriaParmList.h"
#include "Table_CriteriaList.h"
#include "Table_CriteriaParmList.h"



void Database_pluto_main::CreateTable_CriteriaList_CriteriaParmList()
{
	tblCriteriaList_CriteriaParmList = new Table_CriteriaList_CriteriaParmList(this);
}

void Database_pluto_main::DeleteTable_CriteriaList_CriteriaParmList()
{
	delete tblCriteriaList_CriteriaParmList;
}

Table_CriteriaList_CriteriaParmList::~Table_CriteriaList_CriteriaParmList()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CriteriaList_CriteriaParmList *pRow = (Row_CriteriaList_CriteriaParmList *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CriteriaList_CriteriaParmList *pRow = (Row_CriteriaList_CriteriaParmList *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CriteriaList_CriteriaParmList::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_CriteriaList_CriteriaParmList *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CriteriaList_CriteriaParmList *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_CriteriaList,pRow->m_FK_CriteriaParmList);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CriteriaList_CriteriaParmList::Reload()
{
	Row_CriteriaList_CriteriaParmList *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_CriteriaList,pRow->m_FK_CriteriaParmList);
		Row_CriteriaList_CriteriaParmList *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CriteriaList_CriteriaParmList::Row_CriteriaList_CriteriaParmList(Table_CriteriaList_CriteriaParmList *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CriteriaList_CriteriaParmList::SetDefaultValues()
{
	m_FK_CriteriaList = 0;
is_null[0] = false;
m_FK_CriteriaParmList = 0;
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CriteriaList_CriteriaParmList::FK_CriteriaList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaList;}
long int Row_CriteriaList_CriteriaParmList::FK_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmList;}

		
void Row_CriteriaList_CriteriaParmList::FK_CriteriaList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaList = val; is_modified=true; is_null[0]=false;}
void Row_CriteriaList_CriteriaParmList::FK_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmList = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_CriteriaList_CriteriaParmList::FK_CriteriaList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaList);

return buf;
}

string Row_CriteriaList_CriteriaParmList::FK_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmList);

return buf;
}




Table_CriteriaList_CriteriaParmList::Key::Key(long int in_FK_CriteriaList, long int in_FK_CriteriaParmList)
{
			pk_FK_CriteriaList = in_FK_CriteriaList;
pk_FK_CriteriaParmList = in_FK_CriteriaParmList;
	
}

Table_CriteriaList_CriteriaParmList::Key::Key(Row_CriteriaList_CriteriaParmList *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_CriteriaList = pRow->m_FK_CriteriaList;
pk_FK_CriteriaParmList = pRow->m_FK_CriteriaParmList;
	
}		

bool Table_CriteriaList_CriteriaParmList::Key_Less::operator()(const Table_CriteriaList_CriteriaParmList::Key &key1, const Table_CriteriaList_CriteriaParmList::Key &key2) const
{
			if (key1.pk_FK_CriteriaList!=key2.pk_FK_CriteriaList)
return key1.pk_FK_CriteriaList<key2.pk_FK_CriteriaList;
else
if (key1.pk_FK_CriteriaParmList!=key2.pk_FK_CriteriaParmList)
return key1.pk_FK_CriteriaParmList<key2.pk_FK_CriteriaParmList;
else
return false;	
}	

void Table_CriteriaList_CriteriaParmList::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CriteriaList_CriteriaParmList *pRow = (Row_CriteriaList_CriteriaParmList *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_CriteriaList_asSQL()+", "+pRow->FK_CriteriaParmList_asSQL();

	
		string query = "insert into CriteriaList_CriteriaParmList (FK_CriteriaList, FK_CriteriaParmList) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_CriteriaList,pRow->m_FK_CriteriaParmList);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CriteriaList_CriteriaParmList* pRow = (Row_CriteriaList_CriteriaParmList*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_CriteriaList,pRow->m_FK_CriteriaParmList);

		char tmp_FK_CriteriaList[32];
sprintf(tmp_FK_CriteriaList, "%li", key.pk1);

char tmp_FK_CriteriaParmList[32];
sprintf(tmp_FK_CriteriaParmList, "%li", key.pk2);


string condition;
condition = condition + "FK_CriteriaList=" + tmp_FK_CriteriaList+" AND "+"FK_CriteriaParmList=" + tmp_FK_CriteriaParmList;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_CriteriaList="+pRow->FK_CriteriaList_asSQL()+", FK_CriteriaParmList="+pRow->FK_CriteriaParmList_asSQL();

	
		string query = "update CriteriaList_CriteriaParmList set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_CriteriaList_CriteriaParmList* pRow = (Row_CriteriaList_CriteriaParmList*) (*i).second;	

		char tmp_FK_CriteriaList[32];
sprintf(tmp_FK_CriteriaList, "%li", key.pk1);

char tmp_FK_CriteriaParmList[32];
sprintf(tmp_FK_CriteriaParmList, "%li", key.pk2);


string condition;
condition = condition + "FK_CriteriaList=" + tmp_FK_CriteriaList+" AND "+"FK_CriteriaParmList=" + tmp_FK_CriteriaParmList;

	
		string query = "delete from CriteriaList_CriteriaParmList where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CriteriaList_CriteriaParmList::GetRows(string where_statement,vector<class Row_CriteriaList_CriteriaParmList*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from CriteriaList_CriteriaParmList " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from CriteriaList_CriteriaParmList where " + where_statement;
		
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

		Row_CriteriaList_CriteriaParmList *pRow = new Row_CriteriaList_CriteriaParmList(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_CriteriaList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmList = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmList));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_CriteriaList,pRow->m_FK_CriteriaParmList);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CriteriaList_CriteriaParmList *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_CriteriaList_CriteriaParmList* Table_CriteriaList_CriteriaParmList::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CriteriaList_CriteriaParmList *pRow = new Row_CriteriaList_CriteriaParmList(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CriteriaList_CriteriaParmList* Table_CriteriaList_CriteriaParmList::GetRow(long int in_FK_CriteriaList, long int in_FK_CriteriaParmList)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_CriteriaList, in_FK_CriteriaParmList);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CriteriaList_CriteriaParmList*) (*i).second;
	//we have to fetch row
	Row_CriteriaList_CriteriaParmList* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CriteriaList_CriteriaParmList* Table_CriteriaList_CriteriaParmList::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_CriteriaList[32];
sprintf(tmp_FK_CriteriaList, "%li", key.pk1);

char tmp_FK_CriteriaParmList[32];
sprintf(tmp_FK_CriteriaParmList, "%li", key.pk2);


string condition;
condition = condition + "FK_CriteriaList=" + tmp_FK_CriteriaList+" AND "+"FK_CriteriaParmList=" + tmp_FK_CriteriaParmList;


	string query = "select * from CriteriaList_CriteriaParmList where " + condition;		

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

	Row_CriteriaList_CriteriaParmList *pRow = new Row_CriteriaList_CriteriaParmList(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_CriteriaList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmList = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmList));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CriteriaList* Row_CriteriaList_CriteriaParmList::FK_CriteriaList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaList *pTable = table->database->CriteriaList_get();
return pTable->GetRow(m_FK_CriteriaList);
}
class Row_CriteriaParmList* Row_CriteriaList_CriteriaParmList::FK_CriteriaParmList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmList *pTable = table->database->CriteriaParmList_get();
return pTable->GetRow(m_FK_CriteriaParmList);
}





