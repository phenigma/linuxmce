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
// The primary constructor when the class is created as a stand-alone device
Xine_Player::Xine_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Xine_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pXineSlaveControl = new XineSlaveWrapper();

	//m_pSlimServerClient = new SlimServerClient();
//	m_pSlimServerClient->setXineSlaveObject(m_pXineSlaveControl);

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

//<-dceag-const2-b->!

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
    return Connect(PK_DeviceTemplate_get());
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

	if ( sFilename.substr(0, strlen("slim://")).compare("slim://") == 0)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Need to behave like a slim server client");
		m_pXineSlaveControl->stopMedia(iStreamID);
		//m_pSlimServerClient->setMacAddress(GetMacAddress());
		// To not break more stuff
		// m_pSlimServerClient->connectToServer(sFilename.substr(strlen("slim://")), iStreamID);
	}
	else
	{
		//if ( m_pSlimServerClient->isConnected(iStreamID) )
		//	m_pSlimServerClient->disconnectFromServer(iStreamID);

		m_pXineSlaveControl->playStream(sFilename, iStreamID, iMediaPosition, pMessage->m_dwPK_Device_From);
	}
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Xine_Player::CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
    int currentTime, totalTime;

    g_pPlutoLogger->Write(LV_STATUS, "Got a stop media for stream ID %d", iStreamID);
    m_pXineSlaveControl->pauseMediaStream(iStreamID);
    *iMediaPosition = m_pXineSlaveControl->getStreamPlaybackPosition(iStreamID, currentTime, totalTime);
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
    m_pXineSlaveControl->changePlaybackSpeed(iStreamID, (XineSlaveWrapper::PlayBackSpeedType)iMediaPlaybackSpeed);
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
    *pData = NULL;
    *iData_Size = 0;

    m_pXineSlaveControl->getScreenShot(iStreamID, iWidth, iHeight, *pData, *iData_Size, *sFormat, sCMD_Result);
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
}
//<-dceag-c243-b->

	/** @brief COMMAND: #243 - Enable Broadcasting */
	/** Enable broadcasting from here. */
		/** @param #41 StreamID */
			/** The stream to enable broadcast for */
		/** @param #59 MediaURL */
			/** The media url from which this stream can be played. */

void Xine_Player::CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c243-e->
{
    int iBroadcastPort;

    if ( (iBroadcastPort = m_pXineSlaveControl->enableBroadcast(iStreamID)) == 0 )
    {
        sCMD_Result = "CAN'T ENABLE";
        return; // no port was available (couldn't enable broadcast)
    }

    *sMediaURL = string("slave://localhost") + StringUtils::itos(iBroadcastPort);
}
//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #39 Options */
			/** Other options that the player might record for this position. Usefull if we have a non standard encoding of the player position. */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** The reported media position ( in milliseconds since the beginning of the stream). */
		/** @param #106 Media Length */
			/** The complete length of the media stream. Where appliable. */

void Xine_Player::CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
    m_pXineSlaveControl->getStreamPlaybackPosition(iStreamID, *iMediaPosition, *iMedia_Length);
}

string Xine_Player::GetMacAddress()
{
	DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);

	if ( pDeviceData_Base == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "I could not find a device data base object for myself %d: this is very very strange.", m_dwPK_Device);
		return "";
	}

	return pDeviceData_Base->GetMacAddress();
}
//<-dceag-createinst-b->!
