//<-dceag-d-b->
#include "Powerfile_C200.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "exec_grab_output.h"
#include "PlutoUtils/StringUtils.h"
using namespace StringUtils;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Powerfile_C200::Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Powerfile_C200_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Powerfile_C200::Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Powerfile_C200_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Powerfile_C200::~Powerfile_C200()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Powerfile_C200::GetConfig()
{
	if( !Powerfile_C200_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	string sOutput;
	char * args[] = {"/usr/bin/lsscsi", "-g", NULL};
	if (! exec_output(args[0], args, sOutput))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get device names");
		return false;
	}

	vector<string> vect_sOutput_Rows;
	Tokenize(sOutput, "\n", vect_sOutput_Rows);
	int nDrive = 0;
	for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
	{
		vector<string> vect_sFields, vsFF; // FF = Filtered Fields
		vect_sFields.clear();
		vsFF.clear();
		Tokenize(vect_sOutput_Rows[i], " \t", vect_sFields);

		for (size_t j = 0; j < vect_sFields.size(); j++)
		{
			if (vect_sFields[j] == "")
				continue; // skip empty fields since there is no such thing
			vsFF.push_back(vect_sFields[j]);
		}
		if (vsFF[1] == "cd/dvd" && vsFF[2] == "TOSHIBA" && vsFF[3] == "DVD-ROM" && vsFF[4] == "SD-M1212")
		{
			if (nDrive > 1) // sanity check
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "I seem to have found more than 2 drives on this thing!");
				return false;
			}
			g_pPlutoLogger->Write(LV_WARNING, "Found DVD unit %d: %s", nDrive, vsFF[6].c_str());
			m_sDrive[nDrive] = vsFF[6];
			nDrive++;
		}
		else if (vsFF[1] == "mediumx" && vsFF[2] == "Escient" && vsFF[3] == "Powerfile" && vsFF[4] == "C200")
		{
			g_pPlutoLogger->Write(LV_WARNING, "Found changer unit: %s", vsFF[7].c_str());
			m_sChanger = vsFF[7];
		}
	}
	g_pPlutoLogger->Write(LV_STATUS, "Finished config");
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Powerfile_C200::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Powerfile_C200_Command *Create_Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Powerfile_C200(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Powerfile_C200::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Powerfile_C200::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Powerfile_C200::SomeFunction()
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

//<-dceag-c701-b->

	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc to */

void Powerfile_C200::CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c701-e->
{
	cout << "Need to implement command #701 - Load from Slot into Drive" << endl;
	cout << "Parm #151 - Slot_Number=" << iSlot_Number << endl;
	cout << "Parm #152 - Drive_Number=" << iDrive_Number << endl;
}

//<-dceag-c702-b->

	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc from */

void Powerfile_C200::CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c702-e->
{
	cout << "Need to implement command #702 - Unload from Drive into Slot" << endl;
	cout << "Parm #151 - Slot_Number=" << iSlot_Number << endl;
	cout << "Parm #152 - Drive_Number=" << iDrive_Number << endl;
}

//<-dceag-c703-b->

	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

void Powerfile_C200::CMD_Get_Jukebox_Status(string *sJukebox_Status,string &sCMD_Result,Message *pMessage)
//<-dceag-c703-e->
{
	cout << "Need to implement command #703 - Get Jukebox Status" << endl;
	cout << "Parm #153 - Jukebox_Status=" << sJukebox_Status << endl;
}
