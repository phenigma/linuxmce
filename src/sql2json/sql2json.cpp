/*
	sql2json
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

	See the GNU General Public License for more details.
	*/

/**
*
* @file sql2json.cpp
* @brief entry point for the sql2json project
* @author Cristian Miron
*/


#include "PlutoUtils/CommonIncludes.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DatabaseInfo.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
#include "json_generator.h"

#define  VERSION "<=version=>"

using namespace std;

vector<string> g_TablesList;

using namespace DCE; 

int main( int argc, char *argv[] )
{
	cout << " Copyright (C) 2007 Pluto, Inc., a Florida Corporation " << endl
		<< " www.plutohome.com " << endl
		<< " Phone: +1 (877) 758-8648 " << endl
		<< " This program is free software; you can redistribute it " << endl
		<< " and/or modify it under the terms of the GNU General Public License. " << endl
		<< " This program is distributed in the hope that it will be useful, " << endl
		<< " but WITHOUT ANY WARRANTY; without even the implied warranty " <<endl
		<< " of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " <<endl
		<< " See the GNU General Public License for more details. "<< endl << "-------" << endl << endl;	

	// setting some default values.... probablly just temporary
	string sDBHost="dcerouter", sDBUser="root", sDBPassword="", sDBName="pluto_main";
	int iDBPort=3306;

	bool bError = false; // An error parsing the command line
	char c;

	// parsing the command parameters
	for( int optnum=1; optnum < argc; ++optnum )
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c = argv[optnum][1];
		switch (c)
		{
		case 'h':
			sDBHost = argv[++optnum];
			break;
		case 'u':
			sDBUser = argv[++optnum];
			break;
		case 'p':
			sDBPassword = argv[++optnum];
			break;
		case 'D':
			sDBName = argv[++optnum];
			break;
		case 'P':
			iDBPort = atoi(argv[++optnum]);
			break;
		default:
			bError=true;
			break;
		};
	}

	// checking for errors
	if ( bError)
	{

		//                123456789012345678901234567890123456789012345678901234567890			
		cout << "sql2json, v." << VERSION << endl
			<< "Usage: sql2cpp [-h hostname] [-u username] [-p password]" << endl
			<< "[-D database] [-P mysql port] [-o output path]" << endl
			<< "-h hostname    -- address or DNS of database host, " <<endl
			<< "			 default is `dcerouter`" << endl
			<< "-u username    -- username for database connection" << endl
			<< "-p password    -- password for database connection," << endl
			<< "			 default is `` (empty)" << endl
			<< "-D database    -- database name.  default is pluto_main" << endl
			<< "-P port        -- port for database connection, default is 3306" << endl;
		exit(1); // errors
	}

	DatabaseInfo dbInfo( sDBHost, sDBUser, sDBPassword, sDBName, iDBPort ); // creating the DatabaseInfo object
	if( !dbInfo.m_bConnected )
	{
		cerr << "Cannot connect.  Aborting" << endl;
		exit(1);
	}

	dbInfo.getDatabaseTablesList(); // reading tables list	
	dbInfo.ConvertTablesToStrings( &g_TablesList ); // obtaining their names

	vector<class TableInfo_Generator *> *p_listTableInfo=dbInfo.listTableInfo_get(); // reading tables info

	cout << "Loaded information for " << (unsigned int)p_listTableInfo->size() << " tables" << endl; // output stuff

	json_generator jg(sDBName);
	FileUtils::WriteTextFile(sDBName + ".json", jg.generate_json(p_listTableInfo));

	cout << "Classes generated successfully!" << endl;
	return EXIT_SUCCESS;
}


