#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/DatabaseUtils.h"
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
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "DCEConfig.h"

#define  VERSION "<=version=>"

extern DCEConfig dceConfig;

using namespace std;
using namespace DCE;

int CreateDevice::DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia,string sDeviceData,int iPK_Device_RelatedTo,int iPK_Room)
{
	cerr << "CreateDevice::DoIt called with: iPK_DHCPDevice=" << iPK_DHCPDevice << "; iPK_DeviceTemplate=" << iPK_DeviceTemplate
		<< "; sIPAddress=" << sIPAddress << "; sMacAddress=" << sMacAddress << "; PK_Device_ControlledVia=" << PK_Device_ControlledVia 
		<< "; sDeviceData=" << sDeviceData << "; iPK_Device_RelatedToendl=" << iPK_Device_RelatedTo << endl;
	if( !m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		return 0;
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
		return 0;
	}


	if( m_iPK_Installation<1 )
	{
		PlutoSqlResult result;
		MYSQL_ROW row;
		string SQL = "SELECT DISTINCT PK_Installation FROM Installation";
		if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) && result.r->row_count==1 )
			m_iPK_Installation = atoi(row[0]);
		else
		{
			cerr << "You did not specify the installation ID.  You must use -i" << endl;
			return 0;
		}
	}

	map<int,string> mapParametersAdded; // Keep track of what DeviceData we have added as we go through
	
	PlutoSqlResult result,result1,result1b,result2,result3,resultPackage;
	MYSQL_ROW row;

	int iPK_DeviceCategory;
	string SQL = "SELECT FK_DeviceCategory,Description,ConfigureScript,IsPlugAndPlay,FK_Package,FK_InfraredGroup FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		iPK_DeviceCategory = atoi(row[0]);
	else
	{
		cerr << "Cannot find DeviceTemplate: " << iPK_DeviceTemplate << endl;
		return 0;
	}

	if( row[5] && atoi(row[5]) )  // Only ask for the i/r codes if there is an infrared group
		system((string("/usr/pluto/bin/WebDB_GetIR.sh 0 ") + StringUtils::itos(iPK_DeviceTemplate)).c_str());
	
	// Check if this device template has 'one per pc' set and ther's already one
	// Use a temporary PK_Device_ControlledVia_temp because in rare circumstances this can't be determined until after the device is created
	// because the controlling device doesn't exist.  In such cases we don't care because there can't be another such device if there's nothing
	// to control it
	int PK_Device_Existing,PK_Device_ControlledVia_temp = PK_Device_ControlledVia;
	if( !PK_Device_ControlledVia_temp )
		PK_Device_ControlledVia_temp=DatabaseUtils::FindControlledViaCandidate(this,0,iPK_DeviceTemplate,iPK_Device_RelatedTo,m_iPK_Installation);
	if( PK_Device_ControlledVia_temp && (PK_Device_Existing=DatabaseUtils::ViolatesDuplicateRules(this,PK_Device_ControlledVia_temp,iPK_DeviceTemplate))!=0 )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Not creating duplicate device template %d on %d, using %d",iPK_DeviceTemplate,PK_Device_ControlledVia_temp,PK_Device_Existing);
		return PK_Device_Existing;
	}
		
	int iPK_Package = row[4] ? atoi(row[4]) : 0;

	SQL = "INSERT INTO Device(Description,FK_DeviceTemplate,FK_Installation,FK_Room,IPAddress,MACaddress,Status";
	if( PK_Device_ControlledVia )
		SQL += ",FK_Device_ControlledVia";
	SQL+=") VALUES('" + StringUtils::SQLEscape(row[1]) + "'," + StringUtils::itos(iPK_DeviceTemplate) + "," + StringUtils::itos(m_iPK_Installation) + 
		"," + (iPK_Room ? StringUtils::itos(iPK_Room) : "NULL") + ",'" + sIPAddress + "','" + sMacAddress + "','" + (m_bDontCallConfigureScript ? "" : "**RUN_CONFIG**") + "'";
	if( PK_Device_ControlledVia )
		SQL += "," + StringUtils::itos(PK_Device_ControlledVia);
		
	SQL += ");";
	int PK_Device = threaded_mysql_query_withID(SQL);
	if( !PK_Device )
	{
		cerr << "Unable to add device" << endl;
		return 0;
	}

	g_pPlutoLogger->Write(LV_STATUS,"Inserted device: %d Package: %d installation: %d configure: %d",
		PK_Device,iPK_Package,m_iPK_Installation,(int) m_bDontCallConfigureScript);

	bool bIsOrbiter=false;
	// Loop through all the categories
	int iPK_DeviceCategory_Loop = iPK_DeviceCategory;
	while( iPK_DeviceCategory_Loop )
	{
		if( iPK_DeviceCategory_Loop==DEVICECATEGORY_Orbiter_CONST )
			bIsOrbiter=true;
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
				threaded_mysql_query(SQL,true);
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
			threaded_mysql_query(SQL,true);
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
				threaded_mysql_query(SQL,true);
			}
		}
	}

	if( sDeviceData.size() )
	{
		string::size_type pos=0;
		while(pos<sDeviceData.size())
		{
			int PK_DeviceData = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
			string sValue = StringUtils::Tokenize(sDeviceData,"|",pos);
			AssignDeviceData(PK_Device,PK_DeviceData,sValue);
		}
	}
	// Now we've got to see if we have any children we need to create automatically, first by the category
	CreateChildrenByCategory(PK_Device,iPK_DeviceCategory);
	CreateChildrenByTemplate(PK_Device,iPK_DeviceTemplate);
	ConfirmRelations(PK_Device);

	if( iPK_DeviceCategory==DEVICECATEGORY_Core_CONST )
	{
		string SQL = "SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=5"; // Get the default items to add if this is a Core
		PlutoSqlResult result_child_dev;
		if( ( result_child_dev.r=mysql_query_result( SQL ) ) )
		{
			while( row=mysql_fetch_row( result_child_dev.r ) )
			{
	g_pPlutoLogger->Write(LV_STATUS,"Found install wizard %d",atoi(row[0]));
				DoIt(0,atoi(row[0]),"","",0,"",PK_Device);
			}
		}
	}
	else if( iPK_DeviceCategory==DEVICECATEGORY_Media_Director_CONST )
	{
		string SQL = "SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=6"; // Get the default items to add if this is a Core
		PlutoSqlResult result_child_dev;
		if( ( result_child_dev.r=mysql_query_result( SQL ) ) )
		{
			while( row=mysql_fetch_row( result_child_dev.r ) )
			{
	g_pPlutoLogger->Write(LV_STATUS,"Found install wizard %d",atoi(row[0]));
				DoIt(0,atoi(row[0]),"","",0,"",PK_Device);
			}
		}
	}

	// If we weren't given a controlled via, try to find an appropriate one
	// Do this last since the controlled via may be something we added above
	if( !PK_Device_ControlledVia )
	{
		PK_Device_ControlledVia=DatabaseUtils::FindControlledViaCandidate(this,PK_Device,iPK_DeviceTemplate,iPK_Device_RelatedTo,m_iPK_Installation);
		g_pPlutoLogger->Write(LV_STATUS,"Searching for controlled via returned %d",PK_Device_ControlledVia);
		if( PK_Device_ControlledVia)
		{
			SQL = "UPDATE Device SET FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_ControlledVia) +
				" WHERE PK_Device=" + StringUtils::itos(PK_Device);
			if( threaded_mysql_query(SQL)<0 )
			{
				cout << "Error updating device controlled via" << endl;
				return 0;
			}
		}
	}

	// If this is an orbiter, and it's not an on-screen orbiter for the first device created (meaning we're still installing), then DCERouter should be running.  Send a regen message
	if( bIsOrbiter && DatabaseUtils::GetTopMostDevice(this,PK_Device)!=1 )
	{
		g_pPlutoLogger->Write(LV_STATUS,"it's an orbiter, time to generate");
		string SQL = "SELECT PK_Device FROM Device WHERE FK_Installation=" + StringUtils::itos(m_iPK_Installation) + " AND FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_Orbiter_Plugin_CONST);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		{
			int iPK_Device_OP = atoi(row[0]);
			string sCmd = "/usr/pluto/bin/MessageSend localhost 0 " + StringUtils::itos(iPK_Device_OP) + " 1 " +
				StringUtils::itos(COMMAND_Regen_Orbiter_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " " + StringUtils::itos(PK_Device);
			g_pPlutoLogger->Write(LV_STATUS,"Calling %s",sCmd.c_str());
			system(sCmd.c_str());
		}
		else
			cerr << "No Orbiter plugin" << endl;
	}

	if( iPK_Package && !m_bDontInstallPackages )
	{
		int PK_Device_TopMost=DatabaseUtils::GetTopMostDevice(this,PK_Device);
		if( PK_Device_TopMost==dceConfig.m_iPK_Device_Computer )
		{
			SQL = "SELECT Name FROM Package_Source JOIN RepositorySource ON FK_RepositorySource=PK_RepositorySource WHERE FK_RepositoryType=1 and FK_Package=" + StringUtils::itos(iPK_Package);
			PlutoSqlResult result;
			if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
			{
				if( m_bInstallPackagesInBackground )
				{
					ProcessUtils::SpawnApplication("/usr/pluto/bin/InstallNewDevice.sh",
						StringUtils::itos(PK_Device) + "\t" + (row[0] ? row[0] : "") + "\t","newdevice");
					g_pPlutoLogger->Write(LV_STATUS,"Executing /usr/pluto/bin/InstallNewDevice.sh %d %s",
						PK_Device,(row[0] ? row[0] : ""));
				}
				else
				{
					string sCmd = "/usr/pluto/bin/InstallNewDevice.sh " + StringUtils::itos(PK_Device) + " \"" + row[0] + "\"";
					g_pPlutoLogger->Write(LV_STATUS,"Executing %s",sCmd.c_str());
					system(sCmd.c_str());
				}
			}
			else
				g_pPlutoLogger->Write(LV_CRITICAL,"No installation info for package %d",iPK_Package);
		}
		else
			g_pPlutoLogger->Write(LV_STATUS,"Not installing package %d because %d top most is not %d",iPK_Package,PK_Device_TopMost,dceConfig.m_iPK_Device_Computer);
	}
	else
		g_pPlutoLogger->Write(LV_STATUS,"Not installing package %d",iPK_Package);


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
	PlutoSqlResult result_dt,result_related,result_related2,result_related3,result_related4,result_related5;
	MYSQL_ROW row,row3;

	int PK_Device_Core=0,PK_Device_DCERouter=0,PK_Device_Parent=0;
	string SQL = "SELECT PK_Device,FK_Device_ControlledVia FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST);
	if( (result_related4.r=mysql_query_result(SQL)) && (row3=mysql_fetch_row(result_related4.r)) )
	{
		PK_Device_DCERouter = row3[0] ? atoi(row3[0]) : 0;
		PK_Device_Core = row3[1] ? atoi(row3[1]) : 0;
	}

	SQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	if( (result_related5.r=mysql_query_result(SQL)) && (row3=mysql_fetch_row(result_related5.r)) )
		PK_Device_Parent = row3[0] ? atoi(row3[0]) : 0;

	int PK_DeviceTemplate=0;
	SQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	if( (result_dt.r=mysql_query_result(SQL)) && (row=mysql_fetch_row(result_dt.r)) )
		PK_DeviceTemplate=atoi(row[0]);

	if( !PK_DeviceTemplate )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Invalid device template for device %d",PK_Device);
		return;
	}

	// See if this requires dragging any other types of devices
	SQL = "SELECT FK_DeviceTemplate_Related,Value FROM DeviceTemplate_DeviceTemplate_Related WHERE FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate) + " ORDER BY FK_DeviceTemplate_Related";  // Use an order by so we can predict exactly in what order this will be done
	if( ( result_related.r=mysql_query_result( SQL ) ) )
	{
g_pPlutoLogger->Write(LV_STATUS,"Found result_related %d rows with %s",(int) result_related.r->row_count,SQL.c_str());
		while( row=mysql_fetch_row( result_related.r ) )
		{
			int iPK_DeviceTemplate_Related = atoi(row[0]);
			int iRelation= (row[1] ? atoi(row[1]) : 0);

			string sDeviceData;
			if( row[1] )
			{
				char *pDeviceData = strchr(row[1],'|');
				if( pDeviceData )
					sDeviceData = pDeviceData + 1;
			}

			// 1 = Sister device same controlled via
			// 2 = child of the core (dce router's parent)
			// 3 = plugin
			// 4 = anywhere on the same pc
			// 5 = my child
			if( (iRelation>=1 && iRelation<=3) || iRelation==5 )
			{
				// We need this device too.  See if we already have it
				if( iRelation==1 )
					SQL = "SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_Parent);
				else if( iRelation==2 )
					SQL = "SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_Core);
				else if( iRelation==3 )
					SQL = "SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_DCERouter);
				else if( iRelation==5 )
					SQL = "SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);

				if( (result_related2.r=mysql_query_result(SQL)) && result_related2.r->row_count>0 )
					continue;  // It's ok.  We got it

				// That device doesn't exist.  We need to create it
				if( iRelation==1 && !bOnlyAddDevicesOnCore )  // It's a sister device
					DoIt(0,iPK_DeviceTemplate_Related,"","",PK_Device_Parent,sDeviceData);
				else if( iRelation==2 )  // It's a child of the core (ie DCERouter's parent)
					DoIt(0,iPK_DeviceTemplate_Related,"","",PK_Device_Core,sDeviceData);
				else if( iRelation==3 )  // It's a plugin (ie child of DCERouter)
					DoIt(0,iPK_DeviceTemplate_Related,"","",PK_Device_DCERouter,sDeviceData);
				else if( iRelation==5 )  // It's my child
					DoIt(0,iPK_DeviceTemplate_Related,"","",PK_Device,sDeviceData);
			}
			else if( iRelation==4 )
			{
				map<int,int> mapDeviceTree;
				DatabaseUtils::GetAllDevicesInTree(this,PK_Device,mapDeviceTree);

				bool bFound=false;
				for(map<int,int>::iterator it=mapDeviceTree.begin();it!=mapDeviceTree.end();++it)
				{
					if( it->second==iPK_DeviceTemplate_Related )
					{
						bFound=true;
						break;
					}
				}
				if( bFound )
					continue;

				DoIt(0,iPK_DeviceTemplate_Related,"","",0,sDeviceData,PK_Device);
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

void CreateDevice::AssignDeviceData(int PK_Device,int PK_DeviceData,string sValue)
{
	string SQL = "SELECT FK_Device FROM Device_DeviceData WHERE FK_Device="
		+ StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);

	PlutoSqlResult result1;
	result1.r=mysql_query_result( SQL );
	if( !result1.r || result1.r->row_count==0 )
		threaded_mysql_query("INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) "
			"VALUES(" + StringUtils::itos(PK_Device) + "," + StringUtils::itos(PK_DeviceData) + ");",true);

	threaded_mysql_query("UPDATE Device_DeviceData SET IK_DeviceData='"
		+ StringUtils::SQLEscape(sValue) + "' WHERE "
		"FK_Device=" + StringUtils::itos(PK_Device) + 
		" AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData) );
}
