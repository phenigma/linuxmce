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
#include "Table_Package_Directory_File.h"
#include "Table_Package_Directory.h"
#include "Table_OperatingSystem.h"
#include "Table_Distro.h"



void Database_pluto_main::CreateTable_Package_Directory_File()
{
	tblPackage_Directory_File = new Table_Package_Directory_File(this);
}

void Database_pluto_main::DeleteTable_Package_Directory_File()
{
	delete tblPackage_Directory_File;
}

Table_Package_Directory_File::~Table_Package_Directory_File()
{
	map<Table_Package_Directory_File::Key, class Row_Package_Directory_File*, Table_Package_Directory_File::Key_Less>::iterator it;
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


void Row_Package_Directory_File::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Package_Directory_File*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Package_Directory_File::Key key(this);					
			map<Table_Package_Directory_File::Key, Row_Package_Directory_File*, Table_Package_Directory_File::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Package_Directory_File::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Package_Directory_File::Key key(this);		
		Row_Package_Directory_File *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Directory_File::Row_Package_Directory_File(Table_Package_Directory_File *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Directory_File::SetDefaultValues()
{
	m_FK_Package_Directory = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_File = "";
is_null[3] = false;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Directory_File::FK_Package_Directory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package_Directory;}
long int Row_Package_Directory_File::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_OperatingSystem;}
long int Row_Package_Directory_File::FK_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Distro;}
string Row_Package_Directory_File::File_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_File;}
string Row_Package_Directory_File::Search_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Search;}

		
void Row_Package_Directory_File::FK_Package_Directory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package_Directory = val; is_modified=true; is_null[0]=false;}
void Row_Package_Directory_File::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[1]=false;}
void Row_Package_Directory_File::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Distro = val; is_modified=true; is_null[2]=false;}
void Row_Package_Directory_File::File_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_File = val; is_modified=true; is_null[3]=false;}
void Row_Package_Directory_File::Search_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Search = val; is_modified=true; is_null[4]=false;}

		
bool Row_Package_Directory_File::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Package_Directory_File::FK_Distro_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Package_Directory_File::Search_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}

			
void Row_Package_Directory_File::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Package_Directory_File::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Package_Directory_File::Search_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
	

string Row_Package_Directory_File::FK_Package_Directory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package_Directory);

return buf;
}

string Row_Package_Directory_File::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_Package_Directory_File::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_Package_Directory_File::File_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_File.c_str(), (unsigned long) m_File.size());
return string()+"\""+buf+"\"";
}

string Row_Package_Directory_File::Search_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Search.c_str(), (unsigned long) m_Search.size());
return string()+"\""+buf+"\"";
}




Table_Package_Directory_File::Key::Key(long int in_FK_Package_Directory, string in_File)
{
			pk_FK_Package_Directory = in_FK_Package_Directory;
pk_File = in_File;
	
}

Table_Package_Directory_File::Key::Key(Row_Package_Directory_File *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Package_Directory = pRow->m_FK_Package_Directory;
pk_File = pRow->m_File;
	
}		

bool Table_Package_Directory_File::Key_Less::operator()(const Table_Package_Directory_File::Key &key1, const Table_Package_Directory_File::Key &key2) const
{
			if (key1.pk_FK_Package_Directory!=key2.pk_FK_Package_Directory)
return key1.pk_FK_Package_Directory<key2.pk_FK_Package_Directory;
else
if (key1.pk_File!=key2.pk_File)
return key1.pk_File<key2.pk_File;
else
return false;	
}	

void Table_Package_Directory_File::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Package_Directory_File*>::iterator i = addedRows.begin();
	
		Row_Package_Directory_File *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Package_Directory_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->File_asSQL()+", "+pRow->Search_asSQL();

	
		string query = "insert into Package_Directory_File (FK_Package_Directory, FK_OperatingSystem, FK_Distro, File, Search) values ("+
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
	

	for (map<Key, Row_Package_Directory_File*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Package_Directory_File* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk_FK_Package_Directory);

char tmp_File[61];
mysql_real_escape_string(database->db_handle,tmp_File, key.pk_File.c_str(), (unsigned long) key.pk_File.size());


string condition;
condition = condition + "FK_Package_Directory=" + tmp_FK_Package_Directory+" AND "+"File=" + "\"" + tmp_File+ "\"";
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Package_Directory="+pRow->FK_Package_Directory_asSQL()+", FK_OperatingSystem="+pRow->FK_OperatingSystem_asSQL()+", FK_Distro="+pRow->FK_Distro_asSQL()+", File="+pRow->File_asSQL()+", Search="+pRow->Search_asSQL();

	
		string query = "update Package_Directory_File set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Package_Directory_File*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Package_Directory_File*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk_FK_Package_Directory);

char tmp_File[61];
mysql_real_escape_string(database->db_handle,tmp_File, key.pk_File.c_str(), (unsigned long) key.pk_File.size());


string condition;
condition = condition + "FK_Package_Directory=" + tmp_FK_Package_Directory+" AND "+"File=" + "\"" + tmp_File+ "\"";

	
		string query = "delete from Package_Directory_File where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Package_Directory_File::GetRows(string where_statement,vector<class Row_Package_Directory_File*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Package_Directory_File " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Package_Directory_File where " + where_statement;
		
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

		Row_Package_Directory_File *pRow = new Row_Package_Directory_File(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package_Directory = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package_Directory));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_File = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_File = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Search = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Search = string(row[4],lengths[4]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Package_Directory_File::Key, Row_Package_Directory_File*, Table_Package_Directory_File::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Package_Directory_File* Table_Package_Directory_File::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Package_Directory_File *pRow = new Row_Package_Directory_File(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Directory_File* Table_Package_Directory_File::GetRow(long int in_FK_Package_Directory, string in_File)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Package_Directory, in_File);

	map<Key, Row_Package_Directory_File*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Package_Directory_File* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Directory_File* Table_Package_Directory_File::FetchRow(Table_Package_Directory_File::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Package_Directory[32];
sprintf(tmp_FK_Package_Directory, "%li", key.pk_FK_Package_Directory);

char tmp_File[61];
mysql_real_escape_string(database->db_handle,tmp_File, key.pk_File.c_str(), (unsigned long) key.pk_File.size());


string condition;
condition = condition + "FK_Package_Directory=" + tmp_FK_Package_Directory+" AND "+"File=" + "\"" + tmp_File+ "\"";


	string query = "select * from Package_Directory_File where " + condition;		

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

	Row_Package_Directory_File *pRow = new Row_Package_Directory_File(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package_Directory = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package_Directory));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_File = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_File = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Search = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Search = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package_Directory* Row_Package_Directory_File::FK_Package_Directory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory *pTable = table->database->Package_Directory_get();
return pTable->GetRow(m_FK_Package_Directory);
}
class Row_OperatingSystem* Row_Package_Directory_File::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}
class Row_Distro* Row_Package_Directory_File::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}





