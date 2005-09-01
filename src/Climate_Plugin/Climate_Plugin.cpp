/*
 Climate_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

 
//<-dceag-d-b->
#include "Climate_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Climate_Plugin::Climate_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Climate_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = NULL;
}

//<-dceag-getconfig-b->
bool Climate_Plugin::GetConfig()
{
	if( !Climate_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return false;
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Climate_Plugin::~Climate_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Climate_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Climate_Plugin::ClimateScenariosGrid))
		,DATAGRID_Climate_Scenarios_CONST);

    RegisterMsgInterceptor(( MessageInterceptorFn )( &Climate_Plugin::ClimateCommand ), 0, 0, 0, DEVICECATEGORY_Climate_Device_CONST, 0, 0 );

	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Climate_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Climate_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable *Climate_Plugin::ClimateScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	vector<Row_CommandGroup *> vectRowCommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows( COMMANDGROUP_FK_ARRAY_FIELD + string("=") + StringUtils::itos(ARRAY_Climate_Scenarios_CONST) + " AND " 
			+ COMMANDGROUP_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pRouter->iPK_Installation_get()),&vectRowCommandGroup );
	for(size_t s=0;s<vectRowCommandGroup.size();++s)
	{
		Row_CommandGroup *pRow_CommandGroup = vectRowCommandGroup[s];
		pCell = new DataGridCell(pRow_CommandGroup->Description_get(),StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()));
		pDataGrid->SetData(0,(int) s,pCell);
	}

	return pDataGrid;
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-createinst-b->!
void Climate_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	switch(iFloorplanObjectType)
	{
	case FLOORPLANOBJECTTYPE_CLIMATE_THERMOSTAT_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpThermostatControls_CONST;
		break;
	case FLOORPLANOBJECTTYPE_CLIMATE_THERMOMETER_CONST:
		PK_DesignObj_Toolbar=0;
		break;
	case FLOORPLANOBJECTTYPE_CLIMATE_WEATHER_STATION_CONST:
		PK_DesignObj_Toolbar=0;
		break;
	case FLOORPLANOBJECTTYPE_CLIMATE_DAMPER_CONST:
		PK_DesignObj_Toolbar=0;
		break;
	case FLOORPLANOBJECTTYPE_CLIMATE_POOL_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpPoolControls_CONST;
		break;
	case FLOORPLANOBJECTTYPE_CLIMATE_SPRINKLER_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpSprinklerControls_CONST;
		break;
	};
	if( (OSD=pDeviceData_Router->m_sState_get())=="OFF" )
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_OFF_CONST;
	else
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_COOLING_CONST;
}

bool Climate_Plugin::ClimateCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	// This only runs as a plug-in so we can safely cast it
	class DeviceData_Router *pDevice_RouterTo = (class DeviceData_Router *) pDeviceTo;
	PreprocessClimateMessage(pDevice_RouterTo,pMessage);

	class DeviceData_Router *pDevice_RouterFrom = (class DeviceData_Router *) pDeviceFrom;
	PreprocessClimateMessage(pDevice_RouterFrom,pMessage);

	if( pMessage->m_sPK_Device_List_To.length() ) 
	{
		int PK_Device;  size_t pos=0;
		while( true )
		{
			PK_Device=atoi(StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos).c_str());
			DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			PreprocessClimateMessage(pDeviceData_Router,pMessage);
			if( pos>=pMessage->m_sPK_Device_List_To.length() || pos==string::npos )
				break;
		}
	}

	return true;
}

void Climate_Plugin::PreprocessClimateMessage(DeviceData_Router *pDevice,Message *pMessage)
{
	if( !pDevice || !pMessage || !pDevice->WithinCategory(DEVICECATEGORY_Climate_Device_CONST) )
		return;

	// The State is in the format ON|OFF/SET TEMP (CURRENT TEMP)
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
	{
		if( pMessage->m_dwID==COMMAND_Set_Level_CONST )
		{
			string sLevel = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST];
			if( sLevel.size()==0 )
				pMessage->m_dwID=COMMAND_Generic_Off_CONST;
			else
			{
				int iLevel = atoi(sLevel.c_str());
				if( sLevel[0]=='+' )
					iLevel = min(100, GetClimateLevel(pDevice,0)+iLevel);
				else if( sLevel[0]=='-' )
					iLevel = max(0, GetClimateLevel(pDevice,0)+iLevel);

				if( iLevel==0 )
					pMessage->m_dwID=COMMAND_Generic_Off_CONST;
				else
				{
					pDevice->m_sState_set("ON/" + StringUtils::itos(iLevel) + GetTemperature(pDevice));
					pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST] = StringUtils::itos(iLevel);
				}
			}
		}

		if( pMessage->m_dwID==COMMAND_Generic_On_CONST )
			pDevice->m_sState_set("ON/" + StringUtils::itos(GetClimateLevel(pDevice,100)) + GetTemperature(pDevice));
		else if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
			pDevice->m_sState_set("OFF/" + StringUtils::itos(GetClimateLevel(pDevice,0)) + GetTemperature(pDevice));
		else if( pMessage->m_dwID==COMMAND_Set_HeatCool_CONST )
		{
			string sState = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
			if( sState=="H" )
				pDevice->m_sState_set("HEAT/" + StringUtils::itos(GetClimateLevel(pDevice,0)) + GetTemperature(pDevice));
			else if( sState=="C" )
				pDevice->m_sState_set("COOL/" + StringUtils::itos(GetClimateLevel(pDevice,0)) + GetTemperature(pDevice));
			else
				pDevice->m_sState_set("AUTO/" + StringUtils::itos(GetClimateLevel(pDevice,0)) + GetTemperature(pDevice));
		}
	}
	else if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT && pMessage->m_dwID==EVENT_Temperature_Changed_CONST )
	{
		// Replace the current temp
		string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
		string sCurrentState = pDevice->m_sState_get();
		string::size_type pos = sCurrentState.find('(');
		if( pos!=string::npos )
			sCurrentState = sCurrentState.substr(0,pos-1); // Get rid of the current temp
		pDevice->m_sState_set(sCurrentState + " (" + sLevel + ")");
	}
}

int Climate_Plugin::GetClimateLevel(DeviceData_Router *pDevice,int iLevel_Default)
{
	string sState = pDevice->m_sState_get();
	string::size_type pos = sState.find("/");
	if( pos<sState.size()-1 && pos!=string::npos )
		return atoi(sState.substr(pos+1).c_str());
	else
		return iLevel_Default;
}

string Climate_Plugin::GetTemperature(DeviceData_Router *pDevice)
{
	// Replace the current temp
	string sCurrentState = pDevice->m_sState_get();
	string::size_type pos = sCurrentState.find('(');
	if( pos!=string::npos )
		return sCurrentState.substr(pos); // Get rid of the current temp
	else
		return "";
}
