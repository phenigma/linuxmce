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
#include "Table_Orbiter_Users_PasswordReq.h"
#include "Table_Orbiter.h"
#include "Table_Users.h"



void Database_pluto_main::CreateTable_Orbiter_Users_PasswordReq()
{
	tblOrbiter_Users_PasswordReq = new Table_Orbiter_Users_PasswordReq(this);
}

void Database_pluto_main::DeleteTable_Orbiter_Users_PasswordReq()
{
	delete tblOrbiter_Users_PasswordReq;
}

Table_Orbiter_Users_PasswordReq::~Table_Orbiter_Users_PasswordReq()
{
	map<Table_Orbiter_Users_PasswordReq::Key, class Row_Orbiter_Users_PasswordReq*, Table_Orbiter_Users_PasswordReq::Key_Less>::iterator it;
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


void Row_Orbiter_Users_PasswordReq::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Orbiter_Users_PasswordReq*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Orbiter_Users_PasswordReq::Key key(this);					
			map<Table_Orbiter_Users_PasswordReq::Key, Row_Orbiter_Users_PasswordReq*, Table_Orbiter_Users_PasswordReq::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Orbiter_Users_PasswordReq::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Orbiter_Users_PasswordReq::Key key(this);		
		Row_Orbiter_Users_PasswordReq *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Orbiter_Users_PasswordReq::Row_Orbiter_Users_PasswordReq(Table_Orbiter_Users_PasswordReq *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Orbiter_Users_PasswordReq::SetDefaultValues()
{
	m_FK_Orbiter = 0;
is_null[0] = false;
m_FK_Users = 0;
is_null[1] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[2] = false;
m_IsNew_RecordInfo = 1;
is_null[3] = false;
m_IsDeleted_RecordInfo = 0;
is_null[4] = false;
is_null[5] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Orbiter_Users_PasswordReq::FK_Orbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Orbiter;}
long int Row_Orbiter_Users_PasswordReq::FK_Users_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users;}
string Row_Orbiter_Users_PasswordReq::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Orbiter_Users_PasswordReq::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Orbiter_Users_PasswordReq::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Orbiter_Users_PasswordReq::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Orbiter_Users_PasswordReq::FK_Orbiter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Orbiter = val; is_modified=true; is_null[0]=false;}
void Row_Orbiter_Users_PasswordReq::FK_Users_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users = val; is_modified=true; is_null[1]=false;}
void Row_Orbiter_Users_PasswordReq::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[2]=false;}
void Row_Orbiter_Users_PasswordReq::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[3]=false;}
void Row_Orbiter_Users_PasswordReq::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Orbiter_Users_PasswordReq::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[5]=false;}

		
bool Row_Orbiter_Users_PasswordReq::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Orbiter_Users_PasswordReq::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Orbiter_Users_PasswordReq::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_Orbiter_Users_PasswordReq::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Orbiter_Users_PasswordReq::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Orbiter_Users_PasswordReq::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_Orbiter_Users_PasswordReq::FK_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Orbiter);

return buf;
}

string Row_Orbiter_Users_PasswordReq::FK_Users_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users);

return buf;
}

string Row_Orbiter_Users_PasswordReq::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Orbiter_Users_PasswordReq::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Orbiter_Users_PasswordReq::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Orbiter_Users_PasswordReq::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Orbiter_Users_PasswordReq::Key::Key(long int in_FK_Orbiter, long int in_FK_Users)
{
			pk_FK_Orbiter = in_FK_Orbiter;
pk_FK_Users = in_FK_Users;
	
}

Table_Orbiter_Users_PasswordReq::Key::Key(Row_Orbiter_Users_PasswordReq *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Orbiter = pRow->m_FK_Orbiter;
pk_FK_Users = pRow->m_FK_Users;
	
}		

bool Table_Orbiter_Users_PasswordReq::Key_Less::operator()(const Table_Orbiter_Users_PasswordReq::Key &key1, const Table_Orbiter_Users_PasswordReq::Key &key2) const
{
			if (key1.pk_FK_Orbiter!=key2.pk_FK_Orbiter)
return key1.pk_FK_Orbiter<key2.pk_FK_Orbiter;
else
if (key1.pk_FK_Users!=key2.pk_FK_Users)
return key1.pk_FK_Users<key2.pk_FK_Users;
else
return false;	
}	

void Table_Orbiter_Users_PasswordReq::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Orbiter_Users_PasswordReq*>::iterator i = addedRows.begin();
	
		Row_Orbiter_Users_PasswordReq *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Orbiter_asSQL()+", "+pRow->FK_Users_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Orbiter_Users_PasswordReq (FK_Orbiter, FK_Users, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Orbiter_Users_PasswordReq*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Orbiter_Users_PasswordReq* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk_FK_Users);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Users=" + tmp_FK_Users;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Orbiter="+pRow->FK_Orbiter_asSQL()+", FK_Users="+pRow->FK_Users_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Orbiter_Users_PasswordReq set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Orbiter_Users_PasswordReq*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Orbiter_Users_PasswordReq*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk_FK_Users);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Users=" + tmp_FK_Users;

	
		string query = "delete from Orbiter_Users_PasswordReq where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Orbiter_Users_PasswordReq::GetRows(string where_statement,vector<class Row_Orbiter_Users_PasswordReq*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Orbiter_Users_PasswordReq where " + where_statement;
		
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

		Row_Orbiter_Users_PasswordReq *pRow = new Row_Orbiter_Users_PasswordReq(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Modification_RecordInfo = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Orbiter_Users_PasswordReq::Key, Row_Orbiter_Users_PasswordReq*, Table_Orbiter_Users_PasswordReq::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Orbiter_Users_PasswordReq* Table_Orbiter_Users_PasswordReq::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Orbiter_Users_PasswordReq *pRow = new Row_Orbiter_Users_PasswordReq(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Orbiter_Users_PasswordReq* Table_Orbiter_Users_PasswordReq::GetRow(long int in_FK_Orbiter, long int in_FK_Users)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Orbiter, in_FK_Users);

	map<Key, Row_Orbiter_Users_PasswordReq*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Orbiter_Users_PasswordReq* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Orbiter_Users_PasswordReq* Table_Orbiter_Users_PasswordReq::FetchRow(Table_Orbiter_Users_PasswordReq::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Orbiter[32];
sprintf(tmp_FK_Orbiter, "%li", key.pk_FK_Orbiter);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk_FK_Users);


string condition;
condition = condition + "FK_Orbiter=" + tmp_FK_Orbiter+" AND "+"FK_Users=" + tmp_FK_Users;


	string query = "select * from Orbiter_Users_PasswordReq where " + condition;		

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

	Row_Orbiter_Users_PasswordReq *pRow = new Row_Orbiter_Users_PasswordReq(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Modification_RecordInfo = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Orbiter* Row_Orbiter_Users_PasswordReq::FK_Orbiter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter *pTable = table->database->Orbiter_get();
return pTable->GetRow(m_FK_Orbiter);
}
class Row_Users* Row_Orbiter_Users_PasswordReq::FK_Users_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users);
}





