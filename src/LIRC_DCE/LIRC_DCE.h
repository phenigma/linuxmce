/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef LIRC_DCE_h
#define LIRC_DCE_h

//	DCE Implemenation for #1862 mce_usb2

#include "Gen_Devices/LIRC_DCEBase.h"
//<-dceag-d-e->

#include "IRReceiverBase.h"
#include "DCE/DeviceData_Router.h"
#ifndef WIN32
#include <lirc/lirc_client.h>
#endif
class Database_pluto_main;


//<-dceag-decl-b->!
namespace DCE
{
	class LIRC_DCE : public LIRC_DCE_Command, IRReceiverBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
		int lirc_leech(int DeviceID);
		map<string,int> m_mapNameToDevice;

//<-dceag-const-b->!
public:
		// Constructors/Destructor
		LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~LIRC_DCE();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		Router *pRoute;
		int m_DeviceID;
		pthread_t m_LeechingThread;

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Device();
	bool DATA_Get_Ignore();
	string DATA_Get_System_Device();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
