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
#include "Table_ModeChange.h"



void Database_pluto_security::CreateTable_ModeChange()
{
	tblModeChange = new Table_ModeChange(this);
}

void Database_pluto_security::DeleteTable_ModeChange()
{
	delete tblModeChange;
}

Table_ModeChange::~Table_ModeChange()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_ModeChange *pRow = (Row_ModeChange *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_ModeChange *pRow = (Row_ModeChange *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_ModeChange::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_ModeChange *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_ModeChange *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_ModeChange);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_ModeChange::Reload()
{
	Row_ModeChange *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_ModeChange);
		Row_ModeChange *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_ModeChange::Row_ModeChange(Table_ModeChange *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_ModeChange::SetDefaultValues()
{
	m_PK_ModeChange = 0;
is_null[0] = false;
m_EK_HouseMode = 0;
is_null[1] = false;
m_ChangeTime = "0000-00-00 00:00:00";
is_null[2] = false;
m_EK_Users = 0;
is_null[3] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_ModeChange::PK_ModeChange_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_ModeChange;}
long int Row_ModeChange::EK_HouseMode_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_HouseMode;}
string Row_ModeChange::ChangeTime_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ChangeTime;}
long int Row_ModeChange::EK_Users_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_Users;}

		
void Row_ModeChange::PK_ModeChange_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_ModeChange = val; is_modified=true; is_null[0]=false;}
void Row_ModeChange::EK_HouseMode_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_HouseMode = val; is_modified=true; is_null[1]=false;}
void Row_ModeChange::ChangeTime_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ChangeTime = val; is_modified=true; is_null[2]=false;}
void Row_ModeChange::EK_Users_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_Users = val; is_modified=true; is_null[3]=false;}

		

			
	

string Row_ModeChange::PK_ModeChange_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_ModeChange);

return buf;
}

string Row_ModeChange::EK_HouseMode_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_HouseMode);

return buf;
}

string Row_ModeChange::ChangeTime_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[39];
mysql_real_escape_string(table->database->db_handle, buf, m_ChangeTime.c_str(), (unsigned long) m_ChangeTime.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_ModeChange::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}




Table_ModeChange::Key::Key(long int in_PK_ModeChange)
{
			pk_PK_ModeChange = in_PK_ModeChange;
	
}

Table_ModeChange::Key::Key(Row_ModeChange *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_ModeChange = pRow->m_PK_ModeChange;
	
}		

bool Table_ModeChange::Key_Less::operator()(const Table_ModeChange::Key &key1, const Table_ModeChange::Key &key2) const
{
			if (key1.pk_PK_ModeChange!=key2.pk_PK_ModeChange)
return key1.pk_PK_ModeChange<key2.pk_PK_ModeChange;
else
return false;	
}	

bool Table_ModeChange::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_ModeChange *pRow = (Row_ModeChange *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_ModeChange_asSQL()+", "+pRow->EK_HouseMode_asSQL()+", "+pRow->ChangeTime_asSQL()+", "+pRow->EK_Users_asSQL();

	
		string query = "insert into ModeChange (PK_ModeChange, EK_HouseMode, ChangeTime, EK_Users) values ("+
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
pRow->m_PK_ModeChange=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_ModeChange);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_ModeChange* pRow = (Row_ModeChange*) (*i).second;	
		SingleLongKey key(pRow->m_PK_ModeChange);

		char tmp_PK_ModeChange[32];
sprintf(tmp_PK_ModeChange, "%li", key.pk);


string condition;
condition = condition + "PK_ModeChange=" + tmp_PK_ModeChange;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_ModeChange="+pRow->PK_ModeChange_asSQL()+", EK_HouseMode="+pRow->EK_HouseMode_asSQL()+", ChangeTime="+pRow->ChangeTime_asSQL()+", EK_Users="+pRow->EK_Users_asSQL();

	
		string query = "update ModeChange set " + update_values_list + " where " + condition;
			
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
		Row_ModeChange* pRow = (Row_ModeChange*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_ModeChange* pRow = (Row_ModeChange*) (*i).second;	

		char tmp_PK_ModeChange[32];
sprintf(tmp_PK_ModeChange, "%li", key.pk);


string condition;
condition = condition + "PK_ModeChange=" + tmp_PK_ModeChange;

	
		string query = "delete from ModeChange where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_ModeChange*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_ModeChange::GetRows(string where_statement,vector<class Row_ModeChange*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from ModeChange " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from ModeChange where " + where_statement;
		
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

		Row_ModeChange *pRow = new Row_ModeChange(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ModeChange = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ModeChange));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_EK_HouseMode = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_EK_HouseMode));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ChangeTime = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ChangeTime = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_EK_Users));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_ModeChange);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_ModeChange *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_ModeChange* Table_ModeChange::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_ModeChange *pRow = new Row_ModeChange(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_ModeChange* Table_ModeChange::GetRow(long int in_PK_ModeChange)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_ModeChange);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_ModeChange*) (*i).second;
	//we have to fetch row
	Row_ModeChange* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_ModeChange* Table_ModeChange::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_ModeChange[32];
sprintf(tmp_PK_ModeChange, "%li", key.pk);


string condition;
condition = condition + "PK_ModeChange=" + tmp_PK_ModeChange;


	string query = "select * from ModeChange where " + condition;		

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

	Row_ModeChange *pRow = new Row_ModeChange(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ModeChange = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ModeChange));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_EK_HouseMode = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_EK_HouseMode));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ChangeTime = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ChangeTime = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_EK_Users));
}



	mysql_free_result(res);			
	
	return pRow;						
}







