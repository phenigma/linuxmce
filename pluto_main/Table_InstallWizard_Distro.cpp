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
#include "Table_InstallWizard_Distro.h"
#include "Table_OperatingSystem.h"
#include "Table_Distro.h"



void Database_pluto_main::CreateTable_InstallWizard_Distro()
{
	tblInstallWizard_Distro = new Table_InstallWizard_Distro(this);
}

void Database_pluto_main::DeleteTable_InstallWizard_Distro()
{
	delete tblInstallWizard_Distro;
}

Table_InstallWizard_Distro::~Table_InstallWizard_Distro()
{
	map<Table_InstallWizard_Distro::Key, class Row_InstallWizard_Distro*, Table_InstallWizard_Distro::Key_Less>::iterator it;
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


void Row_InstallWizard_Distro::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_InstallWizard_Distro*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_InstallWizard_Distro::Key key(this);					
			map<Table_InstallWizard_Distro::Key, Row_InstallWizard_Distro*, Table_InstallWizard_Distro::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_InstallWizard_Distro::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_InstallWizard_Distro::Key key(this);		
		Row_InstallWizard_Distro *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_InstallWizard_Distro::Row_InstallWizard_Distro(Table_InstallWizard_Distro *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_InstallWizard_Distro::SetDefaultValues()
{
	m_PK_InstallWizard_Distro = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_Comments = "";
is_null[3] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_InstallWizard_Distro::PK_InstallWizard_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_InstallWizard_Distro;}
long int Row_InstallWizard_Distro::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_OperatingSystem;}
long int Row_InstallWizard_Distro::FK_Distro_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Distro;}
string Row_InstallWizard_Distro::Comments_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Comments;}

		
void Row_InstallWizard_Distro::PK_InstallWizard_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_InstallWizard_Distro = val; is_modified=true; is_null[0]=false;}
void Row_InstallWizard_Distro::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[1]=false;}
void Row_InstallWizard_Distro::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Distro = val; is_modified=true; is_null[2]=false;}
void Row_InstallWizard_Distro::Comments_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Comments = val; is_modified=true; is_null[3]=false;}

		
bool Row_InstallWizard_Distro::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_InstallWizard_Distro::FK_Distro_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}

			
void Row_InstallWizard_Distro::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_InstallWizard_Distro::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
	

string Row_InstallWizard_Distro::PK_InstallWizard_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_InstallWizard_Distro);

return buf;
}

string Row_InstallWizard_Distro::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_InstallWizard_Distro::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_InstallWizard_Distro::Comments_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[33554431];
mysql_real_escape_string(table->database->db_handle, buf, m_Comments.c_str(), (unsigned long) m_Comments.size());
return string()+"\""+buf+"\"";
}




Table_InstallWizard_Distro::Key::Key(long int in_PK_InstallWizard_Distro)
{
			pk_PK_InstallWizard_Distro = in_PK_InstallWizard_Distro;
	
}

Table_InstallWizard_Distro::Key::Key(Row_InstallWizard_Distro *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_InstallWizard_Distro = pRow->m_PK_InstallWizard_Distro;
	
}		

bool Table_InstallWizard_Distro::Key_Less::operator()(const Table_InstallWizard_Distro::Key &key1, const Table_InstallWizard_Distro::Key &key2) const
{
			if (key1.pk_PK_InstallWizard_Distro!=key2.pk_PK_InstallWizard_Distro)
return key1.pk_PK_InstallWizard_Distro<key2.pk_PK_InstallWizard_Distro;
else
return false;	
}	

void Table_InstallWizard_Distro::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_InstallWizard_Distro*>::iterator i = addedRows.begin();
	
		Row_InstallWizard_Distro *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_InstallWizard_Distro_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->Comments_asSQL();

	
		string query = "insert into InstallWizard_Distro (PK_InstallWizard_Distro, FK_OperatingSystem, FK_Distro, Comments) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_InstallWizard_Distro=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_InstallWizard_Distro*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_InstallWizard_Distro* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_InstallWizard_Distro[32];
sprintf(tmp_PK_InstallWizard_Distro, "%li", key.pk_PK_InstallWizard_Distro);


string condition;
condition = condition + "PK_InstallWizard_Distro=" + tmp_PK_InstallWizard_Distro;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_InstallWizard_Distro="+pRow->PK_InstallWizard_Distro_asSQL()+", FK_OperatingSystem="+pRow->FK_OperatingSystem_asSQL()+", FK_Distro="+pRow->FK_Distro_asSQL()+", Comments="+pRow->Comments_asSQL();

	
		string query = "update InstallWizard_Distro set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_InstallWizard_Distro*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_InstallWizard_Distro*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_InstallWizard_Distro[32];
sprintf(tmp_PK_InstallWizard_Distro, "%li", key.pk_PK_InstallWizard_Distro);


string condition;
condition = condition + "PK_InstallWizard_Distro=" + tmp_PK_InstallWizard_Distro;

	
		string query = "delete from InstallWizard_Distro where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_InstallWizard_Distro::GetRows(string where_statement,vector<class Row_InstallWizard_Distro*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from InstallWizard_Distro " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from InstallWizard_Distro where " + where_statement;
		
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

		Row_InstallWizard_Distro *pRow = new Row_InstallWizard_Distro(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InstallWizard_Distro = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InstallWizard_Distro));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
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

		Key key(pRow);
		
                map<Table_InstallWizard_Distro::Key, Row_InstallWizard_Distro*, Table_InstallWizard_Distro::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_InstallWizard_Distro* Table_InstallWizard_Distro::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_InstallWizard_Distro *pRow = new Row_InstallWizard_Distro(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_InstallWizard_Distro* Table_InstallWizard_Distro::GetRow(long int in_PK_InstallWizard_Distro)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_InstallWizard_Distro);

	map<Key, Row_InstallWizard_Distro*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_InstallWizard_Distro* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_InstallWizard_Distro* Table_InstallWizard_Distro::FetchRow(Table_InstallWizard_Distro::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_InstallWizard_Distro[32];
sprintf(tmp_PK_InstallWizard_Distro, "%li", key.pk_PK_InstallWizard_Distro);


string condition;
condition = condition + "PK_InstallWizard_Distro=" + tmp_PK_InstallWizard_Distro;


	string query = "select * from InstallWizard_Distro where " + condition;		

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

	Row_InstallWizard_Distro *pRow = new Row_InstallWizard_Distro(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_InstallWizard_Distro = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_InstallWizard_Distro));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Distro));
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


class Row_OperatingSystem* Row_InstallWizard_Distro::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}
class Row_Distro* Row_InstallWizard_Distro::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}





