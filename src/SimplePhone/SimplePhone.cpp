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

#include "DCE/DCEConfig.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_DeviceTemplate.h"

#include "Gen_Devices/AllScreens.h"

static int phone_status=0;
static int call_status=0;

#include "RingData.h"

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
    unregisterWithAsterisk();
}

//<-dceag-getconfig-b->
bool SimplePhone::GetConfig()
{
    if( !SimplePhone_Command::GetConfig() )
        return false;
//<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated
    ringTone = NULL;
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
    char tmp[1024];
    snprintf(tmp,sizeof(tmp)-1,"%s:%s@%s/%s",deviceExtension,devicePassword,asteriskHost,sPhoneExtension.c_str());
    g_pPlutoLogger->Write(LV_STATUS, "Try to call %s", tmp);
    DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media_(GetData()->m_dwPK_Device_ControlledVia,DEVICETEMPLATE_VirtDev_Media_Plugin_CONST,0,0,0,"");
    SendCommand(CMD_MH_Stop_Media_);
    iaxc_millisleep(1000);    
    StopRingTone();    
    iaxc_call(tmp);
    sCMD_Result="OK";
    DCE::SCREEN_DevCallInProgress SCREEN_DevCallInProgress_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia);
    SendCommand(SCREEN_DevCallInProgress_);
    haveActiveCall=true;    
}

//<-dceag-c335-b->

    /** @brief COMMAND: #335 - Phone_Answer */
    /** Answer a call */

void SimplePhone::CMD_Phone_Answer(string &sCMD_Result,Message *pMessage)
//<-dceag-c335-e->
{
    if(phone_status>0)
    {
        iaxc_millisleep(1000);
        StopRingTone();
        DCE::SCREEN_DevCallInProgress SCREEN_DevCallInProgress_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia);
        SendCommand(SCREEN_DevCallInProgress_);
        iaxc_millisleep(1000);
        iaxc_answer_call(0);
        iaxc_select_call(0);
        sCMD_Result="OK";
        haveActiveCall=true;
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
    iaxc_millisleep(1000);
    StopRingTone();
    DCE::SCREEN_Main SCREEN_Main_(m_dwPK_Device,GetData()->m_dwPK_Device_ControlledVia,"");
    SendCommand(SCREEN_Main_);

    if(haveActiveCall)
    {
        iaxc_dump_call();
        sCMD_Result="OK";
        haveActiveCall=false;
    }
    else
    {
        sCMD_Result="ERROR";
    }
}

/* IAXCLIENT PART */
int iaxCallback(iaxc_event e)
{
    if(e.type == IAXC_EVENT_STATE)
    {
        phone_status=0;
        call_status=e.ev.call.state;
        if(e.ev.call.state == IAXC_CALL_STATE_FREE)
        {
            phone_status=-1;
        }
        if(e.ev.call.state & IAXC_CALL_STATE_RINGING)
        {
            phone_status=1;

            if(!strcmp(e.ev.call.remote_name,"plutosecurity"))
            {
                /* the call is actually a speak in the house */
                phone_status=2;
                g_pPlutoLogger->Write(LV_STATUS, "PLUTO AUTOANSWER\n");
            }
            return 0;
        }
    }
    return 0;
}

void SimplePhone::StopRingTone()
{
    g_pPlutoLogger->Write(LV_STATUS, "Stop RING");
    if(ringTone)
    {
        iaxc_stop_sound(ringTone->id);
        free(ringTone);
        ringTone=NULL;
    }
}
void SimplePhone::PlayRingTone()
{
    if(ringTone)
    {
        StopRingTone();
    }
    ringTone = (iaxc_sound *)calloc(1,sizeof(iaxc_sound));
    ringTone->channel = 1;
    ringTone->len  = sizeof(ringData)/sizeof(short);
    ringTone->data = ringData;
    ringTone->repeat = -1;
    iaxc_play_sound(ringTone, 1);
    g_pPlutoLogger->Write(LV_STATUS, "Play RING");    
}

void SimplePhone::doProccess(void)
{
    string tmp;
    int event_status = 0;
    while(!m_bQuit)
    {
        iaxc_process_calls();
        iaxc_millisleep(5);
        if((phone_status>0) && (haveActiveCall==false))
        {
            if(event_status != phone_status)
            {
                DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media_(GetData()->m_dwPK_Device_ControlledVia,DEVICETEMPLATE_VirtDev_Media_Plugin_CONST,0,0,0,"");
                SendCommand(CMD_MH_Stop_Media_);
                if(phone_status>1)
                {
                    CMD_Phone_Answer(tmp,NULL);
                }
                else
                {
                    GetEvents()->SendMessage(new Message(m_dwPK_Device, DEVICETEMPLATE_VirtDev_Telecom_Plugin_CONST, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Incoming_Call_CONST,0));                
                    iaxc_millisleep(1000);                
                    PlayRingTone();
                }
            }
        }
        if(phone_status<0)
        {
            if(event_status != phone_status)
            {
                CMD_Phone_Drop(tmp,NULL);
            }
        }
        event_status = phone_status;
    }
}

void SimplePhone::registerWithAsterisk()
{
    DCEConfig dceconf;

    struct iaxc_audio_device *devices;
    int                      nDevs;
    int                      i;
    int                      input  = 0;
    int                      output = 0;
    int                      ring   = 0;
    haveActiveCall=false;
    if(iaxc_initialize(AUDIO_INTERNAL_ALSA,1)<0)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not start (maybe a sound issue?)");
        m_bQuit = 1;
        exit(1);
    }

    if(iaxc_audio_devices_get(&devices, &nDevs, &input, &output, &ring)<0)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not get access to audio device, will quit now");
        m_bQuit = 1;
        exit(1);
    }

    for(i=0; i<nDevs; i++) {
        if(devices->capabilities & IAXC_AD_INPUT) {
            input = devices->devID;
        }

        if(devices->capabilities & IAXC_AD_OUTPUT) {
            output = devices->devID;
        }

        if(devices->capabilities & IAXC_AD_RING) {
            ring = devices->devID;
        }
        devices++;
    }
    iaxc_audio_devices_set(input,output,ring);

    iaxc_set_formats(IAXC_FORMAT_GSM,IAXC_FORMAT_GSM);
    iaxc_set_silence_threshold(-99);
    iaxc_set_event_callback(iaxCallback);

    deviceExtension = strdup(DATA_Get_PhoneNumber().c_str());
    devicePassword = deviceExtension;
    asteriskHost = strdup(dceconf.m_sDBHost.c_str());

    g_pPlutoLogger->Write(LV_STATUS, "Will try to register as %s:%s@%s",deviceExtension,devicePassword,asteriskHost);
    iaxc_register(deviceExtension,devicePassword,asteriskHost);
}

void SimplePhone::unregisterWithAsterisk()
{
    iaxc_dump_all_calls();
    iaxc_millisleep(1000);
//    free(deviceExtension);
//    free(asteriskHost);
}

void * startIaxThread(void * Arg)
{
    SimplePhone *phone = (SimplePhone *) Arg;
    g_pPlutoLogger->Write(LV_STATUS, "Started IAX2 Thread");
    phone->registerWithAsterisk();
    phone->doProccess();
    phone->unregisterWithAsterisk();
    return NULL;
}

void SimplePhone::CreateChildren()
{
    if (pthread_create(&iaxThread, NULL, startIaxThread, (void *) this))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Failed to create IAX2 Thread");
        m_bQuit = 1;
        exit(1);
    }
    pthread_detach(iaxThread);
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
    char ch;

    g_pPlutoLogger->Write(LV_STATUS, "Received '%s'",sPK_Button.c_str());
    sCMD_Result="ERROR";
    if(call_status && IAXC_CALL_STATE_COMPLETE)
    {
        sscanf(sPK_Button.c_str(),"%d",&ch);
        if((ch>='0' && ch<='9') || (ch=='*') || (ch == '#'))
        {
            g_pPlutoLogger->Write(LV_STATUS, "Will send '%d'(%c) as DTMF",ch,ch);
            iaxc_send_dtmf(ch);
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
