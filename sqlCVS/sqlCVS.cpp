/*
 sqlCVS
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
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
		<< "8.	Reset psc_ fields in all tables ( reset-psc )" << endl
		<< "9.	History on all tables ( history-all )" << endl
		<< "10.	History on no tables ( history-none )" << endl
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
		return "reset-psc";
	else if( s=="9" )
		return "history-all";
	else if( s=="10" )
		return "history-none";
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

		cout<<"	Copyright (C) 2004 Pluto, Inc., a Florida Corporation"<<endl
		    <<"	www.plutohome.com"<<endl
		    <<"	Phone: +1 (877) 758-8648"<<endl
		    <<"	This program is distributed according to the terms of the Pluto Public License, available at: "<<endl
		    <<"	http://plutohome.com/index.php?section=public_license "<<endl
		    <<"	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "<<endl
		    <<"	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "<<endl
		    <<"	See the Pluto Public License for more details."<<endl;	
	
	bool bError=false; /** An error parsing the command line */

	string sUsers;

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
			<< "[-t Table( s )] [-U Users( s )]" << endl
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
			<< "-U users	   -- the users who are logged in " << endl;

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
		g_GlobalConfig.m_pDatabase=&database;

		/** Fill the lists with any repositories or tables that were passed in on the command line */
		database.GetRepositoriesTables( );

		string::size_type pos=0;
		string Token;
		while( ( Token=StringUtils::Tokenize( sUsers, ", ", pos ) ).length( ) )
		{
			string Password = StringUtils::Tokenize( sUsers, ", ", pos );
			g_GlobalConfig.m_mapUsersPasswords[atoi( Token.c_str( ) )]=Password;
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
			else if( g_GlobalConfig.m_sCommand=="reset-psc" )
			{
				database.Reset_psc();
			}
			else if( g_GlobalConfig.m_sCommand=="history-all" )
			{
				database.HasFullHistory_set_all(true);
			}
			else if( g_GlobalConfig.m_sCommand=="history-none" )
			{
				database.HasFullHistory_set_all(false);
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
return 0;
		};
	}
	catch( char *pException )
	{
		cerr << "Caught exception: " << pException;
	}

	delete g_pPlutoLogger;
#ifdef _WIN32
		WSACleanup( );
#endif
	return 0;
}
