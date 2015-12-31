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
#ifndef LMCE_Launch_Manager_h
#define LMCE_Launch_Manager_h

//	DCE Implemenation for #1888 LMCE Launch Manager

#include "Gen_Devices/LMCE_Launch_ManagerBase.h"
//<-dceag-d-e->

class LM;

//<-dceag-decl-b->
namespace DCE
{
	class LMCE_Launch_Manager : public LMCE_Launch_Manager_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
		LM *lmWidget; 	// The cross link back to the main program.

//<-dceag-const-b->
public:
		// Constructors/Destructor
		LMCE_Launch_Manager(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~LMCE_Launch_Manager();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void CreateNewChildren();	// Dealing with spawning new devices.

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		LMCE_Launch_Manager(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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


	/** @brief COMMAND: #912 - Activate PC Desktop */
	/** This is here so programs can send commands to the Launch Manager to switch between the Orbiter and the PC Desktop */
		/** @param #119 True/False */
			/** If true, switch to the last PC desktop.  If false, switch to Orbiter's desktop */

	virtual void CMD_Activate_PC_Desktop(bool bTrueFalse) { string sCMD_Result; CMD_Activate_PC_Desktop(bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
