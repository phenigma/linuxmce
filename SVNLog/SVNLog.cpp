/*
SVNLog
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
* @file SVNLog.cpp
* @brief entry point for the SVNLog project
* @author Aaron
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

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#endif

#define  VERSION "<=version=>"

using namespace std;

class DCELogger *g_pDCELogger;

class Commit
{
public:
	string User;
	int Lines;
	int Revision;
	time_t date;
};

map<int,Commit *> mapCommits;
map<string,Commit *> mapUser;

typedef struct pair<string,int> PAIR_SI;

map<PAIR_SI,int> mapUsersWeeks;

int main( int argc, char *argv[] )
{
	/*	
	cout << " Copyright (C) 2004 Pluto, Inc., a Florida Corporation " << endl
	<< " www.plutohome.com " << endl
	<< " Phone: +1 (877) 758-8648 " << endl
	<< " This program is free software; you can redistribute it " << endl
	<< " and/or modify it under the terms of the GNU General Public License. " << endl
	<< " This program is distributed in the hope that it will be useful, " << endl
	<< " but WITHOUT ANY WARRANTY; without even the implied warranty " <<endl
	<< " of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " <<endl
	<< " See the GNU General Public License for more details. "<< endl << "-------" << endl << endl;	

	// setting some default values.... probablly just temporary
	string sDBHost="dce_router", sDBUser="root", sDBPassword="", sDBName="pluto_main", sOutputPath="", sInputPath=".";
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
	case 'o':
	sOutputPath = argv[++optnum];
	break;
	case 'i':
	sInputPath = argv[++optnum];
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
	cout << "SVNLog, v." << VERSION << endl
	<< "Usage: SVNLog [-h hostname] [-u username] [-p password]" << endl
	<< "[-D database] [-P mysql port] [-o output path] [-i input path]" << endl
	<< "-h hostname    -- address or DNS of database host, " <<endl
	<< "			 default is `dce_router`" << endl
	<< "-u username    -- username for database connection" << endl
	<< "-p password    -- password for database connection," << endl
	<< "			 default is `` (empty)" << endl
	<< "-D database    -- database name.  default is pluto_main" << endl
	<< "-P port        -- port for database connection, default is 3306" << endl
	<< "-o output path -- Where to put the output files. "
	<< "			 Default is ../[database name]" << endl
	<< "-i input path  -- Where to find the template files. " << endl
	<< "			 Default is . then ../SVNLog" << endl;

	exit(0); // errors
	}
	*/

	chdir("/home/Work");
	unlink("output");
	system("svn log -q | grep '^r' > output");

	vector<string> vectLines;
	size_t size;
	string Lines = FileUtils::ReadFileIntoBuffer("output",size);
	cout << "File is: " << Lines.size() << " bytes." << endl;
	StringUtils::Tokenize(Lines,"\r\n",vectLines);
	cout << "File is: " << vectLines.size() << " lines" << endl;

	int LowestWeek=0,HighestWeek=0;

	for(size_t s=0;s<vectLines.size();++s)
	{
		cout << "Line: " << s << " of " << vectLines.size() << endl;
		string Line = vectLines[s];
		int Revision = atoi(Line.substr(1).c_str());
		if( Revision==1 )
			continue;

		string::size_type start_pipe = Line.find('|');
		string::size_type stop_pipe = Line.find('|',start_pipe+1);
		string User = Line.substr(start_pipe+2,stop_pipe-start_pipe-3);
string x = Line.substr(stop_pipe+2);
string y = Line.substr(stop_pipe+7);
string z = Line.substr(stop_pipe+10);
		int Year = atoi(Line.substr(stop_pipe+2).c_str());
		int Month = atoi(Line.substr(stop_pipe+7).c_str());
		int Day = atoi(Line.substr(stop_pipe+10).c_str());

		struct tm tm_time;
		tm_time.tm_year = Year-1900;
		tm_time.tm_mon = Month-1;
		tm_time.tm_mday = Day;
		tm_time.tm_hour = 0;
		tm_time.tm_min = 0;
		tm_time.tm_sec = 0;
		time_t t = mktime(&tm_time);
		int Week = tm_time.tm_yday / 7;

		if( Week > HighestWeek )
			HighestWeek=Week;
		if( Week < LowestWeek || LowestWeek==0 )
			LowestWeek=Week;

		string Cmd = "svn diff --revision " + StringUtils::itos(Revision-1) + ":" + StringUtils::itos(Revision) + " | grep '^Index:\\|^\\@\\@' > output";
		system(Cmd.c_str());
		vector<string> vectFiles;
		Lines = FileUtils::ReadFileIntoBuffer("output",size);
		cout << "File is: " << Lines.size() << " bytes." << endl;
		StringUtils::Tokenize(Lines,"\r\n",vectFiles);
		cout << "File is: " << vectFiles.size() << " lines" << endl;
		int LinesChanged=0;
		for(size_t s2=0;s2<vectFiles.size();++s2)
		{
			string File = vectFiles[s2];
			if( File.substr(0,2)!="@@" )
				continue;

			string::size_type first = File.find('-');
			first = File.find(',',first+1);
			int LinesMinus = first!=string::npos ? atoi(File.substr(first+1).c_str()) : 0;

			first = File.find('+',first+1);
			first = File.find(',',first+1);
			int LinesPlus = first!=string::npos ? atoi(File.substr(first+1).c_str()) : 0;

			File = vectFiles[s2-1].substr(7);
			if( StringUtils::StartsWith(File,"Libraries") || StringUtils::StartsWith(File,"pluto_main") || StringUtils::StartsWith(File,"Gen_Devices") )
			{
				cout << "Skipping: " << File << endl;
				continue;
			}
			cout << "Logging: " << File << " Lines: " << LinesChanged << endl;
			LinesChanged += LinesMinus + LinesPlus;
		}

		Commit *pCommit = new Commit();
		pCommit->date = t;
		pCommit->Lines = LinesChanged;
		pCommit->Revision = Revision;
		pCommit->User = User;
		mapCommits[Revision] = pCommit;
		mapUser[User] = pCommit;

		PAIR_SI psi(User,Week);
		int LinesSoFar = mapUsersWeeks[psi];
		LinesSoFar += LinesChanged;
		mapUsersWeeks[psi] = LinesChanged;
		break;
	}

	cout << "\t";
	for(map<string,Commit *>::iterator it=mapUser.begin();it!=mapUser.end();++it)
		cout << (*it).first << "\t";
	cout << endl;
	for(int i=LowestWeek;i<=HighestWeek;++i)
	{
		cout << i << "\t";
		for(map<string,Commit *>::iterator it=mapUser.begin();it!=mapUser.end();++it)
		{
			PAIR_SI psi( (*it).first,i);
			int Lines = mapUsersWeeks[psi];
			cout << Lines << "\t";
		}
		cout << endl;
	}
}
