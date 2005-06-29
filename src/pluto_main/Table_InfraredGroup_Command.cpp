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
#include "Table_InfraredGroup_Command.h"
#include "Table_InfraredGroup.h"
#include "Table_Command.h"
#include "Table_DeviceTemplate.h"
#include "Table_Device.h"
#include "Table_Users.h"
#include "Table_Installation.h"

#include "Table_InfraredGroup_Command_Preferred.h"


void Database_pluto_main::CreateTable_InfraredGroup_Command()
{
	tblInfraredGroup_Command = new Table_InfraredGroup_Command(this);
}

void Database_pluto_main::DeleteTable_InfraredGroup_Command()
{
	if( tblInfraredGroup_Command )
		delete tblInfraredGroup_Command;
}

Table_InfraredGroup_Command::~Table_InfraredGroup_Command()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_InfraredGroup_Command *pRow = (Row_InfraredGroup_Command *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_InfraredGroup_Command *pRow = (Row_InfraredGroup_Command *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_InfraredGroup_Command::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_InfraredGroup_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_InfraredGroup_Command *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_InfraredGroup_Command);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_InfraredGroup_Command::Reload()
{
	Row_InfraredGroup_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_InfraredGroup_Command);
		Row_InfraredGroup_Command *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_InfraredGroup_Command::Row_InfraredGroup_Command(Table_InfraredGroup_Command *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_InfraredGroup_Command::SetDefaultValues()
{
	m_PK_InfraredGroup_Command = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_Command = 0;
is_null[2] = false;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
m_psc_frozen = 0;
is_null[11] = false;
m_psc_mod = "00000000000000";
is_null[12] = false;
is_null[13] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_InfraredGroup_Command::PK_InfraredGroup_Command_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PK_InfraredGroup_Command;}
long int Row_InfraredGroup_Command::FK_InfraredGroup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_InfraredGroup;}
long int Row_InfraredGroup_Command::FK_Command_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Command;}
long int Row_InfraredGroup_Command::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_DeviceTemplate;}
long int Row_InfraredGroup_Command::FK_Device_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Device;}
string Row_InfraredGroup_Command::IRData_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_IRData;}
long int Row_InfraredGroup_Command::FK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Users;}
long int Row_InfraredGroup_Command::FK_Installation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Installation;}
long int Row_InfraredGroup_Command::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_InfraredGroup_Command::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_InfraredGroup_Command::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_InfraredGroup_Command::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_InfraredGroup_Command::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}
long int Row_InfraredGroup_Command::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_restrict;}

		
void Row_InfraredGroup_Command::PK_InfraredGroup_Command_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PK_InfraredGroup_Command = val; is_modified=true; is_null[0]=false;}
void Row_InfraredGroup_Command::FK_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_InfraredGroup = val; is_modified=true; is_null[1]=false;}
void Row_InfraredGroup_Command::FK_Command_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Command = val; is_modified=true; is_null[2]=false;}
void Row_InfraredGroup_Command::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[3]=false;}
void Row_InfraredGroup_Command::FK_Device_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Device = val; is_modified=true; is_null[4]=false;}
void Row_InfraredGroup_Command::IRData_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_IRData = val; is_modified=true; is_null[5]=false;}
void Row_InfraredGroup_Command::FK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Users = val; is_modified=true; is_null[6]=false;}
void Row_InfraredGroup_Command::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Installation = val; is_modified=true; is_null[7]=false;}
void Row_InfraredGroup_Command::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[8]=false;}
void Row_InfraredGroup_Command::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[9]=false;}
void Row_InfraredGroup_Command::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[10]=false;}
void Row_InfraredGroup_Command::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[11]=false;}
void Row_InfraredGroup_Command::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[12]=false;}
void Row_InfraredGroup_Command::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_restrict = val; is_modified=true; is_null[13]=false;}

		
bool Row_InfraredGroup_Command::FK_InfraredGroup_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[1];}
bool Row_InfraredGroup_Command::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_InfraredGroup_Command::FK_Device_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_InfraredGroup_Command::IRData_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_InfraredGroup_Command::FK_Users_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[6];}
bool Row_InfraredGroup_Command::FK_Installation_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[7];}
bool Row_InfraredGroup_Command::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[8];}
bool Row_InfraredGroup_Command::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[9];}
bool Row_InfraredGroup_Command::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[10];}
bool Row_InfraredGroup_Command::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[11];}
bool Row_InfraredGroup_Command::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[13];}

			
void Row_InfraredGroup_Command::FK_InfraredGroup_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[1]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::FK_Device_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::IRData_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::FK_Users_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[6]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::FK_Installation_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[7]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[8]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[9]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[10]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[11]=val;
is_modified=true;
}
void Row_InfraredGroup_Command::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[13]=val;
is_modified=true;
}
	

string Row_InfraredGroup_Command::PK_InfraredGroup_Command_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_InfraredGroup_Command);

return buf;
}

string Row_InfraredGroup_Command::FK_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_InfraredGroup);

return buf;
}

string Row_InfraredGroup_Command::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_InfraredGroup_Command::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_InfraredGroup_Command::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_InfraredGroup_Command::IRData_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char *buf = new char[5000000];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_IRData.c_str(), (unsigned long) min(16777215,m_IRData.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_InfraredGroup_Command::FK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users);

return buf;
}

string Row_InfraredGroup_Command::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_InfraredGroup_Command::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_InfraredGroup_Command::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_InfraredGroup_Command::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_InfraredGroup_Command::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_InfraredGroup_Command::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[12])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_InfraredGroup_Command::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_InfraredGroup_Command::Key::Key(long int in_PK_InfraredGroup_Command)
{
			pk_PK_InfraredGroup_Command = in_PK_InfraredGroup_Command;
	
}

Table_InfraredGroup_Command::Key::Key(Row_InfraredGroup_Command *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_PK_InfraredGroup_Command = pRow->m_PK_InfraredGroup_Command;
	
}		

bool Table_InfraredGroup_Command::Key_Less::operator()(const Table_InfraredGroup_Command::Key &key1, const Table_InfraredGroup_Command::Key &key2) const
{
			if (key1.pk_PK_InfraredGroup_Command!=key2.pk_PK_InfraredGroup_Command)
return key1.pk_PK_InfraredGroup_Command<key2.pk_PK_InfraredGroup_Command;
else
return false;	
}	

bool Table_InfraredGroup_Command::Commit()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_InfraredGroup_Command *pRow = (Row_InfraredGroup_Command *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_InfraredGroup_Command_asSQL()+", "+pRow->FK_InfraredGroup_asSQL()+", "+pRow->FK_Command_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Device_asSQL()+", "+pRow->IRData_asSQL()+", "+pRow->FK_Users_asSQL()+", "+pRow->FK_Installation_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into InfraredGroup_Command (`PK_InfraredGroup_Command`, `FK_InfraredGroup`, `FK_Command`, `FK_DeviceTemplate`, `FK_Device`, `IRData`, `FK_Users`, `FK_Installation`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
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
pRow->m_PK_InfraredGroup_Command=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_InfraredGroup_Command);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_InfraredGroup_Command* pRow = (Row_InfraredGroup_Command*) (*i).second;	
		SingleLongKey key(pRow->m_PK_InfraredGroup_Command);

		char tmp_PK_InfraredGroup_Command[32];
sprintf(tmp_PK_InfraredGroup_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_InfraredGroup_Command`=" + tmp_PK_InfraredGroup_Command;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_InfraredGroup_Command`="+pRow->PK_InfraredGroup_Command_asSQL()+", `FK_InfraredGroup`="+pRow->FK_InfraredGroup_asSQL()+", `FK_Command`="+pRow->FK_Command_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `FK_Device`="+pRow->FK_Device_asSQL()+", `IRData`="+pRow->IRData_asSQL()+", `FK_Users`="+pRow->FK_Users_asSQL()+", `FK_Installation`="+pRow->FK_Installation_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update InfraredGroup_Command set " + update_values_list + " where " + condition;
			
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
		Row_InfraredGroup_Command* pRow = (Row_InfraredGroup_Command*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_InfraredGroup_Command* pRow = (Row_InfraredGroup_Command*) (*i).second;	

		char tmp_PK_InfraredGroup_Command[32];
sprintf(tmp_PK_InfraredGroup_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_InfraredGroup_Command`=" + tmp_PK_InfraredGroup_Command;

	
		string query = "delete from InfraredGroup_Command where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_InfraredGroup_Command*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_InfraredGroup_Command::GetRows(string where_statement,vector<class Row_InfraredGroup_Command*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `InfraredGroup_Command`.* from InfraredGroup_Command " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `InfraredGroup_Command`.* from InfraredGroup_Command where " + where_statement;
		
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

		Row_InfraredGroup_Command *pRow = new Row_InfraredGroup_Command(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InfraredGroup_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InfraredGroup_Command));
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
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Command));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Device));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IRData = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_IRData = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Installation));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_InfraredGroup_Command);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_InfraredGroup_Command *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_InfraredGroup_Command* Table_InfraredGroup_Command::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_InfraredGroup_Command *pRow = new Row_InfraredGroup_Command(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_InfraredGroup_Command* Table_InfraredGroup_Command::GetRow(long int in_PK_InfraredGroup_Command)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	SingleLongKey row_key(in_PK_InfraredGroup_Command);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_InfraredGroup_Command*) (*i).second;
	//we have to fetch row
	Row_InfraredGroup_Command* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_InfraredGroup_Command* Table_InfraredGroup_Command::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_PK_InfraredGroup_Command[32];
sprintf(tmp_PK_InfraredGroup_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_InfraredGroup_Command`=" + tmp_PK_InfraredGroup_Command;


	string query = "select * from InfraredGroup_Command where " + condition;		

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

	Row_InfraredGroup_Command *pRow = new Row_InfraredGroup_Command(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InfraredGroup_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InfraredGroup_Command));
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
pRow->m_FK_Command = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Command));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Device));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IRData = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_IRData = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Installation));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_restrict));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_InfraredGroup* Row_InfraredGroup_Command::FK_InfraredGroup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_InfraredGroup *pTable = table->database->InfraredGroup_get();
return pTable->GetRow(m_FK_InfraredGroup);
}
class Row_Command* Row_InfraredGroup_Command::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}
class Row_DeviceTemplate* Row_InfraredGroup_Command::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_Device* Row_InfraredGroup_Command::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}
class Row_Users* Row_InfraredGroup_Command::FK_Users_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users);
}
class Row_Installation* Row_InfraredGroup_Command::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}


void Row_InfraredGroup_Command::InfraredGroup_Command_Preferred_FK_InfraredGroup_Command_getrows(vector <class Row_InfraredGroup_Command_Preferred*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_InfraredGroup_Command_Preferred *pTable = table->database->InfraredGroup_Command_Preferred_get();
pTable->GetRows("`FK_InfraredGroup_Command`=" + StringUtils::itos(m_PK_InfraredGroup_Command),rows);
}



