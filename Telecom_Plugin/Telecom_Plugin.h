//<-dceag-d-b->
#ifndef Telecom_Plugin_h
#define Telecom_Plugin_h

//	DCE Implemenation for #34 Telecom Plug-in

#include "Gen_Devices/Telecom_PluginBase.h"
//<-dceag-d-e->

class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Telecom_Plugin : public Telecom_Plugin_Command
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
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	// Datagrids
	class DataGridTable *TelecomScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

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


	/** @brief COMMAND: #232 - PL_Originate */
	/** Originate a call */
		/** @param #2 PK_Device */
			/** Device (phone) from which to place the call */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension) { string sCMD_Result; CMD_PL_Originate(iPK_Device,sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #234 - PL_TransferConferenceDevice */
	/** Transfers a call to other phone */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */

	virtual void CMD_PL_TransferConferenceDevice(int iPK_Device) { string sCMD_Result; CMD_PL_TransferConferenceDevice(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_PL_TransferConferenceDevice(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #236 - PL_Hangup */
	/** Hangs up a call */

	virtual void CMD_PL_Hangup() { string sCMD_Result; CMD_PL_Hangup(sCMD_Result,NULL);};
	virtual void CMD_PL_Hangup(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

private:
	 bool CommandResult( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Router *pDeviceFrom, class DeviceData_Router *pDeviceTo );
	 void ProcessResult(int iCommandID, int iResult, std::string sMessage);
	 
	 bool Ring( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Router *pDeviceFrom, class DeviceData_Router *pDeviceTo );
	 void ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID);

private:
    DeviceData_Router* find_AsteriskDevice();
    DeviceData_Router* find_Device(int iPK_Device);

private:
	int iCmdCounter;
	int generate_NewCommandID();
		
};	

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
