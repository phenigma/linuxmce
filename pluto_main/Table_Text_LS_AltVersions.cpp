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
#include "Table_Text_LS_AltVersions.h"
#include "Table_Text.h"
#include "Table_Language.h"



void Database_pluto_main::CreateTable_Text_LS_AltVersions()
{
	tblText_LS_AltVersions = new Table_Text_LS_AltVersions(this);
}

void Database_pluto_main::DeleteTable_Text_LS_AltVersions()
{
	delete tblText_LS_AltVersions;
}

Table_Text_LS_AltVersions::~Table_Text_LS_AltVersions()
{
	map<Table_Text_LS_AltVersions::Key, class Row_Text_LS_AltVersions*, Table_Text_LS_AltVersions::Key_Less>::iterator it;
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


void Row_Text_LS_AltVersions::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Text_LS_AltVersions*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Text_LS_AltVersions::Key key(this);					
			map<Table_Text_LS_AltVersions::Key, Row_Text_LS_AltVersions*, Table_Text_LS_AltVersions::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Text_LS_AltVersions::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Text_LS_AltVersions::Key key(this);		
		Row_Text_LS_AltVersions *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Text_LS_AltVersions::Row_Text_LS_AltVersions(Table_Text_LS_AltVersions *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Text_LS_AltVersions::SetDefaultValues()
{
	m_FK_Text = 0;
is_null[0] = false;
m_FK_Language = 0;
is_null[1] = false;
m_Version = 0;
is_null[2] = false;
m_Description = "";
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

long int Row_Text_LS_AltVersions::FK_Text_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Text;}
long int Row_Text_LS_AltVersions::FK_Language_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Language;}
long int Row_Text_LS_AltVersions::Version_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Version;}
string Row_Text_LS_AltVersions::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Text_LS_AltVersions::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Text_LS_AltVersions::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Text_LS_AltVersions::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Text_LS_AltVersions::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Text_LS_AltVersions::FK_Text_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Text = val; is_modified=true; is_null[0]=false;}
void Row_Text_LS_AltVersions::FK_Language_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Language = val; is_modified=true; is_null[1]=false;}
void Row_Text_LS_AltVersions::Version_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Version = val; is_modified=true; is_null[2]=false;}
void Row_Text_LS_AltVersions::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_Text_LS_AltVersions::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Text_LS_AltVersions::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Text_LS_AltVersions::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Text_LS_AltVersions::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_Text_LS_AltVersions::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Text_LS_AltVersions::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Text_LS_AltVersions::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Text_LS_AltVersions::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Text_LS_AltVersions::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Text_LS_AltVersions::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Text_LS_AltVersions::FK_Text_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Text);

return buf;
}

string Row_Text_LS_AltVersions::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Language);

return buf;
}

string Row_Text_LS_AltVersions::Version_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Version);

return buf;
}

string Row_Text_LS_AltVersions::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Text_LS_AltVersions::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Text_LS_AltVersions::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Text_LS_AltVersions::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Text_LS_AltVersions::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Text_LS_AltVersions::Key::Key(long int in_FK_Text, long int in_FK_Language, long int in_Version)
{
			pk_FK_Text = in_FK_Text;
pk_FK_Language = in_FK_Language;
pk_Version = in_Version;
	
}

Table_Text_LS_AltVersions::Key::Key(Row_Text_LS_AltVersions *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Text = pRow->m_FK_Text;
pk_FK_Language = pRow->m_FK_Language;
pk_Version = pRow->m_Version;
	
}		

bool Table_Text_LS_AltVersions::Key_Less::operator()(const Table_Text_LS_AltVersions::Key &key1, const Table_Text_LS_AltVersions::Key &key2) const
{
			if (key1.pk_FK_Text!=key2.pk_FK_Text)
return key1.pk_FK_Text<key2.pk_FK_Text;
else
if (key1.pk_FK_Language!=key2.pk_FK_Language)
return key1.pk_FK_Language<key2.pk_FK_Language;
else
if (key1.pk_Version!=key2.pk_Version)
return key1.pk_Version<key2.pk_Version;
else
return false;	
}	

void Table_Text_LS_AltVersions::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Text_LS_AltVersions*>::iterator i = addedRows.begin();
	
		Row_Text_LS_AltVersions *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Text_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->Version_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Text_LS_AltVersions (FK_Text, FK_Language, Version, Description, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
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
	

	for (map<Key, Row_Text_LS_AltVersions*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Text_LS_AltVersions* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_Text[32];
sprintf(tmp_FK_Text, "%li", key.pk_FK_Text);

char tmp_FK_Language[32];
sprintf(tmp_FK_Language, "%li", key.pk_FK_Language);

char tmp_Version[32];
sprintf(tmp_Version, "%li", key.pk_Version);


string condition;
condition = condition + "FK_Text=" + tmp_FK_Text+" AND "+"FK_Language=" + tmp_FK_Language+" AND "+"Version=" + tmp_Version;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Text="+pRow->FK_Text_asSQL()+", FK_Language="+pRow->FK_Language_asSQL()+", Version="+pRow->Version_asSQL()+", Description="+pRow->Description_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Text_LS_AltVersions set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Text_LS_AltVersions*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Text_LS_AltVersions*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_Text[32];
sprintf(tmp_FK_Text, "%li", key.pk_FK_Text);

char tmp_FK_Language[32];
sprintf(tmp_FK_Language, "%li", key.pk_FK_Language);

char tmp_Version[32];
sprintf(tmp_Version, "%li", key.pk_Version);


string condition;
condition = condition + "FK_Text=" + tmp_FK_Text+" AND "+"FK_Language=" + tmp_FK_Language+" AND "+"Version=" + tmp_Version;

	
		string query = "delete from Text_LS_AltVersions where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Text_LS_AltVersions::GetRows(string where_statement,vector<class Row_Text_LS_AltVersions*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Text_LS_AltVersions where " + where_statement;
		
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

		Row_Text_LS_AltVersions *pRow = new Row_Text_LS_AltVersions(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Text = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Text));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Language));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Version = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Version));
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
		
                map<Table_Text_LS_AltVersions::Key, Row_Text_LS_AltVersions*, Table_Text_LS_AltVersions::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Text_LS_AltVersions* Table_Text_LS_AltVersions::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Text_LS_AltVersions *pRow = new Row_Text_LS_AltVersions(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Text_LS_AltVersions* Table_Text_LS_AltVersions::GetRow(long int in_FK_Text, long int in_FK_Language, long int in_Version)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_Text, in_FK_Language, in_Version);

	map<Key, Row_Text_LS_AltVersions*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Text_LS_AltVersions* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Text_LS_AltVersions* Table_Text_LS_AltVersions::FetchRow(Table_Text_LS_AltVersions::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Text[32];
sprintf(tmp_FK_Text, "%li", key.pk_FK_Text);

char tmp_FK_Language[32];
sprintf(tmp_FK_Language, "%li", key.pk_FK_Language);

char tmp_Version[32];
sprintf(tmp_Version, "%li", key.pk_Version);


string condition;
condition = condition + "FK_Text=" + tmp_FK_Text+" AND "+"FK_Language=" + tmp_FK_Language+" AND "+"Version=" + tmp_Version;


	string query = "select * from Text_LS_AltVersions where " + condition;		

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

	Row_Text_LS_AltVersions *pRow = new Row_Text_LS_AltVersions(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Text = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Text));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Language));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Version = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Version));
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


class Row_Text* Row_Text_LS_AltVersions::FK_Text_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Text *pTable = table->database->Text_get();
return pTable->GetRow(m_FK_Text);
}
class Row_Language* Row_Text_LS_AltVersions::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}





