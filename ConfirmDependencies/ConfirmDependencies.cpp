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

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_Package.h"
#include "pluto_main/Table_RepositorySource.h"
#include "pluto_main/Table_RepositorySource_URL.h"
#include "pluto_main/Table_Installation_RepositorySource_URL.h"
#include "pluto_main/Table_Package.h"
#include "pluto_main/Table_Package_Distro.h"
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_DeviceCategory.h"

#define ConfirmDependencies_VERSION "0.1"

using namespace std;
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

void CheckPackage(Row_Device *pRow_Device,map<int,string> &mapPackages);
void CheckPackage(Row_Device *pRow_Device,map<int,string> &mapPackages,bool bDevelopment,Row_Distro *pRow_Distro);

int main(int argc, char *argv[])
{
	DCEConfig dceConfig;
	g_pPlutoLogger = new FileLogger(stdout);

	bool bError=false; // An error parsing the command line
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
		case 'd':
			dceConfig.m_iPK_Device_Computer = atoi(argv[++optnum]);
			break;
		default:
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "ConfirmDependencies, v." << ConfirmDependencies_VERSION << endl
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

	Database_pluto_main database_pluto_main;
	if(!database_pluto_main.Connect(&dceConfig))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return 0;
	}

	Row_Device *pRow_Device = database_pluto_main.Device_get()->GetRow(dceConfig.m_iPK_Device_Computer);
	if( !pRow_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot get device information!");
	}

	map<int,string> mapPackages;

	CheckPackage(pRow_Device,mapPackages);
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
				if( pRow_Device_DeviceData && pRow_Device_DeviceData->Value_get()=="1" )
					CheckPackage(pRow_Device,mapPackages);
			}
		}
	}


	return 0;
}

void CheckPackage(Row_Device *pRow_Device,map<int,string> &mapPackages)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Device->Table_Device_get()->Database_pluto_main_get();
	Row_Device_DeviceData *pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Development_CONST);
	bool bDevelopment = pRow_Device_DeviceData && pRow_Device_DeviceData->Value_get()=="1";
	pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_Distro_CONST);
	int PK_Distro = 1;
	if( pRow_Device_DeviceData )
		PK_Distro = atoi(pRow_Device_DeviceData->Value_get().c_str());

	Row_Distro *pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(PK_Distro);
	if( !pRow_Distro )
		pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(1);

	CheckPackage(pRow_Device,mapPackages,bDevelopment,pRow_Distro);
}

void CheckPackage(Row_Device *pRow_Device,map<int,string> &mapPackages,bool bDevelopment,Row_Distro *pRow_Distro)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Device->Table_Device_get()->Database_pluto_main_get();

	// Get all the packages that this device template uses
	vector<Row_DeviceTemplate_Package *> vectRow_DeviceTemplate_Package;
	pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_Package_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_Package);
	for(size_t s=0;s<vectRow_DeviceTemplate_Package.size();++s)
	{
		Row_DeviceTemplate_Package *pRow_DeviceTemplate_Package = vectRow_DeviceTemplate_Package[s];

		// Skip this package if it's for development only and this is not a development box
		if( pRow_DeviceTemplate_Package->DevelopmentOnly_get()==1 && !bDevelopment )
			continue;

		// Skip this package if it's for a particular distribution other than this one
		if( pRow_DeviceTemplate_Package->FK_Distro_get() && pRow_DeviceTemplate_Package->FK_Distro_get()!=pRow_Distro->PK_Distro_get() )
			continue;

		// Skip this package if it's for a particular operating system other than this one
		if( pRow_DeviceTemplate_Package->FK_OperatingSystem_get() && pRow_DeviceTemplate_Package->FK_Distro_get()!=pRow_Distro->FK_OperatingSystem_get() )
			continue;

		// If we are here, the package is needed.
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

		// First find the available repository sources
		Row_Package_Distro *pRow_Package_Distro = pDatabase_pluto_main->Package_Distro_get()->GetRow(pRow_DeviceTemplate_Package->FK_Package_get(),pRow_Distro->PK_Distro_get());
		if( !pRow_Package_Distro )
		{
			cerr << "Package: " << pRow_DeviceTemplate_Package->FK_Package_getrow()->Description_get() << " is required, but not specified for distro: " << pRow_Distro->Description_get() << endl;
			cerr << "Skipping it, although this could create problems." << endl;
			continue;
		}

		Row_RepositorySource_URL *pRow_RepositorySource_URL=NULL;
		if( !pRow_Package_Distro->FK_RepositorySource_isNull() )
		{
			vector<Row_RepositorySource_URL *> vectRow_RepositorySource_URL;
			pRow_Package_Distro->FK_RepositorySource_getrow()->RepositorySource_URL_FK_RepositorySource_getrows(&vectRow_RepositorySource_URL);
			Row_RepositorySource_URL *pRow_RepositorySource_URL_Country=NULL;
			for(size_t t=0;t<vectRow_RepositorySource_URL.size();++t)
			{
				Row_RepositorySource_URL *pRow_RepositorySource_URL_test = vectRow_RepositorySource_URL[t];
				
				// If this repository is specifically mentioned in the Installation_RepositorySource_URL table, we'll use it.
				Row_Installation_RepositorySource_URL *pInstallation_RepositorySource_URL = 
						pDatabase_pluto_main->Installation_RepositorySource_URL_get()->GetRow(
							pRow_Device->FK_Installation_get(),
							pRow_RepositorySource_URL_test->PK_RepositorySource_URL_get());

				if( pInstallation_RepositorySource_URL )
				{
					pRow_RepositorySource_URL=pRow_RepositorySource_URL_test;
					break;
				}

				// If this source is at least in the same country we'll consider it a tentative match while we keep looking for an exact match
				if( pRow_Device->FK_Installation_getrow()->FK_Country_get()==pRow_RepositorySource_URL_test->FK_Country_get() )
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
		}

		ostringstream sCommand;
		if( pRow_Distro->ConfirmDependencyProgram_get()!="" )
			sCommand << pRow_Distro->ConfirmDependencyProgram_get();
		else
			sCommand << "ConfirmDependency_" << pRow_Distro->PK_Distro_get();

		if( pRow_RepositorySource_URL )
			sCommand << " \"" << pRow_RepositorySource_URL->URL_get() << "\"";
		else
			sCommand << " \"\"";

		sCommand << " \"" << pRow_Package_Distro->Repository_get() << "\""
			<< " \"" << pRow_Package_Distro->FK_RepositoryType_get() << "\""
			<< " \"" << pRow_Package_Distro->Version_get() << "\"";

		system(sCommand.str().c_str());

	}
}
