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
#include "DeviceData_Router.h"

//<-dceag-decl-b->
namespace DCE
{
	class Security_Plugin : public Security_Plugin_Command
	{
//<-dceag-decl-e->
	// Private member variables 
    pluto_pthread_mutex_t m_SecurityMutex;
    class Orbiter_Plugin *m_pOrbiter_Plugin;

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

	/** Datagrids */
	class DataGridTable *SecurityScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign
							, class Message *pMessage );

	bool SetHouseMode(DeviceData_Router *pDevice,int PK_HouseMode,string sHandlingInstructions);
	void HandleSetModeFailure(Message *pMessage);

	/** Interceptors */
    bool SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

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

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sErrors,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { string sCMD_Result; CMD_Set_House_Mode(sValue_To_Assign.c_str(),iPK_Users,sErrors.c_str(),sPassword.c_str(),iPK_DeviceGroup,sHandling_Instructions.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sErrors,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
