/**
 *
 * @file Repository.cpp
 * @brief source file for the Repository class
 *
 */
 
 /**
  *
  * Copyright information goes here
  *
  */
  
#include "PlutoUtils/CommonIncludes.h"	

#include "CommonFunctions.h"
#include <iostream>
#include <sstream>

#include "Database.h"
#include "Repository.h"
#include "Table.h"

#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "R_UpdateRepository.h"
#include "ChangedRow.h"

using namespace sqlCVS;

void Repository::AddDefinitionTable( class Table *pTable )
{
	if( pTable->GetTrailingString( )=="repset" )
		m_pTable_Setting = pTable;
	else if( pTable->GetTrailingString( )=="bathdr" )
		m_pTable_BatchHeader = pTable;
	else if( pTable->GetTrailingString( )=="batdet" )
		m_pTable_BatchDetail = pTable;
	else if( pTable->GetTrailingString( )=="tables" )
		m_pTable_Tables = pTable;
	else if( pTable->GetTrailingString( )=="schema" )
		m_pTable_Schema = pTable;
	else
	{
		cerr << "Cannot figure out what table: " << pTable->Name_get( ) << " is.";
		throw( "Schema problem" );
	}

	/**  We found a match, and this is a system table */
	pTable->SetRepository( this, true );
}

void Repository::MatchUpTables( )
{
	if( m_pTable_Setting==NULL || m_pTable_BatchHeader==NULL || m_pTable_BatchDetail==NULL || m_pTable_Tables==NULL )
	{
		cerr << "Could not find all the system tables for Repository: " << m_sName << endl
			<< "I need _repset, _bathdr, _batdet, _tables" << endl
			<< "This repository has been corrupted, and will need to be rebuilt in order to continue." << endl;
		if( AskYNQuestion("Rebuild repository: " + m_sName,false) )
		{
		 	MapTable mapTable; /** Don't add any tables, just setup an empty repository */
			Setup( );
		}
		else
			throw "Repository corrupt";
	}

	string Tablename = "psc_" + m_sName + "_tables";

	ostringstream sql;
	sql << "SELECT Tablename,filter FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			Table *pTable = m_pDatabase->m_mapTable_Find( row[0] );
			if( !pTable )
			{
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it no longer exists in the database." << endl
					<< "I must delete it from the repository's database to continue." << endl;

				if( !AskYNQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		    m_pDatabase->threaded_mysql_query( sql.str( ) );
				continue; /**< Nothing more to do */
			}
			if( pTable->Repository_get( ) && pTable->Repository_get( )!=this )
			{
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it already exists in the repository: " << pTable->Repository_get( )->Name_get( ) << endl
					<< "I must delete it from this repository's database to continue." << endl;

				if( !AskYNQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		    m_pDatabase->threaded_mysql_query( sql.str( ) );
				continue; /**< Nothing more to do */
			}
			/** Go ahead and add the table */
			m_mapTable[ pTable->Name_get( ) ] = pTable;
			pTable->SetRepository( this, false );
			pTable->m_sFilter = row[1] ? row[1] : "";
		}
	}
}

void Repository::Setup( )
{
	if( !m_pTable_Setting )
		m_pTable_Setting = CreateSettingTable( );
	if( !m_pTable_BatchHeader )
		m_pTable_BatchHeader = CreateBatchHeaderTable( );
	if( !m_pTable_BatchDetail )
		m_pTable_BatchDetail = CreateBatchDetailTable( );
	if( !m_pTable_Tables )
		m_pTable_Tables = CreateTablesTable( );
	if( !m_pTable_Schema )
		m_pTable_Tables = CreateSchemaTable( );

	string Tablename = "psc_" + m_sName + "_tables"; /**< Our _tables table */

	/** Go through all the tables in the map and be sure we have them in our tracking table, and also in our internal map */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		pTable->SetRepository( this, false );
		pTable->TrackChanges_set( true );
		m_mapTable[ pTable->Name_get( ) ] = pTable;
		ostringstream sql;

		sql << "SELECT PK_" << Tablename << " FROM `" << Tablename << "` WHERE Tablename='" << pTable->Name_get( ) << "'";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( !( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) || !( row = mysql_fetch_row( result_set.r ) ) )
		{
			sql.str( "" );
			sql << "INSERT INTO `" << Tablename << "` ( Tablename ) VALUES( '" << pTable->Name_get( ) << "' )";
			if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
			{
				cerr << "SQL failed: " << sql.str( ) << endl;
				throw "Database Error";
			}
		}
	}

	/**
	 * If we have any tables in our existing internal map that aren't in this new list, remove them. 
	 * They're not part of the repository any more 
	 */
	
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( ); )
	{
		if( g_GlobalConfig.m_mapTable.find( ( *it ).first ) == g_GlobalConfig.m_mapTable.end( ) )
		{
			( *it ).second->TrackChanges_set( false );
      m_mapTable.erase( it++ ); /**< This isn't in the repository any more */
		}
		else
			it++;
	}

	/** If we have any tables in the _tables table that aren't in this new list, remove them. They're not part of the repository any more */
	
	ostringstream sql;
	sql << "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			if( g_GlobalConfig.m_mapTable.find( row[0] ) == g_GlobalConfig.m_mapTable.end( ) )
			{
				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		    m_pDatabase->threaded_mysql_query( sql.str( ) );
			}
		}
	}
}

string Repository::GetSetting(string Setting,string Default)
{
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "SELECT Value FROM `" << Tablename << "` WHERE Setting='" + StringUtils::SQLEscape(Setting) + "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) && ( row = mysql_fetch_row( result_set.r ) ) )
		return row[0];

	if( Default.length() )
	{
		sql.str("");
		sql	<< "INSERT INTO `" << Tablename << "` (Setting,Value) VALUES('" + StringUtils::SQLEscape(Setting) + "','"  + StringUtils::SQLEscape(Default) + "')";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
		{
			cerr << "SQL Failed: " << sql.str( ) << endl;
			throw "Database error";
		}
	}

	return Default;
}

void Repository::SetSetting(string Setting,string Value)
{
	GetSetting(Setting,Value); // This will add the record if it's not already there

	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "UPDATE `" << Tablename << "` SET Setting='" + StringUtils::SQLEscape(Setting) + "',Value='"  + StringUtils::SQLEscape(Value) + "' WHERE Setting='" + StringUtils::SQLEscape(Setting) + "'";
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}
}

Table *Repository::CreateSettingTable( )
{
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Setting` varchar( 30 ) NOT NULL default '', " << endl
		<< "`Value` text default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

Table *Repository::CreateSchemaTable( )
{
	string Tablename = "psc_" + m_sName + "_schema";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Value` text NOT NULL default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}
class Table *Repository::CreateBatchHeaderTable( )
{
	string Tablename = "psc_" + m_sName + "_bathdr";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Value` varchar( 30 ) NOT NULL default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

class Table *Repository::CreateBatchDetailTable( )
{
	string Tablename = "psc_" + m_sName + "_batdet";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Value` varchar( 30 ) NOT NULL default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

class Table *Repository::CreateTablesTable( )
{
	string Tablename = "psc_" + m_sName + "_tables";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Tablename` varchar( 60 ) NOT NULL default '', " << endl
		<< "`filter` varchar( 100 ), " << endl
		<< "`frozen` TINYINT( 1 ) DEFAULT '0', " << endl
		<< "`last_psc_id` INT( 11 ) DEFAULT '0', " << endl
		<< "`last_psc_batch` INT( 11 ) DEFAULT '0', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

void Repository::Remove( )
{
	ostringstream sql;
	string Tablename = "psc_" + m_sName + "_repset";
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_bathdr";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_batdet";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_tables";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchDetail=m_pTable_Tables=NULL;
}

bool Repository::DetermineDeletions( )
{
	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!!  */
	RA_Processor ra_Processor( ClientID, SoftwareVersion );

	DCE::Socket *pSocket=NULL;

	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		
		/**
		 * Since we don't need to connect to the server for anything here, but we don't each table to make it's own connection, 
		 * Pass in the connection string and the NULL pointer to the socket so a connection will be made the
		 * first time and preserved for the other tables
		 */
		 
		if( pTable->Repository_get( )==this && !pTable->DetermineDeletions( ra_Processor, "localhost:3485", &pSocket ) )
			return false;
	}
	return true; /**< We succeeded */
}

#pragma warning( "as you add a table to a repository, automatically add all related tables which depend on it, or it depends on them--no, what about constants that are shared???--put warnings next to them" )

bool Repository::CheckIn( )
{
int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion );

	R_CommitChanges r_CommitChanges( m_sName );
	ra_Processor.AddRequest( &r_CommitChanges );
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( "localhost:3485", &pSocket ) );

	if( r_CommitChanges.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_CommitChanges ); /**< It's going to fall out of scope, so we don't want the processor to retry */
		
		cerr << "Unable to commit repository to server" << endl;
		delete pSocket;
		return false;
	}

	/** First add all records */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_New ) )
			return false;
	}
	
	/** Now scan for updates */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_Modify ) )
			return false;
	}
	
	/** Finally handle deletions */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_Delete ) )
			return false;
	}
	return true;
}

bool Repository::Update( )
{
	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion );

	R_UpdateRepository r_UpdateRepository( m_sName );
	ra_Processor.AddRequest( &r_UpdateRepository );
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( "localhost:3485", &pSocket ) );

	if( r_UpdateRepository.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_UpdateRepository ); /**< It's going to fall out of scope, so we don't want the processor to retry */
		cerr << "Unable to update repository" << endl;
		delete pSocket;
		return false;
	}

	/** Update Tables */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->Repository_get( )==this && !pTable->Update( ra_Processor, pSocket ) )
			return false;
	}
	return true;
}

void Repository::AddTablesToMap( )
{
	/** Be sure all tables in this repository are in the map we got */
	
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		g_GlobalConfig.m_mapTable[ ( *it ).first ] = ( *it ).second;
}

int Repository::CreateBatch( )
{
	if( !m_pTable_BatchHeader || !m_pTable_BatchDetail )
	{
		cerr << "Cannot create batch for repository " << m_sName << endl;
		throw "Invalid repository";
	}
	else
	{
		std::ostringstream sSQL;
		sSQL << "INSERT INTO " << m_pTable_BatchHeader->Name_get( ) << "( Value ) VALUES( 'test' )";
		int BatchID = m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) );
		if( !BatchID )
			cerr << "Failed to create batch: " << sSQL.str( ) << endl;
		return BatchID;
	}
	return 0;
}

int Repository::psc_id_last_sync_get( Table *pTable )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		return -1;

	std::ostringstream sSQL;
	sSQL << "SELECT last_psc_id FROM " << m_pTable_Tables->Name_get( ) << " WHERE Tablename='" << pTable->Name_get( ) << "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) )==NULL || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		cerr << "Cannot get last_psc_id for " << pTable->Name_get( ) << endl;
		throw "Internal error";
	}
	return row[0] ? atoi( row[0] ) : 0;
}

void Repository::psc_id_last_sync_set( Table *pTable, int psc_id )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		throw "Internal error Repository::psc_id_set";

	std::ostringstream sSQL;
	sSQL << "UPDATE " << m_pTable_Tables->Name_get( ) << " SET last_psc_id='" << psc_id << "' WHERE Tablename='" << pTable->Name_get( ) << "'";
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
	{
		cerr << "SQL failed: " << sSQL.str( );
		throw "Internal error Repository::psc_id_set #2";
	}
}

int Repository::psc_batch_last_sync_get( Table *pTable )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		return -1;

	std::ostringstream sSQL;
	sSQL << "SELECT last_psc_batch FROM " << m_pTable_Tables->Name_get( ) << " WHERE Tablename='" << pTable->Name_get( ) << "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) )==NULL || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		cerr << "Cannot get last_psc_batch for " << pTable->Name_get( ) << endl;
		throw "Internal error";
	}
	return row[0] ? atoi( row[0] ) : 0;
}

void Repository::psc_batch_last_sync_set( Table *pTable, int psc_batch )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		throw "Internal error Repository::psc_batch_set";

	std::ostringstream sSQL;
	sSQL << "UPDATE " << m_pTable_Tables->Name_get( ) << " SET last_psc_batch='" << psc_batch << "' WHERE Tablename='" << pTable->Name_get( ) << "'";
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
	{
		cerr << "SQL failed: " << sSQL.str( );
		throw "Internal error Repository::psc_batch_set #2";
	}
}

void Repository::Dump( )
{
	SerializeableStrings str;
	str.m_vectString.push_back( StringUtils::itos( ( int ) m_mapTable.size( ) ) );
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( !pTable->Dump( str ) )
		{
			cerr << "Failed to output table data!" << endl;
			return; /**< Just return without doing anything */
		}
	}

	if( (m_pTable_Setting && !m_pTable_Setting->Dump( str )) ||
		(m_pTable_BatchHeader && !m_pTable_BatchHeader->Dump( str )) ||
		(m_pTable_BatchDetail && !m_pTable_BatchDetail->Dump( str )) ||
		(m_pTable_Tables && !m_pTable_Tables->Dump( str )) ||
		(m_pTable_Schema && !m_pTable_Schema->Dump( str )) )
	{
		cerr << "Failed to output table data!" << endl;
		return; /**< Just return without doing anything */
	}

	str.SerializeWrite( );
	FILE *file = fopen( ( m_sName + ".sqlcvs" ).c_str( ), "wb" );
	if( !file )
		throw "Unable to write";

	size_t Size = str.CurrentSize( );
	size_t BytesOut = fwrite( str.m_pcDataBlock, 1, Size, file );
	str.FreeSerializeMemory( );
	fclose( file );
	if( BytesOut!=Size )
		throw "Out of disk space";
	return; /**< Just return without doing anything */
}

void Repository::ListTables( )
{
	cout << m_sName << endl;
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		cout << "\t" << pTable->Name_get() << endl;
	}
}

