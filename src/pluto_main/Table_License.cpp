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
#include "Table_License.h"

#include "Table_Package.h"
#include "Table_PaidLicense.h"


void Database_pluto_main::CreateTable_License()
{
	tblLicense = new Table_License(this);
}

void Database_pluto_main::DeleteTable_License()
{
	delete tblLicense;
}

Table_License::~Table_License()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_License *pRow = (Row_License *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_License *pRow = (Row_License *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_License::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_License *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_License *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_License);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_License::Reload()
{
	Row_License *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_License);
		Row_License *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_License::Row_License(Table_License *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_License::SetDefaultValues()
{
	m_PK_License = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
m_Define = "";
is_null[2] = false;
is_null[3] = true;
is_null[4] = true;
m_RequiresPayment = 0;
is_null[5] = false;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
m_psc_frozen = 0;
is_null[9] = false;
m_psc_mod = "00000000000000";
is_null[10] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_License::PK_License_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_License;}
string Row_License::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_License::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_License::Summary_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Summary;}
string Row_License::URL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_URL;}
short int Row_License::RequiresPayment_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_RequiresPayment;}
long int Row_License::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_License::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_License::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_License::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_License::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_License::PK_License_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_License = val; is_modified=true; is_null[0]=false;}
void Row_License::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_License::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_License::Summary_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Summary = val; is_modified=true; is_null[3]=false;}
void Row_License::URL_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_URL = val; is_modified=true; is_null[4]=false;}
void Row_License::RequiresPayment_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_RequiresPayment = val; is_modified=true; is_null[5]=false;}
void Row_License::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_License::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_License::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_License::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_License::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}

		
bool Row_License::Summary_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_License::URL_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_License::RequiresPayment_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_License::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_License::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_License::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_License::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_License::Summary_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_License::URL_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_License::RequiresPayment_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_License::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_License::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_License::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_License::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_License::PK_License_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_License);

return buf;
}

string Row_License::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_License::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char *buf = new char[61];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_License::Summary_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[5000000];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Summary.c_str(), (unsigned long) m_Summary.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_License::URL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_URL.c_str(), (unsigned long) m_URL.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_License::RequiresPayment_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RequiresPayment);

return buf;
}

string Row_License::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_License::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_License::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_License::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_License::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_License::Key::Key(long int in_PK_License)
{
			pk_PK_License = in_PK_License;
	
}

Table_License::Key::Key(Row_License *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_License = pRow->m_PK_License;
	
}		

bool Table_License::Key_Less::operator()(const Table_License::Key &key1, const Table_License::Key &key2) const
{
			if (key1.pk_PK_License!=key2.pk_PK_License)
return key1.pk_PK_License<key2.pk_PK_License;
else
return false;	
}	

bool Table_License::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_License *pRow = (Row_License *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_License_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Summary_asSQL()+", "+pRow->URL_asSQL()+", "+pRow->RequiresPayment_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into License (`PK_License`, `Description`, `Define`, `Summary`, `URL`, `RequiresPayment`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_License=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_License);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_License* pRow = (Row_License*) (*i).second;	
		SingleLongKey key(pRow->m_PK_License);

		char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk);


string condition;
condition = condition + "`PK_License`=" + tmp_PK_License;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_License`="+pRow->PK_License_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `Summary`="+pRow->Summary_asSQL()+", `URL`="+pRow->URL_asSQL()+", `RequiresPayment`="+pRow->RequiresPayment_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update License set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_License* pRow = (Row_License*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_License* pRow = (Row_License*) (*i).second;	

		char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk);


string condition;
condition = condition + "`PK_License`=" + tmp_PK_License;

	
		string query = "delete from License where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			return false;
		}	
		
		pRow = (Row_License*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_License::GetRows(string where_statement,vector<class Row_License*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from License " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from License where " + where_statement;
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

		Row_License *pRow = new Row_License(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_License = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_License));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Summary = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_URL = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_RequiresPayment = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_RequiresPayment));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_License);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_License *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_License* Table_License::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_License *pRow = new Row_License(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_License* Table_License::GetRow(long int in_PK_License)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_License);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_License*) (*i).second;
	//we have to fetch row
	Row_License* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_License* Table_License::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_License[32];
sprintf(tmp_PK_License, "%li", key.pk);


string condition;
condition = condition + "`PK_License`=" + tmp_PK_License;


	string query = "select * from License where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		cerr << "Cannot perform query: [" << query << "]" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

	Row_License *pRow = new Row_License(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_License = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_License));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Summary = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_URL = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_RequiresPayment = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_RequiresPayment));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}



	mysql_free_result(res);			
	
	return pRow;						
}




void Row_License::Package_FK_License_getrows(vector <class Row_Package*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
pTable->GetRows("`FK_License`=" + StringUtils::itos(m_PK_License),rows);
}
void Row_License::PaidLicense_FK_License_getrows(vector <class Row_PaidLicense*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_PaidLicense *pTable = table->database->PaidLicense_get();
pTable->GetRows("`FK_License`=" + StringUtils::itos(m_PK_License),rows);
}



