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
#include "Table_CriteriaParm.h"
#include "Table_CriteriaParmNesting.h"
#include "Table_CriteriaParmList.h"
#include "Table_CannedEvents_CriteriaParmList.h"



void Database_pluto_main::CreateTable_CriteriaParm()
{
	tblCriteriaParm = new Table_CriteriaParm(this);
}

void Database_pluto_main::DeleteTable_CriteriaParm()
{
	delete tblCriteriaParm;
}

Table_CriteriaParm::~Table_CriteriaParm()
{
	map<Table_CriteriaParm::Key, class Row_CriteriaParm*, Table_CriteriaParm::Key_Less>::iterator it;
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


void Row_CriteriaParm::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_CriteriaParm*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_CriteriaParm::Key key(this);					
			map<Table_CriteriaParm::Key, Row_CriteriaParm*, Table_CriteriaParm::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CriteriaParm::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_CriteriaParm::Key key(this);		
		Row_CriteriaParm *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CriteriaParm::Row_CriteriaParm(Table_CriteriaParm *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CriteriaParm::SetDefaultValues()
{
	m_PK_CriteriaParm = 0;
is_null[0] = false;
m_FK_CriteriaParmNesting = 0;
is_null[1] = false;
m_FK_CriteriaParmList = 0;
is_null[2] = false;
m_Operator = 0;
is_null[3] = false;
m_Value = "";
is_null[4] = false;
is_null[5] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[6] = false;
m_IsNew_RecordInfo = 1;
is_null[7] = false;
m_IsDeleted_RecordInfo = 0;
is_null[8] = false;
is_null[9] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CriteriaParm::PK_CriteriaParm_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CriteriaParm;}
long int Row_CriteriaParm::FK_CriteriaParmNesting_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmNesting;}
long int Row_CriteriaParm::FK_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmList;}
long int Row_CriteriaParm::Operator_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Operator;}
string Row_CriteriaParm::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}
long int Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CannedEvents_CriteriaParmList;}
string Row_CriteriaParm::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_CriteriaParm::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_CriteriaParm::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_CriteriaParm::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_CriteriaParm::PK_CriteriaParm_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CriteriaParm = val; is_modified=true; is_null[0]=false;}
void Row_CriteriaParm::FK_CriteriaParmNesting_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmNesting = val; is_modified=true; is_null[1]=false;}
void Row_CriteriaParm::FK_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmList = val; is_modified=true; is_null[2]=false;}
void Row_CriteriaParm::Operator_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Operator = val; is_modified=true; is_null[3]=false;}
void Row_CriteriaParm::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[4]=false;}
void Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CannedEvents_CriteriaParmList = val; is_modified=true; is_null[5]=false;}
void Row_CriteriaParm::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_CriteriaParm::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_CriteriaParm::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_CriteriaParm::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[9]=false;}

		
bool Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_CriteriaParm::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_CriteriaParm::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_CriteriaParm::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_CriteriaParm::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_CriteriaParm::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_CriteriaParm::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_CriteriaParm::PK_CriteriaParm_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CriteriaParm);

return buf;
}

string Row_CriteriaParm::FK_CriteriaParmNesting_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmNesting);

return buf;
}

string Row_CriteriaParm::FK_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmList);

return buf;
}

string Row_CriteriaParm::Operator_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Operator);

return buf;
}

string Row_CriteriaParm::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CannedEvents_CriteriaParmList);

return buf;
}

string Row_CriteriaParm::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParm::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_CriteriaParm::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_CriteriaParm::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_CriteriaParm::Key::Key(long int in_PK_CriteriaParm)
{
			pk_PK_CriteriaParm = in_PK_CriteriaParm;
	
}

Table_CriteriaParm::Key::Key(Row_CriteriaParm *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CriteriaParm = pRow->m_PK_CriteriaParm;
	
}		

bool Table_CriteriaParm::Key_Less::operator()(const Table_CriteriaParm::Key &key1, const Table_CriteriaParm::Key &key2) const
{
			if (key1.pk_PK_CriteriaParm!=key2.pk_PK_CriteriaParm)
return key1.pk_PK_CriteriaParm<key2.pk_PK_CriteriaParm;
else
return false;	
}	

void Table_CriteriaParm::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_CriteriaParm*>::iterator i = addedRows.begin();
	
		Row_CriteriaParm *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CriteriaParm_asSQL()+", "+pRow->FK_CriteriaParmNesting_asSQL()+", "+pRow->FK_CriteriaParmList_asSQL()+", "+pRow->Operator_asSQL()+", "+pRow->Value_asSQL()+", "+pRow->FK_CannedEvents_CriteriaParmList_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into CriteriaParm (PK_CriteriaParm, FK_CriteriaParmNesting, FK_CriteriaParmList, Operator, Value, FK_CannedEvents_CriteriaParmList, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CriteriaParm=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_CriteriaParm*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_CriteriaParm* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_CriteriaParm[32];
sprintf(tmp_PK_CriteriaParm, "%li", key.pk_PK_CriteriaParm);


string condition;
condition = condition + "PK_CriteriaParm=" + tmp_PK_CriteriaParm;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_CriteriaParm="+pRow->PK_CriteriaParm_asSQL()+", FK_CriteriaParmNesting="+pRow->FK_CriteriaParmNesting_asSQL()+", FK_CriteriaParmList="+pRow->FK_CriteriaParmList_asSQL()+", Operator="+pRow->Operator_asSQL()+", Value="+pRow->Value_asSQL()+", FK_CannedEvents_CriteriaParmList="+pRow->FK_CannedEvents_CriteriaParmList_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update CriteriaParm set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_CriteriaParm*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_CriteriaParm*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_CriteriaParm[32];
sprintf(tmp_PK_CriteriaParm, "%li", key.pk_PK_CriteriaParm);


string condition;
condition = condition + "PK_CriteriaParm=" + tmp_PK_CriteriaParm;

	
		string query = "delete from CriteriaParm where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CriteriaParm::GetRows(string where_statement,vector<class Row_CriteriaParm*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from CriteriaParm where " + where_statement;
		
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

		Row_CriteriaParm *pRow = new Row_CriteriaParm(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParm = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParm));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting));
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
pRow->m_Operator = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Operator));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Value = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_CannedEvents_CriteriaParmList = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_CannedEvents_CriteriaParmList));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Modification_RecordInfo = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_CriteriaParm::Key, Row_CriteriaParm*, Table_CriteriaParm::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_CriteriaParm* Table_CriteriaParm::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CriteriaParm *pRow = new Row_CriteriaParm(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CriteriaParm* Table_CriteriaParm::GetRow(long int in_PK_CriteriaParm)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_CriteriaParm);

	map<Key, Row_CriteriaParm*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_CriteriaParm* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CriteriaParm* Table_CriteriaParm::FetchRow(Table_CriteriaParm::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CriteriaParm[32];
sprintf(tmp_PK_CriteriaParm, "%li", key.pk_PK_CriteriaParm);


string condition;
condition = condition + "PK_CriteriaParm=" + tmp_PK_CriteriaParm;


	string query = "select * from CriteriaParm where " + condition;		

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

	Row_CriteriaParm *pRow = new Row_CriteriaParm(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParm = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParm));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting));
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
pRow->m_Operator = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Operator));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Value = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_CannedEvents_CriteriaParmList = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_CannedEvents_CriteriaParmList));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Modification_RecordInfo = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CriteriaParmNesting* Row_CriteriaParm::FK_CriteriaParmNesting_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmNesting *pTable = table->database->CriteriaParmNesting_get();
return pTable->GetRow(m_FK_CriteriaParmNesting);
}
class Row_CriteriaParmList* Row_CriteriaParm::FK_CriteriaParmList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmList *pTable = table->database->CriteriaParmList_get();
return pTable->GetRow(m_FK_CriteriaParmList);
}
class Row_CannedEvents_CriteriaParmList* Row_CriteriaParm::FK_CannedEvents_CriteriaParmList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CannedEvents_CriteriaParmList *pTable = table->database->CannedEvents_CriteriaParmList_get();
return pTable->GetRow(m_FK_CannedEvents_CriteriaParmList);
}





