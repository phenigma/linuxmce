#include "PlutoUtils/CommonIncludes.h"	
#include "Table.h"

#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

#include "Field.h"
#include "Database.h"

#include "RA/RA_Processor.h"
#include "R_CommitTable.h"
#include "R_CommitRow.h"

using namespace sqlCVS;

Table::Table(class Database *pDatabase,string sName)
{
	m_pDatabase=pDatabase;
	m_sName=sName;
	m_pRepository=NULL;
	m_pField_id=m_pField_batch=m_pField_user=m_pField_mod=NULL;
	m_pTable_History=m_pTable_WeAreHistoryFor=NULL;
	m_bHasAutoIncrement=false;
	GetFields();
}

void Table::GetFields()
{
	for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
		delete (*it).second;
	m_mapField.clear();

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	if( res.r=mysql_list_fields(m_pDatabase->MYSQL_get(), m_sName.c_str(), NULL) )
	{
		int fields_count = mysql_num_fields(res.r);

		MYSQL_FIELD *mysql_fields = mysql_fetch_fields(res.r);

		for (int i=0; i<fields_count; i++)
		{
			Field *pField = new Field(this,mysql_fields+i);
			m_mapField[pField->Name_get()]=pField;

			if( pField->flags & PRI_KEY_FLAG )
				m_listField_PrimaryKey.push_back(pField);
				
			if( pField->flags & AUTO_INCREMENT_FLAG )
				m_bHasAutoIncrement=true;

			if( pField->Name_get()=="psc_id" )
				m_pField_id = pField;
			else if( pField->Name_get()=="psc_batch" )
				m_pField_batch = pField;
			else if( pField->Name_get()=="psc_user" )
				m_pField_user = pField;
			else if( pField->Name_get()=="psc_mod" )
				m_pField_mod = pField;
		}
	}
}

string Table::DefinesRepository()
{
	if( m_sName.substr(0,4)!="psc_" )
		return "";

	return m_sName.substr( 4, m_sName.length() - GetTrailingString().length() - 5 );
}

string Table::GetTrailingString(string sName)
{
	if( sName.length()==0 )
		sName = m_sName;

	string::size_type lastpos=string::npos,pos=0;
	while( (pos = sName.find('_',pos)) != string::npos )
		lastpos = pos++;  // Increment pos after assigning the value
	return lastpos==string::npos ? "" : sName.substr(lastpos+1);
}

Table::~Table()
{
	int k=2;
}

void Table::TrackChanges_set(bool bOn)
{
	if( bOn )
	{
		if( !m_pField_id )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` ADD `psc_id` INT( 11 );");
		if( !m_pField_batch )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` ADD `psc_batch` INT( 11 );");
		if( !m_pField_user )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` ADD `psc_user` INT( 11 );");
		if( !m_pField_mod )
		{
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` ADD `psc_mod` timestamp(14) default '0';");
			m_pDatabase->mysql_query("UPDATE `" + m_sName + "` SET `psc_mod`=0;");
		}
	}
	else
	{
		if( m_pField_id )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` DROP `psc_id`;");
		if( m_pField_batch )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` DROP `psc_batch`;");
		if( m_pField_user )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` DROP `psc_user`;");
		if( m_pField_mod )
			m_pDatabase->mysql_query("ALTER TABLE `" + m_sName + "` DROP `psc_mod`;");
	}
	GetFields();
}

void Table::HasFullHistory_set(bool bOn)
{
	if( bOn )
	{
		if( m_pTable_History )
		{
			// This shouldn't happen
			cerr << "There is already a history table for: " << m_sName << endl;
			if( !AskQuestion("Drop it and make a new one?  You will lose all your history data.",false) )
				return;
			m_pDatabase->mysql_query("DROP TABLE `" + m_sName + "_pschist`;");
			m_pDatabase->m_mapTable_Remove(m_pTable_History->m_sName);
			delete m_pTable_History;
		}
		TrackChanges_set(true); // This must be on
		ostringstream sql;
		sql	<< "CREATE TABLE `" << m_sName << "_pschist`(" << endl;
		for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
		{
			Field *pField = (*it).second;
			sql << "`" << pField->Name_get() << "` " << pField->getDType() << "(" << pField->length << ")," << endl;
		}
		sql << "PRIMARY KEY  (psc_id,psc_batch)" << endl
			<< ") TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
		if( !m_pDatabase->mysql_query(sql.str()) )
			throw "Database error";

		m_pTable_History = new Table(m_pDatabase,m_sName + "_pschist");
		m_pTable_History->m_pTable_WeAreHistoryFor=this;
		m_pDatabase->m_mapTable_Add(m_pTable_History->m_sName,m_pTable_History);
	}
	else
	{
		cout << "Are you sure you want to delete your history for " << m_sName << "?" << endl;
		if( !AskQuestion("The data will be permanently removed unless you made a backup.",false) )
			return;
		m_pDatabase->mysql_query("DROP TABLE `" + m_sName + "_pschist`;");
		m_pDatabase->m_mapTable_Remove(m_pTable_History->m_sName);
		delete m_pTable_History;
		m_pTable_History=NULL;
	}
}

void Table::MatchUpHistory()
{
	if( GetTrailingString()=="pschist" )
	{
		string BaseName = m_sName.substr(0, m_sName.length()-8);
		Table *pMainTable = m_pDatabase->m_mapTable_Find( BaseName );
		if( !pMainTable )
		{
			cerr << "Found a history table: " << m_sName << " with no corresponding data table." << endl;
			throw "Database corrupt";
		}

		pMainTable->m_pTable_History = this;
		m_pTable_WeAreHistoryFor = pMainTable;
	}
}

void Table::GetDependencies()
{
	for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
	{
		Field *pField = (*it).second;
		if( pField->Name_get().substr(0,3)=="FK_" || pField->Name_get().substr(0,3)=="IK_" )
		{
			Table *pTable = m_pDatabase->GetTableFromForeignKey(pField);
			if( !pTable )
			{
				cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get() << " but I cannot find the table it is referring to." << endl;
				throw "Bad Schema";
			}

			// See if this is an indirect foreign key, meaning we have an IV_[identifier] field, which refers to a table
			// [identifier], which has a field Description, which, if it starts with PK_ means our IK_ field refers
			// to a foreign key in that table
			if( pField->Name_get().substr(0,3)=="IK_" )
			{
				Field *pField_Indirect=NULL;
				pField_Indirect = m_mapField_Find("IV_" + pField->Name_get().substr(3));

				ostringstream sql;
				sql << "SELECT Description FROM `" << pField->Name_get().substr(3) << "` WHERE Description LIKE 'PK_%'";
				PlutoSqlResult result_set;
				MYSQL_ROW row=NULL;
				if( (result_set.r=m_pDatabase->mysql_query_result(sql.str())) )
				{
					while( (row = mysql_fetch_row(result_set.r)) )
					{
						string FieldName = row[0];
						class Table *pTable = m_pDatabase->GetTableFromForeignKeyString(this,FieldName);
						Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get() );
						pField->m_listField_IReferTo_Indirectly.push_back(pField_PrimaryKey);
						pField_PrimaryKey->m_listField_ReferringToMe_Indirectly.push_back(pField);
					}
				}
			}
			else
			{
				Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get() );
				if( !pField_PrimaryKey )
				{
					cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get() << " but I cannot find the primary key in the table it is referring to." << endl;
					throw "Bad Schema";
				}

				pField->m_pField_IReferTo_Directly = pField_PrimaryKey;
				pField_PrimaryKey->m_listField_ReferringToMe_Directly.push_back(pField);
			}
		}
	}
}

#pragma warning("todo -- warn if other tables already have a timestamp that we can't use them")

void Table::GetChanges()
{
	ostringstream sql;
	sql << "SELECT psc_id, psc_batch, psc_user FROM `" << m_sName << "` WHERE psc_mod>0";

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( (result_set.r=m_pDatabase->mysql_query_result(sql.str())) )
	{
		while( (row = mysql_fetch_row(result_set.r)) )
		{
			TypeOfChange eTypeOfChange;
			if( row[0] == NULL )
				eTypeOfChange = toc_New;  // It's a new row if psc_id hasn't been assigned
			else
				eTypeOfChange = toc_Modify;
#pragma warning("get a list of id's from the server and determine which rows were deleted")

			ChangedRow *pChangedRow = new ChangedRow( this, eTypeOfChange, row[0] ? atoi(row[0]) : 0, row[1] ? atoi(row[1]) : 0, row[2] ? atoi(row[2]) : 0 );
			ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ];
			if( !pListChangedRow )
			{
				pListChangedRow = new ListChangedRow();
				m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ] = pListChangedRow;
			}
			pListChangedRow->push_back(pChangedRow);

			MapTable *pMapTable = g_GlobalConfig.m_mapUsers[ pChangedRow->m_psc_user ];
			if( !pMapTable )
			{
				pMapTable = new MapTable();
				g_GlobalConfig.m_mapUsers[ pChangedRow->m_psc_user ] = pMapTable;
			}
			(*pMapTable)[m_sName]=this;
		}
	}
}

bool Table::CheckIn(RA_Processor &ra_Processor,DCE::Socket *pSocket,enum TypeOfChange toc)
{
	vector<string> vectFields;
	for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
		vectFields.push_back( (*it).first );

	R_CommitTable r_CommitTable(m_sName,&vectFields);
	ra_Processor.SendRequest(&r_CommitTable,pSocket);

	if( r_CommitTable.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Failed to commit table: " << m_sName << endl;
		return false;
	}

	for(map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsers.begin();it!=g_GlobalConfig.m_mapUsers.end();++it)
	{
		int PK_Users = (*it).first;
		if( m_mapUsers2ChangedRowList.find(PK_Users) != m_mapUsers2ChangedRowList.end() )
		{
			ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[PK_Users];
			for(ListChangedRow::iterator itCR=pListChangedRow->begin();itCR!=pListChangedRow->end();++itCR)
			{
				ChangedRow *pChangedRow = *itCR;
				if( pChangedRow->m_eTypeOfChange != toc )
					continue;

				R_CommitRow r_CommitRow(pChangedRow);

				std::ostringstream sSQL;
				sSQL << "SELECT ";

				for( MapField::iterator it = m_mapField.begin();it != m_mapField.end(); ++it )
				{
					Field *pField = (*it).second;
					sSQL << "`" << pField->Name_get() << "`";
				}

				sSQL << " WHERE psc_id=" << pChangedRow->m_psc_id;

				ra_Processor.SendRequest(&r_CommitRow,pSocket);
				if( r_CommitRow.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
				{
					cerr << "Failed to commit row in table: " << m_sName << endl;
					return false;
				}
			}
		}
	}

	return true;
}
