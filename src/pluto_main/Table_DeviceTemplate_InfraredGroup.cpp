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
#include "Table_DeviceTemplate_InfraredGroup.h"
#include "Table_InfraredGroup.h"
#include "Table_DeviceTemplate.h"



void Database_pluto_main::CreateTable_DeviceTemplate_InfraredGroup()
{
	tblDeviceTemplate_InfraredGroup = new Table_DeviceTemplate_InfraredGroup(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_InfraredGroup()
{
	if( tblDeviceTemplate_InfraredGroup )
		delete tblDeviceTemplate_InfraredGroup;
}

Table_DeviceTemplate_InfraredGroup::~Table_DeviceTemplate_InfraredGroup()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DeviceTemplate_InfraredGroup *pRow = (Row_DeviceTemplate_InfraredGroup *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DeviceTemplate_InfraredGroup *pRow = (Row_DeviceTemplate_InfraredGroup *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DeviceTemplate_InfraredGroup::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_DeviceTemplate_InfraredGroup *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DeviceTemplate_InfraredGroup *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_DeviceTemplate_InfraredGroup);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate_InfraredGroup::Reload()
{
	Row_DeviceTemplate_InfraredGroup *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_DeviceTemplate_InfraredGroup);
		Row_DeviceTemplate_InfraredGroup *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_InfraredGroup::Row_DeviceTemplate_InfraredGroup(Table_DeviceTemplate_InfraredGroup *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_InfraredGroup::SetDefaultValues()
{
	m_PK_DeviceTemplate_InfraredGroup = 0;
is_null[0] = false;
m_FK_InfraredGroup = 0;
is_null[1] = false;
m_FK_DeviceTemplate = 0;
is_null[2] = false;
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

long int Row_DeviceTemplate_InfraredGroup::PK_DeviceTemplate_InfraredGroup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PK_DeviceTemplate_InfraredGroup;}
long int Row_DeviceTemplate_InfraredGroup::FK_InfraredGroup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_InfraredGroup;}
long int Row_DeviceTemplate_InfraredGroup::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_DeviceTemplate;}
long int Row_DeviceTemplate_InfraredGroup::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_DeviceTemplate_InfraredGroup::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_DeviceTemplate_InfraredGroup::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_DeviceTemplate_InfraredGroup::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_DeviceTemplate_InfraredGroup::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}

		
void Row_DeviceTemplate_InfraredGroup::PK_DeviceTemplate_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PK_DeviceTemplate_InfraredGroup = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_InfraredGroup::FK_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_InfraredGroup = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_InfraredGroup::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_InfraredGroup::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_InfraredGroup::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_InfraredGroup::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_InfraredGroup::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_InfraredGroup::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}

		
bool Row_DeviceTemplate_InfraredGroup::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_DeviceTemplate_InfraredGroup::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_DeviceTemplate_InfraredGroup::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_DeviceTemplate_InfraredGroup::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[6];}

			
void Row_DeviceTemplate_InfraredGroup::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DeviceTemplate_InfraredGroup::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DeviceTemplate_InfraredGroup::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DeviceTemplate_InfraredGroup::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[6]=val;
is_modified=true;
}
	

string Row_DeviceTemplate_InfraredGroup::PK_DeviceTemplate_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DeviceTemplate_InfraredGroup);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::FK_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_InfraredGroup);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate_InfraredGroup::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_DeviceTemplate_InfraredGroup::Key::Key(long int in_PK_DeviceTemplate_InfraredGroup)
{
			pk_PK_DeviceTemplate_InfraredGroup = in_PK_DeviceTemplate_InfraredGroup;
	
}

Table_DeviceTemplate_InfraredGroup::Key::Key(Row_DeviceTemplate_InfraredGroup *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_PK_DeviceTemplate_InfraredGroup = pRow->m_PK_DeviceTemplate_InfraredGroup;
	
}		

bool Table_DeviceTemplate_InfraredGroup::Key_Less::operator()(const Table_DeviceTemplate_InfraredGroup::Key &key1, const Table_DeviceTemplate_InfraredGroup::Key &key2) const
{
			if (key1.pk_PK_DeviceTemplate_InfraredGroup!=key2.pk_PK_DeviceTemplate_InfraredGroup)
return key1.pk_PK_DeviceTemplate_InfraredGroup<key2.pk_PK_DeviceTemplate_InfraredGroup;
else
return false;	
}	

bool Table_DeviceTemplate_InfraredGroup::Commit()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_InfraredGroup *pRow = (Row_DeviceTemplate_InfraredGroup *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DeviceTemplate_InfraredGroup_asSQL()+", "+pRow->FK_InfraredGroup_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into DeviceTemplate_InfraredGroup (`PK_DeviceTemplate_InfraredGroup`, `FK_InfraredGroup`, `FK_DeviceTemplate`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_DeviceTemplate_InfraredGroup=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_DeviceTemplate_InfraredGroup);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DeviceTemplate_InfraredGroup* pRow = (Row_DeviceTemplate_InfraredGroup*) (*i).second;	
		SingleLongKey key(pRow->m_PK_DeviceTemplate_InfraredGroup);

		char tmp_PK_DeviceTemplate_InfraredGroup[32];
sprintf(tmp_PK_DeviceTemplate_InfraredGroup, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate_InfraredGroup`=" + tmp_PK_DeviceTemplate_InfraredGroup;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DeviceTemplate_InfraredGroup`="+pRow->PK_DeviceTemplate_InfraredGroup_asSQL()+", `FK_InfraredGroup`="+pRow->FK_InfraredGroup_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update DeviceTemplate_InfraredGroup set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_DeviceTemplate_InfraredGroup* pRow = (Row_DeviceTemplate_InfraredGroup*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_DeviceTemplate_InfraredGroup* pRow = (Row_DeviceTemplate_InfraredGroup*) (*i).second;	

		char tmp_PK_DeviceTemplate_InfraredGroup[32];
sprintf(tmp_PK_DeviceTemplate_InfraredGroup, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate_InfraredGroup`=" + tmp_PK_DeviceTemplate_InfraredGroup;

	
		string query = "delete from DeviceTemplate_InfraredGroup where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_DeviceTemplate_InfraredGroup*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_DeviceTemplate_InfraredGroup::GetRows(string where_statement,vector<class Row_DeviceTemplate_InfraredGroup*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DeviceTemplate_InfraredGroup`.* from DeviceTemplate_InfraredGroup " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `DeviceTemplate_InfraredGroup`.* from DeviceTemplate_InfraredGroup where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
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

		Row_DeviceTemplate_InfraredGroup *pRow = new Row_DeviceTemplate_InfraredGroup(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate_InfraredGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate_InfraredGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceTemplate));
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

		SingleLongKey key(pRow->m_PK_DeviceTemplate_InfraredGroup);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DeviceTemplate_InfraredGroup *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_DeviceTemplate_InfraredGroup* Table_DeviceTemplate_InfraredGroup::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_DeviceTemplate_InfraredGroup *pRow = new Row_DeviceTemplate_InfraredGroup(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_InfraredGroup* Table_DeviceTemplate_InfraredGroup::GetRow(long int in_PK_DeviceTemplate_InfraredGroup)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	SingleLongKey row_key(in_PK_DeviceTemplate_InfraredGroup);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DeviceTemplate_InfraredGroup*) (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_InfraredGroup* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_InfraredGroup* Table_DeviceTemplate_InfraredGroup::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_PK_DeviceTemplate_InfraredGroup[32];
sprintf(tmp_PK_DeviceTemplate_InfraredGroup, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate_InfraredGroup`=" + tmp_PK_DeviceTemplate_InfraredGroup;


	string query = "select * from DeviceTemplate_InfraredGroup where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
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

	Row_DeviceTemplate_InfraredGroup *pRow = new Row_DeviceTemplate_InfraredGroup(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate_InfraredGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate_InfraredGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DeviceTemplate));
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


class Row_InfraredGroup* Row_DeviceTemplate_InfraredGroup::FK_InfraredGroup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_InfraredGroup *pTable = table->database->InfraredGroup_get();
return pTable->GetRow(m_FK_InfraredGroup);
}
class Row_DeviceTemplate* Row_DeviceTemplate_InfraredGroup::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}





