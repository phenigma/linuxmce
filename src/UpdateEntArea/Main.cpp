#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "UpdateEntArea.h"

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
	g_pPlutoLogger = new FileLogger("/var/log/pluto/UpdateEntArea.newlog");

	bool bError=false;
	int iPK_EntertainArea=0,iPK_Template=0;
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
		case 'i':
			dceConfig.m_iPK_Installation = atoi(argv[++optnum]);
			break;
		case 'e':
			iPK_EntertainArea = atoi(argv[++optnum]);
			break;
		case 't':
			iPK_Template = atoi(argv[++optnum]);
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "UpdateEntArea, v." << VERSION << endl
			<< "Usage: UpdateEntArea [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-i PK_Installation] [-e PK_EntertainArea] [-t PK_Template]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "To create a Media scenario, pass in -e and -t" << endl;

		exit(0);
	}

	UpdateEntArea UpdateEntArea(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);

	if( iPK_EntertainArea || iPK_Template )
	{
		Row_EntertainArea *pRow_EntertainArea = UpdateEntArea.m_pDatabase_pluto_main->EntertainArea_get()->GetRow(iPK_EntertainArea);
		if( !pRow_EntertainArea || !iPK_Template )
		{
			cerr << "Bad entertainment area/template" << endl;
			return 1;
		}

		UpdateEntArea.AddDefaultCommandsToEntArea(pRow_EntertainArea,iPK_Template);
	}
	else
		UpdateEntArea.DoIt();
	return 0;
}

