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
#include "Table_Image.h"



void Database_pluto_main::CreateTable_Image()
{
	tblImage = new Table_Image(this);
}

void Database_pluto_main::DeleteTable_Image()
{
	delete tblImage;
}

Table_Image::~Table_Image()
{
	map<Table_Image::Key, class Row_Image*, Table_Image::Key_Less>::iterator it;
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


void Row_Image::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Image*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Image::Key key(this);					
			map<Table_Image::Key, Row_Image*, Table_Image::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Image::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Image::Key key(this);		
		Row_Image *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Image::Row_Image(Table_Image *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Image::SetDefaultValues()
{
	m_PK_Image = "";
is_null[0] = false;
is_null[1] = true;
m_Width = 0;
is_null[2] = false;
m_Height = 0;
is_null[3] = false;
m_Modification_RecordInfo = "00000000000000";
is_null[4] = false;
m_IsNew_RecordInfo = 1;
is_null[5] = false;
m_IsDeleted_RecordInfo = 0;
is_null[6] = false;
is_null[7] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

string Row_Image::PK_Image_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Image;}
string Row_Image::Date_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Date;}
long int Row_Image::Width_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Width;}
long int Row_Image::Height_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Height;}
string Row_Image::Modification_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Modification_RecordInfo;}
short int Row_Image::IsNew_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsNew_RecordInfo;}
short int Row_Image::IsDeleted_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_IsDeleted_RecordInfo;}
long int Row_Image::FK_Users_RecordInfo_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_FK_Users_RecordInfo;}

		
void Row_Image::PK_Image_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Image = val; is_modified=true; is_null[0]=false;}
void Row_Image::Date_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Date = val; is_modified=true; is_null[1]=false;}
void Row_Image::Width_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Width = val; is_modified=true; is_null[2]=false;}
void Row_Image::Height_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Height = val; is_modified=true; is_null[3]=false;}
void Row_Image::Modification_RecordInfo_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Modification_RecordInfo = val; is_modified=true; is_null[4]=false;}
void Row_Image::IsNew_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsNew_RecordInfo = val; is_modified=true; is_null[5]=false;}
void Row_Image::IsDeleted_RecordInfo_set(short int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_IsDeleted_RecordInfo = val; is_modified=true; is_null[6]=false;}
void Row_Image::FK_Users_RecordInfo_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_FK_Users_RecordInfo = val; is_modified=true; is_null[7]=false;}

		
bool Row_Image::Date_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[1];}
bool Row_Image::IsNew_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[5];}
bool Row_Image::IsDeleted_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[6];}
bool Row_Image::FK_Users_RecordInfo_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return is_null[7];}

			
void Row_Image::Date_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[1]=val;}
void Row_Image::IsNew_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[5]=val;}
void Row_Image::IsDeleted_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[6]=val;}
void Row_Image::FK_Users_RecordInfo_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

is_null[7]=val;}
	

string Row_Image::PK_Image_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[201];
mysql_real_escape_string(table->database->db_handle, buf, m_PK_Image.c_str(), (unsigned long) m_PK_Image.size());
return string()+"\""+buf+"\"";
}

string Row_Image::Date_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[39];
mysql_real_escape_string(table->database->db_handle, buf, m_Date.c_str(), (unsigned long) m_Date.size());
return string()+"\""+buf+"\"";
}

string Row_Image::Width_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Width);

return buf;
}

string Row_Image::Height_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Height);

return buf;
}

string Row_Image::Modification_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[29];
mysql_real_escape_string(table->database->db_handle, buf, m_Modification_RecordInfo.c_str(), (unsigned long) m_Modification_RecordInfo.size());
return string()+"\""+buf+"\"";
}

string Row_Image::IsNew_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsNew_RecordInfo);

return buf;
}

string Row_Image::IsDeleted_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsDeleted_RecordInfo);

return buf;
}

string Row_Image::FK_Users_RecordInfo_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_RecordInfo);

return buf;
}




Table_Image::Key::Key(string in_PK_Image)
{
			pk_PK_Image = in_PK_Image;
	
}

Table_Image::Key::Key(Row_Image *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Image = pRow->m_PK_Image;
	
}		

bool Table_Image::Key_Less::operator()(const Table_Image::Key &key1, const Table_Image::Key &key2) const
{
			if (key1.pk_PK_Image!=key2.pk_PK_Image)
return key1.pk_PK_Image<key2.pk_PK_Image;
else
return false;	
}	

void Table_Image::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Image*>::iterator i = addedRows.begin();
	
		Row_Image *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Image_asSQL()+", "+pRow->Date_asSQL()+", "+pRow->Width_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->Modification_RecordInfo_asSQL()+", "+pRow->IsNew_RecordInfo_asSQL()+", "+pRow->IsDeleted_RecordInfo_asSQL()+", "+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "insert into Image (PK_Image, Date, Width, Height, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
				
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Image*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Image* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Image[201];
mysql_real_escape_string(database->db_handle,tmp_PK_Image, key.pk_PK_Image.c_str(), (unsigned long) key.pk_PK_Image.size());


string condition;
condition = condition + "PK_Image=" + "\"" + tmp_PK_Image+ "\"";
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Image="+pRow->PK_Image_asSQL()+", Date="+pRow->Date_asSQL()+", Width="+pRow->Width_asSQL()+", Height="+pRow->Height_asSQL()+", Modification_RecordInfo="+pRow->Modification_RecordInfo_asSQL()+", IsNew_RecordInfo="+pRow->IsNew_RecordInfo_asSQL()+", IsDeleted_RecordInfo="+pRow->IsDeleted_RecordInfo_asSQL()+", FK_Users_RecordInfo="+pRow->FK_Users_RecordInfo_asSQL();

	
		string query = "update Image set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Image*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Image*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Image[201];
mysql_real_escape_string(database->db_handle,tmp_PK_Image, key.pk_PK_Image.c_str(), (unsigned long) key.pk_PK_Image.size());


string condition;
condition = condition + "PK_Image=" + "\"" + tmp_PK_Image+ "\"";

	
		string query = "delete from Image where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Image::GetRows(string where_statement,vector<class Row_Image*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Image where " + where_statement;
		
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

		Row_Image *pRow = new Row_Image(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Image = "";
}
else
{
pRow->is_null[0]=false;
pRow->m_PK_Image = string(row[0],lengths[0]);
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Date = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Width));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Height));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Image::Key, Row_Image*, Table_Image::Key_Less>::iterator i = cachedRows.find(key);
			
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

Row_Image* Table_Image::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Image *pRow = new Row_Image(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Image* Table_Image::GetRow(string in_PK_Image)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Image);

	map<Key, Row_Image*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Image* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Image* Table_Image::FetchRow(Table_Image::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Image[201];
mysql_real_escape_string(database->db_handle,tmp_PK_Image, key.pk_PK_Image.c_str(), (unsigned long) key.pk_PK_Image.size());


string condition;
condition = condition + "PK_Image=" + "\"" + tmp_PK_Image+ "\"";


	string query = "select * from Image where " + condition;		

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

	Row_Image *pRow = new Row_Image(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Image = "";
}
else
{
pRow->is_null[0]=false;
pRow->m_PK_Image = string(row[0],lengths[0]);
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Date = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Date = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Width));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_Height));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Modification_RecordInfo = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Modification_RecordInfo = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_IsNew_RecordInfo = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_IsNew_RecordInfo));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_IsDeleted_RecordInfo = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_IsDeleted_RecordInfo));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Users_RecordInfo = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Users_RecordInfo));
}



	mysql_free_result(res);			
	
	return pRow;						
}







