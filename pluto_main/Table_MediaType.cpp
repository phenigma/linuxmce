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
#include "Table_MediaType.h"
#include "Table_DesignObj.h"

#include "Table_DeviceTemplate_MediaType.h"
#include "Table_MediaType_Broadcast.h"


void Database_pluto_main::CreateTable_MediaType()
{
	tblMediaType = new Table_MediaType(this);
}

void Database_pluto_main::DeleteTable_MediaType()
{
	delete tblMediaType;
}

Table_MediaType::~Table_MediaType()
{
	map<Table_MediaType::Key, class Row_MediaType*, Table_MediaType::Key_Less>::iterator it;
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


void Row_MediaType::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_MediaType*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_MediaType::Key key(this);					
			map<Table_MediaType::Key, Row_MediaType*, Table_MediaType::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_MediaType::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_MediaType::Key key(this);		
		Row_MediaType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MediaType::Row_MediaType(Table_MediaType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MediaType::SetDefaultValues()
{
	m_PK_MediaType = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
m_DCEAware = 0;
is_null[4] = false;
is_null[5] = true;
m_Subdirectory = "";
is_null[6] = false;
m_IsExternalTransmission = 0;
is_null[7] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[8] = false;
m_IsNew_RecordInfo = 1;
is_null[9] = false;
m_IsDeleted_RecordInfo = 0;
is_null[10] = false;
is_null[11] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MediaType::PK_MediaType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_MediaType;}
string Row_MediaType::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_MediaType::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
long int Row_MediaType::FK_DesignObj_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_DesignObj;}
short int Row_MediaType::DCEAware_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DCEAware;}
string Row_MediaType::Extensions_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Extensions;}
string Row_MediaType::Subdirectory_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Subdirectory;}
short int Row_MediaType::IsExternalTransmission_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsExternalTransmission;}
string Row_MediaType::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_MediaType::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_MediaType::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_MediaType::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_MediaType::PK_MediaType_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_MediaType = val; is_modified=true; is_null[0]=false;}
void Row_MediaType::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[1]=false;}
void Row_MediaType::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[2]=false;}
void Row_MediaType::FK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_DesignObj = val; is_modified=true; is_null[3]=false;}
void Row_MediaType::DCEAware_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DCEAware = val; is_modified=true; is_null[4]=false;}
void Row_MediaType::Extensions_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Extensions = val; is_modified=true; is_null[5]=false;}
void Row_MediaType::Subdirectory_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Subdirectory = val; is_modified=true; is_null[6]=false;}
void Row_MediaType::IsExternalTransmission_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsExternalTransmission = val; is_modified=true; is_null[7]=false;}
void Row_MediaType::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_MediaType::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[9]=false;}
void Row_MediaType::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_MediaType::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[11]=false;}

		
bool Row_MediaType::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_MediaType::Description_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_MediaType::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_MediaType::Extensions_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_MediaType::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_MediaType::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_MediaType::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_MediaType::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_MediaType::Description_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_MediaType::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_MediaType::Extensions_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_MediaType::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_MediaType::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_MediaType::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_MediaType::PK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_MediaType);

return buf;
}

string Row_MediaType::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[41];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_MediaType::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[101];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_MediaType::FK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj);

return buf;
}

string Row_MediaType::DCEAware_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DCEAware);

return buf;
}

string Row_MediaType::Extensions_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[81];
mysql_real_escape_string(table->database->db_handle, buf, m_Extensions.c_str(), (unsigned long) m_Extensions.size());
return string()+"\""+buf+"\"";
}

string Row_MediaType::Subdirectory_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[51];
mysql_real_escape_string(table->database->db_handle, buf, m_Subdirectory.c_str(), (unsigned long) m_Subdirectory.size());
return string()+"\""+buf+"\"";
}

string Row_MediaType::IsExternalTransmission_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsExternalTransmission);

return buf;
}

string Row_MediaType::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_MediaType::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_MediaType::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_MediaType::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_MediaType::Key::Key(long int in_PK_MediaType)
{
			pk_PK_MediaType = in_PK_MediaType;
	
}

Table_MediaType::Key::Key(Row_MediaType *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_MediaType = pRow->m_PK_MediaType;
	
}		

bool Table_MediaType::Key_Less::operator()(const Table_MediaType::Key &key1, const Table_MediaType::Key &key2) const
{
			if (key1.pk_PK_MediaType!=key2.pk_PK_MediaType)
return key1.pk_PK_MediaType<key2.pk_PK_MediaType;
else
return false;	
}	

void Table_MediaType::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_MediaType*>::iterator i = addedRows.begin();
	
		Row_MediaType *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_MediaType_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->DCEAware_asSQL()+", "+pRow->Extensions_asSQL()+", "+pRow->Subdirectory_asSQL()+", "+pRow->IsExternalTransmission_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into MediaType (PK_MediaType, Define, Description, FK_DesignObj, DCEAware, Extensions, Subdirectory, IsExternalTransmission, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_MediaType=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_MediaType*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_MediaType* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_MediaType[32];
sprintf(tmp_PK_MediaType, "%li", key.pk_PK_MediaType);


string condition;
condition = condition + "PK_MediaType=" + tmp_PK_MediaType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_MediaType="+pRow->PK_MediaType_asSQL()+", Define="+pRow->Define_asSQL()+", Description="+pRow->Description_asSQL()+", FK_DesignObj="+pRow->FK_DesignObj_asSQL()+", DCEAware="+pRow->DCEAware_asSQL()+", Extensions="+pRow->Extensions_asSQL()+", Subdirectory="+pRow->Subdirectory_asSQL()+", IsExternalTransmission="+pRow->IsExternalTransmission_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update MediaType set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_MediaType*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_MediaType*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_MediaType[32];
sprintf(tmp_PK_MediaType, "%li", key.pk_PK_MediaType);


string condition;
condition = condition + "PK_MediaType=" + tmp_PK_MediaType;

	
		string query = "delete from MediaType where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_MediaType::GetRows(string where_statement,vector<class Row_MediaType*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from MediaType where " + where_statement;
		
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

		Row_MediaType *pRow = new Row_MediaType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Define = string(row[1],lengths[1]);
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
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DCEAware = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_DCEAware));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Extensions = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Extensions = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Subdirectory = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Subdirectory = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsExternalTransmission = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsExternalTransmission));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Modification_RecordInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_MediaType::Key, Row_MediaType*, Table_MediaType::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_MediaType* Table_MediaType::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_MediaType *pRow = new Row_MediaType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MediaType* Table_MediaType::GetRow(long int in_PK_MediaType)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_MediaType);

	map<Key, Row_MediaType*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_MediaType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MediaType* Table_MediaType::FetchRow(Table_MediaType::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_MediaType[32];
sprintf(tmp_PK_MediaType, "%li", key.pk_PK_MediaType);


string condition;
condition = condition + "PK_MediaType=" + tmp_PK_MediaType;


	string query = "select * from MediaType where " + condition;		

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

	Row_MediaType *pRow = new Row_MediaType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Define = string(row[1],lengths[1]);
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
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DCEAware = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_DCEAware));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Extensions = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Extensions = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Subdirectory = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Subdirectory = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsExternalTransmission = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsExternalTransmission));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Modification_RecordInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_DesignObj* Row_MediaType::FK_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj);
}


void Row_MediaType::DeviceTemplate_MediaType_FK_MediaType_getrows(vector <class Row_DeviceTemplate_MediaType*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_MediaType *pTable = table->database->DeviceTemplate_MediaType_get();
pTable->GetRows("FK_MediaType=" + StringUtils::itos(m_PK_MediaType),rows);
}
void Row_MediaType::MediaType_Broadcast_FK_MediaType_getrows(vector <class Row_MediaType_Broadcast*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_MediaType_Broadcast *pTable = table->database->MediaType_Broadcast_get();
pTable->GetRows("FK_MediaType=" + StringUtils::itos(m_PK_MediaType),rows);
}



