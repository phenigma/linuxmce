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
#include "Table_Device.h"
#include "Table_Room.h"
#include "Table_Installation.h"
#include "Table_DesignObj.h"
#include "Table_DeviceTemplate.h"
#include "Table_Device.h"
#include "Table_Input.h"
#include "Table_Device.h"
#include "Table_Input.h"
#include "Table_Device.h"
#include "Table_Device.h"

#include "Table_CommandGroup_Command.h"
#include "Table_CommandGroup_EntGroup.h"
#include "Table_Device.h"
#include "Table_Device.h"
#include "Table_Device.h"
#include "Table_Device.h"
#include "Table_Device_Command.h"
#include "Table_Device_CommandGroup.h"
#include "Table_Device_DeviceData.h"
#include "Table_Device_DeviceGroup.h"
#include "Table_Device_Device_Pipe.h"
#include "Table_Device_Device_Pipe.h"
#include "Table_Device_Device_Related.h"
#include "Table_Device_Device_Related.h"
#include "Table_Device_EntertainArea.h"
#include "Table_Device_HouseMode.h"
#include "Table_Device_Orbiter.h"
#include "Table_Device_Users.h"
#include "Table_InfraredCode.h"


void Database_pluto_main::CreateTable_Device()
{
	tblDevice = new Table_Device(this);
}

void Database_pluto_main::DeleteTable_Device()
{
	delete tblDevice;
}

Table_Device::~Table_Device()
{
	map<Table_Device::Key, class Row_Device*, Table_Device::Key_Less>::iterator it;
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


void Row_Device::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Device*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Device::Key key(this);					
			map<Table_Device::Key, Row_Device*, Table_Device::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Device::Key key(this);		
		Row_Device *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Device::Row_Device(Table_Device *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Device::SetDefaultValues()
{
	m_PK_Device = 0;
is_null[0] = false;
m_Disabled = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_Description = "";
is_null[5] = false;
m_FK_DeviceTemplate = 0;
is_null[6] = false;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
is_null[11] = true;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
m_IgnoreOnOff = 0;
is_null[15] = false;
is_null[16] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[17] = false;
m_IsNew_RecordInfo = 1;
is_null[18] = false;
m_IsDeleted_RecordInfo = 0;
is_null[19] = false;
is_null[20] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Device::PK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Device;}
short int Row_Device::Disabled_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Disabled;}
long int Row_Device::FK_Room_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Room;}
long int Row_Device::FK_Installation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Installation;}
long int Row_Device::FK_DesignObj_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj;}
string Row_Device::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Device::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceTemplate;}
long int Row_Device::FK_Device_Audio_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_Audio;}
long int Row_Device::FK_Input_Audio_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Input_Audio;}
long int Row_Device::FK_Device_Video_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_Video;}
long int Row_Device::FK_Input_Video_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Input_Video;}
long int Row_Device::FK_Device_ControlledVia_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_ControlledVia;}
long int Row_Device::FK_Interface_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Interface;}
string Row_Device::IPaddress_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IPaddress;}
string Row_Device::MACaddress_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_MACaddress;}
short int Row_Device::IgnoreOnOff_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IgnoreOnOff;}
long int Row_Device::FK_Device_SlaveTo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_SlaveTo;}
string Row_Device::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Device::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Device::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Device::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Device::PK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Device = val; is_modified=true; is_null[0]=false;}
void Row_Device::Disabled_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Disabled = val; is_modified=true; is_null[1]=false;}
void Row_Device::FK_Room_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Room = val; is_modified=true; is_null[2]=false;}
void Row_Device::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Installation = val; is_modified=true; is_null[3]=false;}
void Row_Device::FK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj = val; is_modified=true; is_null[4]=false;}
void Row_Device::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_Device::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[6]=false;}
void Row_Device::FK_Device_Audio_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_Audio = val; is_modified=true; is_null[7]=false;}
void Row_Device::FK_Input_Audio_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Input_Audio = val; is_modified=true; is_null[8]=false;}
void Row_Device::FK_Device_Video_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_Video = val; is_modified=true; is_null[9]=false;}
void Row_Device::FK_Input_Video_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Input_Video = val; is_modified=true; is_null[10]=false;}
void Row_Device::FK_Device_ControlledVia_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_ControlledVia = val; is_modified=true; is_null[11]=false;}
void Row_Device::FK_Interface_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Interface = val; is_modified=true; is_null[12]=false;}
void Row_Device::IPaddress_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IPaddress = val; is_modified=true; is_null[13]=false;}
void Row_Device::MACaddress_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_MACaddress = val; is_modified=true; is_null[14]=false;}
void Row_Device::IgnoreOnOff_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IgnoreOnOff = val; is_modified=true; is_null[15]=false;}
void Row_Device::FK_Device_SlaveTo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_SlaveTo = val; is_modified=true; is_null[16]=false;}
void Row_Device::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[17]=false;}
void Row_Device::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[18]=false;}
void Row_Device::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[19]=false;}
void Row_Device::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[20]=false;}

		
bool Row_Device::FK_Room_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Device::FK_Installation_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Device::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Device::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Device::FK_Device_Audio_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Device::FK_Input_Audio_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Device::FK_Device_Video_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_Device::FK_Input_Video_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_Device::FK_Device_ControlledVia_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_Device::FK_Interface_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_Device::IPaddress_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_Device::MACaddress_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_Device::FK_Device_SlaveTo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[16];}
bool Row_Device::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[18];}
bool Row_Device::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[19];}
bool Row_Device::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[20];}

			
void Row_Device::FK_Room_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Device::FK_Installation_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Device::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Device::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Device::FK_Device_Audio_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Device::FK_Input_Audio_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Device::FK_Device_Video_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_Device::FK_Input_Video_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_Device::FK_Device_ControlledVia_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_Device::FK_Interface_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_Device::IPaddress_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
void Row_Device::MACaddress_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_Device::FK_Device_SlaveTo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[16]=val;}
void Row_Device::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[18]=val;}
void Row_Device::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[19]=val;}
void Row_Device::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[20]=val;}
	

string Row_Device::PK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Device);

return buf;
}

string Row_Device::Disabled_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Disabled);

return buf;
}

string Row_Device::FK_Room_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Room);

return buf;
}

string Row_Device::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_Device::FK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj);

return buf;
}

string Row_Device::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Device::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_Device::FK_Device_Audio_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_Audio);

return buf;
}

string Row_Device::FK_Input_Audio_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Input_Audio);

return buf;
}

string Row_Device::FK_Device_Video_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_Video);

return buf;
}

string Row_Device::FK_Input_Video_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Input_Video);

return buf;
}

string Row_Device::FK_Device_ControlledVia_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_ControlledVia);

return buf;
}

string Row_Device::FK_Interface_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Interface);

return buf;
}

string Row_Device::IPaddress_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[31];
mysql_real_escape_string(table->database->db_handle, buf, m_IPaddress.c_str(), (unsigned long) m_IPaddress.size());
return string()+"\""+buf+"\"";
}

string Row_Device::MACaddress_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[37];
mysql_real_escape_string(table->database->db_handle, buf, m_MACaddress.c_str(), (unsigned long) m_MACaddress.size());
return string()+"\""+buf+"\"";
}

string Row_Device::IgnoreOnOff_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IgnoreOnOff);

return buf;
}

string Row_Device::FK_Device_SlaveTo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_SlaveTo);

return buf;
}

string Row_Device::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[17])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Device::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Device::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Device::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Device::Key::Key(long int in_PK_Device)
{
			pk_PK_Device = in_PK_Device;
	
}

Table_Device::Key::Key(Row_Device *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Device = pRow->m_PK_Device;
	
}		

bool Table_Device::Key_Less::operator()(const Table_Device::Key &key1, const Table_Device::Key &key2) const
{
			if (key1.pk_PK_Device!=key2.pk_PK_Device)
return key1.pk_PK_Device<key2.pk_PK_Device;
else
return false;	
}	

void Table_Device::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Device*>::iterator i = addedRows.begin();
	
		Row_Device *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Device_asSQL()+", "+pRow->Disabled_asSQL()+", "+pRow->FK_Room_asSQL()+", "+pRow->FK_Installation_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Device_Audio_asSQL()+", "+pRow->FK_Input_Audio_asSQL()+", "+pRow->FK_Device_Video_asSQL()+", "+pRow->FK_Input_Video_asSQL()+", "+pRow->FK_Device_ControlledVia_asSQL()+", "+pRow->FK_Interface_asSQL()+", "+pRow->IPaddress_asSQL()+", "+pRow->MACaddress_asSQL()+", "+pRow->IgnoreOnOff_asSQL()+", "+pRow->FK_Device_SlaveTo_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Device (PK_Device, Disabled, FK_Room, FK_Installation, FK_DesignObj, Description, FK_DeviceTemplate, FK_Device_Audio, FK_Input_Audio, FK_Device_Video, FK_Input_Video, FK_Device_ControlledVia, FK_Interface, IPaddress, MACaddress, IgnoreOnOff, FK_Device_SlaveTo, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Device=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Device*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Device* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk_PK_Device);


string condition;
condition = condition + "PK_Device=" + tmp_PK_Device;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Device="+pRow->PK_Device_asSQL()+", Disabled="+pRow->Disabled_asSQL()+", FK_Room="+pRow->FK_Room_asSQL()+", FK_Installation="+pRow->FK_Installation_asSQL()+", FK_DesignObj="+pRow->FK_DesignObj_asSQL()+", Description="+pRow->Description_asSQL()+", FK_DeviceTemplate="+pRow->FK_DeviceTemplate_asSQL()+", FK_Device_Audio="+pRow->FK_Device_Audio_asSQL()+", FK_Input_Audio="+pRow->FK_Input_Audio_asSQL()+", FK_Device_Video="+pRow->FK_Device_Video_asSQL()+", FK_Input_Video="+pRow->FK_Input_Video_asSQL()+", FK_Device_ControlledVia="+pRow->FK_Device_ControlledVia_asSQL()+", FK_Interface="+pRow->FK_Interface_asSQL()+", IPaddress="+pRow->IPaddress_asSQL()+", MACaddress="+pRow->MACaddress_asSQL()+", IgnoreOnOff="+pRow->IgnoreOnOff_asSQL()+", FK_Device_SlaveTo="+pRow->FK_Device_SlaveTo_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Device set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Device*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Device*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk_PK_Device);


string condition;
condition = condition + "PK_Device=" + tmp_PK_Device;

	
		string query = "delete from Device where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Device::GetRows(string where_statement,vector<class Row_Device*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Device where " + where_statement;
		
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

		Row_Device *pRow = new Row_Device(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Disabled = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_Disabled));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Room = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Room));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Installation));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObj));
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
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Device_Audio = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Device_Audio));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Input_Audio = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Input_Audio));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Device_Video = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Device_Video));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Input_Video = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Input_Video));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_ControlledVia = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_ControlledVia));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Interface = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Interface));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_IPaddress = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_MACaddress = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_IgnoreOnOff = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_IgnoreOnOff));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Device_SlaveTo = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Device_SlaveTo));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Modification_RecordInfo = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Device::Key, Row_Device*, Table_Device::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Device* Table_Device::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Device *pRow = new Row_Device(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Device* Table_Device::GetRow(long int in_PK_Device)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Device);

	map<Key, Row_Device*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Device* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device* Table_Device::FetchRow(Table_Device::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk_PK_Device);


string condition;
condition = condition + "PK_Device=" + tmp_PK_Device;


	string query = "select * from Device where " + condition;		

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

	Row_Device *pRow = new Row_Device(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Device = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Device));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Disabled = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_Disabled));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Room = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Room));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Installation));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObj));
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
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Device_Audio = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Device_Audio));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Input_Audio = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Input_Audio));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Device_Video = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Device_Video));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Input_Video = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Input_Video));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_ControlledVia = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_ControlledVia));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Interface = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Interface));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_IPaddress = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_MACaddress = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_IgnoreOnOff = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_IgnoreOnOff));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Device_SlaveTo = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Device_SlaveTo));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Modification_RecordInfo = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Room* Row_Device::FK_Room_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Room *pTable = table->database->Room_get();
return pTable->GetRow(m_FK_Room);
}
class Row_Installation* Row_Device::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}
class Row_DesignObj* Row_Device::FK_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj);
}
class Row_DeviceTemplate* Row_Device::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_Device* Row_Device::FK_Device_Audio_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_Audio);
}
class Row_Input* Row_Device::FK_Input_Audio_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Input *pTable = table->database->Input_get();
return pTable->GetRow(m_FK_Input_Audio);
}
class Row_Device* Row_Device::FK_Device_Video_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_Video);
}
class Row_Input* Row_Device::FK_Input_Video_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Input *pTable = table->database->Input_get();
return pTable->GetRow(m_FK_Input_Video);
}
class Row_Device* Row_Device::FK_Device_ControlledVia_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_ControlledVia);
}
class Row_Device* Row_Device::FK_Device_SlaveTo_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_SlaveTo);
}


void Row_Device::CommandGroup_Command_FK_Device_getrows(vector <class Row_CommandGroup_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_Command *pTable = table->database->CommandGroup_Command_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::CommandGroup_EntGroup_FK_Device_getrows(vector <class Row_CommandGroup_EntGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_EntGroup *pTable = table->database->CommandGroup_EntGroup_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_Audio_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_Device_Audio=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_Video_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_Device_Video=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_ControlledVia_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_Device_ControlledVia=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_SlaveTo_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_Device_SlaveTo=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Command_FK_Device_getrows(vector <class Row_Device_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Command *pTable = table->database->Device_Command_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_CommandGroup_FK_Device_getrows(vector <class Row_Device_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_CommandGroup *pTable = table->database->Device_CommandGroup_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_DeviceData_FK_Device_getrows(vector <class Row_Device_DeviceData*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_DeviceData *pTable = table->database->Device_DeviceData_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_DeviceGroup_FK_Device_getrows(vector <class Row_Device_DeviceGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_DeviceGroup *pTable = table->database->Device_DeviceGroup_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Pipe_FK_Device_From_getrows(vector <class Row_Device_Device_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Pipe *pTable = table->database->Device_Device_Pipe_get();
pTable->GetRows("FK_Device_From=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Pipe_FK_Device_To_getrows(vector <class Row_Device_Device_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Pipe *pTable = table->database->Device_Device_Pipe_get();
pTable->GetRows("FK_Device_To=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Related_FK_Device_getrows(vector <class Row_Device_Device_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Related *pTable = table->database->Device_Device_Related_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Related_FK_Device_Related_getrows(vector <class Row_Device_Device_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Related *pTable = table->database->Device_Device_Related_get();
pTable->GetRows("FK_Device_Related=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_EntertainArea_FK_Device_getrows(vector <class Row_Device_EntertainArea*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_EntertainArea *pTable = table->database->Device_EntertainArea_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_HouseMode_FK_Device_getrows(vector <class Row_Device_HouseMode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_HouseMode *pTable = table->database->Device_HouseMode_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Orbiter_FK_Device_getrows(vector <class Row_Device_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Orbiter *pTable = table->database->Device_Orbiter_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Users_FK_Device_getrows(vector <class Row_Device_Users*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Users *pTable = table->database->Device_Users_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::InfraredCode_FK_Device_getrows(vector <class Row_InfraredCode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_InfraredCode *pTable = table->database->InfraredCode_get();
pTable->GetRows("FK_Device=" + StringUtils::itos(m_PK_Device),rows);
}



