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
class Row_ModeChange;
#include "DeviceData_Router.h"
#include "AlarmManager.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"

//<-dceag-decl-b->!
namespace DCE
{
	class Security_Plugin : public Security_Plugin_Command, public DataGridGeneratorPlugIn, public AlarmEvent, public FollowMe_Device
	{
//<-dceag-decl-e->
	// Private member variables 
    pluto_pthread_mutex_t m_SecurityMutex;
    class Orbiter_Plugin *m_pOrbiter_Plugin;
	DeviceData_Router *m_pDeviceData_Router_this;
	class AlarmManager *m_pAlarmManager;
	vector<Row_Alert *> m_vectPendingAlerts;
	map<int,Row_ModeChange *> m_mapRow_ModeChange_Last;  // Map based on the zone (DeviceGroup) or 0 for all zones
    Row_ModeChange *m_mapRow_ModeChange_Last_Find(int PK_DeviceGroup) { map<int,class Row_ModeChange *>::iterator it = m_mapRow_ModeChange_Last.find(PK_DeviceGroup); return it==m_mapRow_ModeChange_Last.end() ? NULL : (*it).second; }
	int m_PK_Device_TextToSpeach;

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
	int GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice);
	void SecurityBreach(DeviceData_Router *pDevice);
	void FireAlarm(DeviceData_Router *pDevice);
	string AlertsSinceLastChange(int PK_DeviceGroup);

	/** Interceptors */
    bool SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	// Alarm callback
	virtual void AlarmCallback(int id, void* param);
	void ProcessCountdownBeforeAlarm();
	void SayToDevices(string sText,map<int,DeviceData_Router *> &mapAudioDevices,DeviceData_Router *pDeviceData_Router);

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_HouseMode();
	void DATA_Set_PK_HouseMode(int Value);

			*****EVENT***** accessors inherited from base class
	void EVENT_Security_Breach(int iPK_Device);
	void EVENT_Fire_Alarm(int iPK_Device);
	void EVENT_Reset_Alarm();

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
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
