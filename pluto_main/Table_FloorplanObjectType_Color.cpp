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
#include "Table_FloorplanObjectType_Color.h"



void Database_pluto_main::CreateTable_FloorplanObjectType_Color()
{
	tblFloorplanObjectType_Color = new Table_FloorplanObjectType_Color(this);
}

void Database_pluto_main::DeleteTable_FloorplanObjectType_Color()
{
	delete tblFloorplanObjectType_Color;
}

Table_FloorplanObjectType_Color::~Table_FloorplanObjectType_Color()
{
	map<Table_FloorplanObjectType_Color::Key, class Row_FloorplanObjectType_Color*, Table_FloorplanObjectType_Color::Key_Less>::iterator it;
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


void Row_FloorplanObjectType_Color::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_FloorplanObjectType_Color*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_FloorplanObjectType_Color::Key key(this);					
			map<Table_FloorplanObjectType_Color::Key, Row_FloorplanObjectType_Color*, Table_FloorplanObjectType_Color::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_FloorplanObjectType_Color::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_FloorplanObjectType_Color::Key key(this);		
		Row_FloorplanObjectType_Color *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_FloorplanObjectType_Color::Row_FloorplanObjectType_Color(Table_FloorplanObjectType_Color *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_FloorplanObjectType_Color::SetDefaultValues()
{
	m_PK_FloorplanDesignObjType_Color = 0;
is_null[0] = false;
m_FK_FloorplanDesignObjType = 0;
is_null[1] = false;
m_Description = "";
is_null[2] = false;
m_Define = "";
is_null[3] = false;
m_Color = 0;
is_null[4] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[5] = false;
m_IsNew_RecordInfo = 1;
is_null[6] = false;
m_IsDeleted_RecordInfo = 0;
is_null[7] = false;
is_null[8] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_FloorplanObjectType_Color::PK_FloorplanDesignObjType_Color_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_FloorplanDesignObjType_Color;}
long int Row_FloorplanObjectType_Color::FK_FloorplanDesignObjType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_FloorplanDesignObjType;}
string Row_FloorplanObjectType_Color::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_FloorplanObjectType_Color::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
long int Row_FloorplanObjectType_Color::Color_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Color;}
string Row_FloorplanObjectType_Color::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_FloorplanObjectType_Color::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_FloorplanObjectType_Color::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_FloorplanObjectType_Color::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_FloorplanObjectType_Color::PK_FloorplanDesignObjType_Color_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_FloorplanDesignObjType_Color = val; is_modified=true; is_null[0]=false;}
void Row_FloorplanObjectType_Color::FK_FloorplanDesignObjType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_FloorplanDesignObjType = val; is_modified=true; is_null[1]=false;}
void Row_FloorplanObjectType_Color::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_FloorplanObjectType_Color::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[3]=false;}
void Row_FloorplanObjectType_Color::Color_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Color = val; is_modified=true; is_null[4]=false;}
void Row_FloorplanObjectType_Color::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_FloorplanObjectType_Color::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_FloorplanObjectType_Color::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[7]=false;}
void Row_FloorplanObjectType_Color::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[8]=false;}

		
bool Row_FloorplanObjectType_Color::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_FloorplanObjectType_Color::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_FloorplanObjectType_Color::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}

			
void Row_FloorplanObjectType_Color::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_FloorplanObjectType_Color::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_FloorplanObjectType_Color::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
	

string Row_FloorplanObjectType_Color::PK_FloorplanDesignObjType_Color_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_FloorplanDesignObjType_Color);

return buf;
}

string Row_FloorplanObjectType_Color::FK_FloorplanDesignObjType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_FloorplanDesignObjType);

return buf;
}

string Row_FloorplanObjectType_Color::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType_Color::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType_Color::Color_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Color);

return buf;
}

string Row_FloorplanObjectType_Color::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType_Color::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_FloorplanObjectType_Color::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_FloorplanObjectType_Color::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_FloorplanObjectType_Color::Key::Key(long int in_PK_FloorplanDesignObjType_Color)
{
			pk_PK_FloorplanDesignObjType_Color = in_PK_FloorplanDesignObjType_Color;
	
}

Table_FloorplanObjectType_Color::Key::Key(Row_FloorplanObjectType_Color *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_FloorplanDesignObjType_Color = pRow->m_PK_FloorplanDesignObjType_Color;
	
}		

bool Table_FloorplanObjectType_Color::Key_Less::operator()(const Table_FloorplanObjectType_Color::Key &key1, const Table_FloorplanObjectType_Color::Key &key2) const
{
			if (key1.pk_PK_FloorplanDesignObjType_Color!=key2.pk_PK_FloorplanDesignObjType_Color)
return key1.pk_PK_FloorplanDesignObjType_Color<key2.pk_PK_FloorplanDesignObjType_Color;
else
return false;	
}	

void Table_FloorplanObjectType_Color::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_FloorplanObjectType_Color*>::iterator i = addedRows.begin();
	
		Row_FloorplanObjectType_Color *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_FloorplanDesignObjType_Color_asSQL()+", "+pRow->FK_FloorplanDesignObjType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Color_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into FloorplanObjectType_Color (PK_FloorplanDesignObjType_Color, FK_FloorplanDesignObjType, Description, Define, Color, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_FloorplanDesignObjType_Color=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_FloorplanObjectType_Color*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_FloorplanObjectType_Color* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_FloorplanDesignObjType_Color[32];
sprintf(tmp_PK_FloorplanDesignObjType_Color, "%li", key.pk_PK_FloorplanDesignObjType_Color);


string condition;
condition = condition + "PK_FloorplanDesignObjType_Color=" + tmp_PK_FloorplanDesignObjType_Color;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_FloorplanDesignObjType_Color="+pRow->PK_FloorplanDesignObjType_Color_asSQL()+", FK_FloorplanDesignObjType="+pRow->FK_FloorplanDesignObjType_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Color="+pRow->Color_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update FloorplanObjectType_Color set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_FloorplanObjectType_Color*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_FloorplanObjectType_Color*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_FloorplanDesignObjType_Color[32];
sprintf(tmp_PK_FloorplanDesignObjType_Color, "%li", key.pk_PK_FloorplanDesignObjType_Color);


string condition;
condition = condition + "PK_FloorplanDesignObjType_Color=" + tmp_PK_FloorplanDesignObjType_Color;

	
		string query = "delete from FloorplanObjectType_Color where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_FloorplanObjectType_Color::GetRows(string where_statement,vector<class Row_FloorplanObjectType_Color*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from FloorplanObjectType_Color where " + where_statement;
		
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

		Row_FloorplanObjectType_Color *pRow = new Row_FloorplanObjectType_Color(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_FloorplanDesignObjType_Color = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_FloorplanDesignObjType_Color));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_FloorplanDesignObjType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_FloorplanDesignObjType));
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
pRow->m_Color = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Color));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Modification_RecordInfo = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_FloorplanObjectType_Color::Key, Row_FloorplanObjectType_Color*, Table_FloorplanObjectType_Color::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_FloorplanObjectType_Color* Table_FloorplanObjectType_Color::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_FloorplanObjectType_Color *pRow = new Row_FloorplanObjectType_Color(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_FloorplanObjectType_Color* Table_FloorplanObjectType_Color::GetRow(long int in_PK_FloorplanDesignObjType_Color)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_FloorplanDesignObjType_Color);

	map<Key, Row_FloorplanObjectType_Color*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_FloorplanObjectType_Color* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_FloorplanObjectType_Color* Table_FloorplanObjectType_Color::FetchRow(Table_FloorplanObjectType_Color::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_FloorplanDesignObjType_Color[32];
sprintf(tmp_PK_FloorplanDesignObjType_Color, "%li", key.pk_PK_FloorplanDesignObjType_Color);


string condition;
condition = condition + "PK_FloorplanDesignObjType_Color=" + tmp_PK_FloorplanDesignObjType_Color;


	string query = "select * from FloorplanObjectType_Color where " + condition;		

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

	Row_FloorplanObjectType_Color *pRow = new Row_FloorplanObjectType_Color(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_FloorplanDesignObjType_Color = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_FloorplanDesignObjType_Color));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_FloorplanDesignObjType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_FloorplanDesignObjType));
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
pRow->m_Color = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Color));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Modification_RecordInfo = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}







