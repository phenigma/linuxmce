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

#include "pluto_main/Define_Button.h"

#include "SlimServerClient.h"

bool g_bHackToBeSureWeStop=false;
void* HackToBeSureWeStop(void* param) 
{
	Sleep(1000);
	if( g_bHackToBeSureWeStop )
		kill(getpid(), SIGSEGV);
	return NULL;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Xine_Player::Xine_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Xine_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_xineSlaveMutex("xine-slave-access-mutex")
{
	pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	m_xineSlaveMutex.Init( &mutexAttr );

	m_pXineSlaveControl = new XineSlaveWrapper();



	//m_pSlimServerClient = new SlimServerClient();
//	m_pSlimServerClient->setXineSlaveObject(m_pXineSlaveControl);

	m_pXineSlaveControl->setXinePlayerObject(this);

    if ( ! m_pXineSlaveControl->createWindow() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't create the xine slave window. This plugin is useless here!");
        delete m_pXineSlaveControl;
        m_pXineSlaveControl = NULL;
		return;
    }

    else if ( ! m_pXineSlaveControl->createXineLibConnection() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't create the Xine Object.");

       delete m_pXineSlaveControl;
       m_pXineSlaveControl = NULL;
	   return;
    }
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

bool Xine_Player::Connect(int iPK_DeviceTemplate )
{
	if ( ! Command_Impl::Connect(iPK_DeviceTemplate) )
		return false;

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::Connect() xine-lib initialization has failed.");
        return false;
    }

	m_pXineSlaveControl->setOutputSpeakerArrangement(DATA_Get_Output_Speaker_arrangement());
	return true;
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
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Play_Media() called for filename: %s.", sFilename.c_str());
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

    if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Play_Media() I don't have a slave to make it play. The slave probably failed to initialize properly.");
        return;
    }

    makeActive(m_pXineSlaveControl->getRenderingWindowName());

	if ( sFilename.substr(0, strlen("slim://")).compare("slim://") == 0)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Need to behave like a slim server client");

		string macAddress = StringUtils::makeUpPlayerAddressFromPlayerId(m_dwPK_Device);
		m_pXineSlaveControl->setSlimClient(true);
		getSlimServerClient()->setMacAddress(macAddress);
		getSlimServerClient()->setMediaStreamID(iStreamID);
		getSlimServerClient()->setRequestingObjectID(pMessage->m_dwPK_Device_From);
		getSlimServerClient()->connectToServer(sFilename.substr(strlen("slim://")), iStreamID);
	}
	else
	{
		m_pXineSlaveControl->setSlimClient(false);
// 		getSlimServerClient()->disconnectFromServer();
		if ( ! m_pXineSlaveControl->createStream(sFilename, iStreamID, pMessage->m_dwPK_Device_From) || ! m_pXineSlaveControl->playStream(iStreamID, iMediaPosition) )
		{
			EVENT_Playback_Completed(iStreamID,true);  // true = there was an error, don't keep repeating
			delete m_pXineSlaveControl;
			m_pXineSlaveControl = NULL;
		}
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
    PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Stop_Media() I don't have a slave to make it play. The slave proabbly failed to initialize properly.");
        return;
    }

    int currentTime, totalTime;

    g_pPlutoLogger->Write(LV_STATUS, "Xine_Player::CMD_Stop_Media() Got a stop media for stream ID %d", iStreamID);
    m_pXineSlaveControl->pauseMediaStream(iStreamID);
    g_pPlutoLogger->Write(LV_STATUS, "Xine_Player::CMD_Stop_Media() After pause media %d", iStreamID);
    *iMediaPosition = m_pXineSlaveControl->getStreamPlaybackPosition(iStreamID, currentTime, totalTime);
    g_pPlutoLogger->Write(LV_STATUS, "Xine_Player::CMD_Stop_Media() position %d", *iMediaPosition);

pthread_t pt;
pthread_create(&pt, NULL, HackToBeSureWeStop, NULL);
g_bHackToBeSureWeStop=true;
    m_pXineSlaveControl->stopMedia(iStreamID);
g_bHackToBeSureWeStop=false;
	g_pPlutoLogger->Write(LV_STATUS, "Xine_Player::CMD_Stop_Media() The stream playback should be stopped at this moment and the resources should be freed!");

	if ( getSlimServerClient()->isConnected(iStreamID) )
		getSlimServerClient()->disconnectFromServer(iStreamID);
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Xine_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Pause_Media() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

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
    PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Restart_Media() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->restartMediaStream(iStreamID);
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void Xine_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

    if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Change_Playback_Speed() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->changePlaybackSpeed(iStreamID, (XineSlaveWrapper::PlayBackSpeedType)iMediaPlaybackSpeed);
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

void Xine_Player::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Skip_Fwd_ChannelTrack_Greater() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_NEXT);
}


//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

void Xine_Player::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Skip_Back_ChannelTrack_Lower() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_PREVIOUS);
}


//<-dceag-sample-b->!

Display *Xine_Player::getDisplay()
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::getDisplay(). I don't have a slave to make it play. The slave proabbly failed to initialize properly.");
        return NULL;
    }

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
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

    if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Navigate_Next() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

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
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Navigate_Prev() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->selectPrevButton(iStreamID);
}


//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's the current video frame from the media player. */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

void Xine_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
       g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Get_Video_Frame() I don't have a slave to use. Ignoring.");
       return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

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
    PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
       g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Goto_Media_Menu() I don't have a slave to use. Ignoring.");
       return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	if  ( iMenuType != 0 && iMenuType != 1 && iMenuType != 2 )
		iMenuType = 0;

	g_pPlutoLogger->Write(LV_STATUS, "Selecting the menu with id: %d", iMenuType);
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
    PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Enable_Broadcasting() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

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
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Report_Playback_Position() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	m_pXineSlaveControl->getStreamPlaybackPosition(iStreamID, *iMediaPosition, *iMedia_Length);
}

//<-dceag-createinst-b->!
//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Xine_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Simulate_Mouse_Click() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->simulateMouseClick(iPosition_X, iPosition_Y);
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */

void Xine_Player::CMD_EnterGo(string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_EnterGo() I don't have a slave to use. Ignoring.");
        return;
    }

	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_SELECT);
	// m_pXineSlaveControl->simulateKeystroke(BUTTON_Enter_CONST);
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */

void Xine_Player::CMD_Move_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Move_Up() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_UP);
	// m_pXineSlaveControl->simulateKeystroke(BUTTON_Up_Arrow_CONST);
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */

void Xine_Player::CMD_Move_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Move_Down() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_DOWN);
	// m_pXineSlaveControl->simulateKeystroke(BUTTON_Down_Arrow_CONST);
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */

void Xine_Player::CMD_Move_Left(string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Move_Left() I don't have a slave to use. Ignoring.");
        return;
    }


	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	// m_pXineSlaveControl->simulateKeystroke(BUTTON_Left_Arrow_CONST);
	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_LEFT);
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */

void Xine_Player::CMD_Move_Right(string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);

	if ( ! m_pXineSlaveControl )
    {
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Move_Right() I don't have a slave to use. Ignoring.");
        return;
    }

	if ( m_pXineSlaveControl->isSlimClient() )
		return;

	makeActive(m_pXineSlaveControl->getRenderingWindowName());
	m_pXineSlaveControl->sendInputEvent(XINE_EVENT_INPUT_RIGHT);
//	m_pXineSlaveControl->simulateKeystroke(BUTTON_Right_Arrow_CONST);
}
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void Xine_Player::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Vol_Up() Not implemented!");
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void Xine_Player::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Vol_Down() Not implemented!");
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

void Xine_Player::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Mute() Not implemented!");
}
//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void Xine_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_0() Not implemented!");
}
//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void Xine_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_2() Not implemented!");
}
//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void Xine_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_2() Not implemented!");
}
//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void Xine_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_3() Not implemented!");
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void Xine_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_4() Not implemented!");
}
//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void Xine_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_5() Not implemented!");
}
//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void Xine_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_6() Not implemented!");
}
//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void Xine_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_7() Not implemented!");
}
//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void Xine_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_8() Not implemented!");
}
//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void Xine_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_9() Not implemented!");
}
//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */

void Xine_Player::CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::CMD_Back_Prior_Menu() Not implemented!");
}

SlimServerClient *Xine_Player::getSlimServerClient()
{
	if ( m_pSlimServerClient == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Creating the Slim Server Client object");
		m_pSlimServerClient = new SlimServerClient();
		m_pSlimServerClient->setXineSlaveObject(m_pXineSlaveControl);
		m_pSlimServerClient->setXineSlaveMutex(&m_xineSlaveMutex);
	}

	return m_pSlimServerClient;
}
