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
#include "Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe.h"
#include "Table_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "Table_Pipe.h"
#include "Table_Input.h"
#include "Table_Output.h"



void Database_pluto_main::CreateTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe()
{
	tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe = new Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe()
{
	delete tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe;
}

Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::~Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe()
{
	map<Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less>::iterator it;
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


void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key key(this);					
			map<Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key, Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key key(this);		
		Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::SetDefaultValues()
{
	m_FK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
is_null[0] = false;
m_FK_Pipe = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
m_ToChild = 0;
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

long int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_DeviceTemplate_DeviceCategory_ControlledVia_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate_DeviceCategory_ControlledVia;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Pipe_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Pipe;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Input;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Output;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::ToChild_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ToChild;}
string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_DeviceTemplate_DeviceCategory_ControlledVia_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate_DeviceCategory_ControlledVia = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Pipe_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Pipe = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Input = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Output = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::ToChild_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ToChild = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[8]=false;}

		
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::ToChild_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::ToChild_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate_DeviceCategory_ControlledVia);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Pipe_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Pipe);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Input);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Output);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::ToChild_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ToChild);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key::Key(long int in_FK_DeviceTemplate_DeviceCategory_ControlledVia, long int in_FK_Pipe)
{
			pk_FK_DeviceTemplate_DeviceCategory_ControlledVia = in_FK_DeviceTemplate_DeviceCategory_ControlledVia;
pk_FK_Pipe = in_FK_Pipe;
	
}

Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key::Key(Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_DeviceTemplate_DeviceCategory_ControlledVia = pRow->m_FK_DeviceTemplate_DeviceCategory_ControlledVia;
pk_FK_Pipe = pRow->m_FK_Pipe;
	
}		

bool Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less::operator()(const Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key &key1, const Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key &key2) const
{
			if (key1.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia!=key2.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia)
return key1.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia<key2.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia;
else
if (key1.pk_FK_Pipe!=key2.pk_FK_Pipe)
return key1.pk_FK_Pipe<key2.pk_FK_Pipe;
else
return false;	
}	

void Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()+", "+pRow->FK_Pipe_asSQL()+", "+pRow->FK_Input_asSQL()+", "+pRow->FK_Output_asSQL()+", "+pRow->ToChild_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into DeviceTemplate_DeviceCategory_ControlledVia_Pipe (FK_DeviceTemplate_DeviceCategory_ControlledVia, FK_Pipe, FK_Input, FK_Output, ToChild, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
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
	

	for (map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk_FK_Pipe);


string condition;
condition = condition + "FK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia+" AND "+"FK_Pipe=" + tmp_FK_Pipe;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_DeviceTemplate_DeviceCategory_ControlledVia="+pRow->FK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL()+", FK_Pipe="+pRow->FK_Pipe_asSQL()+", FK_Input="+pRow->FK_Input_asSQL()+", FK_Output="+pRow->FK_Output_asSQL()+", ToChild="+pRow->ToChild_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update DeviceTemplate_DeviceCategory_ControlledVia_Pipe set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk_FK_Pipe);


string condition;
condition = condition + "FK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia+" AND "+"FK_Pipe=" + tmp_FK_Pipe;

	
		string query = "delete from DeviceTemplate_DeviceCategory_ControlledVia_Pipe where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DeviceTemplate_DeviceCategory_ControlledVia_Pipe where " + where_statement;
		
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

		Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate_DeviceCategory_ControlledVia));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Pipe = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Pipe));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Input = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Input));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Output = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Output));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ToChild = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_ToChild));
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
		
                map<Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key, Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::GetRow(long int in_FK_DeviceTemplate_DeviceCategory_ControlledVia, long int in_FK_Pipe)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_DeviceTemplate_DeviceCategory_ControlledVia, in_FK_Pipe);

	map<Key, Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FetchRow(Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia[32];
sprintf(tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia, "%li", key.pk_FK_DeviceTemplate_DeviceCategory_ControlledVia);

char tmp_FK_Pipe[32];
sprintf(tmp_FK_Pipe, "%li", key.pk_FK_Pipe);


string condition;
condition = condition + "FK_DeviceTemplate_DeviceCategory_ControlledVia=" + tmp_FK_DeviceTemplate_DeviceCategory_ControlledVia+" AND "+"FK_Pipe=" + tmp_FK_Pipe;


	string query = "select * from DeviceTemplate_DeviceCategory_ControlledVia_Pipe where " + condition;		

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

	Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow = new Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate_DeviceCategory_ControlledVia = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate_DeviceCategory_ControlledVia));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Pipe = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Pipe));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Input = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Input));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Output = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Output));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ToChild = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_ToChild));
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


class Row_DeviceTemplate_DeviceCategory_ControlledVia* Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_DeviceTemplate_DeviceCategory_ControlledVia_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceCategory_ControlledVia *pTable = table->database->DeviceTemplate_DeviceCategory_ControlledVia_get();
return pTable->GetRow(m_FK_DeviceTemplate_DeviceCategory_ControlledVia);
}
class Row_Pipe* Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Pipe_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Pipe *pTable = table->database->Pipe_get();
return pTable->GetRow(m_FK_Pipe);
}
class Row_Input* Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Input_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Input *pTable = table->database->Input_get();
return pTable->GetRow(m_FK_Input);
}
class Row_Output* Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::FK_Output_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Output *pTable = table->database->Output_get();
return pTable->GetRow(m_FK_Output);
}





