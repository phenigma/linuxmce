/*
 Repository
 
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
 * @file Repository.cpp
 * @brief source file for the Repository class
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

#include <iomanip>
#include <sstream>

using namespace sqlCVS;

void Repository::AddDefinitionTable( class Table *pTable )
{
	if( pTable->GetTrailingString( )=="repset" )
		m_pTable_Setting = pTable;
	else if( pTable->GetTrailingString( )=="bathdr" )
		m_pTable_BatchHeader = pTable;
	else if( pTable->GetTrailingString( )=="batuser" )
		m_pTable_BatchUser = pTable;
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
	if( m_pTable_Setting==NULL || m_pTable_BatchHeader==NULL || m_pTable_BatchUser==NULL || m_pTable_BatchDetail==NULL || m_pTable_Tables==NULL )
	{
		cerr << "Could not find all the system tables for Repository: " << m_sName << endl
			<< "I need _repset, _bathdr, _batuser, _batdet, _tables" << endl
			<< "This repository has been corrupted, and will need to be rebuilt in order to continue." << endl;
		if( AskYNQuestion("Rebuild repository: " + m_sName,false) )
			ResetSystemTables();
		else
			throw "Repository corrupt";
	}

	string Tablename = "psc_" + m_sName + "_tables";

	ostringstream sql;
	sql << "SELECT Tablename,filter,frozen FROM `" << Tablename << "`";
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
			pTable->m_bFrozen = row[2] && row[2][0]=='1';
		}
	}
}

void Repository::Setup( )
{
	if( !m_pTable_Setting )
		m_pTable_Setting = CreateSettingTable( );
	if( !m_pTable_BatchHeader )
		m_pTable_BatchHeader = CreateBatchHeaderTable( );
	if( !m_pTable_BatchUser )
		m_pTable_BatchUser = CreateBatchUserTable( );
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
		<< "`date` datetime, " << endl
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

class Table *Repository::CreateBatchUserTable( )
{
	string Tablename = "psc_" + m_sName + "_batuser";
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
		<< "`FK_psc_" + m_sName + "_bathdr` int( 11 ) NOT NULL, " << endl
		<< "`psc_user` int( 11 ) NOT NULL, " << endl
		<< "`is_sup` tinyint( 1 ) NOT NULL, " << endl
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
		<< "`FK_psc_" + m_sName + "_bathdr` int( 11 ) NOT NULL, " << endl
		<< "`Tablename` varchar( 60 ) NOT NULL default '', " << endl
		<< "`New` int( 11 ) NOT NULL, " << endl
		<< "`Deleted` int( 11 ) NOT NULL, " << endl
		<< "`Modified` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_orig` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_auth` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_unauth` int( 11 ) NOT NULL, " << endl
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

	Tablename = "psc_" + m_sName + "_batuser";
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
	if( g_GlobalConfig.m_mapUsersPasswords.size()==0 )
		throw "No users"; // Shouldn't happen

	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion );

	R_CommitChanges r_CommitChanges( m_sName, g_GlobalConfig.m_sDefaultUser );
	for(map<string,string>::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
	{
		// We don't care about user 0, or users who didn't log in
		if( (*it).first=="0" || (*it).second=="" )
			continue;
		r_CommitChanges.m_mapUsersPasswords[(*it).first]=(*it).second;
	}

	ra_Processor.AddRequest( &r_CommitChanges );
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( "localhost:3485", &pSocket ) );

	if( r_CommitChanges.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_CommitChanges ); /**< It's going to fall out of scope, so we don't want the processor to retry */
		
		cerr << "Unable to commit repository to server" << endl;
		delete pSocket;
		if( r_CommitChanges.m_cProcessOutcome==LOGIN_FAILED )
		{
			cout << "The username or password was incorrect." << endl;
			if( AskYNQuestion("Do you want to re-enter the passwords and try again?",false) )
			{
				for(map<string,string>::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
					g_GlobalConfig.m_mapUsersPasswords[(*it).first]="";
				return CheckIn();
			}
		}
		return false;
	}

	/** First add all records */
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_New ) )
				return false;
		}
	}
	
	/** Now scan for updates */
	
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_Modify ) )
				return false;
		}
	}
	
	/** Finally handle deletions */
	
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( ra_Processor, pSocket, toc_Delete ) )
				return false;
		}
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

int Repository::CreateBatch( map<int,bool> *mapValidatedUsers )
{
	if( !m_pTable_BatchHeader || !m_pTable_BatchUser || !m_pTable_BatchDetail )
	{
		cerr << "Cannot create batch for repository " << m_sName << endl;
		throw "Invalid repository";
	}
	else
	{
		std::ostringstream sSQL;
		sSQL << "INSERT INTO " << m_pTable_BatchHeader->Name_get( ) << "( date ) VALUES( NOW() )";
		int BatchID = m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) );
		if( !BatchID )
			cerr << "Failed to create batch: " << sSQL.str( ) << endl;
		else
		{
			for(map<int,bool>::iterator it=mapValidatedUsers->begin();it!=mapValidatedUsers->end();++it)
			{
				std::ostringstream sSQL;
				sSQL << "INSERT INTO " << m_pTable_BatchUser->Name_get( ) << "( FK_" 
					<< m_pTable_BatchHeader->Name_get( ) << ",psc_user,is_sup ) VALUES( " << BatchID << "," << (*it).first << "," << ((*it).second ? "1" : "0") << " )";
				int BatchID = m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) );

			}
		}
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
	if( (m_pTable_Setting && !m_pTable_Setting->Dump( str )) ||
		(m_pTable_BatchHeader && !m_pTable_BatchHeader->Dump( str )) ||
		(m_pTable_BatchUser && !m_pTable_BatchUser->Dump( str )) ||
		(m_pTable_BatchDetail && !m_pTable_BatchDetail->Dump( str )) ||
		(m_pTable_Tables && !m_pTable_Tables->Dump( str )) ||
		(m_pTable_Schema && !m_pTable_Schema->Dump( str )) )
	{
		cerr << "Failed to output table data!" << endl;
		return; /**< Just return without doing anything */
	}

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

void Repository::ImportTable(string sTableName,SerializeableStrings &str,size_t &pos,Table *pTable)
{
	int NumFields = atoi( str.m_vectString[pos++].c_str( ) );

	if( !pTable )
	{
		if( m_pDatabase->threaded_mysql_query( "DROP TABLE IF EXISTS " + sTableName )!=0 )
		{
			cerr << "Could not drop table " << sTableName << endl;
			throw "Database error";
		}
	}

	map<string,int> mapFields;
	list<string> listFields;
	int iField_psc_id = -1;

	// We will go through the create table process even if
	// bCleanImport is false and we're not going to use it because we need
	// to pull of the correct strings, and we need the mapFields member
	// populated so we can be sure the schema is correct
	std::ostringstream sSQL;
	sSQL << "CREATE TABLE `" << sTableName << "` ( ";
	string sPrimaryKey;
	bool bContainsAutoIncrement=false;
	for( int j=0;j<NumFields;++j )
	{
		string sField = str.m_vectString[pos++];
		string sType = str.m_vectString[pos++];
		string sNULL = str.m_vectString[pos++];
		string sIndex = str.m_vectString[pos++];
		string sDefault = str.m_vectString[pos++];
		string sExtra = str.m_vectString[pos++];
		if( sExtra=="auto_increment" )
			bContainsAutoIncrement=true;

		if( sField=="psc_id" )
			iField_psc_id = j;
		mapFields[sField]=j;
		listFields.push_back(sField);  // We need both a map and a list

		if( j!=0 )
			sSQL << ", ";
		if( sIndex=="PRI" )
			sPrimaryKey += ( sPrimaryKey.length( ) ? "," : "" ) + string( "`" ) + sField + "`";
		sSQL << "`" << sField << "` " << sType
			<< ( sNULL!="YES" ? " NOT NULL " : "" );
		if( sDefault.length( ) )
			sSQL << " default " << ( sDefault==NULL_TOKEN ? "NULL" : "'" + sDefault + "'" );
		sSQL << " " << sExtra;
	}
	if( sPrimaryKey.length( ) )
		sSQL << ", PRIMARY KEY( " << sPrimaryKey << " )";

	sSQL << " ) TYPE=InnoDB";
	if( bContainsAutoIncrement )
		sSQL << " AUTO_INCREMENT=1000000000";

#pragma warning( "This doesn't really handle types correctly, and may not handle indexes right either" );

	if( !pTable )
	{
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "SQL Failed: " << sSQL.str( ) << endl;
			throw "Database error";
		}
	}
	else
	{
		// Confirm the schema is correct
		for( map<string,int>::iterator it=mapFields.begin();it!=mapFields.end();++it )
		{
			string sField = (*it).first;
			if( !pTable->m_mapField_Find(sField) )
			{
				cerr << "Cannot import.  Field: " << sField << " doesn't  exist in current table." << sTableName << endl;
				throw "Schema error";
			}
		}
		// Confirm the schema is correct
		for( MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it )
		{
			Field *pField = (*it).second;
			if( mapFields.find(pField->Name_get())==mapFields.end() )
			{
				cerr << "Cannot import.  Field: " << pField->Name_get() << " doesn't  exist in import." << sTableName << endl;
				throw "Schema error";
			}
		}
	}

	int NumRows = atoi( str.m_vectString[pos++].c_str( ) );
	cout << "Importing table: " << sTableName << " " << NumRows << " rows" << endl;
	for( int j=0;j<NumRows;++j )
	{
		bool bUpdate=false; // Make this an update rather than an insert
		int i_psc_id=0; // If an update, this will be the psc_id
		if( iField_psc_id!=-1 )
		{
			// See if a record with this psc_id already exists
			i_psc_id = atoi(str.m_vectString[pos+iField_psc_id].c_str());
			sSQL.str( "" );
			sSQL << "SELECT psc_id,psc_mod FROM " << sTableName << " WHERE psc_id=" << i_psc_id;
			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) && ( row = mysql_fetch_row( result_set.r ) ) )
			{
				bUpdate=true;
				if( row[1] && atoi(row[1]) )
				{
					cout << endl << "***Warning*** While importing into table: " << sTableName << " pscid: " << i_psc_id << endl;
					cout << "You modified the row that is being re-imported." << endl;
					char Response = AskMCQuestion("Overwrite your changes (yes,no,abort)?","yna");
					if( Response=='a' )
						exit (1);
					else if( Response=='n' )
					{
						pos += listFields.size();
						continue;
					}
				}
			}
		}

		sSQL.str( "" );
		if( bUpdate )
			sSQL << "UPDATE " << sTableName << " SET ";
		else
			sSQL << "INSERT INTO " << sTableName << " VALUES( ";
		bool bFirst=true;
		for(list<string>::iterator it=listFields.begin();it!=listFields.end();++it)
		{
			if( bFirst )
				bFirst = false;
			else
				sSQL << ",";

			if( bUpdate )
				sSQL << " `" << *it << "`=";

			string Value = str.m_vectString[pos++];
			if( Value==NULL_TOKEN )
				sSQL << "NULL";
			else
				sSQL << "'" << StringUtils::SQLEscape( Value ) << "'";
		}
		if( bUpdate )
			sSQL << " WHERE psc_id=" << i_psc_id;
		else
			sSQL << " )";
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "SQL Failed: " << sSQL.str( ) << endl;
			throw "Database error";
		}
	}
}

bool Repository::UpdateSchema(int PriorSchema)
{
	std::ostringstream sSQL;
	sSQL << "SELECT PK_psc_" << m_sName << "_schema,Value FROM psc_" << m_sName << "_schema WHERE PK_psc_" << m_sName << "_schema>" << PriorSchema << " ORDER BY PK_psc_" << m_sName << "_schema";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
	{
		if( result_set.r->row_count==0 )
			return false; // nothing to update
		 while( ( row = mysql_fetch_row( result_set.r ) ) ) 
		 {
			 vector<string> vectCommands;
			 string Input=row[1];
			 string Tokens="\r\n";
			 StringUtils::Tokenize(Input,Tokens,vectCommands);
			 for(size_t s=0;s<vectCommands.size();++s)
			 {
				 if( vectCommands[s].length()==0 )
					 continue;
				if( m_pDatabase->threaded_mysql_query( vectCommands[s] )!=0 )
				{
					cout << "SQL failed: " << vectCommands[s] << endl;
					cerr << "The database is now corrupted, and the schema is out of sync" << endl
						<< "because the schema update failed!" << endl;
					throw "Schema update failed";
				}
			}
			SetSetting("schema",row[0]);
		}
	 	return true; // We updated the schema
	}
	throw "Database error"; // Can't get schema info??
}

bool Repository::ShowChanges()
{
	while(true)
	{
		cout << "    Table                                      New   Mod   Del" << endl;

		vector<Table *> vectTable; // So we can allow the user to pick a table by number
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			vectTable.push_back(pTable);
			cout << setw( 3 ) << vectTable.size() << " ";
			if( pTable->Name_get().length() > 40 )
				cout << pTable->Name_get().substr(0,40);
			else
			{
				cout << pTable->Name_get() 
					<< StringUtils::RepeatChar( ' ', 40  - ( int )pTable->Name_get( ).length( ) );
			}

			int iNew=0,iMod=0,iDel=0;
			for(map<int, ListChangedRow *>::iterator itCR=pTable->m_mapUsers2ChangedRowList.begin();itCR!=pTable->m_mapUsers2ChangedRowList.end();++itCR)
			{
				ListChangedRow *pListChangedRow = (*itCR).second;
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
			}
			cout << setw( 6 ) << iNew << setw( 6 ) << iMod << setw( 6 ) << iDel << endl;
		}

		cout << "What table do you want more detail on?  Enter 'b' to go back, 'q' to quit" << endl;
		string sTable;
		cin >> sTable;
		if( sTable=="b" || sTable=="B" )
			return true;
		if( sTable=="q" || sTable=="Q" )
			return false;

		int iTable = atoi(sTable.c_str());
		if( iTable < 1 || iTable > vectTable.size() )
			cout << "That is not a valid selection" << endl;
		else
		{
			Table *pTable = vectTable[ iTable-1 ];
			if( !pTable->ShowChanges() )
				return false;
		}
	}
}

void Repository::ResetSystemTables()
{
	// First save the settings in this map
	map<string,string> mapSettings;
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "SELECT Setting,Value FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set.r ) )
		{
			mapSettings[row[0]]=row[1];
		}
	}

	list<string> listTables;
	sql.str("");
	Tablename = "psc_" + m_sName + "_tables";
	sql	<< "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set2;
	if( ( result_set2.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set2.r ) )
			listTables.push_back(row[0]);
	}

	CreateSettingTable( );
	CreateBatchHeaderTable( );
	CreateBatchUserTable( );
	CreateBatchDetailTable( );
	CreateTablesTable( );
	CreateSchemaTable( );

	for(map<string,string>::iterator it=mapSettings.begin();it!=mapSettings.end();++it)
		SetSetting((*it).first,(*it).second);  

	GetSetting("schema","1");  // Be sure we at least have a default schema of 1

	// We reset all the psc_rep_tables, so put back any tables in there
	for(list<string>::iterator it=listTables.begin();it!=listTables.end();++it)
	{
		Table *pTable = m_mapTable_Find(*it);
		string Tablename = "psc_" + m_sName + "_tables"; /**< Our _tables table */
		ostringstream sql;
		sql << "INSERT INTO `" << Tablename << "` ( Tablename,filter,frozen ) VALUES( '" << *it << "','" << (pTable ? pTable->m_sFilter : "") 
			<< "'," << (pTable && pTable->m_bFrozen ? "1" : "0") << ")";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )!=0 )
		{
			cerr << "SQL failed: " << sql.str( ) << endl;
			throw "Database Error";
		}
	}
}