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
#include "Table_Criteria.h"
#include "Table_CriteriaParmNesting.h"
#include "Table_CriteriaList.h"

#include "Table_CommandGroup.h"
#include "Table_CommandGroup_D.h"
#include "Table_DesignObjVariation.h"
#include "Table_EventHandler.h"


void Database_pluto_main::CreateTable_Criteria()
{
	tblCriteria = new Table_Criteria(this);
}

void Database_pluto_main::DeleteTable_Criteria()
{
	delete tblCriteria;
}

Table_Criteria::~Table_Criteria()
{
	map<Table_Criteria::Key, class Row_Criteria*, Table_Criteria::Key_Less>::iterator it;
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


void Row_Criteria::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Criteria*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Criteria::Key key(this);					
			map<Table_Criteria::Key, Row_Criteria*, Table_Criteria::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Criteria::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Criteria::Key key(this);		
		Row_Criteria *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Criteria::Row_Criteria(Table_Criteria *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Criteria::SetDefaultValues()
{
	m_PK_Criteria = 0;
is_null[0] = false;
m_FK_CriteriaParmNesting = 0;
is_null[1] = false;
m_FK_CriteriaList = 0;
is_null[2] = false;
m_Description = "";
is_null[3] = false;
m_Define = "";
is_null[4] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[5] = false;
m_IsNew_RecordInfo = 1;
is_null[6] = false;
m_IsDeleted_RecordInfo = 0;
is_null[7] = false;
is_null[8] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Criteria::PK_Criteria_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Criteria;}
long int Row_Criteria::FK_CriteriaParmNesting_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmNesting;}
long int Row_Criteria::FK_CriteriaList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaList;}
string Row_Criteria::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Criteria::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_Criteria::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Criteria::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Criteria::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Criteria::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Criteria::PK_Criteria_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Criteria = val; is_modified=true; is_null[0]=false;}
void Row_Criteria::FK_CriteriaParmNesting_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmNesting = val; is_modified=true; is_null[1]=false;}
void Row_Criteria::FK_CriteriaList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaList = val; is_modified=true; is_null[2]=false;}
void Row_Criteria::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_Criteria::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[4]=false;}
void Row_Criteria::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Criteria::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Criteria::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_Criteria::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[8]=false;}

		
bool Row_Criteria::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Criteria::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Criteria::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_Criteria::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Criteria::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Criteria::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_Criteria::PK_Criteria_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Criteria);

return buf;
}

string Row_Criteria::FK_CriteriaParmNesting_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmNesting);

return buf;
}

string Row_Criteria::FK_CriteriaList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaList);

return buf;
}

string Row_Criteria::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Criteria::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_Criteria::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Criteria::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Criteria::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Criteria::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Criteria::Key::Key(long int in_PK_Criteria)
{
			pk_PK_Criteria = in_PK_Criteria;
	
}

Table_Criteria::Key::Key(Row_Criteria *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Criteria = pRow->m_PK_Criteria;
	
}		

bool Table_Criteria::Key_Less::operator()(const Table_Criteria::Key &key1, const Table_Criteria::Key &key2) const
{
			if (key1.pk_PK_Criteria!=key2.pk_PK_Criteria)
return key1.pk_PK_Criteria<key2.pk_PK_Criteria;
else
return false;	
}	

void Table_Criteria::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Criteria*>::iterator i = addedRows.begin();
	
		Row_Criteria *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Criteria_asSQL()+", "+pRow->FK_CriteriaParmNesting_asSQL()+", "+pRow->FK_CriteriaList_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Criteria (PK_Criteria, FK_CriteriaParmNesting, FK_CriteriaList, Description, Define, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Criteria=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Criteria*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Criteria* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Criteria[32];
sprintf(tmp_PK_Criteria, "%li", key.pk_PK_Criteria);


string condition;
condition = condition + "PK_Criteria=" + tmp_PK_Criteria;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Criteria="+pRow->PK_Criteria_asSQL()+", FK_CriteriaParmNesting="+pRow->FK_CriteriaParmNesting_asSQL()+", FK_CriteriaList="+pRow->FK_CriteriaList_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Criteria set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Criteria*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Criteria*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Criteria[32];
sprintf(tmp_PK_Criteria, "%li", key.pk_PK_Criteria);


string condition;
condition = condition + "PK_Criteria=" + tmp_PK_Criteria;

	
		string query = "delete from Criteria where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Criteria::GetRows(string where_statement,vector<class Row_Criteria*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Criteria where " + where_statement;
		
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

		Row_Criteria *pRow = new Row_Criteria(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Criteria = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Criteria));
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
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaList));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Define = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Modification_RecordInfo = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Criteria::Key, Row_Criteria*, Table_Criteria::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Criteria* Table_Criteria::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Criteria *pRow = new Row_Criteria(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Criteria* Table_Criteria::GetRow(long int in_PK_Criteria)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Criteria);

	map<Key, Row_Criteria*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Criteria* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Criteria* Table_Criteria::FetchRow(Table_Criteria::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Criteria[32];
sprintf(tmp_PK_Criteria, "%li", key.pk_PK_Criteria);


string condition;
condition = condition + "PK_Criteria=" + tmp_PK_Criteria;


	string query = "select * from Criteria where " + condition;		

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

	Row_Criteria *pRow = new Row_Criteria(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Criteria = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Criteria));
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
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaList));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Define = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Modification_RecordInfo = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CriteriaParmNesting* Row_Criteria::FK_CriteriaParmNesting_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmNesting *pTable = table->database->CriteriaParmNesting_get();
return pTable->GetRow(m_FK_CriteriaParmNesting);
}
class Row_CriteriaList* Row_Criteria::FK_CriteriaList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaList *pTable = table->database->CriteriaList_get();
return pTable->GetRow(m_FK_CriteriaList);
}


void Row_Criteria::CommandGroup_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup *pTable = table->database->CommandGroup_get();
pTable->GetRows("FK_Criteria_Orbiter=" + StringUtils::itos(m_PK_Criteria),rows);
}
void Row_Criteria::CommandGroup_D_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup_D*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
pTable->GetRows("FK_Criteria_Orbiter=" + StringUtils::itos(m_PK_Criteria),rows);
}
void Row_Criteria::DesignObjVariation_FK_Criteria_Orbiter_getrows(vector <class Row_DesignObjVariation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
pTable->GetRows("FK_Criteria_Orbiter=" + StringUtils::itos(m_PK_Criteria),rows);
}
void Row_Criteria::EventHandler_FK_Criteria_getrows(vector <class Row_EventHandler*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EventHandler *pTable = table->database->EventHandler_get();
pTable->GetRows("FK_Criteria=" + StringUtils::itos(m_PK_Criteria),rows);
}



