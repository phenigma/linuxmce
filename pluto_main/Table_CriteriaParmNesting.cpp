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
#include "Table_CriteriaParmNesting.h"
#include "Table_CriteriaParmNesting.h"

#include "Table_Criteria.h"
#include "Table_CriteriaParm.h"
#include "Table_CriteriaParmNesting.h"


void Database_pluto_main::CreateTable_CriteriaParmNesting()
{
	tblCriteriaParmNesting = new Table_CriteriaParmNesting(this);
}

void Database_pluto_main::DeleteTable_CriteriaParmNesting()
{
	delete tblCriteriaParmNesting;
}

Table_CriteriaParmNesting::~Table_CriteriaParmNesting()
{
	map<Table_CriteriaParmNesting::Key, class Row_CriteriaParmNesting*, Table_CriteriaParmNesting::Key_Less>::iterator it;
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


void Row_CriteriaParmNesting::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_CriteriaParmNesting*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_CriteriaParmNesting::Key key(this);					
			map<Table_CriteriaParmNesting::Key, Row_CriteriaParmNesting*, Table_CriteriaParmNesting::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CriteriaParmNesting::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_CriteriaParmNesting::Key key(this);		
		Row_CriteriaParmNesting *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CriteriaParmNesting::Row_CriteriaParmNesting(Table_CriteriaParmNesting *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CriteriaParmNesting::SetDefaultValues()
{
	m_PK_CriteriaParmNesting = 0;
is_null[0] = false;
is_null[1] = true;
m_IsAnd = 0;
is_null[2] = false;
m_IsNot = 0;
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

long int Row_CriteriaParmNesting::PK_CriteriaParmNesting_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CriteriaParmNesting;}
long int Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmNesting_Parent;}
short int Row_CriteriaParmNesting::IsAnd_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsAnd;}
short int Row_CriteriaParmNesting::IsNot_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNot;}
string Row_CriteriaParmNesting::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_CriteriaParmNesting::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_CriteriaParmNesting::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_CriteriaParmNesting::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_CriteriaParmNesting::PK_CriteriaParmNesting_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CriteriaParmNesting = val; is_modified=true; is_null[0]=false;}
void Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmNesting_Parent = val; is_modified=true; is_null[1]=false;}
void Row_CriteriaParmNesting::IsAnd_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsAnd = val; is_modified=true; is_null[2]=false;}
void Row_CriteriaParmNesting::IsNot_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNot = val; is_modified=true; is_null[3]=false;}
void Row_CriteriaParmNesting::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_CriteriaParmNesting::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_CriteriaParmNesting::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_CriteriaParmNesting::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_CriteriaParmNesting::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_CriteriaParmNesting::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_CriteriaParmNesting::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_CriteriaParmNesting::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_CriteriaParmNesting::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_CriteriaParmNesting::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_CriteriaParmNesting::PK_CriteriaParmNesting_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CriteriaParmNesting);

return buf;
}

string Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmNesting_Parent);

return buf;
}

string Row_CriteriaParmNesting::IsAnd_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsAnd);

return buf;
}

string Row_CriteriaParmNesting::IsNot_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNot);

return buf;
}

string Row_CriteriaParmNesting::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_CriteriaParmNesting::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_CriteriaParmNesting::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_CriteriaParmNesting::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_CriteriaParmNesting::Key::Key(long int in_PK_CriteriaParmNesting)
{
			pk_PK_CriteriaParmNesting = in_PK_CriteriaParmNesting;
	
}

Table_CriteriaParmNesting::Key::Key(Row_CriteriaParmNesting *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CriteriaParmNesting = pRow->m_PK_CriteriaParmNesting;
	
}		

bool Table_CriteriaParmNesting::Key_Less::operator()(const Table_CriteriaParmNesting::Key &key1, const Table_CriteriaParmNesting::Key &key2) const
{
			if (key1.pk_PK_CriteriaParmNesting!=key2.pk_PK_CriteriaParmNesting)
return key1.pk_PK_CriteriaParmNesting<key2.pk_PK_CriteriaParmNesting;
else
return false;	
}	

void Table_CriteriaParmNesting::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_CriteriaParmNesting*>::iterator i = addedRows.begin();
	
		Row_CriteriaParmNesting *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CriteriaParmNesting_asSQL()+", "+pRow->FK_CriteriaParmNesting_Parent_asSQL()+", "+pRow->IsAnd_asSQL()+", "+pRow->IsNot_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into CriteriaParmNesting (PK_CriteriaParmNesting, FK_CriteriaParmNesting_Parent, IsAnd, IsNot, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CriteriaParmNesting=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_CriteriaParmNesting*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_CriteriaParmNesting* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_CriteriaParmNesting[32];
sprintf(tmp_PK_CriteriaParmNesting, "%li", key.pk_PK_CriteriaParmNesting);


string condition;
condition = condition + "PK_CriteriaParmNesting=" + tmp_PK_CriteriaParmNesting;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_CriteriaParmNesting="+pRow->PK_CriteriaParmNesting_asSQL()+", FK_CriteriaParmNesting_Parent="+pRow->FK_CriteriaParmNesting_Parent_asSQL()+", IsAnd="+pRow->IsAnd_asSQL()+", IsNot="+pRow->IsNot_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update CriteriaParmNesting set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_CriteriaParmNesting*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_CriteriaParmNesting*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_CriteriaParmNesting[32];
sprintf(tmp_PK_CriteriaParmNesting, "%li", key.pk_PK_CriteriaParmNesting);


string condition;
condition = condition + "PK_CriteriaParmNesting=" + tmp_PK_CriteriaParmNesting;

	
		string query = "delete from CriteriaParmNesting where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CriteriaParmNesting::GetRows(string where_statement,vector<class Row_CriteriaParmNesting*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from CriteriaParmNesting where " + where_statement;
		
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

		Row_CriteriaParmNesting *pRow = new Row_CriteriaParmNesting(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParmNesting = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParmNesting));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting_Parent = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting_Parent));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IsAnd = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_IsAnd));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsNot = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsNot));
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
		
                map<Table_CriteriaParmNesting::Key, Row_CriteriaParmNesting*, Table_CriteriaParmNesting::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_CriteriaParmNesting* Table_CriteriaParmNesting::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CriteriaParmNesting *pRow = new Row_CriteriaParmNesting(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CriteriaParmNesting* Table_CriteriaParmNesting::GetRow(long int in_PK_CriteriaParmNesting)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_CriteriaParmNesting);

	map<Key, Row_CriteriaParmNesting*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_CriteriaParmNesting* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CriteriaParmNesting* Table_CriteriaParmNesting::FetchRow(Table_CriteriaParmNesting::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CriteriaParmNesting[32];
sprintf(tmp_PK_CriteriaParmNesting, "%li", key.pk_PK_CriteriaParmNesting);


string condition;
condition = condition + "PK_CriteriaParmNesting=" + tmp_PK_CriteriaParmNesting;


	string query = "select * from CriteriaParmNesting where " + condition;		

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

	Row_CriteriaParmNesting *pRow = new Row_CriteriaParmNesting(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CriteriaParmNesting = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CriteriaParmNesting));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting_Parent = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting_Parent));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IsAnd = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_IsAnd));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsNot = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsNot));
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


class Row_CriteriaParmNesting* Row_CriteriaParmNesting::FK_CriteriaParmNesting_Parent_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmNesting *pTable = table->database->CriteriaParmNesting_get();
return pTable->GetRow(m_FK_CriteriaParmNesting_Parent);
}


void Row_CriteriaParmNesting::Criteria_FK_CriteriaParmNesting_getrows(vector <class Row_Criteria*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Criteria *pTable = table->database->Criteria_get();
pTable->GetRows("FK_CriteriaParmNesting=" + StringUtils::itos(m_PK_CriteriaParmNesting),rows);
}
void Row_CriteriaParmNesting::CriteriaParm_FK_CriteriaParmNesting_getrows(vector <class Row_CriteriaParm*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParm *pTable = table->database->CriteriaParm_get();
pTable->GetRows("FK_CriteriaParmNesting=" + StringUtils::itos(m_PK_CriteriaParmNesting),rows);
}
void Row_CriteriaParmNesting::CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(vector <class Row_CriteriaParmNesting*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmNesting *pTable = table->database->CriteriaParmNesting_get();
pTable->GetRows("FK_CriteriaParmNesting_Parent=" + StringUtils::itos(m_PK_CriteriaParmNesting),rows);
}



