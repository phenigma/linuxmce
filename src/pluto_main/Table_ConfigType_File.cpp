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
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_ConfigType_File *pRow = (Row_ConfigType_File *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_ConfigType_File *pRow = (Row_ConfigType_File *) (*it).second;
		delete pRow;
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
	Row_ConfigType_File *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_ConfigType_File *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_ConfigType_File);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_ConfigType_File::Reload()
{
	Row_ConfigType_File *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_ConfigType_File);
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
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_psc_frozen = 0;
is_null[8] = false;
m_psc_mod = "00000000000000";
is_null[9] = false;


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
long int Row_ConfigType_File::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_ConfigType_File::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_ConfigType_File::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_ConfigType_File::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_ConfigType_File::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
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
void Row_ConfigType_File::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[5]=false;}
void Row_ConfigType_File::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[6]=false;}
void Row_ConfigType_File::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[7]=false;}
void Row_ConfigType_File::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[8]=false;}
void Row_ConfigType_File::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[9]=false;}

		
bool Row_ConfigType_File::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_ConfigType_File::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_ConfigType_File::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_ConfigType_File::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_ConfigType_File::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_ConfigType_File::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_ConfigType_File::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_ConfigType_File::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

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

char *buf = new char[131071];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_InputFile.c_str(), (unsigned long) m_InputFile.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_ConfigType_File::OutputFile_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_OutputFile.c_str(), (unsigned long) m_OutputFile.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
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

string Row_ConfigType_File::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_ConfigType_File::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_ConfigType_File::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_ConfigType_File::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_ConfigType_File::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
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

bool Table_ConfigType_File::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_ConfigType_File *pRow = (Row_ConfigType_File *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_ConfigType_File_asSQL()+", "+pRow->FK_ConfigType_asSQL()+", "+pRow->InputFile_asSQL()+", "+pRow->OutputFile_asSQL()+", "+pRow->EFS_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into ConfigType_File (`PK_ConfigType_File`, `FK_ConfigType`, `InputFile`, `OutputFile`, `EFS`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_ConfigType_File=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_ConfigType_File);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_ConfigType_File* pRow = (Row_ConfigType_File*) (*i).second;	
		SingleLongKey key(pRow->m_PK_ConfigType_File);

		char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk);


string condition;
condition = condition + "`PK_ConfigType_File`=" + tmp_PK_ConfigType_File;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_ConfigType_File`="+pRow->PK_ConfigType_File_asSQL()+", `FK_ConfigType`="+pRow->FK_ConfigType_asSQL()+", `InputFile`="+pRow->InputFile_asSQL()+", `OutputFile`="+pRow->OutputFile_asSQL()+", `EFS`="+pRow->EFS_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update ConfigType_File set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_ConfigType_File* pRow = (Row_ConfigType_File*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_ConfigType_File* pRow = (Row_ConfigType_File*) (*i).second;	

		char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk);


string condition;
condition = condition + "`PK_ConfigType_File`=" + tmp_PK_ConfigType_File;

	
		string query = "delete from ConfigType_File where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_ConfigType_File*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_ConfigType_File::GetRows(string where_statement,vector<class Row_ConfigType_File*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `ConfigType_File`.* from ConfigType_File " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `ConfigType_File`.* from ConfigType_File where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_ConfigType_File);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_ConfigType_File *)(*i).second;
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

	SingleLongKey row_key(in_PK_ConfigType_File);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_ConfigType_File*) (*i).second;
	//we have to fetch row
	Row_ConfigType_File* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_ConfigType_File* Table_ConfigType_File::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_ConfigType_File[32];
sprintf(tmp_PK_ConfigType_File, "%li", key.pk);


string condition;
condition = condition + "`PK_ConfigType_File`=" + tmp_PK_ConfigType_File;


	string query = "select * from ConfigType_File where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
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
pTable->GetRows("`FK_ConfigType_File`=" + StringUtils::itos(m_PK_ConfigType_File),rows);
}



