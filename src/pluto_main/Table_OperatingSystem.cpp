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
#include "Table_OperatingSystem.h"

#include "Table_DeviceTemplate.h"
#include "Table_Distro.h"
#include "Table_InstallWizard_Distro.h"
#include "Table_Package_Compat.h"
#include "Table_Package_Directory.h"
#include "Table_Package_Directory_File.h"
#include "Table_Package_Source_Compat.h"
#include "Table_RepositorySource.h"


void Database_pluto_main::CreateTable_OperatingSystem()
{
	tblOperatingSystem = new Table_OperatingSystem(this);
}

void Database_pluto_main::DeleteTable_OperatingSystem()
{
	delete tblOperatingSystem;
}

Table_OperatingSystem::~Table_OperatingSystem()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_OperatingSystem *pRow = (Row_OperatingSystem *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_OperatingSystem *pRow = (Row_OperatingSystem *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_OperatingSystem::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_OperatingSystem *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_OperatingSystem *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_OperatingSystem);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_OperatingSystem::Reload()
{
	Row_OperatingSystem *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_OperatingSystem);
		Row_OperatingSystem *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_OperatingSystem::Row_OperatingSystem(Table_OperatingSystem *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_OperatingSystem::SetDefaultValues()
{
	m_PK_OperatingSystem = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
m_psc_frozen = 0;
is_null[6] = false;
m_psc_mod = "00000000000000";
is_null[7] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_OperatingSystem::PK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_OperatingSystem;}
string Row_OperatingSystem::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_OperatingSystem::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_OperatingSystem::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_OperatingSystem::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_OperatingSystem::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_OperatingSystem::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_OperatingSystem::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_OperatingSystem::PK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_OperatingSystem = val; is_modified=true; is_null[0]=false;}
void Row_OperatingSystem::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_OperatingSystem::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_OperatingSystem::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_OperatingSystem::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_OperatingSystem::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_OperatingSystem::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_OperatingSystem::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}

		
bool Row_OperatingSystem::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_OperatingSystem::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_OperatingSystem::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_OperatingSystem::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_OperatingSystem::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_OperatingSystem::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_OperatingSystem::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_OperatingSystem::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_OperatingSystem::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_OperatingSystem::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_OperatingSystem::PK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_OperatingSystem);

return buf;
}

string Row_OperatingSystem::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_OperatingSystem::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_OperatingSystem::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_OperatingSystem::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_OperatingSystem::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_OperatingSystem::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_OperatingSystem::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_OperatingSystem::Key::Key(long int in_PK_OperatingSystem)
{
			pk_PK_OperatingSystem = in_PK_OperatingSystem;
	
}

Table_OperatingSystem::Key::Key(Row_OperatingSystem *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_OperatingSystem = pRow->m_PK_OperatingSystem;
	
}		

bool Table_OperatingSystem::Key_Less::operator()(const Table_OperatingSystem::Key &key1, const Table_OperatingSystem::Key &key2) const
{
			if (key1.pk_PK_OperatingSystem!=key2.pk_PK_OperatingSystem)
return key1.pk_PK_OperatingSystem<key2.pk_PK_OperatingSystem;
else
return false;	
}	

bool Table_OperatingSystem::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_OperatingSystem *pRow = (Row_OperatingSystem *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_OperatingSystem_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into OperatingSystem (`PK_OperatingSystem`, `Description`, `Define`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_OperatingSystem=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_OperatingSystem);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_OperatingSystem* pRow = (Row_OperatingSystem*) (*i).second;	
		SingleLongKey key(pRow->m_PK_OperatingSystem);

		char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_OperatingSystem`=" + tmp_PK_OperatingSystem;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_OperatingSystem`="+pRow->PK_OperatingSystem_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update OperatingSystem set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
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
		Row_OperatingSystem* pRow = (Row_OperatingSystem*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_OperatingSystem* pRow = (Row_OperatingSystem*) (*i).second;	

		char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_OperatingSystem`=" + tmp_PK_OperatingSystem;

	
		string query = "delete from OperatingSystem where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_OperatingSystem*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_OperatingSystem::GetRows(string where_statement,vector<class Row_OperatingSystem*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from OperatingSystem " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from OperatingSystem where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
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

		Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OperatingSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OperatingSystem));
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_OperatingSystem);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_OperatingSystem *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_OperatingSystem* Table_OperatingSystem::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_OperatingSystem* Table_OperatingSystem::GetRow(long int in_PK_OperatingSystem)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_OperatingSystem);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_OperatingSystem*) (*i).second;
	//we have to fetch row
	Row_OperatingSystem* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_OperatingSystem* Table_OperatingSystem::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_OperatingSystem[32];
sprintf(tmp_PK_OperatingSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_OperatingSystem`=" + tmp_PK_OperatingSystem;


	string query = "select * from OperatingSystem where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
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

	Row_OperatingSystem *pRow = new Row_OperatingSystem(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OperatingSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OperatingSystem));
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_OperatingSystem::DeviceTemplate_FK_OperatingSystem_getrows(vector <class Row_DeviceTemplate*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Distro_FK_OperatingSystem_getrows(vector <class Row_Distro*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::InstallWizard_Distro_FK_OperatingSystem_getrows(vector <class Row_InstallWizard_Distro*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_InstallWizard_Distro *pTable = table->database->InstallWizard_Distro_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Compat*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Compat *pTable = table->database->Package_Compat_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Directory_FK_OperatingSystem_getrows(vector <class Row_Package_Directory*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory *pTable = table->database->Package_Directory_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Directory_File_FK_OperatingSystem_getrows(vector <class Row_Package_Directory_File*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Directory_File *pTable = table->database->Package_Directory_File_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::Package_Source_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Source_Compat*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package_Source_Compat *pTable = table->database->Package_Source_Compat_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}
void Row_OperatingSystem::RepositorySource_FK_OperatingSystem_getrows(vector <class Row_RepositorySource*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
pTable->GetRows("`FK_OperatingSystem`=" + StringUtils::itos(m_PK_OperatingSystem),rows);
}



