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
#include "Table_Style.h"
#include "Table_Style.h"
#include "Table_Style.h"
#include "Table_Style.h"

#include "Table_DesignObjVariation_Text_Skin_Language.h"
#include "Table_Skin.h"
#include "Table_Style.h"
#include "Table_Style.h"
#include "Table_Style.h"
#include "Table_StyleVariation.h"


void Database_pluto_main::CreateTable_Style()
{
	tblStyle = new Table_Style(this);
}

void Database_pluto_main::DeleteTable_Style()
{
	delete tblStyle;
}

Table_Style::~Table_Style()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Style *pRow = (Row_Style *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Style *pRow = (Row_Style *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Style::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Style *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Style *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Style);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Style::Reload()
{
	Row_Style *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Style);
		Row_Style *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Style::Row_Style(Table_Style *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Style::SetDefaultValues()
{
	m_PK_Style = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_AlwaysIncludeOnOrbiter = 0;
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

long int Row_Style::PK_Style_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Style;}
string Row_Style::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Style::FK_Style_Selected_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Style_Selected;}
long int Row_Style::FK_Style_Highlighted_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Style_Highlighted;}
long int Row_Style::FK_Style_Alt_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Style_Alt;}
short int Row_Style::AlwaysIncludeOnOrbiter_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_AlwaysIncludeOnOrbiter;}
long int Row_Style::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Style::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Style::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Style::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Style::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Style::PK_Style_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Style = val; is_modified=true; is_null[0]=false;}
void Row_Style::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Style::FK_Style_Selected_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Style_Selected = val; is_modified=true; is_null[2]=false;}
void Row_Style::FK_Style_Highlighted_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Style_Highlighted = val; is_modified=true; is_null[3]=false;}
void Row_Style::FK_Style_Alt_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Style_Alt = val; is_modified=true; is_null[4]=false;}
void Row_Style::AlwaysIncludeOnOrbiter_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_AlwaysIncludeOnOrbiter = val; is_modified=true; is_null[5]=false;}
void Row_Style::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_Style::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_Style::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_Style::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_Style::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}

		
bool Row_Style::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Style::FK_Style_Selected_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Style::FK_Style_Highlighted_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Style::FK_Style_Alt_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Style::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Style::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Style::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Style::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_Style::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Style::FK_Style_Selected_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Style::FK_Style_Highlighted_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Style::FK_Style_Alt_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Style::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Style::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Style::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Style::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_Style::PK_Style_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Style);

return buf;
}

string Row_Style::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Style::FK_Style_Selected_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style_Selected);

return buf;
}

string Row_Style::FK_Style_Highlighted_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style_Highlighted);

return buf;
}

string Row_Style::FK_Style_Alt_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style_Alt);

return buf;
}

string Row_Style::AlwaysIncludeOnOrbiter_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AlwaysIncludeOnOrbiter);

return buf;
}

string Row_Style::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Style::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Style::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Style::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Style::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->db_handle, buf, m_psc_mod.c_str(), (unsigned long) m_psc_mod.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Style::Key::Key(long int in_PK_Style)
{
			pk_PK_Style = in_PK_Style;
	
}

Table_Style::Key::Key(Row_Style *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Style = pRow->m_PK_Style;
	
}		

bool Table_Style::Key_Less::operator()(const Table_Style::Key &key1, const Table_Style::Key &key2) const
{
			if (key1.pk_PK_Style!=key2.pk_PK_Style)
return key1.pk_PK_Style<key2.pk_PK_Style;
else
return false;	
}	

bool Table_Style::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Style *pRow = (Row_Style *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Style_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Style_Selected_asSQL()+", "+pRow->FK_Style_Highlighted_asSQL()+", "+pRow->FK_Style_Alt_asSQL()+", "+pRow->AlwaysIncludeOnOrbiter_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_mod_asSQL();

	
		string query = "insert into Style (PK_Style, Description, FK_Style_Selected, FK_Style_Highlighted, FK_Style_Alt, AlwaysIncludeOnOrbiter, psc_id, psc_batch, psc_user, psc_frozen, psc_mod) values ("+
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
pRow->m_PK_Style=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Style);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Style* pRow = (Row_Style*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Style);

		char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Style="+pRow->PK_Style_asSQL()+", Description="+pRow->Description_asSQL()+", FK_Style_Selected="+pRow->FK_Style_Selected_asSQL()+", FK_Style_Highlighted="+pRow->FK_Style_Highlighted_asSQL()+", FK_Style_Alt="+pRow->FK_Style_Alt_asSQL()+", AlwaysIncludeOnOrbiter="+pRow->AlwaysIncludeOnOrbiter_asSQL()+", psc_id="+pRow->psc_id_asSQL()+", psc_batch="+pRow->psc_batch_asSQL()+", psc_user="+pRow->psc_user_asSQL()+", psc_frozen="+pRow->psc_frozen_asSQL()+", psc_mod="+pRow->psc_mod_asSQL();

	
		string query = "update Style set " + update_values_list + " where " + condition;
			
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
		Row_Style* pRow = (Row_Style*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Style* pRow = (Row_Style*) (*i).second;	

		char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;

	
		string query = "delete from Style where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_Style*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Style::GetRows(string where_statement,vector<class Row_Style*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Style " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Style where " + where_statement;
		
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

		Row_Style *pRow = new Row_Style(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Style = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Style));
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
pRow->m_FK_Style_Selected = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Style_Selected));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Style_Highlighted = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Style_Highlighted));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Style_Alt = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Style_Alt));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AlwaysIncludeOnOrbiter = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AlwaysIncludeOnOrbiter));
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

		SingleLongKey key(pRow->m_PK_Style);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Style *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Style* Table_Style::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Style *pRow = new Row_Style(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Style* Table_Style::GetRow(long int in_PK_Style)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Style);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Style*) (*i).second;
	//we have to fetch row
	Row_Style* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Style* Table_Style::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;


	string query = "select * from Style where " + condition;		

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

	Row_Style *pRow = new Row_Style(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Style = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Style));
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
pRow->m_FK_Style_Selected = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Style_Selected));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Style_Highlighted = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Style_Highlighted));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Style_Alt = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Style_Alt));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AlwaysIncludeOnOrbiter = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AlwaysIncludeOnOrbiter));
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


class Row_Style* Row_Style::FK_Style_Selected_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style_Selected);
}
class Row_Style* Row_Style::FK_Style_Highlighted_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style_Highlighted);
}
class Row_Style* Row_Style::FK_Style_Alt_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style_Alt);
}


void Row_Style::DesignObjVariation_Text_Skin_Language_FK_Style_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation_Text_Skin_Language *pTable = table->database->DesignObjVariation_Text_Skin_Language_get();
pTable->GetRows("FK_Style=" + StringUtils::itos(m_PK_Style),rows);
}
void Row_Style::Skin_FK_Style_getrows(vector <class Row_Skin*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Skin *pTable = table->database->Skin_get();
pTable->GetRows("FK_Style=" + StringUtils::itos(m_PK_Style),rows);
}
void Row_Style::Style_FK_Style_Selected_getrows(vector <class Row_Style*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
pTable->GetRows("FK_Style_Selected=" + StringUtils::itos(m_PK_Style),rows);
}
void Row_Style::Style_FK_Style_Highlighted_getrows(vector <class Row_Style*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
pTable->GetRows("FK_Style_Highlighted=" + StringUtils::itos(m_PK_Style),rows);
}
void Row_Style::Style_FK_Style_Alt_getrows(vector <class Row_Style*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Style *pTable = table->database->Style_get();
pTable->GetRows("FK_Style_Alt=" + StringUtils::itos(m_PK_Style),rows);
}
void Row_Style::StyleVariation_FK_Style_getrows(vector <class Row_StyleVariation*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_StyleVariation *pTable = table->database->StyleVariation_get();
pTable->GetRows("FK_Style=" + StringUtils::itos(m_PK_Style),rows);
}



