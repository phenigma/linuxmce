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
#include "Table_Package_Distro.h"
#include "Table_Package.h"
#include "Table_Distro.h"
#include "Table_RepositorySource.h"



void Database_pluto_main::CreateTable_Package_Distro()
{
	tblPackage_Distro = new Table_Package_Distro(this);
}

void Database_pluto_main::DeleteTable_Package_Distro()
{
	delete tblPackage_Distro;
}

Table_Package_Distro::~Table_Package_Distro()
{
	map<Table_Package_Distro::Key, class Row_Package_Distro*, Table_Package_Distro::Key_Less>::iterator it;
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


void Row_Package_Distro::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Package_Distro*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Package_Distro::Key key(this);					
			map<Table_Package_Distro::Key, Row_Package_Distro*, Table_Package_Distro::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Package_Distro::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Package_Distro::Key key(this);		
		Row_Package_Distro *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Distro::Row_Package_Distro(Table_Package_Distro *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Distro::SetDefaultValues()
{
	m_FK_Package = 0;
is_null[0] = false;
m_FK_Distro = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Distro::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
long int Row_Package_Distro::FK_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Distro;}
string Row_Package_Distro::Name_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Name;}
long int Row_Package_Distro::FK_RepositorySource_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_RepositorySource;}
string Row_Package_Distro::Repository_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Repository;}
string Row_Package_Distro::Version_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Version;}

		
void Row_Package_Distro::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[0]=false;}
void Row_Package_Distro::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Distro = val; is_modified=true; is_null[1]=false;}
void Row_Package_Distro::Name_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Name = val; is_modified=true; is_null[2]=false;}
void Row_Package_Distro::FK_RepositorySource_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_RepositorySource = val; is_modified=true; is_null[3]=false;}
void Row_Package_Distro::Repository_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Repository = val; is_modified=true; is_null[4]=false;}
void Row_Package_Distro::Version_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Version = val; is_modified=true; is_null[5]=false;}

		
bool Row_Package_Distro::Name_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Package_Distro::FK_RepositorySource_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Package_Distro::Repository_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Package_Distro::Version_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_Package_Distro::Name_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Package_Distro::FK_RepositorySource_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Package_Distro::Repository_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Package_Distro::Version_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_Package_Distro::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Package_Distro::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_Package_Distro::Name_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Name.c_str(), (unsigned long) m_Name.size());
return string()+"\""+buf+"\"";
}

string Row_Package_Distro::FK_RepositorySource_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositorySource);

return buf;
}

string Row_Package_Distro::Repository_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Repository.c_str(), (unsigned long) m_Repository.size());
return string()+"\""+buf+"\"";
}

string Row_Package_Distro::Version_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Version.c_str(), (unsigned long) m_Version.size());
return string()+"\""+buf+"\"";
}




Table_Package_Distro::Key::Key(long int in_FK_Package, long int in_FK_Distro)
{
			pk_FK_Package = in_FK_Package;
pk_FK_Distro = in_FK_Distro;
	
}

Table_Package_Distro::Key::Key(Row_Package_Distro *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Package = pRow->m_FK_Package;
pk_FK_Distro = pRow->m_FK_Distro;
	
}		

bool Table_Package_Distro::Key_Less::operator()(const Table_Package_Distro::Key &key1, const Table_Package_Distro::Key &key2) const
{
			if (key1.pk_FK_Package!=key2.pk_FK_Package)
return key1.pk_FK_Package<key2.pk_FK_Package;
else
if (key1.pk_FK_Distro!=key2.pk_FK_Distro)
return key1.pk_FK_Distro<key2.pk_FK_Distro;
else
return false;	
}	

void Table_Package_Distro::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Package_Distro*>::iterator i = addedRows.begin();
	
		Row_Package_Distro *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Package_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->FK_RepositorySource_asSQL()+", "+pRow->Repository_asSQL()+", "+pRow->Version_asSQL();

	
		string query = "insert into Package_Distro (FK_Package, FK_Distro, Name, FK_RepositorySource, Repository, Version) values ("+
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
	

	for (map<Key, Row_Package_Distro*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Package_Distro* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Distro[32];
sprintf(tmp_FK_Distro, "%li", key.pk_FK_Distro);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Distro=" + tmp_FK_Distro;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Package="+pRow->FK_Package_asSQL()+", FK_Distro="+pRow->FK_Distro_asSQL()+", Name="+pRow->Name_asSQL()+", FK_RepositorySource="+pRow->FK_RepositorySource_asSQL()+", Repository="+pRow->Repository_asSQL()+", Version="+pRow->Version_asSQL();

	
		string query = "update Package_Distro set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Package_Distro*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Package_Distro*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Distro[32];
sprintf(tmp_FK_Distro, "%li", key.pk_FK_Distro);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Distro=" + tmp_FK_Distro;

	
		string query = "delete from Package_Distro where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Package_Distro::GetRows(string where_statement,vector<class Row_Package_Distro*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Package_Distro where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query" << endl;
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

		Row_Package_Distro *pRow = new Row_Package_Distro(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Distro));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Repository = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Repository = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Version = string(row[5],lengths[5]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Package_Distro::Key, Row_Package_Distro*, Table_Package_Distro::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Package_Distro* Table_Package_Distro::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Package_Distro *pRow = new Row_Package_Distro(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Distro* Table_Package_Distro::GetRow(long int in_FK_Package, long int in_FK_Distro)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Package, in_FK_Distro);

	map<Key, Row_Package_Distro*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Package_Distro* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Distro* Table_Package_Distro::FetchRow(Table_Package_Distro::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);

char tmp_FK_Distro[32];
sprintf(tmp_FK_Distro, "%li", key.pk_FK_Distro);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Distro=" + tmp_FK_Distro;


	string query = "select * from Package_Distro where " + condition;		

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

	Row_Package_Distro *pRow = new Row_Package_Distro(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Distro));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Repository = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Repository = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Version = string(row[5],lengths[5]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_Package_Distro::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Distro* Row_Package_Distro::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}
class Row_RepositorySource* Row_Package_Distro::FK_RepositorySource_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
return pTable->GetRow(m_FK_RepositorySource);
}





