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
#include "Table_SetupStep.h"
#include "Table_SetupStep.h"
#include "Table_SetupStep.h"

#include "Table_SetupStep.h"
#include "Table_SetupStep.h"


void Database_pluto_main::CreateTable_SetupStep()
{
	tblSetupStep = new Table_SetupStep(this);
}

void Database_pluto_main::DeleteTable_SetupStep()
{
	delete tblSetupStep;
}

Table_SetupStep::~Table_SetupStep()
{
	map<Table_SetupStep::Key, class Row_SetupStep*, Table_SetupStep::Key_Less>::iterator it;
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


void Row_SetupStep::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_SetupStep*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_SetupStep::Key key(this);					
			map<Table_SetupStep::Key, Row_SetupStep*, Table_SetupStep::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_SetupStep::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_SetupStep::Key key(this);		
		Row_SetupStep *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_SetupStep::Row_SetupStep(Table_SetupStep *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_SetupStep::SetDefaultValues()
{
	m_PK_SetupStep = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_Address = "";
is_null[3] = false;
m_Name = "";
is_null[4] = false;
is_null[5] = true;
m_FirstStep = 0;
is_null[6] = false;
is_null[7] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[8] = false;
m_IsNew_RecordInfo = 1;
is_null[9] = false;
m_IsDeleted_RecordInfo = 0;
is_null[10] = false;
is_null[11] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_SetupStep::PK_SetupStep_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_SetupStep;}
long int Row_SetupStep::FK_SetupStep_Next_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_SetupStep_Next;}
long int Row_SetupStep::FK_SetupStep_Parent_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_SetupStep_Parent;}
string Row_SetupStep::Address_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Address;}
string Row_SetupStep::Name_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Name;}
string Row_SetupStep::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
short int Row_SetupStep::FirstStep_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FirstStep;}
string Row_SetupStep::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_SetupStep::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_SetupStep::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_SetupStep::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_SetupStep::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_SetupStep::PK_SetupStep_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_SetupStep = val; is_modified=true; is_null[0]=false;}
void Row_SetupStep::FK_SetupStep_Next_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_SetupStep_Next = val; is_modified=true; is_null[1]=false;}
void Row_SetupStep::FK_SetupStep_Parent_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_SetupStep_Parent = val; is_modified=true; is_null[2]=false;}
void Row_SetupStep::Address_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Address = val; is_modified=true; is_null[3]=false;}
void Row_SetupStep::Name_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Name = val; is_modified=true; is_null[4]=false;}
void Row_SetupStep::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_SetupStep::FirstStep_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FirstStep = val; is_modified=true; is_null[6]=false;}
void Row_SetupStep::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[7]=false;}
void Row_SetupStep::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_SetupStep::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[9]=false;}
void Row_SetupStep::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_SetupStep::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[11]=false;}

		
bool Row_SetupStep::FK_SetupStep_Next_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_SetupStep::FK_SetupStep_Parent_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_SetupStep::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_SetupStep::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_SetupStep::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_SetupStep::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_SetupStep::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_SetupStep::FK_SetupStep_Next_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_SetupStep::FK_SetupStep_Parent_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_SetupStep::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_SetupStep::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_SetupStep::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_SetupStep::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_SetupStep::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_SetupStep::PK_SetupStep_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_SetupStep);

return buf;
}

string Row_SetupStep::FK_SetupStep_Next_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_SetupStep_Next);

return buf;
}

string Row_SetupStep::FK_SetupStep_Parent_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_SetupStep_Parent);

return buf;
}

string Row_SetupStep::Address_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Address.c_str(), (unsigned long) m_Address.size());
return string()+"\""+buf+"\"";
}

string Row_SetupStep::Name_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Name.c_str(), (unsigned long) m_Name.size());
return string()+"\""+buf+"\"";
}

string Row_SetupStep::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_SetupStep::FirstStep_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FirstStep);

return buf;
}

string Row_SetupStep::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_SetupStep::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_SetupStep::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_SetupStep::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_SetupStep::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_SetupStep::Key::Key(long int in_PK_SetupStep)
{
			pk_PK_SetupStep = in_PK_SetupStep;
	
}

Table_SetupStep::Key::Key(Row_SetupStep *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_SetupStep = pRow->m_PK_SetupStep;
	
}		

bool Table_SetupStep::Key_Less::operator()(const Table_SetupStep::Key &key1, const Table_SetupStep::Key &key2) const
{
			if (key1.pk_PK_SetupStep!=key2.pk_PK_SetupStep)
return key1.pk_PK_SetupStep<key2.pk_PK_SetupStep;
else
return false;	
}	

void Table_SetupStep::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_SetupStep*>::iterator i = addedRows.begin();
	
		Row_SetupStep *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_SetupStep_asSQL()+", "+pRow->FK_SetupStep_Next_asSQL()+", "+pRow->FK_SetupStep_Parent_asSQL()+", "+pRow->Address_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FirstStep_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into SetupStep (PK_SetupStep, FK_SetupStep_Next, FK_SetupStep_Parent, Address, Name, Description, FirstStep, Define, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_SetupStep=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_SetupStep*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_SetupStep* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_SetupStep[32];
sprintf(tmp_PK_SetupStep, "%li", key.pk_PK_SetupStep);


string condition;
condition = condition + "PK_SetupStep=" + tmp_PK_SetupStep;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_SetupStep="+pRow->PK_SetupStep_asSQL()+", FK_SetupStep_Next="+pRow->FK_SetupStep_Next_asSQL()+", FK_SetupStep_Parent="+pRow->FK_SetupStep_Parent_asSQL()+", Address="+pRow->Address_asSQL()+", Name="+pRow->Name_asSQL()+", Description="+pRow->Description_asSQL()+", FirstStep="+pRow->FirstStep_asSQL()+", Define="+pRow->Define_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update SetupStep set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_SetupStep*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_SetupStep*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_SetupStep[32];
sprintf(tmp_PK_SetupStep, "%li", key.pk_PK_SetupStep);


string condition;
condition = condition + "PK_SetupStep=" + tmp_PK_SetupStep;

	
		string query = "delete from SetupStep where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_SetupStep::GetRows(string where_statement,vector<class Row_SetupStep*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from SetupStep where " + where_statement;
		
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

		Row_SetupStep *pRow = new Row_SetupStep(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_SetupStep = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_SetupStep));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_SetupStep_Next = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_SetupStep_Next));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_SetupStep_Parent = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_SetupStep_Parent));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Address = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Address = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Name = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FirstStep = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_FirstStep));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Define = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Modification_RecordInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_SetupStep::Key, Row_SetupStep*, Table_SetupStep::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_SetupStep* Table_SetupStep::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_SetupStep *pRow = new Row_SetupStep(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_SetupStep* Table_SetupStep::GetRow(long int in_PK_SetupStep)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_SetupStep);

	map<Key, Row_SetupStep*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_SetupStep* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_SetupStep* Table_SetupStep::FetchRow(Table_SetupStep::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_SetupStep[32];
sprintf(tmp_PK_SetupStep, "%li", key.pk_PK_SetupStep);


string condition;
condition = condition + "PK_SetupStep=" + tmp_PK_SetupStep;


	string query = "select * from SetupStep where " + condition;		

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

	Row_SetupStep *pRow = new Row_SetupStep(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_SetupStep = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_SetupStep));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_SetupStep_Next = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_SetupStep_Next));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_SetupStep_Parent = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_SetupStep_Parent));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Address = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Address = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Name = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FirstStep = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_FirstStep));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Define = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Modification_RecordInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_SetupStep* Row_SetupStep::FK_SetupStep_Next_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SetupStep *pTable = table->database->SetupStep_get();
return pTable->GetRow(m_FK_SetupStep_Next);
}
class Row_SetupStep* Row_SetupStep::FK_SetupStep_Parent_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SetupStep *pTable = table->database->SetupStep_get();
return pTable->GetRow(m_FK_SetupStep_Parent);
}


void Row_SetupStep::SetupStep_FK_SetupStep_Next_getrows(vector <class Row_SetupStep*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SetupStep *pTable = table->database->SetupStep_get();
pTable->GetRows("FK_SetupStep_Next=" + StringUtils::itos(m_PK_SetupStep),rows);
}
void Row_SetupStep::SetupStep_FK_SetupStep_Parent_getrows(vector <class Row_SetupStep*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_SetupStep *pTable = table->database->SetupStep_get();
pTable->GetRows("FK_SetupStep_Parent=" + StringUtils::itos(m_PK_SetupStep),rows);
}



