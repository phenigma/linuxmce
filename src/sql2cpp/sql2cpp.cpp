/*
	sql2cpp
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
 * @file sql2cpp.cpp
 * @brief entry point for the sql2cpp project
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
#include "DatabaseInfo.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"

#include "version.h"

using namespace std;

vector<string> g_TablesList;

using namespace DCE; 

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
	
	// setting some default values.... probablly just temporary
	string sDBHost="dcerouter", sDBUser="root", sDBPassword="", sDBName="pluto_main", sOutputPath="", sInputPath=".";
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
		cout << "sql2cpp, v." << VERSION << endl
			<< "Usage: sql2cpp [-h hostname] [-u username] [-p password]" << endl
			<< "[-D database] [-P mysql port] [-o output path] [-i input path]" << endl
			<< "-h hostname    -- address or DNS of database host, " <<endl
			<< "			 default is `dcerouter`" << endl
			<< "-u username    -- username for database connection" << endl
			<< "-p password    -- password for database connection," << endl
			<< "			 default is `` (empty)" << endl
			<< "-D database    -- database name.  default is pluto_main" << endl
			<< "-P port        -- port for database connection, default is 3306" << endl
			<< "-o output path -- Where to put the output files. "
			<< "			 Default is ../[database name]" << endl
			<< "-i input path  -- Where to find the template files. " << endl
			<< "			 Default is . then ../sql2cpp" << endl;

		exit(1); // errors
	}

	// default value for sOutputPath
	if( sOutputPath=="" )
		sOutputPath = "../" + sDBName;

	StringUtils::Replace( &sOutputPath, "\\", "/" ); // Unix path

	if( sOutputPath[sOutputPath.length()-1]!='/' ) // adding a final /
		sOutputPath+="/";

	FileUtils::MakeDir( sOutputPath ); // creating the directory

	if( !FileUtils::FileExists( sInputPath + "/table.cpp.template" ) )
	{
		if( sInputPath != "." ) // changed from the default
		{
			cout << "Cannot find the templates in " << sInputPath << endl;
			exit(1);
		}

		sInputPath="../sql2cpp/"; // default path for templates
		if( !FileUtils::FileExists( sInputPath + "table.cpp.template" ) )
		{
			cout << "Cannot find the templates.  Please specify an input path." << endl;
			exit(1); // error, default files missing
		}
	}

	if( sInputPath[sInputPath.length()-1] != '/' )
		sInputPath += "/"; // adding a final /

	DatabaseInfo dbInfo( sDBHost, sDBUser, sDBPassword, sDBName, iDBPort ); // creating the DatabaseInfo object
	if( !dbInfo.m_bConnected )
	{
		cerr << "Cannot connect.  Aborting" << endl;
		exit(1);
	}

	dbInfo.getDatabaseTablesList(); // reading tables list	
	dbInfo.ConvertTablesToStrings( &g_TablesList ); // obtaining their names

	vector<class TableInfo_Generator *> *p_listTableInfo=dbInfo.listTableInfo_get(); // reading tables info

	cout << "Loaded information for " << p_listTableInfo->size() << " tables" << endl; // output stuff

	vector<class TableInfo_Generator *>::iterator it;
	for ( it=p_listTableInfo->begin(); it<p_listTableInfo->end(); ++it ) // processing tables
	{
		TableInfo *pTableInfo = (*it);

		// don't create accessor files for pluto sqlcvs history
		if ( pTableInfo->get_table_name().find("_psch")!=string::npos )
			continue;

		if (pTableInfo->HasPrimaryKeys())
		{
			string s;	
			char buf[1024];

			{
				string sHeaderName = sOutputPath + "Table_" + pTableInfo->get_table_name() + ".h";

				ifstream fin( ( sInputPath + "table.h.template" ).c_str() );
				ofstream fout( sHeaderName.c_str() );

				while ( fin )
				{
					//replacing all @VAR@ variables
					fin.getline( buf, 1024 );				
					s = buf;

					int i1, i2;

					while ((i1 = s.find('@')) != -1)
					{
						i2 = s.find('@', i1+1);
						string sVar = s.substr( i1+1, i2-i1-1 );
						string sExpVar;
						if (sVar == "database_name")
							sExpVar = sDBName;
						else
							sExpVar = (*it)->ExpandString(sVar);
						s.replace(i1, i2-i1+1, sExpVar); 								
					} 	

					fout << s << endl;
				}
			}

			{
				string cpp_name = sOutputPath + "Table_" + (*it)->get_table_name() + ".cpp";

				ifstream fin( (sInputPath + "table.cpp.template").c_str() );
				ofstream fout(cpp_name.c_str());

				while (fin)
				{
					//replacing all @VAR@ variables
					fin.getline(buf, 1024);				
					s = buf;

					int i1, i2;

					while ((i1 = s.find('@')) != -1)
					{
						i2 = s.find('@', i1+1);
						string sVar = s.substr(i1+1, i2-i1-1);
						string sExpVar;
						if ( sVar == "database_name" )
							sExpVar = sDBName;
						else
							sExpVar = (*it)->ExpandString( sVar );
						s.replace(i1, i2-i1+1, sExpVar); 								
					} 	

					fout << s << endl;
				}
			}

			{
				string sDefineName = sOutputPath + "Define_" + (*it)->get_table_name() + ".h";

				ifstream fin( (sInputPath + "define.h.template").c_str() );
				ofstream fout(sDefineName.c_str());
				while (fin)
				{
					//replacing all @VAR@ variables
					fin.getline(buf, 1024);				
					s = buf;

					int i1, i2;

					while ((i1 = s.find('@')) != -1)
					{
						i2 = s.find('@', i1+1);
						string sVar = s.substr(i1+1, i2-i1-1);
						string sExpVar;
						if (sVar == "database_name")
							sExpVar = sDBName;
						else
							sExpVar = (*it)->ExpandString(sVar);
						s.replace(i1, i2-i1+1, sExpVar); 								
					} 	

					fout << s << endl;
				}
			}
		}	
	}

	// Make a vcproject files.
	string Sources="\t\t\t<File\r\n\t\t\t\tRelativePath=\".\\Database_" + sDBName + ".cpp\">\r\n\t\t\t</File>\r\n";
	Sources += "\t\t\t<File\r\n\t\t\t\tRelativePath=\".\\TableRow.cpp\">\r\n\t\t\t</File>\r\n";

	for ( it=p_listTableInfo->begin(); it<p_listTableInfo->end(); ++it )
	{
		TableInfo *pTableInfo = (*it);

		// don't add accessor files for pluto sqlcvs history
		if ( pTableInfo->get_table_name().find("_psch")!=string::npos )
			continue;

		Sources += "\t\t\t<File\r\n\t\t\t\tRelativePath=\".\\Table_" + pTableInfo->get_table_name() + ".cpp\">\r\n\t\t\t</File>\r\n";
	}

	StringUtils::Replace( sInputPath + "database.vcproj.template", sOutputPath + sDBName + ".vcproj", "@sources@", Sources );
	StringUtils::Replace( sOutputPath + sDBName + ".vcproj", sOutputPath + sDBName + ".vcproj", "*DBName*",sDBName );

	//preparing header

	string db_h = sOutputPath + "Database_" + sDBName + ".h";
	string makefile = sOutputPath + "Makefile";

	ofstream db_h_out(db_h.c_str());
	ofstream makefile_out(makefile.c_str());

	makefile_out << "ROOT = .." << endl;

	makefile_out << "CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3" << endl;
	makefile_out << "CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG" << endl;
	makefile_out << "CXX = g++" << endl;
	makefile_out << "LDFLAGS = -L$(ROOT)/lib" << endl;

	makefile_out << "sources = Database_" << sDBName << ".cpp TableRow.cpp";

	makefile_out << endl; 
	makefile_out << "#<-mkr_B_use_mysql_wrapper_b->" << endl;
	makefile_out << "CPPFLAGS += -I/usr/include/mysql" << endl;
	makefile_out << "LDFLAGS += -lmysqlclient_r" << endl;
	makefile_out << "#<-mkr_B_use_mysql_wrapper_e->" << endl;
	makefile_out << endl;
	makefile_out << "#" << endl;
	makefile_out << endl;
	makefile_out << "sources += ";


	db_h_out << "#ifndef __Database_"<<sDBName<<"_H_" << endl;
	db_h_out << "#define __Database_"<<sDBName<<"_H_" << endl;

	db_h_out << "#include \"PlutoUtils/DBHelper.h\"" << endl;
	db_h_out << "#include \"DCE/Logger.h\"" << endl;

    db_h_out << "#ifdef WIN32" << endl;
    db_h_out << "#ifdef EXPORT_DLL" << endl;
    db_h_out << "\t#ifndef DECLSPECIFIER" << endl;
    db_h_out << "\t\t#define DECLSPECIFIER __declspec(dllexport)" << endl;
    db_h_out << "\t#endif" << endl;
    db_h_out << "#else" << endl;
    db_h_out << "\t#ifndef DECLSPECIFIER" << endl;
    db_h_out << "\t\t#define DECLSPECIFIER __declspec(dllimport)" << endl;
    db_h_out << "\t#endif" << endl;
    db_h_out << "#endif" << endl;
    db_h_out << "#else" << endl;
    db_h_out << "#define DECLSPECIFIER" << endl;
    db_h_out << "#endif" << endl;

    db_h_out << "class DECLSPECIFIER DBHelper;" << endl;
    db_h_out << "class DECLSPECIFIER SerializeClass;" << endl;
    db_h_out << endl;

	db_h_out << "class DECLSPECIFIER Database_" << sDBName << ": public DBHelper" << endl;
	db_h_out << "{" << endl;
	db_h_out << "public:" << endl;
	db_h_out << "Database_" << sDBName << "(Logger *pLogger=NULL);" << endl;
	db_h_out << "~Database_" << sDBName << "();" << endl;
	db_h_out << "void DeleteAllTables();" << endl;


	db_h_out << "private:" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_h_out << "class Table_" + (*i)->get_table_name() + "* tbl"+(*i)->get_table_name()+";" << endl;
		db_h_out << "bool Commit_" + (*i)->get_table_name() + "(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);" << endl;
		makefile_out << " \\" << endl << "Table_" + (*i)->get_table_name() + ".cpp";
	}

	makefile_out << endl << endl << "all: lib" << sDBName << ".so all-recursive" << endl << endl;


	makefile_out << "lib" << sDBName << ".so: $(sources:.cpp=.o)" << endl;
	makefile_out << "\t$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)" << endl;
	makefile_out << "\tcp $@ ../lib" << endl << endl;
	makefile_out << "clean: clean-recursive" << endl;
	makefile_out << "\trm -f *.o *.d lib" << sDBName << ".so >/dev/null" << endl << endl;
	makefile_out << "install:" << endl;
	makefile_out << "\tmkdir -p $(DESTDIR)/usr/pluto/bin" << endl;
	makefile_out << "\tcp $@ /usr/pluto/bin" << endl << endl;
	makefile_out << "include $(ROOT)/PlutoUtils/make.rules" << endl;
	makefile_out << "-include $(sources:.cpp=.d)" << endl;

	db_h_out << "public:" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i<dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_h_out << "class Table_" + (*i)->get_table_name() + "* " + (*i)->get_table_name() + "_get() { if( !tbl"+(*i)->get_table_name()+" ) CreateTable_" + (*i)->get_table_name() + "(); return tbl"+(*i)->get_table_name()+"; }" << endl;
	}
	db_h_out << "string m_sLastDBError;" << endl;
	db_h_out << "bool Connect(string host, string user, string pass, string sDBName, int port=3306);" << endl;
	db_h_out << "bool Connect(class DCEConfig *pDCEConfig);" << endl;
	db_h_out << "void Disconnect();" << endl;
	db_h_out << "bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);" << endl;

	db_h_out << "private:" << endl;

	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i<dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_h_out << "void CreateTable_" + (*i)->get_table_name() + "();" << endl;
	}
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_h_out << "void DeleteTable_" + (*i)->get_table_name() + "();" << endl;
	}

	db_h_out << "};" << endl;

	db_h_out << "#endif" << endl;


	string db_cpp = sOutputPath + "Database_" + sDBName + ".cpp";
	ofstream db_cpp_out(db_cpp.c_str());

	db_cpp_out << "#ifdef WIN32" << endl << "\t#include <WinSock2.h>" << endl << "#endif" << endl;

	db_cpp_out << "#include <iostream>" << endl
		<< "#include <stdio.h>" << endl
		<< "#include <string>" << endl
		<< endl
		<< "#include <map>" << endl
		<< "#include <vector>" <<  endl
		<< endl
		<< "using namespace std;" << endl;

	db_cpp_out << endl << "#include \"Database_" + sDBName + ".h\"" << endl << endl;
	db_cpp_out << endl << "#include \"DCEConfig.h\"" << endl << endl;

    db_cpp_out << endl << "using namespace DCE;" << endl;

	db_cpp_out << "Database_" << sDBName << "::Database_" << sDBName << "(Logger *pLogger)" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "\tLoggerWrapper::SetInstance(pLogger);" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_cpp_out << "tbl"+(*i)->get_table_name()+"=NULL;" << endl;
	}
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "Database_" << sDBName << "::~Database_" << sDBName << "()" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "\tDeleteAllTables();" << endl;
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "bool Database_" << sDBName << "::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "bool bResult=true;" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_cpp_out << "if( tbl"+(*i)->get_table_name()+"!=NULL )" << endl;
		db_cpp_out << "\tif( !Commit_"+(*i)->get_table_name()+"(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )" << endl;
		db_cpp_out << "\t\tbResult=false;" << endl;
	}
	db_cpp_out << "return bResult;" << endl << endl;
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "void Database_" << sDBName << "::DeleteAllTables()" << endl;
	db_cpp_out << "{" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		// don't create accessor files for pluto sqlcvs history
		if ( (*i)->get_table_name().find("_psch")!=string::npos )
			continue;

		db_cpp_out << "DeleteTable_"+(*i)->get_table_name()+"();" << endl;
	}
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "bool Database_" << sDBName << "::Connect(string host, string user, string pass, string sDBName, int port)" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "return DBConnect(host, user, pass, sDBName, port);" << endl;

	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "bool Database_" << sDBName << "::Connect(class DCEConfig *pDCEConfig)" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "\treturn Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);" << endl;
	db_cpp_out << "}" << endl;

	string tbrow_h = sOutputPath + "TableRow.h";
	FileUtils::PUCopyFile(sInputPath + "TableRow.h",tbrow_h);

	string tbrow_cpp = sOutputPath + "TableRow.cpp";
	FileUtils::PUCopyFile(sInputPath + "TableRow.cpp",tbrow_cpp);

    //def file
    string sBuffer = "EXPORTS\r\n";
    FileUtils::WriteBufferIntoFile(sOutputPath + sDBName + ".def", const_cast<char *>(sBuffer.c_str()), sBuffer.length());

	cout << "Classes generated successfully!" << endl;

	return EXIT_SUCCESS;
}
