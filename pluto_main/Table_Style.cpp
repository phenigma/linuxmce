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
	map<Table_Style::Key, class Row_Style*, Table_Style::Key_Less>::iterator it;
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


void Row_Style::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Style*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Style::Key key(this);					
			map<Table_Style::Key, Row_Style*, Table_Style::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Style::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Style::Key key(this);		
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
m_Modification_RecordInfo = "00000000000000";
is_null[6] = false;
m_IsNew_RecordInfo = 1;
is_null[7] = false;
m_IsDeleted_RecordInfo = 0;
is_null[8] = false;
is_null[9] = true;


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
string Row_Style::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Style::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Style::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Style::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
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
void Row_Style::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Style::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_Style::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_Style::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[9]=false;}

		
bool Row_Style::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Style::FK_Style_Selected_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Style::FK_Style_Highlighted_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_Style::FK_Style_Alt_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_Style::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Style::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Style::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_Style::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Style::FK_Style_Selected_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Style::FK_Style_Highlighted_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_Style::FK_Style_Alt_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_Style::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Style::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Style::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

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

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
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

string Row_Style::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Style::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Style::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Style::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
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

void Table_Style::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Style*>::iterator i = addedRows.begin();
	
		Row_Style *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Style_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Style_Selected_asSQL()+", "+pRow->FK_Style_Highlighted_asSQL()+", "+pRow->FK_Style_Alt_asSQL()+", "+pRow->AlwaysIncludeOnOrbiter_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Style (PK_Style, Description, FK_Style_Selected, FK_Style_Highlighted, FK_Style_Alt, AlwaysIncludeOnOrbiter, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Style=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Style*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Style* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk_PK_Style);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Style="+pRow->PK_Style_asSQL()+", Description="+pRow->Description_asSQL()+", FK_Style_Selected="+pRow->FK_Style_Selected_asSQL()+", FK_Style_Highlighted="+pRow->FK_Style_Highlighted_asSQL()+", FK_Style_Alt="+pRow->FK_Style_Alt_asSQL()+", AlwaysIncludeOnOrbiter="+pRow->AlwaysIncludeOnOrbiter_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Style set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Style*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Style*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk_PK_Style);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;

	
		string query = "delete from Style where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Style::GetRows(string where_statement,vector<class Row_Style*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Style where " + where_statement;
		
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
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Modification_RecordInfo = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Style::Key, Row_Style*, Table_Style::Key_Less>::iterator i = cachedRows.find(key);
			
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

	Key row_key(in_PK_Style);

	map<Key, Row_Style*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Style* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Style* Table_Style::FetchRow(Table_Style::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Style[32];
sprintf(tmp_PK_Style, "%li", key.pk_PK_Style);


string condition;
condition = condition + "PK_Style=" + tmp_PK_Style;


	string query = "select * from Style where " + condition;		

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
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Modification_RecordInfo = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Users_RecordInfo));
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



