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
#include "Table_PlaylistEntry.h"
#include "Table_Playlist.h"
#include "Table_File.h"



void Database_pluto_media::CreateTable_PlaylistEntry()
{
	tblPlaylistEntry = new Table_PlaylistEntry(this);
}

void Database_pluto_media::DeleteTable_PlaylistEntry()
{
	delete tblPlaylistEntry;
}

Table_PlaylistEntry::~Table_PlaylistEntry()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_PlaylistEntry *pRow = (Row_PlaylistEntry *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_PlaylistEntry *pRow = (Row_PlaylistEntry *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_PlaylistEntry::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	Row_PlaylistEntry *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_PlaylistEntry *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_PlaylistEntry);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_PlaylistEntry::Reload()
{
	Row_PlaylistEntry *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_PlaylistEntry);
		Row_PlaylistEntry *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_PlaylistEntry::Row_PlaylistEntry(Table_PlaylistEntry *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_PlaylistEntry::SetDefaultValues()
{
	m_PK_PlaylistEntry = 0;
is_null[0] = false;
m_FK_Playlist = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_PlaylistEntry::PK_PlaylistEntry_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_PlaylistEntry;}
long int Row_PlaylistEntry::FK_Playlist_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Playlist;}
long int Row_PlaylistEntry::FK_File_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_File;}
string Row_PlaylistEntry::Path_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Path;}
string Row_PlaylistEntry::Filename_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Filename;}

		
void Row_PlaylistEntry::PK_PlaylistEntry_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_PlaylistEntry = val; is_modified=true; is_null[0]=false;}
void Row_PlaylistEntry::FK_Playlist_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Playlist = val; is_modified=true; is_null[1]=false;}
void Row_PlaylistEntry::FK_File_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_File = val; is_modified=true; is_null[2]=false;}
void Row_PlaylistEntry::Path_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Path = val; is_modified=true; is_null[3]=false;}
void Row_PlaylistEntry::Filename_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Filename = val; is_modified=true; is_null[4]=false;}

		
bool Row_PlaylistEntry::FK_File_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[2];}
bool Row_PlaylistEntry::Path_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[3];}
bool Row_PlaylistEntry::Filename_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[4];}

			
void Row_PlaylistEntry::FK_File_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[2]=val;}
void Row_PlaylistEntry::Path_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[3]=val;}
void Row_PlaylistEntry::Filename_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[4]=val;}
	

string Row_PlaylistEntry::PK_PlaylistEntry_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_PlaylistEntry);

return buf;
}

string Row_PlaylistEntry::FK_Playlist_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Playlist);

return buf;
}

string Row_PlaylistEntry::FK_File_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_File);

return buf;
}

string Row_PlaylistEntry::Path_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char *buf = new char[301];
mysql_real_escape_string(table->database->db_handle, buf, m_Path.c_str(), (unsigned long) m_Path.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}

string Row_PlaylistEntry::Filename_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char *buf = new char[201];
mysql_real_escape_string(table->database->db_handle, buf, m_Filename.c_str(), (unsigned long) m_Filename.size());
string s=string()+"\""+buf+"\"";
delete buf;
return s;
}




Table_PlaylistEntry::Key::Key(long int in_PK_PlaylistEntry)
{
			pk_PK_PlaylistEntry = in_PK_PlaylistEntry;
	
}

Table_PlaylistEntry::Key::Key(Row_PlaylistEntry *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_PlaylistEntry = pRow->m_PK_PlaylistEntry;
	
}		

bool Table_PlaylistEntry::Key_Less::operator()(const Table_PlaylistEntry::Key &key1, const Table_PlaylistEntry::Key &key2) const
{
			if (key1.pk_PK_PlaylistEntry!=key2.pk_PK_PlaylistEntry)
return key1.pk_PK_PlaylistEntry<key2.pk_PK_PlaylistEntry;
else
return false;	
}	

bool Table_PlaylistEntry::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_PlaylistEntry *pRow = (Row_PlaylistEntry *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_PlaylistEntry_asSQL()+", "+pRow->FK_Playlist_asSQL()+", "+pRow->FK_File_asSQL()+", "+pRow->Path_asSQL()+", "+pRow->Filename_asSQL();

	
		string query = "insert into PlaylistEntry (PK_PlaylistEntry, FK_Playlist, FK_File, Path, Filename) values ("+
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
pRow->m_PK_PlaylistEntry=id;
	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_PlaylistEntry);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_PlaylistEntry* pRow = (Row_PlaylistEntry*) (*i).second;	
		SingleLongKey key(pRow->m_PK_PlaylistEntry);

		char tmp_PK_PlaylistEntry[32];
sprintf(tmp_PK_PlaylistEntry, "%li", key.pk);


string condition;
condition = condition + "PK_PlaylistEntry=" + tmp_PK_PlaylistEntry;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_PlaylistEntry="+pRow->PK_PlaylistEntry_asSQL()+", FK_Playlist="+pRow->FK_Playlist_asSQL()+", FK_File="+pRow->FK_File_asSQL()+", Path="+pRow->Path_asSQL()+", Filename="+pRow->Filename_asSQL();

	
		string query = "update PlaylistEntry set " + update_values_list + " where " + condition;
			
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
		Row_PlaylistEntry *pRow = (Row_PlaylistEntry *)(*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_PlaylistEntry* pRow = (Row_PlaylistEntry*) (*i).second;

		char tmp_PK_PlaylistEntry[32];
sprintf(tmp_PK_PlaylistEntry, "%li", key.pk);


string condition;
condition = condition + "PK_PlaylistEntry=" + tmp_PK_PlaylistEntry;

	
		string query = "delete from PlaylistEntry where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
			return false;
		}	
		
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return true;
}

bool Table_PlaylistEntry::GetRows(string where_statement,vector<class Row_PlaylistEntry*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || StringUtils::StartsWith(where_statement,"join ",true) )
		query = "select * from PlaylistEntry " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else
		query = "select * from PlaylistEntry where " + where_statement;
		
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

		Row_PlaylistEntry *pRow = new Row_PlaylistEntry(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PlaylistEntry = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PlaylistEntry));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Playlist = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Playlist));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_File));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Path = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Filename = string(row[4],lengths[4]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_PlaylistEntry);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_PlaylistEntry *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_PlaylistEntry* Table_PlaylistEntry::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_PlaylistEntry *pRow = new Row_PlaylistEntry(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_PlaylistEntry* Table_PlaylistEntry::GetRow(long int in_PK_PlaylistEntry)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	SingleLongKey row_key(in_PK_PlaylistEntry);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_PlaylistEntry*) (*i).second;
	//we have to fetch row
	Row_PlaylistEntry* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_PlaylistEntry* Table_PlaylistEntry::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_PlaylistEntry[32];
sprintf(tmp_PK_PlaylistEntry, "%li", key.pk);


string condition;
condition = condition + "PK_PlaylistEntry=" + tmp_PK_PlaylistEntry;


	string query = "select * from PlaylistEntry where " + condition;		

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

	Row_PlaylistEntry *pRow = new Row_PlaylistEntry(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PlaylistEntry = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PlaylistEntry));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Playlist = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Playlist));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_File = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_File));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Path = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Filename = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Filename = string(row[4],lengths[4]);
}



	mysql_free_result(res);			
	
	return pRow;						
}


class Row_Playlist* Row_PlaylistEntry::FK_Playlist_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_Playlist *pTable = table->database->Playlist_get();
return pTable->GetRow(m_FK_Playlist);
}
class Row_File* Row_PlaylistEntry::FK_File_getrow()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

class Table_File *pTable = table->database->File_get();
return pTable->GetRow(m_FK_File);
}





