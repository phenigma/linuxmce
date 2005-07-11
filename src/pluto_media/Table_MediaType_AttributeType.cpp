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
#include "Table_MediaType_AttributeType.h"
#include "Table_AttributeType.h"



void Database_pluto_media::CreateTable_MediaType_AttributeType()
{
	tblMediaType_AttributeType = new Table_MediaType_AttributeType(this);
}

void Database_pluto_media::DeleteTable_MediaType_AttributeType()
{
	if( tblMediaType_AttributeType )
		delete tblMediaType_AttributeType;
}

Table_MediaType_AttributeType::~Table_MediaType_AttributeType()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_MediaType_AttributeType *pRow = (Row_MediaType_AttributeType *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_MediaType_AttributeType *pRow = (Row_MediaType_AttributeType *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_MediaType_AttributeType::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_MediaType_AttributeType *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_MediaType_AttributeType *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_AttributeType);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_MediaType_AttributeType::Reload()
{
	Row_MediaType_AttributeType *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_AttributeType);
		Row_MediaType_AttributeType *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MediaType_AttributeType::Row_MediaType_AttributeType(Table_MediaType_AttributeType *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MediaType_AttributeType::SetDefaultValues()
{
	m_EK_MediaType = 0;
is_null[0] = false;
m_FK_AttributeType = 0;
is_null[1] = false;
m_Identifier = 0;
is_null[2] = false;
m_CombineAsOne = 0;
is_null[3] = false;
is_null[4] = true;
m_psc_id = 0;
is_null[5] = true;
m_psc_batch = 0;
is_null[6] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[7] = false;
m_psc_mod = "00000000000000";
is_null[8] = false;
is_null[9] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MediaType_AttributeType::EK_MediaType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_EK_MediaType;}
long int Row_MediaType_AttributeType::FK_AttributeType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_FK_AttributeType;}
short int Row_MediaType_AttributeType::Identifier_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_Identifier;}
short int Row_MediaType_AttributeType::CombineAsOne_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_CombineAsOne;}
long int Row_MediaType_AttributeType::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_id;}
long int Row_MediaType_AttributeType::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_batch;}
long int Row_MediaType_AttributeType::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_user;}
short int Row_MediaType_AttributeType::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_frozen;}
string Row_MediaType_AttributeType::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_mod;}
long int Row_MediaType_AttributeType::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return m_psc_restrict;}

		
void Row_MediaType_AttributeType::EK_MediaType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_EK_MediaType = val; is_modified=true; is_null[0]=false;}
void Row_MediaType_AttributeType::FK_AttributeType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_FK_AttributeType = val; is_modified=true; is_null[1]=false;}
void Row_MediaType_AttributeType::Identifier_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_Identifier = val; is_modified=true; is_null[2]=false;}
void Row_MediaType_AttributeType::CombineAsOne_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_CombineAsOne = val; is_modified=true; is_null[3]=false;}
void Row_MediaType_AttributeType::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_MediaType_AttributeType::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_MediaType_AttributeType::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_MediaType_AttributeType::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_MediaType_AttributeType::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}
void Row_MediaType_AttributeType::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

m_psc_restrict = val; is_modified=true; is_null[9]=false;}

		
bool Row_MediaType_AttributeType::CombineAsOne_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[3];}
bool Row_MediaType_AttributeType::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[4];}
bool Row_MediaType_AttributeType::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[5];}
bool Row_MediaType_AttributeType::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[6];}
bool Row_MediaType_AttributeType::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[7];}
bool Row_MediaType_AttributeType::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

return is_null[9];}

			
void Row_MediaType_AttributeType::CombineAsOne_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[3]=val;
is_modified=true;
}
void Row_MediaType_AttributeType::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[4]=val;
is_modified=true;
}
void Row_MediaType_AttributeType::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[5]=val;
is_modified=true;
}
void Row_MediaType_AttributeType::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[6]=val;
is_modified=true;
}
void Row_MediaType_AttributeType::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[7]=val;
is_modified=true;
}
void Row_MediaType_AttributeType::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
is_null[9]=val;
is_modified=true;
}
	

string Row_MediaType_AttributeType::EK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_MediaType);

return buf;
}

string Row_MediaType_AttributeType::FK_AttributeType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_AttributeType);

return buf;
}

string Row_MediaType_AttributeType::Identifier_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Identifier);

return buf;
}

string Row_MediaType_AttributeType::CombineAsOne_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CombineAsOne);

return buf;
}

string Row_MediaType_AttributeType::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_MediaType_AttributeType::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_MediaType_AttributeType::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_MediaType_AttributeType::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_MediaType_AttributeType::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[8])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MediaType_AttributeType::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_MediaType_AttributeType::Key::Key(long int in_EK_MediaType, long int in_FK_AttributeType)
{
			pk_EK_MediaType = in_EK_MediaType;
pk_FK_AttributeType = in_FK_AttributeType;
	
}

Table_MediaType_AttributeType::Key::Key(Row_MediaType_AttributeType *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_MySqlMutex);

			pk_EK_MediaType = pRow->m_EK_MediaType;
pk_FK_AttributeType = pRow->m_FK_AttributeType;
	
}		

bool Table_MediaType_AttributeType::Key_Less::operator()(const Table_MediaType_AttributeType::Key &key1, const Table_MediaType_AttributeType::Key &key2) const
{
			if (key1.pk_EK_MediaType!=key2.pk_EK_MediaType)
return key1.pk_EK_MediaType<key2.pk_EK_MediaType;
else
if (key1.pk_FK_AttributeType!=key2.pk_FK_AttributeType)
return key1.pk_FK_AttributeType<key2.pk_FK_AttributeType;
else
return false;	
}	

bool Table_MediaType_AttributeType::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_MediaType_AttributeType *pRow = (Row_MediaType_AttributeType *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->EK_MediaType_asSQL()+", "+pRow->FK_AttributeType_asSQL()+", "+pRow->Identifier_asSQL()+", "+pRow->CombineAsOne_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into MediaType_AttributeType (`EK_MediaType`, `FK_AttributeType`, `Identifier`, `CombineAsOne`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
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
			DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_AttributeType);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_MediaType_AttributeType* pRow = (Row_MediaType_AttributeType*) (*i).second;	
		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_AttributeType);

		char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_AttributeType`=" + tmp_FK_AttributeType;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`EK_MediaType`="+pRow->EK_MediaType_asSQL()+", `FK_AttributeType`="+pRow->FK_AttributeType_asSQL()+", `Identifier`="+pRow->Identifier_asSQL()+", `CombineAsOne`="+pRow->CombineAsOne_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update MediaType_AttributeType set " + update_values_list + " where " + condition;
			
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
		Row_MediaType_AttributeType* pRow = (Row_MediaType_AttributeType*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_MediaType_AttributeType* pRow = (Row_MediaType_AttributeType*) (*i).second;	

		char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_AttributeType`=" + tmp_FK_AttributeType;

	
		string query = "delete from MediaType_AttributeType where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_MediaType_AttributeType*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_MediaType_AttributeType::GetRows(string where_statement,vector<class Row_MediaType_AttributeType*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `MediaType_AttributeType`.* from MediaType_AttributeType " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `MediaType_AttributeType`.* from MediaType_AttributeType where " + where_statement;
	else
		query = "select `MediaType_AttributeType`.* from MediaType_AttributeType";
		
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

		Row_MediaType_AttributeType *pRow = new Row_MediaType_AttributeType(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AttributeType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Identifier = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Identifier));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_CombineAsOne = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_CombineAsOne));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_EK_MediaType,pRow->m_FK_AttributeType);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_MediaType_AttributeType *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_MediaType_AttributeType* Table_MediaType_AttributeType::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	Row_MediaType_AttributeType *pRow = new Row_MediaType_AttributeType(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MediaType_AttributeType* Table_MediaType_AttributeType::GetRow(long int in_EK_MediaType, long int in_FK_AttributeType)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	DoubleLongKey row_key(in_EK_MediaType, in_FK_AttributeType);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_MediaType_AttributeType*) (*i).second;
	//we have to fetch row
	Row_MediaType_AttributeType* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MediaType_AttributeType* Table_MediaType_AttributeType::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_MySqlMutex);

	//defines the string query for the value of key
	char tmp_EK_MediaType[32];
sprintf(tmp_EK_MediaType, "%li", key.pk1);

char tmp_FK_AttributeType[32];
sprintf(tmp_FK_AttributeType, "%li", key.pk2);


string condition;
condition = condition + "`EK_MediaType`=" + tmp_EK_MediaType+" AND "+"`FK_AttributeType`=" + tmp_FK_AttributeType;


	string query = "select * from MediaType_AttributeType where " + condition;		

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

	Row_MediaType_AttributeType *pRow = new Row_MediaType_AttributeType(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_MediaType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AttributeType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Identifier = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Identifier));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_CombineAsOne = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_CombineAsOne));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_restrict));
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_AttributeType* Row_MediaType_AttributeType::FK_AttributeType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);

class Table_AttributeType *pTable = table->database->AttributeType_get();
return pTable->GetRow(m_FK_AttributeType);
}





