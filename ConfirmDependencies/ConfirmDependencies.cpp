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
#include <conio.h>
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
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_Directory.h"
#include "pluto_main/Table_Package_Source_Compat.h"
#include "pluto_main/Table_Package_Source.h"
#include "pluto_main/Table_Package_Package.h"
#include "pluto_main/Table_Package_Directory.h"
#include "pluto_main/Table_Package_Directory_File.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

class PackageInfo
{
public:
	Row_Package_Source_Compat *m_pRow_Package_Source_Compat;
	Row_Package_Source *m_pRow_Package_Source;
	Row_RepositorySource_URL *m_pRow_RepositorySource_URL;
	bool m_bMustBuild;

	string m_sBinaryExecutiblesPathPath, m_sSourceIncludesPath, m_sSourceImplementationPath, m_sBinaryLibraryPath, m_sConfiguration;
	vector<Row_Package_Directory_File *> m_vectRow_Package_Directory_File_BinaryExecutibles, m_vectRow_Package_Directory_File_SourceIncludes,
		m_vectRow_Package_Directory_File_SourceImplementation,m_vectRow_Package_Directory_File_BinaryLibrary,
		m_vectRow_Package_Directory_File_Configuration;

	vector<PackageInfo *> m_vectPackageInfo; // Some alternates in case this one fails

	PackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,
		Row_Package_Source *pRow_Package_Source, Row_RepositorySource_URL *pRow_RepositorySource_URL,
		bool bMustBuild)
	{
		m_pRow_Package_Source_Compat=pRow_Package_Source_Compat;
		m_pRow_Package_Source=pRow_Package_Source;
		m_pRow_Package_Source=pRow_Package_Source;
		m_pRow_RepositorySource_URL=pRow_RepositorySource_URL;
		m_bMustBuild=bMustBuild;
	}
};

void CheckDevice(Row_Device *pRow_Device,bool bSourceCode);
void CheckDeviceLoop(Row_Device *pRow_Device,bool bDevelopment);
void CheckPackage(Row_Package *pRow_Package,Row_Device *pRow_Device,bool bDevelopment,Row_Distro *pRow_Distro,bool bMustBuildFromSource);
PackageInfo *MakePackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,bool bMustBuildFromSource);
void VerifyFiles(PackageInfo *pPackageInfo,vector<Row_Package_Directory_File *> &vectRow_Package_Directory_File,string Path);
Row_Package_Directory *GetDirectory(Row_Package *pRow_Package,int PK_Directory);
void InstallPackage(PackageInfo *pPackageInfo, bool bElse = false);
Row_Package_Source_Compat *FindPreferredSource(vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat,Row_Device *pRow_Device);
void AddAlternateSources(vector<Row_Package_Source_Compat *> &vectRow_Package_Source_Compat,PackageInfo *pPackageInfo);

string sCommand;
int iPK_Distro=-1;
Row_Installation *pRow_Installation;
bool bSummary=false;
list<PackageInfo *> listPackageInfo;  // We need a list so we can keep them in the right order
DCEConfig dceConfig;
Row_Distro *pRow_Distro=NULL;

string GetCommand()
{
	cout << "What would you like to do?" << endl
		<< "In the future you can also make your selections on the command line using the command shown in parenthesis." << endl
		<< "1.  View required packages for a device, grouped by child devices. (view)" << endl
		<< "2.  Status, shows the same information as view, but also physically scans the disk for the files to see whether the packages are installed. (status)" << endl
		<< "3.  Just list all the packages in a single list (list)" << endl
		<< "3.	Output an install script for the packages (install)" << endl
		<< "4.	Output a script that will build any packages that were not available as binaries. (build)" << endl
		<< "5.	Output a script that will build all packages, even if they were available as source. (buildall)" << endl
		<< endl << "Q.  Quit" << endl;

	char c=(char) getch();

	switch(c)
	{
	case '1':
		return "view";
	case '2':
		return "status";
	case '3':
		return "list";
	case '4':
		return "build";
	case '5':
		return "buildall";
	case 'Q':
	case 'q':
		exit(1);
	}
	return "";
}


int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger(stdout);

	bool bError=false,bIncludeDisklessMD=true,bSourceCode=false; // An error parsing the command line
	int iPK_Device = dceConfig.m_iPK_Device_Computer;
	char c;
	cout << "#";
	for(int optnum=0;optnum<argc;++optnum)
		cout << " " << argv[optnum];
	cout << endl;

	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			sCommand = argv[optnum];
			if( optnum<argc-1 )
			{
				cout << argv[optnum] << " isn't preceded by a -, but the command must be the last parameter" << endl;
				bError=true;	// The command must be the last thing on the line
			}
			break;
		}

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
		case 'd':
			iPK_Device = atoi(argv[++optnum]);
			break;
		case 'o':
			iPK_Distro = atoi(argv[++optnum]);
			break;
		case 'i':
			bIncludeDisklessMD = false;
			break;
		case 's':
			bSummary = true;
			break;
		case 'c':
			bSourceCode = true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "ConfirmDependencies, v." << VERSION << endl
			<< "Usage: ConfirmDependencies [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-r Repository(-ies)] [-t Table(s)] [-U Users(s)]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "output path -- Where to put the output files.  Default is ../[database name]" << endl
			<< "input path  -- Where to find the template files.  Default is . then ../ConfirmDependencies" << endl;

		exit(0);
	}

	while( sCommand.length()==0 )
		sCommand=GetCommand();

	sCommand=StringUtils::ToLower(sCommand);

	Database_pluto_main database_pluto_main;
	if(!database_pluto_main.Connect(&dceConfig))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return 0;
	}

	Row_Device *pRow_Device = database_pluto_main.Device_get()->GetRow(iPK_Device);
	if( !pRow_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot get device information!");
	}

	if (sCommand == "install")
		cout << "#!/bin/sh" << endl;
	pRow_Installation = pRow_Device->FK_Installation_getrow();

	CheckDevice(pRow_Device,bSourceCode);

	if( bIncludeDisklessMD )
	{
		if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Core_CONST )
		{
			// If this is a core, find all other devices in this installation that are media directors and which are set to diskless boot
			vector<Row_Device *> vectAllDevices;
			pRow_Device->FK_Installation_getrow()->Device_FK_Installation_getrows(&vectAllDevices);
			for(size_t s=0;s<vectAllDevices.size();++s)
			{
				Row_Device *pRow_Device = vectAllDevices[s];
				if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
				{
					Row_Device_DeviceData *pRow_Device_DeviceData = database_pluto_main.Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Diskless_Boot_CONST);
					if( pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1" )
						CheckDevice(pRow_Device,bSourceCode);
				}
			}
		}
	}

	if( !pRow_Distro )
		return 0;

	if( sCommand=="install" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;

//			cout << endl << "-----------------------------------------------------" << endl;
			cout << "# PK_Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << endl;
			cout << "# Rep. type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << endl;
			cout << "# Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get()
				<< " Type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << endl;
			InstallPackage(pPackageInfo);

			for(size_t s=0;s<pPackageInfo->m_vectPackageInfo.size();++s)
			{
				PackageInfo *pPackageInfoAlt = pPackageInfo->m_vectPackageInfo[s];
				InstallPackage(pPackageInfoAlt, true);
			}
			cout << "else" << endl;
//			cout << "\techo '**ERROR** Unable to get package " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "'" << endl;
			cout << "\techo \"***************************************************\"" << endl;
			cout << "\techo \"***ERROR*** Processing of package '" << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "' failed\"" << endl;
			cout << "\techo \"I tried every known option to find this software.  You will need\"" << endl;
			cout << "\techo \"to get it manually.  Press any key to continue.\"" << endl;
			cout << "\techo \"***************************************************\"" << endl;
			cout << "\tread" << endl;
			cout << "fi" << endl;
		}
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_BinaryExecutibles,pPackageInfo->m_sBinaryExecutiblesPathPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_SourceIncludes,pPackageInfo->m_sSourceIncludesPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_SourceImplementation,pPackageInfo->m_sSourceImplementationPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_BinaryLibrary,pPackageInfo->m_sBinaryLibraryPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_Configuration,pPackageInfo->m_sConfiguration);
		}
		cout << "echo \"Installation complete. If there was any source code that needed to be built, we created a file '/usr/pluto/install/compile.sh' which compiles them.\"" << endl;
	}
	else if( sCommand=="build" || sCommand=="buildall" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			if( pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->IsSource_get()==0 || (!pPackageInfo->m_bMustBuild && sCommand!="buildall")  )
				continue;

			cout << "./make "// << pRow_Distro->Installer_get()
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Name_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Repository_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Version_get() << "\"" << endl;
		}
	}
	else if( sCommand=="list" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			cout << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << "|" << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << endl;
		}
	}
	// The ConfirmDependencies program specific to this distro will be given the following parameters:
	// 1. Repository Source -- A URL, such as http://ftp.debian.org
	// 2. Repository -- such as Woody, or Stable
	// 3. Type -- 1: Package	2: Link to source  3: Archive
	// 4. Version
	// And then the following, which are directories or files and indicate where this program will expect to find the file
	// The ConfirmDependencies may choose to ignore these, such as with packages, since they define their own locations
    // 5. Binary Executibles
	// 6. Source Includes
	// 7. Source Implementation
	// 8. Binary Library
	// 9. Configuration
/*

	if( pRow_RepositorySource_URL )
		cout << " \"" << pRow_RepositorySource_URL->URL_get() << "\"";
	else
		cout << " \"\"";

	cout << " \"" << pRow_Package_Source->Repository_get() << "\""
		<< " \"" << pRow_Package_Source->FK_RepositoryType_get() << "\""
		<< " \"" << pRow_Package_Source->Version_get() << "\"";


		}
	}
*/
	return 0;
}

void CheckDevice(Row_Device *pRow_Device,bool bSourceCode)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Device->Table_Device_get()->Database_pluto_main_get();
	bool bDevelopment = bSourceCode;
	if( !bDevelopment )
	{
		Row_Device_DeviceData *pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Development_CONST);
		bDevelopment = pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1";
	}
	if( iPK_Distro==-1 )
	{
		Row_Device_DeviceData *pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_Distro_CONST);
		if( pRow_Device_DeviceData )
			iPK_Distro = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());
		else if( dceConfig.m_iPK_Distro )
			iPK_Distro = dceConfig.m_iPK_Distro;
	}

	pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(iPK_Distro);
	if( !pRow_Distro )
		pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(1);

	CheckDeviceLoop(pRow_Device,bDevelopment);
}

void CheckDeviceLoop(Row_Device *pRow_Device,bool bDevelopment)
{
	if( pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_isNull() )
		cout << "#Device: " << pRow_Device->Description_get() << " (#" << pRow_Device->FK_DeviceTemplate_get() << " " << pRow_Device->FK_DeviceTemplate_getrow()->Description_get() << ") doesn't require any software." << endl;
	else
	{
		string PkgName = pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_getrow()->Description_get();
		CheckPackage(pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_getrow(),pRow_Device,bDevelopment,pRow_Distro,false);
	}

	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
		CheckDeviceLoop( vectRow_Device[s],bDevelopment );
}

void CheckPackage(Row_Package *pRow_Package,Row_Device *pRow_Device,bool bDevelopment,Row_Distro *pRow_Distro,bool bMustBuildFromSource)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Distro->Table_Distro_get()->Database_pluto_main_get();

	string PkgName = pRow_Package->Description_get();

	// Start with the dependencies first, since we want the lowest dependency, and then the top one
	vector<Row_Package_Package *> vectRow_Package_Package;
	pRow_Package->Package_Package_FK_Package_getrows(&vectRow_Package_Package);
	for(size_t s=0;s<vectRow_Package_Package.size();++s)
	{
		Row_Package_Package *pRow_Package_Package = vectRow_Package_Package[s];

		// Skip this package if it's for development only and this is not a development box
		if( pRow_Package_Package->OnlyToBuild_get()==1 && !bDevelopment )
			continue;

		// Development = false because we only assume the first level of packages are intended to be built
		// from source
		CheckPackage(pRow_Package_Package->FK_Package_DependsOn_getrow(),pRow_Device,bDevelopment,pRow_Distro,false);
	}

	if( bDevelopment && !pRow_Package->FK_Package_Sourcecode_isNull() )
	{
		CheckPackage(pRow_Package->FK_Package_Sourcecode_getrow(),pRow_Device,bDevelopment,pRow_Distro,false);
	}

	// If the command is not 'view' or 'status', we don't need to list each package over and over again within
	// each of the dependencies.  For these commands, we just create a single list of all the packages, so if
	// we already got the info for this package, we don't need to do it again
	if(	sCommand!="view" && sCommand!="status" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			if( pPackageInfo->m_pRow_Package_Source->FK_Package_get()==pRow_Package->PK_Package_get() )
				return;
		}
	}

if( pRow_Package->PK_Package_get()==192 )
{
int k=2;
}

	// If we find a source specific for our distro, that does not require a build from source code,
	// that will be considered a perfect match, and added here.
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat_Match;
	// If we find a source that is compatible with our operating system, but not this distro of it,
	// and doesn't require a build from source, we'll set add it here
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat_OS;

	// All sources that are compatible will be put here, so we have some alternates if we can't install from the 
	// primary
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat;

	vector<Row_Package_Source *> vectRow_Package_Source;
	pRow_Package->Package_Source_FK_Package_getrows(&vectRow_Package_Source);
	for(size_t s=0;s<vectRow_Package_Source.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source[s];
		vector<Row_Package_Source_Compat *> vectRPSC;
		pRow_Package_Source->Package_Source_Compat_FK_Package_Source_getrows(&vectRPSC);

		// We've got 3 lists we can put this in.  We'll put it in whatever one is the best
		// We'll update the following when it's a match, and take the best.  The exception is the 
		// source code.  In that case we're not going to look for a source that is specific to 
		// our distribution because then we would always choose to take from a package before SVN, for example,
		// if that's what the user prefers.  So if it's source, just put them all in the "compat"
		// vector and we'll pick the user's preference
		Row_Package_Source_Compat *pRPSC_Match=NULL,*pRPSC_OS=NULL,*pRPSC_Comp=NULL;

		for(size_t t=0;t<vectRPSC.size();++t)
		{
			Row_Package_Source_Compat *pRPSC = vectRPSC[t];
			if( pRPSC->FK_Distro_get()==pRow_Distro->PK_Distro_get() && !pRPSC->MustBuildFromSource_get() )
				pRPSC_Match=pRPSC; // This is considered the perfect match
			else if( pRPSC->FK_OperatingSystem_get()==pRow_Distro->FK_OperatingSystem_get() && !pRPSC->MustBuildFromSource_get() )
				pRPSC_OS=pRPSC; // It matched the OS, so we'll use it if we don't find an exact match for the distro
		
			if( (pRPSC->FK_Distro_isNull() || pRow_Distro->PK_Distro_get()==pRPSC->FK_Distro_get()) && 
					(pRPSC->FK_OperatingSystem_isNull() || pRow_Distro->FK_OperatingSystem_get()==pRPSC->FK_OperatingSystem_get()) )
				pRPSC_Comp=pRPSC;
		}
		if( pRow_Package->IsSource_get()==1 && pRPSC_Comp )  // See comments above
			vectRow_Package_Source_Compat.push_back( pRPSC_Comp );
		else if( pRPSC_Match )
			vectRow_Package_Source_Compat_Match.push_back(pRPSC_Match); // This is considered the perfect match
		else if( pRPSC_OS )
			vectRow_Package_Source_Compat_OS.push_back(pRPSC_OS); // It matched the OS, so we'll use it if we don't find an exact match for the distro
		else if( pRPSC_Comp )
			vectRow_Package_Source_Compat.push_back( pRPSC_Comp );
	}

	// This will be the preferred source
	Row_Package_Source_Compat *pRow_Package_Source_Compat_Preferred=NULL;

	// The installation table will contain the users preferred repository type for getting this package
	// The FindPreferredSource function will search through the vect we pass, and if it finds a source that
	// matches the preferred type, it will return it.  Otherwise it will just return the first
	// entry in the vector.  We will give preference first to sources that were an exact match on our
	// distro, then sources that at least match the operating system, and lastly all other compatible
	// sources, which likely require building from source
	if( vectRow_Package_Source_Compat_Match.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat_Match,pRow_Device);
	else if( vectRow_Package_Source_Compat_OS.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat_OS,pRow_Device);
	else if( vectRow_Package_Source_Compat.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat,pRow_Device);

	if( sCommand=="view" || sCommand=="status" )
		cout << "PROGRAM: " << pRow_Package->Description_get() << " ";

    if( !pRow_Package_Source_Compat_Preferred )
	{
		if( !pRow_Package->FK_Package_Sourcecode_isNull() )
		{
			// We can only build this package from source.  The binaries are not available
			CheckPackage(pRow_Package->FK_Package_Sourcecode_getrow(),pRow_Device,bDevelopment,pRow_Distro,true);
			return;
		}
		else
		{
			cout << "#*ERROR* " << pRow_Package->Description_get() << " not available for this distro" << endl;
			return;
		}
	}

	// Get the info for the primary match
	PackageInfo *pPackageInfo = MakePackageInfo(pRow_Package_Source_Compat_Preferred,bMustBuildFromSource);
	if( !pPackageInfo  )
		return;
	listPackageInfo.push_back( pPackageInfo );

	// This will add the alternate sources from the given vects to pPackageInfo
	AddAlternateSources(vectRow_Package_Source_Compat_Match,pPackageInfo);
	AddAlternateSources(vectRow_Package_Source_Compat_OS,pPackageInfo);
	AddAlternateSources(vectRow_Package_Source_Compat,pPackageInfo);
}

Row_Package_Source_Compat *FindPreferredSource(vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat,Row_Device *pRow_Device)
{
	// Try to find a repository source that matches the preferred type for retrieving source/binaries for this installation
	for(size_t s=0;s<vectRow_Package_Source_Compat.size();++s)
	{
		Row_Package_Source_Compat *pRow_Package_Source_Compat=vectRow_Package_Source_Compat[s];

		if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_Package_getrow()->IsSource_get()==1 )
		{
			if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() ==
					pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Source_get() )
				return pRow_Package_Source_Compat;
		}
		else
		{
			if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() ==
					pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Binaries_get() )
				return pRow_Package_Source_Compat;
		}
	}

	// Otherwise return the first.  We'll be trying all the others anyway if it fails.  The preferred only determines our first choice
	return vectRow_Package_Source_Compat[0];
}

void AddAlternateSources(vector<Row_Package_Source_Compat *> &vectRow_Package_Source_Compat,PackageInfo *pPackageInfo)
{
	// See if we can find some alternates if the primary fails
	for(size_t s=0;s<vectRow_Package_Source_Compat.size();++s)
	{
		Row_Package_Source_Compat *pRow_Package_Source_Compat = vectRow_Package_Source_Compat[s];
		if( pRow_Package_Source_Compat!=pPackageInfo->m_pRow_Package_Source_Compat )
		{
			// This isn't our primary, add this to the list of alternates
			PackageInfo *pPackageInfo_local = MakePackageInfo(pRow_Package_Source_Compat,pRow_Package_Source_Compat->MustBuildFromSource_get()==1);
			if( pPackageInfo_local )
				pPackageInfo->m_vectPackageInfo.push_back( pPackageInfo_local );
		}
	}
}

PackageInfo *MakePackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,bool bMustBuildFromSource)
{
	if (pRow_Package_Source_Compat == NULL)
	{
		cout << "# Got NULL" << endl;
		return NULL;
	}
	Database_pluto_main *pDatabase_pluto_main = pRow_Package_Source_Compat->Table_Package_Source_Compat_get()->Database_pluto_main_get();
	Row_Package *pRow_Package = pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_Package_getrow();

	// First find the available repository sources
	Row_Package_Source *pRow_Package_Source = pRow_Package_Source_Compat->FK_Package_Source_getrow();
	Row_RepositorySource *pRow_RepositorySource = pRow_Package_Source->FK_RepositorySource_getrow();
	Row_RepositorySource_URL *pRow_RepositorySource_URL=NULL;

//	if( !pRow_Package_Source_Compat_Match->FK_Package_Source_getrow()->FK_RepositorySource_isNull() )
//	{
		vector<Row_RepositorySource_URL *> vectRow_RepositorySource_URL;
		pRow_Package_Source->FK_RepositorySource_getrow()->RepositorySource_URL_FK_RepositorySource_getrows(&vectRow_RepositorySource_URL);
		Row_RepositorySource_URL *pRow_RepositorySource_URL_Country=NULL;
		for(size_t t=0;t<vectRow_RepositorySource_URL.size();++t)
		{
			Row_RepositorySource_URL *pRow_RepositorySource_URL_test = vectRow_RepositorySource_URL[t];
			
			// If this repository is specifically mentioned in the Installation_RepositorySource_URL table, we'll use it.
			Row_Installation_RepositorySource_URL *pInstallation_RepositorySource_URL = 
					pDatabase_pluto_main->Installation_RepositorySource_URL_get()->GetRow(
						pRow_Installation->PK_Installation_get(),
						pRow_RepositorySource_URL_test->PK_RepositorySource_URL_get());

			if( pInstallation_RepositorySource_URL )
			{
				pRow_RepositorySource_URL=pRow_RepositorySource_URL_test;
				break;
			}

			// If this source is at least in the same country we'll consider it a tentative match while we keep looking for an exact match
			if( pRow_Installation->FK_Country_get()==pRow_RepositorySource_URL_test->FK_Country_get() )
				pRow_RepositorySource_URL_Country = pRow_RepositorySource_URL_test;  
		}
		if( !pRow_RepositorySource_URL )
		{
			// We didn't find an exact match, see if we found one in the same country
			if( pRow_RepositorySource_URL_Country )
				pRow_RepositorySource_URL=pRow_RepositorySource_URL_Country;
			// If not, just take the first one
			else if( vectRow_RepositorySource_URL.size() )
				pRow_RepositorySource_URL = vectRow_RepositorySource_URL[0];
		}
//	}

	if( !pRow_RepositorySource_URL )
	{
		cout << "**ERROR** Cannot find a URL for Repository Source: " << pRow_RepositorySource->Description_get() << endl;
		return NULL;
	}

	PackageInfo *pPackageInfo = new PackageInfo(pRow_Package_Source_Compat,pRow_Package_Source,pRow_RepositorySource_URL,bMustBuildFromSource);

	Row_Package_Directory *pRow_Package_Directory;
	
	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Binary_Executables_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sBinaryExecutiblesPathPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_BinaryExecutibles);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Source_Includes_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sSourceIncludesPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_SourceIncludes);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Source_Implementation_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sSourceImplementationPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_SourceImplementation);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Binary_Library_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sBinaryLibraryPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_BinaryLibrary);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Configuration_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sConfiguration = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_Configuration);
	}

	if( sCommand=="view" || sCommand=="status" )
	{
		cout << "\tFrom: " << pRow_RepositorySource->Description_get() << "(" << pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << "\tVersion: " << pRow_Package_Source->Version_get() << endl;
		cout << "\tRepository: " <<  pRow_Package_Source->Repository_get();
		if( pRow_RepositorySource_URL )
			cout << "\tAt: " << pRow_RepositorySource_URL->URL_get() << endl;
	}
	return pPackageInfo;
}

void VerifyFiles(PackageInfo *pPackageInfo,vector<Row_Package_Directory_File *> &vectRow_Package_Directory_File,string Path)
{
}

Row_Package_Directory *GetDirectory(Row_Package *pRow_Package,int PK_Directory)
{
	Row_Package_Directory *pRow_Package_Directory_OS=NULL,*pRow_Package_Directory_Empty=NULL;

	vector<Row_Package_Directory *> vectRow_Package_Directory;
	pRow_Package->Package_Directory_FK_Package_getrows(&vectRow_Package_Directory);
	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( pRow_Package_Directory->FK_Directory_get()!=PK_Directory )
			continue;
		if( pRow_Package_Directory->FK_Distro_get() == pRow_Distro->PK_Distro_get() )
			return pRow_Package_Directory;  // Perfect match
		else if( pRow_Package_Directory->FK_OperatingSystem_get() == pRow_Distro->FK_OperatingSystem_get() )
			pRow_Package_Directory_OS = pRow_Package_Directory; // We'll use this if we don't find an exact match on the distro
		else if( pRow_Package_Directory->FK_Distro_isNull() && pRow_Package_Directory->FK_OperatingSystem_isNull() )
			pRow_Package_Directory_Empty = pRow_Package_Directory;
	}

	// We didn't find an exact match, return an OS match if there is one
	if( pRow_Package_Directory_OS )
		return pRow_Package_Directory_OS;
	return pRow_Package_Directory_Empty;  // Will be NULL if there is no directory
}

void InstallPackage(PackageInfo *pPackageInfo, bool bElse)
{
	cout << (bElse ? "el" : "") << "if /usr/pluto/install/" << pRow_Distro->Installer_get()
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Name_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Repository_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Version_get() << "\"" 
		<< " \"" << pPackageInfo->m_sBinaryExecutiblesPathPath << "\"" 
		<< " \"" << pPackageInfo->m_sSourceIncludesPath << "\"" 
		<< " \"" << pPackageInfo->m_sSourceImplementationPath << "\"" 
		<< " \"" << pPackageInfo->m_sBinaryLibraryPath << "\"" 
		<< " \"" << pPackageInfo->m_sConfiguration << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Username_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Password_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Parms_get() << "\""
		<< "; then"
		<< endl;
	cout << "\techo \"Confirmation of package '" << pPackageInfo->m_pRow_Package_Source->Name_get() << "' went ok.\"" << endl;
}
