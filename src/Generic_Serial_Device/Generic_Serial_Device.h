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

//	DCE Implemenation for #74 Generic Serial Device

#include "Gen_Devices/Generic_Serial_DeviceBase.h"
//<-dceag-d-e->
#include "Generic_Serial_Device_MessageProcessor.h"

#include "Serial/GenericIODevice.h"

class Database_pluto_main;

//<-dceag-decl-b->!
namespace DCE
{
	class Generic_Serial_Device : public Generic_Serial_Device_Command, public Generic_Serial_Device_MessageProcessor
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
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual bool Connect(int iPK_DeviceTemplate );
//		virtual bool ReceivedMessage(class Message *pMessageOriginal);
		virtual void Transmit(const char *pData,int iSize);

		virtual void RunThread();
		
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


//<-dceag-h-e->

protected:
		virtual void DispatchMessage(Message* pmsg); /*overriden from MessageProcessor*/
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
