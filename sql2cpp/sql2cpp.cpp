//#define C99_FORMAT_SPECIFIERS
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

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DatabaseInfo.h"
#include "CommonFunctions.h"

#define SQL2CPP_VERSION "0.1"

using namespace std;

vector<string> tables_list;
class Logger *g_pPlutoLogger;

int main(int argc, char *argv[])
{
	string DBHost="dce_router",DBUser="root",DBPassword="",DBName="pluto_main",OutputPath="",InputPath=".";
	int DBPort=3306;

	bool bError=false; // An error parsing the command line
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			DBHost = argv[++optnum];
			break;
		case 'u':
			DBUser = argv[++optnum];
			break;
		case 'p':
			DBPassword = argv[++optnum];
			break;
		case 'D':
			DBName = argv[++optnum];
			break;
		case 'P':
			DBPort = atoi(argv[++optnum]);
			break;
		case 'o':
			OutputPath = argv[++optnum];
			break;
		case 'i':
			InputPath = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "sql2cpp, v." << SQL2CPP_VERSION << endl
			<< "Usage: sql2cpp [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-o output path] [-i input path]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "output path -- Where to put the output files.  Default is ../[database name]" << endl
			<< "input path  -- Where to find the template files.  Default is . then ../sql2cpp" << endl;

		exit(0);
	}

	if( OutputPath=="" )
		OutputPath = "../" + DBName;

	OutputPath = StringUtils::Replace(OutputPath,"\\","/");

	if( OutputPath[OutputPath.length()-1]!='/' )
		OutputPath+="/";

	FileUtils::MakeDir(OutputPath);

	if( !FileUtils::FileExists(InputPath + "/table.cpp.template") )
	{
		if( InputPath!="." ) // changed from the default
		{
			cout << "Cannot find the templates in " << InputPath << endl;
			exit(1);
		}

		InputPath="../sql2cpp/";
		if( !FileUtils::FileExists(InputPath + "table.cpp.template") )
		{
			cout << "Cannot find the templates.  Please specify an input path." << endl;
			exit(1);
		}
	}

	if( InputPath[InputPath.length()-1]!='/' )
		InputPath+="/";

	DatabaseInfo dbInfo(DBHost, DBUser, DBPassword, DBName, DBPort);
	// Reading tables list	
	dbInfo.getDatabaseTablesList();
	dbInfo.ConvertTablesToStrings(&tables_list);

	// Reading tables info
	vector<class TableInfo_Generator *> *p_listTableInfo=dbInfo.listTableInfo_get();

	cout << "Loaded information for " << (int) p_listTableInfo->size() << " tables" << endl;

	vector<class TableInfo_Generator *>::iterator it;
	for (it=p_listTableInfo->begin();it<p_listTableInfo->end();++it)
	{
		TableInfo *pTableInfo = (*it);

		if (pTableInfo->HasPrimaryKeys())
		{
			string s;	
			char buf[1024];

			{
				string header_name = OutputPath + "Table_" + pTableInfo->get_table_name() + ".h";

				ifstream fin( (InputPath + "table.h.template").c_str() );
				ofstream fout(header_name.c_str());

				while (fin)
				{
					//replacing all @VAR@ variables
					fin.getline(buf, 1024);				
					s = buf;

					int i1, i2;

					while ((i1 = (int) s.find('@')) != -1)
					{
						i2 = (int) s.find('@', i1+1);
						string var = s.substr(i1+1, i2-i1-1);
						string exp_var;
						if (var == "database_name")
							exp_var = DBName;
						else
							exp_var = (*it)->ExpandString(var);
						s.replace(i1, i2-i1+1, exp_var); 								
					} 	

					fout << s << endl;
				}
			}

			{
				string cpp_name = OutputPath + "Table_" + (*it)->get_table_name() + ".cpp";

				ifstream fin( (InputPath + "table.cpp.template").c_str() );
				ofstream fout(cpp_name.c_str());

				while (fin)
				{
					//replacing all @VAR@ variables
					fin.getline(buf, 1024);				
					s = buf;

					int i1, i2;

					while ((i1 = (int) s.find('@')) != -1)
					{
						i2 = (int) s.find('@', i1+1);
						string var = s.substr(i1+1, i2-i1-1);
						string exp_var;
						if (var == "database_name")
							exp_var = DBName;
						else
							exp_var = (*it)->ExpandString(var);
						s.replace(i1, i2-i1+1, exp_var); 								
					} 	

					fout << s << endl;
				}
			}

			{
				string define_name = OutputPath + "Define_" + (*it)->get_table_name() + ".h";

				ifstream fin( (InputPath + "define.h.template").c_str() );
				ofstream fout(define_name.c_str());
				while (fin)
				{
					//replacing all @VAR@ variables
					fin.getline(buf, 1024);				
					s = buf;

					int i1, i2;

					while ((i1 = (int) s.find('@')) != -1)
					{
						i2 = (int) s.find('@', i1+1);
						string var = s.substr(i1+1, i2-i1-1);
						string exp_var;
						if (var == "database_name")
							exp_var = DBName;
						else
							exp_var = (*it)->ExpandString(var);
						s.replace(i1, i2-i1+1, exp_var); 								
					} 	

					fout << s << endl;
				}
			}
		}	
	}

	// Make a vcproject files.
	string Sources="\t\t\t<File\r\n\t\t\t\tRelativePath=\".\\Database_" + DBName + ".cpp\">\r\n\t\t\t</File>\r\n";

	for (it=p_listTableInfo->begin();it<p_listTableInfo->end();++it)
	{
		TableInfo *pTableInfo = (*it);
		Sources+="\t\t\t<File\r\n\t\t\t\tRelativePath=\".\\Table_" + pTableInfo->get_table_name() + ".cpp\">\r\n\t\t\t</File>\r\n";
	}

	StringUtils::Replace(InputPath + "database.vcproj.template",OutputPath + DBName + ".vcproj","@sources@",Sources);
	StringUtils::Replace(OutputPath + DBName + ".vcproj",OutputPath + DBName + ".vcproj","*DBName*",DBName);

	//preparing header

	string db_h = OutputPath + "Database_" + DBName + ".h";
	string makefile = OutputPath + "Makefile";

	ofstream db_h_out(db_h.c_str());
	ofstream makefile_out(makefile.c_str());

	makefile_out << "ROOT = .." << endl;
	makefile_out << "CXXFLAGS = -Wall -D_GNU_SOURCE" << endl;
	makefile_out << "CPPFLAGS = -I/../.. -I/.. -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL" << endl;
	makefile_out << "CXX = g++" << endl;
	makefile_out << "LDFLAGS = -L$(ROOT)/lib" << endl;
	makefile_out << "LDLIBS =  -lmysqlclient_r" << endl << endl;

	makefile_out << "APPNAME = pluto_main" << endl;
	makefile_out << "VERSION = 2.0.0" << endl; // this should be changed to a variable
	
	makefile_out << "sources = Database_" << DBName << ".cpp ";

	db_h_out << "#ifndef __Database_"<<DBName<<"_H_" << endl;
	db_h_out << "#define __Database_"<<DBName<<"_H_" << endl;
	db_h_out << "#include <mysql.h>" << endl;

	db_h_out << "#ifdef SQL2CPP_DLLEXPORT" << endl << "#define DLL_EXPORT __declspec(dllexport)" << endl << "#else" << endl << "#define DLL_EXPORT" << endl << "#endif" << endl;

	db_h_out << "class DLL_EXPORT Database_" << DBName << endl;
	db_h_out << "{" << endl;
	db_h_out << "public:" << endl;
	db_h_out << "MYSQL *db_handle;" <<  endl;
	db_h_out << "Database_" << DBName << "();" << endl;
	db_h_out << "~Database_" << DBName << "();" << endl;
	db_h_out << "void DeleteAllTables();" << endl;


	db_h_out << "private:" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	{
		db_h_out << "class Table_" + (*i)->get_table_name() + "* tbl"+(*i)->get_table_name()+";" << endl;
		makefile_out << " \\" << endl << "Table_" + (*i)->get_table_name() + ".cpp";
	}

	makefile_out << endl << endl << "all: lib" << DBName << ".a all-recursive" << endl << endl;
	makefile_out << "lib" << DBName << ".a: $(sources:.cpp=.o)" << endl;
	makefile_out << "\t$(AR) rc $@ $+" << endl;
	makefile_out << "\tcp $@ ../lib" << endl << endl;
	makefile_out << "clean: clean-recursive" << endl;
	makefile_out << "\trm -f *.o *.d lib" << DBName << ".a >/dev/null" << endl << endl;
	makefile_out << "package:" << endl;
	makefile_out << "\t./make_package.sh $(APPNAME) $(VERSION)" << endl << endl;
	makefile_out << "include $(ROOT)/DCE/make.rules" << endl;
	makefile_out << "-include $(sources:.cpp=.d)" << endl;

	db_h_out << "public:" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i<dbInfo.listTableInfo_get()->end(); i++)
	{
		db_h_out << "class Table_" + (*i)->get_table_name() + "* " + (*i)->get_table_name() + "_get() { return tbl"+(*i)->get_table_name()+"; }" << endl;
	}

	db_h_out << "bool Connect(string host, string user, string pass, string DBName, int port);" << endl;
	db_h_out << "void Disconnect();" << endl;

	db_h_out << "private:" << endl;

	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i<dbInfo.listTableInfo_get()->end(); i++)
		db_h_out << "void CreateTable_" + (*i)->get_table_name() + "();" << endl;

	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
		db_h_out << "void DeleteTable_" + (*i)->get_table_name() + "();" << endl;

	db_h_out << "};" << endl;

	db_h_out << "#endif" << endl;


	string db_cpp = OutputPath + "Database_" + DBName + ".cpp";
	ofstream db_cpp_out(db_cpp.c_str());

	db_cpp_out << "#ifdef WIN32" << endl << "\t#include <winsock.h>" << endl << "#endif" << endl;

	db_cpp_out << "#include <iostream>" << endl
		<< "#include <mysql.h>" << endl
		<< "#include <stdio.h>" << endl
		<< "#include <string>" << endl
		<< endl
		<< "#include <map>" << endl
		<< "#include <vector>" <<  endl
		<< endl
		<< "using namespace std;" << endl;


	/*	
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	db_cpp_out << "class Table_" + (*i)->get_table_name() + ";" << endl;
	*/

	db_cpp_out << endl << "#include \"Database_" + DBName + ".h\"" << endl << endl;

	/*	
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	db_cpp_out << "#include \"Table_" + (*i)->get_table_name() + ".h\"" << endl;
	*/

	db_cpp_out << "Database_" << DBName << "::Database_" << DBName << "()" << endl;
	db_cpp_out << "{" << endl;

	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
		db_cpp_out << "CreateTable_"+(*i)->get_table_name()+"();" << endl;

	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "Database_" << DBName << "::~Database_" << DBName << "()" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "\tDeleteAllTables();" << endl;
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "void Database_" << DBName << "::DeleteAllTables()" << endl;
	db_cpp_out << "{" << endl;
	for (vector<TableInfo_Generator *>::iterator i=dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
		db_cpp_out << "DeleteTable_"+(*i)->get_table_name()+"();" << endl;
	db_cpp_out << "}" << endl << endl;

	db_cpp_out << "bool Database_" << DBName << "::Connect(string host, string user, string pass, string DBName, int port)" << endl;
	db_cpp_out << "{" << endl;
	db_cpp_out << "db_handle = mysql_init(NULL);" << endl;

	db_cpp_out << "if (mysql_real_connect(db_handle, host.c_str(), user.c_str(), pass.c_str(), DBName.c_str(), port, NULL, 0) == NULL)" << endl;
	db_cpp_out << "{return false;}" << endl;
	db_cpp_out << "else" << endl;
	db_cpp_out << "{return true;}" << endl;

	db_cpp_out << "}" << endl << endl;

	string tbrow_h = OutputPath + "TableRow.h";
	ofstream tbrow_h_out(tbrow_h.c_str());
	tbrow_h_out << "#ifndef __TableRow_H_\n#define __TableRow_H_\n\nclass TableRow\n{\n};\n\n#endif\n" ;

	/*
	for (vector<TableInfo_Generator *>::iterator i = dbInfo.listTableInfo_get()->begin(); i!=dbInfo.listTableInfo_get()->end(); i++)
	cout << "/*" << (*i)->generateTableClassH() << "" << endl << endl; */

	cout << "Classes generated successfully!" << endl;

	return EXIT_SUCCESS;
}
