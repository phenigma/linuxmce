/*
    Xine_Player

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/


/**
 *
 * @file Xine_Player.cpp
 * @brief source file for the Xine_Player class
 * @author
 *
 */

 //<-dceag-d-b->
#include "Xine_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
Xine_Player::Xine_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Xine_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pXineSlaveControl = new XineSlaveWrapper();

    if ( ! m_pXineSlaveControl->createWindow() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't create the xine slave window. This plugin is useless here!");
        delete m_pXineSlaveControl;
        m_pXineSlaveControl = NULL;
    }

    else if ( ! m_pXineSlaveControl->createXineLibConnection() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't create the Xine Object.");

       delete m_pXineSlaveControl;
       m_pXineSlaveControl = NULL;
    }

    m_pXineSlaveControl->setXinePlayerObject(this);
}

//<-dceag-dest-b->
Xine_Player::~Xine_Player()
//<-dceag-dest-e->
{
    delete m_pXineSlaveControl;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Xine_Player::Register()
//<-dceag-reg-e->
{
    return Connect();
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Xine_Player::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Xine_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

/** @example   **** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void Xine_Player::SomeFunction()
{
    // If this is going to be loaded into the router as a plug-in, you can implement:   virtual bool Register();
    // to do all your registration, such as creating message interceptors

    // If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
    // commands and requests, including the parameters.  See "AllCommandsRequests.h"

    // Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

    // Send the message to a specific orbiter, identified by OrbiterID
    QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

    // Send the message to orbiters 32898 and 27283
    QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

    // Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
    QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

    // Send the message to all "MASTERDEVICELIST_Orbiter_CONST" devices within the room (see pluto_main/Define_MasterDeviceList.h)
    QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,MASTERDEVICELIST_Orbiter_CONST,true,BL_SameRoom,55,77);

    // This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
    string sResult="";
    bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
    // If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

    // A request is like a command, except that it has both "in" and "out" parameters, and the
    // thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
    // events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
    // the socket is busy.

    // We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
    // All out parameters are passed by reference.
    // Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
    // This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
    // request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
    // and there were multiple devices "Standard Plug-in" they would all get the message.
    PlutoDate plutoDate;
    char *FileContents;
    int FileSize;
    bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,MASTERDEVICELIST_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);
    // To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

    // Get our IP address from our data
    string sIP = DATA_Get_IP_Address();

    // Set our data "Filename" to "myfile"
    DATA_Set_Filename("myfile");

    // Fire the "Finished with file" event, which takes no parameters
    EVENT_Finished_with_file();


}
*/

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c37-b->

    /** @brief COMMAND: #37 - Play Media */
    /** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
        /** @param #13 Filename */
            /** The file to play.  The format is specific on the media type and the media player. */
        /** @param #29 PK_MediaType */
            /** The type of media */
        /** @param #41 StreamID */
            /** The media that we need to play. */
        /** @param #42 MediaPosition */
            /** The position at which we need to start playing. */

void Xine_Player::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
    if ( ! m_pXineSlaveControl )
    {
    g_pPlutoLogger->Write(LV_WARNING, "I don't have a slave to make it play. The slave proabbly failed to initialize properly.");
    return;
    }

    makeActive(m_pXineSlaveControl->getRenderingWindowName());
    m_pXineSlaveControl->playStream(sFilename, iStreamID, iMediaPosition, pMessage->m_dwPK_Device_From);
}

//<-dceag-c38-b->

    /** @brief COMMAND: #38 - Stop Media */
    /** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
        /** @param #41 StreamID */
            /** The media needing to be stopped. */

void Xine_Player::CMD_Stop_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Got a stop media for stream ID %d", iStreamID);
    m_pXineSlaveControl->stopMedia(iStreamID);
    g_pPlutoLogger->Write(LV_STATUS, "The stream playback should be stopped at this moment and the resources should be freed!");
}

//<-dceag-c39-b->

    /** @brief COMMAND: #39 - Pause Media */
    /** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
        /** @param #41 StreamID */
            /** The media stream for which we need to pause playback. */

void Xine_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
    m_pXineSlaveControl->pauseMediaStream(iStreamID);
}

//<-dceag-c40-b->

    /** @brief COMMAND: #40 - Restart Media */
    /** This will restart a media was paused with the above command */
        /** @param #41 StreamID */
            /** The media stream that we need to restart playback for. */

void Xine_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
    m_pXineSlaveControl->restartMediaStream(iStreamID);
}

//<-dceag-c41-b->

    /** @brief COMMAND: #41 - Change Playback Speed */
    /** Will make the playback to FF with a configurable amount of speed. */
        /** @param #41 StreamID */
            /** The media needing the playback speed change. */
        /** @param #43 MediaPlaybackSpeed */
            /** The requested media playback speed. This is a multiplier of the normal speed. (If we want 2x playback this parameter will be 2 if we want half of normal speed then the parameter will be 0.5). The formula is NextSpeed = MediaPlaybackSpeed * NormalPlaybackS */

void Xine_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
    m_pXineSlaveControl->changePlaybackSpeed(iStreamID, iMediaPlaybackSpeed);
}

//<-dceag-c63-b->

    /** @brief COMMAND: #63 - Skip Forward */
    /** Chapter/Track Next/Down/Forward */

void Xine_Player::CMD_Skip_Forward(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
}


//<-dceag-c64-b->

    /** @brief COMMAND: #64 - Skip Back */
    /** Chapter/Track Back/Up/Prior */

void Xine_Player::CMD_Skip_Back(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
}

//<-dceag-sample-b->!

Display *Xine_Player::getDisplay()
{
    return m_pXineSlaveControl->XServerDisplay;
}
//<-dceag-c81-b->

    /** @brief COMMAND: #81 - Navigate Next */
    /** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

void Xine_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
    m_pXineSlaveControl->selectNextButton(iStreamID);
}

//<-dceag-c82-b->

    /** @brief COMMAND: #82 - Navigate Prev */
    /** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

void Xine_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
    m_pXineSlaveControl->selectPrevButton(iStreamID);

}

//<-dceag-c83-b->

    /** @brief COMMAND: #83 - Select Current Navigable Area */
    /** Mark the selected area as "clicked". */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

void Xine_Player::CMD_Select_Current_Navigable_Area(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c83-e->
{
    m_pXineSlaveControl->pushCurrentButton(iStreamID);
}


//<-dceag-c84-b->

    /** @brief COMMAND: #84 - Get Video Frame */
    /** Get's the current video frame from the media player. */
        /** @param #19 Data */
            /** The video frame */
        /** @param #20 Format */
            /** One of the following: "jpg", "png" */
        /** @param #23 Disable Aspect Lock */
            /** If true, don't worry about the aspect ratio.  Try to get the requested width and height. */
        /** @param #41 StreamID */
            /** Optional.  For multi stream devices, like media players, this identifies the stream. */
        /** @param #60 Width */
            /** The desired width of the video frame.  The sender need not respect this. */
        /** @param #61 Height */
            /** The desired height of the video frame.  The sender need not respect this. */

void Xine_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Getting the frame!");
    *pData = NULL;
    *iData_Size = 0;

    m_pXineSlaveControl->getScreenShot(iStreamID, iWidth, iHeight, *pData, *iData_Size, *sFormat, sCMD_Result);
    g_pPlutoLogger->Write(LV_STATUS, "Done!");
}

//<-dceag-c87-b->

    /** @brief COMMAND: #87 - Goto Media Menu */
    /** Goto to the current media Root Menu. */
        /** @param #41 StreamID */
            /** The stream ID */
        /** @param #64 MenuType */
            /** The type of menu that the user want to jump to.
(For DVD handlers usually this applies)
0 - Root menu
1 - Title menu
2 - Media menu */

void Xine_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
    m_pXineSlaveControl->selectMenu(iStreamID, iMenuType);
}

void Xine_Player::FireMenuOnScreen(int iDestinationDevice, int iStreamID, bool bOnOff)
{
    g_pPlutoLogger->Write(LV_STATUS, "Sending Menu on screen event %s for stream %d", bOnOff ? "on" : "off", iStreamID);

    EVENT_Menu_Onscreen(iStreamID, bOnOff);
/*    SendMessage(
        new Message(
            m_dwPK_Device,
            iDestinationDevice,
            PRIORITY_NORMAL,
            MESSAGETYPE_EVENT,
            7,                                        // the event ID
            2,
            9, StringUtils::itos(iStream_ID).c_str(), // the stream ID
            10,(bOnOff ? "1" : "0")));                // the On/Off state of the Menu*/
}
