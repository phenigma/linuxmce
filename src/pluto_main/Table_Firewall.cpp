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
#include "Table_Firewall.h"



void Database_pluto_main::CreateTable_Firewall()
{
	tblFirewall = new Table_Firewall(this);
}

void Database_pluto_main::DeleteTable_Firewall()
{
	delete tblFirewall;
}

Table_Firewall::~Table_Firewall()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Firewall *pRow = (Row_Firewall *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Firewall *pRow = (Row_Firewall *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Firewall::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_Firewall *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Firewall *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Firewall);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Firewall::Reload()
{
	Row_Firewall *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Firewall);
		Row_Firewall *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Firewall::Row_Firewall(Table_Firewall *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Firewall::SetDefaultValues()
{
	m_PK_Firewall = 0;
is_null[0] = false;
m_Protocol = "tcp";
is_null[1] = false;
m_SourcePort = 0;
is_null[2] = false;
m_SourcePortEnd = 0;
is_null[3] = false;
m_DestinationPort = 0;
is_null[4] = false;
m_DestinationIP = "";
is_null[5] = false;
m_RuleType = "";
is_null[6] = false;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_psc_frozen = 0;
is_null[10] = false;
m_psc_mod = "00000000000000";
is_null[11] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Firewall::PK_Firewall_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Firewall;}
string Row_Firewall::Protocol_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Protocol;}
long int Row_Firewall::SourcePort_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SourcePort;}
long int Row_Firewall::SourcePortEnd_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_SourcePortEnd;}
long int Row_Firewall::DestinationPort_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DestinationPort;}
string Row_Firewall::DestinationIP_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_DestinationIP;}
string Row_Firewall::RuleType_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_RuleType;}
long int Row_Firewall::psc_id_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_id;}
long int Row_Firewall::psc_batch_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_batch;}
long int Row_Firewall::psc_user_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_user;}
short int Row_Firewall::psc_frozen_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_frozen;}
string Row_Firewall::psc_mod_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_psc_mod;}

		
void Row_Firewall::PK_Firewall_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Firewall = val; is_modified=true; is_null[0]=false;}
void Row_Firewall::Protocol_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Protocol = val; is_modified=true; is_null[1]=false;}
void Row_Firewall::SourcePort_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SourcePort = val; is_modified=true; is_null[2]=false;}
void Row_Firewall::SourcePortEnd_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_SourcePortEnd = val; is_modified=true; is_null[3]=false;}
void Row_Firewall::DestinationPort_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DestinationPort = val; is_modified=true; is_null[4]=false;}
void Row_Firewall::DestinationIP_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_DestinationIP = val; is_modified=true; is_null[5]=false;}
void Row_Firewall::RuleType_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_RuleType = val; is_modified=true; is_null[6]=false;}
void Row_Firewall::psc_id_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_id = val; is_modified=true; is_null[7]=false;}
void Row_Firewall::psc_batch_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_batch = val; is_modified=true; is_null[8]=false;}
void Row_Firewall::psc_user_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_user = val; is_modified=true; is_null[9]=false;}
void Row_Firewall::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_frozen = val; is_modified=true; is_null[10]=false;}
void Row_Firewall::psc_mod_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_psc_mod = val; is_modified=true; is_null[11]=false;}

		
bool Row_Firewall::psc_id_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}
bool Row_Firewall::psc_batch_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[8];}
bool Row_Firewall::psc_user_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[9];}
bool Row_Firewall::psc_frozen_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[10];}

			
void Row_Firewall::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
void Row_Firewall::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[8]=val;}
void Row_Firewall::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[9]=val;}
void Row_Firewall::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[10]=val;}
	

string Row_Firewall::PK_Firewall_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Firewall);

return buf;
}

string Row_Firewall::Protocol_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char *buf = new char[21];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_Protocol.c_str(), (unsigned long) min(10,m_Protocol.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Firewall::SourcePort_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_SourcePort);

return buf;
}

string Row_Firewall::SourcePortEnd_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_SourcePortEnd);

return buf;
}

string Row_Firewall::DestinationPort_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_DestinationPort);

return buf;
}

string Row_Firewall::DestinationIP_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_DestinationIP.c_str(), (unsigned long) min(50,m_DestinationIP.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Firewall::RuleType_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char *buf = new char[101];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_RuleType.c_str(), (unsigned long) min(50,m_RuleType.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_Firewall::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Firewall::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Firewall::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Firewall::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Firewall::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[11])
return "NULL";

char *buf = new char[29];
mysql_real_escape_string(table->database->m_pMySQL, buf, m_psc_mod.c_str(), (unsigned long) min(14,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_Firewall::Key::Key(long int in_PK_Firewall)
{
			pk_PK_Firewall = in_PK_Firewall;
	
}

Table_Firewall::Key::Key(Row_Firewall *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Firewall = pRow->m_PK_Firewall;
	
}		

bool Table_Firewall::Key_Less::operator()(const Table_Firewall::Key &key1, const Table_Firewall::Key &key2) const
{
			if (key1.pk_PK_Firewall!=key2.pk_PK_Firewall)
return key1.pk_PK_Firewall<key2.pk_PK_Firewall;
else
return false;	
}	

bool Table_Firewall::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Firewall *pRow = (Row_Firewall *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Firewall_asSQL()+", "+pRow->Protocol_asSQL()+", "+pRow->SourcePort_asSQL()+", "+pRow->SourcePortEnd_asSQL()+", "+pRow->DestinationPort_asSQL()+", "+pRow->DestinationIP_asSQL()+", "+pRow->RuleType_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL();

	
		string query = "insert into Firewall (`PK_Firewall`, `Protocol`, `SourcePort`, `SourcePortEnd`, `DestinationPort`, `DestinationIP`, `RuleType`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		if (mysql_affected_rows(database->m_pMySQL)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->m_pMySQL);
		
			if (id!=0)
pRow->m_PK_Firewall=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Firewall);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Firewall* pRow = (Row_Firewall*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Firewall);

		char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Firewall`="+pRow->PK_Firewall_asSQL()+", `Protocol`="+pRow->Protocol_asSQL()+", `SourcePort`="+pRow->SourcePort_asSQL()+", `SourcePortEnd`="+pRow->SourcePortEnd_asSQL()+", `DestinationPort`="+pRow->DestinationPort_asSQL()+", `DestinationIP`="+pRow->DestinationIP_asSQL()+", `RuleType`="+pRow->RuleType_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL();

	
		string query = "update Firewall set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Firewall* pRow = (Row_Firewall*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Firewall* pRow = (Row_Firewall*) (*i).second;	

		char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;

	
		string query = "delete from Firewall where " + condition;
		
		if (mysql_query(database->m_pMySQL, query.c_str()))
		{	
			database->m_sLastMySqlError = mysql_error(database->m_pMySQL);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastMySqlError << endl;
			return false;
		}	
		
		pRow = (Row_Firewall*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_Firewall::GetRows(string where_statement,vector<class Row_Firewall*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select `Firewall`.* from Firewall " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select `Firewall`.* from Firewall where " + where_statement;
		
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

		Row_Firewall *pRow = new Row_Firewall(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Firewall = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Firewall));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Protocol = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Protocol = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_SourcePort = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_SourcePort));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_SourcePortEnd = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_SourcePortEnd));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DestinationPort = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_DestinationPort));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_DestinationIP = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_DestinationIP = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_RuleType = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_RuleType = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Firewall);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Firewall *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Firewall* Table_Firewall::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Firewall *pRow = new Row_Firewall(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Firewall* Table_Firewall::GetRow(long int in_PK_Firewall)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_Firewall);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Firewall*) (*i).second;
	//we have to fetch row
	Row_Firewall* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Firewall* Table_Firewall::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;


	string query = "select * from Firewall where " + condition;		

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

	Row_Firewall *pRow = new Row_Firewall(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Firewall = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Firewall));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Protocol = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Protocol = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_SourcePort = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_SourcePort));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_SourcePortEnd = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_SourcePortEnd));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DestinationPort = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_DestinationPort));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_DestinationIP = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_DestinationIP = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_RuleType = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_RuleType = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}



	mysql_free_result(res);			
	
	return pRow;						
}







