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
#include "Table_Text.h"
#include "Table_TextCategory.h"

#include "Table_DesignObjVariation_Text.h"
#include "Table_DesignObjVariation_Text.h"
#include "Table_Text_LS.h"
#include "Table_Text_LS_AltVersions.h"


void Database_pluto_main::CreateTable_Text()
{
	tblText = new Table_Text(this);
}

void Database_pluto_main::DeleteTable_Text()
{
	delete tblText;
}

Table_Text::~Table_Text()
{
	map<Table_Text::Key, class Row_Text*, Table_Text::Key_Less>::iterator it;
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


void Row_Text::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Text*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Text::Key key(this);					
			map<Table_Text::Key, Row_Text*, Table_Text::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Text::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Text::Key key(this);		
		Row_Text *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Text::Row_Text(Table_Text *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Text::SetDefaultValues()
{
	m_PK_Text = 0;
is_null[0] = false;
m_FK_TextCategory = 0;
is_null[1] = false;
m_Description = "";
is_null[2] = false;
m_Define = "";
is_null[3] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[4] = false;
m_IsNew_RecordInfo = 1;
is_null[5] = false;
m_IsDeleted_RecordInfo = 0;
is_null[6] = false;
is_null[7] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Text::PK_Text_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Text;}
long int Row_Text::FK_TextCategory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_TextCategory;}
string Row_Text::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_Text::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_Text::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Text::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Text::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Text::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Text::PK_Text_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Text = val; is_modified=true; is_null[0]=false;}
void Row_Text::FK_TextCategory_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_TextCategory = val; is_modified=true; is_null[1]=false;}
void Row_Text::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_Text::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[3]=false;}
void Row_Text::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Text::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Text::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Text::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_Text::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Text::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Text::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Text::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Text::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Text::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Text::PK_Text_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Text);

return buf;
}

string Row_Text::FK_TextCategory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_TextCategory);

return buf;
}

string Row_Text::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Text::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_Text::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Text::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Text::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Text::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Text::Key::Key(long int in_PK_Text)
{
			pk_PK_Text = in_PK_Text;
	
}

Table_Text::Key::Key(Row_Text *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Text = pRow->m_PK_Text;
	
}		

bool Table_Text::Key_Less::operator()(const Table_Text::Key &key1, const Table_Text::Key &key2) const
{
			if (key1.pk_PK_Text!=key2.pk_PK_Text)
return key1.pk_PK_Text<key2.pk_PK_Text;
else
return false;	
}	

void Table_Text::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Text*>::iterator i = addedRows.begin();
	
		Row_Text *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Text_asSQL()+", "+pRow->FK_TextCategory_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Text (PK_Text, FK_TextCategory, Description, Define, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Text=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Text*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Text* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Text[32];
sprintf(tmp_PK_Text, "%li", key.pk_PK_Text);


string condition;
condition = condition + "PK_Text=" + tmp_PK_Text;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Text="+pRow->PK_Text_asSQL()+", FK_TextCategory="+pRow->FK_TextCategory_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Text set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Text*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Text*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Text[32];
sprintf(tmp_PK_Text, "%li", key.pk_PK_Text);


string condition;
condition = condition + "PK_Text=" + tmp_PK_Text;

	
		string query = "delete from Text where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Text::GetRows(string where_statement,vector<class Row_Text*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Text where " + where_statement;
		
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

		Row_Text *pRow = new Row_Text(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Text = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Text));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_TextCategory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_TextCategory));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Text::Key, Row_Text*, Table_Text::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Text* Table_Text::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Text *pRow = new Row_Text(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Text* Table_Text::GetRow(long int in_PK_Text)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Text);

	map<Key, Row_Text*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Text* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Text* Table_Text::FetchRow(Table_Text::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Text[32];
sprintf(tmp_PK_Text, "%li", key.pk_PK_Text);


string condition;
condition = condition + "PK_Text=" + tmp_PK_Text;


	string query = "select * from Text where " + condition;		

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

	Row_Text *pRow = new Row_Text(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Text = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Text));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_TextCategory = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_TextCategory));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Define = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_TextCategory* Row_Text::FK_TextCategory_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_TextCategory *pTable = table->database->TextCategory_get();
return pTable->GetRow(m_FK_TextCategory);
}


void Row_Text::DesignObjVariation_Text_FK_Text_getrows(vector <class Row_DesignObjVariation_Text*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation_Text *pTable = table->database->DesignObjVariation_Text_get();
pTable->GetRows("FK_Text=" + StringUtils::itos(m_PK_Text),rows);
}
void Row_Text::DesignObjVariation_Text_FK_Text_OverrideFromHeader_getrows(vector <class Row_DesignObjVariation_Text*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObjVariation_Text *pTable = table->database->DesignObjVariation_Text_get();
pTable->GetRows("FK_Text_OverrideFromHeader=" + StringUtils::itos(m_PK_Text),rows);
}
void Row_Text::Text_LS_FK_Text_getrows(vector <class Row_Text_LS*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Text_LS *pTable = table->database->Text_LS_get();
pTable->GetRows("FK_Text=" + StringUtils::itos(m_PK_Text),rows);
}
void Row_Text::Text_LS_AltVersions_FK_Text_getrows(vector <class Row_Text_LS_AltVersions*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Text_LS_AltVersions *pTable = table->database->Text_LS_AltVersions_get();
pTable->GetRows("FK_Text=" + StringUtils::itos(m_PK_Text),rows);
}



