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
#include "Table_Floorplan.h"
#include "Table_Installation.h"
#include "Table_Icon.h"



void Database_pluto_main::CreateTable_Floorplan()
{
	tblFloorplan = new Table_Floorplan(this);
}

void Database_pluto_main::DeleteTable_Floorplan()
{
	delete tblFloorplan;
}

Table_Floorplan::~Table_Floorplan()
{
	map<Table_Floorplan::Key, class Row_Floorplan*, Table_Floorplan::Key_Less>::iterator it;
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


void Row_Floorplan::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Floorplan*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Floorplan::Key key(this);					
			map<Table_Floorplan::Key, Row_Floorplan*, Table_Floorplan::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Floorplan::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Floorplan::Key key(this);		
		Row_Floorplan *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Floorplan::Row_Floorplan(Table_Floorplan *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Floorplan::SetDefaultValues()
{
	m_FK_Installation = 0;
is_null[0] = false;
m_Page = 0;
is_null[1] = false;
m_Description = "";
is_null[2] = false;
is_null[3] = true;
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

long int Row_Floorplan::FK_Installation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Installation;}
long int Row_Floorplan::Page_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Page;}
string Row_Floorplan::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Floorplan::FK_Icon_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Icon;}
string Row_Floorplan::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Floorplan::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Floorplan::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Floorplan::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Floorplan::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Installation = val; is_modified=true; is_null[0]=false;}
void Row_Floorplan::Page_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Page = val; is_modified=true; is_null[1]=false;}
void Row_Floorplan::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_Floorplan::FK_Icon_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Icon = val; is_modified=true; is_null[3]=false;}
void Row_Floorplan::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Floorplan::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Floorplan::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Floorplan::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_Floorplan::FK_Icon_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Floorplan::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Floorplan::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Floorplan::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Floorplan::FK_Icon_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Floorplan::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Floorplan::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Floorplan::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Floorplan::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_Floorplan::Page_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Page);

return buf;
}

string Row_Floorplan::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Floorplan::FK_Icon_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Icon);

return buf;
}

string Row_Floorplan::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Floorplan::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Floorplan::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Floorplan::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Floorplan::Key::Key(long int in_FK_Installation, long int in_Page)
{
			pk_FK_Installation = in_FK_Installation;
pk_Page = in_Page;
	
}

Table_Floorplan::Key::Key(Row_Floorplan *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Installation = pRow->m_FK_Installation;
pk_Page = pRow->m_Page;
	
}		

bool Table_Floorplan::Key_Less::operator()(const Table_Floorplan::Key &key1, const Table_Floorplan::Key &key2) const
{
			if (key1.pk_FK_Installation!=key2.pk_FK_Installation)
return key1.pk_FK_Installation<key2.pk_FK_Installation;
else
if (key1.pk_Page!=key2.pk_Page)
return key1.pk_Page<key2.pk_Page;
else
return false;	
}	

void Table_Floorplan::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Floorplan*>::iterator i = addedRows.begin();
	
		Row_Floorplan *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Installation_asSQL()+", "+pRow->Page_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Icon_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Floorplan (FK_Installation, Page, Description, FK_Icon, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
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
	

	for (map<Key, Row_Floorplan*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Floorplan* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk_FK_Installation);

char tmp_Page[32];
sprintf(tmp_Page, "%li", key.pk_Page);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"Page=" + tmp_Page;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Installation="+pRow->FK_Installation_asSQL()+", Page="+pRow->Page_asSQL()+", Description="+pRow->Description_asSQL()+", FK_Icon="+pRow->FK_Icon_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Floorplan set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Floorplan*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Floorplan*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk_FK_Installation);

char tmp_Page[32];
sprintf(tmp_Page, "%li", key.pk_Page);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"Page=" + tmp_Page;

	
		string query = "delete from Floorplan where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Floorplan::GetRows(string where_statement,vector<class Row_Floorplan*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Floorplan where " + where_statement;
		
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

		Row_Floorplan *pRow = new Row_Floorplan(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Page = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_Page));
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
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Icon));
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
		
                map<Table_Floorplan::Key, Row_Floorplan*, Table_Floorplan::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Floorplan* Table_Floorplan::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Floorplan *pRow = new Row_Floorplan(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Floorplan* Table_Floorplan::GetRow(long int in_FK_Installation, long int in_Page)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Installation, in_Page);

	map<Key, Row_Floorplan*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Floorplan* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Floorplan* Table_Floorplan::FetchRow(Table_Floorplan::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Installation[32];
sprintf(tmp_FK_Installation, "%li", key.pk_FK_Installation);

char tmp_Page[32];
sprintf(tmp_Page, "%li", key.pk_Page);


string condition;
condition = condition + "FK_Installation=" + tmp_FK_Installation+" AND "+"Page=" + tmp_Page;


	string query = "select * from Floorplan where " + condition;		

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

	Row_Floorplan *pRow = new Row_Floorplan(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Page = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_Page));
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
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Icon));
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


class Row_Installation* Row_Floorplan::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}
class Row_Icon* Row_Floorplan::FK_Icon_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Icon *pTable = table->database->Icon_get();
return pTable->GetRow(m_FK_Icon);
}





