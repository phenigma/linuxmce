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
#include "Table_DeviceTemplate.h"
#include "Table_DeviceCategory.h"
#include "Table_Manufacturer.h"
#include "Table_StabilityStatus.h"
#include "Table_Users.h"

#include "Table_CommandGroup_D_Command.h"
#include "Table_ConfigType.h"
#include "Table_Device.h"
#include "Table_DeviceTemplate_DSPMode.h"
#include "Table_DeviceTemplate_DesignObj.h"
#include "Table_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceCommandGroup.h"
#include "Table_DeviceTemplate_DeviceData.h"
#include "Table_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceTemplate_Related.h"
#include "Table_DeviceTemplate_DeviceTemplate_Related.h"
#include "Table_DeviceTemplate_Event.h"
#include "Table_DeviceTemplate_InfraredGroup.h"
#include "Table_DeviceTemplate_Input.h"
#include "Table_DeviceTemplate_MediaType.h"
#include "Table_DeviceTemplate_Output.h"
#include "Table_DeviceTemplate_Package.h"
#include "Table_InfraredCode.h"


void Database_pluto_main::CreateTable_DeviceTemplate()
{
	tblDeviceTemplate = new Table_DeviceTemplate(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate()
{
	delete tblDeviceTemplate;
}

Table_DeviceTemplate::~Table_DeviceTemplate()
{
	map<Table_DeviceTemplate::Key, class Row_DeviceTemplate*, Table_DeviceTemplate::Key_Less>::iterator it;
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


void Row_DeviceTemplate::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DeviceTemplate*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DeviceTemplate::Key key(this);					
			map<Table_DeviceTemplate::Key, Row_DeviceTemplate*, Table_DeviceTemplate::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DeviceTemplate::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DeviceTemplate::Key key(this);		
		Row_DeviceTemplate *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate::Row_DeviceTemplate(Table_DeviceTemplate *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate::SetDefaultValues()
{
	m_PK_DeviceTemplate = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_FK_DeviceCategory = 0;
is_null[3] = false;
is_null[4] = true;
m_Define = "";
is_null[5] = false;
m_ImplementsDCE = 0;
is_null[6] = false;
m_IsEmbedded = 0;
is_null[7] = false;
is_null[8] = true;
m_RequiresGUI = 0;
is_null[9] = false;
m_IsAVDevice = 0;
is_null[10] = false;
m_IsPlugIn = 0;
is_null[11] = false;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
is_null[15] = true;
m_FK_Users_Maintainer = 1;
is_null[16] = false;
is_null[17] = true;
is_null[18] = true;
is_null[19] = true;
m_psc_frozen = 0;
is_null[20] = false;
m_psc_mod = "00000000000000";
is_null[21] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate::PK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_DeviceTemplate;}
string Row_DeviceTemplate::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_DeviceTemplate::Comments_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Comments;}
long int Row_DeviceTemplate::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DeviceCategory;}
long int Row_DeviceTemplate::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Manufacturer;}
string Row_DeviceTemplate::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
short int Row_DeviceTemplate::ImplementsDCE_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ImplementsDCE;}
short int Row_DeviceTemplate::IsEmbedded_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsEmbedded;}
string Row_DeviceTemplate::CommandLine_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_CommandLine;}
short int Row_DeviceTemplate::RequiresGUI_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_RequiresGUI;}
short int Row_DeviceTemplate::IsAVDevice_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsAVDevice;}
short int Row_DeviceTemplate::IsPlugIn_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsPlugIn;}
long int Row_DeviceTemplate::IRFrequency_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IRFrequency;}
long int Row_DeviceTemplate::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_StabilityStatus;}
string Row_DeviceTemplate::DestinationPackage_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DestinationPackage;}
string Row_DeviceTemplate::DestinationDir_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DestinationDir;}
long int Row_DeviceTemplate::FK_Users_Maintainer_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_Maintainer;}
long int Row_DeviceTemplate::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_DeviceTemplate::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_DeviceTemplate::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_DeviceTemplate::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_DeviceTemplate::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_DeviceTemplate::PK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate::Comments_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Comments = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate::FK_DeviceCategory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate::FK_Manufacturer_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Manufacturer = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate::ImplementsDCE_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ImplementsDCE = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate::IsEmbedded_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsEmbedded = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate::CommandLine_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_CommandLine = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate::RequiresGUI_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_RequiresGUI = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate::IsAVDevice_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsAVDevice = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate::IsPlugIn_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsPlugIn = val; is_modified=true; is_null[11]=false;}
void Row_DeviceTemplate::IRFrequency_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IRFrequency = val; is_modified=true; is_null[12]=false;}
void Row_DeviceTemplate::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[13]=false;}
void Row_DeviceTemplate::DestinationPackage_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DestinationPackage = val; is_modified=true; is_null[14]=false;}
void Row_DeviceTemplate::DestinationDir_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DestinationDir = val; is_modified=true; is_null[15]=false;}
void Row_DeviceTemplate::FK_Users_Maintainer_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_Maintainer = val; is_modified=true; is_null[16]=false;}
void Row_DeviceTemplate::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[17]=false;}
void Row_DeviceTemplate::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[18]=false;}
void Row_DeviceTemplate::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[19]=false;}
void Row_DeviceTemplate::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[20]=false;}
void Row_DeviceTemplate::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[21]=false;}

		
bool Row_DeviceTemplate::Comments_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_DeviceTemplate::FK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_DeviceTemplate::IsEmbedded_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_DeviceTemplate::CommandLine_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_DeviceTemplate::IsPlugIn_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_DeviceTemplate::IRFrequency_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_DeviceTemplate::FK_StabilityStatus_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_DeviceTemplate::DestinationPackage_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_DeviceTemplate::DestinationDir_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}
bool Row_DeviceTemplate::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[17];}
bool Row_DeviceTemplate::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[18];}
bool Row_DeviceTemplate::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[19];}
bool Row_DeviceTemplate::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[20];}

			
void Row_DeviceTemplate::Comments_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_DeviceTemplate::FK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_DeviceTemplate::IsEmbedded_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_DeviceTemplate::CommandLine_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_DeviceTemplate::IsPlugIn_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_DeviceTemplate::IRFrequency_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_DeviceTemplate::FK_StabilityStatus_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
void Row_DeviceTemplate::DestinationPackage_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_DeviceTemplate::DestinationDir_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[15]=val;}
void Row_DeviceTemplate::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[17]=val;}
void Row_DeviceTemplate::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[18]=val;}
void Row_DeviceTemplate::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[19]=val;}
void Row_DeviceTemplate::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[20]=val;}
	

string Row_DeviceTemplate::PK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::Comments_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Comments.c_str(), (unsigned long) m_Comments.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCategory);

return buf;
}

string Row_DeviceTemplate::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Manufacturer);

return buf;
}

string Row_DeviceTemplate::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::ImplementsDCE_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ImplementsDCE);

return buf;
}

string Row_DeviceTemplate::IsEmbedded_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsEmbedded);

return buf;
}

string Row_DeviceTemplate::CommandLine_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_CommandLine.c_str(), (unsigned long) m_CommandLine.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::RequiresGUI_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RequiresGUI);

return buf;
}

string Row_DeviceTemplate::IsAVDevice_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsAVDevice);

return buf;
}

string Row_DeviceTemplate::IsPlugIn_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsPlugIn);

return buf;
}

string Row_DeviceTemplate::IRFrequency_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_IRFrequency);

return buf;
}

string Row_DeviceTemplate::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_DeviceTemplate::DestinationPackage_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_DestinationPackage.c_str(), (unsigned long) m_DestinationPackage.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::DestinationDir_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_DestinationDir.c_str(), (unsigned long) m_DestinationDir.size());
return string()+"\""+buf+"\"";
}

string Row_DeviceTemplate::FK_Users_Maintainer_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_Maintainer);

return buf;
}

string Row_DeviceTemplate::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[21])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_DeviceTemplate::Key::Key(long int in_PK_DeviceTemplate)
{
			pk_PK_DeviceTemplate = in_PK_DeviceTemplate;
	
}

Table_DeviceTemplate::Key::Key(Row_DeviceTemplate *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_DeviceTemplate = pRow->m_PK_DeviceTemplate;
	
}		

bool Table_DeviceTemplate::Key_Less::operator()(const Table_DeviceTemplate::Key &key1, const Table_DeviceTemplate::Key &key2) const
{
			if (key1.pk_PK_DeviceTemplate!=key2.pk_PK_DeviceTemplate)
return key1.pk_PK_DeviceTemplate<key2.pk_PK_DeviceTemplate;
else
return false;	
}	

void Table_DeviceTemplate::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DeviceTemplate*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DeviceTemplate_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->ImplementsDCE_asSQL()+", "+pRow->IsEmbedded_asSQL()+", "+pRow->CommandLine_asSQL()+", "+pRow->RequiresGUI_asSQL()+", "+pRow->IsAVDevice_asSQL()+", "+pRow->IsPlugIn_asSQL()+", "+pRow->IRFrequency_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->DestinationPackage_asSQL()+", "+pRow->DestinationDir_asSQL()+", "+pRow->FK_Users_Maintainer_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into DeviceTemplate (PK_DeviceTemplate, Description, Comments, FK_DeviceCategory, FK_Manufacturer, Define, ImplementsDCE, IsEmbedded, CommandLine, RequiresGUI, IsAVDevice, IsPlugIn, IRFrequency, FK_StabilityStatus, DestinationPackage, DestinationDir, FK_Users_Maintainer, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_DeviceTemplate=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_DeviceTemplate*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DeviceTemplate* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk_PK_DeviceTemplate);


string condition;
condition = condition + "PK_DeviceTemplate=" + tmp_PK_DeviceTemplate;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_DeviceTemplate="+pRow->PK_DeviceTemplate_asSQL()+", Description="+pRow->Description_asSQL()+", Comments="+pRow->Comments_asSQL()+", FK_DeviceCategory="+pRow->FK_DeviceCategory_asSQL()+", FK_Manufacturer="+pRow->FK_Manufacturer_asSQL()+", Define="+pRow->Define_asSQL()+", ImplementsDCE="+pRow->ImplementsDCE_asSQL()+", IsEmbedded="+pRow->IsEmbedded_asSQL()+", CommandLine="+pRow->CommandLine_asSQL()+", RequiresGUI="+pRow->RequiresGUI_asSQL()+", IsAVDevice="+pRow->IsAVDevice_asSQL()+", IsPlugIn="+pRow->IsPlugIn_asSQL()+", IRFrequency="+pRow->IRFrequency_asSQL()+", FK_StabilityStatus="+pRow->FK_StabilityStatus_asSQL()+", DestinationPackage="+pRow->DestinationPackage_asSQL()+", DestinationDir="+pRow->DestinationDir_asSQL()+", FK_Users_Maintainer="+pRow->FK_Users_Maintainer_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update DeviceTemplate set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DeviceTemplate*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DeviceTemplate*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk_PK_DeviceTemplate);


string condition;
condition = condition + "PK_DeviceTemplate=" + tmp_PK_DeviceTemplate;

	
		string query = "delete from DeviceTemplate where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DeviceTemplate::GetRows(string where_statement,vector<class Row_DeviceTemplate*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DeviceTemplate where " + where_statement;
		
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

		Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Define = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ImplementsDCE = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_ImplementsDCE));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsEmbedded = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsEmbedded));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_CommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_CommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_RequiresGUI = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_RequiresGUI));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsAVDevice = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsAVDevice));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsPlugIn = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsPlugIn));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IRFrequency = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_IRFrequency));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_DestinationPackage = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_DestinationPackage = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DestinationDir = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_DestinationDir = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_Maintainer = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_Maintainer));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_DeviceTemplate::Key, Row_DeviceTemplate*, Table_DeviceTemplate::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DeviceTemplate* Table_DeviceTemplate::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate* Table_DeviceTemplate::GetRow(long int in_PK_DeviceTemplate)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_DeviceTemplate);

	map<Key, Row_DeviceTemplate*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DeviceTemplate* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate* Table_DeviceTemplate::FetchRow(Table_DeviceTemplate::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk_PK_DeviceTemplate);


string condition;
condition = condition + "PK_DeviceTemplate=" + tmp_PK_DeviceTemplate;


	string query = "select * from DeviceTemplate where " + condition;		

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

	Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Define = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ImplementsDCE = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_ImplementsDCE));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsEmbedded = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsEmbedded));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_CommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_CommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_RequiresGUI = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_RequiresGUI));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsAVDevice = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsAVDevice));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsPlugIn = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsPlugIn));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IRFrequency = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_IRFrequency));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_DestinationPackage = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_DestinationPackage = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DestinationDir = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_DestinationDir = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_Maintainer = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_Maintainer));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DeviceCategory* Row_DeviceTemplate::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}
class Row_Manufacturer* Row_DeviceTemplate::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}
class Row_StabilityStatus* Row_DeviceTemplate::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_Users* Row_DeviceTemplate::FK_Users_Maintainer_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users_Maintainer);
}


void Row_DeviceTemplate::CommandGroup_D_Command_FK_DeviceTemplate_getrows(vector <class Row_CommandGroup_D_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_D_Command *pTable = table->database->CommandGroup_D_Command_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::ConfigType_FK_DeviceTemplate_getrows(vector <class Row_ConfigType*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_ConfigType *pTable = table->database->ConfigType_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::Device_FK_DeviceTemplate_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DSPMode_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DSPMode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DSPMode *pTable = table->database->DeviceTemplate_DSPMode_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DesignObj_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DesignObj *pTable = table->database->DeviceTemplate_DesignObj_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceCategory_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceCategory_ControlledVia *pTable = table->database->DeviceTemplate_DeviceCategory_ControlledVia_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceCommandGroup *pTable = table->database->DeviceTemplate_DeviceCommandGroup_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceData_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceData*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceData *pTable = table->database->DeviceTemplate_DeviceData_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceTemplate_ControlledVia *pTable = table->database->DeviceTemplate_DeviceTemplate_ControlledVia_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceTemplate_ControlledVia *pTable = table->database->DeviceTemplate_DeviceTemplate_ControlledVia_get();
pTable->GetRows("FK_DeviceTemplate_ControlledVia=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceTemplate_Related *pTable = table->database->DeviceTemplate_DeviceTemplate_Related_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_Related_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_DeviceTemplate_Related *pTable = table->database->DeviceTemplate_DeviceTemplate_Related_get();
pTable->GetRows("FK_DeviceTemplate_Related=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Event_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Event*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Event *pTable = table->database->DeviceTemplate_Event_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_InfraredGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_InfraredGroup *pTable = table->database->DeviceTemplate_InfraredGroup_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Input_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Input*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Input *pTable = table->database->DeviceTemplate_Input_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_MediaType*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_MediaType *pTable = table->database->DeviceTemplate_MediaType_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Output_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Output*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Output *pTable = table->database->DeviceTemplate_Output_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Package_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Package*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_Package *pTable = table->database->DeviceTemplate_Package_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::InfraredCode_FK_DeviceTemplate_getrows(vector <class Row_InfraredCode*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_InfraredCode *pTable = table->database->InfraredCode_get();
pTable->GetRows("FK_DeviceTemplate=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}



