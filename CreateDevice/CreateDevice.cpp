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
	g_pPlutoLogger = new FileLogger(stdout);

	int iPK_DeviceTemplate=0,iPK_DHCPDevice=0;
	string sIPAddress,sMacAddress;

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
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError || (!iPK_DHCPDevice && !iPK_DeviceTemplate) )
	{
		cout << "CreateDevice, v." << VERSION << endl
			<< "Usage: MakeRelease [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-c PK_DHCPDevice] [-d PK_DeviceTemplate] [-i PK_Installation] [-I IPAddress] [-M MacAddress]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl;

		exit(0);
	}

	MySqlHelper mySQLHelper(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
	if( !mySQLHelper.m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	if( iPK_DHCPDevice )
	{
		string SQL = "SELECT FK_DeviceTemplate FROM DHCPDevice WHERE PK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);

		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=mySQLHelper.mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
			iPK_DeviceTemplate = atoi(row[0]);
	}

	if( !iPK_DeviceTemplate )
	{
		cerr << "You did not specify the device.  You must use -c or -d" << endl;
		exit(1);
	}

	if( dceConfig.m_iPK_Installation<1 )
	{
		cerr << "You did not specify the installation ID.  You must use -i" << endl;
		exit(1);
	}

	map<int,string> mapParametersAdded; // Keep track of what DeviceData we have added as we go through
	
	PlutoSqlResult result,result1,result1b,result2,result3;
	MYSQL_ROW row;

	int iPK_DeviceCategory;
	string SQL = "SELECT FK_DeviceCategory,Description FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mySQLHelper.mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		iPK_DeviceCategory = atoi(row[0]);
	else
	{
		cerr << "Cannot find DeviceTemplate: " << iPK_DeviceTemplate << endl;
		exit(1);
	}

	SQL = "INSERT INTO Device(Description,FK_DeviceTemplate,FK_Installation,IPAddress,MACaddress) VALUES('" + StringUtils::SQLEscape(row[1]) + "'," + StringUtils::itos(iPK_DeviceTemplate) + "," + StringUtils::itos(dceConfig.m_iPK_Installation) + ",'" + sIPAddress + "','" + sMacAddress + "');";
	int PK_Device = mySQLHelper.threaded_mysql_query_withID(SQL);
	if( !PK_Device )
	{
		cerr << "Unable to add device" << endl;
		exit(1);
	}

	// Loop through all the categories
	while( iPK_DeviceCategory )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceCategory_DeviceData WHERE FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
		if( ( result1.r=mySQLHelper.mysql_query_result( SQL ) ) )
		{
			while( row=mysql_fetch_row( result1.r ) )
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
				SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
					"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
				if( mySQLHelper.threaded_mysql_query(SQL)!=0 )
				{
					cout << "Error adding device" << endl;
					exit(1);
				}
			}
		}
		SQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
		if( ( result1b.r=mySQLHelper.mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result1b.r ) ) && row[0] )
			iPK_DeviceCategory = atoi(row[0]);
		else
			iPK_DeviceCategory = 0;
	}
	SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceTemplate_DeviceData JOIN DeviceData ON PK_DeviceData=FK_DeviceData WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result2.r=mySQLHelper.mysql_query_result( SQL ) ) )
	{
		while( row=mysql_fetch_row( result2.r ) )
		{
			if( mapParametersAdded.find(atoi(row[0]))!=mapParametersAdded.end() )
			{
				SQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "' " +
					"WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + row[0];
			}
			else
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
				SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
					"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
			}
			if( mySQLHelper.threaded_mysql_query(SQL)!=0 )
			{
				cout << "Error adding device" << endl;
				exit(1);
			}
		}
	}

	if( iPK_DHCPDevice )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DHCPDevice_DeviceData WHERE FK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);
		if( ( result2.r=mySQLHelper.mysql_query_result( SQL ) ) )
		{
			while( row=mysql_fetch_row( result2.r ) )
			{
				if( mapParametersAdded.find(atoi(row[0]))!=mapParametersAdded.end() )
				{
					SQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "' " +
						"WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + row[0];
				}
				else
				{
					mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
					SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
						"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
				}
				if( mySQLHelper.threaded_mysql_query(SQL)!=0 )
				{
					cout << "Error adding device" << endl;
					exit(1);
				}
			}
		}
	}

	cout << PK_Device << endl;
	return 0;
}