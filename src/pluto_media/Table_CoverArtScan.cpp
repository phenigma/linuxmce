/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <WinSock2.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_CoverArtScan.h"
#include "Table_File.h"
#include "Table_Disc.h"
#include "Table_Attribute.h"

#include "Table_CoverArtScanEntry.h"


void Database_pluto_media::CreateTable_CoverArtScan()
{
	tblCoverArtScan = new Table_CoverArtScan(this);
}

void Database_pluto_media::DeleteTable_CoverArtScan()
{
	if( tblCoverArtScan )
		delete tblCoverArtScan;
}

bool Database_pluto_media::Commit_CoverArtScan(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblCoverArtScan->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_CoverArtScan::~Table_CoverArtScan()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CoverArtScan *pRow = (Row_CoverArtScan *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CoverArtScan *pRow = (Row_CoverArtScan *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CoverArtScan::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_CoverArtScan *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CoverArtScan *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_CoverArtScan);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_CoverArtScan::Reload()
{
	Row_CoverArtScan *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_CoverArtScan);
		Row_CoverArtScan *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CoverArtScan::Row_CoverArtScan(Table_CoverArtScan *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CoverArtScan::SetDefaultValues()
{
	m_PK_CoverArtScan = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_File = 0;
is_null[2] = true;
m_FK_Disc = 0;
is_null[3] = true;
m_FK_Attribute = 0;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
m_Scanned = 0;
is_null[11] = false;
is_null[12] = true;
m_psc_id = 0;
is_null[13] = true;
m_psc_batch = 0;
is_null[14] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[15] = false;
is_null[16] = true;
is_null[17] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CoverArtScan::PK_CoverArtScan_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_CoverArtScan;}
long int Row_CoverArtScan::FK_File_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_File;}
long int Row_CoverArtScan::FK_Disc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Disc;}
long int Row_CoverArtScan::FK_Attribute_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Attribute;}
string Row_CoverArtScan::Engine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Engine;}
string Row_CoverArtScan::Keyword1Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword1Type;}
string Row_CoverArtScan::Keyword1Search_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword1Search;}
string Row_CoverArtScan::Keyword2Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword2Type;}
string Row_CoverArtScan::Keyword2Search_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword2Search;}
string Row_CoverArtScan::Keyword3Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword3Type;}
string Row_CoverArtScan::Keyword3Search_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Keyword3Search;}
short int Row_CoverArtScan::Scanned_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Scanned;}
long int Row_CoverArtScan::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_CoverArtScan::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_CoverArtScan::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_CoverArtScan::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_CoverArtScan::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_CoverArtScan::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_CoverArtScan::PK_CoverArtScan_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_CoverArtScan = val; is_modified=true; is_null[0]=false;}
void Row_CoverArtScan::FK_File_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_File = val; is_modified=true; is_null[1]=false;}
void Row_CoverArtScan::FK_Disc_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Disc = val; is_modified=true; is_null[2]=false;}
void Row_CoverArtScan::FK_Attribute_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Attribute = val; is_modified=true; is_null[3]=false;}
void Row_CoverArtScan::Engine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Engine = val; is_modified=true; is_null[4]=false;}
void Row_CoverArtScan::Keyword1Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword1Type = val; is_modified=true; is_null[5]=false;}
void Row_CoverArtScan::Keyword1Search_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword1Search = val; is_modified=true; is_null[6]=false;}
void Row_CoverArtScan::Keyword2Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword2Type = val; is_modified=true; is_null[7]=false;}
void Row_CoverArtScan::Keyword2Search_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword2Search = val; is_modified=true; is_null[8]=false;}
void Row_CoverArtScan::Keyword3Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword3Type = val; is_modified=true; is_null[9]=false;}
void Row_CoverArtScan::Keyword3Search_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Keyword3Search = val; is_modified=true; is_null[10]=false;}
void Row_CoverArtScan::Scanned_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Scanned = val; is_modified=true; is_null[11]=false;}
void Row_CoverArtScan::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[12]=false;}
void Row_CoverArtScan::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[13]=false;}
void Row_CoverArtScan::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[14]=false;}
void Row_CoverArtScan::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[15]=false;}
void Row_CoverArtScan::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[16]=false;}
void Row_CoverArtScan::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[17]=false;}

		
bool Row_CoverArtScan::FK_File_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_CoverArtScan::FK_Disc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_CoverArtScan::FK_Attribute_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_CoverArtScan::Engine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_CoverArtScan::Keyword1Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_CoverArtScan::Keyword1Search_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_CoverArtScan::Keyword2Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_CoverArtScan::Keyword2Search_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_CoverArtScan::Keyword3Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_CoverArtScan::Keyword3Search_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_CoverArtScan::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_CoverArtScan::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_CoverArtScan::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_CoverArtScan::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_CoverArtScan::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_CoverArtScan::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}

			
void Row_CoverArtScan::FK_File_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_CoverArtScan::FK_Disc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_CoverArtScan::FK_Attribute_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_CoverArtScan::Engine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword1Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword1Search_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword2Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword2Search_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword3Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_CoverArtScan::Keyword3Search_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_CoverArtScan::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
	

string Row_CoverArtScan::PK_CoverArtScan_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CoverArtScan);

return buf;
}

string Row_CoverArtScan::FK_File_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_File);

return buf;
}

string Row_CoverArtScan::FK_Disc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Disc);

return buf;
}

string Row_CoverArtScan::FK_Attribute_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Attribute);

return buf;
}

string Row_CoverArtScan::Engine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Engine.c_str(), (unsigned long) min((size_t)20,m_Engine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword1Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword1Type.c_str(), (unsigned long) min((size_t)20,m_Keyword1Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword1Search_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword1Search.c_str(), (unsigned long) min((size_t)100,m_Keyword1Search.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword2Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword2Type.c_str(), (unsigned long) min((size_t)20,m_Keyword2Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword2Search_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword2Search.c_str(), (unsigned long) min((size_t)100,m_Keyword2Search.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword3Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword3Type.c_str(), (unsigned long) min((size_t)20,m_Keyword3Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Keyword3Search_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Keyword3Search.c_str(), (unsigned long) min((size_t)100,m_Keyword3Search.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::Scanned_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Scanned);

return buf;
}

string Row_CoverArtScan::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_CoverArtScan::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_CoverArtScan::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_CoverArtScan::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_CoverArtScan::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CoverArtScan::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_CoverArtScan::Key::Key(long int in_PK_CoverArtScan)
{
			pk_PK_CoverArtScan = in_PK_CoverArtScan;
	
}

Table_CoverArtScan::Key::Key(Row_CoverArtScan *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_CoverArtScan = pRow->m_PK_CoverArtScan;
	
}		

bool Table_CoverArtScan::Key_Less::operator()(const Table_CoverArtScan::Key &key1, const Table_CoverArtScan::Key &key2) const
{
			if (key1.pk_PK_CoverArtScan!=key2.pk_PK_CoverArtScan)
return key1.pk_PK_CoverArtScan<key2.pk_PK_CoverArtScan;
else
return false;	
}	

bool Table_CoverArtScan::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CoverArtScan *pRow = (Row_CoverArtScan *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CoverArtScan_asSQL()+", "+pRow->FK_File_asSQL()+", "+pRow->FK_Disc_asSQL()+", "+pRow->FK_Attribute_asSQL()+", "+pRow->Engine_asSQL()+", "+pRow->Keyword1Type_asSQL()+", "+pRow->Keyword1Search_asSQL()+", "+pRow->Keyword2Type_asSQL()+", "+pRow->Keyword2Search_asSQL()+", "+pRow->Keyword3Type_asSQL()+", "+pRow->Keyword3Search_asSQL()+", "+pRow->Scanned_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into CoverArtScan (`PK_CoverArtScan`, `FK_File`, `FK_Disc`, `FK_Attribute`, `Engine`, `Keyword1Type`, `Keyword1Search`, `Keyword2Type`, `Keyword2Search`, `Keyword3Type`, `Keyword3Search`, `Scanned`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedInsertRow )
				{
					addedRows.erase(i);
					delete pRow;
				}
				break;   // Go ahead and continue to do the updates
			}
		}
	
		if (db_wrapper_affected_rows(database->m_pDB)!=0)
		{
			
			
			long int id = (long int) db_wrapper_insert_id(database->m_pDB);
		
			if (id!=0)
		pRow->m_PK_CoverArtScan=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_CoverArtScan is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_CoverArtScan);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CoverArtScan* pRow = (Row_CoverArtScan*) (*i).second;	
		SingleLongKey key(pRow->m_PK_CoverArtScan);

		char tmp_PK_CoverArtScan[32];
sprintf(tmp_PK_CoverArtScan, "%li", key.pk);


string condition;
condition = condition + "`PK_CoverArtScan`=" + tmp_PK_CoverArtScan;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_CoverArtScan`="+pRow->PK_CoverArtScan_asSQL()+", `FK_File`="+pRow->FK_File_asSQL()+", `FK_Disc`="+pRow->FK_Disc_asSQL()+", `FK_Attribute`="+pRow->FK_Attribute_asSQL()+", `Engine`="+pRow->Engine_asSQL()+", `Keyword1Type`="+pRow->Keyword1Type_asSQL()+", `Keyword1Search`="+pRow->Keyword1Search_asSQL()+", `Keyword2Type`="+pRow->Keyword2Type_asSQL()+", `Keyword2Search`="+pRow->Keyword2Search_asSQL()+", `Keyword3Type`="+pRow->Keyword3Type_asSQL()+", `Keyword3Search`="+pRow->Keyword3Search_asSQL()+", `Scanned`="+pRow->Scanned_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update CoverArtScan set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedModifiedRow )
				{
					cachedRows.erase(i);
					delete pRow;
				}
				break;  // Go ahead and do the deletes
			}
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_CoverArtScan* pRow = (Row_CoverArtScan*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_CoverArtScan* pRow = (Row_CoverArtScan*) (*i).second;	

		char tmp_PK_CoverArtScan[32];
sprintf(tmp_PK_CoverArtScan, "%li", key.pk);


string condition;
condition = condition + "`PK_CoverArtScan`=" + tmp_PK_CoverArtScan;

	
		string query = "delete from CoverArtScan where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_CoverArtScan*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_CoverArtScan::GetRows(string where_statement,vector<class Row_CoverArtScan*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `CoverArtScan`.* from CoverArtScan " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `CoverArtScan`.* from CoverArtScan where " + where_statement;
	else
		query = "select `CoverArtScan`.* from CoverArtScan";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_CoverArtScan *pRow = new Row_CoverArtScan(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CoverArtScan = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CoverArtScan));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_File));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Disc = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Disc));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Attribute = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Attribute));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Engine = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Engine = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Keyword1Type = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Keyword1Type = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Keyword1Search = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Keyword1Search = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Keyword2Type = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Keyword2Type = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Keyword2Search = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Keyword2Search = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Keyword3Type = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Keyword3Type = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Keyword3Search = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Keyword3Search = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Scanned = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_Scanned));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_id));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_batch));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_user));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_psc_frozen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_psc_mod = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_CoverArtScan);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CoverArtScan *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_CoverArtScan* Table_CoverArtScan::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_CoverArtScan *pRow = new Row_CoverArtScan(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CoverArtScan* Table_CoverArtScan::GetRow(long int in_PK_CoverArtScan)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_CoverArtScan);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CoverArtScan*) (*i).second;
	//we have to fetch row
	Row_CoverArtScan* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CoverArtScan* Table_CoverArtScan::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_CoverArtScan[32];
sprintf(tmp_PK_CoverArtScan, "%li", key.pk);


string condition;
condition = condition + "`PK_CoverArtScan`=" + tmp_PK_CoverArtScan;


	string query = "select * from CoverArtScan where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CoverArtScan::FetchRow db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return NULL;
	}	
	
	DB_ROW row = db_wrapper_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		db_wrapper_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = db_wrapper_fetch_lengths(res);

	Row_CoverArtScan *pRow = new Row_CoverArtScan(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CoverArtScan = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CoverArtScan));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_File));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Disc = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Disc));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Attribute = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Attribute));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Engine = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Engine = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Keyword1Type = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Keyword1Type = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Keyword1Search = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Keyword1Search = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Keyword2Type = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Keyword2Type = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Keyword2Search = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Keyword2Search = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Keyword3Type = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Keyword3Type = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Keyword3Search = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Keyword3Search = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Scanned = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_Scanned));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_id));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_batch));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_user));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_psc_frozen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_psc_mod = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_File* Row_CoverArtScan::FK_File_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_File *pTable = table->database->File_get();
return pTable->GetRow(m_FK_File);
}
class Row_Disc* Row_CoverArtScan::FK_Disc_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Disc *pTable = table->database->Disc_get();
return pTable->GetRow(m_FK_Disc);
}
class Row_Attribute* Row_CoverArtScan::FK_Attribute_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Attribute *pTable = table->database->Attribute_get();
return pTable->GetRow(m_FK_Attribute);
}


void Row_CoverArtScan::CoverArtScanEntry_FK_CoverArtScan_getrows(vector <class Row_CoverArtScanEntry*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CoverArtScanEntry *pTable = table->database->CoverArtScanEntry_get();
pTable->GetRows("`FK_CoverArtScan`=" + StringUtils::itos(m_PK_CoverArtScan),rows);
}



