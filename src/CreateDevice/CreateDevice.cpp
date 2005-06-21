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
	
	PlutoSqlResult result,result1,result1b,result2,result3,resultPackage;
	MYSQL_ROW row;

	int iPK_DeviceCategory;
	string SQL = "SELECT FK_DeviceCategory,Description,ConfigureScript,IsPlugAndPlay,FK_Package FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		iPK_DeviceCategory = atoi(row[0]);
	else
	{
		cerr << "Cannot find DeviceTemplate: " << iPK_DeviceTemplate << endl;
		exit(1);
	}

	int iPK_Package = row[4] ? atoi(row[4]) : 0;

	SQL = "INSERT INTO Device(Description,FK_DeviceTemplate,FK_Installation,IPAddress,MACaddress,Status";
	if( PK_Device_ControlledVia )
		SQL += ",FK_Device_ControlledVia";
	SQL+=") VALUES('" + StringUtils::SQLEscape(row[1]) + "'," + StringUtils::itos(iPK_DeviceTemplate) + "," + StringUtils::itos(m_iPK_Installation) + ",'" + sIPAddress + "','" + sMacAddress + "','" + (m_bDontCallConfigureScript ? "" : "**RUN_CONFIG**") + "'";
	if( PK_Device_ControlledVia )
		SQL += "," + StringUtils::itos(PK_Device_ControlledVia);
		
	SQL += ");";
	int PK_Device = threaded_mysql_query_withID(SQL);
	if( !PK_Device )
	{
		cerr << "Unable to add device" << endl;
		exit(1);
	}

	g_pPlutoLogger->Write(LV_STATUS,"Inserted device: %d Package: %d configure: %d",PK_Device,iPK_Package,(int) m_bDontCallConfigureScript);

	ConfirmRelations(PK_Device);

	// If we weren't given a controlled via, try to find an appropriate one
	if( !PK_Device_ControlledVia )
	{
		PlutoSqlResult result_cv1,result_cv2;
		int iPK_Device_ControlledVia_New=0;
		SQL = "SELECT PK_Device FROM DeviceTemplate_DeviceTemplate_ControlledVia JOIN Device ON Device.FK_DeviceTemplate=FK_DeviceTemplate_ControlledVia WHERE DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
		if( (result_cv1.r=mysql_query_result(SQL)) && (row=mysql_fetch_row(result_cv1.r)) )
			iPK_Device_ControlledVia_New = atoi(row[0]);
		else
		{
			SQL = "SELECT PK_Device FROM DeviceTemplate_DeviceCategory_ControlledVia JOIN Device ON DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory=Device.FK_DeviceCategory WHERE DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
			if( (result_cv2.r=mysql_query_result(SQL)) && (row=mysql_fetch_row(result_cv2.r)) )
				iPK_Device_ControlledVia_New = atoi(row[0]);
		}

		if( iPK_Device_ControlledVia_New )
		{
			SQL = "UPDATE Device SET FK_Device_ControlledVia=" + StringUtils::itos(iPK_Device_ControlledVia_New) +
				" WHERE PK_Device=" + StringUtils::itos(PK_Device);
			if( threaded_mysql_query(SQL)!=0 )
			{
				cout << "Error updating device controlled via" << endl;
				exit(1);
			}
		}
	}

	// Loop through all the categories
	int iPK_DeviceCategory_Loop = iPK_DeviceCategory;
	while( iPK_DeviceCategory_Loop )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceCategory_DeviceData WHERE FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory_Loop);
		if( ( result1.r=mysql_query_result( SQL ) ) )
		{
g_pPlutoLogger->Write(LV_STATUS,"Found %d rows with %s",(int) result1.r->row_count,SQL.c_str());
			while( row=mysql_fetch_row( result1.r ) )
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
g_pPlutoLogger->Write(LV_STATUS,"Added parameter from category %d %s",atoi(row[0]),mapParametersAdded[ atoi(row[0]) ].c_str());

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
g_pPlutoLogger->Write(LV_STATUS,"Found %d rows with %s",(int) result2.r->row_count,SQL.c_str());
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
g_pPlutoLogger->Write(LV_STATUS,"Executing %s",SQL.c_str());
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
		if( ( result3.r=mysql_query_result( SQL ) ) )
		{
g_pPlutoLogger->Write(LV_STATUS,"Found %d rows with %s",(int) result3.r->row_count,SQL.c_str());
			while( row=mysql_fetch_row( result3.r ) )
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

	string SQL = "SELECT FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceCategory_ControlledVia FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
	if( ( result.r=mysql_query_result( SQL ) ) )
	{
		while( row=mysql_fetch_row( result.r ) )
		{
			int PK_Device = DoIt(0,atoi(row[0]),"","",iPK_Device);
			if( row[1] && atoi(row[1]) )
			{
				// Need to reroute messages to parent
				SQL = "UPDATE Device SET FK_Device_RouteTo=" + StringUtils::itos(iPK_Device) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
				threaded_mysql_query(SQL.c_str());

				PlutoSqlResult result;
				SQL = "SELECT FK_Pipe,FK_Command_Input,FK_Command_Output,ToChild,Command.Description FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe LEFT JOIN Command ON FK_Command_Input=PK_Command WHERE FK_DeviceTemplate_DeviceCategory_ControlledVia="+string(row[2]);
				if( ( result.r=mysql_query_result( SQL ) ) )
				{
					string sLastDescription;
					while( row=mysql_fetch_row( result.r ) )
					{
						SQL = "INSERT INTO Device_Device_Pipe(FK_Device_To,FK_Device_From,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES(";
						if( row[3] && atoi(row[3]) )
							SQL += StringUtils::itos(PK_Device) + "," + StringUtils::itos(iPK_Device);
						else
							SQL += StringUtils::itos(iPK_Device) + "," + StringUtils::itos(PK_Device);
						SQL += string(",") + (row[0] ? row[0] : "NULL") + "," + (row[1] ? row[1] : "NULL")  + "," + (row[2] ? row[2] : "NULL") + ")";
						threaded_mysql_query(SQL.c_str());

						if( row[4] && sLastDescription!=row[4] ) // We've got a description for the input
						{
							sLastDescription=row[4];
							SQL = "UPDATE Device SET Description = concat(Description,'/" + StringUtils::SQLEscape(sLastDescription) + "') WHERE PK_Device=" + StringUtils::itos(PK_Device);
							threaded_mysql_query(SQL.c_str());
						}
					}
				}
			}
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

	string SQL = "SELECT FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceTemplate_ControlledVia=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mysql_query_result( SQL ) ) )
	{
		while( row=mysql_fetch_row( result.r ) )
		{
			int PK_Device = DoIt(0,atoi(row[0]),"","",iPK_Device);
			if( row[1] && atoi(row[1]) )
			{
				// Need to reroute messages to parent
				SQL = "UPDATE Device SET FK_Device_RouteTo=" + StringUtils::itos(iPK_Device) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
				threaded_mysql_query(SQL.c_str());

				PlutoSqlResult result;
				SQL = "SELECT FK_Pipe,FK_Command_Input,FK_Command_Output,ToChild,Command.Description FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe LEFT JOIN Command ON FK_Command_Input=PK_Command WHERE FK_DeviceTemplate_DeviceTemplate_ControlledVia="+string(row[2]);
				if( ( result.r=mysql_query_result( SQL ) ) )
				{
					string sLastDescription;
					while( row=mysql_fetch_row( result.r ) )
					{
						SQL = "INSERT INTO Device_Device_Pipe(FK_Device_To,FK_Device_From,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES(";
						if( row[3] && atoi(row[3]) )
							SQL += StringUtils::itos(PK_Device) + "," + StringUtils::itos(iPK_Device);
						else
							SQL += StringUtils::itos(iPK_Device) + "," + StringUtils::itos(PK_Device);
						SQL += string(",") + (row[0] ? row[0] : "NULL") + "," + (row[1] ? row[1] : "NULL")  + "," + (row[2] ? row[2] : "NULL") + ")";
						threaded_mysql_query(SQL.c_str());

						if( row[4] && sLastDescription!=row[4] ) // We've got a description for the input
						{
							sLastDescription=row[4];
							SQL = "UPDATE Device SET Description = concat(Description,'/" + StringUtils::SQLEscape(sLastDescription) + "') WHERE PK_Device=" + StringUtils::itos(PK_Device);
							threaded_mysql_query(SQL.c_str());
						}
					}
				}
			}
		}
	}
}

void CreateDevice::ConfirmRelations(int PK_Device,bool bRecurseChildren,bool bOnlyAddDevicesOnCore)
{
	PlutoSqlResult result_dt,result_related,result_related2,result_related3;
	MYSQL_ROW row,row3;

	int PK_DeviceTemplate=0;
	string SQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	if( (result_dt.r=mysql_query_result(SQL)) && (row=mysql_fetch_row(result_dt.r)) )
		PK_DeviceTemplate=atoi(row[0]);

	if( !PK_DeviceTemplate )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Invalid device template for device %d",PK_Device);
		return;
	}

	// See if this requires dragging any other types of devices
	SQL = "SELECT FK_DeviceTemplate_Related,Value FROM DeviceTemplate_DeviceTemplate_Related WHERE FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate);
	if( ( result_related.r=mysql_query_result( SQL ) ) )
	{
g_pPlutoLogger->Write(LV_STATUS,"Found result_related %d rows with %s",(int) result_related.r->row_count,SQL.c_str());
		while( row=mysql_fetch_row( result_related.r ) )
		{
			int iPK_DeviceTemplate_Related = atoi(row[0]);
			int iRelation= (row[1] ? atoi(row[1]) : 0);
			if( iRelation>=1 && iRelation<=3 )
			{
				// We need this device too.  See if we already have it
				SQL = "SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related);
				if( (result_related2.r=mysql_query_result(SQL)) && result_related2.r->row_count>0 )
					continue;  // It's ok.  We got it

				// That device doesn't exist.  We need to create it
				if( iRelation==1 && !bOnlyAddDevicesOnCore )  // It's a sister device
				{
					SQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
					if( (result_related3.r=mysql_query_result(SQL)) && (row3=mysql_fetch_row(result_related3.r)) )
						DoIt(0,iPK_DeviceTemplate_Related,"","",atoi(row3[0]));
				}
				else if( iRelation==2 )  // It's a child of the core (ie DCERouter's parent)
				{
					SQL = "SELECT FK_Device_ControlledVia FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST);
					if( (result_related3.r=mysql_query_result(SQL)) && (row3=mysql_fetch_row(result_related3.r)) )
						DoIt(0,iPK_DeviceTemplate_Related,"","",atoi(row3[0]));
				}
				else if( iRelation==3 )  // It's a plugin (ie child of DCERouter)
				{
					SQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST);
					if( (result_related3.r=mysql_query_result(SQL)) && (row3=mysql_fetch_row(result_related3.r)) )
						DoIt(0,iPK_DeviceTemplate_Related,"","",atoi(row3[0]));
				}
			}
		}
	}

	if( bRecurseChildren )
	{
		PlutoSqlResult result_child_dev;
		SQL = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);
		if( ( result_child_dev.r=mysql_query_result( SQL ) ) )
		{
	g_pPlutoLogger->Write(LV_STATUS,"Found result_child_dev %d rows with %s",(int) result_child_dev.r->row_count,SQL.c_str());
			while( row=mysql_fetch_row( result_child_dev.r ) )
				ConfirmRelations(atoi(row[0]),true);
		}
	}
}
