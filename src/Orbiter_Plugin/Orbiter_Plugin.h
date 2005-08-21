//<-dceag-d-b->
#ifndef Orbiter_Plugin_h
#define Orbiter_Plugin_h

//	DCE Implemenation for #12 Orbiter Plug-in

#include "Gen_Devices/Orbiter_PluginBase.h"
//<-dceag-d-e->

#include "Orbiter/Floorplan.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllCommandsRequests.h"

class Database_pluto_main;
class Database_pluto_security;
#include "OH_Orbiter.h"

namespace DCE
{

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

class AllowedConnections
{
public:
    time_t m_tExpirationTime;
    int m_iDeviceIDAllowed;

    AllowedConnections(time_t tExpirationTime, int iDeviceIDAllowed)
    {
        m_tExpirationTime = tExpirationTime;
        m_iDeviceIDAllowed = iDeviceIDAllowed;
    }
};

//<-dceag-decl-b->!
    class Orbiter_Plugin : public Orbiter_Plugin_Command
    {
//<-dceag-decl-e->
        bool m_bFloorPlansArePrepared;
		int m_iThreshHold;

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
    Database_pluto_security *m_pDatabase_pluto_security;
	class FloorplanInfoProvider *m_pLighting_Floorplan,*m_pClimate_Floorplan,*m_pMedia_Floorplan,*m_pSecurity_Floorplan,*m_pTelecom_Floorplan;
	class Media_Plugin *m_pMedia_Plugin;
	class General_Info_Plugin *m_pGeneral_Info_Plugin;
	list<int> m_listNewPnpDevicesWaitingForARoom;

    // Private methods
    map<string,UnknownDeviceInfos *> m_mapUnknownDevices; // A temporary map to match Bluetooth Dongle's with devices they detect
    void PrepareFloorplanInfo();
    UnknownDeviceInfos *m_mapUnknownDevices_Find(string sMacAddress)
    {
        map<string,UnknownDeviceInfos *>::iterator it = m_mapUnknownDevices.find(sMacAddress);
        return it == m_mapUnknownDevices.end() ? NULL : (*it).second;
    }

	void SendAppToPhone(OH_Orbiter *pOH_Orbiter,DeviceData_Base *pDevice_Dongle);

	//from command_impl class
	virtual bool SafeToReload(string *sPendingTasks=NULL);
	list<int> m_listRegenCommands;

	pluto_pthread_mutex_t m_AllowedConnectionsMutex;
    map<string, class AllowedConnections *> m_mapAllowedConnections;

	pluto_pthread_mutex_t m_UnknownDevicesMutex;
    bool m_bNoUnknownDeviceIsProcessing;
	string m_sPK_Device_AllOrbiters;
	map<int,OH_Orbiter *> m_mapRemote_2_Orbiter;
	map<int,OH_Orbiter *> m_mapMD_2_Orbiter;

public:
    // Public member variables
    map<int,OH_Orbiter *> m_mapOH_Orbiter;
    OH_Orbiter *m_mapOH_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapOH_Orbiter.find(PK_Device);
        return it==m_mapOH_Orbiter.end() ? NULL : (*it).second;
    }

    OH_Orbiter *m_mapRemote_2_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapRemote_2_Orbiter.find(PK_Device);
        return it==m_mapRemote_2_Orbiter.end() ? NULL : (*it).second;
    }

    OH_Orbiter *m_mapMD_2_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapMD_2_Orbiter.find(PK_Device);
        return it==m_mapMD_2_Orbiter.end() ? NULL : (*it).second;
    }

	map<string, OH_Orbiter *> m_mapOH_Orbiter_Mac;
    OH_Orbiter *m_mapOH_Orbiter_Mac_Find(string MacAddress) {
        map<string, class OH_Orbiter *>::iterator it = m_mapOH_Orbiter_Mac.find(MacAddress);
        return it==m_mapOH_Orbiter_Mac.end() ? NULL : (*it).second;
    }

    map<int,OH_User *> m_mapOH_User;
    OH_User *m_mapOH_User_Find(int PK_User) {
        map<int,class OH_User *>::iterator it = m_mapOH_User.find(PK_User);
        return it==m_mapOH_User.end() ? NULL : (*it).second;
    }

    class AllowedConnections *m_mapAllowedConnections_Find(string sMacAddress) {
        map<string, class AllowedConnections *>::iterator it = m_mapAllowedConnections.find(sMacAddress);
        return it==m_mapAllowedConnections.end() ? NULL : (*it).second;
    }

    bool ConnectionAllowed(int iDevice, string sMacAddress);

	// Our interceptors
    bool RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool ReloadAborted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool NewPnpDevice( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool OSD_OnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

    void ProcessUnknownDevice();
	void DoneCheckingForUpdates()
	{
		PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

		// We must have started the check for updates because we added a new device.  However we finished
		// getting room info from the user, so he's ready to go
		if( m_listNewPnpDevicesWaitingForARoom.size()==0 )
			DisplayMessageOnOrbiter("","<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>",true);
	}

	/*  If the user wants to use the onscreen display, he may want the tv to always stay on and only
	turn off when the orbiter is specifcally turned off or goes to sleep.  In those cases the user
	will set the DEVICEDATA_Leave_Monitor_on_for_OSD_CONST flag to 1 */
	void OverrideAVPipe(DeviceData_Router *pDevice_OSD,bool bOverride);
	
	void DisplayMessageOnOrbiter(int dwPK_Device,string sMessage,bool bPromptToResetRouter=false,int iTimeout=0,bool bCantGoBack=false,
		string sOption1="",string sMessage1="",string sOption2="",string sMessage2="",string sOption3="",string sMessage3="",string sOption4="", string sMessage4="" )
	{
		DisplayMessageOnOrbiter(StringUtils::itos(dwPK_Device),sMessage,bPromptToResetRouter,iTimeout,bCantGoBack,
			sOption1,sMessage1,sOption2,sMessage2,sOption3,sMessage3,sOption4,sMessage4);
	}

	void DisplayMessageOnOrbiter(string sPK_Device,string sMessage,bool bPromptToResetRouter=false,int iTimeout=0,bool bCantGoBack=false,
		string sOption1="",string sMessage1="",string sOption2="",string sMessage2="",string sOption3="",string sMessage3="",string sOption4="", string sMessage4="" )
	{
		if ( sMessage == "" )
			sMessage = "Unable to save playlist";

		if( sPK_Device.size()==0 )
			sPK_Device = m_sPK_Device_AllOrbiters;
		DCE::CMD_Goto_Screen_DL CMD_Goto_Screen( m_dwPK_Device, sPK_Device, 0, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "", "", false, bCantGoBack );

		DCE::CMD_Set_Text_DL CMD_Set_Text_DL( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), sMessage, TEXT_STATUS_CONST);
		CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text_DL.m_pMessage);

		if( bPromptToResetRouter )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);
		}

		if( sOption1.size() )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);

			DCE::CMD_Set_Text_DL CMD_Set_Text_DL( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), sOption1, TEXT_STATUS_CONST);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text_DL.m_pMessage);

			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sPK_Device, VARIABLE_Misc_Data_1_CONST , sMessage1);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable_DL.m_pMessage);
		}
		if( sOption2.size() )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);

			DCE::CMD_Set_Text_DL CMD_Set_Text_DL( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), sOption2, TEXT_STATUS_CONST);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text_DL.m_pMessage);

			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sPK_Device, VARIABLE_Misc_Data_2_CONST , sMessage2);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable_DL.m_pMessage);
		}
		if( sOption3.size() )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);

			DCE::CMD_Set_Text_DL CMD_Set_Text_DL( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), sOption3, TEXT_STATUS_CONST);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text_DL.m_pMessage);

			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sPK_Device, VARIABLE_Misc_Data_3_CONST , sMessage3);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable_DL.m_pMessage);
		}
		if( sOption4.size() )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);

			DCE::CMD_Set_Text_DL CMD_Set_Text_DL( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), sOption4, TEXT_STATUS_CONST);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text_DL.m_pMessage);

			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sPK_Device, VARIABLE_Misc_Data_4_CONST , sMessage4);
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable_DL.m_pMessage);
		}

		DCE::CMD_Set_Timeout_DL CMD_Set_Timeout( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), StringUtils::itos(iTimeout) );
		CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Timeout.m_pMessage);
		SendCommand( CMD_Goto_Screen );
	}

	void FireFollowMe(string sMask,int iPK_Orbiter,int iPK_Users,int iPK_RoomOrEntArea,int iPK_RoomOrEntArea_Left);
	void SetBoundIcons(int iPK_Users,bool bOnOff,string sType);

    void GenerateVMCFiles();
    void GeneratePlutoMOConfig();

	//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_ThreshHold();

			*****EVENT***** accessors inherited from base class
	void EVENT_Follow_Me_Lighting(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Climate(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Media(int iPK_Orbiter,int iPK_Users,int iPK_EntArea,int iPK_EntArea_Left);
	void EVENT_Follow_Me_Telecom(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Security(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left);

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #77 - Set Current Room */
	/** Set what room the orbiter is in. */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #78 - New Mobile Orbiter */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof */
		/** @param #17 PK_Users */
			/** The primary user of the phone. */
		/** @param #44 PK_DeviceTemplate */
			/** What type of phone it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */

	virtual void CMD_New_Mobile_Orbiter(int iPK_Users,int iPK_DeviceTemplate,string sMac_address) { string sCMD_Result; CMD_New_Mobile_Orbiter(iPK_Users,iPK_DeviceTemplate,sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_New_Mobile_Orbiter(int iPK_Users,int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage);


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
		/** @param #10 ID */
			/** The page number (ie the floorplan ID based). */
		/** @param #46 PK_FloorplanType */
			/** The type of floorplan (lights, climate, etc.) */

	virtual void CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Current_Floorplan(sID.c_str(),iPK_FloorplanType,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #255 - Orbiter Registered */
	/** Indicates the orbiter has registered, or unregistered */
		/** @param #8 On/Off */
			/** 1 means it is registering, 0 means it is closing */
		/** @param #17 PK_Users */
			/** If registering, the current user */
		/** @param #19 Data */
			/** a serialized map containing the media types,file list screens, so the orbiter can display the file lists without asking media plugin each time */
		/** @param #45 PK_EntertainArea */
			/** If registering, the current EA */
		/** @param #57 PK_Room */
			/** If registering, the current room */

	virtual void CMD_Orbiter_Registered(string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { string sCMD_Result; CMD_Orbiter_Registered(sOnOff.c_str(),iPK_Users,sPK_EntertainArea.c_str(),iPK_Room,pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Registered(string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #261 - Set Follow-Me */
	/** Specifies whether the orbiter has follow-me on or off for the given user. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #9 Text */
			/** Can be 'L', 'M', 'C', 'S', 'T' for Lighting, Media, Climate, Security, Telecom followed by 0 or 1 for off/on. */
		/** @param #17 PK_Users */
			/** The User */

	virtual void CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users) { string sCMD_Result; CMD_Set_FollowMe(iPK_Device,sText.c_str(),iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #266 - Regen Orbiter */
	/** Regenerates an Orbiter.  When regeneration is complete, the "Regen Orbiter Finished" command will be sent */
		/** @param #2 PK_Device */
			/** The Orbiter to regenerate */
		/** @param #21 Force */
			/** Can be -r to force a full regen, or -a for a quick one */

	virtual void CMD_Regen_Orbiter(int iPK_Device,string sForce) { string sCMD_Result; CMD_Regen_Orbiter(iPK_Device,sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Regen_Orbiter(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #267 - Regen Orbiter Finished */
	/** Regeneration of the indicated Orbiter has been finished */
		/** @param #2 PK_Device */
			/** The Orbiter */

	virtual void CMD_Regen_Orbiter_Finished(int iPK_Device) { string sCMD_Result; CMD_Regen_Orbiter_Finished(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Regen_Orbiter_Finished(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room) { string sCMD_Result; CMD_Set_Room_For_Device(iPK_Device,iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #404 - Set Auto Switch to Remote */
	/** Specifies whether the given orbiter will automatically switch to the remote control when media starts. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #119 True/False */
			/** If true it will switch automatically */

	virtual void CMD_Set_Auto_Switch_to_Remote(int iPK_Device,bool bTrueFalse) { string sCMD_Result; CMD_Set_Auto_Switch_to_Remote(iPK_Device,bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Set_Auto_Switch_to_Remote(int iPK_Device,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #406 - Display Message */
	/** Displays a message on orbiters, or all orbiters. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 PK_Device_List */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sPK_Device_List) { string sCMD_Result; CMD_Display_Message(sText.c_str(),sType.c_str(),sName.c_str(),sTime.c_str(),sPK_Device_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sPK_Device_List,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #686 - Display Dialog Box On Orbiter */
	/** Display a dialog box on orbiters, or all orbiter, with 0-4 custom buttons. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #39 Options */
			/** A pipe delimited list with options and messages like this: option1|message1|options2|message2 */
		/** @param #103 PK_Device_List */
			/** A comma delimited list of orbiters, or all orbiters if empty */

	virtual void CMD_Display_Dialog_Box_On_Orbiter(string sText,string sOptions,string sPK_Device_List) { string sCMD_Result; CMD_Display_Dialog_Box_On_Orbiter(sText.c_str(),sOptions.c_str(),sPK_Device_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Dialog_Box_On_Orbiter(string sText,string sOptions,string sPK_Device_List,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
