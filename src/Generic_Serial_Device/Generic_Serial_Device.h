/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#ifndef Generic_Serial_Device_h
#define Generic_Serial_Device_h

//	DCE Implemenation for #1632 Link45i

#include "Gen_Devices/Generic_Serial_DeviceBase.h"
//<-dceag-d-e->

#include "MessageTranslation/AVMessageTranslation.h"
#include "Serial/GenericIODevice.h"

#include "GSDMessageProcessing.h"

class Database_pluto_main;

//<-dceag-decl-b->!
namespace DCE
{
	class Generic_Serial_Device : 
			public Generic_Serial_Device_Command, 
			public GSDMessageProcessor
	{
//<-dceag-decl-e->
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Serial_Device();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual bool Connect(int iPK_DeviceTemplate );
//		virtual bool ReceivedMessage(class Message *pMessageOriginal);
		virtual void Transmit(const char *pData,int iSize);

		virtual void RunThread();
		virtual void ParseDeviceHierarchy(DeviceData_Impl *pdevdata = NULL);
		
//<-dceag-const2-b->!

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


	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

	virtual void CMD_Process_Incoming_Data() { string sCMD_Result; CMD_Process_Incoming_Data(sCMD_Result,NULL);};
	virtual void CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

	virtual void CMD_Process_IDLE() { string sCMD_Result; CMD_Process_IDLE(sCMD_Result,NULL);};
	virtual void CMD_Process_IDLE(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

	virtual void CMD_Process_Initialize() { string sCMD_Result; CMD_Process_Initialize(sCMD_Result,NULL);};
	virtual void CMD_Process_Initialize(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

	virtual void CMD_Process_Release() { string sCMD_Result; CMD_Process_Release(sCMD_Result,NULL);};
	virtual void CMD_Process_Release(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

	virtual void CMD_Private_Method_Listing() { string sCMD_Result; CMD_Private_Method_Listing(sCMD_Result,NULL);};
	virtual void CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

	virtual void CMD_Process_Receive_Command_For_Child() { string sCMD_Result; CMD_Process_Receive_Command_For_Child(sCMD_Result,NULL);};
	virtual void CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

protected:
		virtual void DispatchMessage(Message* pmsg); 
/*
public:
		DeviceData_Impl* RecursiveFindChildDevice(unsigned dwPK_Device, DeviceData_Impl* pDeviceData_Impl);
*/

private:
		Database_pluto_main *m_pdbPlutoMain;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
