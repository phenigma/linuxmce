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
#include "Table_InfraredData.h"



void Database_pluto_main::CreateTable_InfraredData()
{
	tblInfraredData = new Table_InfraredData(this);
}

void Database_pluto_main::DeleteTable_InfraredData()
{
	delete tblInfraredData;
}

Table_InfraredData::~Table_InfraredData()
{
	map<Table_InfraredData::Key, class Row_InfraredData*, Table_InfraredData::Key_Less>::iterator it;
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


void Row_InfraredData::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_InfraredData*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_InfraredData::Key key(this);					
			map<Table_InfraredData::Key, Row_InfraredData*, Table_InfraredData::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_InfraredData::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_InfraredData::Key key(this);		
		Row_InfraredData *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_InfraredData::Row_InfraredData(Table_InfraredData *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_InfraredData::SetDefaultValues()
{
	m_PK_InfraredData = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[10] = false;
m_IsNew_RecordInfo = 1;
is_null[11] = false;
m_IsDeleted_RecordInfo = 0;
is_null[12] = false;
is_null[13] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_InfraredData::PK_InfraredData_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_InfraredData;}
string Row_InfraredData::Manufacturer_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Manufacturer;}
string Row_InfraredData::Model_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Model;}
string Row_InfraredData::Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Device;}
string Row_InfraredData::Command_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Command;}
string Row_InfraredData::Pronto_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Pronto;}
long int Row_InfraredData::Frequency_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Frequency;}
string Row_InfraredData::Data_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Data;}
string Row_InfraredData::DataRepeat_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DataRepeat;}
string Row_InfraredData::Leadout_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Leadout;}
string Row_InfraredData::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_InfraredData::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_InfraredData::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_InfraredData::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_InfraredData::PK_InfraredData_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_InfraredData = val; is_modified=true; is_null[0]=false;}
void Row_InfraredData::Manufacturer_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Manufacturer = val; is_modified=true; is_null[1]=false;}
void Row_InfraredData::Model_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Model = val; is_modified=true; is_null[2]=false;}
void Row_InfraredData::Device_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Device = val; is_modified=true; is_null[3]=false;}
void Row_InfraredData::Command_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Command = val; is_modified=true; is_null[4]=false;}
void Row_InfraredData::Pronto_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Pronto = val; is_modified=true; is_null[5]=false;}
void Row_InfraredData::Frequency_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Frequency = val; is_modified=true; is_null[6]=false;}
void Row_InfraredData::Data_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Data = val; is_modified=true; is_null[7]=false;}
void Row_InfraredData::DataRepeat_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DataRepeat = val; is_modified=true; is_null[8]=false;}
void Row_InfraredData::Leadout_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Leadout = val; is_modified=true; is_null[9]=false;}
void Row_InfraredData::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_InfraredData::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[11]=false;}
void Row_InfraredData::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[12]=false;}
void Row_InfraredData::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[13]=false;}

		
bool Row_InfraredData::Manufacturer_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_InfraredData::Model_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_InfraredData::Device_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_InfraredData::Command_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_InfraredData::Pronto_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_InfraredData::Frequency_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_InfraredData::Data_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_InfraredData::DataRepeat_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_InfraredData::Leadout_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_InfraredData::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_InfraredData::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_InfraredData::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}

			
void Row_InfraredData::Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_InfraredData::Model_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_InfraredData::Device_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_InfraredData::Command_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_InfraredData::Pronto_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_InfraredData::Frequency_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_InfraredData::Data_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_InfraredData::DataRepeat_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_InfraredData::Leadout_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_InfraredData::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_InfraredData::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_InfraredData::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
	

string Row_InfraredData::PK_InfraredData_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_InfraredData);

return buf;
}

string Row_InfraredData::Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Manufacturer.c_str(), (unsigned long) m_Manufacturer.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Model_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Model.c_str(), (unsigned long) m_Model.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Device.c_str(), (unsigned long) m_Device.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Command_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Command.c_str(), (unsigned long) m_Command.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Pronto_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Pronto.c_str(), (unsigned long) m_Pronto.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Frequency_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Frequency);

return buf;
}

string Row_InfraredData::Data_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Data.c_str(), (unsigned long) m_Data.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::DataRepeat_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_DataRepeat.c_str(), (unsigned long) m_DataRepeat.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Leadout_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Leadout.c_str(), (unsigned long) m_Leadout.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_InfraredData::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_InfraredData::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_InfraredData::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_InfraredData::Key::Key(long int in_PK_InfraredData)
{
			pk_PK_InfraredData = in_PK_InfraredData;
	
}

Table_InfraredData::Key::Key(Row_InfraredData *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_InfraredData = pRow->m_PK_InfraredData;
	
}		

bool Table_InfraredData::Key_Less::operator()(const Table_InfraredData::Key &key1, const Table_InfraredData::Key &key2) const
{
			if (key1.pk_PK_InfraredData!=key2.pk_PK_InfraredData)
return key1.pk_PK_InfraredData<key2.pk_PK_InfraredData;
else
return false;	
}	

void Table_InfraredData::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_InfraredData*>::iterator i = addedRows.begin();
	
		Row_InfraredData *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_InfraredData_asSQL()+", "+pRow->Manufacturer_asSQL()+", "+pRow->Model_asSQL()+", "+pRow->Device_asSQL()+", "+pRow->Command_asSQL()+", "+pRow->Pronto_asSQL()+", "+pRow->Frequency_asSQL()+", "+pRow->Data_asSQL()+", "+pRow->DataRepeat_asSQL()+", "+pRow->Leadout_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into InfraredData (PK_InfraredData, Manufacturer, Model, Device, Command, Pronto, Frequency, Data, DataRepeat, Leadout, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_InfraredData=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_InfraredData*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_InfraredData* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_InfraredData[32];
sprintf(tmp_PK_InfraredData, "%li", key.pk_PK_InfraredData);


string condition;
condition = condition + "PK_InfraredData=" + tmp_PK_InfraredData;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_InfraredData="+pRow->PK_InfraredData_asSQL()+", Manufacturer="+pRow->Manufacturer_asSQL()+", Model="+pRow->Model_asSQL()+", Device="+pRow->Device_asSQL()+", Command="+pRow->Command_asSQL()+", Pronto="+pRow->Pronto_asSQL()+", Frequency="+pRow->Frequency_asSQL()+", Data="+pRow->Data_asSQL()+", DataRepeat="+pRow->DataRepeat_asSQL()+", Leadout="+pRow->Leadout_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update InfraredData set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_InfraredData*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_InfraredData*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_InfraredData[32];
sprintf(tmp_PK_InfraredData, "%li", key.pk_PK_InfraredData);


string condition;
condition = condition + "PK_InfraredData=" + tmp_PK_InfraredData;

	
		string query = "delete from InfraredData where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_InfraredData::GetRows(string where_statement,vector<class Row_InfraredData*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from InfraredData where " + where_statement;
		
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

		Row_InfraredData *pRow = new Row_InfraredData(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InfraredData = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InfraredData));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Manufacturer = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Manufacturer = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Model = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Model = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Device = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Device = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Command = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Command = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Pronto = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Pronto = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Frequency = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Frequency));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Data = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Data = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DataRepeat = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_DataRepeat = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Leadout = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Leadout = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Modification_RecordInfo = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_InfraredData::Key, Row_InfraredData*, Table_InfraredData::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_InfraredData* Table_InfraredData::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_InfraredData *pRow = new Row_InfraredData(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_InfraredData* Table_InfraredData::GetRow(long int in_PK_InfraredData)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_InfraredData);

	map<Key, Row_InfraredData*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_InfraredData* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_InfraredData* Table_InfraredData::FetchRow(Table_InfraredData::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_InfraredData[32];
sprintf(tmp_PK_InfraredData, "%li", key.pk_PK_InfraredData);


string condition;
condition = condition + "PK_InfraredData=" + tmp_PK_InfraredData;


	string query = "select * from InfraredData where " + condition;		

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

	Row_InfraredData *pRow = new Row_InfraredData(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InfraredData = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InfraredData));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Manufacturer = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Manufacturer = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Model = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Model = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Device = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Device = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Command = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Command = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Pronto = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Pronto = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Frequency = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Frequency));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Data = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Data = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DataRepeat = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_DataRepeat = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Leadout = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Leadout = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Modification_RecordInfo = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}







