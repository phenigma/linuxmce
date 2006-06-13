#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "CreateDevice.h"
#include "UserUtils.h"

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

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;

namespace DCE
{
	Logger *g_pPlutoLogger;
}


#include "PlutoUtils/DatabaseUtils.h"





int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger("/var/log/pluto/CreateDevice.log");

	int iPK_DeviceTemplate=0,iPK_DHCPDevice=0,iPK_Device_Controlled_Via=0,iPK_Device_RelatedTo=0;
	string sIPAddress,sMacAddress,sDeviceData,sUserName;
	bool bDontCallConfigureScript=false,bDontInstallPackages=false,bInstallPackagesInBackground=false;

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
		case 'c':
			iPK_DHCPDevice = atoi(argv[++optnum]);
			break;
		case 'C':
			iPK_Device_Controlled_Via = atoi(argv[++optnum]);
			break;
		case 'd':
			iPK_DeviceTemplate = atoi(argv[++optnum]);
			break;
		case 'i':
			dceConfig.m_iPK_Installation = atoi(argv[++optnum]);
			break;
		case 'I':
			sIPAddress = argv[++optnum];
			break;
		case 'M':
			sMacAddress = argv[++optnum];
			break;
		case 'n':
			bDontCallConfigureScript=true;
			break;
		case 'A':
			sDeviceData = argv[++optnum];
			break;
		case 'R':
			iPK_Device_RelatedTo = atoi(argv[++optnum]);
			break;
		case 'x':
			bDontInstallPackages = true;
			break;
		case 'b':
			bInstallPackagesInBackground = true;
			break;
		case 'U':
			sUserName = argv[++optnum];
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError || (!iPK_DHCPDevice && !iPK_DeviceTemplate && sUserName.size()==0) )
	{
		cout << "CreateDevice, v." << VERSION << endl
			<< "Usage: CreateDevice [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-c PK_DHCPDevice] [-d PK_DeviceTemplate] [-i PK_Installation] [-I IPAddress/auto] [-M MacAddress]" << endl
			<< "[-C PK_Device_Controlled_Via] [-n don't call configure script]" << endl
			<< "[-A Default Device data tab delimited as PK_DeviceData\tValue\t...]" << endl
			<< "[-R PK_Device (related to if there are multiple possible controlled Via's)]" << endl
			<< "[-x don't call InstallNewDevice to add packages]" << endl
			<< "[-b Install packages in background]" << endl
			<< "[-U Username] CreateUser--not device" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl;

		exit(0);
	}

	string args;
	for(int optnum=1;optnum<argc;++optnum)
		args += string(argv[optnum]) + " ";
	g_pPlutoLogger->Write(LV_STATUS,"Called with: %s",args.c_str());

	if( sUserName.size() )
	{
		MySqlHelper mySqlHelper(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
		if( dceConfig.m_iPK_Installation<1 )
		{
			PlutoSqlResult result;
			MYSQL_ROW row;
			string SQL = "SELECT DISTINCT PK_Installation FROM Installation";
			if( ( result.r=mySqlHelper.mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) && result.r->row_count==1 )
				dceConfig.m_iPK_Installation = atoi(row[0]);
		}
		UserUtils userUtils(&mySqlHelper,dceConfig.m_iPK_Installation);
		int PK_Users = userUtils.AddUser(sUserName);
		cout << PK_Users << endl;
		return 0;
	}

	CreateDevice createDevice(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
	createDevice.m_bDontInstallPackages=bDontInstallPackages;
	createDevice.m_bDontCallConfigureScript=bDontCallConfigureScript;
	createDevice.m_bInstallPackagesInBackground=bInstallPackagesInBackground;

	int PK_Device=createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sIPAddress,sMacAddress,iPK_Device_Controlled_Via,sDeviceData,iPK_Device_RelatedTo);
	if( PK_Device==0 )
	{
		cerr << "CreateDevice failed" << endl;
		exit(1);
	}
	
	cout << PK_Device << endl;

	return 0;
}

