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
#include "Table_CriteriaParmList.h"
#include "Table_ParameterType.h"

#include "Table_CannedEvents_CriteriaParmList.h"
#include "Table_CriteriaList_CriteriaParmList.h"
#include "Table_CriteriaParm.h"
#include "Table_CriteriaParm_D.h"


void Database_pluto_main::CreateTable_CriteriaParmList()
{
	tblCriteriaParmList = new Table_CriteriaParmList(this);
}

void Database_pluto_main::DeleteTable_CriteriaParmList()
{
	delete tblCriteriaParmList;
}

Table_CriteriaParmList::~Table_CriteriaParmList()
{
	map<Table_CriteriaParmList::Key, class Row_CriteriaParmList*, Table_CriteriaParmList::Key_Less>::iterator it;
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


void Row_CriteriaParmList::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_CriteriaParmList*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_CriteriaParmList::Key key(this);					
			map<Table_CriteriaParmList::Key, Row_CriteriaParmList*, Table_CriteriaParmList::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CriteriaParmList::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_CriteriaParmList::Key key(this);		
		Row_CriteriaParmList *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CriteriaParmList::Row_CriteriaParmList(Table_CriteriaParmList *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CriteriaParmList::SetDefaultValues()
{
	m_PK_CriteriaParmList = 0;
is_null[0] = false;
m_FK_ParameterType = 0;
is_null[1] = false;
m_Description = "";
is_null[2] = false;
m_Define = "";
is_null[3] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[4] = false;
m_IsNew_RecordInfo = 1;
is_null[5] = false;
m_IsDeleted_RecordInfo = 0;
is_null[6] = false;
is_null[7] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CriteriaParmList::PK_CriteriaParmList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CriteriaParmList;}
long int Row_CriteriaParmList::FK_ParameterType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_ParameterType;}
string Row_CriteriaParmList::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_CriteriaParmList::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_CriteriaParmList::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_CriteriaParmList::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_CriteriaParmList::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_CriteriaParmList::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_CriteriaParmList::PK_CriteriaParmList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CriteriaParmList = val; is_modified=true; is_null[0]=false;}
void Row_CriteriaParmList::FK_ParameterType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_ParameterType = val; is_modified=true; is_null[1]=false;}
void Row_CriteriaParmList::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_CriteriaParmList::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[3]=false;}
void Row_CriteriaParmList::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_CriteriaParmList::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_CriteriaParmList::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_CriteriaParmList::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_CriteriaParmList::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_CriteriaParmList::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_CriteriaParmList::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_CriteriaParmList::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_CriteriaParmList::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_CriteriaParmList::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_CriteriaParmList::PK_CriteriaParmList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CriteriaParmList);

return buf;
}

string Row_CriteriaParmList::FK_ParameterType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_ParameterType);

return buf;
}

string Row_CriteriaParmList::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParmList::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParmList::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParmList::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_CriteriaParmList::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_CriteriaParmList::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_CriteriaParmList::Key::Key(long int in_PK_CriteriaParmList)
{
			pk_PK_CriteriaParmList = in_PK_CriteriaParmList;
	
}

Table_CriteriaParmList::Key::Key(Row_CriteriaParmList *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CriteriaParmList = pRow->m_PK_CriteriaParmList;
	
}		

bool Table_CriteriaParmList::Key_Less::operator()(const Table_CriteriaParmList::Key &key1, const Table_CriteriaParmList::Key &key2) const
{
			if (key1.pk_PK_CriteriaParmList!=key2.pk_PK_CriteriaParmList)
return key1.pk_PK_CriteriaParmList<key2.pk_PK_CriteriaParmList;
else
return false;	
}	

void Table_CriteriaParmList::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_CriteriaParmList*>::iterator i = addedRows.begin();
	
		Row_CriteriaParmList *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CriteriaParmList_asSQL()+", "+pRow->FK_ParameterType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into CriteriaParmList (PK_CriteriaParmList, FK_ParameterType, Description, Define, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CriteriaParmList=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_CriteriaParmList*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_CriteriaParmList* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_CriteriaParmList[32];
sprintf(tmp_PK_CriteriaParmList, "%li", key.pk_PK_CriteriaParmList);


string condition;
condition = condition + "PK_CriteriaParmList=" + tmp_PK_CriteriaParmList;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_CriteriaParmList="+pRow->PK_CriteriaParmList_asSQL()+", FK_ParameterType="+pRow->FK_ParameterType_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update CriteriaParmList set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_CriteriaParmList*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_CriteriaParmList*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_CriteriaParmList[32];
sprintf(tmp_PK_CriteriaParmList, "%li", key.pk_PK_CriteriaParmList);


string condition;
condition = condition + "PK_CriteriaParmList=" + tmp_PK_CriteriaParmList;

	
		string query = "delete from CriteriaParmList where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CriteriaParmList::GetRows(string where_statement,vector<class Row_CriteriaParmList*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from CriteriaParmList where " + where_statement;
		
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

		Row_CriteriaParmList *pRow = new Row_CriteriaParmList(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParmList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParmList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ParameterType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ParameterType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_CriteriaParmList::Key, Row_CriteriaParmList*, Table_CriteriaParmList::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_CriteriaParmList* Table_CriteriaParmList::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CriteriaParmList *pRow = new Row_CriteriaParmList(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CriteriaParmList* Table_CriteriaParmList::GetRow(long int in_PK_CriteriaParmList)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_CriteriaParmList);

	map<Key, Row_CriteriaParmList*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_CriteriaParmList* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CriteriaParmList* Table_CriteriaParmList::FetchRow(Table_CriteriaParmList::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CriteriaParmList[32];
sprintf(tmp_PK_CriteriaParmList, "%li", key.pk_PK_CriteriaParmList);


string condition;
condition = condition + "PK_CriteriaParmList=" + tmp_PK_CriteriaParmList;


	string query = "select * from CriteriaParmList where " + condition;		

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

	Row_CriteriaParmList *pRow = new Row_CriteriaParmList(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParmList = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParmList));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ParameterType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ParameterType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_ParameterType* Row_CriteriaParmList::FK_ParameterType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_ParameterType *pTable = table->database->ParameterType_get();
return pTable->GetRow(m_FK_ParameterType);
}


void Row_CriteriaParmList::CannedEvents_CriteriaParmList_FK_CriteriaParmList_getrows(vector <class Row_CannedEvents_CriteriaParmList*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CannedEvents_CriteriaParmList *pTable = table->database->CannedEvents_CriteriaParmList_get();
pTable->GetRows("FK_CriteriaParmList=" + StringUtils::itos(m_PK_CriteriaParmList),rows);
}
void Row_CriteriaParmList::CriteriaList_CriteriaParmList_FK_CriteriaParmList_getrows(vector <class Row_CriteriaList_CriteriaParmList*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaList_CriteriaParmList *pTable = table->database->CriteriaList_CriteriaParmList_get();
pTable->GetRows("FK_CriteriaParmList=" + StringUtils::itos(m_PK_CriteriaParmList),rows);
}
void Row_CriteriaParmList::CriteriaParm_FK_CriteriaParmList_getrows(vector <class Row_CriteriaParm*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParm *pTable = table->database->CriteriaParm_get();
pTable->GetRows("FK_CriteriaParmList=" + StringUtils::itos(m_PK_CriteriaParmList),rows);
}
void Row_CriteriaParmList::CriteriaParm_D_FK_CriteriaParmList_getrows(vector <class Row_CriteriaParm_D*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParm_D *pTable = table->database->CriteriaParm_D_get();
pTable->GetRows("FK_CriteriaParmList=" + StringUtils::itos(m_PK_CriteriaParmList),rows);
}



