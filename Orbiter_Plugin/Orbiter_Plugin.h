//<-dceag-d-b->
#ifndef Orbiter_Plugin_h
#define Orbiter_Plugin_h

//	DCE Implemenation for #12 Orbiter Plug-in

#include "Gen_Devices/Orbiter_PluginBase.h"
//<-dceag-d-e->
class Database_pluto_main;

namespace DCE
{

// Store an orbiter list within the handler
class OH_Orbiter
{
public:
	class DeviceData_Router *m_pDeviceData_Router;
	class DeviceData_Router *m_pDevice_CurrentDetected;

	int m_iPK_Users;  // The current user
	int m_iPK_EntertainArea;  // The current entertain area
	int m_iPK_Room;  // The current room

	int m_iLastSignalStrength;

	OH_Orbiter(class DeviceData_Router *pDeviceData_Router)
	{
		m_pDeviceData_Router = pDeviceData_Router;
		m_iPK_Users = m_iPK_EntertainArea = m_iPK_Room = 0;
		m_iLastSignalStrength = 0;
		m_pDevice_CurrentDetected = NULL;
	}

};

/*

	class Device *m_pDevice_CurrentDetected;
	class Device *m_pDevice_This;
	class EntGroup *m_pLastEntgroup;
	class Device *m_pEntGroupAudioZone_LockedOn;
	class OCRoom *m_pRoom_LockedOn;
	class OCController *m_pController;
	class PlutoServerCore *m_pCore;
	class OCLogger *m_pOCLogger;
	int m_iLastMenuType;
	string m_sID,m_sWidth,m_sHeight,m_sCurrentInput;
	bool m_bBuildingMenu;
	bool m_bFollowMeLights,m_bFollowMeClimate,m_bFollowMeMedia,m_bFollowMeTelecom,m_bFollowMeSecurity;
	int m_iLastSignalStrength,m_EnterKeyAlarm;

*/ 

//<-dceag-decl-b->!
	class Orbiter_Plugin : public Orbiter_Plugin_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Orbiter_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Orbiter_Plugin();
		virtual bool Register();
//<-dceag-const-e->
	// Private member variables
	Database_pluto_main *m_pDatabase_pluto_main;

	// Private methods
	map<string,int> m_mapUnknownDevices; // A temporary map to match Bluetooth Dongle's with devices they detect

public:
	// Public member variables
	map<int,OH_Orbiter *> m_mapOH_Orbiter;
	OH_Orbiter *m_mapOH_Orbiter_Find(int PK_Device) {
		map<int,class OH_Orbiter *>::iterator it = m_mapOH_Orbiter.find(PK_Device);
		return it==m_mapOH_Orbiter.end() ? NULL : (*it).second;
	}

	map<string, OH_Orbiter *> m_mapOH_Orbiter_Mac;
	OH_Orbiter *m_mapOH_Orbiter_Mac_Find(string MacAddress) {
		map<string, class OH_Orbiter *>::iterator it = m_mapOH_Orbiter_Mac.find(MacAddress);
		return it==m_mapOH_Orbiter_Mac.end() ? NULL : (*it).second;
	} 

	// Our interceptors
	bool RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #26 - Set User Mode
	COMMENTS: Change a user's status (at home, sleeping, etc.)
	PARAMETERS:
		#5 Value To Assign
			A Value from the UserMode table
		#17 PK_Users
			The User to change
*/
	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users) { string sCMD_Result; CMD_Set_User_Mode(sValue_To_Assign.c_str(),iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #58 - Set Current User
	COMMENTS: Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter.
	PARAMETERS:
		#17 PK_Users
			The user currently using the orbiter.
*/
	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #59 - Set Entertainment Area
	COMMENTS: Set what entertainment area the orbiter (the 'from' in the message) is in.
	PARAMETERS:
		#45 PK_EntertainArea
			The current entertainment area where the orbiter is.
*/
	virtual void CMD_Set_Entertainment_Area(int iPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(iPK_EntertainArea,sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(int iPK_EntertainArea,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #77 - Set Current Room
	COMMENTS: Set what room the orbiter is in.
	PARAMETERS:
		#57 PK_Room
			The room
*/
	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #78 - New Mobile Orbiter
	COMMENTS: After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof
	PARAMETERS:
		#44 PK_DeviceTemplate
			What type of phone it is.
		#47 Mac address
			The MAC Address of the phone.
*/
	virtual void CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address) { string sCMD_Result; CMD_New_Mobile_Orbiter(iPK_DeviceTemplate,sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
