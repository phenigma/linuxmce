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
#include "Table_PageSetup.h"
#include "Table_PageSetup.h"
#include "Table_Package.h"

#include "Table_DeviceTemplate_PageSetup.h"
#include "Table_PageSetup.h"


void Database_pluto_main::CreateTable_PageSetup()
{
	tblPageSetup = new Table_PageSetup(this);
}

void Database_pluto_main::DeleteTable_PageSetup()
{
	delete tblPageSetup;
}

Table_PageSetup::~Table_PageSetup()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_PageSetup *pRow = (Row_PageSetup *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_PageSetup *pRow = (Row_PageSetup *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_PageSetup::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_PageSetup *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_PageSetup *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_PageSetup);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_PageSetup::Reload()
{
	Row_PageSetup *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_PageSetup);
		Row_PageSetup *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_PageSetup::Row_PageSetup(Table_PageSetup *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_PageSetup::SetDefaultValues()
{
	m_PK_PageSetup = 0;
is_null[0] = false;
is_null[1] = true;
m_Website = 0;
is_null[2] = false;
m_OrderNum = 0;
is_null[3] = false;
is_null[4] = true;
m_Description = "";
is_null[5] = false;
m_pageURL = "";
is_null[6] = false;
m_showInTopMenu = 0;
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

long int Row_PageSetup::PK_PageSetup_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_PageSetup;}
long int Row_PageSetup::FK_PageSetup_Parent_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_PageSetup_Parent;}
short int Row_PageSetup::Website_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Website;}
long int Row_PageSetup::OrderNum_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_OrderNum;}
long int Row_PageSetup::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
string Row_PageSetup::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_PageSetup::pageURL_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_pageURL;}
short int Row_PageSetup::showInTopMenu_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_showInTopMenu;}
string Row_PageSetup::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_PageSetup::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_PageSetup::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_PageSetup::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_PageSetup::PK_PageSetup_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_PageSetup = val; is_modified=true; is_null[0]=false;}
void Row_PageSetup::FK_PageSetup_Parent_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_PageSetup_Parent = val; is_modified=true; is_null[1]=false;}
void Row_PageSetup::Website_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Website = val; is_modified=true; is_null[2]=false;}
void Row_PageSetup::OrderNum_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_OrderNum = val; is_modified=true; is_null[3]=false;}
void Row_PageSetup::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[4]=false;}
void Row_PageSetup::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_PageSetup::pageURL_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_pageURL = val; is_modified=true; is_null[6]=false;}
void Row_PageSetup::showInTopMenu_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_showInTopMenu = val; is_modified=true; is_null[7]=false;}
void Row_PageSetup::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[8]=false;}
void Row_PageSetup::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[9]=false;}
void Row_PageSetup::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[10]=false;}
void Row_PageSetup::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[11]=false;}

		
bool Row_PageSetup::FK_PageSetup_Parent_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_PageSetup::OrderNum_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_PageSetup::FK_Package_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_PageSetup::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_PageSetup::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}
bool Row_PageSetup::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[11];}

			
void Row_PageSetup::FK_PageSetup_Parent_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_PageSetup::OrderNum_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_PageSetup::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_PageSetup::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_PageSetup::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
void Row_PageSetup::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[11]=val;}
	

string Row_PageSetup::PK_PageSetup_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_PageSetup);

return buf;
}

string Row_PageSetup::FK_PageSetup_Parent_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_PageSetup_Parent);

return buf;
}

string Row_PageSetup::Website_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Website);

return buf;
}

string Row_PageSetup::OrderNum_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_OrderNum);

return buf;
}

string Row_PageSetup::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_PageSetup::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_PageSetup::pageURL_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[511];
mysql_real_escape_string(table->database->db_handle, buf, m_pageURL.c_str(), (unsigned long) m_pageURL.size());
return string()+"\""+buf+"\"";
}

string Row_PageSetup::showInTopMenu_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_showInTopMenu);

return buf;
}

string Row_PageSetup::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_PageSetup::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_PageSetup::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_PageSetup::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_PageSetup::Key::Key(long int in_PK_PageSetup)
{
			pk_PK_PageSetup = in_PK_PageSetup;
	
}

Table_PageSetup::Key::Key(Row_PageSetup *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_PageSetup = pRow->m_PK_PageSetup;
	
}		

bool Table_PageSetup::Key_Less::operator()(const Table_PageSetup::Key &key1, const Table_PageSetup::Key &key2) const
{
			if (key1.pk_PK_PageSetup!=key2.pk_PK_PageSetup)
return key1.pk_PK_PageSetup<key2.pk_PK_PageSetup;
else
return false;	
}	

void Table_PageSetup::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_PageSetup *pRow = (Row_PageSetup *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_PageSetup_asSQL()+", "+pRow->FK_PageSetup_Parent_asSQL()+", "+pRow->Website_asSQL()+", "+pRow->OrderNum_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->pageURL_asSQL()+", "+pRow->showInTopMenu_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into PageSetup (PK_PageSetup, FK_PageSetup_Parent, Website, OrderNum, FK_Package, Description, pageURL, showInTopMenu, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_PageSetup=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_PageSetup);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_PageSetup* pRow = (Row_PageSetup*) (*i).second;	
		SingleLongKey key(pRow->m_PK_PageSetup);

		char tmp_PK_PageSetup[32];
sprintf(tmp_PK_PageSetup, "%li", key.pk);


string condition;
condition = condition + "PK_PageSetup=" + tmp_PK_PageSetup;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_PageSetup="+pRow->PK_PageSetup_asSQL()+", FK_PageSetup_Parent="+pRow->FK_PageSetup_Parent_asSQL()+", Website="+pRow->Website_asSQL()+", OrderNum="+pRow->OrderNum_asSQL()+", FK_Package="+pRow->FK_Package_asSQL()+", Description="+pRow->Description_asSQL()+", pageURL="+pRow->pageURL_asSQL()+", showInTopMenu="+pRow->showInTopMenu_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update PageSetup set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_PageSetup* pRow = (Row_PageSetup*) (*i).second;	

		char tmp_PK_PageSetup[32];
sprintf(tmp_PK_PageSetup, "%li", key.pk);


string condition;
condition = condition + "PK_PageSetup=" + tmp_PK_PageSetup;

	
		string query = "delete from PageSetup where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_PageSetup::GetRows(string where_statement,vector<class Row_PageSetup*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from PageSetup " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from PageSetup where " + where_statement;
		
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

		Row_PageSetup *pRow = new Row_PageSetup(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PageSetup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PageSetup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_PageSetup_Parent = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_PageSetup_Parent));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Website = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Website));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_OrderNum = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_OrderNum));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Package));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_pageURL = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_pageURL = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_showInTopMenu = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_showInTopMenu));
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

		SingleLongKey key(pRow->m_PK_PageSetup);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_PageSetup *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_PageSetup* Table_PageSetup::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_PageSetup *pRow = new Row_PageSetup(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_PageSetup* Table_PageSetup::GetRow(long int in_PK_PageSetup)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_PageSetup);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_PageSetup*) (*i).second;
	//we have to fetch row
	Row_PageSetup* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_PageSetup* Table_PageSetup::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_PageSetup[32];
sprintf(tmp_PK_PageSetup, "%li", key.pk);


string condition;
condition = condition + "PK_PageSetup=" + tmp_PK_PageSetup;


	string query = "select * from PageSetup where " + condition;		

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

	Row_PageSetup *pRow = new Row_PageSetup(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PageSetup = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PageSetup));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_PageSetup_Parent = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_PageSetup_Parent));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Website = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Website));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_OrderNum = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_OrderNum));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Package));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_pageURL = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_pageURL = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_showInTopMenu = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_showInTopMenu));
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


class Row_PageSetup* Row_PageSetup::FK_PageSetup_Parent_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_PageSetup *pTable = table->database->PageSetup_get();
return pTable->GetRow(m_FK_PageSetup_Parent);
}
class Row_Package* Row_PageSetup::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}


void Row_PageSetup::DeviceTemplate_PageSetup_FK_PageSetup_getrows(vector <class Row_DeviceTemplate_PageSetup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_DeviceTemplate_PageSetup *pTable = table->database->DeviceTemplate_PageSetup_get();
pTable->GetRows("FK_PageSetup=" + StringUtils::itos(m_PK_PageSetup),rows);
}
void Row_PageSetup::PageSetup_FK_PageSetup_Parent_getrows(vector <class Row_PageSetup*> *rows)
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_PageSetup *pTable = table->database->PageSetup_get();
pTable->GetRows("FK_PageSetup_Parent=" + StringUtils::itos(m_PK_PageSetup),rows);
}



