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
#include "Table_CommandGroup.h"
#include "Table_Array.h"
#include "Table_Installation.h"
#include "Table_Criteria.h"
#include "Table_DesignObj.h"
#include "Table_Template.h"
#include "Table_Icon.h"

#include "Table_CommandGroup_Command.h"
#include "Table_CommandGroup_EntGroup.h"
#include "Table_CommandGroup_Room.h"
#include "Table_Device_CommandGroup.h"
#include "Table_EventHandler.h"


void Database_pluto_main::CreateTable_CommandGroup()
{
	tblCommandGroup = new Table_CommandGroup(this);
}

void Database_pluto_main::DeleteTable_CommandGroup()
{
	delete tblCommandGroup;
}

Table_CommandGroup::~Table_CommandGroup()
{
	map<Table_CommandGroup::Key, class Row_CommandGroup*, Table_CommandGroup::Key_Less>::iterator it;
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


void Row_CommandGroup::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_CommandGroup*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_CommandGroup::Key key(this);					
			map<Table_CommandGroup::Key, Row_CommandGroup*, Table_CommandGroup::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CommandGroup::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_CommandGroup::Key key(this);		
		Row_CommandGroup *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CommandGroup::Row_CommandGroup(Table_CommandGroup *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CommandGroup::SetDefaultValues()
{
	m_PK_CommandGroup = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_Description = "";
is_null[3] = false;
is_null[4] = true;
m_CanTurnOff = 0;
is_null[5] = false;
m_AlwaysShow = 0;
is_null[6] = false;
m_CanBeHidden = 0;
is_null[7] = false;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
is_null[11] = true;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
is_null[15] = true;
m_psc_frozen = 0;
is_null[16] = false;
m_psc_mod = "00000000000000";
is_null[17] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CommandGroup::PK_CommandGroup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_CommandGroup;}
long int Row_CommandGroup::FK_Array_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Array;}
long int Row_CommandGroup::FK_Installation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Installation;}
string Row_CommandGroup::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_CommandGroup::Hint_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Hint;}
short int Row_CommandGroup::CanTurnOff_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_CanTurnOff;}
short int Row_CommandGroup::AlwaysShow_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_AlwaysShow;}
short int Row_CommandGroup::CanBeHidden_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_CanBeHidden;}
long int Row_CommandGroup::FK_Criteria_Orbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Criteria_Orbiter;}
long int Row_CommandGroup::FK_DesignObj_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj;}
long int Row_CommandGroup::FK_Template_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Template;}
long int Row_CommandGroup::AltID_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_AltID;}
long int Row_CommandGroup::FK_Icon_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Icon;}
long int Row_CommandGroup::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_CommandGroup::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_CommandGroup::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_CommandGroup::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_CommandGroup::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_CommandGroup::PK_CommandGroup_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_CommandGroup = val; is_modified=true; is_null[0]=false;}
void Row_CommandGroup::FK_Array_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Array = val; is_modified=true; is_null[1]=false;}
void Row_CommandGroup::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Installation = val; is_modified=true; is_null[2]=false;}
void Row_CommandGroup::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_CommandGroup::Hint_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Hint = val; is_modified=true; is_null[4]=false;}
void Row_CommandGroup::CanTurnOff_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_CanTurnOff = val; is_modified=true; is_null[5]=false;}
void Row_CommandGroup::AlwaysShow_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_AlwaysShow = val; is_modified=true; is_null[6]=false;}
void Row_CommandGroup::CanBeHidden_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_CanBeHidden = val; is_modified=true; is_null[7]=false;}
void Row_CommandGroup::FK_Criteria_Orbiter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Criteria_Orbiter = val; is_modified=true; is_null[8]=false;}
void Row_CommandGroup::FK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj = val; is_modified=true; is_null[9]=false;}
void Row_CommandGroup::FK_Template_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Template = val; is_modified=true; is_null[10]=false;}
void Row_CommandGroup::AltID_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_AltID = val; is_modified=true; is_null[11]=false;}
void Row_CommandGroup::FK_Icon_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Icon = val; is_modified=true; is_null[12]=false;}
void Row_CommandGroup::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[13]=false;}
void Row_CommandGroup::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[14]=false;}
void Row_CommandGroup::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[15]=false;}
void Row_CommandGroup::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[16]=false;}
void Row_CommandGroup::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[17]=false;}

		
bool Row_CommandGroup::FK_Array_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_CommandGroup::FK_Installation_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_CommandGroup::Hint_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_CommandGroup::FK_Criteria_Orbiter_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_CommandGroup::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_CommandGroup::FK_Template_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_CommandGroup::AltID_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_CommandGroup::FK_Icon_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}
bool Row_CommandGroup::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[13];}
bool Row_CommandGroup::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[14];}
bool Row_CommandGroup::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[15];}
bool Row_CommandGroup::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[16];}

			
void Row_CommandGroup::FK_Array_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_CommandGroup::FK_Installation_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_CommandGroup::Hint_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_CommandGroup::FK_Criteria_Orbiter_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_CommandGroup::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_CommandGroup::FK_Template_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_CommandGroup::AltID_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_CommandGroup::FK_Icon_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
void Row_CommandGroup::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[13]=val;}
void Row_CommandGroup::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[14]=val;}
void Row_CommandGroup::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[15]=val;}
void Row_CommandGroup::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[16]=val;}
	

string Row_CommandGroup::PK_CommandGroup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_CommandGroup);

return buf;
}

string Row_CommandGroup::FK_Array_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Array);

return buf;
}

string Row_CommandGroup::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_CommandGroup::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_CommandGroup::Hint_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Hint.c_str(), (unsigned long) m_Hint.size());
return string()+"\""+buf+"\"";
}

string Row_CommandGroup::CanTurnOff_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CanTurnOff);

return buf;
}

string Row_CommandGroup::AlwaysShow_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AlwaysShow);

return buf;
}

string Row_CommandGroup::CanBeHidden_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CanBeHidden);

return buf;
}

string Row_CommandGroup::FK_Criteria_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Criteria_Orbiter);

return buf;
}

string Row_CommandGroup::FK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj);

return buf;
}

string Row_CommandGroup::FK_Template_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Template);

return buf;
}

string Row_CommandGroup::AltID_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_AltID);

return buf;
}

string Row_CommandGroup::FK_Icon_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Icon);

return buf;
}

string Row_CommandGroup::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_CommandGroup::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_CommandGroup::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_CommandGroup::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_CommandGroup::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[17])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_CommandGroup::Key::Key(long int in_PK_CommandGroup)
{
			pk_PK_CommandGroup = in_PK_CommandGroup;
	
}

Table_CommandGroup::Key::Key(Row_CommandGroup *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_CommandGroup = pRow->m_PK_CommandGroup;
	
}		

bool Table_CommandGroup::Key_Less::operator()(const Table_CommandGroup::Key &key1, const Table_CommandGroup::Key &key2) const
{
			if (key1.pk_PK_CommandGroup!=key2.pk_PK_CommandGroup)
return key1.pk_PK_CommandGroup<key2.pk_PK_CommandGroup;
else
return false;	
}	

void Table_CommandGroup::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_CommandGroup*>::iterator i = addedRows.begin();
	
		Row_CommandGroup *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_CommandGroup_asSQL()+", "+pRow->FK_Array_asSQL()+", "+pRow->FK_Installation_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Hint_asSQL()+", "+pRow->CanTurnOff_asSQL()+", "+pRow->AlwaysShow_asSQL()+", "+pRow->CanBeHidden_asSQL()+", "+pRow->FK_Criteria_Orbiter_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->FK_Template_asSQL()+", "+pRow->AltID_asSQL()+", "+pRow->FK_Icon_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into CommandGroup (PK_CommandGroup, FK_Array, FK_Installation, Description, Hint, CanTurnOff, AlwaysShow, CanBeHidden, FK_Criteria_Orbiter, FK_DesignObj, FK_Template, AltID, FK_Icon, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_CommandGroup=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_CommandGroup*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_CommandGroup* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_CommandGroup[32];
sprintf(tmp_PK_CommandGroup, "%li", key.pk_PK_CommandGroup);


string condition;
condition = condition + "PK_CommandGroup=" + tmp_PK_CommandGroup;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_CommandGroup="+pRow->PK_CommandGroup_asSQL()+", FK_Array="+pRow->FK_Array_asSQL()+", FK_Installation="+pRow->FK_Installation_asSQL()+", Description="+pRow->Description_asSQL()+", Hint="+pRow->Hint_asSQL()+", CanTurnOff="+pRow->CanTurnOff_asSQL()+", AlwaysShow="+pRow->AlwaysShow_asSQL()+", CanBeHidden="+pRow->CanBeHidden_asSQL()+", FK_Criteria_Orbiter="+pRow->FK_Criteria_Orbiter_asSQL()+", FK_DesignObj="+pRow->FK_DesignObj_asSQL()+", FK_Template="+pRow->FK_Template_asSQL()+", AltID="+pRow->AltID_asSQL()+", FK_Icon="+pRow->FK_Icon_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update CommandGroup set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_CommandGroup*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_CommandGroup*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_CommandGroup[32];
sprintf(tmp_PK_CommandGroup, "%li", key.pk_PK_CommandGroup);


string condition;
condition = condition + "PK_CommandGroup=" + tmp_PK_CommandGroup;

	
		string query = "delete from CommandGroup where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_CommandGroup::GetRows(string where_statement,vector<class Row_CommandGroup*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from CommandGroup " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from CommandGroup where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
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

		Row_CommandGroup *pRow = new Row_CommandGroup(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CommandGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CommandGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Array = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Array));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Installation));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Hint = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Hint = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_CanTurnOff = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_CanTurnOff));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_AlwaysShow = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_AlwaysShow));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_CanBeHidden = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_CanBeHidden));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Criteria_Orbiter = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Criteria_Orbiter));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Template = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Template));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_AltID = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_AltID));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Icon));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_id));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_batch));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_user));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_psc_frozen));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_psc_mod = string(row[17],lengths[17]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_CommandGroup::Key, Row_CommandGroup*, Table_CommandGroup::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_CommandGroup* Table_CommandGroup::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_CommandGroup *pRow = new Row_CommandGroup(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CommandGroup* Table_CommandGroup::GetRow(long int in_PK_CommandGroup)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_CommandGroup);

	map<Key, Row_CommandGroup*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_CommandGroup* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CommandGroup* Table_CommandGroup::FetchRow(Table_CommandGroup::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_CommandGroup[32];
sprintf(tmp_PK_CommandGroup, "%li", key.pk_PK_CommandGroup);


string condition;
condition = condition + "PK_CommandGroup=" + tmp_PK_CommandGroup;


	string query = "select * from CommandGroup where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
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

	Row_CommandGroup *pRow = new Row_CommandGroup(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_CommandGroup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_CommandGroup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Array = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Array));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Installation));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Hint = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Hint = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_CanTurnOff = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_CanTurnOff));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_AlwaysShow = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_AlwaysShow));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_CanBeHidden = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_CanBeHidden));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Criteria_Orbiter = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Criteria_Orbiter));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Template = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Template));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_AltID = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_AltID));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Icon));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_id));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_batch));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_user));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_psc_frozen));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_psc_mod = string(row[17],lengths[17]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Array* Row_CommandGroup::FK_Array_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Array *pTable = table->database->Array_get();
return pTable->GetRow(m_FK_Array);
}
class Row_Installation* Row_CommandGroup::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}
class Row_Criteria* Row_CommandGroup::FK_Criteria_Orbiter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Criteria *pTable = table->database->Criteria_get();
return pTable->GetRow(m_FK_Criteria_Orbiter);
}
class Row_DesignObj* Row_CommandGroup::FK_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj);
}
class Row_Template* Row_CommandGroup::FK_Template_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Template *pTable = table->database->Template_get();
return pTable->GetRow(m_FK_Template);
}
class Row_Icon* Row_CommandGroup::FK_Icon_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Icon *pTable = table->database->Icon_get();
return pTable->GetRow(m_FK_Icon);
}


void Row_CommandGroup::CommandGroup_Command_FK_CommandGroup_getrows(vector <class Row_CommandGroup_Command*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_Command *pTable = table->database->CommandGroup_Command_get();
pTable->GetRows("FK_CommandGroup=" + StringUtils::itos(m_PK_CommandGroup),rows);
}
void Row_CommandGroup::CommandGroup_EntGroup_FK_CommandGroup_getrows(vector <class Row_CommandGroup_EntGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_EntGroup *pTable = table->database->CommandGroup_EntGroup_get();
pTable->GetRows("FK_CommandGroup=" + StringUtils::itos(m_PK_CommandGroup),rows);
}
void Row_CommandGroup::CommandGroup_Room_FK_CommandGroup_getrows(vector <class Row_CommandGroup_Room*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup_Room *pTable = table->database->CommandGroup_Room_get();
pTable->GetRows("FK_CommandGroup=" + StringUtils::itos(m_PK_CommandGroup),rows);
}
void Row_CommandGroup::Device_CommandGroup_FK_CommandGroup_getrows(vector <class Row_Device_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Device_CommandGroup *pTable = table->database->Device_CommandGroup_get();
pTable->GetRows("FK_CommandGroup=" + StringUtils::itos(m_PK_CommandGroup),rows);
}
void Row_CommandGroup::EventHandler_FK_CommandGroup_getrows(vector <class Row_EventHandler*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_EventHandler *pTable = table->database->EventHandler_get();
pTable->GetRows("FK_CommandGroup=" + StringUtils::itos(m_PK_CommandGroup),rows);
}



