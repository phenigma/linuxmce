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
#include "General_Info_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

struct ChildDeviceProcessing
{
	string m_sChildren;
	int m_nPK_Device;

	ChildDeviceProcessing(int nPK_Device, string sChildren) 
	{ 
		m_nPK_Device = nPK_Device; 
		m_sChildren = sChildren; 
	}
};

#define PROCESS_CHILD_DEVICES 1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
General_Info_Plugin::General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: General_Info_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pAlarmManager=NULL;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
General_Info_Plugin::General_Info_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: General_Info_Plugin_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
General_Info_Plugin::~General_Info_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool General_Info_Plugin::GetConfig()
{
	if( !General_Info_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);      // number of worker threads
	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool General_Info_Plugin::Register()
//<-dceag-reg-e->
{
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::ReportingChildDevices ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Reporting_Child_Devices_CONST );

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
General_Info_Plugin_Command *Create_General_Info_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new General_Info_Plugin(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void General_Info_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void General_Info_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void General_Info_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c68-b->

	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

void General_Info_Plugin::CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c68-e->
{
	cout << "Need to implement command #68 - Get Device Data" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #52 - PK_DeviceData=" << iPK_DeviceData << endl;
	cout << "Parm #53 - UseDefault=" << bUseDefault << endl;
}

//<-dceag-c71-b->

	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

void General_Info_Plugin::CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c71-e->
{
	cout << "Need to implement command #71 - Request File" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
}

//<-dceag-c239-b->

	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

void General_Info_Plugin::CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage)
//<-dceag-c239-e->
{
	cout << "Need to implement command #239 - Request File And Checksum" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #91 - Checksum=" << sChecksum << endl;
	cout << "Parm #92 - Checksum_Only=" << bChecksum_Only << endl;
}

//<-dceag-c246-b->

	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

void General_Info_Plugin::CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage)
//<-dceag-c246-e->
{
	cout << "Need to implement command #246 - Set Device Data" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #52 - PK_DeviceData=" << iPK_DeviceData << endl;
}

//<-dceag-c247-b->

	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The state data for the device. */

void General_Info_Plugin::CMD_Get_Device_State(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c247-e->
{
	cout << "Need to implement command #247 - Get Device State" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
}

//<-dceag-c248-b->

	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */

void General_Info_Plugin::CMD_Get_Device_Status(string &sCMD_Result,Message *pMessage)
//<-dceag-c248-e->
{
	cout << "Need to implement command #248 - Get Device Status" << endl;
}

//<-dceag-c272-b->

	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

void General_Info_Plugin::CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c272-e->
{
	cout << "Need to implement command #272 - Restart DCERouter" << endl;
	cout << "Parm #21 - Force=" << sForce << endl;
}

//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device, or if negative, this is a PK_PnpQueue and needs to be forwarded to plug and play plugin */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

void General_Info_Plugin::CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
	cout << "Need to implement command #274 - Set Room For Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #50 - Name=" << sName << endl;
	cout << "Parm #57 - PK_Room=" << iPK_Room << endl;
}

//<-dceag-c322-b->

	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

void General_Info_Plugin::CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c322-e->
{
	cout << "Need to implement command #322 - Wake Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
}

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */
		/** @param #47 Mac address */
			/** If PK_Device is not specified (is 0), we'll use the mac address to determine the device id */

void General_Info_Plugin::CMD_Halt_Device(int iPK_Device,string sForce,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{
	cout << "Need to implement command #323 - Halt Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #21 - Force=" << sForce << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
}

//<-dceag-c365-b->

	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

void General_Info_Plugin::CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c365-e->
{
	cout << "Need to implement command #365 - Get Room Description" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #57 - PK_Room=" << iPK_Room << endl;
}

//<-dceag-c371-b->

	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

void General_Info_Plugin::CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c371-e->
{
	cout << "Need to implement command #371 - Is Daytime" << endl;
	cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
}

//<-dceag-c395-b->

	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

void General_Info_Plugin::CMD_Check_for_updates(string &sCMD_Result,Message *pMessage)
//<-dceag-c395-e->
{
	cout << "Need to implement command #395 - Check for updates" << endl;
}

//<-dceag-c396-b->

	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */
		/** @param #230 Failed */
			/** This is true if the script used to check for updates fails. */

void General_Info_Plugin::CMD_Check_for_updates_done(bool bFailed,string &sCMD_Result,Message *pMessage)
//<-dceag-c396-e->
{
	cout << "Need to implement command #396 - Check for updates done" << endl;
	cout << "Parm #230 - Failed=" << bFailed << endl;
}

//<-dceag-c718-b->

	/** @brief COMMAND: #718 - Create Device */
	/** Creates a new device of the given template */
		/** @param #2 PK_Device */
			/** The new device number */
		/** @param #44 PK_DeviceTemplate */
			/** The template */
		/** @param #47 Mac address */
			/** The mac address */
		/** @param #57 PK_Room */
			/** The room for the device.  0=no room, -1=ask user */
		/** @param #58 IP Address */
			/** The IP of the device */
		/** @param #109 Data String */
			/** Extra device data to be assigned when creating the device. It should look like this:
devicedata_id1|devicedata_value1|devicedata_id2|devicedata_value2| etc. */
		/** @param #150 PK_DHCPDevice */
			/** Only needed if this is a dhcp pnp device */
		/** @param #156 PK_Device_ControlledVia */
			/** The controlled via */
		/** @param #163 Description */
			/** The name for the device */
		/** @param #198 PK_Orbiter */
			/** The orbiter which should be used to prompt the user for any extra information.  Zero means all orbiters */
		/** @param #201 PK_Device_Related */
			/** Will make the new device relate to this one if possible */

void General_Info_Plugin::CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,string sDescription,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c718-e->
{
	cout << "Need to implement command #718 - Create Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #44 - PK_DeviceTemplate=" << iPK_DeviceTemplate << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
	cout << "Parm #57 - PK_Room=" << iPK_Room << endl;
	cout << "Parm #58 - IP_Address=" << sIP_Address << endl;
	cout << "Parm #109 - Data_String=" << sData_String << endl;
	cout << "Parm #150 - PK_DHCPDevice=" << iPK_DHCPDevice << endl;
	cout << "Parm #156 - PK_Device_ControlledVia=" << iPK_Device_ControlledVia << endl;
	cout << "Parm #163 - Description=" << sDescription << endl;
	cout << "Parm #198 - PK_Orbiter=" << iPK_Orbiter << endl;
	cout << "Parm #201 - PK_Device_Related=" << iPK_Device_Related << endl;
}

//<-dceag-c719-b->

	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

void General_Info_Plugin::CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c719-e->
{
	cout << "Need to implement command #719 - Delete Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
}

//<-dceag-c752-b->

	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

void General_Info_Plugin::CMD_Check_Mounts(string &sCMD_Result,Message *pMessage)
//<-dceag-c752-e->
{
	cout << "Need to implement command #752 - Check Mounts" << endl;
}

//<-dceag-c765-b->

	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

void General_Info_Plugin::CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage)
//<-dceag-c765-e->
{
	cout << "Need to implement command #765 - Set Device Relations" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #103 - List_PK_Device=" << sList_PK_Device << endl;
	cout << "Parm #205 - Reverse=" << bReverse << endl;
}

//<-dceag-c789-b->

	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

void General_Info_Plugin::CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage)
//<-dceag-c789-e->
{
	cout << "Need to implement command #789 - Force Update Packages" << endl;
}

//<-dceag-c791-b->

	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

void General_Info_Plugin::CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c791-e->
{
	cout << "Need to implement command #791 - Set Enable Status" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c800-b->

	/** @brief COMMAND: #800 - InitAVDeviceTemplateSettings */
	/** Save default values in database for a deviceTemplate */
		/** @param #44 PK_DeviceTemplate */
			/** Template id */

void General_Info_Plugin::CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c800-e->
{
	cout << "Need to implement command #800 - InitAVDeviceTemplateSettings" << endl;
	cout << "Parm #44 - PK_DeviceTemplate=" << iPK_DeviceTemplate << endl;
}

//<-dceag-c802-b->

	/** @brief COMMAND: #802 - Get Available Storage Device */
	/** The the available storage devices */
		/** @param #2 PK_Device */
			/** The device id for the NAS or internal drive being used.  If it will be stored on the Core's internal home directory, this will be the device id for the Core.  0 is no device can save the file. */
		/** @param #163 Description */
			/** The descripition for the device being used for the storage (ie Core, or a device name). */
		/** @param #219 Path */
			/** Returns the fully qualified path to prepend to the filename.  If this is empty, that means there is no device which can save this file */
		/** @param #222 Size */
			/** The expected size of the file in MB.  If specified, only a device with enough space and which can handle it (ie some NAS don't do big files).  If not specified, returns device with the most free space. */

void General_Info_Plugin::CMD_Get_Available_Storage_Device(int iSize,int *iPK_Device,string *sDescription,string *sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c802-e->
{
	cout << "Need to implement command #802 - Get Available Storage Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #163 - Description=" << sDescription << endl;
	cout << "Parm #219 - Path=" << sPath << endl;
	cout << "Parm #222 - Size=" << iSize << endl;
}

//<-dceag-c803-b->

	/** @brief COMMAND: #803 - Blacklist Internal Disk Drive */
	/** Adds an internal disk drive device to the list of blacklisted drives. */
		/** @param #156 PK_Device_ControlledVia */
			/** The 'computer' device that contains the blacklisted drive. */
		/** @param #223 Block Device */
			/** Linux block device representing the Internal Disk Drive. */

void General_Info_Plugin::CMD_Blacklist_Internal_Disk_Drive(int iPK_Device_ControlledVia,string sBlock_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c803-e->
{
	cout << "Need to implement command #803 - Blacklist Internal Disk Drive" << endl;
	cout << "Parm #156 - PK_Device_ControlledVia=" << iPK_Device_ControlledVia << endl;
	cout << "Parm #223 - Block_Device=" << sBlock_Device << endl;
}

//<-dceag-c808-b->

	/** @brief COMMAND: #808 - Get Unused Serial Ports */
	/** Get the unused serial ports */
		/** @param #2 PK_Device */
			/** The computer to get the ports for */
		/** @param #5 Value To Assign */
			/** A comma delimited list of ports */

void General_Info_Plugin::CMD_Get_Unused_Serial_Ports(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c808-e->
{
	cout << "Need to implement command #808 - Get Unused Serial Ports" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
}

//<-dceag-c813-b->

	/** @brief COMMAND: #813 - Add Software */
	/** Install Software on media director */
		/** @param #2 PK_Device */
			/** The MD to install it on */
		/** @param #119 True/False */
			/** True = add the package, false=remove it */
		/** @param #229 PK_Software */
			/** ID of installing package */

void General_Info_Plugin::CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software,string &sCMD_Result,Message *pMessage)
//<-dceag-c813-e->
{
	cout << "Need to implement command #813 - Add Software" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
	cout << "Parm #229 - PK_Software=" << iPK_Software << endl;
}

//<-dceag-c833-b->

	/** @brief COMMAND: #833 - Get User Name */
	/** Given a PK_User, get the users name */
		/** @param #5 Value To Assign */
			/** The user's name */
		/** @param #17 PK_Users */
			/** The user id */

void General_Info_Plugin::CMD_Get_User_Name(int iPK_Users,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c833-e->
{
	cout << "Need to implement command #833 - Get User Name" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
}

//<-dceag-c835-b->

	/** @brief COMMAND: #835 - Get Network Devices Shares */
	/** Get the list with shares for all network devices from this installation. */
		/** @param #244 Custom Response */
			/** Contains the following info:

device id, device ip, share name, user name, password, free space.

Delimiter: '\n' */

void General_Info_Plugin::CMD_Get_Network_Devices_Shares(char **pCustom_Response,int *iCustom_Response_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c835-e->
{
	cout << "Need to implement command #835 - Get Network Devices Shares" << endl;
	cout << "Parm #244 - Custom_Response  (data value)" << endl;
}

//<-dceag-c843-b->

	/** @brief COMMAND: #843 - RemoteAssistance_SetStatus */
	/** Change the state of Remote Assistance */
		/** @param #208 Enable */
			/** Desired state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_SetStatus(bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c843-e->
{
	cout << "Need to implement command #843 - RemoteAssistance_SetStatus" << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c844-b->

	/** @brief COMMAND: #844 - RemoteAssistance_GetStatus */
	/** Get the state of Remote Assistance */
		/** @param #208 Enable */
			/** The state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_GetStatus(bool *bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c844-e->
{
	cout << "Need to implement command #844 - RemoteAssistance_GetStatus" << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void General_Info_Plugin::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
	cout << "Need to implement command #882 - Abort Task" << endl;
	cout << "Parm #248 - Parameter_ID=" << iParameter_ID << endl;
}

//<-dceag-c915-b->

	/** @brief COMMAND: #915 - Enable Device */
	/** Called when instead of creating a device, we re-enabled one that had been disabled and need to call the postcreateoptions again */
		/** @param #2 PK_Device */
			/** The device that was enabled */
		/** @param #198 PK_Orbiter */
			/** The Orbiter that did it, if any */

void General_Info_Plugin::CMD_Enable_Device(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c915-e->
{
	cout << "Need to implement command #915 - Enable Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #198 - PK_Orbiter=" << iPK_Orbiter << endl;
}

//<-dceag-c934-b->

	/** @brief COMMAND: #934 - Get Home Symlink */
	/** Get home symlink */
		/** @param #219 Path */
			/** The physical path like /mnt/device/<deviceid> */
		/** @param #275 Symlink */
			/** The symlink like /home/public/data ... */

void General_Info_Plugin::CMD_Get_Home_Symlink(string sPath,string *sSymlink,string &sCMD_Result,Message *pMessage)
//<-dceag-c934-e->
{
	cout << "Need to implement command #934 - Get Home Symlink" << endl;
	cout << "Parm #219 - Path=" << sPath << endl;
	cout << "Parm #275 - Symlink=" << sSymlink << endl;
}

bool General_Info_Plugin::ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sError = pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
	if( sError.size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d failed to report its children: %s",
			pMessage->m_dwPK_Device_From,sError.c_str());
		return false;
	}

	string sChildren = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	ChildDeviceProcessing *pChildDeviceProcessing = new ChildDeviceProcessing(pMessage->m_dwPK_Device_From,sChildren);  // Do this in a separate thread since create_device is slow and we don't want to block teh socket
	m_pAlarmManager->AddRelativeAlarm(1,this,PROCESS_CHILD_DEVICES,(void *) pChildDeviceProcessing);
	return false;
}

void General_Info_Plugin::AlarmCallback(int id, void* param)
{
	if( id==PROCESS_CHILD_DEVICES )
		ReportingChildDevices_Offline(param);
}

void General_Info_Plugin::ReportingChildDevices_Offline( void *pVoid )
{
	ChildDeviceProcessing *pChildDeviceProcessing = (ChildDeviceProcessing *) pVoid;

	map<int,bool> mapCurrentChildren;
	vector<string> vectLines;
	StringUtils::Tokenize(pChildDeviceProcessing->m_sChildren,"\n",vectLines);
	for(size_t s=0;s<vectLines.size();++s)
	{
		// This will add the child device if it doesn't exist
		int nPK_Device_Child = ProcessChildDevice(pChildDeviceProcessing->m_nPK_Device,vectLines[s]);
		if(0 != nPK_Device_Child)
			mapCurrentChildren[nPK_Device_Child]=true;
	}
/*
	// See if any child devices have since disappeared
	string sSQL = "FK_Device_ControlledVia=" + StringUtils::itos(pChildDeviceProcessing->m_pRow_Device->PK_Device_get());
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		if( pRow_Device->FK_Device_RouteTo_isNull()==false )
		{
			// Skip embedded children, iterate through their children
			sSQL = "FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get());
			vector<Row_Device *> vectRow_Device_Children;
			m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device_Children);
			for(size_t s=0;s<vectRow_Device_Children.size();++s)
			{
				Row_Device *pRow_Device_Child = vectRow_Device_Children[s];
				if( mapCurrentChildren[pRow_Device_Child->PK_Device_get()]==false )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead embedded device %d %s",
						pRow_Device_Child->PK_Device_get(),pRow_Device_Child->Description_get().c_str());
					CMD_Delete_Device(pRow_Device_Child->PK_Device_get());
				}
			}
			continue; // Don't delete the embedded device
		}

		if( mapCurrentChildren[pRow_Device->PK_Device_get()]==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead device %d %s",
				pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str());
			CMD_Delete_Device(pRow_Device->PK_Device_get());
		}
	}
	m_pDatabase_pluto_main->Device_get()->Commit();
	m_pDatabase_pluto_main->Device_DeviceData_get()->Commit();

	delete pChildDeviceProcessing;
*/
}

// It will be like this:
// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \t [PK_DeviceData] \t [Value] ... \n

int General_Info_Plugin::ProcessChildDevice(int nPK_Device, string sLine)
{
	string::size_type pos=0;
	string sInternalID = StringUtils::Tokenize(sLine,"\t",pos);
	string sDescription = StringUtils::Tokenize(sLine,"\t",pos);
	string sRoomName = StringUtils::Tokenize(sLine,"\t",pos);
	int PK_DeviceTemplate = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
	string sPK_FloorplanObjectType = StringUtils::Tokenize(sLine,"\t",pos);
/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ProcessChildDevice: pos=%d, %s", pos, sLine.c_str());
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(PK_DeviceTemplate);
	if( !pRow_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice Line %s malformed",sLine.c_str());
		return NULL;
	}

	// Find the child device with this internal id
	vector<Row_Device *> vectRow_Device_Child;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN Device_DeviceData ON FK_Device=Device.PK_Device "
		" LEFT JOIN Device AS Device_Parent ON Device.FK_Device_ControlledVia = Device_Parent.PK_Device "
		" WHERE (Device.FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " OR Device_Parent.FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get()) + ") "
		" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) +
		" AND IK_DeviceData='" + StringUtils::SQLEscape(sInternalID) + "'",&vectRow_Device_Child);

	bool bCreatedNew=false;
	Row_Device *pRow_Device_Child;
	if( vectRow_Device_Child.size() )
	{
		pRow_Device_Child = vectRow_Device_Child[0];
		pRow_Device_Child->Reload();   // Don't overwrite the room or other data that may already be there
		if( pRow_Device_Child->FK_DeviceTemplate_get()==PK_DeviceTemplate )
			return pRow_Device_Child; // For the time being, don't do anything because it's resetting the device's psc_mod causing orbiter to report the router needs a reload
		else
			CMD_Delete_Device(pRow_Device_Child->PK_Device_get()); // If the device template has changed, delete the device and recreate it
	}

	// Create it since it doesn't exist
	int iPK_Device;
	bCreatedNew=true;
	CMD_Create_Device(PK_DeviceTemplate,"",0,"",
		StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID,
		0,pRow_Device->PK_Device_get(),"",0,0,&iPK_Device);
	pRow_Device_Child = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( !pRow_Device_Child )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice failed to create child %d",iPK_Device);
		return NULL;
	}

	// Don't reset the description if it's already there, the user may have overridden the default name
	if( pRow_Device_Child->Description_get().size()==0 )
	{
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sDescription);
		else
			pRow_Device_Child->Description_set(sInternalID);
	}
	else if( bCreatedNew )
	{
		string sOldDescription = pRow_Device_Child->Description_get();
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sOldDescription + " " + sDescription);
		else
			pRow_Device_Child->Description_set(sOldDescription + " " + sInternalID);
	}
	vector<Row_Room *> vectRow_Room;
	m_pDatabase_pluto_main->Room_get()->GetRows("Description like '" + StringUtils::SQLEscape(sRoomName) + "'",&vectRow_Room);
	if( vectRow_Room.size() )
		pRow_Device_Child->FK_Room_set( vectRow_Room[0]->PK_Room_get() );
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d %s in unknown room %s",
		pRow_Device_Child->PK_Device_get(),sDescription.c_str(),sRoomName.c_str());

	pRow_Device_Child->FK_DeviceTemplate_set( PK_DeviceTemplate );

	if( atoi(sPK_FloorplanObjectType.c_str())>0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessChildDevice setting sPK_FloorplanObjectType to %s for %d",
			sPK_FloorplanObjectType.c_str(), pRow_Device_Child->PK_Device_get());
		Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device_Child->PK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
		if( !pRow_Device_DeviceData )
		{
			pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
			pRow_Device_DeviceData->FK_Device_set(pRow_Device_Child->PK_Device_get());
			pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_PK_FloorplanObjectType_CONST);
		}

		pRow_Device_DeviceData->IK_DeviceData_set( sPK_FloorplanObjectType );
	}

	while(pos<sLine.size())
	{
		int PK_DeviceData = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
		string sValue = StringUtils::Tokenize(sLine,"\t",pos);
		if( PK_DeviceData )
			DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device_Child->PK_Device_get(),PK_DeviceData,sValue);
	}

	return pRow_Device_Child;
*/

	return 0;
}