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
#include "Table_DesignObjVariation_DesignObjParameter.h"
#include "Table_DesignObjVariation.h"
#include "Table_DesignObjParameter.h"



void Database_pluto_main::CreateTable_DesignObjVariation_DesignObjParameter()
{
	tblDesignObjVariation_DesignObjParameter = new Table_DesignObjVariation_DesignObjParameter(this);
}

void Database_pluto_main::DeleteTable_DesignObjVariation_DesignObjParameter()
{
	delete tblDesignObjVariation_DesignObjParameter;
}

Table_DesignObjVariation_DesignObjParameter::~Table_DesignObjVariation_DesignObjParameter()
{
	map<Table_DesignObjVariation_DesignObjParameter::Key, class Row_DesignObjVariation_DesignObjParameter*, Table_DesignObjVariation_DesignObjParameter::Key_Less>::iterator it;
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


void Row_DesignObjVariation_DesignObjParameter::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_DesignObjVariation_DesignObjParameter*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_DesignObjVariation_DesignObjParameter::Key key(this);					
			map<Table_DesignObjVariation_DesignObjParameter::Key, Row_DesignObjVariation_DesignObjParameter*, Table_DesignObjVariation_DesignObjParameter::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DesignObjVariation_DesignObjParameter::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_DesignObjVariation_DesignObjParameter::Key key(this);		
		Row_DesignObjVariation_DesignObjParameter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObjVariation_DesignObjParameter::Row_DesignObjVariation_DesignObjParameter(Table_DesignObjVariation_DesignObjParameter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObjVariation_DesignObjParameter::SetDefaultValues()
{
	m_FK_DesignObjVariation = 0;
is_null[0] = false;
m_FK_DesignObjParameter = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
m_psc_frozen = 0;
is_null[6] = false;
m_psc_mod = "00000000000000";
is_null[7] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DesignObjVariation_DesignObjParameter::FK_DesignObjVariation_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObjVariation;}
long int Row_DesignObjVariation_DesignObjParameter::FK_DesignObjParameter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObjParameter;}
string Row_DesignObjVariation_DesignObjParameter::Value_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Value;}
long int Row_DesignObjVariation_DesignObjParameter::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_DesignObjVariation_DesignObjParameter::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_DesignObjVariation_DesignObjParameter::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_DesignObjVariation_DesignObjParameter::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_DesignObjVariation_DesignObjParameter::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_DesignObjVariation_DesignObjParameter::FK_DesignObjVariation_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObjVariation = val; is_modified=true; is_null[0]=false;}
void Row_DesignObjVariation_DesignObjParameter::FK_DesignObjParameter_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObjParameter = val; is_modified=true; is_null[1]=false;}
void Row_DesignObjVariation_DesignObjParameter::Value_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Value = val; is_modified=true; is_null[2]=false;}
void Row_DesignObjVariation_DesignObjParameter::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_DesignObjVariation_DesignObjParameter::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_DesignObjVariation_DesignObjParameter::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_DesignObjVariation_DesignObjParameter::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_DesignObjVariation_DesignObjParameter::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}

		
bool Row_DesignObjVariation_DesignObjParameter::Value_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_DesignObjVariation_DesignObjParameter::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_DesignObjVariation_DesignObjParameter::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_DesignObjVariation_DesignObjParameter::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_DesignObjVariation_DesignObjParameter::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}

			
void Row_DesignObjVariation_DesignObjParameter::Value_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_DesignObjVariation_DesignObjParameter::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_DesignObjVariation_DesignObjParameter::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_DesignObjVariation_DesignObjParameter::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_DesignObjVariation_DesignObjParameter::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
	

string Row_DesignObjVariation_DesignObjParameter::FK_DesignObjVariation_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjVariation);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::FK_DesignObjParameter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjParameter);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::Value_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Value.c_str(), (unsigned long) m_Value.size());
return string()+"\""+buf+"\"";
}

string Row_DesignObjVariation_DesignObjParameter::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObjVariation_DesignObjParameter::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
return string()+"\""+buf+"\"";
}




Table_DesignObjVariation_DesignObjParameter::Key::Key(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter)
{
			pk_FK_DesignObjVariation = in_FK_DesignObjVariation;
pk_FK_DesignObjParameter = in_FK_DesignObjParameter;
	
}

Table_DesignObjVariation_DesignObjParameter::Key::Key(Row_DesignObjVariation_DesignObjParameter *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_DesignObjVariation = pRow->m_FK_DesignObjVariation;
pk_FK_DesignObjParameter = pRow->m_FK_DesignObjParameter;
	
}		

bool Table_DesignObjVariation_DesignObjParameter::Key_Less::operator()(const Table_DesignObjVariation_DesignObjParameter::Key &key1, const Table_DesignObjVariation_DesignObjParameter::Key &key2) const
{
			if (key1.pk_FK_DesignObjVariation!=key2.pk_FK_DesignObjVariation)
return key1.pk_FK_DesignObjVariation<key2.pk_FK_DesignObjVariation;
else
if (key1.pk_FK_DesignObjParameter!=key2.pk_FK_DesignObjParameter)
return key1.pk_FK_DesignObjParameter<key2.pk_FK_DesignObjParameter;
else
return false;	
}	

void Table_DesignObjVariation_DesignObjParameter::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_DesignObjVariation_DesignObjParameter*>::iterator i = addedRows.begin();
	
		Row_DesignObjVariation_DesignObjParameter *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DesignObjVariation_asSQL()+", "+pRow->FK_DesignObjParameter_asSQL()+", "+pRow->Value_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into DesignObjVariation_DesignObjParameter (FK_DesignObjVariation, FK_DesignObjParameter, Value, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_DesignObjVariation_DesignObjParameter*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_DesignObjVariation_DesignObjParameter* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_FK_DesignObjVariation[32];
sprintf(tmp_FK_DesignObjVariation, "%li", key.pk_FK_DesignObjVariation);

char tmp_FK_DesignObjParameter[32];
sprintf(tmp_FK_DesignObjParameter, "%li", key.pk_FK_DesignObjParameter);


string condition;
condition = condition + "FK_DesignObjVariation=" + tmp_FK_DesignObjVariation+" AND "+"FK_DesignObjParameter=" + tmp_FK_DesignObjParameter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_DesignObjVariation="+pRow->FK_DesignObjVariation_asSQL()+", FK_DesignObjParameter="+pRow->FK_DesignObjParameter_asSQL()+", Value="+pRow->Value_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update DesignObjVariation_DesignObjParameter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_DesignObjVariation_DesignObjParameter*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_DesignObjVariation_DesignObjParameter*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_FK_DesignObjVariation[32];
sprintf(tmp_FK_DesignObjVariation, "%li", key.pk_FK_DesignObjVariation);

char tmp_FK_DesignObjParameter[32];
sprintf(tmp_FK_DesignObjParameter, "%li", key.pk_FK_DesignObjParameter);


string condition;
condition = condition + "FK_DesignObjVariation=" + tmp_FK_DesignObjVariation+" AND "+"FK_DesignObjParameter=" + tmp_FK_DesignObjParameter;

	
		string query = "delete from DesignObjVariation_DesignObjParameter where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_DesignObjVariation_DesignObjParameter::GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObjParameter*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from DesignObjVariation_DesignObjParameter where " + where_statement;
		
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

		Row_DesignObjVariation_DesignObjParameter *pRow = new Row_DesignObjVariation_DesignObjParameter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DesignObjVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DesignObjVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjParameter));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Value = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_DesignObjVariation_DesignObjParameter::Key, Row_DesignObjVariation_DesignObjParameter*, Table_DesignObjVariation_DesignObjParameter::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_DesignObjVariation_DesignObjParameter* Table_DesignObjVariation_DesignObjParameter::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_DesignObjVariation_DesignObjParameter *pRow = new Row_DesignObjVariation_DesignObjParameter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObjVariation_DesignObjParameter* Table_DesignObjVariation_DesignObjParameter::GetRow(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_FK_DesignObjVariation, in_FK_DesignObjParameter);

	map<Key, Row_DesignObjVariation_DesignObjParameter*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_DesignObjVariation_DesignObjParameter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObjVariation_DesignObjParameter* Table_DesignObjVariation_DesignObjParameter::FetchRow(Table_DesignObjVariation_DesignObjParameter::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_DesignObjVariation[32];
sprintf(tmp_FK_DesignObjVariation, "%li", key.pk_FK_DesignObjVariation);

char tmp_FK_DesignObjParameter[32];
sprintf(tmp_FK_DesignObjParameter, "%li", key.pk_FK_DesignObjParameter);


string condition;
condition = condition + "FK_DesignObjVariation=" + tmp_FK_DesignObjVariation+" AND "+"FK_DesignObjParameter=" + tmp_FK_DesignObjParameter;


	string query = "select * from DesignObjVariation_DesignObjParameter where " + condition;		

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

	Row_DesignObjVariation_DesignObjParameter *pRow = new Row_DesignObjVariation_DesignObjParameter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DesignObjVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DesignObjVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjParameter));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Value = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Value = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DesignObjVariation* Row_DesignObjVariation_DesignObjParameter::FK_DesignObjVariation_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
return pTable->GetRow(m_FK_DesignObjVariation);
}
class Row_DesignObjParameter* Row_DesignObjVariation_DesignObjParameter::FK_DesignObjParameter_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjParameter *pTable = table->database->DesignObjParameter_get();
return pTable->GetRow(m_FK_DesignObjParameter);
}





