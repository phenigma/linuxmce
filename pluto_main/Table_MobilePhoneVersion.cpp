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
#include "Table_MobilePhoneVersion.h"



void Database_pluto_main::CreateTable_MobilePhoneVersion()
{
	tblMobilePhoneVersion = new Table_MobilePhoneVersion(this);
}

void Database_pluto_main::DeleteTable_MobilePhoneVersion()
{
	delete tblMobilePhoneVersion;
}

Table_MobilePhoneVersion::~Table_MobilePhoneVersion()
{
	map<Table_MobilePhoneVersion::Key, class Row_MobilePhoneVersion*, Table_MobilePhoneVersion::Key_Less>::iterator it;
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


void Row_MobilePhoneVersion::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_MobilePhoneVersion*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_MobilePhoneVersion::Key key(this);					
			map<Table_MobilePhoneVersion::Key, Row_MobilePhoneVersion*, Table_MobilePhoneVersion::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_MobilePhoneVersion::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_MobilePhoneVersion::Key key(this);		
		Row_MobilePhoneVersion *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MobilePhoneVersion::Row_MobilePhoneVersion(Table_MobilePhoneVersion *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MobilePhoneVersion::SetDefaultValues()
{
	m_PK_MobilePhoneVersion = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MobilePhoneVersion::PK_MobilePhoneVersion_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_MobilePhoneVersion;}
string Row_MobilePhoneVersion::Description_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Description;}
string Row_MobilePhoneVersion::Define_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Define;}
string Row_MobilePhoneVersion::Models_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Models;}
long int Row_MobilePhoneVersion::Height_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Height;}
long int Row_MobilePhoneVersion::Width_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Width;}

		
void Row_MobilePhoneVersion::PK_MobilePhoneVersion_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_MobilePhoneVersion = val; is_modified=true; is_null[0]=false;}
void Row_MobilePhoneVersion::Description_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_MobilePhoneVersion::Define_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_MobilePhoneVersion::Models_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Models = val; is_modified=true; is_null[3]=false;}
void Row_MobilePhoneVersion::Height_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Height = val; is_modified=true; is_null[4]=false;}
void Row_MobilePhoneVersion::Width_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Width = val; is_modified=true; is_null[5]=false;}

		
bool Row_MobilePhoneVersion::Define_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_MobilePhoneVersion::Models_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_MobilePhoneVersion::Height_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}
bool Row_MobilePhoneVersion::Width_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}

			
void Row_MobilePhoneVersion::Define_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_MobilePhoneVersion::Models_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_MobilePhoneVersion::Height_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
void Row_MobilePhoneVersion::Width_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
	

string Row_MobilePhoneVersion::PK_MobilePhoneVersion_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_MobilePhoneVersion);

return buf;
}

string Row_MobilePhoneVersion::Description_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Description.c_str(), (unsigned long) m_Description.size());
return string()+"\""+buf+"\"";
}

string Row_MobilePhoneVersion::Define_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[61];
mysql_real_escape_string(table->database->db_handle, buf, m_Define.c_str(), (unsigned long) m_Define.size());
return string()+"\""+buf+"\"";
}

string Row_MobilePhoneVersion::Models_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Models.c_str(), (unsigned long) m_Models.size());
return string()+"\""+buf+"\"";
}

string Row_MobilePhoneVersion::Height_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Height);

return buf;
}

string Row_MobilePhoneVersion::Width_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Width);

return buf;
}




Table_MobilePhoneVersion::Key::Key(long int in_PK_MobilePhoneVersion)
{
			pk_PK_MobilePhoneVersion = in_PK_MobilePhoneVersion;
	
}

Table_MobilePhoneVersion::Key::Key(Row_MobilePhoneVersion *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_MobilePhoneVersion = pRow->m_PK_MobilePhoneVersion;
	
}		

bool Table_MobilePhoneVersion::Key_Less::operator()(const Table_MobilePhoneVersion::Key &key1, const Table_MobilePhoneVersion::Key &key2) const
{
			if (key1.pk_PK_MobilePhoneVersion!=key2.pk_PK_MobilePhoneVersion)
return key1.pk_PK_MobilePhoneVersion<key2.pk_PK_MobilePhoneVersion;
else
return false;	
}	

void Table_MobilePhoneVersion::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_MobilePhoneVersion*>::iterator i = addedRows.begin();
	
		Row_MobilePhoneVersion *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_MobilePhoneVersion_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->Models_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->Width_asSQL();

	
		string query = "insert into MobilePhoneVersion (PK_MobilePhoneVersion, Description, Define, Models, Height, Width) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_MobilePhoneVersion=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_MobilePhoneVersion*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_MobilePhoneVersion* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_MobilePhoneVersion[32];
sprintf(tmp_PK_MobilePhoneVersion, "%li", key.pk_PK_MobilePhoneVersion);


string condition;
condition = condition + "PK_MobilePhoneVersion=" + tmp_PK_MobilePhoneVersion;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_MobilePhoneVersion="+pRow->PK_MobilePhoneVersion_asSQL()+", Description="+pRow->Description_asSQL()+", Define="+pRow->Define_asSQL()+", Models="+pRow->Models_asSQL()+", Height="+pRow->Height_asSQL()+", Width="+pRow->Width_asSQL();

	
		string query = "update MobilePhoneVersion set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_MobilePhoneVersion*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_MobilePhoneVersion*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_MobilePhoneVersion[32];
sprintf(tmp_PK_MobilePhoneVersion, "%li", key.pk_PK_MobilePhoneVersion);


string condition;
condition = condition + "PK_MobilePhoneVersion=" + tmp_PK_MobilePhoneVersion;

	
		string query = "delete from MobilePhoneVersion where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_MobilePhoneVersion::GetRows(string where_statement,vector<class Row_MobilePhoneVersion*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from MobilePhoneVersion " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from MobilePhoneVersion where " + where_statement;
		
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

		Row_MobilePhoneVersion *pRow = new Row_MobilePhoneVersion(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MobilePhoneVersion = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MobilePhoneVersion));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Models = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Models = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Height));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Width));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_MobilePhoneVersion::Key, Row_MobilePhoneVersion*, Table_MobilePhoneVersion::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_MobilePhoneVersion* Table_MobilePhoneVersion::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_MobilePhoneVersion *pRow = new Row_MobilePhoneVersion(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MobilePhoneVersion* Table_MobilePhoneVersion::GetRow(long int in_PK_MobilePhoneVersion)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_MobilePhoneVersion);

	map<Key, Row_MobilePhoneVersion*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_MobilePhoneVersion* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MobilePhoneVersion* Table_MobilePhoneVersion::FetchRow(Table_MobilePhoneVersion::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_MobilePhoneVersion[32];
sprintf(tmp_PK_MobilePhoneVersion, "%li", key.pk_PK_MobilePhoneVersion);


string condition;
condition = condition + "PK_MobilePhoneVersion=" + tmp_PK_MobilePhoneVersion;


	string query = "select * from MobilePhoneVersion where " + condition;		

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

	Row_MobilePhoneVersion *pRow = new Row_MobilePhoneVersion(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MobilePhoneVersion = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MobilePhoneVersion));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Models = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Models = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Height));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Width));
}



	mysql_free_result(res);			
	
	return pRow;						
}







