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
#include "Table_Package_Users.h"
#include "Table_Package.h"
#include "Table_Users.h"



void Database_pluto_main::CreateTable_Package_Users()
{
	tblPackage_Users = new Table_Package_Users(this);
}

void Database_pluto_main::DeleteTable_Package_Users()
{
	delete tblPackage_Users;
}

Table_Package_Users::~Table_Package_Users()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Package_Users *pRow = (Row_Package_Users *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Package_Users *pRow = (Row_Package_Users *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Package_Users::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Package_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Package_Users *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Package,pRow->m_FK_Users);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Package_Users::Reload()
{
	Row_Package_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Package,pRow->m_FK_Users);
		Row_Package_Users *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Users::Row_Package_Users(Table_Package_Users *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Users::SetDefaultValues()
{
	m_FK_Package = 0;
is_null[0] = false;
m_FK_Users = 0;
is_null[1] = false;
m_Administrator = 0;
is_null[2] = false;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Users::FK_Package_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Package;}
long int Row_Package_Users::FK_Users_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users;}
short int Row_Package_Users::Administrator_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Administrator;}
string Row_Package_Users::Comments_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Comments;}

		
void Row_Package_Users::FK_Package_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Package = val; is_modified=true; is_null[0]=false;}
void Row_Package_Users::FK_Users_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users = val; is_modified=true; is_null[1]=false;}
void Row_Package_Users::Administrator_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Administrator = val; is_modified=true; is_null[2]=false;}
void Row_Package_Users::Comments_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Comments = val; is_modified=true; is_null[3]=false;}

		
bool Row_Package_Users::Administrator_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_Package_Users::Comments_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}

			
void Row_Package_Users::Administrator_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_Package_Users::Comments_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
	

string Row_Package_Users::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Package_Users::FK_Users_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users);

return buf;
}

string Row_Package_Users::Administrator_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Administrator);

return buf;
}

string Row_Package_Users::Comments_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Comments.c_str(), (unsigned long) m_Comments.size());
return string()+"\""+buf+"\"";
}




Table_Package_Users::Key::Key(long int in_FK_Package, long int in_FK_Users)
{
			pk_FK_Package = in_FK_Package;
pk_FK_Users = in_FK_Users;
	
}

Table_Package_Users::Key::Key(Row_Package_Users *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_FK_Package = pRow->m_FK_Package;
pk_FK_Users = pRow->m_FK_Users;
	
}		

bool Table_Package_Users::Key_Less::operator()(const Table_Package_Users::Key &key1, const Table_Package_Users::Key &key2) const
{
			if (key1.pk_FK_Package!=key2.pk_FK_Package)
return key1.pk_FK_Package<key2.pk_FK_Package;
else
if (key1.pk_FK_Users!=key2.pk_FK_Users)
return key1.pk_FK_Users<key2.pk_FK_Users;
else
return false;	
}	

void Table_Package_Users::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Package_Users *pRow = (Row_Package_Users *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Package_asSQL()+", "+pRow->FK_Users_asSQL()+", "+pRow->Administrator_asSQL()+", "+pRow->Comments_asSQL();

	
		string query = "insert into Package_Users (FK_Package, FK_Users, Administrator, Comments) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Package,pRow->m_FK_Users);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Package_Users* pRow = (Row_Package_Users*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Package,pRow->m_FK_Users);

		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk1);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk2);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Users=" + tmp_FK_Users;
	
			
		
string update_values_list;
update_values_list = update_values_list + "FK_Package="+pRow->FK_Package_asSQL()+", FK_Users="+pRow->FK_Users_asSQL()+", Administrator="+pRow->Administrator_asSQL()+", Comments="+pRow->Comments_asSQL();

	
		string query = "update Package_Users set " + update_values_list + " where " + condition;
			
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
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Package_Users* pRow = (Row_Package_Users*) (*i).second;	

		char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk1);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk2);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Users=" + tmp_FK_Users;

	
		string query = "delete from Package_Users where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Package_Users::GetRows(string where_statement,vector<class Row_Package_Users*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Package_Users " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Package_Users where " + where_statement;
		
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

		Row_Package_Users *pRow = new Row_Package_Users(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Administrator = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Administrator));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Comments = string(row[3],lengths[3]);
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Package,pRow->m_FK_Users);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Package_Users *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Package_Users* Table_Package_Users::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Package_Users *pRow = new Row_Package_Users(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Users* Table_Package_Users::GetRow(long int in_FK_Package, long int in_FK_Users)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	DoubleLongKey row_key(in_FK_Package, in_FK_Users);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Package_Users*) (*i).second;
	//we have to fetch row
	Row_Package_Users* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Users* Table_Package_Users::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_FK_Package[32];
sprintf(tmp_FK_Package, "%li", key.pk1);

char tmp_FK_Users[32];
sprintf(tmp_FK_Users, "%li", key.pk2);


string condition;
condition = condition + "FK_Package=" + tmp_FK_Package+" AND "+"FK_Users=" + tmp_FK_Users;


	string query = "select * from Package_Users where " + condition;		

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

	Row_Package_Users *pRow = new Row_Package_Users(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Administrator = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Administrator));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Comments = string(row[3],lengths[3]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_Package_Users::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Users* Row_Package_Users::FK_Users_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users);
}





