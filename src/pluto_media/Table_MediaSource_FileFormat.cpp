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
#include "Table_MediaSource_FileFormat.h"
#include "Table_MediaSource.h"



void Database_pluto_media::CreateTable_MediaSource_FileFormat()
{
	tblMediaSource_FileFormat = new Table_MediaSource_FileFormat(this);
}

void Database_pluto_media::DeleteTable_MediaSource_FileFormat()
{
	if( tblMediaSource_FileFormat )
		delete tblMediaSource_FileFormat;
}

bool Database_pluto_media::Commit_MediaSource_FileFormat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblMediaSource_FileFormat->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_MediaSource_FileFormat::~Table_MediaSource_FileFormat()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_MediaSource_FileFormat *pRow = (Row_MediaSource_FileFormat *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_MediaSource_FileFormat *pRow = (Row_MediaSource_FileFormat *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_MediaSource_FileFormat::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_MediaSource_FileFormat *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_MediaSource_FileFormat *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_MediaSource);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_MediaSource_FileFormat::Reload()
{
	Row_MediaSource_FileFormat *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_MediaSource);
		Row_MediaSource_FileFormat *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MediaSource_FileFormat::Row_MediaSource_FileFormat(Table_MediaSource_FileFormat *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MediaSource_FileFormat::SetDefaultValues()
{
	m_EK_MediaType = 0;
is_null[0] = false;
m_FK_MediaSource = 0;
is_null[1] = false;
is_null[2] = true;
m_psc_id = 0;
is_null[3] = true;
m_psc_batch = 0;
is_null[4] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[5] = false;
m_psc_mod = "00000000000000";
is_null[6] = false;
is_null[7] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MediaSource_FileFormat::EK_MediaType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_EK_MediaType;}
long int Row_MediaSource_FileFormat::FK_MediaSource_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_MediaSource;}
long int Row_MediaSource_FileFormat::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_MediaSource_FileFormat::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_MediaSource_FileFormat::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_MediaSource_FileFormat::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_MediaSource_FileFormat::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}
long int Row_MediaSource_FileFormat::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_restrict;}

		
void Row_MediaSource_FileFormat::EK_MediaType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_EK_MediaType = val; is_modified=true; is_null[0]=false;}
void Row_MediaSource_FileFormat::FK_MediaSource_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_MediaSource = val; is_modified=true; is_null[1]=false;}
void Row_MediaSource_FileFormat::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[2]=false;}
void Row_MediaSource_FileFormat::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[3]=false;}
void Row_MediaSource_FileFormat::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[4]=false;}
void Row_MediaSource_FileFormat::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[5]=false;}
void Row_MediaSource_FileFormat::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[6]=false;}
void Row_MediaSource_FileFormat::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_restrict = val; is_modified=true; is_null[7]=false;}

		
bool Row_MediaSource_FileFormat::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[2];}
bool Row_MediaSource_FileFormat::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_MediaSource_FileFormat::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_MediaSource_FileFormat::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_MediaSource_FileFormat::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[7];}

			
void Row_MediaSource_FileFormat::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[2]=val;
is_modified=true;
}
void Row_MediaSource_FileFormat::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_MediaSource_FileFormat::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_MediaSource_FileFormat::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_MediaSource_FileFormat::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[7]=val;
is_modified=true;
}
	

string Row_MediaSource_FileFormat::EK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_MediaType);

return buf;
}

string Row_MediaSource_FileFormat::FK_MediaSource_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_MediaSource);

return buf;
}

string Row_MediaSource_FileFormat::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_MediaSource_FileFormat::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_MediaSource_FileFormat::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_MediaSource_FileFormat::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_MediaSource_FileFormat::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MediaSource_FileFormat::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_MediaSource_FileFormat::Key::Key(long int in_EK_MediaType, long int in_FK_MediaSource)
{
			pk_EK_MediaType = in_EK_MediaType;
pk_FK_MediaSource = in_FK_MediaSource;
	
}

Table_MediaSource_FileFormat::Key::Key(Row_MediaSource_FileFormat *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_EK_MediaType = pRow->m_EK_MediaType;
pk_FK_MediaSource = pRow->m_FK_MediaSource;
	
}		

bool Table_MediaSource_FileFormat::Key_Less::operator()(const Table_MediaSource_FileFormat::Key &key1, const Table_MediaSource_FileFormat::Key &key2) const
{
			if (key1.pk_EK_MediaType!=key2.pk_EK_MediaType)
return key1.pk_EK_MediaType<key2.pk_EK_MediaType;
else
if (key1.pk_FK_MediaSource!=key2.pk_FK_MediaSource)
return key1.pk_FK_MediaSource<key2.pk_FK_MediaSource;
else
return false;	
}	

bool Table_MediaSource_FileFormat::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_MediaSource_FileFormat *pRow = (Row_MediaSource_FileFormat *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->EK_MediaType_asSQL()+", "+pRow->FK_MediaSource_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into MediaSource_FileFormat (`EK_MediaType`, `FK_MediaSource`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
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
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_MediaSource);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_MediaSource_FileFormat* pRow = (Row_MediaSource_FileFormat*) (*i).second;	
		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_MediaSource);

		char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_MediaSource[32];
sprintf(tmp_FK_MediaSource, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_MediaSource`=" + tmp_FK_MediaSource;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`EK_MediaType`="+pRow->EK_MediaType_asSQL()+", `FK_MediaSource`="+pRow->FK_MediaSource_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update MediaSource_FileFormat set " + update_values_list + " where " + condition;
			
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
		Row_MediaSource_FileFormat* pRow = (Row_MediaSource_FileFormat*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_MediaSource_FileFormat* pRow = (Row_MediaSource_FileFormat*) (*i).second;	

		char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_MediaSource[32];
sprintf(tmp_FK_MediaSource, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_MediaSource`=" + tmp_FK_MediaSource;

	
		string query = "delete from MediaSource_FileFormat where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_MediaSource_FileFormat*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_MediaSource_FileFormat::GetRows(string where_statement,vector<class Row_MediaSource_FileFormat*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `MediaSource_FileFormat`.* from MediaSource_FileFormat " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `MediaSource_FileFormat`.* from MediaSource_FileFormat where " + where_statement;
	else
		query = "select `MediaSource_FileFormat`.* from MediaSource_FileFormat";
		
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

		Row_MediaSource_FileFormat *pRow = new Row_MediaSource_FileFormat(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_MediaSource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_MediaSource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_MediaSource);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_MediaSource_FileFormat *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_MediaSource_FileFormat* Table_MediaSource_FileFormat::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_MediaSource_FileFormat *pRow = new Row_MediaSource_FileFormat(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MediaSource_FileFormat* Table_MediaSource_FileFormat::GetRow(long int in_EK_MediaType, long int in_FK_MediaSource)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	DoubleLongKey row_key(in_EK_MediaType, in_FK_MediaSource);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_MediaSource_FileFormat*) (*i).second;
	//we have to fetch row
	Row_MediaSource_FileFormat* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MediaSource_FileFormat* Table_MediaSource_FileFormat::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_MediaSource[32];
sprintf(tmp_FK_MediaSource, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_MediaSource`=" + tmp_FK_MediaSource;


	string query = "select * from MediaSource_FileFormat where " + condition;		

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

	Row_MediaSource_FileFormat *pRow = new Row_MediaSource_FileFormat(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_MediaSource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_MediaSource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_restrict));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_MediaSource* Row_MediaSource_FileFormat::FK_MediaSource_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_MediaSource *pTable = table->database->MediaSource_get();
return pTable->GetRow(m_FK_MediaSource);
}





