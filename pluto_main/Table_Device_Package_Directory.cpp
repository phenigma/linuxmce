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
#include "Table_Device_Package_Directory.h"
#include "Table_Device.h"
#include "Table_Package_Directory.h"



void Database_pluto_main::CreateTable_Device_Package_Directory()
{
	tblDevice_Package_Directory = new Table_Device_Package_Directory(this);
}

void Database_pluto_main::DeleteTable_Device_Package_Directory()
{
	delete tblDevice_Package_Directory;
}

Table_Device_Package_Directory::~Table_Device_Package_Directory()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Device_Package_Directory *pRow = (Row_Device_Package_Directory *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Device_Package_Directory *pRow = (Row_Device_Package_Directory *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Device_Package_Directory::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Device_Package_Directory *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Device_Package_Directory *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_Package_Directory);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device_Package_Directory::Reload()
{
	Row_Device_Package_Directory *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_Package_Directory);
		Row_Device_Package_Directory *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Device_Package_Directory::Row_Device_Package_Directory(Table_Device_Package_Directory *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Device_Package_Directory::SetDefaultValues()
{
	m_FK_Device = 0;
is_null[0] = false;
m_FK_Package_Directory = 0;
is_null[1] = false;
is_null[2] = true;
m_Verified = 0;
is_null[3] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Device_Package_Directory::FK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device;}
long int Row_Device_Package_Directory::FK_Package_Directory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package_Directory;}
string Row_Device_Package_Directory::Path_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Path;}
short int Row_Device_Package_Directory::Verified_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Verified;}

		
void Row_Device_Package_Directory::FK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device = val; is_modified=true; is_null[0]=false;}
void Row_Device_Package_Directory::FK_Package_Directory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package_Directory = val; is_modified=true; is_null[1]=false;}
void Row_Device_Package_Directory::Path_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Path = val; is_modified=true; is_null[2]=false;}
void Row_Device_Package_Directory::Verified_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Verified = val; is_modified=true; is_null[3]=false;}

		
bool Row_Device_Package_Directory::Path_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_Device_Package_Directory::Path_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_Device_Package_Directory::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_Device_Package_Directory::FK_Package_Directory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package_Directory);

return buf;
}

string Row_Device_Package_Directory::Path_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Path.c_str(), (unsigned long) m_Path.size());
return string()+"\""+buf+"\"";
}

string Row_Device_Package_Directory::Verified_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Verified);

return buf;
}




Table_Device_Package_Directory::Key::Key(long int in_FK_Device, long int in_FK_Package_Directory)
{
			pk_FK_Device = in_FK_Device;
pk_FK_Package_Directory = in_FK_Package_Directory;
	
}

Table_Device_Package_Directory::Key::Key(Row_Device_Package_Directory *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Device = pRow->m_FK_Device;
pk_FK_Package_Directory = pRow->m_FK_Package_Directory;
	
}		

bool Table_Device_Package_Directory::Key_Less::operator()(const Table_Device_Package_Directory::Key &key1, const Table_Device_Package_Directory::Key &key2) const
{
			if (key1.pk_FK_Device!=key2.pk_FK_Device)
return key1.pk_FK_Device<key2.pk_FK_Device;
else
if (key1.pk_FK_Package_Directory!=key2.pk_FK_Package_Directory)
return key1.pk_FK_Package_Directory<key2.pk_FK_Package_Directory;
else
return false;	
}	

void Table_Device_Package_Directory::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Device_Package_Directory *pRow = (Row_Device_Package_Directory *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Device_asSQL()+", "+pRow->FK_Package_Directory_asSQL()+", "+pRow->Path_asSQL()+", "+pRow->Verified_asSQL();

	
		string query = "insert into Device_Package_Directory (FK_Device, FK_Package_Directory, Path, Verified) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_Package_Directory);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Device_Package_Directory* pRow = (Row_Device_Package_Directory*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_Package_Directory);

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk2);


string condition;
condition = condition + "FK_Device=" + tmp_FK_Device+" AND "+"FK_Package_Directory=" + tmp_FK_Package_Directory;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Device="+pRow->FK_Device_asSQL()+", FK_Package_Directory="+pRow->FK_Package_Directory_asSQL()+", Path="+pRow->Path_asSQL()+", Verified="+pRow->Verified_asSQL();

	
		string query = "update Device_Package_Directory set " + update_values_list + " where " + condition;
			
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
		Row_Device_Package_Directory* pRow = (Row_Device_Package_Directory*) (*i).second;	

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk2);


string condition;
condition = condition + "FK_Device=" + tmp_FK_Device+" AND "+"FK_Package_Directory=" + tmp_FK_Package_Directory;

	
		string query = "delete from Device_Package_Directory where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Device_Package_Directory::GetRows(string where_statement,vector<class Row_Device_Package_Directory*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Device_Package_Directory " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Device_Package_Directory where " + where_statement;
		
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

		Row_Device_Package_Directory *pRow = new Row_Device_Package_Directory(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package_Directory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package_Directory));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Path = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Verified = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Verified));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_Package_Directory);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Device_Package_Directory *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Device_Package_Directory* Table_Device_Package_Directory::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Device_Package_Directory *pRow = new Row_Device_Package_Directory(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Device_Package_Directory* Table_Device_Package_Directory::GetRow(long int in_FK_Device, long int in_FK_Package_Directory)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_Device, in_FK_Package_Directory);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Device_Package_Directory*) (*i).second;
	//we have to fetch row
	Row_Device_Package_Directory* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device_Package_Directory* Table_Device_Package_Directory::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk2);


string condition;
condition = condition + "FK_Device=" + tmp_FK_Device+" AND "+"FK_Package_Directory=" + tmp_FK_Package_Directory;


	string query = "select * from Device_Package_Directory where " + condition;		

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

	Row_Device_Package_Directory *pRow = new Row_Device_Package_Directory(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package_Directory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package_Directory));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Path = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Verified = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Verified));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Device* Row_Device_Package_Directory::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}
class Row_Package_Directory* Row_Device_Package_Directory::FK_Package_Directory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory *pTable = table->database->Package_Directory_get();
return pTable->GetRow(m_FK_Package_Directory);
}





