//<-dceag-d-b->
#ifndef Orbiter_Plugin_h
#define Orbiter_Plugin_h

//	DCE Implemenation for #12 Orbiter Plug-in

#include "Gen_Devices/Orbiter_PluginBase.h"
//<-dceag-d-e->

#include "Orbiter/Floorplan.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "Gen_Devices/AllCommandsRequests.h"

class Database_pluto_main;
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
        bool m_bFloorPlansArePrepared;

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
	class FollowMe_Plugin *m_pLighting_Followme,*m_pMedia_Followme,*m_pClimate_Followme,*m_pSecurity_Followme,*m_pTelecom_Followme;
	class FloorplanInfoProvider *m_pLighting_Floorplan,*m_pClimate_Floorplan,*m_pMedia_Floorplan,*m_pSecurity_Floorplan,*m_pTelecom_Floorplan;
	class Media_Plugin *m_pMedia_Plugin;

    // Private methods
    map<string,UnknownDeviceInfos *> m_mapUnknownDevices; // A temporary map to match Bluetooth Dongle's with devices they detect
    void PrepareFloorplanInfo();
    UnknownDeviceInfos *m_mapUnknownDevices_Find(string sMacAddress)
    {
        map<string,UnknownDeviceInfos *>::iterator it = m_mapUnknownDevices.find(sMacAddress);
        return it == m_mapUnknownDevices.end() ? NULL : (*it).second;
    }

	//from command_impl class
	virtual bool SafeToReload();
	list<int> m_listRegenCommands;

	pluto_pthread_mutex_t m_UnknownDevicesMutex;
    bool m_bNoUnknownDeviceIsProcessing;
	string m_sPK_Device_AllOrbiters;

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
    bool RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool NewPnpDevice( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

    void ProcessUnknownDevice();
	void DisplayMessageOnOrbiter(int dwPK_Device,string sMessage,bool bPromptToResetRouter=false,int iTimeout=0,bool bCantGoBack=false)
	{
		if ( sMessage == "" )
			sMessage = "Unable to save playlist";

		string sPK_Device = dwPK_Device ? StringUtils::itos(dwPK_Device) : m_sPK_Device_AllOrbiters;
		DCE::CMD_Set_Text_DL CMD_Set_Text( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), sMessage, TEXT_STATUS_CONST);

		DCE::CMD_Goto_Screen_DL CMD_Goto_Screen( m_dwPK_Device, sPK_Device, 0, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "", "", false, bCantGoBack );
		CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text.m_pMessage);
		if( bPromptToResetRouter )
		{
			DCE::CMD_Show_Object_DL CMD_Show_Object( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object.m_pMessage);
		}
		if( iTimeout )
		{
			DCE::CMD_Set_Timeout_DL CMD_Set_Timeout( m_dwPK_Device, sPK_Device, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), StringUtils::itos(iTimeout) );
			CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Timeout.m_pMessage);
		}
		SendCommand( CMD_Goto_Screen );
	}

	void SetNowPlaying( int dwPK_Device, string sNowPlaying )
	{
		DCE::CMD_Set_Now_Playing CMD_Set_Now_Playing( m_dwPK_Device, dwPK_Device, sNowPlaying );
		SendCommand( CMD_Set_Now_Playing );
	}

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

	virtual void CMD_Orbiter_Registered(string sOnOff) { string sCMD_Result; CMD_Orbiter_Registered(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Registered(string sOnOff,string &sCMD_Result,Message *pMessage);


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


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
