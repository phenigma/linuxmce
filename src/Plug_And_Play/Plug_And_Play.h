//<-dceag-d-b->
#ifndef Plug_And_Play_h
#define Plug_And_Play_h

//	DCE Implemenation for #1791 Plug And Play

#include "Gen_Devices/Plug_And_PlayBase.h"
//<-dceag-d-e->

class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Plug_And_Play : public Plug_And_Play_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Plug_And_Play(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Plug_And_Play();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Plug_And_Play(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

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


	/** @brief COMMAND: #797 - PlugAndPlayAddDevice */
	/** Adds or enables the newly discovered device */
		/** @param #9 Text */
			/** The MessageSend parameters for creating the discovered device */
		/** @param #44 PK_DeviceTemplate */
			/** The device template ID for the discovered device */
		/** @param #70 Tokens */
			/** Extra parameters in the form of | (pipe) separated tokens */
		/** @param #214 PNPSerialNo */
			/** The serial number of the device, computed by the PNP discovery daemons */
		/** @param #215 PK_PnpProtocol */
			/** The PNP protocol of the daemon that discovered the device */

	virtual void CMD_PlugAndPlayAddDevice(string sText,int iPK_DeviceTemplate,string sTokens,string sPNPSerialNo,int iPK_PnpProtocol) { string sCMD_Result; CMD_PlugAndPlayAddDevice(sText.c_str(),iPK_DeviceTemplate,sTokens.c_str(),sPNPSerialNo.c_str(),iPK_PnpProtocol,sCMD_Result,NULL);};
	virtual void CMD_PlugAndPlayAddDevice(string sText,int iPK_DeviceTemplate,string sTokens,string sPNPSerialNo,int iPK_PnpProtocol,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #798 - PlugAndPlayRemoveDevice */
	/** Disables the newly discovered device */
		/** @param #214 PNPSerialNo */
			/** The serial number detected by the PNP discovery layers */

	virtual void CMD_PlugAndPlayRemoveDevice(string sPNPSerialNo) { string sCMD_Result; CMD_PlugAndPlayRemoveDevice(sPNPSerialNo.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PlugAndPlayRemoveDevice(string sPNPSerialNo,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

private: 
	Database_pluto_main *m_pDatabase_pluto_main;
	
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
