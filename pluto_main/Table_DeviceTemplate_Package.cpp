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
#include "Table_DeviceTemplate_Package.h"
#include "Table_DeviceTemplate.h"
#include "Table_Package.h"



void Database_pluto_main::CreateTable_DeviceTemplate_Package()
{
	tblDeviceTemplate_Package = new Table_DeviceTemplate_Package(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_Package()
{
	delete tblDeviceTemplate_Package;
}

Table_DeviceTemplate_Package::~Table_DeviceTemplate_Package()
{
	map<Table_DeviceTemplate_Package::Key, class Row_DeviceTemplate_Package*, Table_DeviceTemplate_Package::Key_Less>::iterator it;
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


void Row_DeviceTemplate_Package::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DeviceTemplate_Package*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DeviceTemplate_Package::Key key(this);					
			map<Table_DeviceTemplate_Package::Key, Row_DeviceTemplate_Package*, Table_DeviceTemplate_Package::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate_Package::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DeviceTemplate_Package::Key key(this);		
		Row_DeviceTemplate_Package *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_Package::Row_DeviceTemplate_Package(Table_DeviceTemplate_Package *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_Package::SetDefaultValues()
{
	m_FK_DeviceTemplate = 0;
is_null[0] = false;
m_FK_Package = 0;
is_null[1] = false;
m_DevelopmentOnly = 0;
is_null[2] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[3] = false;
m_IsNew_RecordInfo = 1;
is_null[4] = false;
m_IsDeleted_RecordInfo = 0;
is_null[5] = false;
is_null[6] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate_Package::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_DeviceTemplate_Package::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
short int Row_DeviceTemplate_Package::DevelopmentOnly_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DevelopmentOnly;}
string Row_DeviceTemplate_Package::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_DeviceTemplate_Package::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_DeviceTemplate_Package::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_DeviceTemplate_Package::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_DeviceTemplate_Package::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_Package::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_Package::DevelopmentOnly_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DevelopmentOnly = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_Package::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_Package::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_Package::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_Package::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[6]=false;}

		
bool Row_DeviceTemplate_Package::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_DeviceTemplate_Package::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_DeviceTemplate_Package::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_DeviceTemplate_Package::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_DeviceTemplate_Package::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_DeviceTemplate_Package::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_DeviceTemplate_Package::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_Package::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_DeviceTemplate_Package::DevelopmentOnly_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DevelopmentOnly);

return buf;
}

string Row_DeviceTemplate_Package::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate_Package::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_DeviceTemplate_Package::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_DeviceTemplate_Package::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_DeviceTemplate_Package::Key::Key(long int in_FK_DeviceTemplate, long int in_FK_Package)
{
			pk_FK_DeviceTemplate = in_FK_DeviceTemplate;
pk_FK_Package = in_FK_Package;
	
}

Table_DeviceTemplate_Package::Key::Key(Row_DeviceTemplate_Package *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_DeviceTemplate = pRow->m_FK_DeviceTemplate;
pk_FK_Package = pRow->m_FK_Package;
	
}		

bool Table_DeviceTemplate_Package::Key_Less::operator()(const Table_DeviceTemplate_Package::Key &key1, const Table_DeviceTemplate_Package::Key &key2) const
{
			if (key1.pk_FK_DeviceTemplate!=key2.pk_FK_DeviceTemplate)
return key1.pk_FK_DeviceTemplate<key2.pk_FK_DeviceTemplate;
else
if (key1.pk_FK_Package!=key2.pk_FK_Package)
return key1.pk_FK_Package<key2.pk_FK_Package;
else
return false;	
}	

void Table_DeviceTemplate_Package::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DeviceTemplate_Package*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_Package *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->DevelopmentOnly_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into DeviceTemplate_Package (FK_DeviceTemplate, FK_Package, DevelopmentOnly, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
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
	

	for (map<Key, Row_DeviceTemplate_Package*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DeviceTemplate_Package* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);

char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate+" AND "+"FK_Package=" + tmp_FK_Package;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_DeviceTemplate="+pRow->FK_DeviceTemplate_asSQL()+", FK_Package="+pRow->FK_Package_asSQL()+", DevelopmentOnly="+pRow->DevelopmentOnly_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update DeviceTemplate_Package set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DeviceTemplate_Package*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DeviceTemplate_Package*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);

char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate+" AND "+"FK_Package=" + tmp_FK_Package;

	
		string query = "delete from DeviceTemplate_Package where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DeviceTemplate_Package::GetRows(string where_statement,vector<class Row_DeviceTemplate_Package*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DeviceTemplate_Package where " + where_statement;
		
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

		Row_DeviceTemplate_Package *pRow = new Row_DeviceTemplate_Package(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_DevelopmentOnly = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_DevelopmentOnly));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Modification_RecordInfo = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_DeviceTemplate_Package::Key, Row_DeviceTemplate_Package*, Table_DeviceTemplate_Package::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DeviceTemplate_Package* Table_DeviceTemplate_Package::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceTemplate_Package *pRow = new Row_DeviceTemplate_Package(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_Package* Table_DeviceTemplate_Package::GetRow(long int in_FK_DeviceTemplate, long int in_FK_Package)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_DeviceTemplate, in_FK_Package);

	map<Key, Row_DeviceTemplate_Package*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_Package* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_Package* Table_DeviceTemplate_Package::FetchRow(Table_DeviceTemplate_Package::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);

char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk_FK_Package);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate+" AND "+"FK_Package=" + tmp_FK_Package;


	string query = "select * from DeviceTemplate_Package where " + condition;		

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

	Row_DeviceTemplate_Package *pRow = new Row_DeviceTemplate_Package(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_DevelopmentOnly = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_DevelopmentOnly));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Modification_RecordInfo = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DeviceTemplate* Row_DeviceTemplate_Package::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_Package* Row_DeviceTemplate_Package::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}





