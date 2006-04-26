//<-dceag-d-b->
#ifndef Plug_And_Play_h
#define Plug_And_Play_h

//	DCE Implemenation for #1791 Plug And Play

#include "Gen_Devices/Plug_And_PlayBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Plug_And_Play : public Plug_And_Play_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:

		/** Flags for capabilities.
		  * 0 2^0 2^1 2^2
		  */
		enum Capabilities { NOTHING=0, CONFIG=1 };
		
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


	/** @brief COMMAND: #798 - PlugAndPlayAddDevice */
	/** Adds or enables the newly discovered device */
		/** @param #44 PK_DeviceTemplate */
			/** The device template ID for the discovered device */
		/** @param #58 IP Address */
			/** The IP address where was the new device detected. */
		/** @param #70 Tokens */
			/** Extra parameters in the form of | (pipe) separated tokens */
		/** @param #214 PNPSerialNo */
			/** The serial number of the device, computed by the PNP discovery daemons */
		/** @param #215 PK_PnpProtocol */
			/** The PNP protocol of the daemon that discovered the device */
		/** @param #216 Identifier */
			/** The device model and producer identifier */
		/** @param #217 Capabilities */
			/** A variable that defines the daemons' capability to perform custom operations */
		/** @param #218 PK_CommMethod */
			/** Communication method */
		/** @param #219 Path */
			/** The path where should run the scripts and binaries used by PnP. */

	virtual void CMD_PlugAndPlayAddDevice(int iPK_DeviceTemplate,string sIP_Address,string sTokens,string sPNPSerialNo,int iPK_PnpProtocol,string sIdentifier,int iCapabilities,int iPK_CommMethod,string sPath) { string sCMD_Result; CMD_PlugAndPlayAddDevice(iPK_DeviceTemplate,sIP_Address.c_str(),sTokens.c_str(),sPNPSerialNo.c_str(),iPK_PnpProtocol,sIdentifier.c_str(),iCapabilities,iPK_CommMethod,sPath.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PlugAndPlayAddDevice(int iPK_DeviceTemplate,string sIP_Address,string sTokens,string sPNPSerialNo,int iPK_PnpProtocol,string sIdentifier,int iCapabilities,int iPK_CommMethod,string sPath,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #799 - PlugAndPlayRemoveDevice */
	/** Disables the newly discovered device */
		/** @param #214 PNPSerialNo */
			/** The serial number detected by the PNP discovery layers */

	virtual void CMD_PlugAndPlayRemoveDevice(string sPNPSerialNo) { string sCMD_Result; CMD_PlugAndPlayRemoveDevice(sPNPSerialNo.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PlugAndPlayRemoveDevice(string sPNPSerialNo,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

	virtual void CheckQueue();

private:

	class PnPPrivate;
	PnPPrivate * d;
	
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
