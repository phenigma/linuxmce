#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "CreateDevice.h"

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

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger("/var/log/pluto/CreateDevice.newlog");

	int iPK_DeviceTemplate=0,iPK_DHCPDevice=0,iPK_Device_Controlled_Via=0;
	string sIPAddress,sMacAddress;
	bool bDontCallConfigureScript=false;

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
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError || (!iPK_DHCPDevice && !iPK_DeviceTemplate) )
	{
		cout << "CreateDevice, v." << VERSION << endl
			<< "Usage: CreateDevice [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-c PK_DHCPDevice] [-d PK_DeviceTemplate] [-i PK_Installation] [-I IPAddress] [-M MacAddress]" << endl
			<< "[-C PK_Device_Controlled_Via] [-n don't call configure script]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl;

		exit(0);
	}

	CreateDevice createDevice(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
	createDevice.m_bDontCallConfigureScript=bDontCallConfigureScript;

	int PK_Device=createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sIPAddress,sMacAddress,iPK_Device_Controlled_Via);
	if( PK_Device==0 )
	{
		cerr << "CreateDevice failed" << endl;
		exit(1);
	}
	
	cout << PK_Device << endl;

	return 0;
}

