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
#include "Table_Size.h"

#include "Table_Orbiter.h"


void Database_pluto_main::CreateTable_Size()
{
	tblSize = new Table_Size(this);
}

void Database_pluto_main::DeleteTable_Size()
{
	delete tblSize;
}

Table_Size::~Table_Size()
{
	map<Table_Size::Key, class Row_Size*, Table_Size::Key_Less>::iterator it;
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


void Row_Size::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Size*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Size::Key key(this);					
			map<Table_Size::Key, Row_Size*, Table_Size::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Size::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Size::Key key(this);		
		Row_Size *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Size::Row_Size(Table_Size *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Size::SetDefaultValues()
{
	m_PK_Size = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
m_Width = 0;
is_null[2] = false;
m_Height = 0;
is_null[3] = false;
m_ScaleX = 0;
is_null[4] = false;
m_ScaleY = 0;
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

long int Row_Size::PK_Size_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Size;}
string Row_Size::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_Size::Width_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Width;}
long int Row_Size::Height_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Height;}
long int Row_Size::ScaleX_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ScaleX;}
long int Row_Size::ScaleY_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ScaleY;}
string Row_Size::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Size::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Size::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Size::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Size::PK_Size_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Size = val; is_modified=true; is_null[0]=false;}
void Row_Size::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Size::Width_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Width = val; is_modified=true; is_null[2]=false;}
void Row_Size::Height_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Height = val; is_modified=true; is_null[3]=false;}
void Row_Size::ScaleX_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ScaleX = val; is_modified=true; is_null[4]=false;}
void Row_Size::ScaleY_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ScaleY = val; is_modified=true; is_null[5]=false;}
void Row_Size::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Size::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_Size::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_Size::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[9]=false;}

		
bool Row_Size::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Size::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Size::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}

			
void Row_Size::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Size::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Size::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
	

string Row_Size::PK_Size_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Size);

return buf;
}

string Row_Size::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_Size::Width_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Width);

return buf;
}

string Row_Size::Height_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Height);

return buf;
}

string Row_Size::ScaleX_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ScaleX);

return buf;
}

string Row_Size::ScaleY_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ScaleY);

return buf;
}

string Row_Size::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Size::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Size::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Size::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Size::Key::Key(long int in_PK_Size)
{
			pk_PK_Size = in_PK_Size;
	
}

Table_Size::Key::Key(Row_Size *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Size = pRow->m_PK_Size;
	
}		

bool Table_Size::Key_Less::operator()(const Table_Size::Key &key1, const Table_Size::Key &key2) const
{
			if (key1.pk_PK_Size!=key2.pk_PK_Size)
return key1.pk_PK_Size<key2.pk_PK_Size;
else
return false;	
}	

void Table_Size::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Size*>::iterator i = addedRows.begin();
	
		Row_Size *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Size_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Width_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->ScaleX_asSQL()+", "+pRow->ScaleY_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Size (PK_Size, Description, Width, Height, ScaleX, ScaleY, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Size=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Size*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Size* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Size[32];
sprintf(tmp_PK_Size, "%li", key.pk_PK_Size);


string condition;
condition = condition + "PK_Size=" + tmp_PK_Size;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Size="+pRow->PK_Size_asSQL()+", Description="+pRow->Description_asSQL()+", Width="+pRow->Width_asSQL()+", Height="+pRow->Height_asSQL()+", ScaleX="+pRow->ScaleX_asSQL()+", ScaleY="+pRow->ScaleY_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Size set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Size*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Size*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Size[32];
sprintf(tmp_PK_Size, "%li", key.pk_PK_Size);


string condition;
condition = condition + "PK_Size=" + tmp_PK_Size;

	
		string query = "delete from Size where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Size::GetRows(string where_statement,vector<class Row_Size*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Size where " + where_statement;
		
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

		Row_Size *pRow = new Row_Size(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Size = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Size));
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
pRow->m_Width = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Width));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Height));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ScaleX = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_ScaleX));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ScaleY = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ScaleY));
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
		
                map<Table_Size::Key, Row_Size*, Table_Size::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Size* Table_Size::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Size *pRow = new Row_Size(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Size* Table_Size::GetRow(long int in_PK_Size)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Size);

	map<Key, Row_Size*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Size* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Size* Table_Size::FetchRow(Table_Size::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Size[32];
sprintf(tmp_PK_Size, "%li", key.pk_PK_Size);


string condition;
condition = condition + "PK_Size=" + tmp_PK_Size;


	string query = "select * from Size where " + condition;		

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

	Row_Size *pRow = new Row_Size(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Size = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Size));
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
pRow->m_Width = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Width));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Height));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ScaleX = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_ScaleX));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ScaleY = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ScaleY));
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




void Row_Size::Orbiter_FK_Size_getrows(vector <class Row_Orbiter*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Orbiter *pTable = table->database->Orbiter_get();
pTable->GetRows("FK_Size=" + StringUtils::itos(m_PK_Size),rows);
}



