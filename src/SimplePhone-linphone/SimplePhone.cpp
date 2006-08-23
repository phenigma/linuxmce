//<-dceag-d-b->
#include "SimplePhone.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Variable.h"

#include "Gen_Devices/AllScreens.h"

#include "SIP_Thread.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SimplePhone::SimplePhone(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : SimplePhone_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SimplePhone::SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
    : SimplePhone_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
SimplePhone::~SimplePhone()
//<-dceag-dest-e->
{
	LS_bQuit = true;
}

//<-dceag-getconfig-b->
bool SimplePhone::GetConfig()
{
    if( !SimplePhone_Command::GetConfig() )
        return false;
//<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated
    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SimplePhone::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
    cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
    can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SimplePhone_Command *Create_SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
    return new SimplePhone(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all    commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void SimplePhone::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void SimplePhone::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*        **** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void SimplePhone::SomeFunction()
{
    // If this is going to be loaded into the router as a plug-in, you can implement:     virtual bool Register();
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

//<-dceag-c334-b->

    /** @brief COMMAND: #334 - Phone_Initiate */
    /** Initiates a call */
        /** @param #83 PhoneExtension */
            /** Extention to dial */

void SimplePhone::CMD_Phone_Initiate(string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c334-e->
{
    if(LS_ActiveCall())
    {
        g_pPlutoLogger->Write(LV_STATUS, "Already have a call");
        sCMD_Result="ERROR";
        return;
    }
    g_pPlutoLogger->Write(LV_STATUS, "Try to call %s", sPhoneExtension.c_str());
	StopMedia();
	LS_InitiateCall(sPhoneExtension.c_str());
    sCMD_Result="OK";
    CMD_Set_Variable CMD_Set_Variable_name(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,VARIABLE_Caller_name_CONST,string(""));
    SendCommand(CMD_Set_Variable_name);
    CMD_Set_Variable CMD_Set_Variable_number(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,VARIABLE_Caller_number_CONST,sPhoneExtension);
    SendCommand(CMD_Set_Variable_number);
    DCE::SCREEN_DevCallInProgress SCREEN_DevCallInProgress_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia);
    SendCommand(SCREEN_DevCallInProgress_);
}

//<-dceag-c335-b->

    /** @brief COMMAND: #335 - Phone_Answer */
    /** Answer a call */

void SimplePhone::CMD_Phone_Answer(string &sCMD_Result,Message *pMessage)
//<-dceag-c335-e->
{
	CallInProgressScreen();
	Sleep(1000);

    if(!LS_ActiveCall())
    {
        Sleep(1000);
		LS_AcceptCall();
        sCMD_Result="OK";
    }
    else
    {
        sCMD_Result="ERROR";
    }
}

//<-dceag-c336-b->

    /** @brief COMMAND: #336 - Phone_Drop */
    /** Drop a call */

void SimplePhone::CMD_Phone_Drop(string &sCMD_Result,Message *pMessage)
//<-dceag-c336-e->
{
    if(LS_ActiveCall())
    {
        Sleep(1000);
        LS_DropCall();
        sCMD_Result="OK";
    }
    else
    {
        sCMD_Result="ERROR";
    }
}

void SimplePhone::StopMedia()
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone::StopMedia()");
	DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media_(GetData()->m_dwPK_Device_ControlledVia,DEVICETEMPLATE_VirtDev_Media_Plugin_CONST,0,0,0,"");
	SendCommand(CMD_MH_Stop_Media_);
	Sleep(1000);
}

void SimplePhone::IncomingCallScreen(string sCallerID)
{
	StopMedia();
	CMD_Set_Variable CMD_Set_Variable_name(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,VARIABLE_Caller_name_CONST,sCallerID);
	SendCommand(CMD_Set_Variable_name);
	CMD_Set_Variable CMD_Set_Variable_number(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,VARIABLE_Caller_number_CONST,sCallerID);
	SendCommand(CMD_Set_Variable_number);
	GetEvents()->SendMessage(new Message(m_dwPK_Device, DEVICETEMPLATE_VirtDev_Telecom_Plugin_CONST, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Incoming_Call_CONST,0));
}

void SimplePhone::CallDroppedScreen()
{
    DCE::SCREEN_Main SCREEN_Main_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,"");
    SendCommand(SCREEN_Main_);
}

void SimplePhone::CallInProgressScreen()
{
	DCE::SCREEN_DevCallInProgress SCREEN_DevCallInProgress_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia);
	SendCommand(SCREEN_DevCallInProgress_);
}

void SimplePhone::CreateChildren()
{
    if (pthread_create(&m_SIP_Thread, NULL, LS_Thread, (void *) this))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Failed to create Linphone SIP Thread");
        m_bQuit = 1;
        exit(1);
    }
    pthread_detach(m_SIP_Thread);
}
//<-dceag-c28-b->

    /** @brief COMMAND: #28 - Simulate Keypress */
    /** Send a DTMF code */
        /** @param #26 PK_Button */
            /** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
        /** @param #50 Name */
            /** The application to send the keypress to. If not specified, it goes to the DCE device. */

void SimplePhone::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	(void) sName; // prevent warning of unused variable

    char ch;

    g_pPlutoLogger->Write(LV_STATUS, "Received '%s'",sPK_Button.c_str());
    sCMD_Result="ERROR";
    if(LS_ActiveCall())
    {
		ch = atoi(sPK_Button.c_str());
        if((ch>='0' && ch<='9') || (ch=='*') || (ch == '#'))
        {
            g_pPlutoLogger->Write(LV_STATUS, "Will send '%d'(%c) as DTMF",ch,ch);
            LS_SentDTMF(ch);
            sCMD_Result="OK";
        }
        else
        {
            g_pPlutoLogger->Write(LV_STATUS, "'%d'(%c) is invalid DTMF character",ch,ch);
        }
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "Looks like there is no call");
    }
}
