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
#include "Table_Installation.h"
#include "Table_Version.h"

#include "Table_CommandGroup.h"
#include "Table_Device.h"
#include "Table_DeviceGroup.h"
#include "Table_EventHandler.h"
#include "Table_Floorplan.h"
#include "Table_Household_Installation.h"
#include "Table_Installation_Users.h"
#include "Table_Room.h"
#include "Table_Users.h"


void Database_pluto_main::CreateTable_Installation()
{
	tblInstallation = new Table_Installation(this);
}

void Database_pluto_main::DeleteTable_Installation()
{
	delete tblInstallation;
}

Table_Installation::~Table_Installation()
{
	map<Table_Installation::Key, class Row_Installation*, Table_Installation::Key_Less>::iterator it;
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


void Row_Installation::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Installation*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Installation::Key key(this);					
			map<Table_Installation::Key, Row_Installation*, Table_Installation::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Installation::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Installation::Key key(this);		
		Row_Installation *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Installation::Row_Installation(Table_Installation *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Installation::SetDefaultValues()
{
	m_PK_Installation = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_isActive = 0;
is_null[10] = false;
is_null[11] = true;
is_null[12] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[13] = false;
m_IsNew_RecordInfo = 1;
is_null[14] = false;
m_IsDeleted_RecordInfo = 0;
is_null[15] = false;
is_null[16] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Installation::PK_Installation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Installation;}
string Row_Installation::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Installation::Name_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Name;}
string Row_Installation::Address_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Address;}
string Row_Installation::City_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_City;}
string Row_Installation::State_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_State;}
string Row_Installation::Zip_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Zip;}
string Row_Installation::ActivationCode_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ActivationCode;}
string Row_Installation::LastStatus_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_LastStatus;}
string Row_Installation::LastAlive_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_LastAlive;}
short int Row_Installation::isActive_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_isActive;}
long int Row_Installation::FK_Version_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Version;}
short int Row_Installation::isMonitored_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_isMonitored;}
string Row_Installation::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Installation::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Installation::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Installation::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Installation::PK_Installation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Installation = val; is_modified=true; is_null[0]=false;}
void Row_Installation::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Installation::Name_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Name = val; is_modified=true; is_null[2]=false;}
void Row_Installation::Address_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Address = val; is_modified=true; is_null[3]=false;}
void Row_Installation::City_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_City = val; is_modified=true; is_null[4]=false;}
void Row_Installation::State_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_State = val; is_modified=true; is_null[5]=false;}
void Row_Installation::Zip_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Zip = val; is_modified=true; is_null[6]=false;}
void Row_Installation::ActivationCode_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ActivationCode = val; is_modified=true; is_null[7]=false;}
void Row_Installation::LastStatus_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_LastStatus = val; is_modified=true; is_null[8]=false;}
void Row_Installation::LastAlive_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_LastAlive = val; is_modified=true; is_null[9]=false;}
void Row_Installation::isActive_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_isActive = val; is_modified=true; is_null[10]=false;}
void Row_Installation::FK_Version_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Version = val; is_modified=true; is_null[11]=false;}
void Row_Installation::isMonitored_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_isMonitored = val; is_modified=true; is_null[12]=false;}
void Row_Installation::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[13]=false;}
void Row_Installation::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[14]=false;}
void Row_Installation::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[15]=false;}
void Row_Installation::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[16]=false;}

		
bool Row_Installation::Name_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Installation::Address_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Installation::City_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Installation::State_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Installation::Zip_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Installation::ActivationCode_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Installation::LastStatus_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Installation::LastAlive_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_Installation::FK_Version_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_Installation::isMonitored_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_Installation::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_Installation::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}
bool Row_Installation::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[16];}

			
void Row_Installation::Name_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Installation::Address_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Installation::City_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Installation::State_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Installation::Zip_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Installation::ActivationCode_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Installation::LastStatus_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Installation::LastAlive_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_Installation::FK_Version_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_Installation::isMonitored_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_Installation::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_Installation::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[15]=val;}
void Row_Installation::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[16]=val;}
	

string Row_Installation::PK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Installation);

return buf;
}

string Row_Installation::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::Name_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Name.c_str(), (unsigned long) m_Name.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::Address_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Address.c_str(), (unsigned long) m_Address.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::City_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_City.c_str(), (unsigned long) m_City.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::State_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[21];
mysql_real_escape_string(table->database->db_handle, buf, m_State.c_str(), (unsigned long) m_State.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::Zip_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[21];
mysql_real_escape_string(table->database->db_handle, buf, m_Zip.c_str(), (unsigned long) m_Zip.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::ActivationCode_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_ActivationCode.c_str(), (unsigned long) m_ActivationCode.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::LastStatus_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_LastStatus.c_str(), (unsigned long) m_LastStatus.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::LastAlive_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[39];
mysql_real_escape_string(table->database->db_handle, buf, m_LastAlive.c_str(), (unsigned long) m_LastAlive.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::isActive_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_isActive);

return buf;
}

string Row_Installation::FK_Version_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Version);

return buf;
}

string Row_Installation::isMonitored_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_isMonitored);

return buf;
}

string Row_Installation::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Installation::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Installation::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Installation::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Installation::Key::Key(long int in_PK_Installation)
{
			pk_PK_Installation = in_PK_Installation;
	
}

Table_Installation::Key::Key(Row_Installation *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Installation = pRow->m_PK_Installation;
	
}		

bool Table_Installation::Key_Less::operator()(const Table_Installation::Key &key1, const Table_Installation::Key &key2) const
{
			if (key1.pk_PK_Installation!=key2.pk_PK_Installation)
return key1.pk_PK_Installation<key2.pk_PK_Installation;
else
return false;	
}	

void Table_Installation::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Installation*>::iterator i = addedRows.begin();
	
		Row_Installation *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Installation_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Address_asSQL()+", "+pRow->City_asSQL()+", "+pRow->State_asSQL()+", "+pRow->Zip_asSQL()+", "+pRow->ActivationCode_asSQL()+", "+pRow->LastStatus_asSQL()+", "+pRow->LastAlive_asSQL()+", "+pRow->isActive_asSQL()+", "+pRow->FK_Version_asSQL()+", "+pRow->isMonitored_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Installation (PK_Installation, Description, Name, Address, City, State, Zip, ActivationCode, LastStatus, LastAlive, isActive, FK_Version, isMonitored, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Installation=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Installation*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Installation* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk_PK_Installation);


string condition;
condition = condition + "PK_Installation=" + tmp_PK_Installation;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Installation="+pRow->PK_Installation_asSQL()+", Description="+pRow->Description_asSQL()+", Name="+pRow->Name_asSQL()+", Address="+pRow->Address_asSQL()+", City="+pRow->City_asSQL()+", State="+pRow->State_asSQL()+", Zip="+pRow->Zip_asSQL()+", ActivationCode="+pRow->ActivationCode_asSQL()+", LastStatus="+pRow->LastStatus_asSQL()+", LastAlive="+pRow->LastAlive_asSQL()+", isActive="+pRow->isActive_asSQL()+", FK_Version="+pRow->FK_Version_asSQL()+", isMonitored="+pRow->isMonitored_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Installation set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Installation*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Installation*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk_PK_Installation);


string condition;
condition = condition + "PK_Installation=" + tmp_PK_Installation;

	
		string query = "delete from Installation where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Installation::GetRows(string where_statement,vector<class Row_Installation*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Installation where " + where_statement;
		
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

		Row_Installation *pRow = new Row_Installation(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
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
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Zip = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Zip = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ActivationCode = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ActivationCode = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastStatus = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_LastStatus = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_LastAlive = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_LastAlive = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_isActive = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_isActive));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Version));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_isMonitored = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_isMonitored));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Modification_RecordInfo = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Installation::Key, Row_Installation*, Table_Installation::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Installation* Table_Installation::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Installation *pRow = new Row_Installation(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Installation* Table_Installation::GetRow(long int in_PK_Installation)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Installation);

	map<Key, Row_Installation*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Installation* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Installation* Table_Installation::FetchRow(Table_Installation::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk_PK_Installation);


string condition;
condition = condition + "PK_Installation=" + tmp_PK_Installation;


	string query = "select * from Installation where " + condition;		

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

	Row_Installation *pRow = new Row_Installation(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Installation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
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
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Zip = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Zip = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ActivationCode = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ActivationCode = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastStatus = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_LastStatus = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_LastAlive = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_LastAlive = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_isActive = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_isActive));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Version));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_isMonitored = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_isMonitored));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Modification_RecordInfo = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Version* Row_Installation::FK_Version_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Version *pTable = table->database->Version_get();
return pTable->GetRow(m_FK_Version);
}


void Row_Installation::CommandGroup_FK_Installation_getrows(vector <class Row_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup *pTable = table->database->CommandGroup_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Device_FK_Installation_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::DeviceGroup_FK_Installation_getrows(vector <class Row_DeviceGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceGroup *pTable = table->database->DeviceGroup_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::EventHandler_FK_Installation_getrows(vector <class Row_EventHandler*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EventHandler *pTable = table->database->EventHandler_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Floorplan_FK_Installation_getrows(vector <class Row_Floorplan*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Floorplan *pTable = table->database->Floorplan_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Household_Installation_FK_Installation_getrows(vector <class Row_Household_Installation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Household_Installation *pTable = table->database->Household_Installation_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Installation_Users_FK_Installation_getrows(vector <class Row_Installation_Users*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation_Users *pTable = table->database->Installation_Users_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Room_FK_Installation_getrows(vector <class Row_Room*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Room *pTable = table->database->Room_get();
pTable->GetRows("FK_Installation=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Users_FK_Installation_Main_getrows(vector <class Row_Users*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Users *pTable = table->database->Users_get();
pTable->GetRows("FK_Installation_Main=" + StringUtils::itos(m_PK_Installation),rows);
}



