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
#include "Table_DeviceTemplate_AV.h"
#include "Table_DeviceTemplate.h"



void Database_pluto_main::CreateTable_DeviceTemplate_AV()
{
	tblDeviceTemplate_AV = new Table_DeviceTemplate_AV(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_AV()
{
	delete tblDeviceTemplate_AV;
}

Table_DeviceTemplate_AV::~Table_DeviceTemplate_AV()
{
	map<Table_DeviceTemplate_AV::Key, class Row_DeviceTemplate_AV*, Table_DeviceTemplate_AV::Key_Less>::iterator it;
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


void Row_DeviceTemplate_AV::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DeviceTemplate_AV*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DeviceTemplate_AV::Key key(this);					
			map<Table_DeviceTemplate_AV::Key, Row_DeviceTemplate_AV*, Table_DeviceTemplate_AV::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate_AV::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DeviceTemplate_AV::Key key(this);		
		Row_DeviceTemplate_AV *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_AV::Row_DeviceTemplate_AV(Table_DeviceTemplate_AV *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_AV::SetDefaultValues()
{
	m_FK_DeviceTemplate = 0;
is_null[0] = false;
m_IR_PowerDelay = 0;
is_null[1] = false;
m_IR_ModeDelay = 0;
is_null[2] = false;
m_DigitDelay = 0;
is_null[3] = false;
m_TogglePower = 0;
is_null[4] = false;
m_ToggleDSP = 0;
is_null[5] = false;
m_ToggleInput = 0;
is_null[6] = false;
m_ToggleOutput = 0;
is_null[7] = false;
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

long int Row_DeviceTemplate_AV::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_DeviceTemplate_AV::IR_PowerDelay_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IR_PowerDelay;}
long int Row_DeviceTemplate_AV::IR_ModeDelay_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IR_ModeDelay;}
long int Row_DeviceTemplate_AV::DigitDelay_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DigitDelay;}
long int Row_DeviceTemplate_AV::TogglePower_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_TogglePower;}
long int Row_DeviceTemplate_AV::ToggleDSP_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ToggleDSP;}
long int Row_DeviceTemplate_AV::ToggleInput_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ToggleInput;}
long int Row_DeviceTemplate_AV::ToggleOutput_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ToggleOutput;}
string Row_DeviceTemplate_AV::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_DeviceTemplate_AV::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_DeviceTemplate_AV::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_DeviceTemplate_AV::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_DeviceTemplate_AV::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_AV::IR_PowerDelay_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IR_PowerDelay = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_AV::IR_ModeDelay_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IR_ModeDelay = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_AV::DigitDelay_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DigitDelay = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_AV::TogglePower_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_TogglePower = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_AV::ToggleDSP_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ToggleDSP = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_AV::ToggleInput_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ToggleInput = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_AV::ToggleOutput_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ToggleOutput = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate_AV::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate_AV::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate_AV::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate_AV::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[11]=false;}

		
bool Row_DeviceTemplate_AV::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_DeviceTemplate_AV::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_DeviceTemplate_AV::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_DeviceTemplate_AV::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_DeviceTemplate_AV::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_DeviceTemplate_AV::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_DeviceTemplate_AV::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_AV::IR_PowerDelay_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_IR_PowerDelay);

return buf;
}

string Row_DeviceTemplate_AV::IR_ModeDelay_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_IR_ModeDelay);

return buf;
}

string Row_DeviceTemplate_AV::DigitDelay_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_DigitDelay);

return buf;
}

string Row_DeviceTemplate_AV::TogglePower_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_TogglePower);

return buf;
}

string Row_DeviceTemplate_AV::ToggleDSP_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ToggleDSP);

return buf;
}

string Row_DeviceTemplate_AV::ToggleInput_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ToggleInput);

return buf;
}

string Row_DeviceTemplate_AV::ToggleOutput_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ToggleOutput);

return buf;
}

string Row_DeviceTemplate_AV::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate_AV::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_DeviceTemplate_AV::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_DeviceTemplate_AV::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_DeviceTemplate_AV::Key::Key(long int in_FK_DeviceTemplate)
{
			pk_FK_DeviceTemplate = in_FK_DeviceTemplate;
	
}

Table_DeviceTemplate_AV::Key::Key(Row_DeviceTemplate_AV *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_DeviceTemplate = pRow->m_FK_DeviceTemplate;
	
}		

bool Table_DeviceTemplate_AV::Key_Less::operator()(const Table_DeviceTemplate_AV::Key &key1, const Table_DeviceTemplate_AV::Key &key2) const
{
			if (key1.pk_FK_DeviceTemplate!=key2.pk_FK_DeviceTemplate)
return key1.pk_FK_DeviceTemplate<key2.pk_FK_DeviceTemplate;
else
return false;	
}	

void Table_DeviceTemplate_AV::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DeviceTemplate_AV*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_AV *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DeviceTemplate_asSQL()+", "+pRow->IR_PowerDelay_asSQL()+", "+pRow->IR_ModeDelay_asSQL()+", "+pRow->DigitDelay_asSQL()+", "+pRow->TogglePower_asSQL()+", "+pRow->ToggleDSP_asSQL()+", "+pRow->ToggleInput_asSQL()+", "+pRow->ToggleOutput_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into DeviceTemplate_AV (FK_DeviceTemplate, IR_PowerDelay, IR_ModeDelay, DigitDelay, TogglePower, ToggleDSP, ToggleInput, ToggleOutput, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
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
	

	for (map<Key, Row_DeviceTemplate_AV*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DeviceTemplate_AV* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_DeviceTemplate="+pRow->FK_DeviceTemplate_asSQL()+", IR_PowerDelay="+pRow->IR_PowerDelay_asSQL()+", IR_ModeDelay="+pRow->IR_ModeDelay_asSQL()+", DigitDelay="+pRow->DigitDelay_asSQL()+", TogglePower="+pRow->TogglePower_asSQL()+", ToggleDSP="+pRow->ToggleDSP_asSQL()+", ToggleInput="+pRow->ToggleInput_asSQL()+", ToggleOutput="+pRow->ToggleOutput_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update DeviceTemplate_AV set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DeviceTemplate_AV*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DeviceTemplate_AV*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate;

	
		string query = "delete from DeviceTemplate_AV where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DeviceTemplate_AV::GetRows(string where_statement,vector<class Row_DeviceTemplate_AV*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DeviceTemplate_AV where " + where_statement;
		
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

		Row_DeviceTemplate_AV *pRow = new Row_DeviceTemplate_AV(this);
		
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
pRow->m_IR_PowerDelay = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_IR_PowerDelay));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IR_ModeDelay = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_IR_ModeDelay));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DigitDelay = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_DigitDelay));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_TogglePower = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_TogglePower));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ToggleDSP = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ToggleDSP));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ToggleInput = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_ToggleInput));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ToggleOutput = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_ToggleOutput));
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
		
                map<Table_DeviceTemplate_AV::Key, Row_DeviceTemplate_AV*, Table_DeviceTemplate_AV::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DeviceTemplate_AV* Table_DeviceTemplate_AV::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceTemplate_AV *pRow = new Row_DeviceTemplate_AV(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_AV* Table_DeviceTemplate_AV::GetRow(long int in_FK_DeviceTemplate)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_DeviceTemplate);

	map<Key, Row_DeviceTemplate_AV*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_AV* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_AV* Table_DeviceTemplate_AV::FetchRow(Table_DeviceTemplate_AV::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk_FK_DeviceTemplate);


string condition;
condition = condition + "FK_DeviceTemplate=" + tmp_FK_DeviceTemplate;


	string query = "select * from DeviceTemplate_AV where " + condition;		

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

	Row_DeviceTemplate_AV *pRow = new Row_DeviceTemplate_AV(this);
		
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
pRow->m_IR_PowerDelay = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_IR_PowerDelay));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IR_ModeDelay = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_IR_ModeDelay));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DigitDelay = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_DigitDelay));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_TogglePower = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_TogglePower));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ToggleDSP = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ToggleDSP));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ToggleInput = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_ToggleInput));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ToggleOutput = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_ToggleOutput));
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


class Row_DeviceTemplate* Row_DeviceTemplate_AV::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}





