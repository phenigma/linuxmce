#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
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

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;


int CreateDevice::DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia)
{
	if( !m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	if( iPK_DHCPDevice )
	{
		string SQL = "SELECT FK_DeviceTemplate FROM DHCPDevice WHERE PK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);

		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
			iPK_DeviceTemplate = atoi(row[0]);
	}

	if( !iPK_DeviceTemplate )
	{
		cerr << "You did not specify the device.  You must use -c or -d" << endl;
		exit(1);
	}

	if( m_iPK_Installation<1 )
	{
		cerr << "You did not specify the installation ID.  You must use -i" << endl;
		exit(1);
	}

	map<int,string> mapParametersAdded; // Keep track of what DeviceData we have added as we go through
	
	PlutoSqlResult result,result1,result1b,result2,result3;
	MYSQL_ROW row;

	int iPK_DeviceCategory;
	string SQL = "SELECT FK_DeviceCategory,Description FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		iPK_DeviceCategory = atoi(row[0]);
	else
	{
		cerr << "Cannot find DeviceTemplate: " << iPK_DeviceTemplate << endl;
		exit(1);
	}

	SQL = "INSERT INTO Device(Description,FK_DeviceTemplate,FK_Installation,IPAddress,MACaddress";
	if( PK_Device_ControlledVia )
		SQL += ",FK_Device_ControlledVia";
	SQL+=") VALUES('" + StringUtils::SQLEscape(row[1]) + "'," + StringUtils::itos(iPK_DeviceTemplate) + "," + StringUtils::itos(m_iPK_Installation) + ",'" + sIPAddress + "','" + sMacAddress + "'";
	if( PK_Device_ControlledVia )
		SQL += "," + StringUtils::itos(PK_Device_ControlledVia);
		
	SQL += ");";
	int PK_Device = threaded_mysql_query_withID(SQL);
	if( !PK_Device )
	{
		cerr << "Unable to add device" << endl;
		exit(1);
	}

	// Loop through all the categories
	int iPK_DeviceCategory_Loop = iPK_DeviceCategory;
	while( iPK_DeviceCategory_Loop )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceCategory_DeviceData WHERE FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory_Loop);
		if( ( result1.r=mysql_query_result( SQL ) ) )
		{
			while( row=mysql_fetch_row( result1.r ) )
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
				SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
					"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
				if( threaded_mysql_query(SQL)!=0 )
				{
					cout << "Error adding device" << endl;
					exit(1);
				}
			}
		}
		SQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory_Loop);
		if( ( result1b.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result1b.r ) ) && row[0] )
			iPK_DeviceCategory_Loop = atoi(row[0]);
		else
			iPK_DeviceCategory_Loop = 0;
	}

	SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceTemplate_DeviceData JOIN DeviceData ON PK_DeviceData=FK_DeviceData WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result2.r=mysql_query_result( SQL ) ) )
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
			if( threaded_mysql_query(SQL)!=0 )
			{
				cout << "Error adding device" << endl;
				exit(1);
			}
		}
	}

	if( iPK_DHCPDevice )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DHCPDevice_DeviceData WHERE FK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);
		if( ( result2.r=mysql_query_result( SQL ) ) )
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
				if( threaded_mysql_query(SQL)!=0 )
				{
					cout << "Error adding device" << endl;
					exit(1);
				}
			}
		}
	}

	// Now we've got to see if we have any children we need to create automatically, first by the category
	CreateChildrenByCategory(PK_Device,iPK_DeviceCategory);
	CreateChildrenByTemplate(PK_Device,iPK_DeviceTemplate);

	return PK_Device;
}

void CreateDevice::CreateChildrenByCategory(int iPK_Device,int iPK_DeviceCategory)
{
	PlutoSqlResult result,result2;
	MYSQL_ROW row;

	string SQL = "SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
	if( ( result.r=mysql_query_result( SQL ) ) )
	{
		while( row=mysql_fetch_row( result.r ) )
		{
			DoIt(0,atoi(row[0]),"","",iPK_Device);
		}
	}

	SQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
	if( ( result2.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result2.r ))!=NULL && row[0] )
		CreateChildrenByCategory( iPK_Device, atoi(row[0]) );
}

void CreateDevice::CreateChildrenByTemplate(int iPK_Device,int iPK_DeviceTemplate)
{
	PlutoSqlResult result;
	MYSQL_ROW row;

	string SQL = "SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceTemplate_ControlledVia=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mysql_query_result( SQL ) ) )
	{
		while( row=mysql_fetch_row( result.r ) )
		{
			DoIt(0,atoi(row[0]),"","",iPK_Device);
		}
	}
}
