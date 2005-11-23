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
#include "Table_Screen_CommandParameter.h"
#include "Table_Screen.h"
#include "Table_CommandParameter.h"



void Database_pluto_main::CreateTable_Screen_CommandParameter()
{
	tblScreen_CommandParameter = new Table_Screen_CommandParameter(this);
}

void Database_pluto_main::DeleteTable_Screen_CommandParameter()
{
	if( tblScreen_CommandParameter )
		delete tblScreen_CommandParameter;
}

bool Database_pluto_main::Commit_Screen_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblScreen_CommandParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Screen_CommandParameter::~Table_Screen_CommandParameter()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Screen_CommandParameter *pRow = (Row_Screen_CommandParameter *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Screen_CommandParameter *pRow = (Row_Screen_CommandParameter *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Screen_CommandParameter::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Screen_CommandParameter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Screen_CommandParameter *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Screen,pRow->m_FK_CommandParameter);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Screen_CommandParameter::Reload()
{
	Row_Screen_CommandParameter *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Screen,pRow->m_FK_CommandParameter);
		Row_Screen_CommandParameter *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Screen_CommandParameter::Row_Screen_CommandParameter(Table_Screen_CommandParameter *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Screen_CommandParameter::SetDefaultValues()
{
	m_FK_Screen = 0;
is_null[0] = false;
m_FK_CommandParameter = 0;
is_null[1] = false;
is_null[2] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Screen_CommandParameter::FK_Screen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_Screen;}
long int Row_Screen_CommandParameter::FK_CommandParameter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_CommandParameter;}
string Row_Screen_CommandParameter::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Description;}

		
void Row_Screen_CommandParameter::FK_Screen_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_Screen = val; is_modified=true; is_null[0]=false;}
void Row_Screen_CommandParameter::FK_CommandParameter_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_CommandParameter = val; is_modified=true; is_null[1]=false;}
void Row_Screen_CommandParameter::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Description = val; is_modified=true; is_null[2]=false;}

		
bool Row_Screen_CommandParameter::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[2];}

			
void Row_Screen_CommandParameter::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[2]=val;
is_modified=true;
}
	

string Row_Screen_CommandParameter::FK_Screen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen);

return buf;
}

string Row_Screen_CommandParameter::FK_CommandParameter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandParameter);

return buf;
}

string Row_Screen_CommandParameter::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Description.c_str(), (unsigned long) min(65535,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_Screen_CommandParameter::Key::Key(long int in_FK_Screen, long int in_FK_CommandParameter)
{
			pk_FK_Screen = in_FK_Screen;
pk_FK_CommandParameter = in_FK_CommandParameter;
	
}

Table_Screen_CommandParameter::Key::Key(Row_Screen_CommandParameter *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_FK_Screen = pRow->m_FK_Screen;
pk_FK_CommandParameter = pRow->m_FK_CommandParameter;
	
}		

bool Table_Screen_CommandParameter::Key_Less::operator()(const Table_Screen_CommandParameter::Key &key1, const Table_Screen_CommandParameter::Key &key2) const
{
			if (key1.pk_FK_Screen!=key2.pk_FK_Screen)
return key1.pk_FK_Screen<key2.pk_FK_Screen;
else
if (key1.pk_FK_CommandParameter!=key2.pk_FK_CommandParameter)
return key1.pk_FK_CommandParameter<key2.pk_FK_CommandParameter;
else
return false;	
}	

bool Table_Screen_CommandParameter::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Screen_CommandParameter *pRow = (Row_Screen_CommandParameter *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Screen_asSQL()+", "+pRow->FK_CommandParameter_asSQL()+", "+pRow->Description_asSQL();

	
		string query = "insert into Screen_CommandParameter (`FK_Screen`, `FK_CommandParameter`, `Description`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			if( bDeleteFailedInsertRow )
			{
				addedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
				
			
			addedRows.erase(i);
			DoubleLongKey key(pRow->m_FK_Screen,pRow->m_FK_CommandParameter);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Screen_CommandParameter* pRow = (Row_Screen_CommandParameter*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Screen,pRow->m_FK_CommandParameter);

		char tmp_FK_Screen[32];
sprintf(tmp_FK_Screen, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "`FK_Screen`=" + tmp_FK_Screen+" AND "+"`FK_CommandParameter`=" + tmp_FK_CommandParameter;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_Screen`="+pRow->FK_Screen_asSQL()+", `FK_CommandParameter`="+pRow->FK_CommandParameter_asSQL()+", `Description`="+pRow->Description_asSQL();

	
		string query = "update Screen_CommandParameter set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			if( bDeleteFailedModifiedRow )
			{
				cachedRows.erase(i);
				delete pRow;
			}
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Screen_CommandParameter* pRow = (Row_Screen_CommandParameter*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Screen_CommandParameter* pRow = (Row_Screen_CommandParameter*) (*i).second;	

		char tmp_FK_Screen[32];
sprintf(tmp_FK_Screen, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "`FK_Screen`=" + tmp_FK_Screen+" AND "+"`FK_CommandParameter`=" + tmp_FK_CommandParameter;

	
		string query = "delete from Screen_CommandParameter where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Screen_CommandParameter*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Screen_CommandParameter::GetRows(string where_statement,vector<class Row_Screen_CommandParameter*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Screen_CommandParameter`.* from Screen_CommandParameter " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Screen_CommandParameter`.* from Screen_CommandParameter where " + where_statement;
	else
		query = "select `Screen_CommandParameter`.* from Screen_CommandParameter";
		
	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_Screen_CommandParameter *pRow = new Row_Screen_CommandParameter(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Screen = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Screen));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandParameter));
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



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Screen,pRow->m_FK_CommandParameter);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Screen_CommandParameter *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Screen_CommandParameter* Table_Screen_CommandParameter::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_Screen_CommandParameter *pRow = new Row_Screen_CommandParameter(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Screen_CommandParameter* Table_Screen_CommandParameter::GetRow(long int in_FK_Screen, long int in_FK_CommandParameter)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	DoubleLongKey row_key(in_FK_Screen, in_FK_CommandParameter);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Screen_CommandParameter*) (*i).second;
	//we have to fetch row
	Row_Screen_CommandParameter* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Screen_CommandParameter* Table_Screen_CommandParameter::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_FK_Screen[32];
sprintf(tmp_FK_Screen, "%li", key.pk1);

char tmp_FK_CommandParameter[32];
sprintf(tmp_FK_CommandParameter, "%li", key.pk2);


string condition;
condition = condition + "`FK_Screen`=" + tmp_FK_Screen+" AND "+"`FK_CommandParameter`=" + tmp_FK_CommandParameter;


	string query = "select * from Screen_CommandParameter where " + condition;		

	if (mysql_query(database->m_pMySQL, query.c_str()))
	{	
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->m_pMySQL);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
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

	Row_Screen_CommandParameter *pRow = new Row_Screen_CommandParameter(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Screen = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Screen));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_CommandParameter = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_CommandParameter));
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



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Screen* Row_Screen_CommandParameter::FK_Screen_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen);
}
class Row_CommandParameter* Row_Screen_CommandParameter::FK_CommandParameter_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_CommandParameter *pTable = table->database->CommandParameter_get();
return pTable->GetRow(m_FK_CommandParameter);
}





