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
#include "Table_Distro.h"
#include "Table_OperatingSystem.h"

#include "Table_DeviceTemplate_Package.h"
#include "Table_Package_Distro.h"
#include "Table_RepositorySource.h"


void Database_pluto_main::CreateTable_Distro()
{
	tblDistro = new Table_Distro(this);
}

void Database_pluto_main::DeleteTable_Distro()
{
	delete tblDistro;
}

Table_Distro::~Table_Distro()
{
	map<Table_Distro::Key, class Row_Distro*, Table_Distro::Key_Less>::iterator it;
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


void Row_Distro::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Distro*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Distro::Key key(this);					
			map<Table_Distro::Key, Row_Distro*, Table_Distro::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Distro::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Distro::Key key(this);		
		Row_Distro *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Distro::Row_Distro(Table_Distro *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Distro::SetDefaultValues()
{
	m_PK_Distro = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_FK_OperatingSystem = 0;
is_null[3] = false;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Distro::PK_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Distro;}
string Row_Distro::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Distro::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_Distro::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_OperatingSystem;}
string Row_Distro::ConfirmDependencyProgram_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ConfirmDependencyProgram;}

		
void Row_Distro::PK_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Distro = val; is_modified=true; is_null[0]=false;}
void Row_Distro::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Distro::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_Distro::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[3]=false;}
void Row_Distro::ConfirmDependencyProgram_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ConfirmDependencyProgram = val; is_modified=true; is_null[4]=false;}

		
bool Row_Distro::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Distro::ConfirmDependencyProgram_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}

			
void Row_Distro::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Distro::ConfirmDependencyProgram_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
	

string Row_Distro::PK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Distro);

return buf;
}

string Row_Distro::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Distro::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_Distro::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_Distro::ConfirmDependencyProgram_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_ConfirmDependencyProgram.c_str(), (unsigned long) m_ConfirmDependencyProgram.size());
return string()+"\""+buf+"\"";
}




Table_Distro::Key::Key(long int in_PK_Distro)
{
			pk_PK_Distro = in_PK_Distro;
	
}

Table_Distro::Key::Key(Row_Distro *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Distro = pRow->m_PK_Distro;
	
}		

bool Table_Distro::Key_Less::operator()(const Table_Distro::Key &key1, const Table_Distro::Key &key2) const
{
			if (key1.pk_PK_Distro!=key2.pk_PK_Distro)
return key1.pk_PK_Distro<key2.pk_PK_Distro;
else
return false;	
}	

void Table_Distro::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Distro*>::iterator i = addedRows.begin();
	
		Row_Distro *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Distro_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->ConfirmDependencyProgram_asSQL();

	
		string query = "insert into Distro (PK_Distro, Description, Define, FK_OperatingSystem, ConfirmDependencyProgram) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Distro=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Distro*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Distro* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Distro[32];
sprintf(tmp_PK_Distro, "%li", key.pk_PK_Distro);


string condition;
condition = condition + "PK_Distro=" + tmp_PK_Distro;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Distro="+pRow->PK_Distro_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", FK_OperatingSystem="+pRow->FK_OperatingSystem_asSQL()+", ConfirmDependencyProgram="+pRow->ConfirmDependencyProgram_asSQL();

	
		string query = "update Distro set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Distro*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Distro*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Distro[32];
sprintf(tmp_PK_Distro, "%li", key.pk_PK_Distro);


string condition;
condition = condition + "PK_Distro=" + tmp_PK_Distro;

	
		string query = "delete from Distro where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Distro::GetRows(string where_statement,vector<class Row_Distro*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Distro where " + where_statement;
		
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

		Row_Distro *pRow = new Row_Distro(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Distro = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Distro));
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

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ConfirmDependencyProgram = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ConfirmDependencyProgram = string(row[4],lengths[4]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Distro::Key, Row_Distro*, Table_Distro::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Distro* Table_Distro::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Distro *pRow = new Row_Distro(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Distro* Table_Distro::GetRow(long int in_PK_Distro)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Distro);

	map<Key, Row_Distro*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Distro* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Distro* Table_Distro::FetchRow(Table_Distro::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Distro[32];
sprintf(tmp_PK_Distro, "%li", key.pk_PK_Distro);


string condition;
condition = condition + "PK_Distro=" + tmp_PK_Distro;


	string query = "select * from Distro where " + condition;		

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

	Row_Distro *pRow = new Row_Distro(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Distro = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Distro));
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

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ConfirmDependencyProgram = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ConfirmDependencyProgram = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_OperatingSystem* Row_Distro::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}


void Row_Distro::DeviceTemplate_Package_FK_Distro_getrows(vector <class Row_DeviceTemplate_Package*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Package *pTable = table->database->DeviceTemplate_Package_get();
pTable->GetRows("FK_Distro=" + StringUtils::itos(m_PK_Distro),rows);
}
void Row_Distro::Package_Distro_FK_Distro_getrows(vector <class Row_Package_Distro*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Distro *pTable = table->database->Package_Distro_get();
pTable->GetRows("FK_Distro=" + StringUtils::itos(m_PK_Distro),rows);
}
void Row_Distro::RepositorySource_FK_Distro_getrows(vector <class Row_RepositorySource*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
pTable->GetRows("FK_Distro=" + StringUtils::itos(m_PK_Distro),rows);
}



