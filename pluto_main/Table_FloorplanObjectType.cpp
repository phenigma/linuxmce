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
#include "Table_FloorplanObjectType.h"
#include "Table_FloorplanType.h"
#include "Table_DesignObj.h"



void Database_pluto_main::CreateTable_FloorplanObjectType()
{
	tblFloorplanObjectType = new Table_FloorplanObjectType(this);
}

void Database_pluto_main::DeleteTable_FloorplanObjectType()
{
	delete tblFloorplanObjectType;
}

Table_FloorplanObjectType::~Table_FloorplanObjectType()
{
	map<Table_FloorplanObjectType::Key, class Row_FloorplanObjectType*, Table_FloorplanObjectType::Key_Less>::iterator it;
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


void Row_FloorplanObjectType::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_FloorplanObjectType*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_FloorplanObjectType::Key key(this);					
			map<Table_FloorplanObjectType::Key, Row_FloorplanObjectType*, Table_FloorplanObjectType::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_FloorplanObjectType::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_FloorplanObjectType::Key key(this);		
		Row_FloorplanObjectType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_FloorplanObjectType::Row_FloorplanObjectType(Table_FloorplanObjectType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_FloorplanObjectType::SetDefaultValues()
{
	m_PK_FloorplanDesignObjType = 0;
is_null[0] = false;
m_FK_FloorplanType = 0;
is_null[1] = false;
m_Description = "";
is_null[2] = false;
m_Define = "";
is_null[3] = false;
m_Direction = "";
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
m_FillX = 0;
is_null[7] = false;
m_FillY = 0;
is_null[8] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[9] = false;
m_IsNew_RecordInfo = 1;
is_null[10] = false;
m_IsDeleted_RecordInfo = 0;
is_null[11] = false;
is_null[12] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_FloorplanObjectType::PK_FloorplanDesignObjType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_FloorplanDesignObjType;}
long int Row_FloorplanObjectType::FK_FloorplanType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_FloorplanType;}
string Row_FloorplanObjectType::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_FloorplanObjectType::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_FloorplanObjectType::Direction_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Direction;}
long int Row_FloorplanObjectType::FK_DesignObj_Control_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj_Control;}
string Row_FloorplanObjectType::Filename_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Filename;}
long int Row_FloorplanObjectType::FillX_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FillX;}
long int Row_FloorplanObjectType::FillY_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FillY;}
string Row_FloorplanObjectType::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_FloorplanObjectType::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_FloorplanObjectType::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_FloorplanObjectType::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_FloorplanObjectType::PK_FloorplanDesignObjType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_FloorplanDesignObjType = val; is_modified=true; is_null[0]=false;}
void Row_FloorplanObjectType::FK_FloorplanType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_FloorplanType = val; is_modified=true; is_null[1]=false;}
void Row_FloorplanObjectType::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_FloorplanObjectType::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[3]=false;}
void Row_FloorplanObjectType::Direction_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Direction = val; is_modified=true; is_null[4]=false;}
void Row_FloorplanObjectType::FK_DesignObj_Control_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj_Control = val; is_modified=true; is_null[5]=false;}
void Row_FloorplanObjectType::Filename_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Filename = val; is_modified=true; is_null[6]=false;}
void Row_FloorplanObjectType::FillX_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FillX = val; is_modified=true; is_null[7]=false;}
void Row_FloorplanObjectType::FillY_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FillY = val; is_modified=true; is_null[8]=false;}
void Row_FloorplanObjectType::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[9]=false;}
void Row_FloorplanObjectType::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_FloorplanObjectType::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[11]=false;}
void Row_FloorplanObjectType::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[12]=false;}

		
bool Row_FloorplanObjectType::FK_DesignObj_Control_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_FloorplanObjectType::Filename_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_FloorplanObjectType::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_FloorplanObjectType::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}
bool Row_FloorplanObjectType::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[12];}

			
void Row_FloorplanObjectType::FK_DesignObj_Control_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_FloorplanObjectType::Filename_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_FloorplanObjectType::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_FloorplanObjectType::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
void Row_FloorplanObjectType::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[12]=val;}
	

string Row_FloorplanObjectType::PK_FloorplanDesignObjType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_FloorplanDesignObjType);

return buf;
}

string Row_FloorplanObjectType::FK_FloorplanType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_FloorplanType);

return buf;
}

string Row_FloorplanObjectType::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType::Direction_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[9];
mysql_real_escape_string(table->database->db_handle, buf, m_Direction.c_str(), (unsigned long) m_Direction.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType::FK_DesignObj_Control_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Control);

return buf;
}

string Row_FloorplanObjectType::Filename_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[401];
mysql_real_escape_string(table->database->db_handle, buf, m_Filename.c_str(), (unsigned long) m_Filename.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType::FillX_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FillX);

return buf;
}

string Row_FloorplanObjectType::FillY_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FillY);

return buf;
}

string Row_FloorplanObjectType::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_FloorplanObjectType::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_FloorplanObjectType::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_FloorplanObjectType::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_FloorplanObjectType::Key::Key(long int in_PK_FloorplanDesignObjType)
{
			pk_PK_FloorplanDesignObjType = in_PK_FloorplanDesignObjType;
	
}

Table_FloorplanObjectType::Key::Key(Row_FloorplanObjectType *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_FloorplanDesignObjType = pRow->m_PK_FloorplanDesignObjType;
	
}		

bool Table_FloorplanObjectType::Key_Less::operator()(const Table_FloorplanObjectType::Key &key1, const Table_FloorplanObjectType::Key &key2) const
{
			if (key1.pk_PK_FloorplanDesignObjType!=key2.pk_PK_FloorplanDesignObjType)
return key1.pk_PK_FloorplanDesignObjType<key2.pk_PK_FloorplanDesignObjType;
else
return false;	
}	

void Table_FloorplanObjectType::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_FloorplanObjectType*>::iterator i = addedRows.begin();
	
		Row_FloorplanObjectType *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_FloorplanDesignObjType_asSQL()+", "+pRow->FK_FloorplanType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Direction_asSQL()+", "+pRow->FK_DesignObj_Control_asSQL()+", "+pRow->Filename_asSQL()+", "+pRow->FillX_asSQL()+", "+pRow->FillY_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into FloorplanObjectType (PK_FloorplanDesignObjType, FK_FloorplanType, Description, Define, Direction, FK_DesignObj_Control, Filename, FillX, FillY, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_FloorplanDesignObjType=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_FloorplanObjectType*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_FloorplanObjectType* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_FloorplanDesignObjType[32];
sprintf(tmp_PK_FloorplanDesignObjType, "%li", key.pk_PK_FloorplanDesignObjType);


string condition;
condition = condition + "PK_FloorplanDesignObjType=" + tmp_PK_FloorplanDesignObjType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_FloorplanDesignObjType="+pRow->PK_FloorplanDesignObjType_asSQL()+", FK_FloorplanType="+pRow->FK_FloorplanType_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Direction="+pRow->Direction_asSQL()+", FK_DesignObj_Control="+pRow->FK_DesignObj_Control_asSQL()+", Filename="+pRow->Filename_asSQL()+", FillX="+pRow->FillX_asSQL()+", FillY="+pRow->FillY_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update FloorplanObjectType set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_FloorplanObjectType*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_FloorplanObjectType*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_FloorplanDesignObjType[32];
sprintf(tmp_PK_FloorplanDesignObjType, "%li", key.pk_PK_FloorplanDesignObjType);


string condition;
condition = condition + "PK_FloorplanDesignObjType=" + tmp_PK_FloorplanDesignObjType;

	
		string query = "delete from FloorplanObjectType where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_FloorplanObjectType::GetRows(string where_statement,vector<class Row_FloorplanObjectType*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from FloorplanObjectType where " + where_statement;
		
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

		Row_FloorplanObjectType *pRow = new Row_FloorplanObjectType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_FloorplanDesignObjType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_FloorplanDesignObjType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_FloorplanType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_FloorplanType));
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
pRow->m_Direction = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Direction = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_DesignObj_Control = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_DesignObj_Control));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Filename = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FillX = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FillX));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FillY = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FillY));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Modification_RecordInfo = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_FloorplanObjectType::Key, Row_FloorplanObjectType*, Table_FloorplanObjectType::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_FloorplanObjectType* Table_FloorplanObjectType::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_FloorplanObjectType *pRow = new Row_FloorplanObjectType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_FloorplanObjectType* Table_FloorplanObjectType::GetRow(long int in_PK_FloorplanDesignObjType)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_FloorplanDesignObjType);

	map<Key, Row_FloorplanObjectType*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_FloorplanObjectType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_FloorplanObjectType* Table_FloorplanObjectType::FetchRow(Table_FloorplanObjectType::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_FloorplanDesignObjType[32];
sprintf(tmp_PK_FloorplanDesignObjType, "%li", key.pk_PK_FloorplanDesignObjType);


string condition;
condition = condition + "PK_FloorplanDesignObjType=" + tmp_PK_FloorplanDesignObjType;


	string query = "select * from FloorplanObjectType where " + condition;		

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

	Row_FloorplanObjectType *pRow = new Row_FloorplanObjectType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_FloorplanDesignObjType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_FloorplanDesignObjType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_FloorplanType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_FloorplanType));
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
pRow->m_Direction = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Direction = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_DesignObj_Control = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_DesignObj_Control));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Filename = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FillX = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FillX));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FillY = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FillY));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Modification_RecordInfo = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_FloorplanType* Row_FloorplanObjectType::FK_FloorplanType_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_FloorplanType *pTable = table->database->FloorplanType_get();
return pTable->GetRow(m_FK_FloorplanType);
}
class Row_DesignObj* Row_FloorplanObjectType::FK_DesignObj_Control_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Control);
}





