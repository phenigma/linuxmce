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
#include "Table_Icon.h"

#include "Table_CommandGroup.h"
#include "Table_Floorplan.h"
#include "Table_Room.h"


void Database_pluto_main::CreateTable_Icon()
{
	tblIcon = new Table_Icon(this);
}

void Database_pluto_main::DeleteTable_Icon()
{
	if( tblIcon )
		delete tblIcon;
}

Table_Icon::~Table_Icon()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Icon *pRow = (Row_Icon *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Icon *pRow = (Row_Icon *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Icon::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Icon *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Icon *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Icon);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Icon::Reload()
{
	Row_Icon *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Icon);
		Row_Icon *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Icon::Row_Icon(Table_Icon *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Icon::SetDefaultValues()
{
	m_PK_Icon = 0;
is_null[0] = false;
m_Define = "";
is_null[1] = false;
m_Description = "";
is_null[2] = false;
is_null[3] = true;
m_MainFileName = "";
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
m_psc_frozen = 0;
is_null[11] = false;
m_psc_mod = "00000000000000";
is_null[12] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Icon::PK_Icon_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Icon;}
string Row_Icon::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_Icon::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Icon::TransparentColor_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_TransparentColor;}
string Row_Icon::MainFileName_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_MainFileName;}
string Row_Icon::SelectedFileName_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SelectedFileName;}
string Row_Icon::AltFileNames_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_AltFileNames;}
string Row_Icon::BackgroundFileName_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_BackgroundFileName;}
long int Row_Icon::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Icon::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Icon::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Icon::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Icon::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Icon::PK_Icon_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Icon = val; is_modified=true; is_null[0]=false;}
void Row_Icon::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[1]=false;}
void Row_Icon::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_Icon::TransparentColor_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_TransparentColor = val; is_modified=true; is_null[3]=false;}
void Row_Icon::MainFileName_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_MainFileName = val; is_modified=true; is_null[4]=false;}
void Row_Icon::SelectedFileName_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SelectedFileName = val; is_modified=true; is_null[5]=false;}
void Row_Icon::AltFileNames_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_AltFileNames = val; is_modified=true; is_null[6]=false;}
void Row_Icon::BackgroundFileName_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_BackgroundFileName = val; is_modified=true; is_null[7]=false;}
void Row_Icon::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[8]=false;}
void Row_Icon::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[9]=false;}
void Row_Icon::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[10]=false;}
void Row_Icon::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[11]=false;}
void Row_Icon::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[12]=false;}

		
bool Row_Icon::TransparentColor_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Icon::SelectedFileName_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Icon::AltFileNames_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Icon::BackgroundFileName_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Icon::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Icon::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_Icon::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_Icon::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_Icon::TransparentColor_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Icon::SelectedFileName_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Icon::AltFileNames_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Icon::BackgroundFileName_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Icon::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Icon::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_Icon::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_Icon::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_Icon::PK_Icon_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Icon);

return buf;
}

string Row_Icon::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Define.c_str(), (unsigned long) min(25,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) min(25,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::TransparentColor_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_TransparentColor);

return buf;
}

string Row_Icon::MainFileName_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_MainFileName.c_str(), (unsigned long) min(50,m_MainFileName.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::SelectedFileName_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_SelectedFileName.c_str(), (unsigned long) min(50,m_SelectedFileName.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::AltFileNames_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char *buf = new char[401];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_AltFileNames.c_str(), (unsigned long) min(200,m_AltFileNames.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::BackgroundFileName_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_BackgroundFileName.c_str(), (unsigned long) min(50,m_BackgroundFileName.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Icon::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Icon::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Icon::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Icon::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Icon::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Icon::Key::Key(long int in_PK_Icon)
{
			pk_PK_Icon = in_PK_Icon;
	
}

Table_Icon::Key::Key(Row_Icon *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Icon = pRow->m_PK_Icon;
	
}		

bool Table_Icon::Key_Less::operator()(const Table_Icon::Key &key1, const Table_Icon::Key &key2) const
{
			if (key1.pk_PK_Icon!=key2.pk_PK_Icon)
return key1.pk_PK_Icon<key2.pk_PK_Icon;
else
return false;	
}	

bool Table_Icon::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Icon *pRow = (Row_Icon *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Icon_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->TransparentColor_asSQL()+", "+pRow->MainFileName_asSQL()+", "+pRow->SelectedFileName_asSQL()+", "+pRow->AltFileNames_asSQL()+", "+pRow->BackgroundFileName_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Icon (`PK_Icon`, `Define`, `Description`, `TransparentColor`, `MainFileName`, `SelectedFileName`, `AltFileNames`, `BackgroundFileName`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
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
pRow->m_PK_Icon=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Icon);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Icon* pRow = (Row_Icon*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Icon);

		char tmp_PK_Icon[32];
sprintf(tmp_PK_Icon, "%li", key.pk);


string condition;
condition = condition + "`PK_Icon`=" + tmp_PK_Icon;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Icon`="+pRow->PK_Icon_asSQL()+", `Define`="+pRow->Define_asSQL()+", `Description`="+pRow->Description_asSQL()+", `TransparentColor`="+pRow->TransparentColor_asSQL()+", `MainFileName`="+pRow->MainFileName_asSQL()+", `SelectedFileName`="+pRow->SelectedFileName_asSQL()+", `AltFileNames`="+pRow->AltFileNames_asSQL()+", `BackgroundFileName`="+pRow->BackgroundFileName_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Icon set " + update_values_list + " where " + condition;
			
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
		Row_Icon* pRow = (Row_Icon*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Icon* pRow = (Row_Icon*) (*i).second;	

		char tmp_PK_Icon[32];
sprintf(tmp_PK_Icon, "%li", key.pk);


string condition;
condition = condition + "`PK_Icon`=" + tmp_PK_Icon;

	
		string query = "delete from Icon where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Icon*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Icon::GetRows(string where_statement,vector<class Row_Icon*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Icon`.* from Icon " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Icon`.* from Icon where " + where_statement;
		
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

		Row_Icon *pRow = new Row_Icon(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Icon = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Icon));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Define = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_TransparentColor = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_TransparentColor));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_MainFileName = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_MainFileName = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SelectedFileName = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_SelectedFileName = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_AltFileNames = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_AltFileNames = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_BackgroundFileName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_BackgroundFileName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Icon);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Icon *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Icon* Table_Icon::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Icon *pRow = new Row_Icon(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Icon* Table_Icon::GetRow(long int in_PK_Icon)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Icon);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Icon*) (*i).second;
	//we have to fetch row
	Row_Icon* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Icon* Table_Icon::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Icon[32];
sprintf(tmp_PK_Icon, "%li", key.pk);


string condition;
condition = condition + "`PK_Icon`=" + tmp_PK_Icon;


	string query = "select * from Icon where " + condition;		

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

	Row_Icon *pRow = new Row_Icon(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Icon = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Icon));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Define = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Description = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_TransparentColor = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_TransparentColor));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_MainFileName = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_MainFileName = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SelectedFileName = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_SelectedFileName = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_AltFileNames = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_AltFileNames = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_BackgroundFileName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_BackgroundFileName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_id));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_batch));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_user));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_psc_frozen));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_psc_mod = string(row[12],lengths[12]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_Icon::CommandGroup_FK_Icon_getrows(vector <class Row_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CommandGroup *pTable = table->database->CommandGroup_get();
pTable->GetRows("`FK_Icon`=" + StringUtils::itos(m_PK_Icon),rows);
}
void Row_Icon::Floorplan_FK_Icon_getrows(vector <class Row_Floorplan*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Floorplan *pTable = table->database->Floorplan_get();
pTable->GetRows("`FK_Icon`=" + StringUtils::itos(m_PK_Icon),rows);
}
void Row_Icon::Room_FK_Icon_getrows(vector <class Row_Room*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Room *pTable = table->database->Room_get();
pTable->GetRows("`FK_Icon`=" + StringUtils::itos(m_PK_Icon),rows);
}



