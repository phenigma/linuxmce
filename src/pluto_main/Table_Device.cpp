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
#include "Table_Device.h"
#include "Table_Room.h"
#include "Table_Installation.h"
#include "Table_DesignObj.h"
#include "Table_DeviceTemplate.h"
#include "Table_Device.h"
#include "Table_Device.h"

#include "Table_CommandGroup_Command.h"
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
#include "Table_Device_StartupScript.h"
#include "Table_Device_Users.h"
#include "Table_InfraredGroup_Command.h"


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
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Device *pRow = (Row_Device *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Device *pRow = (Row_Device *) (*it).second;
		delete pRow;
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
	Row_Device *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Device *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Device);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Device::Reload()
{
	Row_Device *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Device);
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
m_IgnoreOnOff = 0;
is_null[10] = false;
is_null[11] = true;
m_NeedConfigure = 1;
is_null[12] = false;
is_null[13] = true;
is_null[14] = true;
is_null[15] = true;
is_null[16] = true;
is_null[17] = true;
m_psc_frozen = 0;
is_null[18] = false;
m_psc_mod = "00000000000000";
is_null[19] = false;


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
long int Row_Device::FK_Device_ControlledVia_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_ControlledVia;}
string Row_Device::IPaddress_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IPaddress;}
string Row_Device::MACaddress_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_MACaddress;}
short int Row_Device::IgnoreOnOff_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IgnoreOnOff;}
long int Row_Device::FK_Device_RouteTo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Device_RouteTo;}
short int Row_Device::NeedConfigure_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_NeedConfigure;}
string Row_Device::State_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_State;}
string Row_Device::Status_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Status;}
long int Row_Device::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Device::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Device::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Device::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Device::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
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
void Row_Device::FK_Device_ControlledVia_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_ControlledVia = val; is_modified=true; is_null[7]=false;}
void Row_Device::IPaddress_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IPaddress = val; is_modified=true; is_null[8]=false;}
void Row_Device::MACaddress_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_MACaddress = val; is_modified=true; is_null[9]=false;}
void Row_Device::IgnoreOnOff_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IgnoreOnOff = val; is_modified=true; is_null[10]=false;}
void Row_Device::FK_Device_RouteTo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Device_RouteTo = val; is_modified=true; is_null[11]=false;}
void Row_Device::NeedConfigure_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_NeedConfigure = val; is_modified=true; is_null[12]=false;}
void Row_Device::State_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_State = val; is_modified=true; is_null[13]=false;}
void Row_Device::Status_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Status = val; is_modified=true; is_null[14]=false;}
void Row_Device::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[15]=false;}
void Row_Device::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[16]=false;}
void Row_Device::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[17]=false;}
void Row_Device::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[18]=false;}
void Row_Device::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[19]=false;}

		
bool Row_Device::FK_Room_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Device::FK_Installation_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Device::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Device::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Device::FK_Device_ControlledVia_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Device::IPaddress_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Device::MACaddress_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_Device::FK_Device_RouteTo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_Device::NeedConfigure_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_Device::State_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_Device::Status_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_Device::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}
bool Row_Device::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[16];}
bool Row_Device::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[17];}
bool Row_Device::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[18];}

			
void Row_Device::FK_Room_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Device::FK_Installation_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Device::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Device::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Device::FK_Device_ControlledVia_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Device::IPaddress_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Device::MACaddress_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_Device::FK_Device_RouteTo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_Device::NeedConfigure_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_Device::State_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
void Row_Device::Status_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_Device::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[15]=val;}
void Row_Device::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[16]=val;}
void Row_Device::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[17]=val;}
void Row_Device::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[18]=val;}
	

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

char *buf = new char[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
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

string Row_Device::FK_Device_ControlledVia_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_ControlledVia);

return buf;
}

string Row_Device::IPaddress_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char *buf = new char[31];
mysql_real_escape_string(table->database->db_handle, buf, m_IPaddress.c_str(), (unsigned long) m_IPaddress.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Device::MACaddress_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char *buf = new char[37];
mysql_real_escape_string(table->database->db_handle, buf, m_MACaddress.c_str(), (unsigned long) m_MACaddress.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Device::IgnoreOnOff_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IgnoreOnOff);

return buf;
}

string Row_Device::FK_Device_RouteTo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_RouteTo);

return buf;
}

string Row_Device::NeedConfigure_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_NeedConfigure);

return buf;
}

string Row_Device::State_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char *buf = new char[81];
mysql_real_escape_string(table->database->db_handle, buf, m_State.c_str(), (unsigned long) m_State.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Device::Status_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char *buf = new char[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Status.c_str(), (unsigned long) m_Status.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Device::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Device::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Device::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Device::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Device::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[19])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
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

bool Table_Device::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Device *pRow = (Row_Device *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Device_asSQL()+", "+pRow->Disabled_asSQL()+", "+pRow->FK_Room_asSQL()+", "+pRow->FK_Installation_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Device_ControlledVia_asSQL()+", "+pRow->IPaddress_asSQL()+", "+pRow->MACaddress_asSQL()+", "+pRow->IgnoreOnOff_asSQL()+", "+pRow->FK_Device_RouteTo_asSQL()+", "+pRow->NeedConfigure_asSQL()+", "+pRow->State_asSQL()+", "+pRow->Status_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Device (`PK_Device`, `Disabled`, `FK_Room`, `FK_Installation`, `FK_DesignObj`, `Description`, `FK_DeviceTemplate`, `FK_Device_ControlledVia`, `IPaddress`, `MACaddress`, `IgnoreOnOff`, `FK_Device_RouteTo`, `NeedConfigure`, `State`, `Status`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Device=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Device);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Device* pRow = (Row_Device*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Device);

		char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk);


string condition;
condition = condition + "`PK_Device`=" + tmp_PK_Device;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Device`="+pRow->PK_Device_asSQL()+", `Disabled`="+pRow->Disabled_asSQL()+", `FK_Room`="+pRow->FK_Room_asSQL()+", `FK_Installation`="+pRow->FK_Installation_asSQL()+", `FK_DesignObj`="+pRow->FK_DesignObj_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `FK_Device_ControlledVia`="+pRow->FK_Device_ControlledVia_asSQL()+", `IPaddress`="+pRow->IPaddress_asSQL()+", `MACaddress`="+pRow->MACaddress_asSQL()+", `IgnoreOnOff`="+pRow->IgnoreOnOff_asSQL()+", `FK_Device_RouteTo`="+pRow->FK_Device_RouteTo_asSQL()+", `NeedConfigure`="+pRow->NeedConfigure_asSQL()+", `State`="+pRow->State_asSQL()+", `Status`="+pRow->Status_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Device set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
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
		Row_Device* pRow = (Row_Device*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Device* pRow = (Row_Device*) (*i).second;	

		char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk);


string condition;
condition = condition + "`PK_Device`=" + tmp_PK_Device;

	
		string query = "delete from Device where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_Device*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Device::GetRows(string where_statement,vector<class Row_Device*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Device " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Device where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
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
pRow->m_FK_Device_ControlledVia = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Device_ControlledVia));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_IPaddress = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_MACaddress = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IgnoreOnOff = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IgnoreOnOff));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_RouteTo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_RouteTo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_NeedConfigure = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_NeedConfigure));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_State = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Status = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Status = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_id));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_batch));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_user));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_psc_frozen));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_psc_mod = string(row[19],lengths[19]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Device);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Device *)(*i).second;
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

	SingleLongKey row_key(in_PK_Device);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Device*) (*i).second;
	//we have to fetch row
	Row_Device* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Device* Table_Device::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Device[32];
sprintf(tmp_PK_Device, "%li", key.pk);


string condition;
condition = condition + "`PK_Device`=" + tmp_PK_Device;


	string query = "select * from Device where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
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
pRow->m_FK_Device_ControlledVia = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Device_ControlledVia));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_IPaddress = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_MACaddress = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IgnoreOnOff = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IgnoreOnOff));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_RouteTo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_RouteTo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_NeedConfigure = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_NeedConfigure));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_State = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Status = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Status = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_id));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_batch));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_user));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_psc_frozen));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_psc_mod = string(row[19],lengths[19]);
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
class Row_Device* Row_Device::FK_Device_ControlledVia_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_ControlledVia);
}
class Row_Device* Row_Device::FK_Device_RouteTo_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_RouteTo);
}


void Row_Device::CommandGroup_Command_FK_Device_getrows(vector <class Row_CommandGroup_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_Command *pTable = table->database->CommandGroup_Command_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_ControlledVia_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_Device_ControlledVia`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_FK_Device_RouteTo_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_Device_RouteTo`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Command_FK_Device_getrows(vector <class Row_Device_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Command *pTable = table->database->Device_Command_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_CommandGroup_FK_Device_getrows(vector <class Row_Device_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_CommandGroup *pTable = table->database->Device_CommandGroup_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_DeviceData_FK_Device_getrows(vector <class Row_Device_DeviceData*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_DeviceData *pTable = table->database->Device_DeviceData_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_DeviceGroup_FK_Device_getrows(vector <class Row_Device_DeviceGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_DeviceGroup *pTable = table->database->Device_DeviceGroup_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Pipe_FK_Device_From_getrows(vector <class Row_Device_Device_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Pipe *pTable = table->database->Device_Device_Pipe_get();
pTable->GetRows("`FK_Device_From`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Pipe_FK_Device_To_getrows(vector <class Row_Device_Device_Pipe*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Pipe *pTable = table->database->Device_Device_Pipe_get();
pTable->GetRows("`FK_Device_To`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Related_FK_Device_getrows(vector <class Row_Device_Device_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Related *pTable = table->database->Device_Device_Related_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Device_Related_FK_Device_Related_getrows(vector <class Row_Device_Device_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Device_Related *pTable = table->database->Device_Device_Related_get();
pTable->GetRows("`FK_Device_Related`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_EntertainArea_FK_Device_getrows(vector <class Row_Device_EntertainArea*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_EntertainArea *pTable = table->database->Device_EntertainArea_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_HouseMode_FK_Device_getrows(vector <class Row_Device_HouseMode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_HouseMode *pTable = table->database->Device_HouseMode_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Orbiter_FK_Device_getrows(vector <class Row_Device_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Orbiter *pTable = table->database->Device_Orbiter_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_StartupScript_FK_Device_getrows(vector <class Row_Device_StartupScript*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_StartupScript *pTable = table->database->Device_StartupScript_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::Device_Users_FK_Device_getrows(vector <class Row_Device_Users*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Users *pTable = table->database->Device_Users_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}
void Row_Device::InfraredGroup_Command_FK_Device_getrows(vector <class Row_InfraredGroup_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_InfraredGroup_Command *pTable = table->database->InfraredGroup_Command_get();
pTable->GetRows("`FK_Device`=" + StringUtils::itos(m_PK_Device),rows);
}



