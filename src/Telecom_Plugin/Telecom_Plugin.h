//<-dceag-d-b->
#ifndef Telecom_Plugin_h
#define Telecom_Plugin_h

//	DCE Implemenation for #34 Telecom Plug-in

#include "Gen_Devices/Telecom_PluginBase.h"
//<-dceag-d-e->

#include "DCE/DeviceData_Router.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "Orbiter/Floorplan.h"
class Database_pluto_main;
class Database_pluto_telecom;
class Row_PhoneNumber;

//<-dceag-decl-b->!
namespace DCE
{
	class Telecom_Plugin : public Telecom_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device
	{
//<-dceag-decl-e->
	// Private member variables

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Telecom_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Telecom_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;
	Database_pluto_telecom *m_pDatabase_pluto_telecom;

	// Datagrids
	class DataGridTable *TelecomScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *PhoneBookAutoCompl(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *PhoneBookListOfNos(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *ActiveCallsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left) {}

	// Floorplan
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);

	// Other
	string GetDialNumber(Row_PhoneNumber *pRow_PhoneNumber);

	// Interceptors
	bool OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

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


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a DTMF code */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #232 - PL_Originate */
	/** Originate a call */
		/** @param #2 PK_Device */
			/** Device (phone) from which to place the call */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */
		/** @param #84 PhoneCallerID */
			/** Caller ID */

	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { string sCMD_Result; CMD_PL_Originate(iPK_Device,sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #234 - PL_Transfer */
	/** Transfers a call to other phone */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */
		/** @param #17 PK_Users */
			/** User ID to transfer call to */
		/** @param #83 PhoneExtension */
			/** Local Extension to transfer call to */
		/** @param #196 IsConference */
			/** Transfer the call to a conference room? */

	virtual void CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,bool bIsConference) { string sCMD_Result; CMD_PL_Transfer(iPK_Device,iPK_Users,sPhoneExtension.c_str(),bIsConference,sCMD_Result,NULL);};
	virtual void CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,bool bIsConference,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #236 - PL_Hangup */
	/** Hangs up a call */

	virtual void CMD_PL_Hangup() { string sCMD_Result; CMD_PL_Hangup(sCMD_Result,NULL);};
	virtual void CMD_PL_Hangup(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

	virtual void CMD_Phone_Initiate(string sPhoneExtension) { string sCMD_Result; CMD_Phone_Initiate(sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Phone_Initiate(string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

	virtual void CMD_Phone_Answer() { string sCMD_Result; CMD_Phone_Answer(sCMD_Result,NULL);};
	virtual void CMD_Phone_Answer(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

	virtual void CMD_Phone_Drop() { string sCMD_Result; CMD_Phone_Drop(sCMD_Result,NULL);};
	virtual void CMD_Phone_Drop(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #414 - PL External Originate */
	/** Originate an external call */
		/** @param #75 PhoneNumber */
			/** Phone to call */
		/** @param #81 CallerID */
			/** Caller ID */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

	virtual void CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension) { string sCMD_Result; CMD_PL_External_Originate(sPhoneNumber.c_str(),sCallerID.c_str(),sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #744 - Set User Mode */
	/** Set a user to a given mode (sleeping, etc.) */
		/** @param #17 PK_Users */
			/** The user */
		/** @param #194 PK_UserMode */
			/** The user mode */

	virtual void CMD_Set_User_Mode(int iPK_Users,int iPK_UserMode) { string sCMD_Result; CMD_Set_User_Mode(iPK_Users,iPK_UserMode,sCMD_Result,NULL);};
	virtual void CMD_Set_User_Mode(int iPK_Users,int iPK_UserMode,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #751 - PL_Add_VOIP_Account */
	/** Add a VOIP account */
		/** @param #50 Name */
			/** Provider name */
		/** @param #75 PhoneNumber */
			/** Phone number */
		/** @param #99 Password */
			/** Password */
		/** @param #189 Users */
			/** User name */

	virtual void CMD_PL_Add_VOIP_Account(string sName,string sPhoneNumber,string sPassword,string sUsers) { string sCMD_Result; CMD_PL_Add_VOIP_Account(sName.c_str(),sPhoneNumber.c_str(),sPassword.c_str(),sUsers.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Add_VOIP_Account(string sName,string sPhoneNumber,string sPassword,string sUsers,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

private:
	 bool CommandResult( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	 void ProcessResult(int iCommandID, int iResult, std::string sMessage);
	 
	 bool Ring( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	 void ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID);
	 
	bool IncomingCall( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool Hangup( class Socket *pSocket, class Message *pMessage,
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );						
private:
    DeviceData_Router* find_AsteriskDevice();
    DeviceData_Router* find_Device(int iPK_Device);
	int ParseChannel(const std::string channel, int* iextension, string *sextension);

private:
	int iCmdCounter;
	int generate_NewCommandID();
	map<int,int> map_orbiter2embedphone;
	map<int,int> map_embedphone2orbiter;
	map<int,int> map_ext2device;
	map<int,int> map_device2ext;
		
};	

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
