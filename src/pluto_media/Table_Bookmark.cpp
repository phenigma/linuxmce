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
#include "Table_Bookmark.h"
#include "Table_File.h"
#include "Table_Picture.h"



void Database_pluto_media::CreateTable_Bookmark()
{
	tblBookmark = new Table_Bookmark(this);
}

void Database_pluto_media::DeleteTable_Bookmark()
{
	if( tblBookmark )
		delete tblBookmark;
}

Table_Bookmark::~Table_Bookmark()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Bookmark *pRow = (Row_Bookmark *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Bookmark *pRow = (Row_Bookmark *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Bookmark::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Bookmark *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Bookmark *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Bookmark);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Bookmark::Reload()
{
	Row_Bookmark *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Bookmark);
		Row_Bookmark *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Bookmark::Row_Bookmark(Table_Bookmark *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Bookmark::SetDefaultValues()
{
	m_PK_Bookmark = 0;
is_null[0] = false;
m_FK_File = 0;
is_null[1] = false;
m_EK_MediaType = 0;
is_null[2] = false;
is_null[3] = true;
m_FK_Picture = 0;
is_null[4] = true;
m_EK_Users = 0;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_psc_id = 0;
is_null[8] = true;
m_psc_batch = 0;
is_null[9] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[10] = false;
m_psc_mod = "00000000000000";
is_null[11] = false;
is_null[12] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Bookmark::PK_Bookmark_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_PK_Bookmark;}
long int Row_Bookmark::FK_File_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_File;}
long int Row_Bookmark::EK_MediaType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_EK_MediaType;}
long int Row_Bookmark::FK_Picture_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Picture;}
long int Row_Bookmark::EK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_EK_Users;}
string Row_Bookmark::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Description;}
string Row_Bookmark::Position_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Position;}
long int Row_Bookmark::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_Bookmark::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_Bookmark::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_Bookmark::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_Bookmark::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}
long int Row_Bookmark::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_restrict;}

		
void Row_Bookmark::PK_Bookmark_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_PK_Bookmark = val; is_modified=true; is_null[0]=false;}
void Row_Bookmark::FK_File_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_File = val; is_modified=true; is_null[1]=false;}
void Row_Bookmark::EK_MediaType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_EK_MediaType = val; is_modified=true; is_null[2]=false;}
void Row_Bookmark::FK_Picture_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Picture = val; is_modified=true; is_null[3]=false;}
void Row_Bookmark::EK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_EK_Users = val; is_modified=true; is_null[4]=false;}
void Row_Bookmark::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_Bookmark::Position_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Position = val; is_modified=true; is_null[6]=false;}
void Row_Bookmark::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[7]=false;}
void Row_Bookmark::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[8]=false;}
void Row_Bookmark::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[9]=false;}
void Row_Bookmark::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[10]=false;}
void Row_Bookmark::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[11]=false;}
void Row_Bookmark::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_restrict = val; is_modified=true; is_null[12]=false;}

		
bool Row_Bookmark::FK_Picture_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_Bookmark::EK_Users_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_Bookmark::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_Bookmark::Position_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[6];}
bool Row_Bookmark::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[7];}
bool Row_Bookmark::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[8];}
bool Row_Bookmark::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[9];}
bool Row_Bookmark::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[10];}
bool Row_Bookmark::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[12];}

			
void Row_Bookmark::FK_Picture_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Bookmark::EK_Users_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Bookmark::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Bookmark::Position_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Bookmark::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Bookmark::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Bookmark::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Bookmark::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Bookmark::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[12]=val;
is_modified=true;
}
	

string Row_Bookmark::PK_Bookmark_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Bookmark);

return buf;
}

string Row_Bookmark::FK_File_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_File);

return buf;
}

string Row_Bookmark::EK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_MediaType);

return buf;
}

string Row_Bookmark::FK_Picture_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Picture);

return buf;
}

string Row_Bookmark::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}

string Row_Bookmark::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char *buf = new char[121];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) min(60,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Bookmark::Position_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char *buf = new char[511];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Position.c_str(), (unsigned long) min(255,m_Position.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Bookmark::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Bookmark::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Bookmark::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Bookmark::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Bookmark::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[11])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Bookmark::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Bookmark::Key::Key(long int in_PK_Bookmark)
{
			pk_PK_Bookmark = in_PK_Bookmark;
	
}

Table_Bookmark::Key::Key(Row_Bookmark *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_PK_Bookmark = pRow->m_PK_Bookmark;
	
}		

bool Table_Bookmark::Key_Less::operator()(const Table_Bookmark::Key &key1, const Table_Bookmark::Key &key2) const
{
			if (key1.pk_PK_Bookmark!=key2.pk_PK_Bookmark)
return key1.pk_PK_Bookmark<key2.pk_PK_Bookmark;
else
return false;	
}	

bool Table_Bookmark::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Bookmark *pRow = (Row_Bookmark *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Bookmark_asSQL()+", "+pRow->FK_File_asSQL()+", "+pRow->EK_MediaType_asSQL()+", "+pRow->FK_Picture_asSQL()+", "+pRow->EK_Users_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Position_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Bookmark (`PK_Bookmark`, `FK_File`, `EK_MediaType`, `FK_Picture`, `EK_Users`, `Description`, `Position`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
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
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_Bookmark=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Bookmark);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Bookmark* pRow = (Row_Bookmark*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Bookmark);

		char tmp_PK_Bookmark[32];
sprintf(tmp_PK_Bookmark, "%li", key.pk);


string condition;
condition = condition + "`PK_Bookmark`=" + tmp_PK_Bookmark;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Bookmark`="+pRow->PK_Bookmark_asSQL()+", `FK_File`="+pRow->FK_File_asSQL()+", `EK_MediaType`="+pRow->EK_MediaType_asSQL()+", `FK_Picture`="+pRow->FK_Picture_asSQL()+", `EK_Users`="+pRow->EK_Users_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Position`="+pRow->Position_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Bookmark set " + update_values_list + " where " + condition;
			
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
		Row_Bookmark* pRow = (Row_Bookmark*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Bookmark* pRow = (Row_Bookmark*) (*i).second;	

		char tmp_PK_Bookmark[32];
sprintf(tmp_PK_Bookmark, "%li", key.pk);


string condition;
condition = condition + "`PK_Bookmark`=" + tmp_PK_Bookmark;

	
		string query = "delete from Bookmark where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Bookmark*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Bookmark::GetRows(string where_statement,vector<class Row_Bookmark*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Bookmark`.* from Bookmark " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Bookmark`.* from Bookmark where " + where_statement;
	else
		query = "select `Bookmark`.* from Bookmark";
		
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

		Row_Bookmark *pRow = new Row_Bookmark(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Bookmark = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Bookmark));
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
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_MediaType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Picture = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Picture));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_Users));
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
pRow->m_Position = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Position = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Bookmark);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Bookmark *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Bookmark* Table_Bookmark::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_Bookmark *pRow = new Row_Bookmark(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Bookmark* Table_Bookmark::GetRow(long int in_PK_Bookmark)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	SingleLongKey row_key(in_PK_Bookmark);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Bookmark*) (*i).second;
	//we have to fetch row
	Row_Bookmark* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Bookmark* Table_Bookmark::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_PK_Bookmark[32];
sprintf(tmp_PK_Bookmark, "%li", key.pk);


string condition;
condition = condition + "`PK_Bookmark`=" + tmp_PK_Bookmark;


	string query = "select * from Bookmark where " + condition;		

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

	Row_Bookmark *pRow = new Row_Bookmark(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Bookmark = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Bookmark));
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
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_MediaType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Picture = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Picture));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_Users));
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
pRow->m_Position = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Position = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_restrict));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_File* Row_Bookmark::FK_File_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_File *pTable = table->database->File_get();
return pTable->GetRow(m_FK_File);
}
class Row_Picture* Row_Bookmark::FK_Picture_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Picture *pTable = table->database->Picture_get();
return pTable->GetRow(m_FK_Picture);
}





