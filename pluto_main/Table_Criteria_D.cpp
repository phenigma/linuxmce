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
#include "Table_Criteria_D.h"
#include "Table_CriteriaParmNesting_D.h"
#include "Table_CriteriaList.h"

#include "Table_DesignObjVariation.h"
#include "Table_Skin.h"
#include "Table_StyleVariation.h"


void Database_pluto_main::CreateTable_Criteria_D()
{
	tblCriteria_D = new Table_Criteria_D(this);
}

void Database_pluto_main::DeleteTable_Criteria_D()
{
	delete tblCriteria_D;
}

Table_Criteria_D::~Table_Criteria_D()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Criteria_D *pRow = (Row_Criteria_D *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Criteria_D *pRow = (Row_Criteria_D *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Criteria_D::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Criteria_D *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Criteria_D *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Criteria_D);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Criteria_D::Reload()
{
	Row_Criteria_D *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Criteria_D);
		Row_Criteria_D *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Criteria_D::Row_Criteria_D(Table_Criteria_D *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Criteria_D::SetDefaultValues()
{
	m_PK_Criteria_D = 0;
is_null[0] = false;
m_FK_CriteriaParmNesting_D = 0;
is_null[1] = false;
m_FK_CriteriaList = 0;
is_null[2] = false;
m_Description = "";
is_null[3] = false;
m_Define = "";
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_psc_frozen = 0;
is_null[8] = false;
m_psc_mod = "00000000000000";
is_null[9] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Criteria_D::PK_Criteria_D_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Criteria_D;}
long int Row_Criteria_D::FK_CriteriaParmNesting_D_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaParmNesting_D;}
long int Row_Criteria_D::FK_CriteriaList_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_CriteriaList;}
string Row_Criteria_D::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Criteria_D::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_Criteria_D::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Criteria_D::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Criteria_D::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Criteria_D::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Criteria_D::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Criteria_D::PK_Criteria_D_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Criteria_D = val; is_modified=true; is_null[0]=false;}
void Row_Criteria_D::FK_CriteriaParmNesting_D_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaParmNesting_D = val; is_modified=true; is_null[1]=false;}
void Row_Criteria_D::FK_CriteriaList_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_CriteriaList = val; is_modified=true; is_null[2]=false;}
void Row_Criteria_D::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_Criteria_D::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[4]=false;}
void Row_Criteria_D::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[5]=false;}
void Row_Criteria_D::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[6]=false;}
void Row_Criteria_D::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[7]=false;}
void Row_Criteria_D::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[8]=false;}
void Row_Criteria_D::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[9]=false;}

		
bool Row_Criteria_D::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Criteria_D::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Criteria_D::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Criteria_D::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_Criteria_D::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Criteria_D::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Criteria_D::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Criteria_D::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_Criteria_D::PK_Criteria_D_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Criteria_D);

return buf;
}

string Row_Criteria_D::FK_CriteriaParmNesting_D_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaParmNesting_D);

return buf;
}

string Row_Criteria_D::FK_CriteriaList_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CriteriaList);

return buf;
}

string Row_Criteria_D::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Criteria_D::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Criteria_D::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Criteria_D::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Criteria_D::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Criteria_D::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Criteria_D::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Criteria_D::Key::Key(long int in_PK_Criteria_D)
{
			pk_PK_Criteria_D = in_PK_Criteria_D;
	
}

Table_Criteria_D::Key::Key(Row_Criteria_D *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Criteria_D = pRow->m_PK_Criteria_D;
	
}		

bool Table_Criteria_D::Key_Less::operator()(const Table_Criteria_D::Key &key1, const Table_Criteria_D::Key &key2) const
{
			if (key1.pk_PK_Criteria_D!=key2.pk_PK_Criteria_D)
return key1.pk_PK_Criteria_D<key2.pk_PK_Criteria_D;
else
return false;	
}	

bool Table_Criteria_D::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Criteria_D *pRow = (Row_Criteria_D *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Criteria_D_asSQL()+", "+pRow->FK_CriteriaParmNesting_D_asSQL()+", "+pRow->FK_CriteriaList_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into Criteria_D (PK_Criteria_D, FK_CriteriaParmNesting_D, FK_CriteriaList, Description, Define, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Criteria_D=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Criteria_D);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Criteria_D* pRow = (Row_Criteria_D*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Criteria_D);

		char tmp_PK_Criteria_D[32];
sprintf(tmp_PK_Criteria_D, "%li", key.pk);


string condition;
condition = condition + "PK_Criteria_D=" + tmp_PK_Criteria_D;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Criteria_D="+pRow->PK_Criteria_D_asSQL()+", FK_CriteriaParmNesting_D="+pRow->FK_CriteriaParmNesting_D_asSQL()+", FK_CriteriaList="+pRow->FK_CriteriaList_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update Criteria_D set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Criteria_D* pRow = (Row_Criteria_D*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Criteria_D* pRow = (Row_Criteria_D*) (*i).second;	

		char tmp_PK_Criteria_D[32];
sprintf(tmp_PK_Criteria_D, "%li", key.pk);


string condition;
condition = condition + "PK_Criteria_D=" + tmp_PK_Criteria_D;

	
		string query = "delete from Criteria_D where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_Criteria_D*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Criteria_D::GetRows(string where_statement,vector<class Row_Criteria_D*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Criteria_D " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Criteria_D where " + where_statement;
		
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

		Row_Criteria_D *pRow = new Row_Criteria_D(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Criteria_D = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Criteria_D));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting_D = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting_D));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaList));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Define = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Criteria_D);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Criteria_D *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Criteria_D* Table_Criteria_D::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Criteria_D *pRow = new Row_Criteria_D(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Criteria_D* Table_Criteria_D::GetRow(long int in_PK_Criteria_D)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Criteria_D);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Criteria_D*) (*i).second;
	//we have to fetch row
	Row_Criteria_D* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Criteria_D* Table_Criteria_D::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Criteria_D[32];
sprintf(tmp_PK_Criteria_D, "%li", key.pk);


string condition;
condition = condition + "PK_Criteria_D=" + tmp_PK_Criteria_D;


	string query = "select * from Criteria_D where " + condition;		

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

	Row_Criteria_D *pRow = new Row_Criteria_D(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Criteria_D = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Criteria_D));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CriteriaParmNesting_D = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CriteriaParmNesting_D));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_CriteriaList = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_CriteriaList));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Define = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_batch));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_user));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_psc_frozen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_psc_mod = string(row[9],lengths[9]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_CriteriaParmNesting_D* Row_Criteria_D::FK_CriteriaParmNesting_D_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaParmNesting_D *pTable = table->database->CriteriaParmNesting_D_get();
return pTable->GetRow(m_FK_CriteriaParmNesting_D);
}
class Row_CriteriaList* Row_Criteria_D::FK_CriteriaList_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_CriteriaList *pTable = table->database->CriteriaList_get();
return pTable->GetRow(m_FK_CriteriaList);
}


void Row_Criteria_D::DesignObjVariation_FK_Criteria_D_getrows(vector <class Row_DesignObjVariation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
pTable->GetRows("FK_Criteria_D=" + StringUtils::itos(m_PK_Criteria_D),rows);
}
void Row_Criteria_D::Skin_FK_Criteria_D_getrows(vector <class Row_Skin*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Skin *pTable = table->database->Skin_get();
pTable->GetRows("FK_Criteria_D=" + StringUtils::itos(m_PK_Criteria_D),rows);
}
void Row_Criteria_D::StyleVariation_FK_Criteria_D_getrows(vector <class Row_StyleVariation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StyleVariation *pTable = table->database->StyleVariation_get();
pTable->GetRows("FK_Criteria_D=" + StringUtils::itos(m_PK_Criteria_D),rows);
}



