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
#include "Table_CannedEvents_CriteriaParmList.h"
#include "Table_CannedEvents.h"
#include "Table_CriteriaParmList.h"

#include "Table_CriteriaParm.h"
#include "Table_CriteriaParm_D.h"


void Database_pluto_main::CreateTable_CannedEvents_CriteriaParmList()
{
	tblCannedEvents_CriteriaParmList = new Table_CannedEvents_CriteriaParmList(this);
}

void Database_pluto_main::DeleteTable_CannedEvents_CriteriaParmList()
{
	delete tblCannedEvents_CriteriaParmList;
}

Table_CannedEvents_CriteriaParmList::~Table_CannedEvents_CriteriaParmList()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CannedEvents_CriteriaParmList *pRow = (Row_CannedEvents_CriteriaParmList *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CannedEvents_CriteriaParmList *pRow = (Row_CannedEvents_CriteriaParmList *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CannedEvents_CriteriaParmList::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_CannedEvents_CriteriaParmList *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CannedEvents_CriteriaParmList *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_CannedEvents_CriteriaParmList);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CannedEvents_CriteriaParmList::Reload()
{
	Row_CannedEvents_CriteriaParmList *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_CannedEvents_CriteriaParmList);
		Row_CannedEvents_CriteriaParmList *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CannedEvents_CriteriaParmList::Row_CannedEvents_CriteriaParmList(Table_CannedEvents_CriteriaParmList *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CannedEvents_CriteriaParmList::SetDefaultValues()
{
	m_PK_CannedEvents_CriteriaParmList = 0;
is_null[0] = false;
m_FK_CannedEvents = 0;
is_null[1] = false;
m_FK_CriteriaParmList = 0;
is_null[2] = false;
m_Description = "";
is_null[3] = false;
is_null[4] = true;
m_Operator = 0;
is_null[5] = false;
is_null[6] = true;
is_null[7] = true;
m_Required = 0;
is_null[8] = false;
is_null[9] = true;
is_null[10] = true;
is_null[11] = true;
m_psc_frozen = 0;
is_null[12] = false;
m_psc_mod = "00000000000000";
is_null[13] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CannedEvents_CriteriaParmList::PK_CannedEvents_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CannedEvents_CriteriaParmList;}
long int Row_CannedEvents_CriteriaParmList::FK_CannedEvents_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CannedEvents;}
long int Row_CannedEvents_CriteriaParmList::FK_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmList;}
string Row_CannedEvents_CriteriaParmList::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_CannedEvents_CriteriaParmList::Comments_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Comments;}
short int Row_CannedEvents_CriteriaParmList::Operator_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Operator;}
string Row_CannedEvents_CriteriaParmList::DefaultValue_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DefaultValue;}
string Row_CannedEvents_CriteriaParmList::ExtraInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ExtraInfo;}
short int Row_CannedEvents_CriteriaParmList::Required_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Required;}
long int Row_CannedEvents_CriteriaParmList::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_CannedEvents_CriteriaParmList::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_CannedEvents_CriteriaParmList::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_CannedEvents_CriteriaParmList::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_CannedEvents_CriteriaParmList::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_CannedEvents_CriteriaParmList::PK_CannedEvents_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CannedEvents_CriteriaParmList = val; is_modified=true; is_null[0]=false;}
void Row_CannedEvents_CriteriaParmList::FK_CannedEvents_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CannedEvents = val; is_modified=true; is_null[1]=false;}
void Row_CannedEvents_CriteriaParmList::FK_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmList = val; is_modified=true; is_null[2]=false;}
void Row_CannedEvents_CriteriaParmList::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_CannedEvents_CriteriaParmList::Comments_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Comments = val; is_modified=true; is_null[4]=false;}
void Row_CannedEvents_CriteriaParmList::Operator_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Operator = val; is_modified=true; is_null[5]=false;}
void Row_CannedEvents_CriteriaParmList::DefaultValue_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DefaultValue = val; is_modified=true; is_null[6]=false;}
void Row_CannedEvents_CriteriaParmList::ExtraInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ExtraInfo = val; is_modified=true; is_null[7]=false;}
void Row_CannedEvents_CriteriaParmList::Required_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Required = val; is_modified=true; is_null[8]=false;}
void Row_CannedEvents_CriteriaParmList::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[9]=false;}
void Row_CannedEvents_CriteriaParmList::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[10]=false;}
void Row_CannedEvents_CriteriaParmList::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[11]=false;}
void Row_CannedEvents_CriteriaParmList::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[12]=false;}
void Row_CannedEvents_CriteriaParmList::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[13]=false;}

		
bool Row_CannedEvents_CriteriaParmList::Comments_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_CannedEvents_CriteriaParmList::DefaultValue_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_CannedEvents_CriteriaParmList::ExtraInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_CannedEvents_CriteriaParmList::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_CannedEvents_CriteriaParmList::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_CannedEvents_CriteriaParmList::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_CannedEvents_CriteriaParmList::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}

			
void Row_CannedEvents_CriteriaParmList::Comments_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_CannedEvents_CriteriaParmList::DefaultValue_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_CannedEvents_CriteriaParmList::ExtraInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_CannedEvents_CriteriaParmList::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_CannedEvents_CriteriaParmList::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_CannedEvents_CriteriaParmList::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_CannedEvents_CriteriaParmList::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
	

string Row_CannedEvents_CriteriaParmList::PK_CannedEvents_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CannedEvents_CriteriaParmList);

return buf;
}

string Row_CannedEvents_CriteriaParmList::FK_CannedEvents_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CannedEvents);

return buf;
}

string Row_CannedEvents_CriteriaParmList::FK_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmList);

return buf;
}

string Row_CannedEvents_CriteriaParmList::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_CannedEvents_CriteriaParmList::Comments_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Comments.c_str(), (unsigned long) m_Comments.size());
return string()+"\""+buf+"\"";
}

string Row_CannedEvents_CriteriaParmList::Operator_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Operator);

return buf;
}

string Row_CannedEvents_CriteriaParmList::DefaultValue_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_DefaultValue.c_str(), (unsigned long) m_DefaultValue.size());
return string()+"\""+buf+"\"";
}

string Row_CannedEvents_CriteriaParmList::ExtraInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_ExtraInfo.c_str(), (unsigned long) m_ExtraInfo.size());
return string()+"\""+buf+"\"";
}

string Row_CannedEvents_CriteriaParmList::Required_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Required);

return buf;
}

string Row_CannedEvents_CriteriaParmList::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_CannedEvents_CriteriaParmList::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_CannedEvents_CriteriaParmList::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_CannedEvents_CriteriaParmList::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_CannedEvents_CriteriaParmList::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_CannedEvents_CriteriaParmList::Key::Key(long int in_PK_CannedEvents_CriteriaParmList)
{
			pk_PK_CannedEvents_CriteriaParmList = in_PK_CannedEvents_CriteriaParmList;
	
}

Table_CannedEvents_CriteriaParmList::Key::Key(Row_CannedEvents_CriteriaParmList *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CannedEvents_CriteriaParmList = pRow->m_PK_CannedEvents_CriteriaParmList;
	
}		

bool Table_CannedEvents_CriteriaParmList::Key_Less::operator()(const Table_CannedEvents_CriteriaParmList::Key &key1, const Table_CannedEvents_CriteriaParmList::Key &key2) const
{
			if (key1.pk_PK_CannedEvents_CriteriaParmList!=key2.pk_PK_CannedEvents_CriteriaParmList)
return key1.pk_PK_CannedEvents_CriteriaParmList<key2.pk_PK_CannedEvents_CriteriaParmList;
else
return false;	
}	

void Table_CannedEvents_CriteriaParmList::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CannedEvents_CriteriaParmList *pRow = (Row_CannedEvents_CriteriaParmList *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CannedEvents_CriteriaParmList_asSQL()+", "+pRow->FK_CannedEvents_asSQL()+", "+pRow->FK_CriteriaParmList_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->Operator_asSQL()+", "+pRow->DefaultValue_asSQL()+", "+pRow->ExtraInfo_asSQL()+", "+pRow->Required_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into CannedEvents_CriteriaParmList (PK_CannedEvents_CriteriaParmList, FK_CannedEvents, FK_CriteriaParmList, Description, Comments, Operator, DefaultValue, ExtraInfo, Required, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CannedEvents_CriteriaParmList=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_CannedEvents_CriteriaParmList);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CannedEvents_CriteriaParmList* pRow = (Row_CannedEvents_CriteriaParmList*) (*i).second;	
		SingleLongKey key(pRow->m_PK_CannedEvents_CriteriaParmList);

		char tmp_PK_CannedEvents_CriteriaParmList[32];
sprintf(tmp_PK_CannedEvents_CriteriaParmList, "%li", key.pk);


string condition;
condition = condition + "PK_CannedEvents_CriteriaParmList=" + tmp_PK_CannedEvents_CriteriaParmList;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_CannedEvents_CriteriaParmList="+pRow->PK_CannedEvents_CriteriaParmList_asSQL()+", FK_CannedEvents="+pRow->FK_CannedEvents_asSQL()+", FK_CriteriaParmList="+pRow->FK_CriteriaParmList_asSQL()+", Description="+pRow->Description_asSQL()+", Comments="+pRow->Comments_asSQL()+", Operator="+pRow->Operator_asSQL()+", DefaultValue="+pRow->DefaultValue_asSQL()+", ExtraInfo="+pRow->ExtraInfo_asSQL()+", Required="+pRow->Required_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update CannedEvents_CriteriaParmList set " + update_values_list + " where " + condition;
			
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
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_CannedEvents_CriteriaParmList* pRow = (Row_CannedEvents_CriteriaParmList*) (*i).second;	

		char tmp_PK_CannedEvents_CriteriaParmList[32];
sprintf(tmp_PK_CannedEvents_CriteriaParmList, "%li", key.pk);


string condition;
condition = condition + "PK_CannedEvents_CriteriaParmList=" + tmp_PK_CannedEvents_CriteriaParmList;

	
		string query = "delete from CannedEvents_CriteriaParmList where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CannedEvents_CriteriaParmList::GetRows(string where_statement,vector<class Row_CannedEvents_CriteriaParmList*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from CannedEvents_CriteriaParmList " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from CannedEvents_CriteriaParmList where " + where_statement;
		
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

		Row_CannedEvents_CriteriaParmList *pRow = new Row_CannedEvents_CriteriaParmList(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CannedEvents_CriteriaParmList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CannedEvents_CriteriaParmList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CannedEvents = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CannedEvents));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_CriteriaParmList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaParmList));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Comments = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Operator = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Operator));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_DefaultValue = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_DefaultValue = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ExtraInfo = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ExtraInfo = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Required = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Required));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_CannedEvents_CriteriaParmList);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CannedEvents_CriteriaParmList *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_CannedEvents_CriteriaParmList* Table_CannedEvents_CriteriaParmList::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CannedEvents_CriteriaParmList *pRow = new Row_CannedEvents_CriteriaParmList(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CannedEvents_CriteriaParmList* Table_CannedEvents_CriteriaParmList::GetRow(long int in_PK_CannedEvents_CriteriaParmList)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_CannedEvents_CriteriaParmList);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CannedEvents_CriteriaParmList*) (*i).second;
	//we have to fetch row
	Row_CannedEvents_CriteriaParmList* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CannedEvents_CriteriaParmList* Table_CannedEvents_CriteriaParmList::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CannedEvents_CriteriaParmList[32];
sprintf(tmp_PK_CannedEvents_CriteriaParmList, "%li", key.pk);


string condition;
condition = condition + "PK_CannedEvents_CriteriaParmList=" + tmp_PK_CannedEvents_CriteriaParmList;


	string query = "select * from CannedEvents_CriteriaParmList where " + condition;		

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

	Row_CannedEvents_CriteriaParmList *pRow = new Row_CannedEvents_CriteriaParmList(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CannedEvents_CriteriaParmList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CannedEvents_CriteriaParmList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CannedEvents = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CannedEvents));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_CriteriaParmList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaParmList));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Comments = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Operator = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Operator));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_DefaultValue = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_DefaultValue = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ExtraInfo = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ExtraInfo = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Required = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Required));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CannedEvents* Row_CannedEvents_CriteriaParmList::FK_CannedEvents_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CannedEvents *pTable = table->database->CannedEvents_get();
return pTable->GetRow(m_FK_CannedEvents);
}
class Row_CriteriaParmList* Row_CannedEvents_CriteriaParmList::FK_CriteriaParmList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmList *pTable = table->database->CriteriaParmList_get();
return pTable->GetRow(m_FK_CriteriaParmList);
}


void Row_CannedEvents_CriteriaParmList::CriteriaParm_FK_CannedEvents_CriteriaParmList_getrows(vector <class Row_CriteriaParm*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParm *pTable = table->database->CriteriaParm_get();
pTable->GetRows("FK_CannedEvents_CriteriaParmList=" + StringUtils::itos(m_PK_CannedEvents_CriteriaParmList),rows);
}
void Row_CannedEvents_CriteriaParmList::CriteriaParm_D_FK_CannedEvents_CriteriaParmList_getrows(vector <class Row_CriteriaParm_D*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParm_D *pTable = table->database->CriteriaParm_D_get();
pTable->GetRows("FK_CannedEvents_CriteriaParmList=" + StringUtils::itos(m_PK_CannedEvents_CriteriaParmList),rows);
}



