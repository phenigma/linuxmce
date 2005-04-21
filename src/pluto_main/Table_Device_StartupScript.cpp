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
#include "Table_Device_StartupScript.h"
#include "Table_Device.h"
#include "Table_StartupScript.h"



void Database_pluto_main::CreateTable_Device_StartupScript()
{
	tblDevice_StartupScript = new Table_Device_StartupScript(this);
}

void Database_pluto_main::DeleteTable_Device_StartupScript()
{
	if( tblDevice_StartupScript )
		delete tblDevice_StartupScript;
}

Table_Device_StartupScript::~Table_Device_StartupScript()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Device_StartupScript *pRow = (Row_Device_StartupScript *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Device_StartupScript *pRow = (Row_Device_StartupScript *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Device_StartupScript::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Device_StartupScript *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Device_StartupScript *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_StartupScript);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device_StartupScript::Reload()
{
	Row_Device_StartupScript *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_StartupScript);
		Row_Device_StartupScript *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Device_StartupScript::Row_Device_StartupScript(Table_Device_StartupScript *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Device_StartupScript::SetDefaultValues()
{
	m_FK_Device = 0;
is_null[0] = false;
m_FK_StartupScript = 0;
is_null[1] = false;
m_Boot_Order = 99;
is_null[2] = false;
m_Background = 0;
is_null[3] = false;
m_Enabled = 0;
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
m_psc_frozen = 0;
is_null[9] = false;
m_psc_mod = "00000000000000";
is_null[10] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Device_StartupScript::FK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device;}
long int Row_Device_StartupScript::FK_StartupScript_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_StartupScript;}
long int Row_Device_StartupScript::Boot_Order_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Boot_Order;}
short int Row_Device_StartupScript::Background_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Background;}
short int Row_Device_StartupScript::Enabled_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Enabled;}
string Row_Device_StartupScript::Parameter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Parameter;}
long int Row_Device_StartupScript::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Device_StartupScript::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Device_StartupScript::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Device_StartupScript::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Device_StartupScript::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Device_StartupScript::FK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device = val; is_modified=true; is_null[0]=false;}
void Row_Device_StartupScript::FK_StartupScript_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_StartupScript = val; is_modified=true; is_null[1]=false;}
void Row_Device_StartupScript::Boot_Order_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Boot_Order = val; is_modified=true; is_null[2]=false;}
void Row_Device_StartupScript::Background_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Background = val; is_modified=true; is_null[3]=false;}
void Row_Device_StartupScript::Enabled_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Enabled = val; is_modified=true; is_null[4]=false;}
void Row_Device_StartupScript::Parameter_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Parameter = val; is_modified=true; is_null[5]=false;}
void Row_Device_StartupScript::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_Device_StartupScript::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_Device_StartupScript::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_Device_StartupScript::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_Device_StartupScript::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}

		
bool Row_Device_StartupScript::Parameter_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Device_StartupScript::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Device_StartupScript::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Device_StartupScript::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Device_StartupScript::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_Device_StartupScript::Parameter_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[5]=val;
is_modified=true;
}
void Row_Device_StartupScript::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[6]=val;
is_modified=true;
}
void Row_Device_StartupScript::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[7]=val;
is_modified=true;
}
void Row_Device_StartupScript::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[8]=val;
is_modified=true;
}
void Row_Device_StartupScript::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[9]=val;
is_modified=true;
}
	

string Row_Device_StartupScript::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_Device_StartupScript::FK_StartupScript_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StartupScript);

return buf;
}

string Row_Device_StartupScript::Boot_Order_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Boot_Order);

return buf;
}

string Row_Device_StartupScript::Background_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Background);

return buf;
}

string Row_Device_StartupScript::Enabled_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Enabled);

return buf;
}

string Row_Device_StartupScript::Parameter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Parameter.c_str(), (unsigned long) min(50,m_Parameter.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Device_StartupScript::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Device_StartupScript::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Device_StartupScript::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Device_StartupScript::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Device_StartupScript::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Device_StartupScript::Key::Key(long int in_FK_Device, long int in_FK_StartupScript)
{
			pk_FK_Device = in_FK_Device;
pk_FK_StartupScript = in_FK_StartupScript;
	
}

Table_Device_StartupScript::Key::Key(Row_Device_StartupScript *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_FK_Device = pRow->m_FK_Device;
pk_FK_StartupScript = pRow->m_FK_StartupScript;
	
}		

bool Table_Device_StartupScript::Key_Less::operator()(const Table_Device_StartupScript::Key &key1, const Table_Device_StartupScript::Key &key2) const
{
			if (key1.pk_FK_Device!=key2.pk_FK_Device)
return key1.pk_FK_Device<key2.pk_FK_Device;
else
if (key1.pk_FK_StartupScript!=key2.pk_FK_StartupScript)
return key1.pk_FK_StartupScript<key2.pk_FK_StartupScript;
else
return false;	
}	

bool Table_Device_StartupScript::Commit()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Device_StartupScript *pRow = (Row_Device_StartupScript *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Device_asSQL()+", "+pRow->FK_StartupScript_asSQL()+", "+pRow->Boot_Order_asSQL()+", "+pRow->Background_asSQL()+", "+pRow->Enabled_asSQL()+", "+pRow->Parameter_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Device_StartupScript (`FK_Device`, `FK_StartupScript`, `Boot_Order`, `Background`, `Enabled`, `Parameter`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
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
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_StartupScript);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Device_StartupScript* pRow = (Row_Device_StartupScript*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_StartupScript);

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_StartupScript[32];
sprintf(tmp_FK_StartupScript, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_StartupScript`=" + tmp_FK_StartupScript;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_Device`="+pRow->FK_Device_asSQL()+", `FK_StartupScript`="+pRow->FK_StartupScript_asSQL()+", `Boot_Order`="+pRow->Boot_Order_asSQL()+", `Background`="+pRow->Background_asSQL()+", `Enabled`="+pRow->Enabled_asSQL()+", `Parameter`="+pRow->Parameter_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Device_StartupScript set " + update_values_list + " where " + condition;
			
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
		Row_Device_StartupScript* pRow = (Row_Device_StartupScript*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Device_StartupScript* pRow = (Row_Device_StartupScript*) (*i).second;	

		char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_StartupScript[32];
sprintf(tmp_FK_StartupScript, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_StartupScript`=" + tmp_FK_StartupScript;

	
		string query = "delete from Device_StartupScript where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Device_StartupScript*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Device_StartupScript::GetRows(string where_statement,vector<class Row_Device_StartupScript*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Device_StartupScript`.* from Device_StartupScript " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Device_StartupScript`.* from Device_StartupScript where " + where_statement;
		
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

		Row_Device_StartupScript *pRow = new Row_Device_StartupScript(this);
		
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
pRow->m_FK_StartupScript = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_StartupScript));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Boot_Order = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Boot_Order));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Background = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Background));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Enabled = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_Enabled));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Parameter = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Parameter = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Device,pRow->m_FK_StartupScript);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Device_StartupScript *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Device_StartupScript* Table_Device_StartupScript::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_Device_StartupScript *pRow = new Row_Device_StartupScript(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Device_StartupScript* Table_Device_StartupScript::GetRow(long int in_FK_Device, long int in_FK_StartupScript)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	DoubleLongKey row_key(in_FK_Device, in_FK_StartupScript);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Device_StartupScript*) (*i).second;
	//we have to fetch row
	Row_Device_StartupScript* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device_StartupScript* Table_Device_StartupScript::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_FK_Device[32];
sprintf(tmp_FK_Device, "%li", key.pk1);

char tmp_FK_StartupScript[32];
sprintf(tmp_FK_StartupScript, "%li", key.pk2);


string condition;
condition = condition + "`FK_Device`=" + tmp_FK_Device+" AND "+"`FK_StartupScript`=" + tmp_FK_StartupScript;


	string query = "select * from Device_StartupScript where " + condition;		

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

	Row_Device_StartupScript *pRow = new Row_Device_StartupScript(this);
		
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
pRow->m_FK_StartupScript = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_StartupScript));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Boot_Order = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Boot_Order));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Background = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Background));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Enabled = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_Enabled));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Parameter = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Parameter = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Device* Row_Device_StartupScript::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}
class Row_StartupScript* Row_Device_StartupScript::FK_StartupScript_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StartupScript *pTable = table->database->StartupScript_get();
return pTable->GetRow(m_FK_StartupScript);
}





