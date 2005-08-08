/*
    App_Server

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/

/*
	AppServer will also set the State of the m/d to indicate the currently installed operating system on the hard drive (LINUX / WINDOWS)
	and will set the Status to one of the following:
	MD_ON, PC_ON, MD_BLACK, PC_BLACK, MD_OFF, PC_OFF, MD_REBOOTING, PC_REBOOTING

	Where MD means it booted up or will boot up as a media director over the network, and PC means a normal hard drive boot
	ON = it's on and the display is on.  BLACK = The display was shut off
	OFF = It's turned off   REBOOTING = It's being rebooted and should come online soon
*/

//<-dceag-d-b->
#include "App_Server.h"
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

#ifndef WIN32
#include "utilities/linux/RatpoisonHandler.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <signal.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif

#ifdef WIN32
void EnablePrivileges()
{
	HANDLE hToken; // handle to process token
	TOKEN_PRIVILEGES tkp; // pointer to token structure
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES |
					TOKEN_QUERY, &hToken); // Get the LUID for shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// Get shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
}
#endif

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
App_Server *g_pAppServer = NULL;

void sh(int i) /* signal handler */
{
    if ( g_pAppServer && g_pAppServer->m_bQuit )
		return;

    int status = 0;
    pid_t pid = 0;


    pid = wait(&status);

    if ( g_pAppServer )
        g_pAppServer->ProcessExited(pid, WEXITSTATUS(status));
}
#endif

#define LOGO_APPLICATION_NAME "gimageview"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
App_Server::App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: App_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_AppMutex("AppServer")
{
	m_AppMutex.Init(NULL);
#ifndef WIN32
	g_pAppServer = this;
    signal(SIGCHLD, sh); /* install handler */
#endif
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
App_Server::~App_Server()
//<-dceag-dest-e->
{
//	vector<void *> data;
//	ProcessUtils::KillApplication(LOGO_APPLICATION_NAME, data);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool App_Server::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

bool App_Server::Connect(int iPK_DeviceTemplate )
{
	bool bResult = App_Server_Command::Connect(iPK_DeviceTemplate);
	m_dwPK_Device_MD = m_dwPK_Device; // Shouldn't use this value.  Only relevant if something is corrupted below

	// Find the media director
	DeviceData_Base *pDevice_This = m_pData->m_AllDevices.m_mapDeviceData_Base_Find( m_dwPK_Device );
	if( pDevice_This && pDevice_This->m_pDevice_ControlledVia && pDevice_This->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Computers_CONST) )
		m_dwPK_Device_MD = pDevice_This->m_pDevice_ControlledVia->m_dwPK_Device;
	else if( pDevice_This && pDevice_This->m_pDevice_ControlledVia && pDevice_This->m_pDevice_ControlledVia->m_pDevice_ControlledVia &&
			pDevice_This->m_pDevice_ControlledVia->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Computers_CONST) )
		m_dwPK_Device_MD = pDevice_This->m_pDevice_ControlledVia->m_dwPK_Device;


	if( m_bHardDrive )
	{
		// Store whether this hard drive boot is windows or linux
#ifdef WIN32
		SetState("WINDOWS",m_dwPK_Device_MD);
#else
		SetState("LINUX",m_dwPK_Device_MD);
#endif
	}

	if( m_bHardDrive )
		SetStatus("PC_ON",m_dwPK_Device_MD);
	else
		SetStatus("MD_ON",m_dwPK_Device_MD);

	return bResult;
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
void App_Server::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void App_Server::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a keypress event to an application */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void App_Server::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    cout << "Need to implement command #28 - Simulate Keypress" << endl;
    cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c67-b->

	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawn an application */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments, tab delimited */
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */
		/** @param #115 Show logo */
			/** If this is set then we will first select the logo  before spawning the application. */

void App_Server::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
	PLUTO_SAFETY_LOCK(ap,m_AppMutex);
	g_pPlutoLogger->Write(LV_STATUS,"SpawnApp file: %s name %s args %s failure %s logo %d",
		sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),(int) bShow_logo);
	if ( bShow_logo )
	{
		// Popup a black window so the user doesn't see some other program that may be running while he's
		// waiting for this one to appear
#ifndef WIN32
		Display *XServerDisplay;
	    if( (XServerDisplay = XOpenDisplay(getenv("DISPLAY"))) != NULL)
	    {
		    int iCurrentScreen   = XDefaultScreen(XServerDisplay);
			static Window w=0;
			if( w )
				XDestroyWindow(XServerDisplay,w);

			w = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
							0, 0, (DisplayWidth(XServerDisplay, iCurrentScreen)), DisplayHeight(XServerDisplay, iCurrentScreen),
							1, 0, 0);

			XClassHint		classHint;
			classHint.res_name = (char*) ("AppServer " + StringUtils::itos(m_dwPK_Device)).c_str();
			classHint.res_class = (char*) ("AppServer " + StringUtils::itos(m_dwPK_Device)).c_str();
			XSetClassHint ( XServerDisplay, w, &classHint );

			XMapWindow(XServerDisplay, w);

			XSync(XServerDisplay, True);
			g_pPlutoLogger->Write(LV_STATUS,"Window %d",(int) w);
		}
#endif
	}

	if (! ProcessUtils::SpawnApplication(sFilename, sArguments, sName, new pair<string, string>(sSendOnFailure, sSendOnSuccess)) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Can't spawn '%s': %s %s.", sName.c_str(), sFilename.c_str(), sArguments.c_str());
        sCMD_Result = "Failed";
    }
    else
    {
        sCMD_Result = "OK";
    }
	g_pPlutoLogger->Write(LV_WARNING, "Finished spawning '%s': %s %s.", sName.c_str(), sFilename.c_str(), sArguments.c_str());
}

//<-dceag-c69-b->

	/** @brief COMMAND: #69 - Kill Application */
	/** Kill an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

void App_Server::CMD_Kill_Application(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c69-e->
{
#ifndef WIN32
	vector<void *> messagesToSend;
	if ( ! ProcessUtils::KillApplication(sName, messagesToSend) )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Failed to kill the application with name: %s", sName.c_str());
	}

	for(vector<void *>::const_iterator itAttachedData = messagesToSend.begin();itAttachedData != messagesToSend.end();++itAttachedData)
	{
void *pV = (*itAttachedData);
g_pPlutoLogger->Write(LV_WARNING, "pV %p",pV);
		pair<string, string> *pStringsPair = (pair<string, string> *)(*itAttachedData);
g_pPlutoLogger->Write(LV_WARNING, "s1 %s s2 %s",pStringsPair->first.c_str(),pStringsPair->second.c_str());

		SendMessageList(pStringsPair->first);
g_pPlutoLogger->Write(LV_WARNING, "after send message");
		delete pStringsPair;
	}
#endif
}

//<-dceag-c70-b->

	/** @brief COMMAND: #70 - Hide Application */
	/** Hide an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

void App_Server::CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c70-e->
{
    cout << "Need to implement command #70 - Hide Application" << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

/* Private */
void App_Server::EnsureLogoIsDisplayed()
{
	return;  // don't do this anymore, it was a real mess
	if ( ! ProcessUtils::ApplicationIsLaunchedByMe(LOGO_APPLICATION_NAME) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "The application is not running! Need to launch it!");
		ProcessUtils::SpawnApplication("gimv", "/usr/pluto/share/wait-screen.gif", LOGO_APPLICATION_NAME, NULL);
	}

	g_pPlutoLogger->Write(LV_STATUS, "The 'application spawning' background is running!. Telling the orbiter to select the application!");

	DCE::CMD_Activate_Window_Cat activateApplicationCommand(m_dwPK_Device, DEVICECATEGORY_Orbiter_CONST, true, BL_SameComputer, LOGO_APPLICATION_NAME);

	string response; // this is not used but we need it to force the framework to not return until the command is actually executed.
	SendCommand(activateApplicationCommand, &response);
}

void App_Server::ProcessExited(int pid, int status)
{
	PLUTO_SAFETY_LOCK(ap,m_AppMutex);
	string applicationName;
	void *data;

	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "App_Server::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
		return;
	}

	if ( data )
	{
		pair<string, string> *pMessagesLists = (pair<string, string>*)data;
		SendMessageList( status ? pMessagesLists->first : pMessagesLists->second);
		delete pMessagesLists;
	}
}

void App_Server::SendMessageList(string messageList)
{
    g_pPlutoLogger->Write(LV_STATUS, "Sending this command chain: -->%s<--", messageList.c_str() );

    vector<string> commands;
    StringUtils::Tokenize(messageList, ",", commands);

    g_pPlutoLogger->Write(LV_STATUS, "Command count: %d", commands.size());

    if ( commands.size() == 0 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Empty command array resulted from the string: %s", messageList.c_str());
        return;
    }

    Message *pMessage = new Message(commands[0]);    // empty message (to only caryy the rest with him).
	g_pPlutoLogger->Write(LV_STATUS, "Message %s to: %d type: %d id: %d",commands[0].c_str(),
			pMessage->m_dwID,pMessage->m_dwMessage_Type,pMessage->m_dwID);

	for ( unsigned int i = 1; i < commands.size(); i++ )
	{
		pMessage->m_vectExtraMessages.push_back(new Message(commands[i]));
		g_pPlutoLogger->Write(LV_STATUS, "Adding Message %s ",commands[i].c_str());
	}

	QueueMessageToRouter( pMessage  );
    g_pPlutoLogger->Write(LV_STATUS, "After queing the message to the router queue!");
}

//<-dceag-sample-b->!
//<-dceag-createinst-b->!

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halt or reboot this device */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */

void App_Server::CMD_Halt_Device(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{
	if( sForce=="" )
		sForce = "H"; // HACK - todo: figure out how to determine if we have suspend capabilities

	// See who sent this to us.  If it wasn't GeneralInfoPlugin we want to forward it to it
	DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessage->m_dwPK_Device_From);
	if( !pDeviceData_Base || pDeviceData_Base->m_dwPK_DeviceCategory!=DEVICECATEGORY_General_Info_Plugins_CONST )
	{
		Message *pMessageCopy = new Message(pMessage);
		pMessageCopy->m_dwPK_Device_To=0;
		pMessageCopy->m_dwPK_Device_Category_To = DEVICECATEGORY_General_Info_Plugins_CONST;

	}

	switch( sForce[0] )
	{
	case 'H':
		if( m_bHardDrive )
			SetStatus("PC_OFF",m_dwPK_Device_MD);
		else
			SetStatus("MD_OFF",m_dwPK_Device_MD);
#ifndef WIN32
		g_pPlutoLogger->Write(LV_STATUS,"Calling halt");
		system("halt");
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
#endif

		// TODO: Power off this system
		break;
	case 'S':
		if( m_bHardDrive )
			SetStatus("PC_SUSPEND",m_dwPK_Device_MD);
		else
			SetStatus("MD_SUSPEND",m_dwPK_Device_MD);

#ifndef WIN32
		g_pPlutoLogger->Write(LV_STATUS,"Calling halt");
		system("halt");  // Don't know how to do this
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
#endif
		break;
	case 'R':
	case 'N':
	case 'V':
		if( sForce[0]=='R' )
			SetStatus("REBOOTING",m_dwPK_Device_MD);
		else if( sForce[0]=='N' )
			SetStatus("MD_REBOOTING",m_dwPK_Device_MD);
		else if( sForce[0]=='V' )
			SetStatus("PC_REBOOTING",m_dwPK_Device_MD);

#ifndef WIN32
		g_pPlutoLogger->Write(LV_STATUS,"Calling reboot");
		system("reboot");  // Don't know how to do this
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
#endif
		break;
	}
}

void App_Server::KillSpawnedDevices()
{
#ifndef WIN32
	signal(SIGCHLD, SIG_IGN);
#endif
	App_Server_Command::KillSpawnedDevices();

	// This will only be called when we are dying, so we won't care about what happens to our spawned children.
	// We had a problem that KillSpawnedDevices called KillPids.sh, which when exited, called the sh signal handler and hung at wait
}
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Increase the volume 1% */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void App_Server::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	if (iRepeat_Command < 1)
		iRepeat_Command = 1;
#ifndef WIN32
	int pid = fork();
	if (pid == 0)
	{
		execl("/usr/bin/amixer", "amixer", "sset", "Master", (StringUtils::itos(iRepeat_Command) + "+").c_str(), NULL);
		exit(99);
	}
#endif	
	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Decrease the volume 1% */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void App_Server::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	if (iRepeat_Command < 1)
		iRepeat_Command = 1;

#ifndef WIN32
	int pid = fork();
	if (pid == 0)
	{
		execl("/usr/bin/amixer", "amixer", "sset", "Master", (StringUtils::itos(iRepeat_Command) + "-").c_str(), NULL);
		exit(99);
	}
#endif

	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c313-b->

	/** @brief COMMAND: #313 - Set Volume */
	/** Set the volume to a specific level between 0-100 */
		/** @param #76 Level */
			/** A value between 0 and 100 where 0 is mute and 100 is full volume.  Numbers preceded with a - or + are relative.  +15 means up 15, -10 means down 10 */

void App_Server::CMD_Set_Volume(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c313-e->
{
	bool bRelative = false;
	if ( sLevel.size()>1 && sLevel[0] == '+' || sLevel[0] == '-')
		bRelative = true;

#ifndef WIN32
	int pid = fork();
	if (pid == 0)
	{
		execl("/usr/bin/amixer", "amixer", "sset", "Master", (sLevel.substr(bRelative ? 1 : 0) + "%" + (bRelative ? sLevel.substr(0, 1) : "")).c_str(), NULL);
		exit(99);
	}
#endif

	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}
