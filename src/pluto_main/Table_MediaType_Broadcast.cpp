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
#include "Table_MediaType_Broadcast.h"
#include "Table_MediaType.h"
#include "Table_Broadcast.h"



void Database_pluto_main::CreateTable_MediaType_Broadcast()
{
	tblMediaType_Broadcast = new Table_MediaType_Broadcast(this);
}

void Database_pluto_main::DeleteTable_MediaType_Broadcast()
{
	delete tblMediaType_Broadcast;
}

Table_MediaType_Broadcast::~Table_MediaType_Broadcast()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_MediaType_Broadcast *pRow = (Row_MediaType_Broadcast *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_MediaType_Broadcast *pRow = (Row_MediaType_Broadcast *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_MediaType_Broadcast::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_MediaType_Broadcast *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_MediaType_Broadcast *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_MediaType,pRow->m_FK_Broadcast);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_MediaType_Broadcast::Reload()
{
	Row_MediaType_Broadcast *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_MediaType,pRow->m_FK_Broadcast);
		Row_MediaType_Broadcast *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MediaType_Broadcast::Row_MediaType_Broadcast(Table_MediaType_Broadcast *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MediaType_Broadcast::SetDefaultValues()
{
	m_FK_MediaType = 0;
is_null[0] = false;
m_FK_Broadcast = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_psc_frozen = 0;
is_null[5] = false;
m_psc_mod = "00000000000000";
is_null[6] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MediaType_Broadcast::FK_MediaType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_MediaType;}
long int Row_MediaType_Broadcast::FK_Broadcast_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Broadcast;}
long int Row_MediaType_Broadcast::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_MediaType_Broadcast::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_MediaType_Broadcast::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_MediaType_Broadcast::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_MediaType_Broadcast::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_MediaType_Broadcast::FK_MediaType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_MediaType = val; is_modified=true; is_null[0]=false;}
void Row_MediaType_Broadcast::FK_Broadcast_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Broadcast = val; is_modified=true; is_null[1]=false;}
void Row_MediaType_Broadcast::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[2]=false;}
void Row_MediaType_Broadcast::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[3]=false;}
void Row_MediaType_Broadcast::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[4]=false;}
void Row_MediaType_Broadcast::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[5]=false;}
void Row_MediaType_Broadcast::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[6]=false;}

		
bool Row_MediaType_Broadcast::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_MediaType_Broadcast::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_MediaType_Broadcast::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_MediaType_Broadcast::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_MediaType_Broadcast::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_MediaType_Broadcast::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_MediaType_Broadcast::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_MediaType_Broadcast::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_MediaType_Broadcast::FK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_MediaType);

return buf;
}

string Row_MediaType_Broadcast::FK_Broadcast_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Broadcast);

return buf;
}

string Row_MediaType_Broadcast::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_MediaType_Broadcast::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_MediaType_Broadcast::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_MediaType_Broadcast::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_MediaType_Broadcast::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_MediaType_Broadcast::Key::Key(long int in_FK_MediaType, long int in_FK_Broadcast)
{
			pk_FK_MediaType = in_FK_MediaType;
pk_FK_Broadcast = in_FK_Broadcast;
	
}

Table_MediaType_Broadcast::Key::Key(Row_MediaType_Broadcast *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_MediaType = pRow->m_FK_MediaType;
pk_FK_Broadcast = pRow->m_FK_Broadcast;
	
}		

bool Table_MediaType_Broadcast::Key_Less::operator()(const Table_MediaType_Broadcast::Key &key1, const Table_MediaType_Broadcast::Key &key2) const
{
			if (key1.pk_FK_MediaType!=key2.pk_FK_MediaType)
return key1.pk_FK_MediaType<key2.pk_FK_MediaType;
else
if (key1.pk_FK_Broadcast!=key2.pk_FK_Broadcast)
return key1.pk_FK_Broadcast<key2.pk_FK_Broadcast;
else
return false;	
}	

bool Table_MediaType_Broadcast::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_MediaType_Broadcast *pRow = (Row_MediaType_Broadcast *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_MediaType_asSQL()+", "+pRow->FK_Broadcast_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into MediaType_Broadcast (`FK_MediaType`, `FK_Broadcast`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
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
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_MediaType,pRow->m_FK_Broadcast);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_MediaType_Broadcast* pRow = (Row_MediaType_Broadcast*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_MediaType,pRow->m_FK_Broadcast);

		char tmp_FK_MediaType[32];
sprintf(tmp_FK_MediaType, "%li", key.pk1);

char tmp_FK_Broadcast[32];
sprintf(tmp_FK_Broadcast, "%li", key.pk2);


string condition;
condition = condition + "`FK_MediaType`=" + tmp_FK_MediaType+" AND "+"`FK_Broadcast`=" + tmp_FK_Broadcast;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_MediaType`="+pRow->FK_MediaType_asSQL()+", `FK_Broadcast`="+pRow->FK_Broadcast_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update MediaType_Broadcast set " + update_values_list + " where " + condition;
			
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
		Row_MediaType_Broadcast* pRow = (Row_MediaType_Broadcast*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_MediaType_Broadcast* pRow = (Row_MediaType_Broadcast*) (*i).second;	

		char tmp_FK_MediaType[32];
sprintf(tmp_FK_MediaType, "%li", key.pk1);

char tmp_FK_Broadcast[32];
sprintf(tmp_FK_Broadcast, "%li", key.pk2);


string condition;
condition = condition + "`FK_MediaType`=" + tmp_FK_MediaType+" AND "+"`FK_Broadcast`=" + tmp_FK_Broadcast;

	
		string query = "delete from MediaType_Broadcast where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_MediaType_Broadcast*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_MediaType_Broadcast::GetRows(string where_statement,vector<class Row_MediaType_Broadcast*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `MediaType_Broadcast`.* from MediaType_Broadcast " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `MediaType_Broadcast`.* from MediaType_Broadcast where " + where_statement;
		
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

		Row_MediaType_Broadcast *pRow = new Row_MediaType_Broadcast(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Broadcast = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Broadcast));
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



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_MediaType,pRow->m_FK_Broadcast);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_MediaType_Broadcast *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_MediaType_Broadcast* Table_MediaType_Broadcast::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_MediaType_Broadcast *pRow = new Row_MediaType_Broadcast(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MediaType_Broadcast* Table_MediaType_Broadcast::GetRow(long int in_FK_MediaType, long int in_FK_Broadcast)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_MediaType, in_FK_Broadcast);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_MediaType_Broadcast*) (*i).second;
	//we have to fetch row
	Row_MediaType_Broadcast* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MediaType_Broadcast* Table_MediaType_Broadcast::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_MediaType[32];
sprintf(tmp_FK_MediaType, "%li", key.pk1);

char tmp_FK_Broadcast[32];
sprintf(tmp_FK_Broadcast, "%li", key.pk2);


string condition;
condition = condition + "`FK_MediaType`=" + tmp_FK_MediaType+" AND "+"`FK_Broadcast`=" + tmp_FK_Broadcast;


	string query = "select * from MediaType_Broadcast where " + condition;		

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

	Row_MediaType_Broadcast *pRow = new Row_MediaType_Broadcast(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Broadcast = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Broadcast));
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



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_MediaType* Row_MediaType_Broadcast::FK_MediaType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_MediaType *pTable = table->database->MediaType_get();
return pTable->GetRow(m_FK_MediaType);
}
class Row_Broadcast* Row_MediaType_Broadcast::FK_Broadcast_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Broadcast *pTable = table->database->Broadcast_get();
return pTable->GetRow(m_FK_Broadcast);
}





