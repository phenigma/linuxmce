/*
    MythTV_Player

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
 * @file MythTV_Player.cpp
 * @brief source file for the MythTV_Player class
 *
 */

 //<-dceag-d-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"

#include "PlutoUtils/ProcessUtils.h"

#include <sstream>
// #include <qsqldatabase.h>
//
// #include <libmythtv/programinfo.h>
// #include <libmythtv/remoteencoder.h>
// #include <libmythtv/remoteutil.h>

#include <pthread.h>
// #include "MythMainWindowResizable.h"

#include "utilities/linux/RatpoisonHandler.h"

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>

// we only have signals on Linux and the global var is only used there. so we ifndef it..
#ifndef WIN32
#include <sys/wait.h>

MythTV_Player *g_pMythPlayer = NULL;

void sh(int i) /* signal handler */
{
    if ( g_pMythPlayer && g_pMythPlayer->m_bQuit )
		return;

    int status = 0;
    pid_t pid = 0;

    pid = wait(&status);

    if ( g_pMythPlayer )
        g_pMythPlayer->ProcessExited(pid, status);
}
#endif

#define MYTH_WINDOW_NAME "mythfrontend"

// This shoould be the class name of the gimageview application
#define LOGO_APPLICATION_NAME "gimageview"

class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
    bool commandRatPoison(string command) { return RatpoisonHandler<RatPoisonWrapper>::commandRatPoison(command); }
};

// MythContext *gContext;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_MythMutex("myth")
{
	m_MythMutex.Init(NULL);
    m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));

    m_iMythFrontendWindowId = 0;

#ifndef WIN32
	g_pMythPlayer = this;
    signal(SIGCHLD, sh); /* install handler */
#endif

}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_Player::~MythTV_Player()
//<-dceag-dest-e->
{
	// Kill any instances we spawned
	vector<void *> data;
	ProcessUtils::KillApplication(MYTH_WINDOW_NAME, data);

    delete m_pRatWrapper;

}

bool MythTV_Player::LaunchMythFrontend()
{
	if ( ! m_pRatWrapper )
        m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));

	m_pRatWrapper->commandRatPoison(":select " LOGO_APPLICATION_NAME);
	ProcessUtils::SpawnApplication("/usr/bin/mythfrontend", "", MYTH_WINDOW_NAME);

    selectWindow();
    locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));

    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_Player::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

bool MythTV_Player::checkXServerConnection()
{
	if ( ! m_pRatWrapper || ! m_pRatWrapper->getDisplay() )
	{
		if ( !m_pRatWrapper )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "The ratpoison command handler value is null. This usually means the XServer connection is down or useless");
			return false;
		}

		if ( !m_pRatWrapper->getDisplay() )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "The Display* value in the ratpoison command handler is null. This ususally means the XServer connection is down or useless");
			return false;
		}
	}

	return true;
}
void MythTV_Player::CreateChildren()
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

    if ( ! locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay())) )
    {
		LaunchMythFrontend();
        locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
    }
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
void MythTV_Player::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->Window
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void MythTV_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

void MythTV_Player::selectWindow()
{
    m_pRatWrapper->commandRatPoison(":select " MYTH_WINDOW_NAME);
}

void MythTV_Player::processKeyBoardInputRequest(int iXKeySym)
{
    Window oldWindow;
    int oldRevertBehaviour;

	if ( ! checkXServerConnection())
		return;

	if ( ! locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay())) )
    {
            LaunchMythFrontend();
            locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
    }

	selectWindow();

	XGetInputFocus( m_pRatWrapper->getDisplay(), &oldWindow, &oldRevertBehaviour);
    XSetInputFocus( m_pRatWrapper->getDisplay(), (Window)m_iMythFrontendWindowId, RevertToParent, CurrentTime );
    XTestFakeKeyEvent( m_pRatWrapper->getDisplay(), XKeysymToKeycode(m_pRatWrapper->getDisplay(), iXKeySym), True, 0 );
    XTestFakeKeyEvent( m_pRatWrapper->getDisplay(), XKeysymToKeycode(m_pRatWrapper->getDisplay(), iXKeySym), False, 0 );
    if ( oldWindow )
        XSetInputFocus( m_pRatWrapper->getDisplay(), oldWindow, oldRevertBehaviour, CurrentTime );

    XFlush(m_pRatWrapper->getDisplay());

}

bool MythTV_Player::checkWindowName(long unsigned int window, string windowName)
{
    XTextProperty text;

	if ( ! checkXServerConnection())
		return false;

	if ( XGetWMName (m_pRatWrapper->getDisplay(), window, &text) && windowName == string((const char*)text.value) )
        return true;

    return false;
}

bool MythTV_Player::locateMythTvFrontendWindow(long unsigned int window)
{
    Window parent_win, root_win, *child_windows;
    unsigned int num_child_windows;

	if ( ! checkXServerConnection())
		return false;

    if ( checkWindowName(window, MYTH_WINDOW_NAME ) )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Found window id: 0x%x", window );
        m_iMythFrontendWindowId = window;
        return true;
    }

    XQueryTree(m_pRatWrapper->getDisplay(), (Window)window, &root_win, &parent_win, &child_windows, &num_child_windows);

    for ( unsigned int i = 0; i < num_child_windows; i++ )
        if ( locateMythTvFrontendWindow(child_windows[i]) )
            return true;

    /* we need to free the list of child IDs, as it was dynamically allocated */
    /* by the XQueryTree function.                                            */
    XFree(child_windows);
    return false;
}

/*

 COMMANDS TO IMPLEMENT

*/

/*
void MythTV_Player::CMD_Start_TV(string &sCMD_Result,Message *pMessage)
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

	if ( ! locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay())) )
    {
            LaunchMythFrontend();
            locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
    }

    selectWindow();
}


void MythTV_Player::CMD_Stop_TV(string &sCMD_Result,Message *pMessage)
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

	vector<void *> data;
	if ( ProcessUtils::KillApplication(MYTH_WINDOW_NAME, data) == false )
		g_pPlutoLogger->Write(LV_WARNING, "I failed to kill the application launched with name: %s", MYTH_WINDOW_NAME);
}
*/

//<-dceag-c187-b->

	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

void MythTV_Player::CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c187-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

	vector<string> numbers;
    StringUtils::Tokenize( sProgramID, "|", numbers );

	if ( numbers.size() == 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Invalid channel specification: \"%d\"", sProgramID.c_str());
		return;
	}

    string channelNumber = numbers[0];
    for( unsigned int i = 0; i < channelNumber.size(); i++ )
    {
        switch ( channelNumber[i] )
        {
            case '0': processKeyBoardInputRequest(XK_0); break;
            case '1': processKeyBoardInputRequest(XK_1); break;
            case '2': processKeyBoardInputRequest(XK_2); break;
            case '3': processKeyBoardInputRequest(XK_3); break;
            case '4': processKeyBoardInputRequest(XK_4); break;
            case '5': processKeyBoardInputRequest(XK_5); break;
            case '6': processKeyBoardInputRequest(XK_6); break;
            case '7': processKeyBoardInputRequest(XK_7); break;
            case '8': processKeyBoardInputRequest(XK_8); break;
            case '9': processKeyBoardInputRequest(XK_9); break;
            default:
                g_pPlutoLogger->Write(LV_STATUS, "Invalid character %c in channel identifier %s", channelNumber[i], channelNumber.c_str());
        }
    }

	processKeyBoardInputRequest(XK_Return);
}

void MythTV_Player::KillSpawnedDevices()
{
#ifndef WIN32
	signal(SIGCHLD, SIG_IGN);
#endif
	MythTV_Player_Command::KillSpawnedDevices();

	// This will only be called when we are dying, so we won't care about what happens to our spawned children.
	// We had a problem that KillSpawnedDevices called KillPids.sh, which when exited, called the sh signal handler and hung at wait
}

void MythTV_Player::ProcessExited(int pid, int status)
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Process exited %d %d", pid, status);

	void *data;
	string applicationName;
	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
		return;

	g_pPlutoLogger->Write(LV_STATUS, "Got application name: %s compare with %s", applicationName.c_str(), MYTH_WINDOW_NAME);

	if ( applicationName.compare(MYTH_WINDOW_NAME) == 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Send go back to the caller!");
		DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"");
		SendCommand(CMD_MH_Stop_Media_Cat);
	}
}

//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Capture a Video frame */
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

void MythTV_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Method was called here");
//     if ( m_pMythTV == NULL || m_pMythTV->GetState() != kState_WatchingLiveTV )
//     {
//         g_pPlutoLogger->Write(LV_STATUS, "Invalid state.");
//         EVENT_Error_Occured("Not playing TV at this time. Can't take a screen shot");
//         return;
//     }
//
//     VideoFrame *grabbedFrame;
//     VideoFrame actualFrame;
//
//     grabbedFrame = m_pMythTV->activenvp->GetCurrentFrame(actualFrame.width, actualFrame.height);
//
//     actualFrame.bpp = grabbedFrame->bpp;
//     actualFrame.size = grabbedFrame->size;
//     actualFrame.frameNumber = grabbedFrame->frameNumber;
//     actualFrame.timecode = grabbedFrame->timecode;
//     actualFrame.codec = grabbedFrame->codec;
//     actualFrame.interlaced_frame = grabbedFrame->interlaced_frame;
//     actualFrame.top_field_first = grabbedFrame->top_field_first;
//
//     actualFrame.buf = new unsigned char[actualFrame.size];
//     memcpy(actualFrame.buf, grabbedFrame->buf, actualFrame.size);
//
//     m_pMythTV->activenvp->ReleaseCurrentFrame(grabbedFrame);
//
//     g_pPlutoLogger->Write(LV_STATUS, "Got frame size %dx%d (%d) %d)",
//                 actualFrame.width, actualFrame.height,
//                 actualFrame.size, actualFrame.codec);
//
//     delete actualFrame.buf;
//     int x, y, nWidth, nHeight;
//     m_pMythTV->activenvp->getVideoOutput()->GetDrawSize(x, y, nWidth, nHeight);
//
//     g_pPlutoLogger->Write(LV_STATUS, "DrawSize: %dx%d %dx%d", x, y, nWidth, nHeight);

//     sCMD_Result = m_pMythTV->GetScreenFrame(sDisable_Aspect_Lock, iWidth, iHeight, pData, iData_Size, sFormat);
}


//<-dceag-c129-b->

	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

void MythTV_Player::CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c129-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

	CMD_Move_Up(sCMD_Result, pMessage);
}

//<-dceag-c130-b->

	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

void MythTV_Player::CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c130-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

	CMD_Move_Down(sCMD_Result, pMessage);
}
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */

void MythTV_Player::CMD_EnterGo(string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	processKeyBoardInputRequest(XK_Return);
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Up */

void MythTV_Player::CMD_Move_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	processKeyBoardInputRequest(XK_Up);
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Down */

void MythTV_Player::CMD_Move_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	processKeyBoardInputRequest(XK_Down);
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Left */

void MythTV_Player::CMD_Move_Left(string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_Left);
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Right */

void MythTV_Player::CMD_Move_Right(string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_Right);
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void MythTV_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_0);
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void MythTV_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_1);
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void MythTV_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_2);
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void MythTV_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_3);
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void MythTV_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_4);
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void MythTV_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_5);
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void MythTV_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_6);
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void MythTV_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_7);
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void MythTV_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_8);
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void MythTV_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_9);
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */

void MythTV_Player::CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
    processKeyBoardInputRequest(XK_Escape);
}
//<-dceag-createinst-b->!
//<-dceag-sample-b->!
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

void MythTV_Player::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void MythTV_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void MythTV_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void MythTV_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void MythTV_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void MythTV_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void MythTV_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

void MythTV_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void MythTV_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
