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
#include "Table_ConfigType_File.h"
#include "Table_ConfigType.h"

#include "Table_ConfigType_Token.h"


void Database_pluto_main::CreateTable_ConfigType_File()
{
	tblConfigType_File = new Table_ConfigType_File(this);
}

void Database_pluto_main::DeleteTable_ConfigType_File()
{
	delete tblConfigType_File;
}

Table_ConfigType_File::~Table_ConfigType_File()
{
	map<Table_ConfigType_File::Key, class Row_ConfigType_File*, Table_ConfigType_File::Key_Less>::iterator it;
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


void Row_ConfigType_File::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_ConfigType_File*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_ConfigType_File::Key key(this);					
			map<Table_ConfigType_File::Key, Row_ConfigType_File*, Table_ConfigType_File::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_ConfigType_File::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_ConfigType_File::Key key(this);		
		Row_ConfigType_File *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_ConfigType_File::Row_ConfigType_File(Table_ConfigType_File *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_ConfigType_File::SetDefaultValues()
{
	m_PK_ConfigType_File = 0;
is_null[0] = false;
m_FK_ConfigType = 0;
is_null[1] = false;
m_InputFile = "";
is_null[2] = false;
m_OutputFile = "";
is_null[3] = false;
m_EFS = 0;
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

long int Row_ConfigType_File::PK_ConfigType_File_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_ConfigType_File;}
long int Row_ConfigType_File::FK_ConfigType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_ConfigType;}
string Row_ConfigType_File::InputFile_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_InputFile;}
string Row_ConfigType_File::OutputFile_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_OutputFile;}
short int Row_ConfigType_File::EFS_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EFS;}
string Row_ConfigType_File::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_ConfigType_File::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_ConfigType_File::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_ConfigType_File::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_ConfigType_File::PK_ConfigType_File_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_ConfigType_File = val; is_modified=true; is_null[0]=false;}
void Row_ConfigType_File::FK_ConfigType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_ConfigType = val; is_modified=true; is_null[1]=false;}
void Row_ConfigType_File::InputFile_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_InputFile = val; is_modified=true; is_null[2]=false;}
void Row_ConfigType_File::OutputFile_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_OutputFile = val; is_modified=true; is_null[3]=false;}
void Row_ConfigType_File::EFS_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EFS = val; is_modified=true; is_null[4]=false;}
void Row_ConfigType_File::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_ConfigType_File::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_ConfigType_File::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_ConfigType_File::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[8]=false;}

		
bool Row_ConfigType_File::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_ConfigType_File::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_ConfigType_File::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_ConfigType_File::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_ConfigType_File::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_ConfigType_File::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_ConfigType_File::PK_ConfigType_File_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_ConfigType_File);

return buf;
}

string Row_ConfigType_File::FK_ConfigType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_ConfigType);

return buf;
}

string Row_ConfigType_File::InputFile_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_InputFile.c_str(), (unsigned long) m_InputFile.size());
return string()+"\""+buf+"\"";
}

string Row_ConfigType_File::OutputFile_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_OutputFile.c_str(), (unsigned long) m_OutputFile.size());
return string()+"\""+buf+"\"";
}

string Row_ConfigType_File::EFS_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_EFS);

return buf;
}

string Row_ConfigType_File::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_ConfigType_File::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_ConfigType_File::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_ConfigType_File::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_ConfigType_File::Key::Key(long int in_PK_ConfigType_File)
{
			pk_PK_ConfigType_File = in_PK_ConfigType_File;
	
}

Table_ConfigType_File::Key::Key(Row_ConfigType_File *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_ConfigType_File = pRow->m_PK_ConfigType_File;
	
}		

bool Table_ConfigType_File::Key_Less::operator()(const Table_ConfigType_File::Key &key1, const Table_ConfigType_File::Key &key2) const
{
			if (key1.pk_PK_ConfigType_File!=key2.pk_PK_ConfigType_File)
return key1.pk_PK_ConfigType_File<key2.pk_PK_ConfigType_File;
else
return false;	
}	

void Table_ConfigType_File::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_ConfigType_File*>::iterator i = addedRows.begin();
	
		Row_ConfigType_File *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_ConfigType_File_asSQL()+", "+pRow->FK_ConfigType_asSQL()+", "+pRow->InputFile_asSQL()+", "+pRow->OutputFile_asSQL()+", "+pRow->EFS_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into ConfigType_File (PK_ConfigType_File, FK_ConfigType, InputFile, OutputFile, EFS, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_ConfigType_File=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_ConfigType_File*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_ConfigType_File* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk_PK_ConfigType_File);


string condition;
condition = condition + "PK_ConfigType_File=" + tmp_PK_ConfigType_File;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_ConfigType_File="+pRow->PK_ConfigType_File_asSQL()+", FK_ConfigType="+pRow->FK_ConfigType_asSQL()+", InputFile="+pRow->InputFile_asSQL()+", OutputFile="+pRow->OutputFile_asSQL()+", EFS="+pRow->EFS_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update ConfigType_File set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_ConfigType_File*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_ConfigType_File*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk_PK_ConfigType_File);


string condition;
condition = condition + "PK_ConfigType_File=" + tmp_PK_ConfigType_File;

	
		string query = "delete from ConfigType_File where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_ConfigType_File::GetRows(string where_statement,vector<class Row_ConfigType_File*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from ConfigType_File where " + where_statement;
		
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

		Row_ConfigType_File *pRow = new Row_ConfigType_File(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ConfigType_File = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ConfigType_File));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ConfigType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ConfigType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_InputFile = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_InputFile = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_OutputFile = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_OutputFile = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EFS = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_EFS));
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
		
                map<Table_ConfigType_File::Key, Row_ConfigType_File*, Table_ConfigType_File::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_ConfigType_File* Table_ConfigType_File::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_ConfigType_File *pRow = new Row_ConfigType_File(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_ConfigType_File* Table_ConfigType_File::GetRow(long int in_PK_ConfigType_File)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_ConfigType_File);

	map<Key, Row_ConfigType_File*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_ConfigType_File* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_ConfigType_File* Table_ConfigType_File::FetchRow(Table_ConfigType_File::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk_PK_ConfigType_File);


string condition;
condition = condition + "PK_ConfigType_File=" + tmp_PK_ConfigType_File;


	string query = "select * from ConfigType_File where " + condition;		

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

	Row_ConfigType_File *pRow = new Row_ConfigType_File(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ConfigType_File = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ConfigType_File));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ConfigType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ConfigType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_InputFile = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_InputFile = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_OutputFile = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_OutputFile = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EFS = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_EFS));
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


class Row_ConfigType* Row_ConfigType_File::FK_ConfigType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_ConfigType *pTable = table->database->ConfigType_get();
return pTable->GetRow(m_FK_ConfigType);
}


void Row_ConfigType_File::ConfigType_Token_FK_ConfigType_File_getrows(vector <class Row_ConfigType_Token*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_ConfigType_Token *pTable = table->database->ConfigType_Token_get();
pTable->GetRows("FK_ConfigType_File=" + StringUtils::itos(m_PK_ConfigType_File),rows);
}



