/*
 Table
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

/**
 *
 * @file Table.cpp
 * @brief source file for the Table class
 *
 */
 
  
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
#include "R_GetAll_psc_id.h"
#include "R_UpdateTable.h"
#include "A_UpdateRow.h"
#include "sqlCVSprocessor.h"

#include <iomanip>
#include <sstream>

using namespace sqlCVS;

Table::Table( class Database *pDatabase, string sName )
{
	m_pDatabase=pDatabase;
	m_sName=sName;
	m_pRepository=NULL;
	m_pField_id=m_pField_batch=m_pField_user=m_pField_frozen=m_pField_mod=NULL;
	m_pTable_History=m_pTable_History_Mask=m_pTable_WeAreHistoryFor=m_pTable_WeAreHistoryMaskFor=NULL;
	m_pField_AutoIncrement=NULL;
	GetFields( );
	m_psc_id_next=1;
	m_psc_id_last_sync=m_psc_batch_last_sync=-1;
	if( m_pField_id )
	{
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result("SELECT max( psc_id ) FROM " + m_sName) ) && ( row = mysql_fetch_row( result_set.r ) ) )
			m_psc_id_next = row[0] ? atoi( row[0] )+1 : 1;
	}
}

void Table::GetFields( )
{
	m_pField_id=m_pField_batch=m_pField_user=m_pField_frozen=m_pField_mod=NULL;

	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
		delete ( *it ).second;
	m_mapField.clear( );
	m_listField_PrimaryKey.clear();

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	if( res.r=mysql_list_fields( m_pDatabase->MYSQL_get( ), m_sName.c_str( ), NULL ) )
	{
		int fields_count = mysql_num_fields( res.r );

		MYSQL_FIELD *mysql_fields = mysql_fetch_fields( res.r );

		for ( int i=0; i<fields_count; i++ )
		{
			Field *pField = new Field( this, mysql_fields+i );
			m_mapField[pField->Name_get( )]=pField;

			if( pField->m_iFlags & PRI_KEY_FLAG )
				m_listField_PrimaryKey.push_back( pField );
				
			if( pField->m_iFlags & AUTO_INCREMENT_FLAG )
				m_pField_AutoIncrement = pField;

			if( pField->Name_get( )=="psc_id" )
				m_pField_id = pField;
			else if( pField->Name_get( )=="psc_batch" )
				m_pField_batch = pField;
			else if( pField->Name_get( )=="psc_user" )
				m_pField_user = pField;
			else if( pField->Name_get( )=="psc_frozen" )
				m_pField_frozen = pField;
			else if( pField->Name_get( )=="psc_mod" )
				m_pField_mod = pField;
		}
	}
}

string Table::DefinesRepository( )
{
	if( m_sName.substr( 0, 4 )!="psc_" )
		return "";

	return m_sName.substr( 4, m_sName.length( ) - GetTrailingString( ).length( ) - 5 );
}

string Table::GetTrailingString( string sName )
{
	if( sName.length( )==0 )
		sName = m_sName;

	string::size_type lastpos=string::npos, pos=0;
	while( ( pos = sName.find( '_', pos ) ) != string::npos )
		lastpos = pos++; /**< Increment pos after assigning the value */
	return lastpos==string::npos ? "" : sName.substr( lastpos+1 );
}

Table::~Table( )
{
	int k=2;
}

void Table::TrackChanges_set( bool bOn )
{
	if( bOn )
	{
		if( !m_pField_id )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_id` INT( 11 );" );
		if( !m_pField_batch )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_batch` INT( 11 );" );
		if( !m_pField_user )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_user` INT( 11 );" );
		if( !m_pField_frozen )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_frozen` TINYINT( 1 ) default '0';" );
		if( !m_pField_mod )
		{
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_mod` timestamp( 14 ) default '0';" );
			m_pDatabase->threaded_mysql_query( "UPDATE `" + m_sName + "` SET `psc_mod`=0;" );
		}
	}
	else
	{
		if( m_pField_id )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_id`;" );
		if( m_pField_batch )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_batch`;" );
		if( m_pField_user )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_user`;" );
		if( m_pField_frozen )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_frozen`;" );
		if( m_pField_mod )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_mod`;" );
	}
	GetFields( );
}

void Table::HasFullHistory_set( bool bOn )
{
	if( m_bIsSystemTable )
		return; // We don't track these

	if( bOn )
	{
		if( m_pTable_History || m_pTable_History_Mask)
		{
			/** This shouldn't happen */
			cerr << "There is already a history table for: " << m_sName << endl;
			if( !AskYNQuestion("Drop it and make a new one?  You will lose all your history data.",false) )
				return;
			if( m_pTable_History )
			{
				m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
				m_pDatabase->m_mapTable_Remove( m_pTable_History->m_sName );
				delete m_pTable_History;
			}
			if( m_pTable_History_Mask )
			{
				m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
				m_pDatabase->m_mapTable_Remove( m_pTable_History_Mask->m_sName );
				delete m_pTable_History_Mask;
			}
		}
		TrackChanges_set( true ); /** This must be on */
		ostringstream sqlHistory,sqlHistoryMask;
		sqlHistory	<< "CREATE TABLE `" << m_sName << "_pschist`( " << endl;
		sqlHistoryMask	<< "CREATE TABLE `" << m_sName << "_pschmask`( " << endl;

		PlutoSqlResult result_set2;
		MYSQL_ROW row=NULL;
		if( !( result_set2.r=m_pDatabase->mysql_query_result( ( "DESCRIBE " +m_sName ).c_str( ) ) ) )
			throw "Error getting table info";

		while( ( row = mysql_fetch_row( result_set2.r ) ) )
		{
			string sField = row[0];
			string sType = row[1];

			sqlHistory << "`" << sField << "` " << sType << ", ";
			if( sField=="psc_batch" || sField=="psc_id" )
				sqlHistoryMask << "`" << sField << "` int(11), ";
			else
				sqlHistoryMask << "`" << sField << "` tinyint(1), ";
		}

		sqlHistory << "PRIMARY KEY ( psc_id, psc_batch )" << endl
			<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
		sqlHistoryMask << "PRIMARY KEY ( psc_id, psc_batch )" << endl
			<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;

		if( m_pDatabase->threaded_mysql_query( sqlHistory.str( ) )!=0 || m_pDatabase->threaded_mysql_query( sqlHistoryMask.str( ) )!=0 )
		{
			cerr << "sqlHistory Failed: " << sqlHistory.str( ) << endl;
			throw "Database error";
		}

		m_pTable_History = new Table( m_pDatabase, m_sName + "_pschist" );
		m_pTable_History->m_pTable_WeAreHistoryFor=this;
		m_pDatabase->m_mapTable_Add( m_pTable_History->m_sName, m_pTable_History );

		m_pTable_History_Mask = new Table( m_pDatabase, m_sName + "_pschmask" );
		m_pTable_History_Mask->m_pTable_WeAreHistoryMaskFor=this;
		m_pDatabase->m_mapTable_Add( m_pTable_History_Mask->m_sName, m_pTable_History_Mask );

		// Now populate it with the initial values
		PlutoSqlResult result_set3;
		if( !( result_set3.r=m_pDatabase->mysql_query_result( ("SELECT * FROM `" + m_sName + "`").c_str( ) ) ) )
			throw "Error getting table history";

		while( ( row = mysql_fetch_row( result_set3.r ) ) )
		{
			ostringstream sSql,sSqlMask;
			sSql << "INSERT INTO `" << m_sName << "_pschist` (";
			sSqlMask << "INSERT INTO `" << m_sName << "_pschmask` (";
			for(int i=0;i<result_set3.r->field_count;++i)
			{
				if( i!=0 )
				{
					sSql << ",";
					sSqlMask << ",";
				}
				sSql << "`" << result_set3.r->fields[i].name << "`";
				sSqlMask << "`" << result_set3.r->fields[i].name << "`";
			}
			sSql << ") VALUES (";
			sSqlMask << ") VALUES (";
			for(int i=0;i<result_set3.r->field_count;++i)
			{
				if( i!=0 )
				{
					sSql << ",";
					sSqlMask << ",";
				}
				if( strcmp(result_set3.r->fields[i].name,"psc_id")==0 || strcmp(result_set3.r->fields[i].name,"psc_batch")==0 )
				{
					if( row[i] )
						sSqlMask << row[i];
					else
						sSqlMask << "0";
				}
				else
					sSqlMask << "1";

				if( row[i] )
					sSql << "'" << StringUtils::SQLEscape(row[i]) << "'";
				else
				{
					if( strcmp(result_set3.r->fields[i].name,"psc_id")==0 || strcmp(result_set3.r->fields[i].name,"psc_batch")==0 )
						sSql << "0";
					else
						sSql << "NULL";
				}
			}
			sSql << ")";
			sSqlMask << ")";
			if( m_pDatabase->threaded_mysql_query(sSql.str())!=0 || m_pDatabase->threaded_mysql_query(sSqlMask.str())!=0 )
			{
				cerr << "Unable to add current values to history table" << endl;
				throw "Database error";
			}
		}
	}
	else
	{
		cout << "Are you sure you want to delete your history for " << m_sName << "?" << endl;
		if( !AskYNQuestion("The data will be permanently removed unless you made a backup.",false) )
			return;
		m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
		m_pDatabase->m_mapTable_Remove( m_pTable_History->m_sName );
		delete m_pTable_History;
		m_pTable_History=NULL;

		m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschmask`;" );
		m_pDatabase->m_mapTable_Remove( m_pTable_History_Mask->m_sName );
		delete m_pTable_History_Mask;
		m_pTable_History_Mask=NULL;
	}
}

void Table::MatchUpHistory( )
{
	if( GetTrailingString( )=="pschist" )
	{
		string BaseName = m_sName.substr( 0, m_sName.length( )-8 );
		Table *pMainTable = m_pDatabase->m_mapTable_Find( BaseName );
		if( !pMainTable )
		{
			cerr << "Found a history table: " << m_sName << " with no corresponding data table." << endl;
			throw "Database corrupt";
		}

		pMainTable->m_pTable_History = this;
		m_pTable_WeAreHistoryFor = pMainTable;
		m_bIsSystemTable = true;
	}
	if( GetTrailingString( )=="pschmask" )
	{
		string BaseName = m_sName.substr( 0, m_sName.length( )-9 );
		Table *pMainTable = m_pDatabase->m_mapTable_Find( BaseName );
		if( !pMainTable )
		{
			cerr << "Found a history mask table: " << m_sName << " with no corresponding data table." << endl;
			throw "Database corrupt";
		}

		pMainTable->m_pTable_History_Mask = this;
		m_pTable_WeAreHistoryMaskFor = pMainTable;
		m_bIsSystemTable = true;
	}

	if( m_pTable_WeAreHistoryFor || m_pTable_WeAreHistoryMaskFor )
	{
		// Go through all the columns in the other table and be sure we have columns for them in this one too
		Table *pTable = m_pTable_WeAreHistoryFor ? m_pTable_WeAreHistoryFor : m_pTable_WeAreHistoryMaskFor;
		for(MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it)
		{
			Field *pField = (*it).second;
			if( !m_mapField_Find(pField->Name_get()) )
			{
				// We need to add this field to our table
				ostringstream sql;
				if( m_pTable_WeAreHistoryMaskFor )
					sql << "ALTER TABLE `" + m_sName + "` ADD `" << pField->Name_get() << "`" << "tinyint(1)";
				else
				{
					// This is a bit round-about way of getting the field type for the add, but the
					// advantage of using DESCRIBE is that if MySql adds field types we don't need
					// to worry about keeping our own map to go from FieldInfo to a valid descriptor.
					PlutoSqlResult result_set;
					MYSQL_ROW row=NULL;
					if( !( result_set.r=m_pDatabase->mysql_query_result( ( "DESCRIBE " +pTable->Name_get() ).c_str( ) ) ) )
						throw "Error getting table info";

					while( ( row = mysql_fetch_row( result_set.r ) ) )
					{
						string sField = row[0];
						string sType = row[1];
						if( sField == pField->Name_get() )
						{
							sql << "ALTER TABLE `" + m_sName + "` ADD `" << sField << "` " << sType;
							break;
						}
					}
				}
				if( m_pDatabase->threaded_mysql_query( sql.str() )!=0 )
				{
					cerr << "Could not synchronize history table schema";
					throw "Database error";
				}
			}
		}
	}
}

void Table::GetDependencies( )
{
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;
		if( pField->Name_get( ).substr( 0, 3 )=="FK_" || pField->Name_get( ).substr( 0, 3 )=="IK_" )
		{
			Table *pTable = m_pDatabase->GetTableFromForeignKey( pField );
			if( !pTable )
			{
				cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get( ) << " but I cannot find the table it is referring to." << endl;
				throw "Bad Schema";
			}

			/**
			 * See if this is an indirect foreign key, meaning we have an IV_[identifier] field, which refers to a table
			 * [identifier], which has a field Description, which, if it starts with PK_ means our IK_ field refers
			 * to a foreign key in that table
			*/
			
			if( pField->Name_get( ).substr( 0, 3 )=="IK_" )
			{
				Field *pField_Indirect=NULL;
				pField_Indirect = m_mapField_Find( "IV_" + pField->Name_get( ).substr( 3 ) );

				ostringstream sql;
				sql << "SELECT Description FROM `" << pField->Name_get( ).substr( 3 ) << "` WHERE Description LIKE 'PK_%'";
				PlutoSqlResult result_set;
				MYSQL_ROW row=NULL;
				if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
				{
					while( ( row = mysql_fetch_row( result_set.r ) ) )
					{
						string FieldName = row[0];
						class Table *pTable = m_pDatabase->GetTableFromForeignKeyString( this, FieldName );
						Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get( ) );
						pField->m_listField_IReferTo_Indirectly.push_back( pField_PrimaryKey );
						pField_PrimaryKey->m_listField_ReferringToMe_Indirectly.push_back( pField );
					}
				}
			}
			else
			{
				Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get( ) );
				if( !pField_PrimaryKey )
				{
					cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get( ) << " but I cannot find the primary key in the table it is referring to." << endl;
					throw "Bad Schema";
				}

				pField->m_pField_IReferTo_Directly = pField_PrimaryKey;
				pField_PrimaryKey->m_listField_ReferringToMe_Directly.push_back( pField );
			}
		}
	}
}

#pragma warning( "todo -- warn if other tables already have a timestamp that we can't use them" )

bool Table::Update( RA_Processor &ra_Processor, DCE::Socket *pSocket )
{
	vector<string> vectFields;
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;
		if( pField==m_pField_id || pField==m_pField_batch || pField==m_pField_user || pField==m_pField_mod )
			continue;
		vectFields.push_back( ( *it ).first );
	}

	R_UpdateTable r_UpdateTable( m_sName, m_psc_batch_last_sync, m_psc_id_last_sync, &vectFields );
	ra_Processor.SendRequest( &r_UpdateTable, pSocket );
	if( r_UpdateTable.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		/**
		 * It's going to fall out of scope, so we don't want the processor to retry sending a deleted request
		 */
		ra_Processor.RemoveRequest( &r_UpdateTable ); 
		cerr << "Failed to update table: " << m_sName << endl;
		return false;
	}

	/**
	 * This will have already fired back actions, that did the actual update. All we need to do is update the psc_id and psc_batch 
	 */
	 
	if( r_UpdateTable.m_psc_id_last_sync>m_psc_id_last_sync )
		m_pRepository->psc_id_last_sync_set( this, r_UpdateTable.m_psc_id_last_sync );
	if( r_UpdateTable.m_psc_batch_last_sync>m_psc_batch_last_sync )
		m_pRepository->psc_batch_last_sync_set( this, r_UpdateTable.m_psc_batch_last_sync );

	return true;
}
void Table::GetChanges( R_UpdateTable *pR_UpdateTable )
{
#pragma warning( "take into account filter" )
	ostringstream sSql;
	sSql << "SELECT ";
	for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
		sSql << "`" << ( *pR_UpdateTable->m_pvectFields )[s] << "`, ";

	sSql << "psc_id, psc_batch, psc_user FROM " << m_sName << " WHERE psc_batch>" << pR_UpdateTable->m_psc_batch_last_sync;

	int i_psc_id_field = ( int ) pR_UpdateTable->m_pvectFields->size( );
	int i_psc_batch_field = i_psc_id_field+1;
	int i_psc_user_field = i_psc_batch_field+1;

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( !( result_set.r=m_pDatabase->mysql_query_result( sSql.str( ) ) ) )
	{
		cerr << "Cannot get changes from SQL: " << sSql.str( ) << endl;
		throw "Database error";
	}

	while( ( row = mysql_fetch_row( result_set.r ) ) )
	{
		int psc_id=atoi( row[i_psc_id_field] );
		int psc_batch=atoi( row[i_psc_batch_field] );
		int psc_user=row[i_psc_user_field] ? atoi( row[i_psc_user_field] ) : 0;
		if( psc_id>pR_UpdateTable->m_psc_id_last_sync )
			pR_UpdateTable->m_psc_id_last_sync = psc_id;
		if( psc_batch>pR_UpdateTable->m_psc_batch_last_sync )
			pR_UpdateTable->m_psc_batch_last_sync = psc_batch;

		A_UpdateRow *pa_UpdateRow = new A_UpdateRow( psc_id, psc_batch, psc_user );

		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
			pa_UpdateRow->m_vectValues.push_back( row[s] ? row[s] : NULL_TOKEN );
		pR_UpdateTable->AddActionsToResponse( pa_UpdateRow );
	}
}

void Table::GetChanges( )
{
	if( m_bIsSystemTable )
		return;

	int iAutoIncrField=-1; /** If there is an auto increment field in the key, we will point to it's "slot" here */
	ostringstream sql;
	sql << "SELECT psc_id, psc_batch, psc_user";

	int FieldNum=3;
	for( ListField::iterator it=m_listField_PrimaryKey.begin( );it!=m_listField_PrimaryKey.end( );++it )
	{
		Field *pField = *it;
		sql << ", `" << pField->Name_get( ) << "`";
		if( pField==m_pField_AutoIncrement )
			iAutoIncrField=FieldNum;
		FieldNum++;
	}

	sql << " FROM `" << m_sName << "` WHERE psc_mod>0";

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			TypeOfChange eTypeOfChange;
			if( row[0] == NULL )
				eTypeOfChange = toc_New; /** It's a new row if psc_id hasn't been assigned */
			else
				eTypeOfChange = toc_Modify;
#pragma warning( "get a list of id's from the server and determine which rows were deleted" )

			/** Add all the primary key values   */
			vector<string> vectPrimaryKeys;
			for( size_t s=0;s<m_listField_PrimaryKey.size( );++s )
				vectPrimaryKeys.push_back( row[s+3] );

			int iAutoIncrValue = iAutoIncrField>=0 ? atoi( row[iAutoIncrField] ) : 0;

			ChangedRow *pChangedRow = new ChangedRow( this, eTypeOfChange, row[0] ? atoi( row[0] ) : 0, row[1] ? atoi( row[1] ) : 0, row[2] ? atoi( row[2] ) : 0, iAutoIncrValue, vectPrimaryKeys );
			AddChangedRow( pChangedRow );
		}
	}
}

void Table::AddChangedRow( ChangedRow *pChangedRow )
{
	ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ];
	if( !pListChangedRow )
	{
		pListChangedRow = new ListChangedRow( );
		m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ] = pListChangedRow;
	}
	pListChangedRow->push_back( pChangedRow );

	MapTable *pMapTable = g_GlobalConfig.m_mapUsersTables[ pChangedRow->m_psc_user ];
	if( !pMapTable )
	{
		pMapTable = new MapTable( );
		g_GlobalConfig.m_mapUsersTables[ pChangedRow->m_psc_user ] = pMapTable;
	}
	( *pMapTable )[m_sName]=this;
}

bool Table::ConfirmDependencies( )
{
	bool bDependenciesMet=true;

	for( map<int, MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int PK_Users = ( *it ).first;
		if( m_mapUsers2ChangedRowList.find( PK_Users ) != m_mapUsers2ChangedRowList.end( ) )
		{
			ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[PK_Users];
			for( ListChangedRow::iterator itCR=pListChangedRow->begin( );itCR!=pListChangedRow->end( );++itCR )
			{
				ChangedRow *pChangedRow = *itCR;

				/**
				 * Go through all our fields. If any are foreign keys to another table, 
				 * that row in the other table must be checked in as well 
				 */
				
				for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
				{
					Field *pField = ( *it ).second;
					if( pField->m_pField_IReferTo_Directly && !ConfirmDependency( pChangedRow, pField, pField->m_pField_IReferTo_Directly ) )
					{
						bDependenciesMet=false;
						cerr << "Field: " << m_sName << ":" << pField->Name_get( ) << " Refers to: " << pField->m_pField_IReferTo_Directly->m_pTable->Name_get( ) << " which is not being checked in." << endl;
					}
					for( ListField::iterator itList=pField->m_listField_IReferTo_Indirectly.begin( );itList!=pField->m_listField_IReferTo_Indirectly.end( );++itList )
					{
						Field *pField_Indirect = *itList;
						if( !ConfirmDependency( pChangedRow, pField, pField_Indirect ) )
						{
							bDependenciesMet=false;
							cerr << "Field: " << m_sName << ":" << pField->Name_get( ) << " indirectly refers to: " << pField_Indirect->m_pTable->Name_get( ) << " which is not being checked in." << endl;
						}
					}
				}
			}
		}
	}
	return bDependenciesMet;
}

bool Table::ConfirmDependency( ChangedRow *pChangedRow, Field *pField_Referring, Field *pField_ReferredTo )
{
	/** If we're checking the table that we're referring to, then we're fine. Any changes will be propagated. */
	
	if( g_GlobalConfig.m_mapTable.find( pField_ReferredTo->m_pTable->Name_get( ) )!=g_GlobalConfig.m_mapTable.end( ) )
		return true;

	/** We refer to a table that's not being checked in. That's okay if the row we're referncing has not been modified. 
	 * If it has changed, then we must check in that table too, otherwise we will be uploading a row to the server that references
	 * a row the server doesn't have, and the server's database will become corrupted. First we have to get the value of the field here.
	 */

	if( !pField_ReferredTo->m_pTable->m_pField_mod )
	{
		cerr << "Table: " << pField_ReferredTo->m_pTable->Name_get( ) << " does not have a psc_mod field. I can't tell if the dependency is met" << endl;
		return false;
	}
#pragma warning( "THIS ISN'T GOING TO WORK WITH INDIRECT FIELDS LIKE THIS" )
	std::ostringstream sSQL;
	sSQL << "SELECT `" << pField_Referring->Name_get( ) << "` FROM " << m_sName << pChangedRow->GetWhereClause( );
	PlutoSqlResult res, res2;
	MYSQL_ROW row=NULL;
	res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r || res.r->row_count!=1 )
	{
		cerr << "Problem retrieving row with query: " << sSQL << endl;
		return false;
	}
	row = mysql_fetch_row( res.r );

	if( !row[0] )
		return true; /** It's a null value */

	sSQL.str( "" );
	sSQL << "SELECT psc_mod FROM " << pField_ReferredTo->m_pTable->Name_get( ) << " WHERE `" << pField_ReferredTo->Name_get( ) << "`=" << row[0];
	res2.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res2.r || res2.r->row_count!=1 )
	{
		cerr << "Problem retrieving row with query: " << sSQL << endl;
		return false;
	}
	row = mysql_fetch_row( res2.r );
	if( !row[0] || atoi( row[0] )==0 )
		return true;

	cerr << "Table: " << pField_ReferredTo->m_pTable->Name_get( ) << " needs to be checked in." << endl;
	return false;
}

bool Table::DetermineDeletions( RA_Processor &ra_Processor, string Connection, DCE::Socket **ppSocket )
{
	if( m_psc_id_last_sync<1 )
		return true; /** We haven't synced any records with the server anyway */
	R_GetAll_psc_id r_GetAll_psc_id( m_sName, m_psc_id_last_sync );
	ra_Processor.AddRequest( &r_GetAll_psc_id );
	while( ra_Processor.SendRequests( Connection, ppSocket ) );

	if( r_GetAll_psc_id.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server" << endl;
		throw "Communication error";
	}
	std::ostringstream sSQL;

	sSQL << "SELECT psc_id FROM " << m_sName << " ORDER BY psc_id";
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
	size_t pos=0; // A position in the vector
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query: " << sSQL << endl;
		throw "Database error";
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL << endl;
				throw "Database error";
			}
			/** If the value of our local row[] is > than the server's vect, then we deleted some records locally */
			while( pos<r_GetAll_psc_id.m_vectAll_psc_id.size( ) && atoi( row[0] )>r_GetAll_psc_id.m_vectAll_psc_id[pos] )
			{
				ChangedRow *pChangedRow = new ChangedRow( this, r_GetAll_psc_id.m_vectAll_psc_id[pos] );
				AddChangedRow( pChangedRow );
				pos++;
			} 
			/** If the value in the server's vect is > than our local row[], then the server deleted this row */
			if( pos>=r_GetAll_psc_id.m_vectAll_psc_id.size( ) || r_GetAll_psc_id.m_vectAll_psc_id[pos]>atoi( row[0] ) )
			{
				m_vectRowsToDelete.push_back( atoi( row[0] ) );
				continue; 
			}
			if( pos<r_GetAll_psc_id.m_vectAll_psc_id.size( ) && atoi( row[0] )==r_GetAll_psc_id.m_vectAll_psc_id[pos] )
			{
				/** So far so good, keep going */
				pos++;
				continue;
			}
		}
		if( pos==r_GetAll_psc_id.m_vectAll_psc_id.size( ) )
			return true; /** We finished at the same time, nothing more to do */

		/** There are still more rows in the server's vect. We must have deleted them */
		
		for( ;pos<r_GetAll_psc_id.m_vectAll_psc_id.size( );++pos )
		{
			ChangedRow *pChangedRow = new ChangedRow( this, r_GetAll_psc_id.m_vectAll_psc_id[pos] );
			AddChangedRow( pChangedRow );
		}
	}
	return true;
}

bool Table::CheckIn( RA_Processor &ra_Processor, DCE::Socket *pSocket, enum TypeOfChange toc )
{
	vector<string> vectFields;
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;
		
		/** If this a new record, don't add the auto increment field */
		
		if( pField==m_pField_id || pField==m_pField_batch || pField==m_pField_user || pField==m_pField_mod || pField==m_pField_frozen || ( pField==m_pField_AutoIncrement && toc==toc_New ) )
			continue;
		vectFields.push_back( ( *it ).first );
	}

	R_CommitTable r_CommitTable( m_sName, &vectFields );
	ra_Processor.SendRequest( &r_CommitTable, pSocket );
	if( r_CommitTable.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_CommitTable ); /** It's going to fall out of scope, so we don't the processor to retry */
		cerr << "Failed to commit table: " << m_sName << endl;
		return false;
	}

	for( map<int, MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int PK_Users = ( *it ).first;
		if( m_mapUsers2ChangedRowList.find( PK_Users ) != m_mapUsers2ChangedRowList.end( ) )
		{
			ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[PK_Users];
			for( ListChangedRow::iterator itCR=pListChangedRow->begin( );itCR!=pListChangedRow->end( );++itCR )
			{
				ChangedRow *pChangedRow = *itCR;
				if( pChangedRow->m_eTypeOfChange != toc )
					continue;

				R_CommitRow r_CommitRow( pChangedRow );

				std::ostringstream sSQL;
				sSQL << "SELECT ";

				for( size_t s=0;s<vectFields.size( );++s )
				{
					if( s>0 )
						sSQL << ", ";
					sSQL << "`" << vectFields[s] << "`";
				}

				sSQL << " FROM " << m_sName;

				sSQL << pChangedRow->GetWhereClause( );

				PlutoSqlResult res;
				MYSQL_ROW row=NULL;
				res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
				if( !res.r || res.r->row_count!=1 )
				{
					cerr << "Problem retrieving row with query: " << sSQL << endl;
					return false;
				}
				row = mysql_fetch_row( res.r );
				for( size_t s=0;s<vectFields.size( );++s )
				{
					r_CommitRow.m_vectValues.push_back( row[s] ? row[s] : NULL_TOKEN );
				}

				ra_Processor.SendRequest( &r_CommitRow, pSocket );
#pragma warning( "take into account return value of 'record frozen'" )
				if( r_CommitRow.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
				{
					 /** It's going to fall out of scope, so we don't want the processor to retry sending a deleted request */
					ra_Processor.RemoveRequest( &r_CommitRow );
					cerr << "Failed to commit row in table: " << m_sName << endl;
					return false;
				}

				if( toc==toc_New )
				{
					if( !r_CommitRow.m_psc_id_new )
					{
						cerr << "Didn't get back a valid psc_id for: " << m_sName << endl;
						return false;
					}
					sSQL.str( "" );
					sSQL << "UPDATE " << m_sName << " SET psc_id=" << r_CommitRow.m_psc_id_new << pChangedRow->GetWhereClause( );

					if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
					{
						cerr << "SQL failed: " << sSQL.str( );
						return false;
					}

					if( r_CommitRow.m_psc_id_new>m_psc_id_last_sync )
						m_pRepository->psc_id_last_sync_set( this, r_CommitRow.m_psc_id_new );
					if( r_CommitRow.m_psc_batch_new>m_psc_batch_last_sync )
						m_pRepository->psc_batch_last_sync_set( this, r_CommitRow.m_psc_batch_new );
				}

				if( r_CommitRow.m_psc_batch_new && r_CommitRow.m_psc_batch_new!=r_CommitRow.m_psc_batch )
				{
					sSQL.str( "" );
					sSQL << "UPDATE " << m_sName << " SET psc_batch=" << r_CommitRow.m_psc_batch_new << pChangedRow->GetWhereClause( );
					if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
					{
						cerr << "SQL failed: " << sSQL.str( );
						return false;
					}
				}

				/** Do this last since the 2 above may be relying on the old primary key */
				
				if( r_CommitRow.m_iNewAutoIncrID && r_CommitRow.m_iNewAutoIncrID!=r_CommitRow.m_iOriginalAutoIncrID )
				{
					if( !m_pField_AutoIncrement )
						throw ( "Internal error: Trying to set a non existant auto increment in table: " + m_sName ).c_str( );

					sSQL.str( "" );
					sSQL << "UPDATE " << m_sName << " SET " << m_pField_AutoIncrement->Name_get() << "=" << r_CommitRow.m_iNewAutoIncrID << pChangedRow->GetWhereClause( );
					if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
					{
						cerr << "SQL failed: " << sSQL.str( );
						return false;
					}
				}

				sSQL.str( "" );
				sSQL << "UPDATE " << m_sName << " SET psc_mod=0 WHERE psc_id=" << ( toc==toc_New ? r_CommitRow.m_psc_id_new : r_CommitRow.m_psc_id );
				if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
				{
					cerr << "SQL failed: " << sSQL.str( );
					return false;
				}

				/**
				 * First update this table's auto increment field before setting the psc_mod to 0. Then, handle the 
				 * propagate. That way if this table contains a foreign key to itself, or is somehow modified by 
				 * the propagate, it's modification flag will be set again
				 */
				 
				if( r_CommitRow.m_iNewAutoIncrID && r_CommitRow.m_iNewAutoIncrID!=r_CommitRow.m_iOriginalAutoIncrID )
				{
					PropagateUpdatedField( m_pField_AutoIncrement, StringUtils::itos( r_CommitRow.m_iNewAutoIncrID ), StringUtils::itos( r_CommitRow.m_iOriginalAutoIncrID ), pChangedRow );
				}
			}
		}
	}

	return true;
}

void Table::PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow )
{
	for( ListField::iterator itList=pField_Changed->m_listField_ReferringToMe_Directly.begin( );itList!=pField_Changed->m_listField_ReferringToMe_Directly.end( );++itList )
	{
		Field *pField_FK = *itList;
		PropagateUpdatedField( pField_Changed, NewValue, OldValue, pChangedRow, pField_FK );
	}
/** @test
	for( ListField::iterator itList=pField_Changed->m_listField_ReferringToMe_Indirectly.begin( );itList!=pField_Changed->m_listField_ReferringToMe_Indirectly.end( );++itList )
	{
#pragma warning( "HANDLE INDIRECT" )
		Field *pField_FK = *itList;
		PropageUpdatedField( pField_Changed, pChangedRow, pField_FK );
	}
*/
}

void Table::PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow, Field *pField_FK )
{
	std::ostringstream sSQL;
	sSQL << "UPDATE " << pField_FK->m_pTable->Name_get( ) << " SET " << pField_FK->Name_get( ) << "='" << NewValue << "'" << " WHERE " << pField_FK->Name_get( ) << "='" << OldValue << "'";
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
	{
		cerr << "Failed to propagate change: " << sSQL.str( ) << endl;
		throw "Failed to propagate change";
	}
}

void Table::UpdateRow( A_UpdateRow *pA_UpdateRow, R_UpdateTable *pR_UpdateTable, sqlCVSprocessor *psqlCVSprocessor )
{
	std::ostringstream sSQL;

	/** See if this is a new row, or an updated one */
	if( pA_UpdateRow->m_psc_id>m_psc_id_last_sync )
	{
		sSQL << "INSERT INTO " << m_sName << " ( ";

		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
			sSQL << "`" << ( *pR_UpdateTable->m_pvectFields )[s] << "`, "; 

		sSQL << "psc_id, psc_batch, psc_user ) VALUES( ";

		for( size_t s=0;s<pA_UpdateRow->m_vectValues.size( );++s )
		{
			if( pA_UpdateRow->m_vectValues[s]==NULL_TOKEN )
				sSQL << "NULL, ";
			else
				sSQL << "'" << StringUtils::SQLEscape( pA_UpdateRow->m_vectValues[s] ) << "', ";
		}

		sSQL << pA_UpdateRow->m_psc_id << ", " << pA_UpdateRow->m_psc_batch << ", " << pA_UpdateRow->m_psc_user << " )";

		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "Failed to insert row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
	}
	else
	{
		sSQL << "UPDATE " << m_sName << " SET ";

		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
		{
			sSQL << "`" << ( *pR_UpdateTable->m_pvectFields )[s] << "`="; 
			if( pA_UpdateRow->m_vectValues[s]==NULL_TOKEN )
				sSQL << "NULL, ";
			else
				sSQL << "'" << StringUtils::SQLEscape( pA_UpdateRow->m_vectValues[s] ) << "', ";
		}
		sSQL << "psc_batch=" << pA_UpdateRow->m_psc_batch << ", psc_user=" << pA_UpdateRow->m_psc_user << " WHERE psc_id=" << pA_UpdateRow->m_psc_id;
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "Failed to update row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
	}
}

void Table::UpdateRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen, int &psc_user )
{
	// If the user is a supervisor, he can do anything, otherwise be sure we're allowed to do this
	if( !psqlCVSprocessor->m_bSupervisor )
	{
		std::ostringstream sSQL;
		sSQL << "SELECT psc_user,psc_frozen FROM " << m_sName << " WHERE psc_id=" << pR_CommitRow->m_psc_id;
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result(sSQL.str()) ) && ( row = mysql_fetch_row( result_set.r ) ) )
		{
			if( row[1] && row[1][0]=='1' )
				bFrozen=true;
			else
				bFrozen=false;

			int i = row[0] ? atoi(row[0]) : 0;
			if( i && psqlCVSprocessor->m_mapValidatedUsers.find(i)==psqlCVSprocessor->m_mapValidatedUsers.end() )
				psc_user = i;
			else
				psc_user = 0;

			if( bFrozen || psc_user )
			{
				pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->UnauthorizedBatch(psc_user);
				AddToHistory( pR_CommitRow, psqlCVSprocessor );
				return;
			}
		}
		else
			throw "Error: row not found";
	}

	std::ostringstream sSQL;
	sSQL << "UPDATE " << m_sName << " SET ";

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
	{
		sSQL << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`="; 
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSQL << "NULL, ";
		else
			sSQL << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";
	}
	sSQL << "psc_batch=" << psqlCVSprocessor->m_i_psc_batch << " WHERE psc_id=" << pR_CommitRow->m_psc_id; /** @todo -- AND NOT FROZEN */
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
	{
		cerr << "Failed to insert row: " << sSQL.str( ) << endl;
		throw "Failed to insert row";
	}
	pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->m_i_psc_batch;
	AddToHistory( pR_CommitRow, psqlCVSprocessor );
}

void Table::AddRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor )
{
	std::ostringstream sSQL;
	sSQL << "INSERT INTO " << m_sName << "( ";

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
		sSQL << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";

	sSQL << "psc_id, psc_batch ) VALUES( ";

	for( size_t s=0;s<pR_CommitRow->m_vectValues.size( );++s )
	{
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSQL << "NULL, ";
		else
			sSQL << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";
	}

	sSQL << m_psc_id_next << ", " << psqlCVSprocessor->m_i_psc_batch << " )"; /** @warning batch # todo - hack */

	pR_CommitRow->m_psc_id_new = m_psc_id_next;
	pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->m_i_psc_batch;

	if( m_pField_AutoIncrement )
	{
		if( !( pR_CommitRow->m_iNewAutoIncrID=m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) ) ) )
		{
			cerr << "Failed to insert row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
	}
	else
	{
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "Failed to insert row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
		pR_CommitRow->m_iNewAutoIncrID=0;
	}

	AddToHistory( pR_CommitRow, psqlCVSprocessor );
	m_psc_id_next++;
}

void Table::GetCurrentValues(int psc_id,map<string,string> *mapCurrentValues)
{
	std::ostringstream sSql;
	sSql << "SELECT * FROM " << m_sName << " WHERE psc_id=" << psc_id;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSql.str() ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			for(int i=0;i<result_set.r->field_count;++i)
			{
				if( row[i] )
					(*mapCurrentValues)[result_set.r->fields[i].name] = row[i];
				else
					(*mapCurrentValues)[result_set.r->fields[i].name] = NULL_TOKEN;
			}
		}
	}
}

void Table::AddToHistory( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor )
{
	if( !m_pTable_History || !m_pTable_History_Mask )
		return;

	map<string,string> mapCurrentValues;
	GetCurrentValues(pR_CommitRow->m_psc_id,&mapCurrentValues);
	int NumChangedFields=0;

	std::ostringstream sSqlHistory,sSqlMask;
	sSqlHistory << "INSERT INTO " << m_pTable_History->Name_get( ) << "( ";
	sSqlMask << "INSERT INTO " << m_pTable_History_Mask->Name_get( ) << "( ";

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
		sSqlHistory << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";

	/** If this isn't a new row, any auto-increment fields are passed like normal  */
	
	if( m_pField_AutoIncrement && pR_CommitRow->m_eTypeOfChange==toc_New )
		sSqlHistory << m_pField_AutoIncrement->Name_get( ) << ", ";

	sSqlHistory << "psc_id, psc_batch ) VALUES( ";

	for( size_t s=0;s<pR_CommitRow->m_vectValues.size( );++s )
	{
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSqlHistory << "NULL, ";
		else
			sSqlHistory << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";

		if( mapCurrentValues[( *psqlCVSprocessor->m_pvectFields )[s]] != pR_CommitRow->m_vectValues[s] )
		{
			sSqlMask << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";
			NumChangedFields++;
		}
	}

	sSqlMask << "psc_id, psc_batch ) VALUES( ";
	for(int i=0;i<NumChangedFields;++i)
		sSqlMask << "1,";

	if( m_pField_AutoIncrement && pR_CommitRow->m_eTypeOfChange==toc_New )
		sSqlHistory << pR_CommitRow->m_iNewAutoIncrID << ", ";

	sSqlHistory << ( pR_CommitRow->m_eTypeOfChange==toc_New ? pR_CommitRow->m_psc_id_new : pR_CommitRow->m_psc_id ) << ", " << psqlCVSprocessor->m_i_psc_batch << " )"; // batch # todo - hack
	sSqlMask << ( pR_CommitRow->m_eTypeOfChange==toc_New ? pR_CommitRow->m_psc_id_new : pR_CommitRow->m_psc_id ) << ", " << psqlCVSprocessor->m_i_psc_batch << " )"; // batch # todo - hack
	if( m_pDatabase->threaded_mysql_query( sSqlHistory.str( ) )!=0 || m_pDatabase->threaded_mysql_query( sSqlMask.str( ) )!=0 )
	{
		cerr << "Failed to insert history row: " << sSqlHistory.str( ) << endl;
		throw "Failed to insert row";
	}

}



bool Table::Dump( SerializeableStrings &str )
{
	str.m_vectString.push_back( m_sName );

	std::ostringstream sSQL;
	sSQL << "DESCRIBE " << m_sName;
	PlutoSqlResult result_set2;
	MYSQL_ROW row=NULL;
	if( !( result_set2.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		return false;

	string sFieldList;
	str.m_vectString.push_back( StringUtils::itos( ( int ) ( result_set2.r->row_count ) ) );
	while( ( row = mysql_fetch_row( result_set2.r ) ) )
	{
		if( sFieldList.length( ) )
			sFieldList += ", ";

		sFieldList += string( "`" ) + row[0] + "`";
		for( size_t s=0;s<6;++s )
			str.m_vectString.push_back( row[s] ? row[s] : NULL_TOKEN );
	}

	sSQL.str( "" );
	sSQL << "SELECT " << sFieldList << " FROM " << m_sName << " " << m_sFilter;
	PlutoSqlResult result_set;
	if( !( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		return false;

	cout << "Table: " << m_sName << "\t\tDumping " << result_set.r->row_count << " rows" << endl;

	str.m_vectString.push_back( StringUtils::itos( ( int ) result_set.r->row_count ) );
	while( ( row = mysql_fetch_row( result_set.r ) ) )
	{
		for( size_t s=0;s<m_mapField.size( );++s )
			str.m_vectString.push_back( row[s] ? row[s] : NULL_TOKEN );
	}

	return true;
}

bool Table::ShowChanges()
{
	while(true)
	{
		cout << "Changes for table: " << m_sName << endl;
		cout << "    User     New   Mod   Del" << endl;

		for(map<int, ListChangedRow *>::iterator itCR=m_mapUsers2ChangedRowList.begin();itCR!=m_mapUsers2ChangedRowList.end();++itCR)
		{
			int iUser = (*itCR).first;
			ListChangedRow *pListChangedRow = (*itCR).second;

			cout << setw( 7 ) << iUser << " ";

			int iNew=0,iMod=0,iDel=0;
			for(ListChangedRow::iterator itLCR=pListChangedRow->begin();itLCR!=pListChangedRow->end();++itLCR)
			{
				ChangedRow *pChangedRow = *itLCR;
				if( pChangedRow->m_eTypeOfChange==toc_New )
					iNew++;
				else if( pChangedRow->m_eTypeOfChange==toc_Modify )
					iMod++;
				else if( pChangedRow->m_eTypeOfChange==toc_Delete )
					iDel++;
			}
			cout << setw( 6 ) << iNew << setw( 6 ) << iMod << setw( 6 ) << iDel << endl;
		}

		cout << "What user do you want more detail on?  Enter 'b' to go back, 'q' to quit" << endl;
		string sUser;
		cin >> sUser;
		if( sUser=="b" || sUser=="B" )
			return true;
		if( sUser=="q" || sUser=="Q" )
			return false;

		if( !ShowChanges( atoi(sUser.c_str()) ) )
			return false;
	}
}

bool Table::ShowChanges(int psc_user)
{
	ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList_Find(psc_user);
	while(true)
	{
		cout << "Changes to table: " << m_sName << " by user: " << psc_user << endl;
		int iNumFields=4,iColumnWidth=18;

		ostringstream sql;
		sql << "SELECT psc_id,";
		int iFieldCount=0;
		for(MapField::iterator it = m_mapField.begin();it != m_mapField.end();++it)
		{
			Field *pField = (*it).second;
			if( iFieldCount )
				sql << ",";
			sql << "`" << pField->Name_get() << "`";
			if( ++iFieldCount>=iNumFields )
				break;
		}

		sql << " FROM " << m_sName << " WHERE psc_id IN (";
		bool bFirst=true;
		for(ListChangedRow::iterator it = pListChangedRow->begin();it != pListChangedRow->end();++it)
		{
			if( bFirst )
				bFirst=false;
			else
				sql << ",";
			ChangedRow *pChangedRow = *it;
			sql << pChangedRow->m_psc_id;
		}
		sql << ")";

		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
		{
			cout << "psc_id  ";
			for(int i=1;i<result_set.r->field_count;++i)
			{
				string sField = result_set.r->fields[i].name;
				if( sField.length()>iColumnWidth )
					cout << sField.substr(0,iColumnWidth);
				else
					cout << sField << StringUtils::RepeatChar( ' ', iColumnWidth - sField.length( ) );
			}
			cout << endl;

			while( ( row = mysql_fetch_row( result_set.r ) ) )
			{
				cout << setw(8) << row[0];
				for(int i=1;i<result_set.r->field_count;++i)
				{
					string sValue = row[i] ? row[i] : NULL_TOKEN;
					if( sValue.length()>iColumnWidth )
						cout << sValue.substr(0,iColumnWidth);
					else
						cout << sValue << StringUtils::RepeatChar( ' ', iColumnWidth - sValue.length( ) );
				}
				cout << endl;
			}
		}

		cout << "What row do you want more detail on?  Enter 'b' to go back, 'q' to quit" << endl;
		string sRow;
		cin >> sRow;
		if( sRow=="b" || sRow=="B" )
			return true;
		if( sRow=="q" || sRow=="Q" )
			return false;

		if( !ShowChanges( atoi(sRow.c_str()) ) )
			return false;
	}
}
