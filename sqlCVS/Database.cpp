/**
 *
 * @file Database.cpp
 * @brief source file for the Database, SafetyDBLock, SafetyTransaction classes
 *
 */
 
 /**
  *
  * Copyright information goes here
  *
  */
  
#include "PlutoUtils/CommonIncludes.h"	
#include "Database.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "CommonFunctions.h"
#include <iomanip>
#include <sstream>

using namespace sqlCVS;

Database::Database( string db_host, string db_user, string db_pass, string db_name, int db_port )
	: MySqlHelper( db_host, db_user, db_pass, db_name, db_port )
{
	if( !m_bConnected )
	{
		// If we didn't connect, maybe the connection is valid but the database doesn't exist.  If so, try creating it if the command is import
		if( g_GlobalConfig.m_sCommand=="import" )
		{
			// Try connecting without a database
			m_sMySQLDBName = "";
			MySQLConnect();
			if( !m_bConnected )
				return;  // Nope, the connection is no good

			// Try creating the dabase
			threaded_mysql_query("CREATE DATABASE " + db_name);
			m_sMySQLDBName = db_name; // Set it back and try again
			MySQLConnect();
			if( !m_bConnected )
				return;
		}
		else
			return;
	}

	m_bInvalid=false;
	try
	{
		/** Establishing database connection	*/
		m_bInteractiveMode=false;

		/** And build our list of tables and repositories	*/
		PlutoSqlResult res;
		MYSQL_ROW row=NULL;
		if( res.r=mysql_list_tables( &m_MySQL, NULL ) )
		{
			while ( ( row = mysql_fetch_row( res.r ) ) )
			{
				Table *pTable = new Table( this, row[0] );
				m_mapTable[row[0]] = pTable;
				/** Does this table help define a repository ( ie table is psc_repositoryname_[x] */
				string sRepository = pTable->DefinesRepository( );
				if( sRepository.size( ) )
				{
					Repository *pRepository = m_mapRepository[sRepository];
					if( !pRepository )
					{
						pRepository = new Repository( this, sRepository );
						m_mapRepository[sRepository] = pRepository;
					}
					pRepository->AddDefinitionTable( pTable );
				}
			}
		}

		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
			( *it ).second->MatchUpTables( );
		for( MapTable::iterator itT=m_mapTable.begin( );itT!=m_mapTable.end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			pTable->MatchUpHistory( );
			pTable->GetDependencies( );
			if( pTable->Repository_get( ) )
			{
				pTable->psc_id_last_sync_set( pTable->Repository_get( )->psc_id_last_sync_get( pTable ) );
				pTable->psc_batch_last_sync_set( pTable->Repository_get( )->psc_batch_last_sync_get( pTable ) );
			}
		}
	}
	catch( const char *pException )
	{
		cerr << "Caught exception: " << pException << endl << "***DATABASE SCHEMA INVALID. IMPORT IS THE ONLY VALID OPTION***" << endl;
		m_bInvalid=true;
	}
}

void Database::GetRepositoriesTables( )
{
	string s, sUnknownRepository;
	string::size_type pos=0;

	while( ( s=StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ", ", pos ) ).length( ) )
	{
		Repository *pRepository = m_mapRepository_Find( s );
		if( !pRepository )
			sUnknownRepository += ( sUnknownRepository.length( ) ? ", " : "" ) + s;
		else
			g_GlobalConfig.m_mapRepository[s]=pRepository;
	}
	g_GlobalConfig.m_sRepository=sUnknownRepository;

	pos=0;
	while( ( s=StringUtils::Tokenize( g_GlobalConfig.m_sTable, ", ", pos ) ).length( ) )
	{
		Table *pTable = m_mapTable[s];
		if( !pTable )
		{
			cerr << "Unknown Table: " << s << endl;
			throw "Unknown Table";
		}
		g_GlobalConfig.m_mapTable[s]=pTable;
	}
}

void Database::CreateRepository( )
{
	vector<string> vect_sRepository;
	StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ", ", vect_sRepository );

	if( vect_sRepository.size( )>1 && g_GlobalConfig.m_mapTable.size( )>0 )
	{
		cerr << "You cannot specify more than 1 repository to create at a time if you explicitly list tables" << endl;
		throw "Bad arguments";
	}

	if( vect_sRepository.size( )==0 )
	{
		while( g_GlobalConfig.m_sRepository.length( )==0 )
		{
			cout << "What do you want to call this repository? ( enter q to quit ): ";
			cin >> g_GlobalConfig.m_sRepository;
			if( g_GlobalConfig.m_sRepository=="q" )
				return; /**< Do nothing */
			vect_sRepository.push_back( g_GlobalConfig.m_sRepository );
		}
	}

	/** We must have at least 1 repository specified, maybe more if the user passed them in the command line		*/
	for( size_t s=0;s<vect_sRepository.size( );++s )
	{
		Repository *pRepository = m_mapRepository_Find( vect_sRepository[s] );
		if( pRepository )
		{

			if( !AskYNQuestion( "That repository exists. Edit the exiting one?", false ) )
				return; /**< Nope, just abort */
		}
		else
			pRepository = new Repository( this, vect_sRepository[s] );

		int Result;
		
		/** If the user didn't specify tables on the command line, prompt for them */
		if( g_GlobalConfig.m_mapTable.size( )==0 && ( Result=PromptForTablesInRepository( pRepository, g_GlobalConfig.m_mapTable ) )<1 ) 
		{
			if( Result<0 )
				return; /**< Just return without doing anything */
			pRepository->Remove( ); /**< Remove this repository if there are no tables */
			MapRepository::iterator it = m_mapRepository.find( pRepository->Name_get( ) );
			
			/** It could be that, even though the user chose create, he typed in the name of an existing repository  */
			if( it!=m_mapRepository.end( ) ) 
				m_mapRepository.erase( it );
			delete pRepository;

			return; /**< PromptForTablesInRepository returns the number of tables the user selected, or -1 to exit */
		}

		pRepository->Setup( ); /**< Sets up the repository */
		m_mapRepository[ pRepository->Name_get( ) ] = pRepository;
	}
}



int Database::PromptForTablesInRepository( Repository *pRepository, MapTable &mapTable )
{
	m_bInteractiveMode=true;

	int iNumColumns = SCREEN_WIDTH / ( ( int )MaxTablenameLength( )+8 );
	int iColumnWidth = SCREEN_WIDTH/iNumColumns;

	/**
	 * Since the user may have called this with an existing repository, do a quick run 
	 * first to be sure we add any tables that are already in the repository to the map
	 * The user can always remove them in the main loop
	 */
	 
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( StringUtils::StartsWith( pTable->Name_get( ), "psc_" ) || StringUtils::EndsWith( pTable->Name_get( ), "_pschist" ) )
			continue; /**< It's a system table */

		/**
		 * If the table is not in any repository, that's okay -- the user will be adding it. But if it already is in a repository, 
		 * it must point to this one if it's already in the list
		 */
		 
		if( pTable->Repository_get( ) )
		{
			/** 
			 * If it's in the list, and points to us, that's ok. If it's not in the list and points to another, that's also ok. 
			 * But if it's in the list and points to another, that's an error
			 */
			 
			if( mapTable.find( pTable->Name_get( ) )!=mapTable.end( ) && pTable->Repository_get( )!=pRepository )
			{
				/**
				 * We would never allow this to happen, but maybe the user specified everything in the command line and 
				 * is trying to put a table into a repository that is already in another repository
				 */
				 
				cerr << "Table: " << pTable->Name_get( ) << " is already in the repository " << pTable->Repository_get( )->Name_get( ) << endl;
				cerr << "It cannot be added to repository: " << pRepository->Name_get( ) << endl;
				throw "Bad arguments";
			}
			
			/**
			 * and if it's not in the list, but points to us, we need to add it, since, by default, it will be selected. 
			 * The user can remove it
			 */
			 
			else if( mapTable.find( pTable->Name_get( ) )==mapTable.end( ) && pTable->Repository_get( )==pRepository )
				mapTable[ pTable->Name_get( ) ] = pTable;
		}
	}

	string sError="";
	while( true )
	{
		NewSection( );
		int iColumnNum=0;
		vector<Table *> vectTable; /**< A numeric reference to the tables the user can change */
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			if( StringUtils::StartsWith( pTable->Name_get( ), "psc_" ) || StringUtils::EndsWith( pTable->Name_get( ), "_pschist" ) )
				continue; /**< It's a system table */

			bool bTableAlreadyInList = mapTable.find( pTable->Name_get( ) )!=mapTable.end( );

			/** 001 *H TableName   */
			if( pTable->Repository_get( ) && pTable->Repository_get( )!=pRepository )
				cout << "XXX"; /**< It's in another repository, we can't touch it */
			else
			{
				vectTable.push_back( pTable );
				cout << setw( 3 ) << ( int ) vectTable.size( );
			}

			cout << ( bTableAlreadyInList ? " *" : " " ) << ( pTable->HasFullHistory_get( ) ? "H " : " " );
			if( ( int ) pTable->Name_get( ).length( )>iColumnWidth )
				cout << pTable->Name_get( ).substr( 0, iColumnWidth );
			else
				cout << pTable->Name_get( );

			if( ++iColumnNum >= iNumColumns )
			{
				iColumnNum=0;
				cout << endl;
			}
			else 
				cout << StringUtils::RepeatChar( ' ', iColumnWidth - ( int )pTable->Name_get( ).length( ) );
		}

		cout << "Tables marked with * are to be included in the repository: " << pRepository->Name_get( ) << endl
			<< "Enter the table numbers, separated by commas, to toggle the * flag." << endl
			<< "XXX means the table is already in another repository, and cannot be added" << endl
			<< "Tables with an H in front will have history tracking." << endl
			<< "Enter H followed by the table numbers to toggle the history flag." << endl
			<< "Enter W[NumColumns], [ColumnWidth] to change layout" << endl
			<< "Enter D when you have finished, Q to quit." << endl;
		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return ( int ) mapTable.size( );
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort  */
		if( sAction[0]=='W' || sAction[0]=='w' )
		{
			string::size_type pos=1; /**< Skip the w */
			string sNumColumns = StringUtils::Tokenize( sAction, ", ", pos );
			string sWidth = StringUtils::Tokenize( sAction, ", ", pos );
			if( atoi( sNumColumns.c_str( ) ) )
				iNumColumns = atoi( sNumColumns.c_str( ) );
			if( atoi( sWidth.c_str( ) ) )
				iColumnWidth = atoi( sWidth.c_str( ) );
		}

		bool bHistory = sAction[0]=='h' || sAction[0]=='H';
		string::size_type pos = bHistory ? 1 : 0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ", ", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectTable.size( )-1 )
			{
				sError ="Invalid table number";
				break;  /**< stops the while */
			}
			Table *pTable = vectTable[iNumber];
			if( bHistory )
				pTable->HasFullHistory_set( !pTable->HasFullHistory_get( ) );
			else if( mapTable.find( pTable->Name_get( ) )!=mapTable.end( ) )
				mapTable.erase( pTable->Name_get( ) );
			else
				mapTable[pTable->Name_get( )] = pTable;
		}
	}
	return ( int ) mapTable.size( );
}

size_t Database::MaxTablenameLength( )
{
	size_t MaxLength=0;
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
		if( ( *it ).first.length( )>MaxLength )
			MaxLength = ( *it ).first.length( );
	}

	return MaxLength;
}

class Table *Database::GetTableFromForeignKey( Field *pField )
{
	string sTable = pField->Name_get( ).substr( 3 );

	while( sTable.length( )>0 )
	{
		/** See if table is a valid Table Name */
		Table *pTable = m_mapTable_Find( sTable );
		if( pTable )
			return pTable;

		string Tail = pField->Table_get( )->GetTrailingString( sTable );	/**< Use the helper function in table */
		sTable = sTable.substr( 0, sTable.length( ) - Tail.length( ) - 1 );	/**< Strip off the last part */
	}

	cerr << "I cannot find the table referred to in the field: " << pField->Table_get( )->Name_get( ) << ":" << pField->Name_get( ) << endl;
	throw "Bad Schema";
}

class Table *Database::GetTableFromForeignKeyString( Table *pTable, string sField )
{
	string sTable = sField.substr( 3 );

	while( sTable.length( )>0 )
	{
		/** See if table is a valid Table Name  */
		Table *pTable = m_mapTable_Find( sTable );
		if( pTable )
			return pTable;

		string Tail = pTable->GetTrailingString( sTable );			/**< Use the helper function in table */
		sTable = sTable.substr( 0, sTable.length( ) - Tail.length( ) - 1 );	/**< Strip off the last part */
	}

	cerr << "I cannot find the table referred to in the table: " << pTable->Name_get( ) << " by " << sField << endl;
	throw "Bad Schema";
}

void Database::CheckIn( )
{
	GetTablesToCheckIn( );
	
	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		cerr << "No tables found to add. Aborting." << endl;
		return;
	}

	SafetyDBLock sl( this );	/**< The DB will unlock when this falls out of scope   */

	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		pTable->GetChanges( );
	}

	bool bDependenciesMet=true;
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		if( !( *it ).second->ConfirmDependencies( ) )
			bDependenciesMet=false;
	}
	
	if( !bDependenciesMet )
	{
		cerr << "Aborting checkin due to unmet dependencies" << endl;
		throw "Unmet dependencies";
	}

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
	{
		Repository *pRepository = ( *it ).second;
		if( !pRepository->DetermineDeletions( ) )
			return; /**< This will throw an exception with the transaction */
	}

#pragma warning( "Allow users to be on the command line so this can be non-interactive" )

	/** Now mapTable has all the tables we need to check in. Confirm the users if none were passed in on the command line   */
	if( g_GlobalConfig.m_mapUsersPasswords.size( )==0 && ConfirmUsersToCheckIn( )<1 )
		return;

	/** If any of the records to be checked in refer to a record with an auto-increement field that is new and is not being checked in,
	 * this will fail
	 */
	if( !ConfirmRecordsToCheckIn( ) )
		return;

	 /** An exception will be thrown and a roll back called if this falls out of scope and hasn't been committed or rolled back */
	SafetyTransaction st( this );

	try
	{
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( !pRepository->CheckIn( ) )
			{
				cerr << "Checkin failed" << endl;
				st.Rollback( );
				return;
			}
		}
	}
	catch( const char *pException )
	{
		cerr << "Checkin threw exception: " << pException << endl;
		st.Rollback( );
		return;
	}
#pragma warning( "Repeat the process with the same tables any rows that are now again set to modified since modifying other rows may have modified rows that weren't already in the list" )
	st.Commit( ); /** @todo - have a confirmation */
}

void Database::Update( )
{
	GetTablesToCheckIn( );
	
	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		cerr << "No tables found to add. Aborting." << endl;
		return;
	}

	SafetyDBLock sl( this ); /**< The DB will unlock when this falls out of scope  */


#pragma warning( "Allow users to be on the command line so this can be non-interactive" )

	/** Now mapTable has all the tables we need to check in. Confirm the users if none were passed in on the command line */
	if( g_GlobalConfig.m_mapUsersPasswords.size( )==0 && ConfirmUsersToCheckIn( )<1 )
		return;

	/** An exception will be thrown and a roll back called if this falls out of scope and hasn't been committed or rolled back */
	SafetyTransaction st( this ); 

	try
	{
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( !pRepository->Update( ) )
			{
				cerr << "ERROR: Update returned false" << endl;
				st.Rollback( );
				return; 
			}
		}
	}
	catch( const char *pException )
	{
		cerr << "Update threw exception: " << pException << endl;
		st.Rollback( );
		return;
	}
#pragma warning( "Repeat the process with the same tables any rows that are now again set to modified since modifying other rows may have modified rows that weren't already in the list" )
	st.Commit( ); /** @todo - have a confirmation */
}

void Database::GetTablesToCheckIn( )
{
	/** If the user passed specific tables on the command line, that's okay. Otherwise we need to add the tables from the repository  */
	
	int flag = 0;
	MapTable mapTableToCheckIn;
	MapTable::iterator itToChkIn = g_GlobalConfig.m_mapTable.begin( );

	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		if( g_GlobalConfig.m_mapRepository.size( )==0 )
			if( PromptForRepositories( )<1 )
				return;

		/** Since the user didn't specify tables explicitly, but rather repositories, just add all tables in the repository  */
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			pRepository->AddTablesToMap( ); /**< Be sure all the tables in the repositories are added */
		}
	}

	return;
// following is Cristelu's code -- not used I guess	
	int iMaxLenofTableName = 0;

	PlutoSqlResult result_set, result_set2;
	MYSQL_ROW row;
	string query_string;

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
	{
		Repository *pRepository = ( *it ).second;
		
		query_string = "SELECT Tablename FROM psc_" + pRepository->Name_get( ) + "_tables ";
		result_set.r = mysql_query_result( query_string );
    
		while ( row = mysql_fetch_row( result_set.r ) ){
			string sTable = row[0];	
			
			Table *pTable;
			
			if ( pTable = m_mapTable_Find( sTable ) )			
				g_GlobalConfig.m_mapTable[ sTable ] = pTable;
			
			if ( iMaxLenofTableName < ( int ) pTable->Name_get( ).length( ) )
				iMaxLenofTableName = ( int ) pTable->Name_get( ).length( );
		}
	}

	if ( g_GlobalConfig.m_mapTable.size( ) != 0 )
		while ( true )
		{
			NewSection( );
			
			vector<Table *> vectTable;
			int i;
			
			for ( i = 0; i< iMaxLenofTableName; i++ )
        cout << " " ; 
			cout << "\t\t\tNew\tMod\tDel" << endl;
		
			PlutoSqlResult result_set;
			MYSQL_ROW row;
			string query_string;
			i = 1;

			for ( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( ); it!=g_GlobalConfig.m_mapTable.end( ); ++it )
			{
				Table *pTable = ( *it ).second;
				vectTable.push_back( pTable );
				
				cout << "  " << i++ << "\t" << pTable->Name_get( ) << "\t";

				for ( int j = ( int ) pTable->Name_get( ).length( );j < iMaxLenofTableName; j++ )
					cout << " ";

				cout << "\t";
				
				/** number of new rows   */
				query_string = "SELECT COUNT( * ) FROM " + pTable->Name_get( ) + " WHERE psc_new>0";
				result_set.r = mysql_query_result( query_string );
				row = mysql_fetch_row( result_set.r );
				cout << row[0] << "\t"; 

				/** number of deleted rows */
				query_string = "SELECT COUNT( * ) FROM " + pTable->Name_get( ) + " WHERE psc_del > 0";
				result_set.r = mysql_query_result( query_string );
				row = mysql_fetch_row( result_set.r );
				cout << row[0] << "\t"; 

				/** number of modified rows */
				query_string = "SELECT COUNT( * ) FROM " + pTable->Name_get( ) + " WHERE psc_mod > 0";
				result_set.r = mysql_query_result( query_string );
				row = mysql_fetch_row( result_set.r );
				cout << row[0] << "\t" << endl;
			}
		
			cout << "To view specific changes for the tables listed above" 
				<< " enter the table numbers, " << endl << " separated by commas" << endl
				<< "Enter D when you have finished, Q to quit." << endl;
	
			string sError;

			if( sError.length( ) )
			{
				cout << "***" + sError + "***" << endl;
				sError="";
			}

			string sAction;
			cin >> sAction;
			
			if( sAction=="D" || sAction=="d" )
				break;
			if( sAction=="Q" || sAction=="q" )
				return; /**< abort */

			string::size_type pos = 0;
			string sNumber;
	
			while( ( sNumber=StringUtils::Tokenize( sAction, ", ", pos ) ).length( ) )
			{
				int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
				if( iNumber<0 || iNumber>( int ) vectTable.size( )-1 )
				{
					sError ="Invalid table number";
					break;
				}
				
				flag = 1;

				Table *pTable = vectTable[iNumber];
				
				NewSection( );

				cout << pTable->Name_get( ) << endl;
				
				/** showing the modified rows  */
				query_string = "DESCRIBE " + pTable->Name_get( );
				result_set.r = mysql_query_result( query_string );
				query_string = "";
				i = 0;
				int flagbreak = 0; 
        while ( ( row = mysql_fetch_row( result_set.r ) ) && ( flagbreak == 0 ) )
				{
					string s = "Modification_RecordInfo";
					if ( row[0] == s ){
            flagbreak = 1;
					}
					else
						if ( query_string == "" ){
							query_string = row[0];
							i++;
						}
						else{
							query_string = query_string + ", " + row[0];
							i++;
						}
				}

				query_string = "SELECT " + query_string + " FROM " + pTable->Name_get( ) + " WHERE ( ( psc_new>0 ) || ( ( psc_del>0 ) || ( psc_mod>0 ) ) )";
				result_set.r = mysql_query_result( query_string );

				while ( row = mysql_fetch_row( result_set.r ) ){					
					for ( int j = 0;j < i; j++ )
						if ( row[j] )
							cout << row[j] << "  ";
						else
							cout << "NULL" << "  ";
					cout << endl;
				}
	 
				cout << endl << "If you want to check in changes press S." << endl
					<< "If you don't want to check in changes press Q." << endl;

				string sAnsw;
				cin >> sAnsw;
				
				if ( sAnsw == "S" || sAnsw == "s" ){
					itToChkIn = g_GlobalConfig.m_mapTable.find( pTable->Name_get( ) );
					mapTableToCheckIn.insert( *itToChkIn );

					g_GlobalConfig.m_mapTable.erase( pTable->Name_get( ) );
				}

				if ( sAnsw == "Q" || sAnsw == "q" )
					g_GlobalConfig.m_mapTable.erase( pTable->Name_get( ) );
			}
		}

	if ( flag == 1 ) {
		g_GlobalConfig.m_mapTable.clear( );
		for( MapTable::iterator it=mapTableToCheckIn.begin( );it!=mapTableToCheckIn.end( );++it )
			g_GlobalConfig.m_mapTable[ ( *it ).first ] = ( *it ).second;
	}

}

int Database::PromptForRepositories( )
{
	m_bInteractiveMode=true;

	string sError="";
	while( true )
	{
		NewSection( );
		vector<Repository *> vectRepository; /**< A numeric reference to the tables the user can change */
		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			vectRepository.push_back( pRepository );
			cout << setw( 3 ) << ( int ) vectRepository.size( );
			cout << ( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) ? " *" : " " ) << pRepository->Name_get( ) << endl;
		}

		cout << "Repositories marked with * are to be included." << endl
			<< "Enter the Repository numbers, separated by commas, to toggle the * flag." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return ( int ) g_GlobalConfig.m_mapRepository.size( );
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort  */

		string::size_type pos=0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ", ", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectRepository.size( )-1 )
			{
				sError ="Invalid Repository number";
				break;
			}
			Repository *pRepository = vectRepository[iNumber];
			if( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) )
				g_GlobalConfig.m_mapRepository.erase( pRepository->Name_get( ) );
			else
				g_GlobalConfig.m_mapRepository[pRepository->Name_get( )] = pRepository;
		}
	}
}

int Database::PromptForSqlCvsFiles( )
{
	m_bInteractiveMode=true;

	string sError="";
	while( true )
	{
		NewSection( );

		vector<string> vectFiles; /**< A numeric reference to the tables the user can change */

#ifdef WIN32  /** Windows */

		intptr_t ptrFileList;
		_finddata_t finddata;

		ptrFileList = _findfirst( "*.sqlcvs", & finddata );
		while ( ptrFileList != -1 )
		{
			if ( finddata.attrib == _A_SUBDIR || finddata.name[0] == '.' )
				continue;
			
			vectFiles.push_back( finddata.name );

			if ( _findnext( ptrFileList, & finddata ) < 0 )
				break;
		}
		_findclose( ptrFileList );
#else /** Linux */
#endif
		for( size_t s=0;s<vectFiles.size( );++s )
		{
			string sRepository = FileUtils::FilenameWithoutPath( vectFiles[s], false );
			Repository *pRepository = ( g_GlobalConfig.m_mapRepository.find( sRepository )!=g_GlobalConfig.m_mapRepository.end( ) ? g_GlobalConfig.m_mapRepository[sRepository] : NULL );
			Repository *pRepository_Valid = m_mapRepository_Find( sRepository );
			bool bFound = g_GlobalConfig.m_sRepository.find( ", " + sRepository + ", " )!=string::npos;
			cout << setw( 3 ) << ( int ) s+1;
			cout << ( pRepository || bFound ? " *" : " " )
				<< ( pRepository_Valid ? "!" : " " )
				<< vectFiles[s] << endl;
		}

		cout << "Repositories marked with * are to be included. ! are existing repositories." << endl
			<< "Enter the Repository numbers, separated by commas, to toggle the * flag." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			 /** We don't know exactly how many since it could be a combination of comma delimited, plus existing repositories */
			return 1;
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort */

		string::size_type pos=0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ", ", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectFiles.size( )-1 )
			{
				sError ="Invalid Repository number";
				break;
			}
			string sRepository = FileUtils::FilenameWithoutPath( vectFiles[iNumber], false );
			Repository *pRepository = m_mapRepository_Find( sRepository );
			if( pRepository )
			{
				/** The file is an existing repository */
				if( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) )
					g_GlobalConfig.m_mapRepository.erase( pRepository->Name_get( ) );
				else
					g_GlobalConfig.m_mapRepository[pRepository->Name_get( )] = pRepository;
			}
			else
			{
				/** The file is not an existing repository */
				if( g_GlobalConfig.m_sRepository.find( ", " + sRepository + ", " )!=string::npos )
					StringUtils::Replace( g_GlobalConfig.m_sRepository, ", " + sRepository + ", ", "" );
				else
				{
					if( g_GlobalConfig.m_sRepository.length( )==0 )
						g_GlobalConfig.m_sRepository = ", ";
					g_GlobalConfig.m_sRepository += sRepository + ", ";
				}
			}
		}
	}
}

int Database::ConfirmUsersToCheckIn( )
{
	string sError="";
	while( true )
	{
		NewSection( );
		for( map<int, MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
		{
			MapTable *pMapTable = ( *it ).second;
			cout << ( g_GlobalConfig.m_mapUsersPasswords.find( ( *it ).first )!=g_GlobalConfig.m_mapUsersPasswords.end( ) ? "* " : " " )
				<< "User ID: " << ( *it ).first << " modified the following tables: " << endl
				<< "\t\t\t";
			int ColumnPosition=0;
			for( MapTable::iterator it=pMapTable->begin( );it!=pMapTable->end( );++it )
			{
				Table *pTable = ( *it ).second;
				if( ColumnPosition && ColumnPosition + pTable->Name_get( ).length( ) > SCREEN_WIDTH )
				{
					cout << endl;
					ColumnPosition=0;
				}
				else
				{
					if( ColumnPosition )
						cout << ", ";
					++ColumnPosition;
				}
				cout << pTable->Name_get( );
			}
			if( ColumnPosition )
				cout << endl;
		}

		cout << "Users marked with * are to be included." << endl
			<< "Enter the users ID's, separated by commas, to toggle the * flag." << endl
			<< "Enter + to select all users, - to deselect all users." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return ( int ) g_GlobalConfig.m_mapUsersPasswords.size( );
		else if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort */

		if ( sAction == "-" )
			g_GlobalConfig.m_mapUsersPasswords.clear( );
		else 
			if ( sAction == "+" )
				for ( map<int, MapTable*>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
					g_GlobalConfig.m_mapUsersPasswords[( *it ).first]="";

			else
			{
				string::size_type pos=0;
				string sNumber;
				while( ( sNumber=StringUtils::Tokenize( sAction, ", ", pos ) ).length( ) )
				{
					int iNumber=atoi( sNumber.c_str( ) );
					if( g_GlobalConfig.m_mapUsersTables.find( iNumber )==g_GlobalConfig.m_mapUsersTables.end( ) )
					{
						sError ="Invalid User ID";
						break;
					}
					if( g_GlobalConfig.m_mapUsersPasswords.find( iNumber )!=g_GlobalConfig.m_mapUsersPasswords.end( ) )
						g_GlobalConfig.m_mapUsersPasswords.erase( iNumber );
					else
						g_GlobalConfig.m_mapUsersPasswords[iNumber] = "";
				}
			}
	}
}

bool Database::ConfirmRecordsToCheckIn( )
{
/** If any of the records to be checked in refer to a record with an auto-increement field that is new and is not being checked in, this will fail */

	return true;
}

void Database::Dump( )
{
	if( g_GlobalConfig.m_mapRepository.size( )==0 )
		if( PromptForRepositories( )<1 )
			return; /**< abort*/

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
	{
		Repository *pRepository = ( *it ).second;
		pRepository->Dump( );
	}
}

void Database::Import( )
{
	if( g_GlobalConfig.m_mapRepository.size( )==0 && g_GlobalConfig.m_sRepository.length( )==0 )
	{
		if( PromptForSqlCvsFiles( )<0 )
			return;  /**< abort */
	}

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		Import( ( *it ).first, ( *it ).second );

	string::size_type pos=0;
	string sRepository;
	while( true )
	{
		sRepository=StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ", ", pos );
		if( sRepository.length( ) )
			Import( sRepository, NULL );
		if( pos==string::npos || pos>=g_GlobalConfig.m_sRepository.length( ) )
			break;
	}
}

class SerializeableStrings : public SerializeClass
{
public:
	vector<string> m_vectString;
	void SetupSerialization( ) {	StartSerializeList( ) + m_vectString; 	}
};

void Database::Import( string sRepository, Repository *pRepository )
{
	SerializeableStrings str;
	if( !str.SerializeRead( sRepository + ".sqlcvs" ) )
		throw "Cannot read file";

	size_t pos=0;
	int NumTables = atoi( str.m_vectString[pos++].c_str( ) );
	for( int i=0;i<NumTables;++i )
	{
		std::ostringstream sSQL;
		string sTable = str.m_vectString[pos++];
		int NumFields = atoi( str.m_vectString[pos++].c_str( ) );

		if( threaded_mysql_query( "DROP TABLE IF EXISTS " + sTable )!=0 )
		{
			cerr << "Could not drop table " << sTable << endl;
			throw "Database error";
		}

		sSQL << "CREATE TABLE `" << sTable << "` ( ";
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
				
			if( j!=0 )
				sSQL << ", ";
			if( sIndex=="PRI" )
				sPrimaryKey += ( sPrimaryKey.length( ) ? ", " : "" ) + string( "`" ) + sField + "`";
			sSQL << "`" << sField << "` " << sType
				<< ( sNULL!="YES" ? " NOT NULL " : "" );
			if( sDefault.length( ) )
				sSQL << " default " << ( sDefault=="**( NULL )**" ? "NULL" : "'" + sDefault + "'" );
			sSQL << " " << sExtra;
		}
		if( sPrimaryKey.length( ) )
			sSQL << ", PRIMARY KEY( " << sPrimaryKey << " )";
		
		sSQL << " ) TYPE=InnoDB";
		if( bContainsAutoIncrement )
			sSQL << " AUTO_INCREMENT=1000000000";
#pragma warning( "This doesn't really handle types correctly, and may not handle indexes right either" );

		if( threaded_mysql_query( sSQL.str( ) )!=0 )
		{
			cerr << "SQL Failed: " << sSQL.str( ) << endl;
			throw "Database error";
		}

		int NumRows = atoi( str.m_vectString[pos++].c_str( ) );
		for( int j=0;j<NumRows;++j )
		{
			sSQL.str( "" );
			sSQL << "INSERT INTO " << sTable << " VALUES( ";
			for( int k=0;k<NumFields;++k )
			{
				if( k )
					sSQL << ", ";

				string Value = str.m_vectString[pos++];
				if( Value=="**( NULL )**" )
					sSQL << "NULL";
				else
					sSQL << "'" << StringUtils::SQLEscape( Value ) << "'";
			}
			sSQL << " )";
			if( threaded_mysql_query( sSQL.str( ) )!=0 )
			{
				cerr << "SQL Failed: " << sSQL.str( ) << endl;
				throw "Database error";
			}
		}
	}

	str.FreeSerializeMemory( );
}

void Database::ListTables( )
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		cout << "Table: " << pTable->Name_get();
		if( pTable->Repository_get() )
			cout << "\t\tRepository: " << pTable->Repository_get()->Name_get();
		else
			cout << "\t\t**no repository**";
		cout << endl;
	}
}

void Database::ListRepositories( )
{
	for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		pRepository->ListTables();
	}
}

void Database::HasFullHistory_set_all( bool bOn )
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() )
			pTable->HasFullHistory_set( bOn );
	}
}

void Database::Reset_psc()
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() )
		{
			pTable->TrackChanges_set(false);
			pTable->TrackChanges_set(true);

			// We need to update all the psc_id's to have a sequential number.  First figure out the primary keys for this table
			if( pTable->m_listField_PrimaryKey.size()==0 )
			{
				cout << "**Error**  Table: " << pTable->Name_get() << " has no primary key.  Can't update it." << endl;
				throw "Database error";
			}
			ostringstream sql;
			sql << "SELECT ";
			bool bFirst=true;
			for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
			{
				Field *pField = *it;
				if( bFirst )
					bFirst=false;
				else
					sql << ",";

				sql << "`" << pField->Name_get( ) << "`";
			}
			
			sql << " FROM `" << pTable->Name_get() << "`";
			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			int RowCount=1;
			if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			{
				while ( row = mysql_fetch_row( result_set.r ) )
				{
					sql.str("");
					sql << "UPDATE `" << pTable->Name_get() << "` SET psc_mod=0,psc_id=" << RowCount++ << " WHERE ";
					int FieldCount=0;
					bool bFirst=true;
					for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
					{
						Field *pField = *it;
						if( bFirst )
							bFirst=false;
						else
							sql << " AND ";

						sql << "`" << pField->Name_get( ) << "`='";
						if( row[FieldCount] )
							sql << row[FieldCount];
						sql << "'";
						FieldCount++;
					}
					if( threaded_mysql_query(sql.str())!=0 )
					{
						cerr << "Could not update table " << pTable->Name_get() << endl;
						throw "Database error";
					}
				}
			}
		}
		else
			pTable->TrackChanges_set(false);
	}

	for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		// First save the settings in this map
		map<string,string> mapSettings;
		string Tablename = "psc_" + pRepository->Name_get() + "_repset";
		ostringstream sql;

		sql	<< "SELECT Setting,Value FROM `" << Tablename << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
		{
			while ( row = mysql_fetch_row( result_set.r ) )
			{
				mapSettings[row[0]]=row[1];
			}
		}

		pRepository->CreateSettingTable( );
		pRepository->CreateBatchHeaderTable( );
		pRepository->CreateBatchDetailTable( );
		pRepository->CreateTablesTable( );
		pRepository->CreateSchemaTable( );

		for(map<string,string>::iterator it=mapSettings.begin();it!=mapSettings.end();++it)
		{
			pRepository->SetSetting((*it).first,(*it).second);  
		}

		pRepository->GetSetting("schema","1");  // Be sure we at least have a default schema of 1
	}

	// We reset all the psc_rep_tables, so put back any tables in there
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() && !pTable->m_bIsSystemTable )
		{
			string Tablename = "psc_" + pTable->Repository_get()->Name_get() + "_tables"; /**< Our _tables table */
			ostringstream sql;
			sql << "INSERT INTO `" << Tablename << "` ( Tablename ) VALUES( '" << pTable->Name_get( ) << "' )";
			if( threaded_mysql_query( sql.str( ) )!=0 )
			{
				cerr << "SQL failed: " << sql.str( ) << endl;
				throw "Database Error";
			}
		}
	}
}
