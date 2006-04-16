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
#include "pluto_main/Define_Button.h"
#include "PlutoUtils/ProcessUtils.h"

#include <sstream>
// #include <qsqldatabase.h>
//
// #include <libmythtv/programinfo.h>
// #include <libmythtv/remoteencoder.h>
// #include <libmythtv/remoteutil.h>

#include <pthread.h>
// #include "MythMainWindowResizable.h"
MythTV_Player *g_pMythPlayer = NULL;
#define MYTH_SOCKET_TIMEOUT	5  // SECONDS

#ifndef WIN32
#include "utilities/linux/RatpoisonHandler.h"

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>

#include <sys/wait.h>


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

#define MYTH_WINDOW_NAME "mythfrontend"

class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
    bool commandRatPoison(string command) { return RatpoisonHandler<RatPoisonWrapper>::commandRatPoison(command); }
};

#endif

void* monitorMythThread(void* param)
{
	while(g_pMythPlayer->m_bExiting==false)
	{
		// If we're disconnected, check more frequently for the signal to start up Myth.
		
		Sleep((g_pMythPlayer->m_mythStatus == MYTHSTATUS_DISCONNECTED) ? 10 : 900);
		g_pMythPlayer->pollMythStatus();
	}
	return NULL;
}

// MythContext *gContext;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_MythMutex("myth")
{
	m_MythMutex.Init(NULL);
	g_pMythPlayer = this;
	m_pDevice_MythTV_Plugin=NULL;
#ifndef WIN32 
	m_pRatWrapper = NULL;
	m_pMythSocket = NULL;
#endif
    m_iMythFrontendWindowId = 0;
    m_bExiting = false;
    m_threadMonitorMyth = 0;
    m_mythStatus = MYTHSTATUS_DISCONNECTED;
    
}

//<-dceag-getconfig-b->
bool MythTV_Player::GetConfig()
{
	if( !MythTV_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	m_pDevice_MythTV_Plugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_MythTV_PlugIn_CONST);
	if( !m_pDevice_MythTV_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"I need a myth plugin to function");
		return false;
	}
	// Kill any existing myth front ends.   The application spawner / window focus isn't happy
	// if we didn't create it ourselves.
	
 	system("killall mythfrontend");
	Sleep(500);
	m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));
	signal(SIGCHLD, sh); /* install handler */
 	pthread_create(&m_threadMonitorMyth, NULL, monitorMythThread, NULL);
 
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_Player::~MythTV_Player()
//<-dceag-dest-e->
{
	// Kill any instances we spawned
	vector<void *> data;
#ifndef WIN32
	ProcessUtils::KillApplication(MYTH_WINDOW_NAME, data);

    delete m_pRatWrapper;
#endif
	m_bExiting = true;
	if (m_threadMonitorMyth != 0)
		pthread_join(m_threadMonitorMyth, NULL);
	delete m_pMythSocket;
}

bool MythTV_Player::LaunchMythFrontend(bool bSelectWindow)
{
#ifndef WIN32
	if ( ! m_pRatWrapper )
		m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));

	ProcessUtils::SpawnApplication("/usr/bin/mythfrontend", "", MYTH_WINDOW_NAME);
	
	if( bSelectWindow )
	{
		selectWindow();
		locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
	}
#endif	
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
#ifndef WIN32
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
#endif
	return true;
}
void MythTV_Player::CreateChildren()
{
/*
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;

#ifndef WIN32
    if ( ! locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay())) )
    {
		LaunchMythFrontend(false);
        locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
    }
#endif
i*/
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
void MythTV_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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


void MythTV_Player::updateMode(string toMode)
{
	if (toMode!=m_CurrentMode)
	{				
		m_CurrentMode = toMode;
		g_pPlutoLogger->Write(LV_WARNING,"Changing mode: %s",toMode.c_str());
		DCE::CMD_Set_Active_Menu CMD_Set_Active_Menu_(m_dwPK_Device,m_pDevice_MythTV_Plugin->m_dwPK_Device, toMode);
		SendCommand(CMD_Set_Active_Menu_);
	}
}

void MythTV_Player::pollMythStatus()
{
	if (m_mythStatus == MYTHSTATUS_STARTUP)
	{
		PLUTO_SAFETY_LOCK(mm,m_MythMutex);

		LaunchMythFrontend();
		locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
		m_CurrentMode.clear();
		m_CurrentProgram.clear();
		m_mythStatus = MYTHSTATUS_LIVETV;
		
		// TODO: The controllers don't get updated if I set this there. Why?
		// updateMode("live");
	
		string sResult;
		time_t timeout=40+time(NULL);
		
		do
		{
		    mm.Release();
		    Sleep(100);
		    mm.Relock();
		    sResult = sendMythCommand("jump livetv");
		    g_pPlutoLogger->Write(LV_WARNING, "%s", sResult.c_str());
		} while(time(NULL) < timeout && sResult != "OK" && m_mythStatus == MYTHSTATUS_LIVETV);
		if (time(NULL) >= timeout)
		{
			DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"");
			SendCommand(CMD_MH_Stop_Media_Cat);
			m_mythStatus = MYTHSTATUS_DISCONNECTED;
			g_pPlutoLogger->Write(LV_CRITICAL,"Failed initial communications with Mythfrontend.");
			vector<void *> data;
			ProcessUtils::KillApplication(MYTH_WINDOW_NAME, data);			
		}		
	} 
	else if (m_mythStatus != MYTHSTATUS_DISCONNECTED)
	{
		PLUTO_SAFETY_LOCK(mm2,m_MythMutex);
		
		string sResult = sendMythCommand("query location");

		if(sResult.length())
		{
			vector<string> vectResults;
			StringUtils::Tokenize(sResult, " ", vectResults);

			if (vectResults[0]=="Playback")
			{
				DCE::CMD_Update_Time_Code CMD_Update_Time_Code_(m_dwPK_Device,m_pDevice_MythTV_Plugin->m_dwPK_Device, 0, vectResults[2], vectResults[4], (vectResults[5]=="1x") ? string("") : vectResults[5], "", ""); 
				SendCommand(CMD_Update_Time_Code_);                
			}
			string SetMode = vectResults[0];
			if (SetMode == "Playback")
			{
				m_mythStatus = MYTHSTATUS_PLAYBACK;
				if (vectResults[1]=="LiveTV")
				{
					int CurTime = StringUtils::TimeAsSeconds(vectResults[2]);
					int EndTime = StringUtils::TimeAsSeconds(vectResults[4]);
					
					// Have a 2 second "buffer" for switching between live and nonlive modes so we don't get 
					// flapping.    Unfortunately it takes live TV so long to start up at times that we need
					// to show up to 8 seconds or so as live. 
					
					if (m_CurrentMode == "live")
					{
						if (CurTime < EndTime - 10)
							SetMode = "nonlive";
						else
							SetMode = "live";
					} 
					else
					{
						if (CurTime < EndTime - 8)
							SetMode = "nonlive";
						else
							SetMode = "live";
					}
				}
				else
					SetMode="nonlive";
			}
			else
			{
				if (SetMode == "GuideGrid")
					m_mythStatus = MYTHSTATUS_GUIDEGRID;
				else
					m_mythStatus = MYTHSTATUS_MENU;
			}
			updateMode(SetMode);	
		}
	}
}

void MythTV_Player::selectWindow()
{
#ifndef WIN32
    m_pRatWrapper->commandRatPoison(":select " MYTH_WINDOW_NAME);
#endif
}

void MythTV_Player::processKeyBoardInputRequest(int iXKeySym)
{
#ifndef WIN32
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
#endif
}

string MythTV_Player::sendMythCommand(const char *Cmd)
{
	string sResponse;
	char ch=0;

	g_pPlutoLogger->Write(LV_WARNING,"Going to send command %s",Cmd);
	
	if (m_pMythSocket == NULL)
	{
		m_pMythSocket = new PlainClientSocket("localhost:10001");
		if( !m_pMythSocket->Connect() )
		{
			delete m_pMythSocket;
			m_pMythSocket = NULL;
			g_pPlutoLogger->Write(LV_CRITICAL,"Unable to connect to MythTV client");
			return "";
		}
		g_pPlutoLogger->Write(LV_STATUS, "Connected");
	}
	

	// Wait for Myth's console prompt.
	do
	{
		if ( !m_pMythSocket->ReceiveData( 1, &ch, MYTH_SOCKET_TIMEOUT ) ) 
		{
			delete m_pMythSocket;
			m_pMythSocket = NULL;
			g_pPlutoLogger->Write(LV_CRITICAL,"Timeout waiting for Myth prompt.");
			return "";
		}
	} while(ch!='#');
	if( !m_pMythSocket->SendString(Cmd) )
	{
		delete m_pMythSocket;
		m_pMythSocket = NULL;
		g_pPlutoLogger->Write(LV_CRITICAL,"Could not send string");
		return "";
	}
	// Receive the response
	do
	{
		if( !m_pMythSocket->ReceiveString(sResponse,MYTH_SOCKET_TIMEOUT) )
		{
			delete m_pMythSocket;
			m_pMythSocket = NULL;
			g_pPlutoLogger->Write(LV_CRITICAL,"Didn't get reply.");
			return "";
		}
	// FIXIT: In Myth's networkcontrol.cpp, it's possible that Myth replies to a previous command with a "lost" query location.  If we get "OK" when asking for status,
	// we're off kilter.  Receive one more line.
	} while (!strcmp(Cmd, "query location") && sResponse=="OK");
		
	sResponse = StringUtils::TrimSpaces(sResponse);
	g_pPlutoLogger->Write(LV_WARNING,"Myth Responded %s",sResponse.c_str());
	
/*	if( !m_pMythSocket->SendString("QUIT") )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Could not send QUIT string");
		return sResponse;
	}
	m_pMythSocket->Close(); */
	return sResponse;
}

bool MythTV_Player::checkWindowName(long unsigned int window, string windowName)
{
#ifndef WIN32
	XTextProperty text;

	if ( ! checkXServerConnection())
		return false;

	if ( XGetWMName (m_pRatWrapper->getDisplay(), window, &text) && windowName == string((const char*)text.value) )
        return true;
#endif
    return false;
}

bool MythTV_Player::locateMythTvFrontendWindow(long unsigned int window)
{
#ifndef WIN32
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
#endif
    return false;
}

/*

 COMMANDS TO IMPLEMENT

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

	if ( numbers.size() == 0 )	{
		g_pPlutoLogger->Write(LV_STATUS, "Invalid channel specification: \"%d\"", sProgramID.c_str());
		return;
	}

	string sTuneCMD = string("play channel ")+numbers[0];
	sendMythCommand(sTuneCMD.c_str());
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
#ifndef WIN32
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
	m_mythStatus = MYTHSTATUS_DISCONNECTED;
#endif
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
	/** Select the currently highlighted menu item */

void MythTV_Player::CMD_EnterGo(string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key enter");
//	processKeyBoardInputRequest(XK_Return);
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */

void MythTV_Player::CMD_Move_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	// sendMythCommand("key up");
	processKeyBoardInputRequest(XK_Up);
	
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */

void MythTV_Player::CMD_Move_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key down");
//	processKeyBoardInputRequest(XK_Down);
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */

void MythTV_Player::CMD_Move_Left(string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key left");
//	processKeyBoardInputRequest(XK_Left);

}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */

void MythTV_Player::CMD_Move_Right(string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key right");
//	processKeyBoardInputRequest(XK_Right);
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void MythTV_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 0");
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void MythTV_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 1");
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void MythTV_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 2");
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void MythTV_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 3");
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void MythTV_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 4");
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void MythTV_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 5");
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void MythTV_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 6");
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void MythTV_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 7");
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void MythTV_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 8");
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void MythTV_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 9");
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */

void MythTV_Player::CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key escape");
#endif
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
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	if ( ! checkXServerConnection())
		return;
	
	if ( ! locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay())) )
	{
		m_mythStatus = MYTHSTATUS_STARTUP;
	}
	else
	{
		if (m_mythStatus == MYTHSTATUS_LIVETV || m_mythStatus == MYTHSTATUS_PLAYBACK)
			sendMythCommand("play speed normal");
		else
			sendMythCommand("key P");
	}
	selectWindow();
#endif
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void MythTV_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	m_mythStatus = MYTHSTATUS_DISCONNECTED;

	if ( ! checkXServerConnection())
		return;

	vector<void *> data;
	if ( ProcessUtils::KillApplication(MYTH_WINDOW_NAME, data) == false )
		g_pPlutoLogger->Write(LV_WARNING, "I failed to kill the application launched with name: %s", MYTH_WINDOW_NAME);
#endif
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void MythTV_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("play speed pause");
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void MythTV_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	sendMythCommand("play speed normal");
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void MythTV_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	// We send left/right for this
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	switch(iMediaPlaybackSpeed)
	{
	case 0:
		sendMythCommand("play speed pause");
		break;
	case 1:
	case 1000:
		sendMythCommand("play speed normal");
		break;
	case 2:
	case 2000:
		sendMythCommand("play speed 2x");
		break;
	case -1:
	case -1000:
		// Myth cannot do -1x with hdtv.
		sendMythCommand("play speed -2x");
		break;
	case -2:
	case -2000:
		sendMythCommand("play speed -2x");
		break;
	case 4000:
		sendMythCommand("play speed 4x");
		break;
	case 8000:
		sendMythCommand("play speed 8x");
		break;
	case 16000:
		sendMythCommand("play speed 16x");
		break;
	case -4000:
		sendMythCommand("play speed -4x");
		break;
	case -8000:
		sendMythCommand("play speed -8x");
		break;
	case -16000:
		sendMythCommand("play speed -16x");
		break;
	default:
		g_pPlutoLogger->Write(LV_STATUS, "Don't know how to handle playback speed: \"%d\"", iMediaPlaybackSpeed);
		break;
	}

//	if( iMediaPlaybackSpeed==2 )
//		processKeyBoardInputRequest(XK_Right);
//	else
//		processKeyBoardInputRequest(XK_Left);
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void MythTV_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
	
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if (atoi(sValue_To_Assign.c_str()) < 0) 
		sendMythCommand("play seek rewind");
	else
		sendMythCommand("play seek forward");
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void MythTV_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if( sValue_To_Assign.size()==0 || sValue_To_Assign[0]=='+')
	{
		if (m_mythStatus == MYTHSTATUS_PLAYBACK)
			sendMythCommand("play channel up");
		else
			sendMythCommand("key pageup");
	}
	else
	{
		if (m_mythStatus == MYTHSTATUS_PLAYBACK)
			sendMythCommand("play channel down");
		else
			sendMythCommand("key pagedown");
	}
#endif
}

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
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	*sText = sendMythCommand("query location");

	g_pPlutoLogger->Write(LV_STATUS, "Status position: \"%s\"", sText->c_str());
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void MythTV_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void MythTV_Player::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	switch(atoi(sPK_Button.c_str()))
	{
	case BUTTON_Up_Arrow_CONST:
		sendMythCommand("key up");
		break;		
	case BUTTON_Down_Arrow_CONST:
		sendMythCommand("key down");
		break;	
	case BUTTON_Left_Arrow_CONST:
		sendMythCommand("key left");
		break;	
	case BUTTON_Right_Arrow_CONST:
		sendMythCommand("key right");
		break;	
	case BUTTON_Scroll_Up_CONST:
		sendMythCommand("key pageup");
		break;
	case BUTTON_Scroll_Down_CONST:
		sendMythCommand("key pagedown");
		break;
	case BUTTON_Enter_CONST:
		sendMythCommand("key enter");
		break;	
		
	}
}

//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void MythTV_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
}
//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MythTV_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
}
//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MythTV_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
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

void MythTV_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
}
//<-dceag-c123-b->

	/** @brief COMMAND: #123 - Info */
	/** Info about the currently playing program */
		/** @param #9 Text */
			/** nimic */

void MythTV_Player::CMD_Info(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c123-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key I");
#endif
}
//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Go to the Guide */

void MythTV_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	updateMode("GuideGrid");	

	if (m_mythStatus == MYTHSTATUS_PLAYBACK)
		sendMythCommand("key S");
	else
		sendMythCommand("jump guidegrid");

	LockOrbiterPointer();
}
//<-dceag-c367-b->

	/** @brief COMMAND: #367 - Text */
	/**  */

void MythTV_Player::CMD_Text(string &sCMD_Result,Message *pMessage)
//<-dceag-c367-e->
{
}
//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Go to the PVR's main menu */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */

void MythTV_Player::CMD_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if( sText=="old_recordings" )
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump playbackrecordings");
	}
	else if( sText=="future_recordings" )
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump managerecordings");
	}
	else
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump mainmenu");
	}
}

//<-dceag-c762-b->

	/** @brief COMMAND: #762 - Live TV */
	/** Go to Live TV */

void MythTV_Player::CMD_Live_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c762-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	updateMode("live");
	sendMythCommand("jump livetv");
}
//<-dceag-c763-b->

	/** @brief COMMAND: #763 - Exit */
	/** Exit guide */

void MythTV_Player::CMD_Exit(string &sCMD_Result,Message *pMessage)
//<-dceag-c763-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key escape");
#endif
}

//<-dceag-c102-b->

	/** @brief COMMAND: #102 - Record */
	/** Record the current show */

void MythTV_Player::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key R");
}

//<-dceag-c777-b->

	/** @brief COMMAND: #777 - Recorded TV Menu */
	/**  */

void MythTV_Player::CMD_Recorded_TV_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c777-e->
{
	//TODO: implement me!
}

void MythTV_Player::LockOrbiterPointer()
{
	DCE::CMD_Set_Mouse_Behavior CMD_Set_Mouse_Behavior(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","K",false,"X");
	DCE::CMD_Set_Mouse_Behavior CMD_Set_Mouse_Behavior2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","K",false,"Y");
	DCE::CMD_Remove_Popup CMD_Remove_Popup(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","horiz");
	DCE::CMD_Remove_Popup CMD_Remove_Popup2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","left");

	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Mouse_Behavior2.m_pMessage);
	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup.m_pMessage);
	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup2.m_pMessage);
	SendCommand(CMD_Set_Mouse_Behavior);
}

void MythTV_Player::ReleaseOrbiterPointer()
{
	DCE::CMD_Remove_Popup CMD_Remove_Popup(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","horiz");
	DCE::CMD_Remove_Popup CMD_Remove_Popup2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","left");

	CMD_Remove_Popup.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup2.m_pMessage);
	SendCommand(CMD_Remove_Popup);
}

