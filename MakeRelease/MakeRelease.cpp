#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_RepositorySource.h"
#include "pluto_main/Table_RepositoryType.h"
#include "pluto_main/Table_RepositorySource_URL.h"
#include "pluto_main/Table_Installation_RepositorySource_URL.h"
#include "pluto_main/Table_Package.h"
#include "pluto_main/Table_Package_Compat.h"
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Table_Version.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_Directory.h"
#include "pluto_main/Table_Package_Source_Compat.h"
#include "pluto_main/Table_Package_Source.h"
#include "pluto_main/Table_Package_Package.h"
#include "pluto_main/Table_Package_Directory.h"
#include "pluto_main/Table_Package_Directory_File.h"

#define MakeRelease_VERSION "0.1"

using namespace std;
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

// When we need to move files, we will use a list of this structure
class FileInfo
{
public:
	FileInfo(string sSource,string sDestination,Row_Package_Directory *pRow_Package_Directory)
	{
		m_sSource=sSource;
		m_sDestination=sDestination;
		m_pRow_Package_Directory=pRow_Package_Directory;
	}

	string m_sSource;
	string m_sDestination;
	Row_Package_Directory *m_pRow_Package_Directory;
};

string g_sPackages, g_sManufacturer, g_sSourcecodePrefix, g_sNonSourcecodePrefix;
string g_sPK_RepositorySource;
int g_iPK_Distro=0;
bool g_bBuildSource = true, g_bCreatePackage = true, g_bInteractive = false, g_bSimulate = false;
Database_pluto_main *g_pDatabase_pluto_main;
Row_Version *g_pRow_Version;
Row_Distro *g_pRow_Distro;
DCEConfig dceConfig;

// Int is the package ID, bool is true/false if it's been built.  This will be prepopulated with all the packages and 'false'
map<int,bool> g_mapPackagesToBuild;  

// The source code needs to be compiled in a certain order.  So we call CompileSourceInOrder first
// which is recursive and will call itself for all it's dependencies first.  It will then call
// CompileSource to do the actual compilation.  g_mapPackagesToBuild ensures we don't build the 
// same source code twice.
bool CompileSourceInOrder(Row_Package *pRow_Package);
bool CompileSource(Row_Package *pRow_Package);

// This will create the repository sources that we specified on the command line
bool CreateSources(Row_Package *pRow_Package);
bool CreateSource(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);

// These are the specific output functions that need to be implemented and expanded upon
bool CreateSource_PlutoDebian(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);
bool CreateSource_FTPHTTP(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);

// This will figure out what files need to be moved into the output
bool GetSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo);
bool GetNonSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo);

// Create archives
bool TarFiles(string sArchiveFileName);
bool ZipFiles(string sArchiveFileName);

// Misc
bool PackageIsCompatible(Row_Package *pRow_Package);

fstream fstr_compile,fstr_make_release;

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger(stdout);

	bool bError=false;
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			dceConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'k':
			g_sPackages = argv[++optnum];
			break;
		case 'm':
			g_sManufacturer = argv[++optnum];
			break;
		case 'o':
			g_iPK_Distro = atoi(argv[++optnum]);
			break;
		case 'r':
			g_sPK_RepositorySource = argv[++optnum];
			break;
		case 'b':
			g_bBuildSource = false;
			break;
		case 'c':
			g_bCreatePackage = false;
			break;
		case 'i':
			g_bInteractive = true;
			break;
		case 's':
			g_sSourcecodePrefix = argv[++optnum];
			break;
		case 'n':
			g_sNonSourcecodePrefix = argv[++optnum];
			break;
		case 'S':
			g_bSimulate = true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "MakeRelease, v." << MakeRelease_VERSION << endl
			<< "Usage: MakeRelease [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-k Packages] [-m Manufacturers] [-o Distro]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "output path -- Where to put the output files.  Default is ../[database name]" << endl
			<< "input path  -- Where to find the template files.  Default is . then ../MakeRelease" << endl;

		exit(0);
	}

	string sWhere;

	if( g_sPackages.length() )
		sWhere = "PK_Package IN (" + g_sPackages + ")";

	if( g_sManufacturer.length() )
	{
		if( sWhere.length() )
			sWhere += " AND ";
		sWhere += "FK_Manufacturer IN (" + g_sManufacturer + ")";
	}

	if( g_sSourcecodePrefix.length() && g_sSourcecodePrefix[ g_sSourcecodePrefix.length()-1 ]!='/' )
		g_sSourcecodePrefix += "/";
	if( g_sNonSourcecodePrefix.length() && g_sNonSourcecodePrefix[ g_sNonSourcecodePrefix.length()-1 ]!='/' )
		g_sNonSourcecodePrefix += "/";

	fstr_compile.open("Compile.script",fstream::out);  // A log of all the commands we executed to do the compile
	fstr_make_release.open("MakeRelease.script",fstream::out);  // A log of all the commands we executed to make the release

	g_pDatabase_pluto_main = new Database_pluto_main();
	if(!g_pDatabase_pluto_main->Connect(&dceConfig))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return 0;
	}

	// Get a list of all the packages that we are going to build based on the package and manufacturer data passed in
	vector<Row_Package *> vectPackages_Main;
	g_pDatabase_pluto_main->Package_get()->GetRows( sWhere.length()>0 ? sWhere : "1=1", &vectPackages_Main );
	cout << "Found the following " << vectPackages_Main.size() << " packages:" << endl;
	for(size_t s=0;s<vectPackages_Main.size();++s)
	{
		g_mapPackagesToBuild[vectPackages_Main[s]->PK_Package_get()]=false;
		cout << vectPackages_Main[s]->Description_get() << " ";
//		if( (s+1) % 3==0 )
			cout << endl;
	}

	if( !AskYNQuestion("Continue?",false) )
	{
		return 1;
	}

	g_pRow_Distro = g_pDatabase_pluto_main->Distro_get()->GetRow(g_iPK_Distro);
	if( !g_pRow_Distro )
	{
		cout << "Cannot find that distro" << endl;
		return 1;
	}

	cout << "What is the PK_Version?";
	string sPK_Version = StringUtils::GetStringFromConsole();
	g_pRow_Version = g_pDatabase_pluto_main->Version_get()->GetRow( atoi(sPK_Version.c_str()) );
	if( !g_pRow_Version )
	{
		cout << "Cannot find that version" << endl;
		return 1;
	}
#pragma warning("had to use tiny text because medium text fails due to char[] in commit with field asSQL");
	
	cout << "Building version: " << g_pRow_Version->Description_get() << endl;  

	cout << "********************************************************************" << endl
		<< "COMPILATION PHASE" << endl
		<< "********************************************************************" << endl;
	if( g_bBuildSource )
	{
		for(size_t s=0;s<vectPackages_Main.size();++s)
		{
			Row_Package *pRow_Package = vectPackages_Main[s];
			if( !PackageIsCompatible(pRow_Package) )
			{
				cout << "Skipping: " << pRow_Package->Description_get() << " because it is not compatible" << endl;
				continue;
			}

			if( g_bInteractive && pRow_Package->FK_Package_Sourcecode_isNull() && pRow_Package->IsSource_get()==0 )
			{
				cout << "There is no source code for package: " << pRow_Package->Description_get() << endl;
				if( !AskYNQuestion("Continue without building it?",false)  )
					return 1;
			}
			if( !pRow_Package->FK_Package_Sourcecode_isNull() )
			{
				if( !CompileSourceInOrder(pRow_Package) )
				{
					cout << "Aborting!" << endl;
					return 1;
				}
			}
		}
	}

	cout << "********************************************************************" << endl
		<< "PACKAGE CREATION PHASE" << endl
		<< "********************************************************************" << endl;

	if( g_bCreatePackage )
	{
		for(size_t s=0;s<vectPackages_Main.size();++s)
		{
			Row_Package *pRow_Package = vectPackages_Main[s];
			if( !CreateSources(pRow_Package) )
			{
				cout << "Aborting!" << endl;
				return 1;
			}
		}
	}

	cout << "Done!" << endl;
}

bool PackageIsCompatible(Row_Package *pRow_Package)
{
	vector<Row_Package_Compat *> vectRow_Package_Compat;
	pRow_Package->Table_Package_get()->Database_pluto_main_get()->Package_Compat_get()->GetRows(
		"(FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) +") AND " +
		"(FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + ") AND "+
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()),
		&vectRow_Package_Compat);
	return vectRow_Package_Compat.size()!=0;
	
}

bool CreateSources(Row_Package *pRow_Package)
{
	cout << "====================================================" << endl;
	cout << "Creating sources for : " << pRow_Package->Description_get() << endl;
	cout << "====================================================" << endl;

	vector<Row_Package_Source *> vectRow_Package_Source_All;
	string::size_type pos=0;
	string sPK_RepositorySource;
	while( (sPK_RepositorySource=StringUtils::Tokenize(g_sPK_RepositorySource,",",pos)).length() )
	{
		vector<Row_Package_Source *> vectRow_Package_Source;
		g_pDatabase_pluto_main->Package_Source_get()->GetRows( 
			"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND " +
			"FK_RepositorySource=" + sPK_RepositorySource,&vectRow_Package_Source);
		if( vectRow_Package_Source.size()==0 )
		{
			cout << "There is no info for this package and repository source " << sPK_RepositorySource << endl;
			if( g_bInteractive && !AskYNQuestion("Is this okay?",false) )
				return false;
			continue;
		}

		for(size_t s=0;s<vectRow_Package_Source.size();++s)
		{
			Row_Package_Source *pRow_Package_Source = vectRow_Package_Source[s];
			vectRow_Package_Source_All.push_back(pRow_Package_Source);
		}
	}

	cout << "Sources:" << endl;
	for(size_t s=0;s<vectRow_Package_Source_All.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source_All[s];
		cout << pRow_Package_Source->FK_RepositorySource_getrow()->Description_get() << ":" << pRow_Package_Source->Repository_get();
		if( (s+1) % 3==0 )
			cout << endl;
	}
	cout << endl;

	// Now go through and figure out what files we need to move over.  The process is different if this is a source package versus a binary package
	list<FileInfo *> listFileInfo;
	if( pRow_Package->IsSource_get() && !GetSourceFilesToMove(pRow_Package,listFileInfo) )
		return false;
	else if( !GetNonSourceFilesToMove(pRow_Package,listFileInfo) )
		return false;

	if( listFileInfo.size()==0 )
	{
		cout << "***ERROR*** No files found for this package!" << endl;
		return false;
	}

	// Now go through and figure out what executible files we need to move over.
	if( g_bInteractive )
	{
		if( AskYNQuestion("View the list of files?",false) )
		{
			for(list<FileInfo *>::iterator it=listFileInfo.begin();it!=listFileInfo.end();++it)
			{
				FileInfo *pFileInfo = *it;
				cout << pFileInfo->m_sSource << "  -->  " << pFileInfo->m_sDestination << endl;
			}

		if( !AskYNQuestion("Continue?",false) )
			return false;
		}
	}

	for(size_t s=0;s<vectRow_Package_Source_All.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source_All[s];
		if( !CreateSource(pRow_Package_Source,listFileInfo) )
		{
			cout << "***ABORTING***" << endl;
			return false;
		}
	}
	return true;
}

bool CreateSource(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	cout << "\tCreating source: " << pRow_Package_Source->FK_RepositorySource_getrow()->Description_get() << endl;
	cout << "\t--------------------------" << endl;

	if( pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_FTP_download_CONST ||
		pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_HTTP_Download_CONST ||
		pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_FTP_or_HTTP_Download_CONST )
	{
		CreateSource_FTPHTTP(pRow_Package_Source,listFileInfo);
	}
	else
	{
		switch(pRow_Package_Source->FK_RepositorySource_get())
		{
		case REPOSITORYSOURCE_Pluto_Debian_CONST:
			CreateSource_PlutoDebian(pRow_Package_Source,listFileInfo);
			break;
		default:
			cout << "**Error** Don't know how to create this source." << endl;
			return false;
		}
	}

	// Update the version record
	pRow_Package_Source->Version_set(g_pRow_Version->VersionName_get());
	pRow_Package_Source->Table_Package_Source_get()->Commit();

	return true;
}

bool GetSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo)
{
	// Start with source code.
	// First is there a separate directory specified for the source headers?
	Row_Package_Directory *pRow_Package_Directory_SourceIncludes=NULL;
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Source_Includes_CONST),
		&vectRow_Package_Directory);
	if( vectRow_Package_Directory.size() )
		pRow_Package_Directory_SourceIncludes = vectRow_Package_Directory[0];

	// What are the source code implementation files for this?
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Source_Implementation_CONST),
		&vectRow_Package_Directory);
	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		string sDirectory;
		if( pRow_Package_Directory->Path_get()[0]=='/' )
			sDirectory = pRow_Package_Directory->Path_get();
		else
			sDirectory += g_sSourcecodePrefix + "/" + pRow_Package_Directory->Path_get();
#pragma warning("Need something other than this");

		vector<Row_Package_Directory_File *> vectPackage_Directory_File;
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&vectPackage_Directory_File);
		for(size_t s2=0;s2<vectPackage_Directory_File.size();++s2)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectPackage_Directory_File[s2];
			if( !pRow_Package_Directory_File->MakeCommand_isNull() )
				continue; // Don't add this to the list of source code.  This is something we make

			FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + pRow_Package_Directory_File->File_get(),pRow_Package_Directory->Path_get() + "/" + pRow_Package_Directory_File->File_get(),pRow_Package_Directory_File->FK_Package_Directory_getrow());
			listFileInfo.push_back(pFileInfo);
		}

		list<string> listFiles;
		FileUtils::FindFiles(listFiles,sDirectory,"*.cpp,*.c,Makefile*");
		for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
		{
			FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + *it,pRow_Package_Directory->Path_get() + "/" + *it,pRow_Package_Directory);
			listFileInfo.push_back(pFileInfo);
		}

		listFiles.clear();
		FileUtils::FindFiles(listFiles,sDirectory,"*.h");
		for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
		{
			FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + *it,
				(pRow_Package_Directory_SourceIncludes ? pRow_Package_Directory_SourceIncludes->Path_get() : pRow_Package_Directory->Path_get()) + "/" + *it,
				pRow_Package_Directory);
			listFileInfo.push_back(pFileInfo);
		}
	}
	return true;
}

bool GetNonSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo)
{
	// First is there a directory specified for finding the compiled output?
	Row_Package_Directory *pRow_Package_Directory_CompiledOutput=NULL;
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),
		&vectRow_Package_Directory);
	if( vectRow_Package_Directory.size() )
		pRow_Package_Directory_CompiledOutput = vectRow_Package_Directory[0];

	// What are the files for this?
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND (FK_Directory=" + StringUtils::itos(DIRECTORY_Binary_Executables_CONST) + 
		" OR FK_Directory=" + StringUtils::itos(DIRECTORY_Binary_Library_CONST) + " OR FK_Directory=" + StringUtils::itos(DIRECTORY_Configuration_CONST) + 
		" OR FK_Directory=" + StringUtils::itos(DIRECTORY_Miscellaneous_Files_CONST) + ")",&vectRow_Package_Directory);

	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( (!pRow_Package_Directory->FK_Distro_isNull() && pRow_Package_Directory->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
				(!pRow_Package_Directory->FK_OperatingSystem_isNull() && pRow_Package_Directory->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
			continue;
		string sDirectory;

		// If there's a compiled output directory, use that
		if( pRow_Package_Directory_CompiledOutput && 
			(pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Binary_Executables_CONST ||
			pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Binary_Library_CONST) )
		{
			sDirectory = g_sSourcecodePrefix + pRow_Package_Directory_CompiledOutput->Path_get();
		}
		else
		{
			sDirectory = g_sNonSourcecodePrefix + pRow_Package_Directory->Path_get();
		}


		if( sDirectory.length()==0 )
		{
			if( !AskYNQuestion("**WARNING** Directory is empty.  Continue?",false) )
				return false;

			sDirectory = g_sNonSourcecodePrefix;
		}
//		else if( sDirectory[0]!='/' && sDirectory.substr(1,2) != ":\\" && sDirectory.substr(1,2) != ":/")
// put the prefix above			sDirectory =  + sDirectory;
	
		vector<Row_Package_Directory_File *> vectPackage_Directory_File;
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&vectPackage_Directory_File);
		for(size_t s2=0;s2<vectPackage_Directory_File.size();++s2)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectPackage_Directory_File[s2];
			if( (!pRow_Package_Directory_File->FK_Distro_isNull() && pRow_Package_Directory_File->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
					(!pRow_Package_Directory_File->FK_OperatingSystem_isNull() && pRow_Package_Directory_File->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
				continue;

	cout << "Checking file: " << pRow_Package_Directory_File->PK_Package_Directory_File_get() << " " << pRow_Package_Directory_File->File_get() << endl;
	cout << "Distro: " << g_pRow_Distro->PK_Distro_get() << endl;
   cout << "x:" << pRow_Package_Directory_File->FK_Distro_get() << " " << pRow_Package_Directory_File->FK_Distro_isNull() << endl;

			string File = pRow_Package_Directory_File->File_get();
			if( File.find('*')!=string::npos || File.find('?')!=string::npos )
			{
				list<string> listFiles;
				FileUtils::FindFiles(listFiles,sDirectory,File,true);
				if( listFiles.size()==0 )
				{
					cout << "**WARNING** No files found in " << sDirectory << endl;
					if( !AskYNQuestion("Continue?",false) )
						return false;
				}
				for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
				{
					FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + *it,
						pRow_Package_Directory->Path_get() + "/" + *it,
						pRow_Package_Directory);
					listFileInfo.push_back(pFileInfo);
				}
			}
			else
			{
				if( !FileUtils::FileExists(sDirectory + "/" + File) )
				{
					cout << "**WARNING** " << sDirectory << "/" << File << " not found" <<  endl;
					if( !AskYNQuestion("Continue?",false) )
						return false;
				}
				FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + File,
					pRow_Package_Directory->Path_get() + "/" + File,
					pRow_Package_Directory);
				listFileInfo.push_back(pFileInfo);
			}
		}
	}
	return true;
}

bool CompileSourceInOrder(Row_Package *pRow_Package)
{
	if( g_mapPackagesToBuild.find(pRow_Package->PK_Package_get())==g_mapPackagesToBuild.end() )
		return true; // It's okay--this isn't one of the packages in our list anyway

	// Get all the dependencies of this package and build them first
	vector<Row_Package_Package *> vectRow_Package_Package;
	pRow_Package->Package_Package_FK_Package_getrows(&vectRow_Package_Package);
	for(size_t s=0;s<vectRow_Package_Package.size();++s)
	{
		Row_Package_Package *pRow_Package_Package = vectRow_Package_Package[s];
		CompileSourceInOrder(pRow_Package_Package->FK_Package_DependsOn_getrow());
	}

	if( !pRow_Package->FK_Package_Sourcecode_isNull() && !CompileSourceInOrder(pRow_Package->FK_Package_Sourcecode_getrow()) )
	{
		cout << "BuildSource failed for: " << pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get() << " " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << endl;
		return false;
	}

	// We know this package is in the map.  If it was built already, we don't need to do it again
	if( g_mapPackagesToBuild[pRow_Package->PK_Package_get()]==true )
		return true;

	if( !CompileSource(pRow_Package) )
	{
		cout << "BuildSource failed for: " << pRow_Package->PK_Package_get() << " " << pRow_Package->Description_get() << endl;
		return false;
	}
	return true;
}

bool CompileSource(Row_Package *pRow_Package)
{
	if( pRow_Package->FK_Package_Sourcecode_isNull() )
		return true; // It's okay.  Nothing to do here

	if( g_mapPackagesToBuild.find(pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get())==g_mapPackagesToBuild.end() )
	{
		cout << "***ERROR***: The package: " << pRow_Package->Description_get() << " needs to be built" << endl 
			<< "But it's source code: " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << " isn't in the include list!" << endl;
		return false;
	}

	if( pRow_Package->FK_Package_Sourcecode_getrow()->IsSource_get()==0 )
	{
		cout << "***ERROR***: The package: " << pRow_Package->Description_get() << " indicated its source is in" << endl 
			<< pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << " but that isn't marked as a source package" << endl;
		return false;
	}

	cout << "====================================================" << endl;
	cout << "Compiling source package: " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << endl;
	cout << "====================================================" << endl;

	vector<Row_Package_Source *> vectRow_Package_Source;
	g_pDatabase_pluto_main->Package_Source_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) + " AND FK_RepositorySource=" + StringUtils::itos(REPOSITORYSOURCE_Pluto_SVN_CONST),
		&vectRow_Package_Source);
	if( vectRow_Package_Source.size()>1 )
	{
		cout << "Package: " << pRow_Package->Description_get() << " is in SVN more than once!" << endl;
		return false;
	}

	// Find out where this is in our SVN repository
	Row_Package_Source *pRow_Package_Source_SVN=NULL;
	if( vectRow_Package_Source.size()==0 )
	{
		cout << "Package: " << pRow_Package->Description_get() << " does not have an entry in Package_Source table for SVN." << endl;
		if( AskYNQuestion("Create one?",true) )
		{
			pRow_Package_Source_SVN = g_pDatabase_pluto_main->Package_Source_get()->AddRow();
			pRow_Package_Source_SVN->FK_RepositorySource_set(REPOSITORYSOURCE_Pluto_SVN_CONST);
			pRow_Package_Source_SVN->FK_Package_set(pRow_Package->FK_Package_Sourcecode_get());
AsksSourceQuests:
			cout << "What is the name?";
			string s = StringUtils::GetStringFromConsole();
			pRow_Package_Source_SVN->Name_set(s);

			cout << "What is the repository?"; 
			s = StringUtils::GetStringFromConsole();
			pRow_Package_Source_SVN->Repository_set(s);

			cout << "Are there any parms?";
			s = StringUtils::GetStringFromConsole();

			pRow_Package_Source_SVN->Parms_set(s);

			char c = AskMCQuestion("Is this okay? (Yes/No/Abort)","yna");
			if( c=='a' )
			{
				return false;
			}
			if( c=='n' )
				goto AsksSourceQuests;
		}
		else
		{
			return true;  // Not necessarily a critical error.  Keep going
		}

		pRow_Package_Source_SVN->Version_set(g_pRow_Version->VersionName_get());
		g_pDatabase_pluto_main->Package_Source_get()->Commit();
	}
	else
		pRow_Package_Source_SVN = vectRow_Package_Source[0];

	// What directories contain the source code?
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Source_Implementation_CONST),
		&vectRow_Package_Directory);
	if( vectRow_Package_Directory.size()==0 )
	{
		cout << "There is no directory specified for the source code implementation." << endl;
		char c = AskMCQuestion("Specify one now? (Yes/No/Abort)","yna");
		if( c=='a' )
		{
			return false;
		}
		if( c=='y' )
		{
			Row_Package_Directory *pRow_Package_Directory = g_pDatabase_pluto_main->Package_Directory_get()->AddRow();
			pRow_Package_Directory->FK_Package_set(pRow_Package->FK_Package_Sourcecode_get());
			pRow_Package_Directory->FK_Directory_set(DIRECTORY_Source_Implementation_CONST);
			pRow_Package_Directory->FK_Distro_set(g_iPK_Distro);

			cout << "What is the name?"; 
			string s = StringUtils::GetStringFromConsole();
			pRow_Package_Directory->Path_set(s);
			g_pDatabase_pluto_main->Package_Directory_get()->Commit();
			vectRow_Package_Directory.push_back(pRow_Package_Directory);
		}
	}

	// What directory contains the compiled output?
	vector<Row_Package_Directory *> vectRow_Package_Directory_CO;
	Row_Package_Directory *pRow_Package_Directory_CompiledOutput=NULL;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),
		&vectRow_Package_Directory_CO);
	if( vectRow_Package_Directory_CO.size()==0 )
	{
		cout << "There is no compiled output directory specified for this package." << endl;
		char c = AskMCQuestion("Specify one now? (Yes/No/Abort)","yna");
		if( c=='a' )
		{
			return false;
		}
		if( c=='y' )
		{
			pRow_Package_Directory_CompiledOutput = g_pDatabase_pluto_main->Package_Directory_get()->AddRow();
			pRow_Package_Directory_CompiledOutput->FK_Package_set(pRow_Package->FK_Package_Sourcecode_get());
			pRow_Package_Directory_CompiledOutput->FK_Directory_set(DIRECTORY_Compiled_Output_CONST);
			pRow_Package_Directory_CompiledOutput->FK_Distro_set(g_iPK_Distro);

			cout << "What is the name?";
			string s = StringUtils::GetStringFromConsole();
			pRow_Package_Directory_CompiledOutput->Path_set(s);
			pRow_Package_Directory_CompiledOutput->Table_Package_Directory_get()->Commit();
		}
	}
	else
		pRow_Package_Directory_CompiledOutput = vectRow_Package_Directory_CO[0];

	string sCompiledOutput;
	if( pRow_Package_Directory_CompiledOutput->Path_get()[0]=='/' )
		sCompiledOutput = pRow_Package_Directory_CompiledOutput->Path_get();
	else
		sCompiledOutput += g_sSourcecodePrefix + "/" + pRow_Package_Directory_CompiledOutput->Path_get();

	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		cout << "\tChecking directory: " << pRow_Package_Directory->Path_get() << endl;
		cout << "\t------------------------" << endl;

		Row_Package_Directory *pRow_Package_Directory_Binary = NULL;

		// Where are we going to output?
		vector<Row_Package_Directory *> vectRow_Package_Directory;
		g_pDatabase_pluto_main->Package_Directory_get()->GetRows(
				"FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),
				&vectRow_Package_Directory);

		if( vectRow_Package_Directory.size() )
			pRow_Package_Directory_Binary = vectRow_Package_Directory[0];
		else
		{
			cout << "This package has no output directory specified." << endl;
			string sOutput = StringUtils::GetStringFromConsole();
			if( sOutput.length()==0 )
			{
				cout << "***ABORTING***  You didn't specify an output directory" << endl;
				return false;
			}

			pRow_Package_Directory_Binary = g_pDatabase_pluto_main->Package_Directory_get()->AddRow();
			pRow_Package_Directory_Binary->FK_Package_set(pRow_Package->PK_Package_get());
			pRow_Package_Directory_Binary->FK_Directory_set(DIRECTORY_Compiled_Output_CONST);
			pRow_Package_Directory_Binary->FK_Distro_set(g_iPK_Distro);
			pRow_Package_Directory_Binary->Path_set(sOutput);
			g_pDatabase_pluto_main->Package_Directory_get()->Commit();
		}

		// We now the source code is in pRow_Package_Directory and the binary goes in pRow_Package_Directory_Binary
		if( pRow_Package_Directory->Path_get().length()==0 )
		{
			cout << "Error: The directory for PackageDirectory: " << pRow_Package_Directory->PK_Package_Directory_get() << " is empty" << endl;
			return false;
		}

		// If the directory starts with a /, it is considered absolute.  Otherwise it's relative
		string sSourceDirectory = g_sSourcecodePrefix;
		if( pRow_Package_Directory->Path_get()[0]=='/' )
			sSourceDirectory = pRow_Package_Directory->Path_get();
		else
			sSourceDirectory += "/" + pRow_Package_Directory->Path_get();

		if( !FileUtils::DirExists(sSourceDirectory) )
		{
			cout << "Error: The directory " << sSourceDirectory << " does not exist" << endl;
			return false;
		}

		if( !g_bSimulate )
		{
			chdir(sSourceDirectory.c_str());
		}
		fstr_compile << "cd " << sSourceDirectory << endl;

		// Find the make command for pRow_Package_Source_SVN in the _Compat table.  First look for a match on this distro
		Row_Package_Source_Compat *pRow_Package_Source_Compat=NULL;

		vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat;
		g_pDatabase_pluto_main->Package_Source_Compat_get()->GetRows(
				"FK_Package_Source=" + StringUtils::itos(pRow_Package_Source_SVN->PK_Package_Source_get()) + 
				" AND (FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) + ")" +
				"AND (FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + ")",&vectRow_Package_Source_Compat);
		if( vectRow_Package_Source_Compat.size()==0 )
		{
			if( !g_bInteractive )
			{
				cout << "***Source code is not compatible with this distro***" << endl;
				return true; // It's okay
			}

			cout << "I can't find a corresponding row in Package_Source_Compat." << endl
				<< "This is normal if this package cannot be built with this distro." << endl;
				
			if( !AskYNQuestion("Add a record Package_Source_Compat and try to build it?",false) )
				return true;
			pRow_Package_Source_Compat = g_pDatabase_pluto_main->Package_Source_Compat_get()->AddRow();
			pRow_Package_Source_Compat->FK_Package_Source_set(pRow_Package_Source_SVN->PK_Package_Source_get());
			pRow_Package_Source_Compat->FK_Distro_set( g_pRow_Distro->PK_Distro_get() );
			g_pDatabase_pluto_main->Package_Source_Compat_get()->Commit();
		}
		else
			pRow_Package_Source_Compat = vectRow_Package_Source_Compat[0];

		// See what files this should be outputing
		vector<Row_Package_Directory_File *> vectRow_Package_Directory_File;
		g_pDatabase_pluto_main->Package_Directory_File_get()->GetRows(
			"FK_Package_Directory=" + StringUtils::itos(pRow_Package_Directory->PK_Package_Directory_get()) +
			" AND (FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) +") " +
			" AND (FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) +")",
			&vectRow_Package_Directory_File);

		if( vectRow_Package_Directory_File.size()==0 )
		{
			if( !g_bInteractive )
			{
				cout << "Nothing to build in this directory" << endl;
				continue; // We just assume there is nothing to build here
			}

			cout << "No output file is specified.  You need one to compile this directory." << endl;
			if( !AskYNQuestion("Specify one?",false) )
				continue;

			cout << "What is the file?" << endl;
			string s = StringUtils::GetStringFromConsole();
			Row_Package_Directory_File *pRow_Package_Directory_File = g_pDatabase_pluto_main->Package_Directory_File_get()->AddRow();
			pRow_Package_Directory_File->FK_Package_Directory_set(pRow_Package_Directory->PK_Package_Directory_get());
			pRow_Package_Directory_File->FK_Distro_set(g_pRow_Distro->PK_Distro_get());
			pRow_Package_Directory_File->File_set(s);
			g_pDatabase_pluto_main->Package_Directory_File_get()->Commit();
			vectRow_Package_Directory_File.push_back(pRow_Package_Directory_File);
		}

		// First remove any output files that we should be building from source
		for(size_t s=0;s<vectRow_Package_Directory_File.size();++s)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectRow_Package_Directory_File[s];
			remove(pRow_Package_Directory_File->File_get().c_str());
		}

		// The make command succeeded.  Now the binary files should be here
		for(size_t s=0;s<vectRow_Package_Directory_File.size();++s)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectRow_Package_Directory_File[s];

			if( g_bInteractive )
			{
				cout << "About to execute: " << pRow_Package_Directory_File->MakeCommand_get() << endl
					<< "In directory: " << sSourceDirectory << endl;
				if( !AskYNQuestion("Execute command?",false) )
					return false;
			}
			fstr_compile << pRow_Package_Directory_File->MakeCommand_get() << endl;
			if( !g_bSimulate && system(pRow_Package_Directory_File->MakeCommand_get().c_str()) )
			{
				cout << pRow_Package_Directory_File->MakeCommand_get() << " ****FAILED****" << endl;
				cout << "Error: " << pRow_Package_Directory_File->MakeCommand_get() << " failed!" << endl;
				return false;
			}
			cout << pRow_Package_Directory_File->MakeCommand_get() << " succeeded" << endl;

			if( !g_bSimulate && !FileUtils::FileExists(sCompiledOutput + "/" + pRow_Package_Directory_File->File_get()) ) 
			{
				cout << "**ERROR** The file: " << sCompiledOutput << "/" << pRow_Package_Directory_File->File_get() << " was not created.";
 				return false;
			}
			cout << sCompiledOutput << "/" << pRow_Package_Directory_File->File_get() << " exists" << endl;
		}

		// We're done with this directory!!!
	}
	g_mapPackagesToBuild[pRow_Package->PK_Package_get()]=true;
	g_mapPackagesToBuild[pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get()]=true;
	return true;
}

/* CreateSource_Pluto<type> Create specified type of package using given files */

// list<FileInfo *> listFileInfo: files that go into the package
// assumptions for the Pluto Debian repository:
// - the .deb file will go in dists/<pluto2 (repository name - var in package source)>/
//   intended to be run from a directory with this dists directory under it
// - the target destination will be /usr/pluto/<relative path>

// listFileInfo is input not output!!! (we should get used to using const when a reference is for input)

bool CreateSource_PlutoDebian(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	string Version("");
	if (!pRow_Package_Source->Version_isNull())
	{
		Version = pRow_Package_Source->Version_get();
	}
	if (Version == "")
	{
		Version = "2.0.0.0";
	}

/*	string VCopy(Version);
	string sV[4];
	string::size_type pos = 0;
	for (int i = 0; i < 4; i++)
	{
		sV[i] = StringUtils::Tokenize(VCopy, ".", pos);
	}
	int x;
	sscanf(sV[3].c_str(), "%d", &x);
	Version = sV[0] + "." + sV[1] + "." + sV[2] + "." + StringUtils::itos(x + 1);
	pRow_Package_Source->Version_set(Version);
*/	
	string Package_Name = StringUtils::ToLower(pRow_Package_Source->Name_get());
	string Dir("/home/tmp/pluto-build/" + Package_Name + "-" + Version);
	string Prefix("/usr/");
	if (pRow_Package_Source->FK_Package_getrow()->IsSource_get())
	{
		Prefix = "/usr/pluto/src/";
	}

	FILE * f;

#ifndef WIN32
	system(("rm -rf " + Dir).c_str());
	system(("mkdir -p " + Dir + "/root").c_str());
	char CurrentDir[1024];
	getcwd(CurrentDir, 1024);
	chdir(Dir.c_str());

string Makefile = "none:\n"
"\t\n"
"\n"
"install:\n"
"\tcp -a root/* $(DESTDIR)\n";

	f = fopen((Dir + "/Makefile").c_str(), "w");
	if (!f)
	{
		cout << "Error: cannot open Makefile" << endl;
		return false;
	}
	fprintf(f, "%s", Makefile.c_str(), Makefile.length());
	fclose(f);
	system("echo | dh_make -c gpl -s -n");
//	mkdir("DEBIAN", 0666);
#endif

	list<FileInfo *>::iterator iFileInfo;
	for (iFileInfo = listFileInfo.begin(); iFileInfo != listFileInfo.end(); iFileInfo++)
	{
		string sSource = (*iFileInfo)->m_sSource;

		// TODO: Error checks
		if (sSource.find("mkr_postinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "POSTINST: " << sSource << endl;
			// put mkr_postinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/postinst.ex", "r");
			g = fopen("debian/postinst", "w");
			fchmod(fileno(g), 0644);
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "configure)") != NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");
					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
#endif
		}
		else if (sSource.find("mkr_preinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "PREINST: " << sSource << endl;
			// put mkr_preinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/preinst.ex", "r");
			g = fopen("debian/preinst", "w");
			fchmod(fileno(g), 0644);
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "install|upgrade)") != NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");
					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
#endif
		}
		else
		{
			// no prefix for absolute paths (the ones that start with a slash)
			string thePrefix(Prefix);
			if ((*iFileInfo)->m_sDestination[0] == '/')
				thePrefix = "";
		
			string sDestination = Dir + "/root/" + thePrefix + (*iFileInfo)->m_sDestination;
		
			cout << "COPY: " << sSource << " --> " + sDestination << endl;
			FileUtils::PUCopyFile(sSource, sDestination);
		}
	}

	//f = fopen((Dir + "/root/dummy-file").c_str(), "w");
	//if( !f )
	//{
	//	cout << "Error: cannot open dummy-file" << endl;
	//	return false;
	//}
	//fclose(f);

	// Get a list of all the other packages which we depend on, and which have Debian sources.  We are going to add them to the .deb as dependencies
	vector<Row_Package_Source *> vect_pRow_Package_Source_Dependencies;
	pRow_Package_Source->Table_Package_Source_get()->GetRows("JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package_Source.FK_Package AND Package_Package.FK_Package=" + 
		StringUtils::itos(pRow_Package_Source->FK_Package_get()) + " WHERE FK_RepositorySource=" + StringUtils::itos(REPOSITORYSOURCE_Pluto_Debian_CONST),
		&vect_pRow_Package_Source_Dependencies);

	string sDepends;

	for (size_t s=0;s<vect_pRow_Package_Source_Dependencies.size();++s)
	{
		Row_Package_Source *pRow_Package_Source_Dependency = vect_pRow_Package_Source_Dependencies[s];
		sDepends += ", " + pRow_Package_Source_Dependency->Name_get();
	}
	cout << "Depends list: " << sDepends << endl;

#ifndef WIN32
	system(("sed -i 's/^Depends:.*$/Depends: ${shlibs:Depends}, ${misc:Depends}" + sDepends + "/' " + Dir + "/debian/control").c_str());
	if( system("dpkg-buildpackage -b -rfakeroot") )
	{
		cout << "dpkg-buildpackage -b failed.  Aborting." << endl;
		return false;
	}
#endif
	
//	cout << "------------DEBIAN PACKAGE OUTPUT" << endl;
//	cout << " rep: " << pRow_Package_Source->Repository_get() << " ver: " << pRow_Package_Source->Version_get() << " parm: " << pRow_Package_Source->Parms_get() << endl;
//	cout << "Press any key to continue..." << endl;
//	char c = getch();
					
#ifndef WIN32
	chdir(CurrentDir);
	system(("rm -rf " + Dir).c_str());
#endif
	
	return true;
}

bool CreateSource_FTPHTTP(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	string ArchiveFileName = "/var/www/download/" + pRow_Package_Source->Repository_get() + "/";
	system(("mkdir -p " + ArchiveFileName).c_str());
	
	ArchiveFileName += pRow_Package_Source->Name_get() + "_" + pRow_Package_Source->Version_get() + pRow_Package_Source->Parms_get();
	cout << "Creating Archive: " << ArchiveFileName << endl;
	chdir("/home/tmp");
	
#ifdef WIN32
	string TempDir = mktemp("mra_XXXXXX");
#else
	char templ[12]="mra_XXXXXX";
	string TempDir = mktemp(templ);
	system(("mkdir -p /home/tmp/" + TempDir).c_str());
#endif
	unlink( ("/home/tmp/" + ArchiveFileName).c_str() );
	chdir(("/home/tmp/" + TempDir).c_str());
	
	for(list<FileInfo *>::iterator it=listFileInfo.begin();it!=listFileInfo.end();++it)
	{
		FileInfo *pFileInfo = *it;
		if( !FileUtils::PUCopyFile(pFileInfo->m_sSource,"/home/tmp/" + TempDir + "/" + pFileInfo->m_sDestination) )
		{
			cout << "**Error: Unable to copy file " <<  pFileInfo->m_sSource << " -> " << TempDir << "/" <<  pFileInfo->m_sDestination << endl;
			return false;
		}
	}
	// See what type it is
	if( pRow_Package_Source->Parms_get()==".tar.gz" )
	{
		if( !TarFiles(ArchiveFileName) )
		{
			cout << "**ERROR:** Tar " << ArchiveFileName << endl;
			return false;
		}
	}
	else if( pRow_Package_Source->Parms_get()==".zip" )
	{
		if( !ZipFiles(ArchiveFileName) )
		{
			cout << "**ERROR:** Zip " << ArchiveFileName << endl;
			return false;
		}
	}
	else
	{
		cout << "**ERROR:** Unknown archive type: " << pRow_Package_Source->Parms_get() << endl;
		return false;
	}
	chdir("/");
#ifndef WIN32
	system(("rm -rf /home/tmp/" + TempDir).c_str());
#endif
	
	return true;
}

bool TarFiles(string sArchiveFileName)
{
	string SystemCall = "tar -zcvf " + sArchiveFileName + " *";
	cout << "Executing: " << SystemCall << endl;
	return system(SystemCall.c_str())==0;
}

bool ZipFiles(string sArchiveFileName)
{
	string SystemCall = "zip -r " + sArchiveFileName + " *";
	cout << "executing: " << SystemCall << endl;
	return system(SystemCall.c_str())==0;
}
