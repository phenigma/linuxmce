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

#include "DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"

#include "Orbiter_Plugin/Orbiter_Plugin.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_HouseMode.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Table_Text_LS.h"

#include "pluto_security/Table_Alert.h"
#include "pluto_security/Table_Alert_Device.h"
#include "pluto_security/Table_AlertType.h"
#include "pluto_security/Table_ModeChange.h"

// Alarm Types
#define PROCESS_ALERT					1
#define PROCESS_COUNTDOWN_BEFORE_ALARM	2
#define PROCESS_COUNTDOWN_BEFORE_ARMED	3

// Non-alert types
#define ALERTTYPE_DONOTHING				0
#define ALERTTYPE_ANNOUNCMENT			-1
#define ALERTTYPE_PHOTO					-2

#define RAPID_COUNTDOWN					10  // For all countdowns, once it gets below this # of seconds, it will only state the number alone

#define COUNTDOWN_INTERVAL_1			10  // How many seconds to wait when making the initial, full announcement
#define COUNTDOWN_INTERVAL_2			5	// How many seconds to wait for subsequence announcement, while still not in RAPID_COUNTDOWN
#define COUNTDOWN_INTERVAL_3			1	// How many seconds to wait for the RAPID_COUNTDOWN

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Security_Plugin::Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Security_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_SecurityMutex("security")
{
	m_tExitTime=0;
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_SecurityMutex.Init(&m_MutexAttr);

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

	// Get the last mode change for each zone
	string sql = "select max(PK_ModeChange),EK_DeviceGroup FROM ModeChange GROUP BY EK_DeviceGroup";
	// Don't worry about multi-threading--this won't be a concern until we register

	PlutoSqlResult result;
    MYSQL_ROW row;
	if( mysql_query(m_pDatabase_pluto_security->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_security->m_pMySQL)) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
			m_mapRow_ModeChange_Last[row[1] ? atoi(row[1]) : 0] = m_pDatabase_pluto_security->ModeChange_get()->GetRow( atoi(row[0]) );
	}

	m_pDeviceData_Router_this = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);

	Row_Text_LS *pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_alarm_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sCountdownBeforeAlarm = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_alarm_CONST,1);
		if( pRow_Text_LS )
			m_sCountdownBeforeAlarm = pRow_Text_LS->Description_get();
		else
			m_sCountdownBeforeAlarm = "Alarm will sound in <%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_armed_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sCountdownBeforeArmed = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_armed_CONST,1);
		if( pRow_Text_LS )
			m_sCountdownBeforeArmed = pRow_Text_LS->Description_get();
		else
			m_sCountdownBeforeArmed = "Alarm will be active in <%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_alarm_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sShortCountdownBeforeAlarm = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_alarm_CONST,1);
		if( pRow_Text_LS )
			m_sShortCountdownBeforeAlarm = pRow_Text_LS->Description_get();
		else
			m_sShortCountdownBeforeAlarm = "<%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_armed_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sShortCountdownBeforeArmed = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_armed_CONST,1);
		if( pRow_Text_LS )
			m_sShortCountdownBeforeArmed = pRow_Text_LS->Description_get();
		else
			m_sShortCountdownBeforeArmed = "<%=SEC%> seconds.";
	}

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads

	DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Text_To_Speech_CONST);
	m_PK_Device_TextToSpeach = pDeviceData_Base ? pDeviceData_Base->m_dwPK_Device : 0;

	FileUtils::MakeDir(DATA_Get_Path()); // Just be sure it's here
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Security_Plugin::~Security_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_security;
	
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
    RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::OrbiterRegistered) ,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Orbiter_Registered_CONST);

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
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void Security_Plugin::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
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
		<< m_pRouter->iPK_Installation_get() << " AND userCanChangeHouseMode=1 AND (Password='" << sPassword 
		<< "' OR Password='" << m_pRouter->md5(sPassword) << "' OR PINCode='" << sPassword << "' OR PINCode='" << m_pRouter->md5(sPassword) << "')";
	if( iPK_Users )
		sql << " AND PK_Users=" << iPK_Users;

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pRouter->mysql_query_result( sql.str( ) ) )==0 || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING,"User: %d failed to set house mode: %d",iPK_Users,PK_HouseMode);
		DCE::CMD_Set_Text CMD_Set_Text( 0, pMessage->m_dwPK_Device_From, "", "***Invalid PIN***", TEXT_PIN_Code_CONST );
		DCE::CMD_Set_Variable CMD_Set_Variable( 0, pMessage->m_dwPK_Device_From, VARIABLE_PasswordPin_CONST, "" );
		CMD_Set_Text.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
		SendCommand( CMD_Set_Text );
		return;		
	}

	m_tExitTime=0;
	m_pAlarmManager->CancelAlarmByType(PROCESS_COUNTDOWN_BEFORE_ARMED);

	bool bFailed=false; // Will be set to true if any of the devices fail to get set
	bool bSensorsActive=false; // Will be set to true if any sensors are set to active/arme
	map<int,DeviceData_Router *> mapResetDevices; // Keep track of the devices we change so we can reset any pending alarms that are affected

	DeviceGroup *pDeviceGroup=NULL;
	if( iPK_DeviceGroup )
	{
		pDeviceGroup = m_pRouter->m_mapDeviceGroup_Find(iPK_DeviceGroup);
		if( !pDeviceGroup )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Trying to set house mode with invalid device group: %d",iPK_DeviceGroup);
			return;
		}

		for(size_t s=0;s<pDeviceGroup->m_vectDeviceData_Base.size();++s)
		{
			DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceGroup->m_vectDeviceData_Base[s];  // it's coming from the router, so it's safe to cast it
			if( pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Security_Device_CONST )
				continue;
			mapResetDevices[pDevice->m_dwPK_Device] = pDevice;
			if( !SetHouseMode(pDevice,iPK_Users,PK_HouseMode,sHandling_Instructions) )
			{
				bFailed=true;
				break;
			}
			if( !bSensorsActive && GetAlertType(PK_HouseMode,pDevice)>0 )
				bSensorsActive=true;
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
				if( !bSensorsActive && GetAlertType(PK_HouseMode,pDevice)>0 )
					bSensorsActive=true;
			}
		}
	}
	if( bFailed )
	{
		DCE::CMD_Goto_Screen CMD_Goto_Screen( 0, pMessage->m_dwPK_Device_From, 0, StringUtils::itos(DESIGNOBJ_mnuSensorsNotReady_CONST), "", "", false, false );
		SendCommand( CMD_Goto_Screen );
		return;
	}

	m_pDeviceData_Router_this->m_sState_set(GetModeString(PK_HouseMode));
	Row_AlertType *pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Security_CONST);
	string sAlerts = AlertsSinceLastChange(iPK_DeviceGroup);

	// Set the house indicators on either all orbiters, or just orbiters in this group if we only changed a zone
	if( pDeviceGroup )
	{
		// Only set the house mode on the orbiters within this group
		for(size_t s=0;s<pDeviceGroup->m_vectDeviceData_Base.size();++s)
		{
			DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceGroup->m_vectDeviceData_Base[s];  // it's coming from the router, so it's safe to cast it
			if( pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_Orbiter_CONST )
			{
				DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pDevice->m_dwPK_Device,StringUtils::itos(PK_HouseMode),"housemode");
				SendCommand(CMD_Set_Bound_Icon);
			}
		}
	}
	else
	{
		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos(PK_HouseMode),"housemode");
			SendCommand(CMD_Set_Bound_Icon);
		}
	}
	
	DCE::CMD_Goto_Screen CMD_Goto_Screen( 0, pMessage->m_dwPK_Device_From, 0, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), "", "", false, true );

	DCE::CMD_Set_Text CMD_Set_Text( 0, pMessage->m_dwPK_Device_From, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), pRow_AlertType->ExitDelay_get() && bSensorsActive ? "<%=CD%> seconds" : "IMMEDIATELY",
		TEXT_House_Mode_Time_CONST );
	CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text.m_pMessage);

	g_pPlutoLogger->Write(LV_STATUS,"Set House Mode.  Sensors active: %d exit delay: %d",(int) bSensorsActive,pRow_AlertType->ExitDelay_get());
	if( pRow_AlertType->ExitDelay_get() )
	{
		DCE::CMD_Select_Object CMD_Select_Object( 0, pMessage->m_dwPK_Device_From, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST),StringUtils::itos(pRow_AlertType->ExitDelay_get()) );
		CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Select_Object.m_pMessage);

		if( bSensorsActive && DATA_Get_PK_Device().size() )
		{
			m_bExitDelay_New=true;
			m_tExitTime=time(NULL) + pRow_AlertType->ExitDelay_get();
			m_pAlarmManager->AddRelativeAlarm(0,this,PROCESS_COUNTDOWN_BEFORE_ARMED,NULL);
		}
	}

	DCE::CMD_Set_Text CMD_Set_TextAlert( 0, pMessage->m_dwPK_Device_From, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), sAlerts,
		TEXT_Alerts_Placeholder_CONST );
	CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_TextAlert.m_pMessage);

	SendCommand( CMD_Goto_Screen );

	Row_ModeChange *pRow_ModeChange = m_pDatabase_pluto_security->ModeChange_get()->AddRow();
	pRow_ModeChange->EK_HouseMode_set(PK_HouseMode);
	pRow_ModeChange->ChangeTime_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_ModeChange->EK_Users_set(iPK_Users);
	pRow_ModeChange->Table_ModeChange_get()->Commit();
	m_mapRow_ModeChange_Last[pDeviceGroup ? pDeviceGroup->m_dwPK_DeviceGroup : 0]=pRow_ModeChange;

	string NewMode = GetModeString(PK_HouseMode);
	for(map<int,DeviceData_Router *>::iterator it=mapResetDevices.begin();it!=mapResetDevices.end();++it)
	{
		DeviceData_Router *pDevice = (*it).second;

		string State = pDevice->m_sState_get();
		string::size_type pos=0;
		string Mode = StringUtils::Tokenize(State,",",pos);
		string Bypass = StringUtils::Tokenize(State,",",pos);
		pDevice->m_sState_set(NewMode + "," + Bypass + "," + StringUtils::itos(pRow_ModeChange->PK_ModeChange_get()));
	}

	for(vector<Row_Alert *>::iterator it=m_vectPendingAlerts.begin();it!=m_vectPendingAlerts.end();)
	{
		Row_Alert *pRow_Alert = *it;
		if( mapResetDevices.find(pRow_Alert->EK_Device_get())!=mapResetDevices.end() )
		{
			// We've reset this device
			pRow_Alert->ResetTime_set(StringUtils::SQLDateTime(time(NULL)));
			pRow_Alert->Table_Alert_get()->Commit();
			it=m_vectPendingAlerts.erase(it);
		}
		else 
			it++;
	}
	EVENT_Reset_Alarm();
}

string Security_Plugin::AlertsSinceLastChange(int PK_DeviceGroup)
{
	Row_ModeChange *pRow_ModeChange = m_mapRow_ModeChange_Last_Find(PK_DeviceGroup);
	if( !pRow_ModeChange )
		return "";

	vector<Row_Alert *> vectRow_Alert;

	return "todo";
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

	return true;
}

bool Security_Plugin::SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice)
{
	if( pDevice->m_sStatus_get()!="TRIPPED" )
		return false;

	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice);
	return PK_AlertType>0;
}

bool Security_Plugin::SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceFrom;

	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	if( !pDevice || (pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Security_Device_CONST && pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Generic_IO_CONST) )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Receieved a sensor trip from an unrecognized device: %d",pMessage->m_dwPK_Device_From);
		return false;
	}

	bool bTripped = pMessage->m_mapParameters[EVENTPARAMETER_Tripped_CONST]=="1";
	pDevice->m_sStatus_set(bTripped ? "TRIPPED" : "NORMAL");

	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);
	string sPK_ModeChange = StringUtils::Tokenize(State,",",pos);

	int PK_HouseMode = GetModeID(m_pDeviceData_Router_this->m_sState_get());

	if( bTripped==false )
	{
		if( Bypass=="WAIT" )
			pDevice->m_sState_set(Mode + ",," + sPK_ModeChange );
		return true;
	}

	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice);
	if( PK_AlertType==ALERTTYPE_DONOTHING )
		return true;
	else if( PK_AlertType<0 )
	{
		// It's some type of information alert
		if( PK_AlertType==ALERTTYPE_ANNOUNCMENT )
			AnnounceAlert(pDevice);
		LogAlert( m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Information_CONST),pDevice);
	}

	Row_AlertType *pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(PK_AlertType);

	g_pPlutoLogger->Write(LV_STATUS,"Alert type is %d %p, delay %s #pending: %d",
		PK_AlertType,pRow_AlertType,sPK_ModeChange.c_str(),(int) m_vectPendingAlerts.size());

	if( sPK_ModeChange.length() && pRow_AlertType->ExitDelay_get() )
	{
		Row_ModeChange *pRow_ModeChange = m_pDatabase_pluto_security->ModeChange_get()->GetRow(atoi(sPK_ModeChange.c_str()));
		time_t tChange = StringUtils::SQLDateTime(pRow_ModeChange->ChangeTime_get());
time_t tnow = time(NULL);
		g_pPlutoLogger->Write(LV_STATUS,"Alert was %d-%d=%d seconds ago",
			(int) tnow, (int) tChange, (int) tChange-tnow);
		if( pRow_ModeChange && tChange + pRow_AlertType->ExitDelay_get() > time(NULL) )
		{
			g_pPlutoLogger->Write(LV_STATUS,"Still in exit delay");
			return true;
		}
	}

	if( Bypass!="BYPASS" && Bypass!="PERMBYPASS" && SensorIsTripped(PK_HouseMode,pDevice) )
	{
		Row_Alert *pRow_Alert=LogAlert(pRow_AlertType,pDevice);
		if( pRow_Alert )
		{
			m_pAlarmManager->AddRelativeAlarm(0,this,PROCESS_COUNTDOWN_BEFORE_ALARM,(void *) pRow_Alert);
			m_pAlarmManager->AddAbsoluteAlarm(StringUtils::SQLDateTime(pRow_Alert->ExpirationTime_get()),this,PROCESS_ALERT,(void *) pRow_Alert);
		}
	}

	return true;
}

int Security_Plugin::GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice)
{
	// 0=Do nothing, 1=fire alert, 2=make announcement, 3=snap phto
	int Alert=0;
	string sAlertData = pDevice->m_mapParameters[DEVICEDATA_Alert_CONST];
	//monitor mode, disarmed, armed - away, armed - at home, sleeping, entertaining, extended away
	sAlertData="0,0,1,1,1,1,1";
	if( sAlertData.length()<13 )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Alert data not specified for %d",pDevice->m_dwPK_Device);
		return 0;
	}
	Alert = sAlertData[PK_HouseMode * 2]-48;
	if( Alert==0 )
		return ALERTTYPE_DONOTHING;
	else if( Alert==2 )
		return ALERTTYPE_ANNOUNCMENT;
	else if( Alert==3 )
		return ALERTTYPE_PHOTO;
	return atoi( pDevice->m_mapParameters[DEVICEDATA_EK_AlertType_CONST].c_str() );
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
	if( id==PROCESS_ALERT )
		ProcessAlert((Row_Alert *) param);
	else if( id==PROCESS_COUNTDOWN_BEFORE_ALARM )
		ProcessCountdown(id,(Row_Alert *) param);
	else if( id==PROCESS_COUNTDOWN_BEFORE_ARMED )
		ProcessCountdown(id,NULL);
}

void Security_Plugin::ProcessAlert(Row_Alert *pRow_Alert)
{
	g_pPlutoLogger->Write(LV_STATUS,"Processing alert %d type %d",pRow_Alert->PK_Alert_get(),pRow_Alert->FK_AlertType_get());
	if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Security_CONST )
		EVENT_Security_Breach(pRow_Alert->EK_Device_get());
	else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Fire_CONST )
		EVENT_Fire_Alarm(pRow_Alert->EK_Device_get());
	else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Air_Quality_CONST )
		EVENT_Air_Quality(pRow_Alert->EK_Device_get());
}

void Security_Plugin::ProcessCountdown(int id,Row_Alert *pRow_Alert)
{
	if( DATA_Get_PK_Device().size()==0 || !m_PK_Device_TextToSpeach )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Cannot make security announcements.  Need to have a Text_To_Speach device %d and media devices %s",m_PK_Device_TextToSpeach,DATA_Get_PK_Device().c_str());
		return; // Nothing to do
	}

	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	int iRemainingSeconds;
	if( pRow_Alert )
		iRemainingSeconds = StringUtils::SQLDateTime(pRow_Alert->ExpirationTime_get())-time(NULL);
	else
		iRemainingSeconds = m_tExitTime-time(NULL);

	if( iRemainingSeconds<1 )
		return;  // It already expired

	bool bRapidCountdown = iRemainingSeconds<RAPID_COUNTDOWN;
	string sAnnouncement;
	if( (!pRow_Alert && m_bExitDelay_New) || (pRow_Alert && m_mapAlarm_New[pRow_Alert->PK_Alert_get()]) )
	{
		if( pRow_Alert )
			sAnnouncement = StringUtils::Replace(m_sCountdownBeforeAlarm,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
		else
			sAnnouncement = StringUtils::Replace(m_sCountdownBeforeArmed,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
	}
	else if( bRapidCountdown )
		sAnnouncement = StringUtils::itos(iRemainingSeconds);
	else
	{
		if( pRow_Alert )
			sAnnouncement = StringUtils::Replace(m_sShortCountdownBeforeAlarm,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
		else
			sAnnouncement = StringUtils::Replace(m_sShortCountdownBeforeArmed,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
	}

	SayToDevices(sAnnouncement,NULL);

	if(	(!pRow_Alert && m_bExitDelay_New) || (pRow_Alert && m_mapAlarm_New[pRow_Alert->PK_Alert_get()]) )
	{
		if( !pRow_Alert )
			m_bExitDelay_New=false;
		else
			m_mapAlarm_New[pRow_Alert->PK_Alert_get()]=false;
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_1,this,id,pRow_Alert);
	}
	else if( !bRapidCountdown )
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_2,this,id,pRow_Alert);
	else	
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_3,this,id,pRow_Alert);
}

void Security_Plugin::SayToDevices(string sText,DeviceData_Router *pDeviceData_Router)
{
	DCE::CMD_Send_Audio_To_Device CMD_Send_Audio_To_Device(m_dwPK_Device,m_PK_Device_TextToSpeach,sText,DATA_Get_PK_Device());
	SendCommand(CMD_Send_Audio_To_Device);
}

string Security_Plugin::GetModeString(int PK_HouseMode)
{
	switch(PK_HouseMode)
	{
	case HOUSEMODE_Armed_away_CONST:
		return "ARMED";
	case HOUSEMODE_Armed_at_home_CONST:
		return "AT HOME SECURE";
	case HOUSEMODE_Sleeping_CONST:
		return "SLEEPING";
	case HOUSEMODE_Entertaining_CONST:
		return "ENTERTAINING";
	case HOUSEMODE_Armed_Extended_away_CONST:
		return "EXTENDED AWAY";
	case HOUSEMODE_Unarmed_at_home_CONST:
	default:
		return "UNARMED";
	}
}
int Security_Plugin::GetModeID(string Mode)
{
	if( Mode=="ARMED" )
		return HOUSEMODE_Armed_away_CONST;
	else if( Mode=="AT HOME SECURE" )
		return HOUSEMODE_Armed_at_home_CONST;
	else if( Mode=="SLEEPING" )
		return HOUSEMODE_Sleeping_CONST;
	else if( Mode=="ENTERTAINING" )
		return HOUSEMODE_Entertaining_CONST;
	else if( Mode=="EXTENDED AWAY" )
		return HOUSEMODE_Armed_Extended_away_CONST;
	else
		return HOUSEMODE_Unarmed_at_home_CONST;
}

bool Security_Plugin::OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	bool bRegistered = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1";
	int PK_HouseMode = GetModeID(m_pDeviceData_Router_this->m_sState_get());
	if( bRegistered )
	{
		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pMessage->m_dwPK_Device_From,StringUtils::itos(PK_HouseMode),"housemode");
		SendCommand(CMD_Set_Bound_Icon);
	}
	return true;
}
//<-dceag-createinst-b->!

void Security_Plugin::AnnounceAlert(DeviceData_Router *pDevice)
{
}

void Security_Plugin::SnapPhoto(Row_Alert_Device *pRow_Alert_Device,DeviceData_Router *pDevice)
{
	for(map<int,DeviceRelation *>::iterator it=pDevice->m_mapDeviceRelation.begin();it!=pDevice->m_mapDeviceRelation.end();++it)
	{
		DeviceRelation *pDeviceRelation = (*it).second;
		DeviceData_Router *pDevice_Camera = pDeviceRelation->m_pDevice;
		if( pDevice_Camera->m_dwPK_DeviceCategory!=DEVICECATEGORY_Surveillance_Cameras_CONST )
			continue;

		char *pData=NULL;
		int iData_Size=0;
		string sFormat="jpg";
		DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame(m_dwPK_Device,pDevice_Camera->m_dwPK_Device,"",0,0,0,&pData,&iData_Size,&sFormat);
		if( SendCommand(CMD_Get_Video_Frame) && iData_Size )
		{
			string sFile = DATA_Get_Path() + "/" + StringUtils::itos(pRow_Alert_Device->PK_Alert_Device_get()) + "." + sFormat;
			FILE *pFile = fopen( sFile.c_str(),"wb");
			if( pFile )
			{
				fwrite(pData,iData_Size,1,pFile);
				fclose(pFile);
			}
			else
				g_pPlutoLogger->Write(LV_CRITICAL,"Unable to write photo to %s",sFile.c_str());
		}
		else
			g_pPlutoLogger->Write(LV_CRITICAL,"Unable to get pic from %d %s",
				pDevice_Camera->m_dwPK_Device,pDevice_Camera->m_sDescription.c_str());
	}
}

Row_Alert *Security_Plugin::LogAlert(Row_AlertType *pRow_AlertType,DeviceData_Router *pDevice)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	Row_Alert *pRow_Alert=NULL;
	bool bNewAlert=false;
	// Let's see if there are any pending alerts to pool this with
	for(size_t s=0;s<m_vectPendingAlerts.size();++s)
	{
		Row_Alert *p = m_vectPendingAlerts[s];
		if( p->FK_AlertType_get()==pRow_AlertType->PK_AlertType_get() )
		{
g_pPlutoLogger->Write(LV_STATUS,"Found a matching alert PK: %d detect: %d pool: %d  now: %d",
p->PK_Alert_get(),(int) StringUtils::SQLDateTime(p->DetectionTime_get()),
pRow_AlertType->PoolAlerts_get(),(int) time(NULL));
			// It's the same type of alert.  See if we should be pooling it
			if( StringUtils::SQLDateTime(p->DetectionTime_get()) + pRow_AlertType->PoolAlerts_get() > time(NULL) )
			{
				pRow_Alert=p;
g_pPlutoLogger->Write(LV_STATUS,"Pooling alert PK: %d",pRow_Alert->PK_Alert_get());
				break;
			}
		}
	}
		
	if( !pRow_Alert )
	{
		bNewAlert=true;
		pRow_Alert = m_pDatabase_pluto_security->Alert_get()->AddRow();
		pRow_Alert->FK_AlertType_set(pRow_AlertType->PK_AlertType_get());
		pRow_Alert->EK_Device_set(pDevice->m_dwPK_Device);
		pRow_Alert->DetectionTime_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_Alert->ExpirationTime_set(StringUtils::SQLDateTime(time(NULL) + (pRow_AlertType ? pRow_AlertType->ExitDelay_get() : 0) ));
		pRow_Alert->Table_Alert_get()->Commit();
		m_vectPendingAlerts.push_back(pRow_Alert);
		m_mapAlarm_New[pRow_Alert->PK_Alert_get()]=true;
g_pPlutoLogger->Write(LV_STATUS,"new alert PK: %d",pRow_Alert->PK_Alert_get());
	}

	Row_Alert_Device *pRow_Alert_Device = m_pDatabase_pluto_security->Alert_Device_get()->AddRow();
	pRow_Alert_Device->FK_Alert_set(pRow_Alert->PK_Alert_get());
	pRow_Alert_Device->EK_Device_set(pDevice->m_dwPK_Device);
	pRow_Alert_Device->DetectionTime_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_Alert_Device->Table_Alert_Device_get()->Commit();

	SnapPhoto(pRow_Alert_Device,pDevice);

	return bNewAlert ? pRow_Alert : NULL;
}
