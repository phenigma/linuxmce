/**
 *
 * @file Security_Plugin.h
 * @brief header file for the Security_Plugin class
 *
 */
 
//<-dceag-d-b->
#ifndef Security_Plugin_h
#define Security_Plugin_h

//	DCE Implemenation for #33 Security Plug-in

#include "Gen_Devices/Security_PluginBase.h"
//<-dceag-d-e->

class Database_pluto_main;
class Database_pluto_security;
class Row_Alert;
class Row_AlertType;
class Row_Alert_Device;
class Row_ModeChange;
#include "DeviceData_Router.h"
#include "AlarmManager.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"

/*
	All sensors' states are in this form: Mode,Bypass,Delay
	where Mode=one of the house modes used in GetModeString: ARMED, AT HOME SECURE, SLEEPING, ENTERTAINING, EXTENDED AWAY,UNARMED
	Bypass=PERMBYPASS (permanently bypass the sensor, ignoring anything that happens to it),
		WAIT (wait for the sensor to return to a normal state, and then consider it active),
		BYPASS (bypass the sensor for this session.  When the sensor's mode changes, revert to normal)
		[empty] (normal, the sensor is active)
	ModeChange=[PK_ModeChange] The record in the database when we changed this mode.  We will do a lookup if the sensor
		is triggered to see if the change occurred during the exit delay

	So each sensor can be independent.  This allows for unlimited 'zones', since each sensor can do its own thing.
	The SetHouseMode command goes through and sets all the sensors to the appropriate mode.  If SetHouseMode is passed
	in a PK_DeviceGroup, it sets only the sensors in that group, thus the device group acts like a zone.

	When a sensor is tripped, SensorTrippedEvent is called.  If the sensor is in WAIT bypass and returns to normal, the bypass flag is cleared.
	Otherwise the house mode for that sensor is determined, and GetAlertType is called to determine which type of alert
	is appropriate, see pluto_security db, table: AlertType (e.g., Security, Fire, Air Quality, Movement, Information).
	Or perhaps it's not an alert, just an announcement or a snap photo.

*/

//<-dceag-decl-b->!
namespace DCE
{
	class Security_Plugin : public Security_Plugin_Command, public DataGridGeneratorPlugIn, public AlarmEvent, public FollowMe_Device
	{
//<-dceag-decl-e->
	// Private member variables 
    pluto_pthread_mutex_t m_SecurityMutex;
	pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
    class Orbiter_Plugin *m_pOrbiter_Plugin;
	DeviceData_Router *m_pDeviceData_Router_this;
	class AlarmManager *m_pAlarmManager;
	vector<Row_Alert *> m_vectPendingAlerts;
	map<int,Row_ModeChange *> m_mapRow_ModeChange_Last;  // Map based on the zone (DeviceGroup) or 0 for all zones
    Row_ModeChange *m_mapRow_ModeChange_Last_Find(int PK_DeviceGroup) { map<int,class Row_ModeChange *>::iterator it = m_mapRow_ModeChange_Last.find(PK_DeviceGroup); return it==m_mapRow_ModeChange_Last.end() ? NULL : (*it).second; }
	int m_PK_Device_TextToSpeach;
	map<pthread_t,class Notification *> m_mapNotification; // Any pending notifications
	bool m_bMonitorMode; // True if the user is monitoring events
	
	// The announcements we'll need to make are stored here.  They come from the Text_LS table
	string m_sCountdownBeforeAlarm,m_sCountdownBeforeArmed,m_sShortCountdownBeforeAlarm,m_sShortCountdownBeforeArmed;

	// If there is a pending exit delay that we are counting down for, the time at which the delay expires
	// will be stored here
	time_t m_tExitTime;
	bool m_bExitDelay_New; // Set to true when a new exit delay is started so we know to make a full announcement.
	map<int,bool> m_mapAlarm_New; // For a PK_Alert Set to true when a new exit delay is started so we know to make a full announcement.

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Security_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	class Datagrid_Plugin *m_pDatagrid_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;
	Database_pluto_security *m_pDatabase_pluto_security;

	/** Datagrids */
	class DataGridTable *SecurityScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign
							, class Message *pMessage );

	bool SetHouseMode(DeviceData_Router *pDevice,int iPK_Users,int PK_HouseMode,string sHandlingInstructions);
	void HandleSetModeFailure(Message *pMessage);
	bool SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice);
	string GetModeString(int PK_HouseMode);
	int GetModeID(string Mode);
	// Returns the PK_AlertType for this mode/house.  Or ALERTTYPE_DONOTHING, ALERTTYPE_ANNOUNCMENT, ALERTTYPE_PHOTO
	int GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice);
	void SecurityBreach(DeviceData_Router *pDevice);
	void FireAlarm(DeviceData_Router *pDevice);
	string AlertsSinceLastChange(int PK_DeviceGroup);

	/** Interceptors */
    bool SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	// Alarm callback
	virtual void AlarmCallback(int id, void* param);
	void ProcessCountdown(int id,Row_Alert *pRow_Alert);
	void ProcessAlert(Row_Alert *pRow_Alert);
	void SayToDevices(string sText,DeviceData_Router *pDeviceData_Router);

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}

	// Actions
	void AnnounceAlert(DeviceData_Router *pDevice);
	void SnapPhoto(Row_Alert_Device *pRow_Alert_Device,DeviceData_Router *pDevice);
	Row_Alert *LogAlert(Row_AlertType *pRow_AlertType,DeviceData_Router *pDevice);  // Returns NULL if the alert was pooled with another

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Path();
	string DATA_Get_Mobile_Orbiter_Notification();
	string DATA_Get_Other_Phone_Notifications();
	string DATA_Get_Neighbors_to_Call();
	int DATA_Get_PK_HouseMode();
	void DATA_Set_PK_HouseMode(int Value);
	string DATA_Get_PK_Device();

			*****EVENT***** accessors inherited from base class
	void EVENT_Security_Breach(int iPK_Device);
	void EVENT_Fire_Alarm(int iPK_Device);
	void EVENT_Reset_Alarm();
	void EVENT_Air_Quality(int iPK_Device);
	void EVENT_Doorbell(int iPK_Device);
	void EVENT_Monitor_Mode(int iPK_Device);

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { string sCMD_Result; CMD_Set_House_Mode(sValue_To_Assign.c_str(),iPK_Users,sPassword.c_str(),iPK_DeviceGroup,sHandling_Instructions.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	// make these inline so Orbiter Plugin can call them without adding the security plugin.cpp file
	// TODO: All the plugins should call the other plugins using the shared objects/dll's rather than static linking
	void Security_Plugin::SetMonitorModeBoundIcon(OH_Orbiter *pOH_Orbiter_Compare=NULL)
	{
		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter==NULL || pOH_Orbiter==pOH_Orbiter_Compare )
			{
				DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos((int) m_bMonitorMode),"monitormode");
				SendCommand(CMD_Set_Bound_Icon);
			}
		}
	}

	void Security_Plugin::SetHouseModeBoundIcon(OH_Orbiter *pOH_Orbiter_Compare=NULL)
	{
		int PK_HouseMode = atoi(m_pDeviceData_Router_this->m_sState_get().c_str());
		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter==NULL || pOH_Orbiter==pOH_Orbiter_Compare )
			{
				DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos(PK_HouseMode),"housemode");
				SendCommand(CMD_Set_Bound_Icon);
			}
		}
	}
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
