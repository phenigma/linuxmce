//<-dceag-d-b->
#ifndef Orbiter_Plugin_h
#define Orbiter_Plugin_h

//	DCE Implemenation for #12 Orbiter Plug-in

#include "Gen_Devices/Orbiter_PluginBase.h"
//<-dceag-d-e->

#include "Orbiter/Floorplan.h"

class Database_pluto_main;

namespace DCE
{

	// Store an orbiter list within the handler
	class OH_Orbiter
	{
	public:
		map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;
		FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page) { map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page);	return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second; }

		class DeviceData_Router *m_pDeviceData_Router;
		class DeviceData_Router *m_pDevice_CurrentDetected;

		int m_iPK_Users;  // The current user
		int m_iPK_EntertainArea;  // The current entertain area
		int m_dwPK_Room;  // The current room

		int m_iLastSignalStrength;

		int m_iFailedToConnectCount;

		OH_Orbiter(class DeviceData_Router *pDeviceData_Router)
		{
			m_pDeviceData_Router = pDeviceData_Router;
			m_iPK_Users = m_iPK_EntertainArea = m_dwPK_Room = 0;
			m_iLastSignalStrength = 0;
			m_pDevice_CurrentDetected = NULL;
			m_iFailedToConnectCount = 0;
		}

	};

class UnknownDeviceInfos
{
public:
	class DeviceData_Router *m_pDeviceFrom;
	int m_iDeviceIDFrom;
	string m_sID;
	bool m_bProcessed;

	UnknownDeviceInfos(class DeviceData_Router *pDeviceFrom, int iDeviceIDFrom, string sID)
	{
		m_pDeviceFrom = pDeviceFrom;
		m_iDeviceIDFrom = iDeviceIDFrom;
		m_sID = sID;
		m_bProcessed = false;
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
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	// Private member variables
	Database_pluto_main *m_pDatabase_pluto_main;

	// Private methods
	map<string,UnknownDeviceInfos *> m_mapUnknownDevices; // A temporary map to match Bluetooth Dongle's with devices they detect
	void PrepareFloorplanInfo();
	UnknownDeviceInfos *m_mapUnknownDevices_Find(string sMacAddress) 
	{
		map<string,UnknownDeviceInfos *>::iterator it = m_mapUnknownDevices.find(sMacAddress);
		return it == m_mapUnknownDevices.end() ? NULL : (*it).second;
	}

	pluto_pthread_mutex_t m_UnknownDevicesMutex;
	bool m_bNoUnknownDeviceIsProcessing;

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

	void ProcessUnknownDevice();

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


	/** @brief COMMAND: #26 - Set User Mode */
	/** Change a user's status (at home, sleeping, etc.) */
		/** @param #5 Value To Assign */
			/** A Value from the UserMode table */
		/** @param #17 PK_Users */
			/** The User to change */

	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users) { string sCMD_Result; CMD_Set_User_Mode(sValue_To_Assign.c_str(),iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #58 - Set Current User */
	/** Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** Set what entertainment area the orbiter (the 'from' in the message) is in. */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

	virtual void CMD_Set_Entertainment_Area(int iPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(iPK_EntertainArea,sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(int iPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #77 - Set Current Room */
	/** Set what room the orbiter is in. */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #78 - New Mobile Orbiter */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof */
		/** @param #44 PK_DeviceTemplate */
			/** What type of phone it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */

	virtual void CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address) { string sCMD_Result; CMD_New_Mobile_Orbiter(iPK_DeviceTemplate,sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #79 - Add Unknown Device */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that the device is a phone and it should be added */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address) { string sCMD_Result; CMD_Add_Unknown_Device(sText.c_str(),sID.c_str(),sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #183 - Get Floorplan Layout */
	/** Gets the layout of all floorplans for the orbiter. */
		/** @param #5 Value To Assign */
			/** A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}} */

	virtual void CMD_Get_Floorplan_Layout(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Floorplan_Layout(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Floorplan_Layout(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #186 - Get Current Floorplan */
	/** Gets the current Floorplan status (ie what items are on/off, etc.) for the specified Floorplan type. */
		/** @param #5 Value To Assign */
			/** The status of all the devices within the floorplan. */
		/** @param #46 PK_FloorplanType */
			/** The type of floorplan (lights, climate, etc.) */

	virtual void CMD_Get_Current_Floorplan(int iPK_FloorplanType,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Current_Floorplan(iPK_FloorplanType,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Current_Floorplan(int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
