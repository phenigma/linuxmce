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
#include "Table_Alert.h"
#include "Table_AlertType.h"

#include "Table_Notification.h"
#include "Table_Picture.h"


void Database_pluto_security::CreateTable_Alert()
{
	tblAlert = new Table_Alert(this);
}

void Database_pluto_security::DeleteTable_Alert()
{
	delete tblAlert;
}

Table_Alert::~Table_Alert()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Alert *pRow = (Row_Alert *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Alert *pRow = (Row_Alert *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Alert::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Alert *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Alert *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Alert);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Alert::Reload()
{
	Row_Alert *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Alert);
		Row_Alert *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Alert::Row_Alert(Table_Alert *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Alert::SetDefaultValues()
{
	m_PK_Alert = 0;
is_null[0] = false;
is_null[1] = true;
m_EK_Device = 0;
is_null[2] = false;
m_DetectionTime = "0000-00-00 00:00:00";
is_null[3] = false;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Alert::PK_Alert_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Alert;}
long int Row_Alert::FK_AlertType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_AlertType;}
long int Row_Alert::EK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_Device;}
string Row_Alert::DetectionTime_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DetectionTime;}
string Row_Alert::ExpirationTime_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ExpirationTime;}
short int Row_Alert::ResetBeforeExpiration_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ResetBeforeExpiration;}
short int Row_Alert::Benign_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Benign;}
string Row_Alert::ResetTime_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ResetTime;}
long int Row_Alert::EK_Users_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_Users;}

		
void Row_Alert::PK_Alert_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Alert = val; is_modified=true; is_null[0]=false;}
void Row_Alert::FK_AlertType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_AlertType = val; is_modified=true; is_null[1]=false;}
void Row_Alert::EK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_Device = val; is_modified=true; is_null[2]=false;}
void Row_Alert::DetectionTime_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DetectionTime = val; is_modified=true; is_null[3]=false;}
void Row_Alert::ExpirationTime_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ExpirationTime = val; is_modified=true; is_null[4]=false;}
void Row_Alert::ResetBeforeExpiration_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ResetBeforeExpiration = val; is_modified=true; is_null[5]=false;}
void Row_Alert::Benign_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Benign = val; is_modified=true; is_null[6]=false;}
void Row_Alert::ResetTime_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ResetTime = val; is_modified=true; is_null[7]=false;}
void Row_Alert::EK_Users_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_Users = val; is_modified=true; is_null[8]=false;}

		
bool Row_Alert::FK_AlertType_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Alert::ExpirationTime_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Alert::ResetBeforeExpiration_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Alert::Benign_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Alert::ResetTime_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Alert::EK_Users_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_Alert::FK_AlertType_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Alert::ExpirationTime_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Alert::ResetBeforeExpiration_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Alert::Benign_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Alert::ResetTime_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Alert::EK_Users_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_Alert::PK_Alert_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Alert);

return buf;
}

string Row_Alert::FK_AlertType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_AlertType);

return buf;
}

string Row_Alert::EK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Device);

return buf;
}

string Row_Alert::DetectionTime_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[39];
mysql_real_escape_string(table->database->db_handle, buf, m_DetectionTime.c_str(), (unsigned long) m_DetectionTime.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Alert::ExpirationTime_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[39];
mysql_real_escape_string(table->database->db_handle, buf, m_ExpirationTime.c_str(), (unsigned long) m_ExpirationTime.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Alert::ResetBeforeExpiration_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ResetBeforeExpiration);

return buf;
}

string Row_Alert::Benign_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Benign);

return buf;
}

string Row_Alert::ResetTime_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char *buf = new char[39];
mysql_real_escape_string(table->database->db_handle, buf, m_ResetTime.c_str(), (unsigned long) m_ResetTime.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Alert::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}




Table_Alert::Key::Key(long int in_PK_Alert)
{
			pk_PK_Alert = in_PK_Alert;
	
}

Table_Alert::Key::Key(Row_Alert *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Alert = pRow->m_PK_Alert;
	
}		

bool Table_Alert::Key_Less::operator()(const Table_Alert::Key &key1, const Table_Alert::Key &key2) const
{
			if (key1.pk_PK_Alert!=key2.pk_PK_Alert)
return key1.pk_PK_Alert<key2.pk_PK_Alert;
else
return false;	
}	

bool Table_Alert::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Alert *pRow = (Row_Alert *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Alert_asSQL()+", "+pRow->FK_AlertType_asSQL()+", "+pRow->EK_Device_asSQL()+", "+pRow->DetectionTime_asSQL()+", "+pRow->ExpirationTime_asSQL()+", "+pRow->ResetBeforeExpiration_asSQL()+", "+pRow->Benign_asSQL()+", "+pRow->ResetTime_asSQL()+", "+pRow->EK_Users_asSQL();

	
		string query = "insert into Alert (PK_Alert, FK_AlertType, EK_Device, DetectionTime, ExpirationTime, ResetBeforeExpiration, Benign, ResetTime, EK_Users) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Alert=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Alert);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Alert* pRow = (Row_Alert*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Alert);

		char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "PK_Alert=" + tmp_PK_Alert;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Alert="+pRow->PK_Alert_asSQL()+", FK_AlertType="+pRow->FK_AlertType_asSQL()+", EK_Device="+pRow->EK_Device_asSQL()+", DetectionTime="+pRow->DetectionTime_asSQL()+", ExpirationTime="+pRow->ExpirationTime_asSQL()+", ResetBeforeExpiration="+pRow->ResetBeforeExpiration_asSQL()+", Benign="+pRow->Benign_asSQL()+", ResetTime="+pRow->ResetTime_asSQL()+", EK_Users="+pRow->EK_Users_asSQL();

	
		string query = "update Alert set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Alert* pRow = (Row_Alert*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Alert* pRow = (Row_Alert*) (*i).second;	

		char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "PK_Alert=" + tmp_PK_Alert;

	
		string query = "delete from Alert where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_Alert*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Alert::GetRows(string where_statement,vector<class Row_Alert*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Alert " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Alert where " + where_statement;
		
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

		Row_Alert *pRow = new Row_Alert(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Alert = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Alert));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AlertType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AlertType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DetectionTime = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_DetectionTime = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExpirationTime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExpirationTime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ResetBeforeExpiration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ResetBeforeExpiration));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Benign = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Benign));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ResetTime = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ResetTime = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_EK_Users));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Alert);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Alert *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Alert* Table_Alert::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Alert *pRow = new Row_Alert(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Alert* Table_Alert::GetRow(long int in_PK_Alert)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Alert);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Alert*) (*i).second;
	//we have to fetch row
	Row_Alert* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Alert* Table_Alert::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "PK_Alert=" + tmp_PK_Alert;


	string query = "select * from Alert where " + condition;		

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

	Row_Alert *pRow = new Row_Alert(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Alert = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Alert));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AlertType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AlertType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DetectionTime = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_DetectionTime = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExpirationTime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExpirationTime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ResetBeforeExpiration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ResetBeforeExpiration));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Benign = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Benign));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ResetTime = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ResetTime = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_EK_Users));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_AlertType* Row_Alert::FK_AlertType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_AlertType *pTable = table->database->AlertType_get();
return pTable->GetRow(m_FK_AlertType);
}


void Row_Alert::Notification_FK_Alert_getrows(vector <class Row_Notification*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Notification *pTable = table->database->Notification_get();
pTable->GetRows("FK_Alert=" + StringUtils::itos(m_PK_Alert),rows);
}
void Row_Alert::Picture_FK_Alert_getrows(vector <class Row_Picture*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Picture *pTable = table->database->Picture_get();
pTable->GetRows("FK_Alert=" + StringUtils::itos(m_PK_Alert),rows);
}



