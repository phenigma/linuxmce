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
#include "Table_Orbiter.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_EntertainArea.h"
#include "Table_System.h"
#include "Table_Skin.h"
#include "Table_Size.h"
#include "Table_Version.h"
#include "Table_Language.h"

#include "Table_CachedScreens.h"
#include "Table_Device_Orbiter.h"
#include "Table_Orbiter_Users_PasswordReq.h"
#include "Table_Orbiter_Variable.h"


void Database_pluto_main::CreateTable_Orbiter()
{
	tblOrbiter = new Table_Orbiter(this);
}

void Database_pluto_main::DeleteTable_Orbiter()
{
	delete tblOrbiter;
}

Table_Orbiter::~Table_Orbiter()
{
	map<Table_Orbiter::Key, class Row_Orbiter*, Table_Orbiter::Key_Less>::iterator it;
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


void Row_Orbiter::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Orbiter*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Orbiter::Key key(this);					
			map<Table_Orbiter::Key, Row_Orbiter*, Table_Orbiter::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Orbiter::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Orbiter::Key key(this);		
		Row_Orbiter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Orbiter::Row_Orbiter(Table_Orbiter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Orbiter::SetDefaultValues()
{
	m_PK_Orbiter = 0;
is_null[0] = false;
m_FK_DesignObj_MainMenu = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_FK_System = 0;
is_null[5] = false;
m_FK_Skin = 0;
is_null[6] = false;
m_FK_Size = 0;
is_null[7] = false;
m_FK_Version = 0;
is_null[8] = false;
m_FK_Language = 0;
is_null[9] = false;
m_NoEffects = 0;
is_null[10] = false;
is_null[11] = true;
m_Modification_RecordInfo = "00000000000000";
is_null[12] = false;
m_IsNew_RecordInfo = 1;
is_null[13] = false;
m_IsDeleted_RecordInfo = 0;
is_null[14] = false;
is_null[15] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Orbiter::PK_Orbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Orbiter;}
long int Row_Orbiter::FK_DesignObj_MainMenu_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_MainMenu;}
long int Row_Orbiter::FK_DesignObj_Sleeping_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_Sleeping;}
long int Row_Orbiter::FK_DesignObj_ScreenSaver_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_ScreenSaver;}
long int Row_Orbiter::FK_EntertainArea_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_EntertainArea;}
long int Row_Orbiter::FK_System_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_System;}
long int Row_Orbiter::FK_Skin_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Skin;}
long int Row_Orbiter::FK_Size_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Size;}
long int Row_Orbiter::FK_Version_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Version;}
long int Row_Orbiter::FK_Language_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Language;}
short int Row_Orbiter::NoEffects_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_NoEffects;}
string Row_Orbiter::Modification_LastGen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_LastGen;}
string Row_Orbiter::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Orbiter::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Orbiter::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Orbiter::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Orbiter::PK_Orbiter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Orbiter = val; is_modified=true; is_null[0]=false;}
void Row_Orbiter::FK_DesignObj_MainMenu_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_MainMenu = val; is_modified=true; is_null[1]=false;}
void Row_Orbiter::FK_DesignObj_Sleeping_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_Sleeping = val; is_modified=true; is_null[2]=false;}
void Row_Orbiter::FK_DesignObj_ScreenSaver_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_ScreenSaver = val; is_modified=true; is_null[3]=false;}
void Row_Orbiter::FK_EntertainArea_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_EntertainArea = val; is_modified=true; is_null[4]=false;}
void Row_Orbiter::FK_System_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_System = val; is_modified=true; is_null[5]=false;}
void Row_Orbiter::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Skin = val; is_modified=true; is_null[6]=false;}
void Row_Orbiter::FK_Size_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Size = val; is_modified=true; is_null[7]=false;}
void Row_Orbiter::FK_Version_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Version = val; is_modified=true; is_null[8]=false;}
void Row_Orbiter::FK_Language_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Language = val; is_modified=true; is_null[9]=false;}
void Row_Orbiter::NoEffects_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_NoEffects = val; is_modified=true; is_null[10]=false;}
void Row_Orbiter::Modification_LastGen_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_LastGen = val; is_modified=true; is_null[11]=false;}
void Row_Orbiter::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[12]=false;}
void Row_Orbiter::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[13]=false;}
void Row_Orbiter::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[14]=false;}
void Row_Orbiter::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[15]=false;}

		
bool Row_Orbiter::FK_DesignObj_Sleeping_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Orbiter::FK_DesignObj_ScreenSaver_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Orbiter::FK_EntertainArea_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Orbiter::Modification_LastGen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_Orbiter::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_Orbiter::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_Orbiter::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}

			
void Row_Orbiter::FK_DesignObj_Sleeping_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Orbiter::FK_DesignObj_ScreenSaver_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Orbiter::FK_EntertainArea_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Orbiter::Modification_LastGen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_Orbiter::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
void Row_Orbiter::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_Orbiter::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[15]=val;}
	

string Row_Orbiter::PK_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Orbiter);

return buf;
}

string Row_Orbiter::FK_DesignObj_MainMenu_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_MainMenu);

return buf;
}

string Row_Orbiter::FK_DesignObj_Sleeping_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Sleeping);

return buf;
}

string Row_Orbiter::FK_DesignObj_ScreenSaver_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_ScreenSaver);

return buf;
}

string Row_Orbiter::FK_EntertainArea_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_EntertainArea);

return buf;
}

string Row_Orbiter::FK_System_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_System);

return buf;
}

string Row_Orbiter::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_Orbiter::FK_Size_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Size);

return buf;
}

string Row_Orbiter::FK_Version_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Version);

return buf;
}

string Row_Orbiter::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Language);

return buf;
}

string Row_Orbiter::NoEffects_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_NoEffects);

return buf;
}

string Row_Orbiter::Modification_LastGen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[39];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_LastGen.c_str(), (unsigned long) m_Modification_LastGen.size());
return string()+"\""+buf+"\"";
}

string Row_Orbiter::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Orbiter::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Orbiter::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Orbiter::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Orbiter::Key::Key(long int in_PK_Orbiter)
{
			pk_PK_Orbiter = in_PK_Orbiter;
	
}

Table_Orbiter::Key::Key(Row_Orbiter *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Orbiter = pRow->m_PK_Orbiter;
	
}		

bool Table_Orbiter::Key_Less::operator()(const Table_Orbiter::Key &key1, const Table_Orbiter::Key &key2) const
{
			if (key1.pk_PK_Orbiter!=key2.pk_PK_Orbiter)
return key1.pk_PK_Orbiter<key2.pk_PK_Orbiter;
else
return false;	
}	

void Table_Orbiter::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Orbiter*>::iterator i = addedRows.begin();
	
		Row_Orbiter *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Orbiter_asSQL()+", "+pRow->FK_DesignObj_MainMenu_asSQL()+", "+pRow->FK_DesignObj_Sleeping_asSQL()+", "+pRow->FK_DesignObj_ScreenSaver_asSQL()+", "+pRow->FK_EntertainArea_asSQL()+", "+pRow->FK_System_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->FK_Size_asSQL()+", "+pRow->FK_Version_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->NoEffects_asSQL()+", "+pRow->Modification_LastGen_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Orbiter (PK_Orbiter, FK_DesignObj_MainMenu, FK_DesignObj_Sleeping, FK_DesignObj_ScreenSaver, FK_EntertainArea, FK_System, FK_Skin, FK_Size, FK_Version, FK_Language, NoEffects, Modification_LastGen, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Orbiter=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Orbiter*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Orbiter* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk_PK_Orbiter);


string condition;
condition = condition + "PK_Orbiter=" + tmp_PK_Orbiter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Orbiter="+pRow->PK_Orbiter_asSQL()+", FK_DesignObj_MainMenu="+pRow->FK_DesignObj_MainMenu_asSQL()+", FK_DesignObj_Sleeping="+pRow->FK_DesignObj_Sleeping_asSQL()+", FK_DesignObj_ScreenSaver="+pRow->FK_DesignObj_ScreenSaver_asSQL()+", FK_EntertainArea="+pRow->FK_EntertainArea_asSQL()+", FK_System="+pRow->FK_System_asSQL()+", FK_Skin="+pRow->FK_Skin_asSQL()+", FK_Size="+pRow->FK_Size_asSQL()+", FK_Version="+pRow->FK_Version_asSQL()+", FK_Language="+pRow->FK_Language_asSQL()+", NoEffects="+pRow->NoEffects_asSQL()+", Modification_LastGen="+pRow->Modification_LastGen_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Orbiter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Orbiter*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Orbiter*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk_PK_Orbiter);


string condition;
condition = condition + "PK_Orbiter=" + tmp_PK_Orbiter;

	
		string query = "delete from Orbiter where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Orbiter::GetRows(string where_statement,vector<class Row_Orbiter*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Orbiter where " + where_statement;
		
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

		Row_Orbiter *pRow = new Row_Orbiter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObj_MainMenu = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObj_MainMenu));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DesignObj_Sleeping = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DesignObj_Sleeping));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_ScreenSaver = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj_ScreenSaver));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_EntertainArea = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_EntertainArea));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_System = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_System));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Skin));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Size = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Size));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Version));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Language));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_NoEffects = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_NoEffects));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Modification_LastGen = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Modification_LastGen = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Modification_RecordInfo = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Orbiter::Key, Row_Orbiter*, Table_Orbiter::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Orbiter* Table_Orbiter::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Orbiter *pRow = new Row_Orbiter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Orbiter* Table_Orbiter::GetRow(long int in_PK_Orbiter)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Orbiter);

	map<Key, Row_Orbiter*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Orbiter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Orbiter* Table_Orbiter::FetchRow(Table_Orbiter::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Orbiter[32];
sprintf(tmp_PK_Orbiter, "%li", key.pk_PK_Orbiter);


string condition;
condition = condition + "PK_Orbiter=" + tmp_PK_Orbiter;


	string query = "select * from Orbiter where " + condition;		

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

	Row_Orbiter *pRow = new Row_Orbiter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Orbiter = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Orbiter));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObj_MainMenu = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObj_MainMenu));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_DesignObj_Sleeping = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_DesignObj_Sleeping));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_ScreenSaver = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj_ScreenSaver));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_EntertainArea = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_EntertainArea));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_System = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_System));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Skin));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Size = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Size));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Version));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Language));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_NoEffects = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_NoEffects));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Modification_LastGen = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Modification_LastGen = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Modification_RecordInfo = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DesignObj* Row_Orbiter::FK_DesignObj_MainMenu_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_MainMenu);
}
class Row_DesignObj* Row_Orbiter::FK_DesignObj_Sleeping_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Sleeping);
}
class Row_DesignObj* Row_Orbiter::FK_DesignObj_ScreenSaver_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_ScreenSaver);
}
class Row_EntertainArea* Row_Orbiter::FK_EntertainArea_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EntertainArea *pTable = table->database->EntertainArea_get();
return pTable->GetRow(m_FK_EntertainArea);
}
class Row_System* Row_Orbiter::FK_System_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_System *pTable = table->database->System_get();
return pTable->GetRow(m_FK_System);
}
class Row_Skin* Row_Orbiter::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}
class Row_Size* Row_Orbiter::FK_Size_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Size *pTable = table->database->Size_get();
return pTable->GetRow(m_FK_Size);
}
class Row_Version* Row_Orbiter::FK_Version_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Version *pTable = table->database->Version_get();
return pTable->GetRow(m_FK_Version);
}
class Row_Language* Row_Orbiter::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}


void Row_Orbiter::CachedScreens_FK_Orbiter_getrows(vector <class Row_CachedScreens*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CachedScreens *pTable = table->database->CachedScreens_get();
pTable->GetRows("FK_Orbiter=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Device_Orbiter_FK_Orbiter_getrows(vector <class Row_Device_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_Orbiter *pTable = table->database->Device_Orbiter_get();
pTable->GetRows("FK_Orbiter=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Orbiter_Users_PasswordReq_FK_Orbiter_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter_Users_PasswordReq *pTable = table->database->Orbiter_Users_PasswordReq_get();
pTable->GetRows("FK_Orbiter=" + StringUtils::itos(m_PK_Orbiter),rows);
}
void Row_Orbiter::Orbiter_Variable_FK_Orbiter_getrows(vector <class Row_Orbiter_Variable*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter_Variable *pTable = table->database->Orbiter_Variable_get();
pTable->GetRows("FK_Orbiter=" + StringUtils::itos(m_PK_Orbiter),rows);
}



