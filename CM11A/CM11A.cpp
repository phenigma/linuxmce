//<-dceag-d-b->
#include "CM11A.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter/DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "cm11aconsts.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
CM11A::CM11A(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: CM11A_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
CM11A::~CM11A()
//<-dceag-dest-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"Destroying CM11A");
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CM11A::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool CM11A::Connect(int iPK_DeviceTemplate) {
	if(!CM11A_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}
	
	devpoll.Run(false);
	return true;
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void CM11A::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Command %d received for child.", pMessage->m_dwID);
	
	
	// find child device
	DeviceData_Impl* pDeviceData_Impl = NULL;
	
	VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
	for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		if(vDeviceData[i]->m_dwPK_Device == pMessage->m_dwPK_Device_To) {
			pDeviceData_Impl = vDeviceData[i];
			break;
		}
	}
	
	if(!pDeviceData_Impl) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Child device %d not found.", pMessage->m_dwPK_Device_To);
		return;
	}
	
	string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Channel_CONST);
	g_pPlutoLogger->Write(LV_STATUS, "Child device %d has channel %s.", pMessage->m_dwPK_Device_To, sChannel.c_str());
	
	switch(pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST: {
				CM11ADEV::Message msg;
				msg.setAddress(sChannel);
				msg.setFunctionCode(CM11A_FUNC_0N);
				devpoll.SendRequest(&msg);
			}
			break;
		case COMMAND_Generic_Off_CONST: {
				CM11ADEV::Message msg;
				msg.setAddress(sChannel);
				msg.setFunctionCode(CM11A_FUNC_0FF);
				devpoll.SendRequest(&msg);
			}
			break;
		case COMMAND_Set_Level_CONST: {
				CM11ADEV::Message msg;
				msg.setAddress(sChannel);
				msg.setFunctionCode(CM11A_FUNC_DIMM);
				
				string sDimmLevel = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST];
				msg.setDimmLevel(atoi(sDimmLevel.c_str()));
				
				devpoll.SendRequest(&msg);
			}
			break;
		default:
			g_pPlutoLogger->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
	}
		
//	string sSrcPhone = pDeviceData_Base->mapParameters_Find(1);
	
	sCMD_Result = "OK";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void CM11A::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void CM11A::SomeFunction()
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


