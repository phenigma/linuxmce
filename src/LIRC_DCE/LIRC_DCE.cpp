//<-dceag-d-b->
#include "LIRC_DCE.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
LIRC_DCE::LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: LIRC_DCE_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	FILE *fp;
	string sCOM1 = "1";
	string sCOM2 = "2";
	string sLIRCDriver = DATA_Get_LIRC_Driver();
	string sSerialPort = DATA_Get_Serial_Port();
	
	g_pPlutoLogger->Write(LV_STATUS, "Making hardware config");
	if(sSerialPort == sCOM1) {
		sSerialPort = "/dev/ttyS0";
		g_pPlutoLogger->Write(LV_STATUS, "->Found Serial 1 in database");
	} else if(sSerialPort == sCOM2) {
		sSerialPort = "/dev/ttyS1";
		g_pPlutoLogger->Write(LV_STATUS, "->Found Serial 2 in database");
	} else {
		return;
	}
	
	if(sSerialPort == "" || sLIRCDriver == "") {
		g_pPlutoLogger->Write(LV_STATUS, "No SerialPort or Driver selected");
		return;
	}
	g_pPlutoLogger->Write(LV_STATUS, "->Found Driver %s in database",sLIRCDriver.c_str());
	system("rm -f /etc/lirc/hardware.conf");
	fp = fopen("/etc/lirc/hardware.conf","wt");
	fprintf(fp,"# /etc/lirc/hardware.conf\n");
	fprintf(fp,"#\n");
	fprintf(fp,"# Arguments which will be used when launching lircd\n");
	fprintf(fp,"LIRCD_ARGS=\"\"\n");
	fprintf(fp,"#Dont start lircmd even if there seams to be a good config file\n");
	fprintf(fp,"START_LIRCMD=false\n");
	fprintf(fp,"#Try to load appropriate kernel modules\n");
	fprintf(fp,"LOAD_MODULES=false\n");
	fprintf(fp,"# Run \"lircd --driver=help\" for a list of supported drivers.\n");
	fprintf(fp,"DRIVER=\"%s\"\n",sLIRCDriver.c_str());
	fprintf(fp,"DEVICE=\"%s\"\n",sSerialPort.c_str());
	fprintf(fp,"#MODULES=\"lirc_dev lirc_serial\"\n");
	fprintf(fp,"# Default configuration files for your hardware if any\n");
	fprintf(fp,"LIRCD_CONF=\"lircd.conf\"\n");
	fprintf(fp,"LIRCMD_CONF=\"lircmd.conf\"\n");
	fprintf(fp,"MODULES=\"UNCONFIGURED\"\n");
	fclose(fp);
	g_pPlutoLogger->Write(LV_STATUS, "Making Software config");
	string sConfiguration = DATA_Get_Configuration();
	if(sConfiguration == "") {
		g_pPlutoLogger->Write(LV_STATUS, "No valid configuration found into the database");
		return;
	}
	system("rm -f /etc/lirc/lircd.conf");
	fp = fopen("/etc/lirc/lircd.conf","wt");
	fprintf(fp,"%s",sConfiguration.c_str());
	fclose(fp);
	system("/etc/init.d/lirc restart");
// Check if it started
	lirc_leech(DeviceID);	
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
LIRC_DCE::LIRC_DCE(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: LIRC_DCE_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
LIRC_DCE::~LIRC_DCE()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool LIRC_DCE::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
LIRC_DCE_Command *Create_LIRC_DCE(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new LIRC_DCE(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void LIRC_DCE::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void LIRC_DCE::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void LIRC_DCE::SomeFunction()
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

int LIRC_DCE::lirc_leech(int DeviceID) {
	if(lirc_init("irexec",1)==-1) return 1;

	char *code;
	char *hex;
	int ret;
	string::size_type iPos = 0;
	string::size_type iPos2 = 0;
	string::size_type iPos3 = 0;
	string::size_type iPos4 = 0;	
	string sMapping;
	string sToken = "\n";
	string sToken2 = "|";
	string sToken3 = "x";
	string sToken4 = " ";
	string sLine, sHex, sMesg, sHexx, sMsg;
	Message* pMessage = NULL;
		

	hex = new char[16];
	while(lirc_nextcode(&code)==0)
	{
		strncpy(hex,code,16);
		sMapping = DATA_Get_Mapping();
		iPos = 0;
		do {
			sLine = StringUtils::Tokenize(sMapping, sToken, iPos);
			if(strcmp(sLine.c_str(),"") == 0)
				break;
			iPos2 = 0;
			sHexx = StringUtils::Tokenize(sLine, sToken2, iPos2);
			iPos3 = 0;
			sHex = StringUtils::Tokenize(sHexx, sToken3, iPos3);
			sHex = StringUtils::Tokenize(sHexx, sToken3, iPos3);
			sMesg = StringUtils::Tokenize(sLine, sToken2, iPos2);
			sHex = StringUtils::ToLower(sHex);
			sHexx = StringUtils::ToLower(hex);
			if(strcmp(sHex.c_str(),hex) == 0) {
//				g_pPlutoLogger->Write(LV_STATUS, "Sending Message : [%s]",sMesg.c_str());
				
				int msgFrom, msgTo, msgType, msgID;
				iPos4 = 0;
				
//				sMsg = StringUtils::Tokenize(sMesg, sToken4, iPos4);
//				msgFrom = atoi(sMsg.c_str());
				msgFrom = DeviceID;
				sMsg = StringUtils::Tokenize(sMesg, sToken4, iPos4);
				msgTo = atoi(sMsg.c_str());
				sMsg = StringUtils::Tokenize(sMesg, sToken4, iPos4);
				msgType = atoi(sMsg.c_str());
				sMsg = StringUtils::Tokenize(sMesg, sToken4, iPos4);
				msgID = atoi(sMsg.c_str());
			
				pMessage = new Message(msgFrom, msgTo, PRIORITY_NORMAL, 	msgType, msgID, 0);
				if(pMessage) {
					if(!GetEvents()->SendMessage(pMessage)) {
						g_pPlutoLogger->Write(LV_WARNING, "Error sending Event."); 
					}
				}	

			}
			
		} while(strcmp(sLine.c_str(),"") != 0);

		free(code);
		if(ret==-1) break;
	}
	delete hex;
	lirc_deinit();
	return 0;
}
