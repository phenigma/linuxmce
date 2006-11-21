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
#include "Table_Text_LS.h"
#include "Table_Text.h"
#include "Table_Language.h"



void Database_pluto_main::CreateTable_Text_LS()
{
	tblText_LS = new Table_Text_LS(this);
}

void Database_pluto_main::DeleteTable_Text_LS()
{
	if( tblText_LS )
		delete tblText_LS;
}

bool Database_pluto_main::Commit_Text_LS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblText_LS->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Text_LS::~Table_Text_LS()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Text_LS *pRow = (Row_Text_LS *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Text_LS *pRow = (Row_Text_LS *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Text_LS::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	Row_Text_LS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Text_LS *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Text,pRow->m_FK_Language);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Text_LS::Reload()
{
	Row_Text_LS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_MySqlMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Text,pRow->m_FK_Language);
		Row_Text_LS *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Text_LS::Row_Text_LS(Table_Text_LS *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Text_LS::SetDefaultValues()
{
