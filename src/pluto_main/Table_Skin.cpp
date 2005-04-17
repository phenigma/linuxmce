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
#include "Table_Skin.h"
#include "Table_Criteria_D.h"
#include "Table_Style.h"
#include "Table_Skin.h"
#include "Table_StabilityStatus.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"

#include "Table_DesignObjVariation_Text_Skin_Language.h"
#include "Table_Skin.h"
#include "Table_StyleVariation.h"


void Database_pluto_main::CreateTable_Skin()
{
	tblSkin = new Table_Skin(this);
}

void Database_pluto_main::DeleteTable_Skin()
{
	if( tblSkin )
		delete tblSkin;
}

Table_Skin::~Table_Skin()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Skin *pRow = (Row_Skin *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Skin *pRow = (Row_Skin *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Skin::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Skin *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Skin);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Skin::Reload()
{
	Row_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Skin);
		Row_Skin *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Skin::Row_Skin(Table_Skin *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Skin::SetDefaultValues()
{
	m_PK_Skin = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_MergeStandardVariation = 1;
is_null[3] = false;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
m_DrawTextBeforeChildren = 0;
is_null[7] = false;
m_FK_StabilityStatus = 1;
is_null[8] = false;
m_FK_DesignObj_MainMenu = 1255;
is_null[9] = false;
m_FK_DesignObj_Sleeping = 1778;
is_null[10] = false;
m_FK_DesignObj_ScreenSaver = 3319;
is_null[11] = false;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
m_psc_frozen = 0;
is_null[15] = false;
m_psc_mod = "00000000000000";
is_null[16] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Skin::PK_Skin_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Skin;}
string Row_Skin::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Skin::FK_Criteria_D_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Criteria_D;}
short int Row_Skin::MergeStandardVariation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_MergeStandardVariation;}
string Row_Skin::DataSubdirectory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DataSubdirectory;}
long int Row_Skin::FK_Style_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Style;}
long int Row_Skin::FK_Skin_TextPlacement_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Skin_TextPlacement;}
short int Row_Skin::DrawTextBeforeChildren_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DrawTextBeforeChildren;}
long int Row_Skin::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_StabilityStatus;}
long int Row_Skin::FK_DesignObj_MainMenu_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_MainMenu;}
long int Row_Skin::FK_DesignObj_Sleeping_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_Sleeping;}
long int Row_Skin::FK_DesignObj_ScreenSaver_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_ScreenSaver;}
long int Row_Skin::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Skin::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Skin::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Skin::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Skin::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Skin::PK_Skin_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Skin = val; is_modified=true; is_null[0]=false;}
void Row_Skin::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Skin::FK_Criteria_D_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Criteria_D = val; is_modified=true; is_null[2]=false;}
void Row_Skin::MergeStandardVariation_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_MergeStandardVariation = val; is_modified=true; is_null[3]=false;}
void Row_Skin::DataSubdirectory_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DataSubdirectory = val; is_modified=true; is_null[4]=false;}
void Row_Skin::FK_Style_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Style = val; is_modified=true; is_null[5]=false;}
void Row_Skin::FK_Skin_TextPlacement_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Skin_TextPlacement = val; is_modified=true; is_null[6]=false;}
void Row_Skin::DrawTextBeforeChildren_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DrawTextBeforeChildren = val; is_modified=true; is_null[7]=false;}
void Row_Skin::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[8]=false;}
void Row_Skin::FK_DesignObj_MainMenu_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_MainMenu = val; is_modified=true; is_null[9]=false;}
void Row_Skin::FK_DesignObj_Sleeping_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_Sleeping = val; is_modified=true; is_null[10]=false;}
void Row_Skin::FK_DesignObj_ScreenSaver_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_ScreenSaver = val; is_modified=true; is_null[11]=false;}
void Row_Skin::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[12]=false;}
void Row_Skin::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[13]=false;}
void Row_Skin::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[14]=false;}
void Row_Skin::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[15]=false;}
void Row_Skin::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[16]=false;}

		
bool Row_Skin::FK_Criteria_D_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Skin::DataSubdirectory_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Skin::FK_Style_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Skin::FK_Skin_TextPlacement_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Skin::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_Skin::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_Skin::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_Skin::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}

			
void Row_Skin::FK_Criteria_D_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[2]=val;
is_modified=true;
}
void Row_Skin::DataSubdirectory_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[4]=val;
is_modified=true;
}
void Row_Skin::FK_Style_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[5]=val;
is_modified=true;
}
void Row_Skin::FK_Skin_TextPlacement_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[6]=val;
is_modified=true;
}
void Row_Skin::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[12]=val;
is_modified=true;
}
void Row_Skin::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[13]=val;
is_modified=true;
}
void Row_Skin::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[14]=val;
is_modified=true;
}
void Row_Skin::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);
is_null[15]=val;
is_modified=true;
}
	

string Row_Skin::PK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Skin);

return buf;
}

string Row_Skin::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) min(30,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Skin::FK_Criteria_D_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Criteria_D);

return buf;
}

string Row_Skin::MergeStandardVariation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MergeStandardVariation);

return buf;
}

string Row_Skin::DataSubdirectory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_DataSubdirectory.c_str(), (unsigned long) min(30,m_DataSubdirectory.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Skin::FK_Style_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style);

return buf;
}

string Row_Skin::FK_Skin_TextPlacement_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin_TextPlacement);

return buf;
}

string Row_Skin::DrawTextBeforeChildren_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DrawTextBeforeChildren);

return buf;
}

string Row_Skin::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_Skin::FK_DesignObj_MainMenu_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_MainMenu);

return buf;
}

string Row_Skin::FK_DesignObj_Sleeping_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Sleeping);

return buf;
}

string Row_Skin::FK_DesignObj_ScreenSaver_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_ScreenSaver);

return buf;
}

string Row_Skin::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Skin::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Skin::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Skin::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Skin::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Skin::Key::Key(long int in_PK_Skin)
{
			pk_PK_Skin = in_PK_Skin;
	
}

Table_Skin::Key::Key(Row_Skin *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Skin = pRow->m_PK_Skin;
	
}		

bool Table_Skin::Key_Less::operator()(const Table_Skin::Key &key1, const Table_Skin::Key &key2) const
{
			if (key1.pk_PK_Skin!=key2.pk_PK_Skin)
return key1.pk_PK_Skin<key2.pk_PK_Skin;
else
return false;	
}	

bool Table_Skin::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Skin *pRow = (Row_Skin *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Skin_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Criteria_D_asSQL()+", "+pRow->MergeStandardVariation_asSQL()+", "+pRow->DataSubdirectory_asSQL()+", "+pRow->FK_Style_asSQL()+", "+pRow->FK_Skin_TextPlacement_asSQL()+", "+pRow->DrawTextBeforeChildren_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->FK_DesignObj_MainMenu_asSQL()+", "+pRow->FK_DesignObj_Sleeping_asSQL()+", "+pRow->FK_DesignObj_ScreenSaver_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Skin (`PK_Skin`, `Description`, `FK_Criteria_D`, `MergeStandardVariation`, `DataSubdirectory`, `FK_Style`, `FK_Skin_TextPlacement`, `DrawTextBeforeChildren`, `FK_StabilityStatus`, `FK_DesignObj_MainMenu`, `FK_DesignObj_Sleeping`, `FK_DesignObj_ScreenSaver`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_Skin=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Skin);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Skin* pRow = (Row_Skin*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Skin);

		char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Skin`="+pRow->PK_Skin_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_Criteria_D`="+pRow->FK_Criteria_D_asSQL()+", `MergeStandardVariation`="+pRow->MergeStandardVariation_asSQL()+", `DataSubdirectory`="+pRow->DataSubdirectory_asSQL()+", `FK_Style`="+pRow->FK_Style_asSQL()+", `FK_Skin_TextPlacement`="+pRow->FK_Skin_TextPlacement_asSQL()+", `DrawTextBeforeChildren`="+pRow->DrawTextBeforeChildren_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `FK_DesignObj_MainMenu`="+pRow->FK_DesignObj_MainMenu_asSQL()+", `FK_DesignObj_Sleeping`="+pRow->FK_DesignObj_Sleeping_asSQL()+", `FK_DesignObj_ScreenSaver`="+pRow->FK_DesignObj_ScreenSaver_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Skin set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Skin* pRow = (Row_Skin*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Skin* pRow = (Row_Skin*) (*i).second;	

		char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;

	
		string query = "delete from Skin where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Skin*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Skin::GetRows(string where_statement,vector<class Row_Skin*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Skin`.* from Skin " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Skin`.* from Skin where " + where_statement;
		
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

		Row_Skin *pRow = new Row_Skin(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Skin = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Skin));
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
pRow->m_FK_Criteria_D = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Criteria_D));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_MergeStandardVariation = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_MergeStandardVariation));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DataSubdirectory = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_DataSubdirectory = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Style));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Skin_TextPlacement = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Skin_TextPlacement));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_DrawTextBeforeChildren = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_DrawTextBeforeChildren));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj_MainMenu = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj_MainMenu));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DesignObj_Sleeping = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DesignObj_Sleeping));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_DesignObj_ScreenSaver = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_DesignObj_ScreenSaver));
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



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Skin);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Skin *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Skin* Table_Skin::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Skin *pRow = new Row_Skin(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Skin* Table_Skin::GetRow(long int in_PK_Skin)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Skin);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Skin*) (*i).second;
	//we have to fetch row
	Row_Skin* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Skin* Table_Skin::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;


	string query = "select * from Skin where " + condition;		

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

	Row_Skin *pRow = new Row_Skin(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Skin = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Skin));
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
pRow->m_FK_Criteria_D = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Criteria_D));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_MergeStandardVariation = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_MergeStandardVariation));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DataSubdirectory = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_DataSubdirectory = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Style));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Skin_TextPlacement = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Skin_TextPlacement));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_DrawTextBeforeChildren = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_DrawTextBeforeChildren));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj_MainMenu = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj_MainMenu));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DesignObj_Sleeping = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DesignObj_Sleeping));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_DesignObj_ScreenSaver = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_DesignObj_ScreenSaver));
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



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Criteria_D* Row_Skin::FK_Criteria_D_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Criteria_D *pTable = table->database->Criteria_D_get();
return pTable->GetRow(m_FK_Criteria_D);
}
class Row_Style* Row_Skin::FK_Style_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style);
}
class Row_Skin* Row_Skin::FK_Skin_TextPlacement_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin_TextPlacement);
}
class Row_StabilityStatus* Row_Skin::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_DesignObj* Row_Skin::FK_DesignObj_MainMenu_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_MainMenu);
}
class Row_DesignObj* Row_Skin::FK_DesignObj_Sleeping_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Sleeping);
}
class Row_DesignObj* Row_Skin::FK_DesignObj_ScreenSaver_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_ScreenSaver);
}


void Row_Skin::DesignObjVariation_Text_Skin_Language_FK_Skin_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation_Text_Skin_Language *pTable = table->database->DesignObjVariation_Text_Skin_Language_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::Skin_FK_Skin_TextPlacement_getrows(vector <class Row_Skin*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Skin *pTable = table->database->Skin_get();
pTable->GetRows("`FK_Skin_TextPlacement`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::StyleVariation_FK_Skin_getrows(vector <class Row_StyleVariation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StyleVariation *pTable = table->database->StyleVariation_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}



