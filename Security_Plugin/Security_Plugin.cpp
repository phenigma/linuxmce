/*
 Security_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

/**
 *
 * @file  Security_Plugin.cpp
 * @brief source file for the Security_Plugin class
 *
 */
 
//<-dceag-d-b->
#include "Security_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <sstream>

#include "DCERouter/DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_HouseMode.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"

#include "pluto_security/Table_Alert.h"
#include "pluto_security/Table_AlertType.h"
#include "pluto_security/Table_ModeChange.h"

// Alarm Types
#define PROCESS_ALERT		1

//<-dceag-const-b->
Security_Plugin::Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Security_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_SecurityMutex("security")
{
	m_SecurityMutex.Init(NULL);

	m_pDatabase_pluto_main = new Database_pluto_main( );
	if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		m_bQuit=true;
		return;
	}

	m_pDatabase_pluto_security = new Database_pluto_security( );
	if( !m_pDatabase_pluto_security->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_security", m_pRouter->iDBPort_get( ) ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		m_bQuit=true;
		return;
	}

	m_pDeviceData_Router_this = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
	m_pRow_ModeChange_Last=NULL;

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(4);      //4 = number of worker threads
}

//<-dceag-dest-b->
Security_Plugin::~Security_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Security_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Datagrid_Plugin_CONST );

	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Media grids cannot find datagrid handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}

	m_pDatagrid_Plugin=( Datagrid_Plugin * ) pListCommand_Impl->front( );

	m_pOrbiter_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Orbiter_Plugin_CONST );

	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter grids cannot find Orbiter plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}

	m_pOrbiter_Plugin=( Orbiter_Plugin * ) pListCommand_Impl->front( );

	m_pDatagrid_Plugin->RegisterDatagridGenerator( 
		new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Security_Plugin::SecurityScenariosGrid ) )
		, DATAGRID_Security_Scenarios_CONST );

    RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::SensorTrippedEvent) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Sensor_Tripped_CONST);

	return Connect( ); 
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
void Security_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Security_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable *Security_Plugin::SecurityScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	vector<Row_CommandGroup *> vectRowCommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get( )->GetRows( COMMANDGROUP_FK_ARRAY_FIELD + string( "=" ) + StringUtils::itos( ARRAY_Security_Scenarios_CONST ) + " AND " 
			+ COMMANDGROUP_FK_INSTALLATION_FIELD + "=" + StringUtils::itos( m_pRouter->iPK_Installation_get( ) ), &vectRowCommandGroup );
	for( size_t s=0;s<vectRowCommandGroup.size( );++s )
	{
		Row_CommandGroup *pRow_CommandGroup = vectRowCommandGroup[s];
		pCell = new DataGridCell( pRow_CommandGroup->Description_get( ), StringUtils::itos( pRow_CommandGroup->PK_CommandGroup_get( ) ) );
		pDataGrid->SetData( 0, ( int ) s, pCell );
	}

	return pDataGrid;
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c19-b->

	/** @brief COMMAND: #19 - Set House Mode */
	/** Sets the current security setting (at home, away, etc.) for the house */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #18 Errors */
			/** not used by the Orbiter.  This is used only when sending the action to the core. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void Security_Plugin::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sErrors,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
//<-dceag-c19-e->
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	int PK_HouseMode = atoi(sValue_To_Assign.c_str());
	if( PK_HouseMode<HOUSEMODE_Unarmed_at_home_CONST || PK_HouseMode>HOUSEMODE_Armed_Extended_away_CONST )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Attempt to set invalid house mode: %d",PK_HouseMode);
		return;
	}

	// The password can either be the password or teh PIN code, and either plain text or md5.  iPK_Users is optional
	ostringstream sql;
	sql << "SELECT PK_Users,Username FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		<< m_pRouter->iPK_Installation_get() << " AND userCanChangeHouseMode=1 AND (Password=" << sPassword 
		<< " OR Password=" << m_pRouter->md5(sPassword) << " OR PINCode=" << sPassword << " OR PINCode=" << m_pRouter->md5(sPassword) << ")";
	if( iPK_Users )
		sql << " AND PK_Users=" << iPK_Users;

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pRouter->mysql_query_result( sql.str( ) ) )==0 || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING,"User: %d failed to set house mode: %d",iPK_Users,PK_HouseMode);
		DCE::CMD_Goto_Screen CMD_Goto_Screen( 0, pMessage->m_dwPK_Device_From, 0, StringUtils::itos(DESIGNOBJ_mnuMain_CONST), "", "", false );
		SendCommand( CMD_Goto_Screen );
		return;		
	}

	bool bFailed=false; // Will be set to true if any of the devices fail to get set
	map<int,DeviceData_Router *> mapResetDevices; // Keep track of the devices we change so we can reset any pending alarms that are affected

	if( iPK_DeviceGroup )
	{
		DeviceGroup *pDeviceGroup = m_pRouter->m_mapDeviceGroup_Find(iPK_DeviceGroup);
		if( !pDeviceGroup )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Trying to set house mode with invalid device group: %d",iPK_DeviceGroup);
			return;
		}

		for(size_t s=0;s<pDeviceGroup->m_vectDeviceData_Base.size();++s)
		{
			DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceGroup->m_vectDeviceData_Base[s];  // it's coming from the router, so it's safe to cast it
			mapResetDevices[pDevice->m_dwPK_Device] = pDevice;
			if( !SetHouseMode(pDevice,iPK_Users,PK_HouseMode,sHandling_Instructions) )
			{
				bFailed=true;
				break;
			}
		}
	}
	else
	{
		ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Security_Device_CONST);
		if( !pListDeviceData_Router )
			g_pPlutoLogger->Write(LV_CRITICAL,"Trying to set house mode with no security devices");
		else
		{
			for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
			{
				DeviceData_Router *pDevice = *it;
				mapResetDevices[pDevice->m_dwPK_Device] = pDevice;
				if( !SetHouseMode(pDevice,iPK_Users,PK_HouseMode,sHandling_Instructions) )
				{
					bFailed=true;
					break;
				}
			}
		}
	}
	if( bFailed )
	{
		DCE::CMD_Goto_Screen CMD_Goto_Screen( 0, pMessage->m_dwPK_Device_From, 0, StringUtils::itos(DESIGNOBJ_mnuMain_CONST), "", "", false );
		SendCommand( CMD_Goto_Screen );
	}
	for(vector<Row_Alert *>::iterator it=m_vectPendingAlerts.begin();it!=m_vectPendingAlerts.end();++it)
	{
		Row_Alert *pRow_Alert = *it;
		if( mapResetDevices.find(pRow_Alert->EK_Device_get())!=mapResetDevices.end() )
		{
			// We've reset this device
			pRow_Alert->ResetTime_set(StringUtils::SQLDateTime(time(NULL)));
			pRow_Alert->Table_Alert_get()->Commit();
			m_vectPendingAlerts.erase(it);
		}
	}
	EVENT_Reset_Alarm();
}

bool Security_Plugin::SetHouseMode(DeviceData_Router *pDevice,int iPK_Users,int PK_HouseMode,string sHandlingInstructions) 
{
	string NewMode = GetModeString(PK_HouseMode);

	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);

	if( Bypass=="PERMBYPASS" )
	{
		pDevice->m_sState_set(NewMode + "," + Bypass);
		return true; // We're bypassing this one
	}

	if( SensorIsTripped(PK_HouseMode,pDevice) )
	{
		if( sHandlingInstructions!="W" && sHandlingInstructions!="B" )
			return false;  // Sensors are tripped.  The user needs to figure out what to do

		if( sHandlingInstructions=="W" )
			Bypass = "WAIT";
		else if( sHandlingInstructions=="B" )
			Bypass = "BYPASS";
		else
			Bypass = "";
	}
	else
		Bypass = "";

	pDevice->m_sState_set(NewMode + "," + Bypass);

	m_pDeviceData_Router_this->m_sState_set("NewMode");

	Row_ModeChange *pRow_ModeChange = m_pDatabase_pluto_security->ModeChange_get()->AddRow();
	pRow_ModeChange->EK_HouseMode_set(PK_HouseMode);
	pRow_ModeChange->ChangeTime_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_ModeChange->EK_Users_set(iPK_Users);
	pRow_ModeChange->Table_ModeChange_get()->Commit();
	m_pRow_ModeChange_Last=pRow_ModeChange;

	return true;
}

bool Security_Plugin::SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice)
{
	if( pDevice->m_sState_get()!="TRIPPED" )
		return false;

	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice);
	return PK_AlertType==ALERTTYPE_Security_CONST || PK_AlertType==ALERTTYPE_Fire_CONST;
}


bool Security_Plugin::SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceFrom;

	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	if( !pDevice || pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Security_Device_CONST )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Receieved a sensor trip from an unrecognized device: %d",pMessage->m_dwPK_Device_From);
		return false;
	}

	bool m_bTripped = pMessage->m_mapParameters[EVENTPARAMETER_Tripped_CONST]=="1";
	pDevice->m_sStatus_set(m_bTripped ? "NORMAL" : "TRIPPED");

	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);

	int PK_HouseMode = GetModeID(m_pDeviceData_Router_this->m_sState_get());

	if( m_bTripped==false && Bypass=="WAIT" )
	{
		pDevice->m_sState_set(Mode);
		return true;
	}

	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice);
	Row_AlertType *pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(PK_AlertType);
	if( m_pRow_ModeChange_Last && pRow_AlertType && StringUtils::SQLDateTime(m_pRow_ModeChange_Last->ChangeTime_get()) + pRow_AlertType->ExitDelay_get() > time(NULL) )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Still in exit delay");
		return true;
	}

	if( m_bTripped && Bypass!="BYPASS" && Bypass!="PERMBYPASS" && SensorIsTripped(PK_HouseMode,pDevice) )
	{
		Row_Alert *pRow_Alert = m_pDatabase_pluto_security->Alert_get()->AddRow();
		pRow_Alert->FK_AlertType_set(PK_AlertType);
		pRow_Alert->EK_Device_set(pDevice->m_dwPK_Device);
		pRow_Alert->DetectionTime_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_Alert->ExpirationTime_set(StringUtils::SQLDateTime(time(NULL) + pRow_AlertType ? pRow_AlertType->ExitDelay_get() : 0 ));
		pRow_Alert->Table_Alert_get()->Commit();
		m_vectPendingAlerts.push_back(pRow_Alert);
		m_pAlarmManager->AddAbsoluteAlarm(StringUtils::SQLDateTime(pRow_Alert->ExpirationTime_get()),this,PROCESS_ALERT,&pRow_Alert);
	}

	return true;
}

int Security_Plugin::GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice)
{
	return ALERTTYPE_Security_CONST;
}

void Security_Plugin::SecurityBreach(DeviceData_Router *pDevice)
{
	EVENT_Security_Breach(pDevice->m_dwPK_Device);
}

void Security_Plugin::FireAlarm(DeviceData_Router *pDevice)
{
	EVENT_Fire_Alarm(pDevice->m_dwPK_Device);
}

void Security_Plugin::AlarmCallback(int id, void* param)
{
}

