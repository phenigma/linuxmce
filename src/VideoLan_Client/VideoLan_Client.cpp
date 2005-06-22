//<-dceag-d-b->
#include "VideoLan_Client.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "utilities/linux/RatpoisonHandler.h"

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define VLC_WINDOW_NAME "Unnamed"
// This shoould be the class name of the gimageview application
#define LOGO_APPLICATION_NAME "gimageview"

#include "PlutoUtils/ProcessUtils.h"

class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
    void commandRatPoison(string command) { RatpoisonHandler<RatPoisonWrapper>::commandRatPoison(command); }
};


#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
VideoLan_Client *g_pVideoLan_Client = NULL;

void sh(int i) /* signal handler */
{
    if ( g_pVideoLan_Client && g_pVideoLan_Client->m_bQuit )
		return;

    int status = 0;
    pid_t pid = 0;


    pid = wait(&status);

    if ( g_pVideoLan_Client )
        g_pVideoLan_Client->ProcessExited(pid, status);
}
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VideoLan_Client::VideoLan_Client(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VideoLan_Client_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));
    m_iVideoLanWindowId = 0;
	m_dwPK_Device_VideoLan_Server = 0;
#ifndef WIN32
	g_pVideoLan_Client = this;
    signal(SIGCHLD, sh); /* install handler */
#endif
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
VideoLan_Client::VideoLan_Client(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: VideoLan_Client_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
VideoLan_Client::~VideoLan_Client()
//<-dceag-dest-e->
{
	// Kill any instances we spawned
	vector<void *> data;
	ProcessUtils::KillApplication("vlc_c", data);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VideoLan_Client::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
VideoLan_Client_Command *Create_VideoLan_Client(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new VideoLan_Client(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void VideoLan_Client::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void VideoLan_Client::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

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

void VideoLan_Client::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << iMediaPosition << endl;

	m_dwPK_Device_VideoLan_Server = pMessage->m_dwPK_Device_From;

	string sCommand = "--intf\trc\t-f\t" + sFilename;

	if ( ! m_pRatWrapper )
        m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));

//	m_pRatWrapper->commandRatPoison(":select " LOGO_APPLICATION_NAME);
	if( ProcessUtils::SpawnApplication("vlc", sCommand.c_str(), "vlc_c")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to start videolan client");

    selectWindow();
    locateVlcFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));

/*

	VideoLanClientInstance *pVideoLanClientInstance = new VideoLanClientInstance(this,iStreamID,sCommand);
	pthread_create(&pVideoLanClientInstance->m_pthread_t, NULL, SpawnVideoLanClient, (void *) pVideoLanClientInstance);

	g_pPlutoLogger->Write(LV_STATUS,"Sleeping for 5 seconds");
	Sleep(5000);
	g_pPlutoLogger->Write(LV_STATUS,"active frame");
    makeActive("frame");
	Sleep(5000);
	g_pPlutoLogger->Write(LV_STATUS,"active Frame");
    makeActive("Frame");
	Sleep(5000);
	g_pPlutoLogger->Write(LV_STATUS,"active Unnamed");
    makeActive("Unnamed");
	Sleep(5000);
*/
}

bool VideoLan_Client::checkXServerConnection()
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

void VideoLan_Client::selectWindow()
{
    m_pRatWrapper->commandRatPoison(":select " VLC_WINDOW_NAME);
}

bool VideoLan_Client::checkWindowName(long unsigned int window, string windowName)
{
    XTextProperty text;

	if ( ! checkXServerConnection())
		return false;

	if ( XGetWMName (m_pRatWrapper->getDisplay(), window, &text) && windowName == string((const char*)text.value) )
        return true;

    return false;
}

bool VideoLan_Client::locateVlcFrontendWindow(long unsigned int window)
{
    Window parent_win, root_win, *child_windows;
    unsigned int num_child_windows;

	if ( ! checkXServerConnection())
		return false;

    if ( checkWindowName(window, VLC_WINDOW_NAME ) )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Found window id: 0x%x", window );
        m_iVideoLanWindowId = window;
        return true;
    }

    XQueryTree(m_pRatWrapper->getDisplay(), (Window)window, &root_win, &parent_win, &child_windows, &num_child_windows);

    for ( unsigned int i = 0; i < num_child_windows; i++ )
        if ( locateVlcFrontendWindow(child_windows[i]) )
            return true;

    /* we need to free the list of child IDs, as it was dynamically allocated */
    /* by the XQueryTree function.                                            */
    XFree(child_windows);
    return false;
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void VideoLan_Client::CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << iMediaPosition << endl;

	vector<void *> data;
	if( ProcessUtils::KillApplication("vlc_c" + StringUtils::itos(iStreamID),data)==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to stop VideoLan Client");
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void VideoLan_Client::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	cout << "Need to implement command #39 - Pause Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void VideoLan_Client::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void VideoLan_Client::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

void VideoLan_Client::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

void VideoLan_Client::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
}

//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void VideoLan_Client::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	cout << "Need to implement command #89 - Vol Up" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;
}

//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void VideoLan_Client::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	cout << "Need to implement command #90 - Vol Down" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;
}

//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

void VideoLan_Client::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	cout << "Need to implement command #97 - Mute" << endl;
}

//<-dceag-c243-b->

	/** @brief COMMAND: #243 - Enable Broadcasting */
	/** Enable broadcasting from here. */
		/** @param #41 StreamID */
			/** The stream to enable broadcast for */
		/** @param #59 MediaURL */
			/** The media url from which this stream can be played. */

void VideoLan_Client::CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c243-e->
{
	cout << "Need to implement command #243 - Enable Broadcasting" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
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

void VideoLan_Client::CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #39 - Options=" << sOptions << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << iMediaPosition << endl;
	cout << "Parm #106 - Media_Length=" << iMedia_Length << endl;
}

void VideoLan_Client::ProcessExited(int pid, int status)
{
	string applicationName;
	void *data;

	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
		g_pPlutoLogger->Write(LV_WARNING, "VideoLan_Client::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
	else
		g_pPlutoLogger->Write(LV_WARNING, "VideoLan_Client::ProcessExited() Child with pid %d terminated", pid);
}
