/*
 sqlCVS
 
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/


/**
 * @file sqlCVS.cpp
 * @ brief  the main class for the sqlCVS
 *
*/


//#define C99_FORMAT_SPECIFIERS
#include "PlutoUtils/CommonIncludes.h"	

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#else
#include <conio.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
#include "RA/RAServer.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace sqlCVS;

vector<string> tables_list;

namespace DCE
{
	class Logger *g_pPlutoLogger;
}

namespace sqlCVS
{
	GlobalConfig g_GlobalConfig;
}

using namespace DCE;
using namespace sqlCVS;

string GetCommand( )
{
	cout << "What would you like to do?" << endl
		<< "In the future you can also make your selections on the command line using the command shown in parenthesis." << endl
		<< "------Server-side functions------" << endl
		<< "1.	Create a new sqlCVS repository from an existing database ( create )" << endl
		<< "2.	Edit an existing repository ( edit )" << endl
		<< "3.	Start listening for incoming connections from clients ( listen )" << endl
		<< "4.	Permanently roll-back checkins ( rollback )" << endl
		<< "5.	Create a 'dump' file with the tables in the current repository ( dump )" << endl
		<< "6.	List all the tables and what repositories they are in ( list-tables )" << endl
		<< "7.	List all the repositories and what tables they have ( list-repositories )" << endl
		<< "8.	History on all tables ( history-all )" << endl
		<< "9.	History on no tables ( history-none )" << endl
		<< "---The following are not normally used---" << endl
		<< "20.	Reset entire database--sqlCVS clients will be out of sync ( reset-all )" << endl
		<< "21.	Reset the system tables ( reset-sys )" << endl
		<< "22.	Update psc_id's ( update-psc )" << endl
		<< "------Client-side functions------" << endl
		<< "A.	Import a 'dump' file from a server and make a local, working copy ( import )" << endl
		<< "B.	Check-in changes you've made locally ( checkin )" << endl
		<< "C.	Update my local copy with changes from the server ( update )" << endl
		<< "D.	Synchronize my database with the server. Same as checkin+update ( sync )" << endl
		<< "E.	View my local changes ( diff )" << endl
		<< endl << "Q. Quit" << endl;

	string s;
	cin >> s;

	if( s=="1" )
		return "create";
	else if( s=="2" )
		return "edit";
	else if( s=="3" )
		return "listen";
	else if( s=="4" )
		return "rollback";
	else if( s=="5" )
		return "dump";
	else if( s=="6" )
		return "list-tables";
	else if( s=="7" )
		return "list-repositories";
	else if( s=="8" )
		return "history-all";
	else if( s=="9" )
		return "history-none";
	else if( s=="20" )
		return "reset-all";
	else if( s=="21" )
		return "reset-sys";
	else if( s=="22" )
		return "update-psc";
	else if( s=="a" || s=="A" )
		return "import";
	else if( s=="b" || s=="B" )
		return "checkin";
	else if( s=="c" || s=="C" )
		return "update";
	else if( s=="d" || s=="D" )
		return "sync";
	else if( s=="e" || s=="E" )
		return "diff";
	else if( s=="q" || s=="Q" )
		exit( 1 );

	return "";
}

int main( int argc, char *argv[] )
{

	cout << " Copyright (C) 2004 Pluto, Inc., a Florida Corporation " << endl
	    	<< " www.plutohome.com " << endl
	    	<< " Phone: +1 (877) 758-8648 " << endl
	    	<< " This program is free software; you can redistribute it " << endl
		<< " and/or modify it under the terms of the GNU General Public License. " << endl
		<< " This program is distributed in the hope that it will be useful, " << endl
		<< " but WITHOUT ANY WARRANTY; without even the implied warranty " <<endl
		<< " of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " <<endl
		<< " See the GNU General Public License for more details. "<< endl << "-------" << endl << endl;	

	bool bError=false; /** An error parsing the command line */

	string sUsers;
g_GlobalConfig.m_sDBHost="10.0.0.150";
g_GlobalConfig.m_sDBName="pluto_server"; // so I don't accidentally overwrite pluto_main
	char c;
	for( int optnum=1;optnum<argc;++optnum )
	{
		if( argv[optnum][0]!='-' )
		{
			g_GlobalConfig.m_sCommand = argv[optnum];
			if( optnum<argc-1 )
				bError=true;	/** The command must be the last thing on the line */
			break;
		}

		c=argv[optnum][1];
		switch ( c )
		{
		case 'h':
			g_GlobalConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			g_GlobalConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			g_GlobalConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			g_GlobalConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			g_GlobalConfig.m_iDBPort = atoi( argv[++optnum] );
			break;
		case 'r':
			g_GlobalConfig.m_sRepository = argv[++optnum];
			break;
		case 't':
			g_GlobalConfig.m_sTable = argv[++optnum];
			break;
		case 'd':
			g_GlobalConfig.m_sDefaultUser = argv[++optnum];
			break;
		case 'U':
			sUsers = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		
//                123456789012345678901234567890123456789012345678901234567890	
		cout << "sqlCVS, v." << VERSION << endl
			<< "Usage: sqlCVS [-h hostname] [-u username] [-p password]" << endl
			<< "[-D database] [-P mysql port] [-r Repository( -ies )]" << endl
			<< "[-t Table( s )] [-U Username[~Password][,...]] [-d username]" << endl
			<< "-h hostname    -- address or DNS of database host," << endl
			<< "			default is `dce_router`" << endl
			<< "-u username    -- username for database connection" << endl
			<< "-p password    -- password for database connection, "<< endl
			<< "			default is `` ( empty )" << endl
			<< "-D database    -- database name. default is pluto_main" << endl
			<< "-P port    	   -- port for database connection, " << endl
			<< "			default is 3306" << endl
			<< "-r output path -- Where to put the output files. "<< endl
			<< "			Default is ../[database name]" << endl
			<< "-t input path  -- Where to find the template files. " << endl
			<< "			Default is . then ../sqlCVS" << endl
			<< "-U user~pass   -- the user(s) who are logged in and will be committing rows" << endl
			<< "-d username    -- the owner of any unclaimed new records" << endl
			<< "            Default is the first user checking in records" << endl;

		exit( 0 );
	}

	try
	{
		g_pPlutoLogger = new DCE::FileLogger( stdout );
		if( g_pPlutoLogger == NULL )
		{
			cerr << "Problem creating logger. Check params." << endl;
			exit( 1 );
		}

#ifdef WIN32
		long err;
		WSADATA wsaData;
		err = WSAStartup( MAKEWORD( 1, 1 ), ( LPWSADATA ) &wsaData );
#endif
		Database database( g_GlobalConfig.m_sDBHost, g_GlobalConfig.m_sDBUser, g_GlobalConfig.m_sDBPassword, g_GlobalConfig.m_sDBName, g_GlobalConfig.m_iDBPort );
		if( !database.m_bConnected )
		{
			cerr << "Cannot connect to database" << endl;
			exit(1);
		}
		g_GlobalConfig.m_pDatabase=&database;

		/** Fill the lists with any repositories or tables that were passed in on the command line */
		database.GetRepositoriesTables( );

		string::size_type pos=0;
		string Token;
		while( ( Token=StringUtils::Tokenize( sUsers, ",", pos ) ).length( ) )
		{
			string::size_type pos2=0;
			string Username = StringUtils::Tokenize( Token, "~", pos2 );
			string Password = StringUtils::Tokenize( Token, "~", pos2 );
			g_GlobalConfig.m_mapUsersPasswords[Username]=Password;
		}

		while( true ) /** An endless loop processing commands */
		{
			if( g_GlobalConfig.m_sCommand.length( )==0 )
			{
				g_GlobalConfig.m_sCommand=GetCommand( );
				database.m_bInteractiveMode=true;
			}
			if( database.bIsInvalid( ) && g_GlobalConfig.m_sCommand!="import" )
			{
				g_GlobalConfig.m_sCommand="";
				cerr << "Database is invalid. Import is the only available option" << endl;
				continue;
			}
			if( g_GlobalConfig.m_sCommand=="create" )
				database.CreateRepository( );
			else if( g_GlobalConfig.m_sCommand=="checkin" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{  
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.CheckIn( );
			}
			else if( g_GlobalConfig.m_sCommand=="update" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{  
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.Update( );
			}
			else if( g_GlobalConfig.m_sCommand=="listen" )
			{
				RAServer *pServer = new RAServer( );
				pServer->Initialize( );
				pServer->StartListening( g_GlobalConfig.m_iSqlCVSPort );
				pServer->Run( );
				delete pServer;
			}
			else if( g_GlobalConfig.m_sCommand=="dump" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{  
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.Dump( );
			}
			else if( g_GlobalConfig.m_sCommand=="import" )
			{
				database.Import( );
			}
			else if( g_GlobalConfig.m_sCommand=="list-tables" )
			{
				database.ListTables( );
			}
			else if( g_GlobalConfig.m_sCommand=="list-repositories" )
			{
				database.ListRepositories( );
			}
			else if( g_GlobalConfig.m_sCommand=="reset-all" )
			{
				database.Reset_all();
			}
			else if( g_GlobalConfig.m_sCommand=="reset-sys" )
			{
				database.Reset_sys();
			}
			else if( g_GlobalConfig.m_sCommand=="update-psc" )
			{
				database.Update_psc();
			}
			else if( g_GlobalConfig.m_sCommand=="history-all" )
			{
				database.HasFullHistory_set_all(true);
			}
			else if( g_GlobalConfig.m_sCommand=="history-none" )
			{
				database.HasFullHistory_set_all(false);
			}
			else if( g_GlobalConfig.m_sCommand=="diff" )
			{
				database.ShowChanges();
			}
		
			if( database.m_bInteractiveMode )
			{
				cout << "To do this non-interactively, use the following command:" << endl;
				cout << "sqlCVS -h \"" << g_GlobalConfig.m_sDBHost << "\" -u \"" << g_GlobalConfig.m_sDBUser 
					<< "\" -p \"" << g_GlobalConfig.m_sDBPassword << "\" -D \"" << g_GlobalConfig.m_sDBName 
					<< "\" -P \"" << g_GlobalConfig.m_iDBPort << "\" -r \"" << g_GlobalConfig.m_sRepository ;
				if( g_GlobalConfig.m_mapRepository.size() )
				{
					for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
						cout << (g_GlobalConfig.m_sRepository.size() || it!=g_GlobalConfig.m_mapRepository.begin() ? "," : "") << (*it).first;
				}
				cout << "\" -t \"" << g_GlobalConfig.m_sTable;
				if( g_GlobalConfig.m_mapTable.size()  )
				{
					for(MapTable::iterator it=g_GlobalConfig.m_mapTable.begin();it!=g_GlobalConfig.m_mapTable.end();++it)
						cout << (g_GlobalConfig.m_sTable.size() || it!=g_GlobalConfig.m_mapTable.begin() ? "," : "") << (*it).first;
				}
				
				cout << "\" -U \"" << sUsers
					<< "\" " << g_GlobalConfig.m_sCommand << endl;
			}
/** @test
			else if( Command=="add-tables" )
				database.AddTablesToRepository( sRepository, &listCommandParms );
			else if( Command=="remove-tables" )
				database.RemoveTablesFromRepository( sRepository, &listCommandParms );
			else if( Command=="listen" )
				rp.Listen( iListenPort, &database, sRepository );
			else if( Command=="dump" )
				database.DumpTables( sRepository, &listCommandParms );

*/
getch();
return 0;
		};
	}
	catch( char *pException )
	{
		cerr << "Caught exception: " << pException << endl;
getch();
		exit(1);
	}
	catch( const char *pException )
	{
		cerr << "Caught exception: " << pException << endl;
getch();
		exit(1);
	}
	catch( string sException )
	{
		cerr << "Caught exception: " << sException << endl;
getch();
		exit(1);
		return 1;
	}
	catch( ... )
	{
		cerr << "Caught unknown exception" << endl;
getch();
		exit(1);
	}

	delete g_pPlutoLogger;
#ifdef _WIN32
		WSACleanup( );
#endif
	cout << "Everything okay";
	return 0;
}
