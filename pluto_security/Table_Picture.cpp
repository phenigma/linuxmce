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
#include "Table_Picture.h"
#include "Table_Alert.h"



void Database_pluto_security::CreateTable_Picture()
{
	tblPicture = new Table_Picture(this);
}

void Database_pluto_security::DeleteTable_Picture()
{
	delete tblPicture;
}

Table_Picture::~Table_Picture()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Picture *pRow = (Row_Picture *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Picture *pRow = (Row_Picture *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Picture::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Picture *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Picture *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Picture);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Picture::Reload()
{
	Row_Picture *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Picture);
		Row_Picture *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Picture::Row_Picture(Table_Picture *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Picture::SetDefaultValues()
{
	m_PK_Picture = 0;
is_null[0] = false;
m_FK_Alert = 0;
is_null[1] = false;
m_EK_Device = 0;
is_null[2] = false;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Picture::PK_Picture_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Picture;}
long int Row_Picture::FK_Alert_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Alert;}
long int Row_Picture::EK_Device_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_EK_Device;}
string Row_Picture::Filename_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Filename;}

		
void Row_Picture::PK_Picture_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Picture = val; is_modified=true; is_null[0]=false;}
void Row_Picture::FK_Alert_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Alert = val; is_modified=true; is_null[1]=false;}
void Row_Picture::EK_Device_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_EK_Device = val; is_modified=true; is_null[2]=false;}
void Row_Picture::Filename_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Filename = val; is_modified=true; is_null[3]=false;}

		
bool Row_Picture::Filename_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}

			
void Row_Picture::Filename_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
	

string Row_Picture::PK_Picture_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Picture);

return buf;
}

string Row_Picture::FK_Alert_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Alert);

return buf;
}

string Row_Picture::EK_Device_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Device);

return buf;
}

string Row_Picture::Filename_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Filename.c_str(), (unsigned long) m_Filename.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Picture::Key::Key(long int in_PK_Picture)
{
			pk_PK_Picture = in_PK_Picture;
	
}

Table_Picture::Key::Key(Row_Picture *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Picture = pRow->m_PK_Picture;
	
}		

bool Table_Picture::Key_Less::operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const
{
			if (key1.pk_PK_Picture!=key2.pk_PK_Picture)
return key1.pk_PK_Picture<key2.pk_PK_Picture;
else
return false;	
}	

bool Table_Picture::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Picture *pRow = (Row_Picture *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Picture_asSQL()+", "+pRow->FK_Alert_asSQL()+", "+pRow->EK_Device_asSQL()+", "+pRow->Filename_asSQL();

	
		string query = "insert into Picture (PK_Picture, FK_Alert, EK_Device, Filename) values ("+
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
pRow->m_PK_Picture=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Picture);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Picture* pRow = (Row_Picture*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Picture);

		char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Picture="+pRow->PK_Picture_asSQL()+", FK_Alert="+pRow->FK_Alert_asSQL()+", EK_Device="+pRow->EK_Device_asSQL()+", Filename="+pRow->Filename_asSQL();

	
		string query = "update Picture set " + update_values_list + " where " + condition;
			
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
		Row_Picture* pRow = (Row_Picture*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Picture* pRow = (Row_Picture*) (*i).second;	

		char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;

	
		string query = "delete from Picture where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		pRow = (Row_Picture*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Picture::GetRows(string where_statement,vector<class Row_Picture*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from Picture " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from Picture where " + where_statement;
		
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

		Row_Picture *pRow = new Row_Picture(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Picture));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Alert = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Alert));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Filename = string(row[3],lengths[3]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Picture);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Picture *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Picture* Table_Picture::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Picture *pRow = new Row_Picture(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Picture* Table_Picture::GetRow(long int in_PK_Picture)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Picture);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Picture*) (*i).second;
	//we have to fetch row
	Row_Picture* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Picture* Table_Picture::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Picture[32];
sprintf(tmp_PK_Picture, "%li", key.pk);


string condition;
condition = condition + "PK_Picture=" + tmp_PK_Picture;


	string query = "select * from Picture where " + condition;		

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

	Row_Picture *pRow = new Row_Picture(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Picture = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Picture));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Alert = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Alert));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Filename = string(row[3],lengths[3]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Alert* Row_Picture::FK_Alert_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Alert *pTable = table->database->Alert_get();
return pTable->GetRow(m_FK_Alert);
}





