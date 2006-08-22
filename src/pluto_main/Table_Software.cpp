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
#include "Table_Software.h"
#include "Table_Device.h"



void Database_pluto_main::CreateTable_Software()
{
	tblSoftware = new Table_Software(this);
}

void Database_pluto_main::DeleteTable_Software()
{
	if( tblSoftware )
		delete tblSoftware;
}

bool Database_pluto_main::Commit_Software(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblSoftware->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Software::~Table_Software()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Software *pRow = (Row_Software *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Software *pRow = (Row_Software *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Software::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Software *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Software *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Software);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Software::Reload()
{
	Row_Software *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Software);
		Row_Software *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Software::Row_Software(Table_Software *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Software::SetDefaultValues()
{
	m_PK_Software = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_Device = 0;
is_null[2] = true;
is_null[3] = true;
m_Title = "unknown";
is_null[4] = false;
is_null[5] = true;
m_HomeURL = "unknown";
is_null[6] = false;
m_Category = "unknown";
is_null[7] = false;
m_Downloadurl = "unknown";
is_null[8] = false;
m_PackageName = "unknown";
is_null[9] = false;
m_Misc = "unknown";
is_null[10] = false;
m_Version = "unknown";
is_null[11] = false;
m_Target = "unknown";
is_null[12] = false;
m_Importance = "unknown";
is_null[13] = false;
m_Sum_md5 = "";
is_null[14] = false;
m_Sum_sha = "";
is_null[15] = false;
m_Rating = 0.00;
is_null[16] = false;
m_Virus_Free = "No";
is_null[17] = false;
m_PC_Type = "SM";
is_null[18] = false;
is_null[19] = true;
is_null[20] = true;
m_Installation_status = "No";
is_null[21] = false;
is_null[22] = true;
m_psc_id = 0;
is_null[23] = true;
m_psc_batch = 0;
is_null[24] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[25] = false;
m_psc_mod = "00000000000000";
is_null[26] = false;
is_null[27] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Software::PK_Software_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PK_Software;}
long int Row_Software::FK_Device_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Device;}
string Row_Software::RepositoryName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_RepositoryName;}
string Row_Software::Iconstr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Iconstr;}
string Row_Software::Title_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Title;}
string Row_Software::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Description;}
string Row_Software::HomeURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_HomeURL;}
string Row_Software::Category_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Category;}
string Row_Software::Downloadurl_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Downloadurl;}
string Row_Software::PackageName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PackageName;}
string Row_Software::Misc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Misc;}
string Row_Software::Version_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Version;}
string Row_Software::Target_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Target;}
string Row_Software::Importance_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Importance;}
string Row_Software::Sum_md5_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Sum_md5;}
string Row_Software::Sum_sha_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Sum_sha;}
float Row_Software::Rating_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Rating;}
string Row_Software::Virus_Free_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Virus_Free;}
string Row_Software::PC_Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PC_Type;}
string Row_Software::Required_Version_Min_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Required_Version_Min;}
string Row_Software::Required_Version_Max_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Required_Version_Max;}
string Row_Software::Installation_status_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Installation_status;}
long int Row_Software::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_Software::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_Software::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_Software::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_Software::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}
long int Row_Software::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_restrict;}

		
void Row_Software::PK_Software_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PK_Software = val; is_modified=true; is_null[0]=false;}
void Row_Software::FK_Device_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Device = val; is_modified=true; is_null[1]=false;}
void Row_Software::RepositoryName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_RepositoryName = val; is_modified=true; is_null[2]=false;}
void Row_Software::Iconstr_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Iconstr = val; is_modified=true; is_null[3]=false;}
void Row_Software::Title_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Title = val; is_modified=true; is_null[4]=false;}
void Row_Software::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_Software::HomeURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_HomeURL = val; is_modified=true; is_null[6]=false;}
void Row_Software::Category_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Category = val; is_modified=true; is_null[7]=false;}
void Row_Software::Downloadurl_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Downloadurl = val; is_modified=true; is_null[8]=false;}
void Row_Software::PackageName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PackageName = val; is_modified=true; is_null[9]=false;}
void Row_Software::Misc_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Misc = val; is_modified=true; is_null[10]=false;}
void Row_Software::Version_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Version = val; is_modified=true; is_null[11]=false;}
void Row_Software::Target_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Target = val; is_modified=true; is_null[12]=false;}
void Row_Software::Importance_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Importance = val; is_modified=true; is_null[13]=false;}
void Row_Software::Sum_md5_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Sum_md5 = val; is_modified=true; is_null[14]=false;}
void Row_Software::Sum_sha_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Sum_sha = val; is_modified=true; is_null[15]=false;}
void Row_Software::Rating_set(float val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Rating = val; is_modified=true; is_null[16]=false;}
void Row_Software::Virus_Free_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Virus_Free = val; is_modified=true; is_null[17]=false;}
void Row_Software::PC_Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PC_Type = val; is_modified=true; is_null[18]=false;}
void Row_Software::Required_Version_Min_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Required_Version_Min = val; is_modified=true; is_null[19]=false;}
void Row_Software::Required_Version_Max_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Required_Version_Max = val; is_modified=true; is_null[20]=false;}
void Row_Software::Installation_status_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Installation_status = val; is_modified=true; is_null[21]=false;}
void Row_Software::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[22]=false;}
void Row_Software::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[23]=false;}
void Row_Software::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[24]=false;}
void Row_Software::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[25]=false;}
void Row_Software::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[26]=false;}
void Row_Software::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_restrict = val; is_modified=true; is_null[27]=false;}

		
bool Row_Software::FK_Device_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[1];}
bool Row_Software::RepositoryName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[2];}
bool Row_Software::Iconstr_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_Software::Title_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_Software::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_Software::HomeURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[6];}
bool Row_Software::Category_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[7];}
bool Row_Software::Downloadurl_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[8];}
bool Row_Software::PackageName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[9];}
bool Row_Software::Misc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[10];}
bool Row_Software::Version_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[11];}
bool Row_Software::Target_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[12];}
bool Row_Software::Importance_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[13];}
bool Row_Software::Sum_md5_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[14];}
bool Row_Software::Sum_sha_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[15];}
bool Row_Software::Virus_Free_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[17];}
bool Row_Software::PC_Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[18];}
bool Row_Software::Required_Version_Min_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[19];}
bool Row_Software::Required_Version_Max_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[20];}
bool Row_Software::Installation_status_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[21];}
bool Row_Software::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[22];}
bool Row_Software::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[23];}
bool Row_Software::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[24];}
bool Row_Software::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[25];}
bool Row_Software::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[27];}

			
void Row_Software::FK_Device_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Software::RepositoryName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Software::Iconstr_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Software::Title_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Software::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Software::HomeURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Software::Category_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Software::Downloadurl_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Software::PackageName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Software::Misc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Software::Version_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Software::Target_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Software::Importance_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Software::Sum_md5_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Software::Sum_sha_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Software::Virus_Free_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Software::PC_Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Software::Required_Version_Min_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Software::Required_Version_Max_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Software::Installation_status_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[21]=val;
is_modified=true;
}
void Row_Software::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[22]=val;
is_modified=true;
}
void Row_Software::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[23]=val;
is_modified=true;
}
void Row_Software::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[24]=val;
is_modified=true;
}
void Row_Software::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[25]=val;
is_modified=true;
}
void Row_Software::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[27]=val;
is_modified=true;
}
	

string Row_Software::PK_Software_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Software);

return buf;
}

string Row_Software::FK_Device_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device);

return buf;
}

string Row_Software::RepositoryName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_RepositoryName.c_str(), (unsigned long) min(100,m_RepositoryName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Iconstr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Iconstr.c_str(), (unsigned long) min(65535,m_Iconstr.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Title_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Title.c_str(), (unsigned long) min(20,m_Title.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char *buf = new char[131071];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) min(65535,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::HomeURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_HomeURL.c_str(), (unsigned long) min(100,m_HomeURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Category_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Category.c_str(), (unsigned long) min(20,m_Category.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Downloadurl_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[8])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Downloadurl.c_str(), (unsigned long) min(100,m_Downloadurl.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::PackageName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[9])
return "NULL";

char *buf = new char[129];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_PackageName.c_str(), (unsigned long) min(64,m_PackageName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Misc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[10])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Misc.c_str(), (unsigned long) min(20,m_Misc.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Version_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[11])
return "NULL";

char *buf = new char[21];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Version.c_str(), (unsigned long) min(10,m_Version.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Target_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[12])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Target.c_str(), (unsigned long) min(20,m_Target.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Importance_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[13])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Importance.c_str(), (unsigned long) min(20,m_Importance.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Sum_md5_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[14])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Sum_md5.c_str(), (unsigned long) min(20,m_Sum_md5.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Sum_sha_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[15])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Sum_sha.c_str(), (unsigned long) min(20,m_Sum_sha.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Rating_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%f", m_Rating);

return buf;
}

string Row_Software::Virus_Free_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[17])
return "NULL";

char *buf = new char[7];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Virus_Free.c_str(), (unsigned long) min(3,m_Virus_Free.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::PC_Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[18])
return "NULL";

char *buf = new char[5];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_PC_Type.c_str(), (unsigned long) min(2,m_PC_Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Required_Version_Min_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[19])
return "NULL";

char *buf = new char[23];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Required_Version_Min.c_str(), (unsigned long) min(11,m_Required_Version_Min.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Required_Version_Max_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[20])
return "NULL";

char *buf = new char[23];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Required_Version_Max.c_str(), (unsigned long) min(11,m_Required_Version_Max.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::Installation_status_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[21])
return "NULL";

char *buf = new char[21];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Installation_status.c_str(), (unsigned long) min(10,m_Installation_status.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Software::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Software::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Software::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Software::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[26])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Software::Key::Key(long int in_PK_Software)
{
			pk_PK_Software = in_PK_Software;
	
}

Table_Software::Key::Key(Row_Software *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_PK_Software = pRow->m_PK_Software;
	
}		

bool Table_Software::Key_Less::operator()(const Table_Software::Key &key1, const Table_Software::Key &key2) const
{
			if (key1.pk_PK_Software!=key2.pk_PK_Software)
return key1.pk_PK_Software<key2.pk_PK_Software;
else
return false;	
}	

bool Table_Software::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Software *pRow = (Row_Software *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Software_asSQL()+", "+pRow->FK_Device_asSQL()+", "+pRow->RepositoryName_asSQL()+", "+pRow->Iconstr_asSQL()+", "+pRow->Title_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->HomeURL_asSQL()+", "+pRow->Category_asSQL()+", "+pRow->Downloadurl_asSQL()+", "+pRow->PackageName_asSQL()+", "+pRow->Misc_asSQL()+", "+pRow->Version_asSQL()+", "+pRow->Target_asSQL()+", "+pRow->Importance_asSQL()+", "+pRow->Sum_md5_asSQL()+", "+pRow->Sum_sha_asSQL()+", "+pRow->Rating_asSQL()+", "+pRow->Virus_Free_asSQL()+", "+pRow->PC_Type_asSQL()+", "+pRow->Required_Version_Min_asSQL()+", "+pRow->Required_Version_Max_asSQL()+", "+pRow->Installation_status_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Software (`PK_Software`, `FK_Device`, `RepositoryName`, `Iconstr`, `Title`, `Description`, `HomeURL`, `Category`, `Downloadurl`, `PackageName`, `Misc`, `Version`, `Target`, `Importance`, `Sum_md5`, `Sum_sha`, `Rating`, `Virus_Free`, `PC_Type`, `Required_Version_Min`, `Required_Version_Max`, `Installation_status`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			if( bDeleteFailedInsertRow )
			{
				addedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id = (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_Software=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Software);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Software* pRow = (Row_Software*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Software);

		char tmp_PK_Software[32];
sprintf(tmp_PK_Software, "%li", key.pk);


string condition;
condition = condition + "`PK_Software`=" + tmp_PK_Software;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Software`="+pRow->PK_Software_asSQL()+", `FK_Device`="+pRow->FK_Device_asSQL()+", `RepositoryName`="+pRow->RepositoryName_asSQL()+", `Iconstr`="+pRow->Iconstr_asSQL()+", `Title`="+pRow->Title_asSQL()+", `Description`="+pRow->Description_asSQL()+", `HomeURL`="+pRow->HomeURL_asSQL()+", `Category`="+pRow->Category_asSQL()+", `Downloadurl`="+pRow->Downloadurl_asSQL()+", `PackageName`="+pRow->PackageName_asSQL()+", `Misc`="+pRow->Misc_asSQL()+", `Version`="+pRow->Version_asSQL()+", `Target`="+pRow->Target_asSQL()+", `Importance`="+pRow->Importance_asSQL()+", `Sum_md5`="+pRow->Sum_md5_asSQL()+", `Sum_sha`="+pRow->Sum_sha_asSQL()+", `Rating`="+pRow->Rating_asSQL()+", `Virus_Free`="+pRow->Virus_Free_asSQL()+", `PC_Type`="+pRow->PC_Type_asSQL()+", `Required_Version_Min`="+pRow->Required_Version_Min_asSQL()+", `Required_Version_Max`="+pRow->Required_Version_Max_asSQL()+", `Installation_status`="+pRow->Installation_status_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Software set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			if( bDeleteFailedModifiedRow )
			{
				cachedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Software* pRow = (Row_Software*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Software* pRow = (Row_Software*) (*i).second;	

		char tmp_PK_Software[32];
sprintf(tmp_PK_Software, "%li", key.pk);


string condition;
condition = condition + "`PK_Software`=" + tmp_PK_Software;

	
		string query = "delete from Software where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Software*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Software::GetRows(string where_statement,vector<class Row_Software*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Software`.* from Software " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Software`.* from Software where " + where_statement;
	else
		query = "select `Software`.* from Software";
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
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

		Row_Software *pRow = new Row_Software(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Software = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Software));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Device));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_RepositoryName = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_RepositoryName = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Iconstr = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Iconstr = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Title = string(row[4],lengths[4]);
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
pRow->m_HomeURL = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_HomeURL = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Category = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Downloadurl = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Downloadurl = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_PackageName = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_PackageName = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Misc = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Misc = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Version = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Target = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Target = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Importance = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Importance = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Sum_md5 = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Sum_md5 = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Sum_sha = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Sum_sha = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Rating = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%f", &(pRow->m_Rating));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Virus_Free = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Virus_Free = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_PC_Type = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_PC_Type = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_Required_Version_Min = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_Required_Version_Min = string(row[19],lengths[19]);
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_Required_Version_Max = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_Required_Version_Max = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_Installation_status = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_Installation_status = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_id));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_batch));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_user));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%hi", &(pRow->m_psc_frozen));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[26]=false;
pRow->m_psc_mod = string(row[26],lengths[26]);
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Software);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Software *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Software* Table_Software::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_Software *pRow = new Row_Software(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Software* Table_Software::GetRow(long int in_PK_Software)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	SingleLongKey row_key(in_PK_Software);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Software*) (*i).second;
	//we have to fetch row
	Row_Software* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Software* Table_Software::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_PK_Software[32];
sprintf(tmp_PK_Software, "%li", key.pk);


string condition;
condition = condition + "`PK_Software`=" + tmp_PK_Software;


	string query = "select * from Software where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
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

	Row_Software *pRow = new Row_Software(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Software = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Software));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Device = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Device));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_RepositoryName = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_RepositoryName = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Iconstr = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Iconstr = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Title = string(row[4],lengths[4]);
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
pRow->m_HomeURL = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_HomeURL = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Category = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Downloadurl = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Downloadurl = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_PackageName = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_PackageName = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Misc = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Misc = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Version = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Target = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Target = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Importance = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Importance = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Sum_md5 = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Sum_md5 = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Sum_sha = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Sum_sha = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Rating = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%f", &(pRow->m_Rating));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Virus_Free = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Virus_Free = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_PC_Type = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_PC_Type = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_Required_Version_Min = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_Required_Version_Min = string(row[19],lengths[19]);
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_Required_Version_Max = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_Required_Version_Max = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_Installation_status = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_Installation_status = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_id));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_batch));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_user));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%hi", &(pRow->m_psc_frozen));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[26]=false;
pRow->m_psc_mod = string(row[26],lengths[26]);
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_restrict));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Device* Row_Software::FK_Device_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device);
}





