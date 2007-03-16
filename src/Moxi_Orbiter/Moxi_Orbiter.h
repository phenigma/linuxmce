/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//!<-dceag-d-b->
#ifndef Moxi_Orbiter_h
#define Moxi_Orbiter_h

//	DCE Implemenation for #1872 Moxi Orbiter

#include "../Orbiter/Orbiter.h"
//<-dceag-d-e->

//!<-dceag-decl-b->
namespace DCE
{
	class Moxi_Orbiter : public Orbiter
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//!<-dceag-const-b->
public:
		// Constructors/Destructor
		Moxi_Orbiter(int DeviceID,  int PK_DeviceTemplate, string ServerAddress);
		virtual ~Moxi_Orbiter();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_Distro();
	bool DATA_Get_Development();
	bool DATA_Get_No_Effects();
	int DATA_Get_PK_Skin();
	int DATA_Get_PK_Size();
	int DATA_Get_PK_Language();
	string DATA_Get_FK_EntertainArea();
	string DATA_Get_Timeout();
	string DATA_Get_CacheFolder();
	int DATA_Get_CacheSize();
	bool DATA_Get_Use_OCG_Format();
	int DATA_Get_VideoFrameInterval();
	int DATA_Get_ImageQuality();
	void DATA_Set_ImageQuality(int Value);
	string DATA_Get_PK_Device();
	bool DATA_Get_Ignore();
	bool DATA_Get_Dont_Auto_Jump_to_Remote();
	int DATA_Get_ScreenWidth();
	int DATA_Get_ScreenHeight();
	int DATA_Get_Rotation();
	int DATA_Get_PK_Screen();
	bool DATA_Get_Get_Time_Code_for_Media();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
