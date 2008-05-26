/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file Generic_Serial_Device.h
A core piece of LinuxMCE.

Requires 2 other modules to be present:
<ul>
<li>MessageTranslation
<li>Serial
<li>DCE
<li>PlutoUtils
<li>pluto_main
</ul>
*/
//<-dceag-d-b->
#ifndef Generic_Serial_Device_h
#define Generic_Serial_Device_h

//	DCE Implemenation for #1932 Insteon PLM

#include "Gen_Devices/Generic_Serial_DeviceBase.h"
//<-dceag-d-e->

#include "MessageTranslation/AVMessageTranslation.h"
#include "Serial/GenericIODevice.h"

#include "GSDMessageProcessing.h"


//<-dceag-decl-b->!

/** @namespace DCE
The Data Commands and Events (DCE) namespace.

The "Generic Serial Device" module (GSD) is designed for the thousands
of devices that are controlled over RS232 and Ethernet.
It allows a LinuxMCE driver to be added using a point-and-click,
fill-in-the-blanks interface.
Basic protocols can be implemented with no programming at all.

For more complicated tasks GSD embeds the Ruby language.
Ruby is a high-level interpreted language with flexible string handling,
like Perl, but the syntax is simpler.
The protocol and embedded Ruby for GSD devices is stored in the database
and automatically synchronized with all other LinuxMCE systems as long
as you leave checked the "Share my IR codes and GSD device" box on
the A/V Equipment wizard.
So there's no compiling and no code to check in.

GSD devices that control a/v equipment, lighting, etc., can be done
in as little as an hour or two, almost never more than a day for
complex protocols.
*/
namespace DCE
{

/** @class Generic_Serial_Device
A class for adding support for new devices.

The "Generic Serial Device" module (GSD) is designed for the thousands
of devices that are controlled over RS232 and Ethernet.
It allows a LinuxMCE driver to be added using a point-and-click,
fill-in-the-blanks interface.
Basic protocols can be implemented with no programming at all.

For more complicated tasks GSD embeds the Ruby language.
Ruby is a high-level interpreted language with flexible string handling,
like Perl, but the syntax is simpler.
The protocol and embedded Ruby for GSD devices is stored in the database
and automatically synchronized with all other LinuxMCE systems as long
as you leave checked the "Share my IR codes and GSD device" box on
the A/V Equipment wizard.
So there's no compiling and no code to check in.

GSD devices that control a/v equipment, lighting, etc., can be done
in as little as an hour or two, almost never more than a day for
complex protocols.
*/
	class Generic_Serial_Device :
			public Generic_Serial_Device_Command,
			public GSDMessageProcessor
	{
//<-dceag-decl-e->
		// Private methods
public:
		// Public member variables
		/** Logging thread */
		static pthread_t m_LoggingThread;
		static bool m_bStopLogging;
		static int m_FdPipe[2];

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Serial_Device();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

        /** Connect the serial device
        */
		virtual bool Connect(int iPK_DeviceTemplate );

        // Unused
//		virtual bool ReceivedMessage(class Message *pMessageOriginal);

        /** Transmit.

        Currently does nothing.
        */
		virtual void Transmit(const char *pData,int iSize);

        /** Split off a thread and run it.
        */
		virtual void RunThread();

        /** Parse
        */
		virtual void ParseDeviceHierarchy(DeviceData_Impl *pdevdata = NULL);

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	string DATA_Get_COM_Port_BaudRate();
	void DATA_Set_COM_Port_BaudRate(string Value,bool bUpdateDatabase=false);

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


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters) { string sCMD_Result; CMD_Send_Command_To_Child(sID.c_str(),iPK_Command,sParameters.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

	virtual void CMD_Reset(string sArguments) { string sCMD_Result; CMD_Reset(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

	virtual void CMD_StatusReport(string sArguments) { string sCMD_Result; CMD_StatusReport(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

protected:
		virtual void DispatchMessage(Message* pmsg);
/*
public:
		DeviceData_Impl* RecursiveFindChildDevice(unsigned dwPK_Device, DeviceData_Impl* pDeviceData_Impl);
*/

	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
