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
#include "Table_CommandGroup_D_Command.h"
#include "Table_CommandGroup_D.h"
#include "Table_Command.h"
#include "Table_DeviceTemplate.h"
#include "Table_DeviceCategory.h"

#include "Table_CommandGroup_D_Command_CommandParameter.h"


void Database_pluto_main::CreateTable_CommandGroup_D_Command()
{
	tblCommandGroup_D_Command = new Table_CommandGroup_D_Command(this);
}

void Database_pluto_main::DeleteTable_CommandGroup_D_Command()
{
	delete tblCommandGroup_D_Command;
}

Table_CommandGroup_D_Command::~Table_CommandGroup_D_Command()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CommandGroup_D_Command *pRow = (Row_CommandGroup_D_Command *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CommandGroup_D_Command *pRow = (Row_CommandGroup_D_Command *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CommandGroup_D_Command::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_CommandGroup_D_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CommandGroup_D_Command *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_CommandGroup_D_Command);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CommandGroup_D_Command::Reload()
{
	Row_CommandGroup_D_Command *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_CommandGroup_D_Command);
		Row_CommandGroup_D_Command *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CommandGroup_D_Command::Row_CommandGroup_D_Command(Table_CommandGroup_D_Command *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CommandGroup_D_Command::SetDefaultValues()
{
	m_PK_CommandGroup_D_Command = 0;
is_null[0] = false;
m_FK_CommandGroup_D = 0;
is_null[1] = false;
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


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CommandGroup_D_Command::PK_CommandGroup_D_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CommandGroup_D_Command;}
long int Row_CommandGroup_D_Command::FK_CommandGroup_D_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CommandGroup_D;}
long int Row_CommandGroup_D_Command::FK_Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Command;}
long int Row_CommandGroup_D_Command::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_CommandGroup_D_Command::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceCategory;}
short int Row_CommandGroup_D_Command::BroadcastLevel_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_BroadcastLevel;}
short int Row_CommandGroup_D_Command::RelativeToSender_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_RelativeToSender;}
long int Row_CommandGroup_D_Command::OrderNum_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_OrderNum;}
long int Row_CommandGroup_D_Command::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_CommandGroup_D_Command::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_CommandGroup_D_Command::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_CommandGroup_D_Command::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_CommandGroup_D_Command::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_CommandGroup_D_Command::PK_CommandGroup_D_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CommandGroup_D_Command = val; is_modified=true; is_null[0]=false;}
void Row_CommandGroup_D_Command::FK_CommandGroup_D_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CommandGroup_D = val; is_modified=true; is_null[1]=false;}
void Row_CommandGroup_D_Command::FK_Command_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Command = val; is_modified=true; is_null[2]=false;}
void Row_CommandGroup_D_Command::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[3]=false;}
void Row_CommandGroup_D_Command::FK_DeviceCategory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[4]=false;}
void Row_CommandGroup_D_Command::BroadcastLevel_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_BroadcastLevel = val; is_modified=true; is_null[5]=false;}
void Row_CommandGroup_D_Command::RelativeToSender_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_RelativeToSender = val; is_modified=true; is_null[6]=false;}
void Row_CommandGroup_D_Command::OrderNum_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_OrderNum = val; is_modified=true; is_null[7]=false;}
void Row_CommandGroup_D_Command::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[8]=false;}
void Row_CommandGroup_D_Command::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[9]=false;}
void Row_CommandGroup_D_Command::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[10]=false;}
void Row_CommandGroup_D_Command::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[11]=false;}
void Row_CommandGroup_D_Command::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[12]=false;}

		
bool Row_CommandGroup_D_Command::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_CommandGroup_D_Command::FK_DeviceCategory_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_CommandGroup_D_Command::BroadcastLevel_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_CommandGroup_D_Command::RelativeToSender_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_CommandGroup_D_Command::OrderNum_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_CommandGroup_D_Command::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_CommandGroup_D_Command::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_CommandGroup_D_Command::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_CommandGroup_D_Command::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_CommandGroup_D_Command::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_CommandGroup_D_Command::FK_DeviceCategory_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_CommandGroup_D_Command::BroadcastLevel_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_CommandGroup_D_Command::RelativeToSender_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_CommandGroup_D_Command::OrderNum_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_CommandGroup_D_Command::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_CommandGroup_D_Command::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_CommandGroup_D_Command::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_CommandGroup_D_Command::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_CommandGroup_D_Command::PK_CommandGroup_D_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CommandGroup_D_Command);

return buf;
}

string Row_CommandGroup_D_Command::FK_CommandGroup_D_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandGroup_D);

return buf;
}

string Row_CommandGroup_D_Command::FK_Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Command);

return buf;
}

string Row_CommandGroup_D_Command::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_CommandGroup_D_Command::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCategory);

return buf;
}

string Row_CommandGroup_D_Command::BroadcastLevel_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_BroadcastLevel);

return buf;
}

string Row_CommandGroup_D_Command::RelativeToSender_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RelativeToSender);

return buf;
}

string Row_CommandGroup_D_Command::OrderNum_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_OrderNum);

return buf;
}

string Row_CommandGroup_D_Command::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_CommandGroup_D_Command::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_CommandGroup_D_Command::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_CommandGroup_D_Command::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_CommandGroup_D_Command::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_CommandGroup_D_Command::Key::Key(long int in_PK_CommandGroup_D_Command)
{
			pk_PK_CommandGroup_D_Command = in_PK_CommandGroup_D_Command;
	
}

Table_CommandGroup_D_Command::Key::Key(Row_CommandGroup_D_Command *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CommandGroup_D_Command = pRow->m_PK_CommandGroup_D_Command;
	
}		

bool Table_CommandGroup_D_Command::Key_Less::operator()(const Table_CommandGroup_D_Command::Key &key1, const Table_CommandGroup_D_Command::Key &key2) const
{
			if (key1.pk_PK_CommandGroup_D_Command!=key2.pk_PK_CommandGroup_D_Command)
return key1.pk_PK_CommandGroup_D_Command<key2.pk_PK_CommandGroup_D_Command;
else
return false;	
}	

bool Table_CommandGroup_D_Command::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CommandGroup_D_Command *pRow = (Row_CommandGroup_D_Command *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CommandGroup_D_Command_asSQL()+", "+pRow->FK_CommandGroup_D_asSQL()+", "+pRow->FK_Command_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->BroadcastLevel_asSQL()+", "+pRow->RelativeToSender_asSQL()+", "+pRow->OrderNum_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into CommandGroup_D_Command (`PK_CommandGroup_D_Command`, `FK_CommandGroup_D`, `FK_Command`, `FK_DeviceTemplate`, `FK_DeviceCategory`, `BroadcastLevel`, `RelativeToSender`, `OrderNum`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CommandGroup_D_Command=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_CommandGroup_D_Command);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CommandGroup_D_Command* pRow = (Row_CommandGroup_D_Command*) (*i).second;	
		SingleLongKey key(pRow->m_PK_CommandGroup_D_Command);

		char tmp_PK_CommandGroup_D_Command[32];
sprintf(tmp_PK_CommandGroup_D_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_CommandGroup_D_Command`=" + tmp_PK_CommandGroup_D_Command;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_CommandGroup_D_Command`="+pRow->PK_CommandGroup_D_Command_asSQL()+", `FK_CommandGroup_D`="+pRow->FK_CommandGroup_D_asSQL()+", `FK_Command`="+pRow->FK_Command_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `FK_DeviceCategory`="+pRow->FK_DeviceCategory_asSQL()+", `BroadcastLevel`="+pRow->BroadcastLevel_asSQL()+", `RelativeToSender`="+pRow->RelativeToSender_asSQL()+", `OrderNum`="+pRow->OrderNum_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update CommandGroup_D_Command set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
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
		Row_CommandGroup_D_Command* pRow = (Row_CommandGroup_D_Command*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_CommandGroup_D_Command* pRow = (Row_CommandGroup_D_Command*) (*i).second;	

		char tmp_PK_CommandGroup_D_Command[32];
sprintf(tmp_PK_CommandGroup_D_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_CommandGroup_D_Command`=" + tmp_PK_CommandGroup_D_Command;

	
		string query = "delete from CommandGroup_D_Command where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_CommandGroup_D_Command*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_CommandGroup_D_Command::GetRows(string where_statement,vector<class Row_CommandGroup_D_Command*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from CommandGroup_D_Command " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from CommandGroup_D_Command where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
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

		Row_CommandGroup_D_Command *pRow = new Row_CommandGroup_D_Command(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CommandGroup_D_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CommandGroup_D_Command));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandGroup_D = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandGroup_D));
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
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_BroadcastLevel = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_BroadcastLevel));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_RelativeToSender = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_RelativeToSender));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_OrderNum = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_OrderNum));
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



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_CommandGroup_D_Command);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CommandGroup_D_Command *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_CommandGroup_D_Command* Table_CommandGroup_D_Command::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CommandGroup_D_Command *pRow = new Row_CommandGroup_D_Command(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CommandGroup_D_Command* Table_CommandGroup_D_Command::GetRow(long int in_PK_CommandGroup_D_Command)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_CommandGroup_D_Command);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CommandGroup_D_Command*) (*i).second;
	//we have to fetch row
	Row_CommandGroup_D_Command* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CommandGroup_D_Command* Table_CommandGroup_D_Command::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CommandGroup_D_Command[32];
sprintf(tmp_PK_CommandGroup_D_Command, "%li", key.pk);


string condition;
condition = condition + "`PK_CommandGroup_D_Command`=" + tmp_PK_CommandGroup_D_Command;


	string query = "select * from CommandGroup_D_Command where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
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

	Row_CommandGroup_D_Command *pRow = new Row_CommandGroup_D_Command(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CommandGroup_D_Command = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CommandGroup_D_Command));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandGroup_D = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandGroup_D));
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
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_BroadcastLevel = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_BroadcastLevel));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_RelativeToSender = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_RelativeToSender));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_OrderNum = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_OrderNum));
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



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CommandGroup_D* Row_CommandGroup_D_Command::FK_CommandGroup_D_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D);
}
class Row_Command* Row_CommandGroup_D_Command::FK_Command_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Command *pTable = table->database->Command_get();
return pTable->GetRow(m_FK_Command);
}
class Row_DeviceTemplate* Row_CommandGroup_D_Command::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_DeviceCategory* Row_CommandGroup_D_Command::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}


void Row_CommandGroup_D_Command::CommandGroup_D_Command_CommandParameter_FK_CommandGroup_D_Command_getrows(vector <class Row_CommandGroup_D_Command_CommandParameter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_D_Command_CommandParameter *pTable = table->database->CommandGroup_D_Command_CommandParameter_get();
pTable->GetRows("`FK_CommandGroup_D_Command=`" + StringUtils::itos(m_PK_CommandGroup_D_Command),rows);
}



