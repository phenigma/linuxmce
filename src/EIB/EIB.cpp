//<-dceag-d-b->
#include "EIB.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "telegrammessage.h"

using namespace EIBBUS;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
EIB::EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: EIB_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
EIB::EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: EIB_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
EIB::~EIB()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool EIB::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool EIB::Connect(int iPK_DeviceTemplate) {
	if(!EIB_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}

//	m_msgPool.regInterceptor(this);
	m_msgPool.Run(false);
	
/*	
	TelegramMessage tlmsg;
	tlmsg.setActionType(TelegramMessage::READ);
//	tlmsg.setActionType(TelegramMessage::WRITE);
	tlmsg.setShortUserData(1);
	tlmsg.setGroupAddress("1/0/0");
//	tlmsg.setUserData("1");
	m_msgPool.sendTelegram(&tlmsg); 
	
	TelegramMessage tlmsg1;
	while(1) {
		m_msgPool.readTelegram(&tlmsg1);
		if(tlmsg1.getActionType() == TelegramMessage::ANSWER) {
			g_pPlutoLogger->Write(LV_STATUS, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Value for %s is: %d", 
			tlmsg1.getGroupAddress(), tlmsg1.getShortUserData());
		}
	}
*/	
	
	return true;
}


/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
EIB_Command *Create_EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new EIB(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void EIB::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void EIB::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void EIB::handleTelegram(const TelegramMessage *pt) {
/*	g_pPlutoLogger->Write(LV_STATUS, "Processing received Telegram...");
	if(pt->getActionType() == TelegramMessage::ANSWER) {
		g_pPlutoLogger->Write(LV_STATUS, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!: %d", pt->getShortUserData());
	}*/
}


//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void EIB::SomeFunction()
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



//<-dceag-c273-b->

	/** @brief COMMAND: #273 - EIB_Write */
	/** Peforms a WRITE request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */
		/** @param #109 Data */
			/** EIB Data */
		/** @param #110 DataType */
			/** EIB Data Type:
[1]-switch (1b)
[2]-dimming-control (4b)
[3]-time (3B)
[4]-date (4B)
[5]-value (2B)
[6]-scaling (1B)
[7]-drive control
[9]-float
[10]-16bit counter
[11]-32bit counter
[13]-ASCII character
[14]-8bit counter
[15]-character string */

void EIB::CMD_EIB_Write(string sAddress,string sData,int iDataType,string &sCMD_Result,Message *pMessage)
//<-dceag-c273-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Received a Write request: (To= %s, Data Type=%d, Data=%s) ", 
												sAddress.c_str(), iDataType, sData.c_str());
	
	if(sAddress.length() == 0) {
		g_pPlutoLogger->Write(LV_STATUS, "Empty Group Address passed as parameter");
		return;
	}
												
	TelegramMessage tlmsg;
	tlmsg.setGroupAddress(sAddress.c_str());
	
	switch(iDataType) {
	case DT_SWITCH:
	case DT_DIMMING_CONTROL:
	case DT_DRIVE_CONTROL:
	case 8:
		tlmsg.setShortUserData(atoi(sData.c_str()));
		break;
		
	case DT_SCALING:
	case DT_ASCIICHAR:
	case DT_COUNTER8: {
		unsigned char data = atoi(sData.c_str());
		tlmsg.setUserData(&data, 1);
		} break;
		
	case DT_VALUE:
	case DT_COUNTER16: {
		unsigned short data = atoi(sData.c_str());
		tlmsg.setUserData((unsigned char*)&data, 2);
		} break;
	
	case DT_TIME:
	case DT_DATE: {
		unsigned int data = atoi(sData.c_str());
		tlmsg.setUserData((unsigned char*)&data, 3);
		} break;
	
	case DT_FLOAT:
	case DT_COUNTER32:
	case 12: {
		unsigned int data = atoi(sData.c_str());
		tlmsg.setUserData((unsigned char*)&data, 4);
		} break;
	
	case DT_STRING: {
		tlmsg.setUserData((unsigned char*)sData.c_str(), sData.length());
		} break;
	
	default:
		g_pPlutoLogger->Write(LV_WARNING, "Unknown TYPE specified for Write request");
		return;
	}
	
	m_msgPool.sendTelegram(&tlmsg);
}
//<-dceag-c275-b->

	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

void EIB::CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage)
//<-dceag-c275-e->
