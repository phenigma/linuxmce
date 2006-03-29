/*
 Disk_Drive

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#include "Disk_Drive.h"
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
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stropts.h>

#include <sys/wait.h>

extern "C"
{
    #include <linux/cdrom.h>
}

#define SERVER_PORT 5150

//<-dceag-const-b->! custom
Disk_Drive::Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Disk_Drive_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
        m_monitorEnabled(true), m_serverPid(-1), m_serverPort(SERVER_PORT)
//<-dceag-const-e->
{
}

//<-dceag-getconfig-b->
bool Disk_Drive::GetConfig()
{
	if( !Disk_Drive_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	string sDrive;
	
	system("modprobe ide-generic");
	system("modprobe ide-cd");
	sDrive = DATA_Get_Drive();
	if (sDrive == "")
	{
		sDrive = "/dev/cdrom";
		if (!FileUtils::FileExists(sDrive))
			sDrive = "/dev/cdrom1";
	}
	
	m_pDisk_Drive_Functions = new Disk_Drive_Functions(this, sDrive);

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Disk_Drive::~Disk_Drive()
//<-dceag-dest-e->
{
	delete m_pDisk_Drive_Functions;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Disk_Drive::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will
	get all	commands for your children in ReceivedCommandForChild, where
	pDeviceData_Base is the child device.  If you handle the message, you
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Disk_Drive::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Disk_Drive::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}


/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c45-b->

	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage)
//<-dceag-c45-e->
{
    g_pPlutoLogger->Write(LV_ACTION,"Turning ON the disk drive monitoring.");
    m_monitorEnabled = true;
    m_pDisk_Drive_Functions->cdrom_lock(0);
}

//<-dceag-c46-b->

	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage)
//<-dceag-c46-e->
{
    g_pPlutoLogger->Write(LV_STATUS,"Turning OFF the Disk Drive Monitoring.");
    m_monitorEnabled = false;
    m_pDisk_Drive_Functions->cdrom_lock(1);
}

//<-dceag-c47-b->

	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Disk_Drive::CMD_Reset_Disk_Drive(int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c47-e->
{
	m_pDisk_Drive_Functions->m_mediaInserted = false;
	m_pDisk_Drive_Functions->m_mediaDiskStatus = DISCTYPE_NONE;
	m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Checking disc...");
	
	m_pDisk_Drive_Functions->internal_reset_drive(true);
}

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Disk_Drive::CMD_Eject_Disk(int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
	static time_t tLastEject=0;

	g_pPlutoLogger->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk  tLastEject %d (%d) tray open: %d",
		(int) tLastEject, (int) time(NULL), (int) m_pDisk_Drive_Functions->m_bTrayOpen);

	if( time(NULL)-tLastEject<=2 )  // It can take the drive a while to spin down and the user hits eject multiple times
	{
		g_pPlutoLogger->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk skipping eject within last 2 seconds");
		return;
	}
	if( m_pDisk_Drive_Functions->m_bTrayOpen )
	{
		m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Closing tray...");
	    system("eject -t");
	}
	else
	{
		m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Opening tray...");
	    system("eject");
	}

	m_pDisk_Drive_Functions->m_mediaInserted = false;  // Be sure we re-identify any media in there
	m_pDisk_Drive_Functions->m_mediaDiskStatus = DISCTYPE_NONE;
	tLastEject = time(NULL); // Put this after the system call so we know when it's been less than 2 seconds since a successful one
}

//<-dceag-c49-b->

	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

void Disk_Drive::CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c49-e->
{
    cout << "Need to implement command #49 - Start Burn Session" << endl;
}

//<-dceag-c50-b->

	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

void Disk_Drive::CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c50-e->
{
    cout << "Need to implement command #50 - Start Ripping Session" << endl;
}

//<-dceag-c51-b->

	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

void Disk_Drive::CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c51-e->
{
    cout << "Need to implement command #51 - Add File To Burning Session" << endl;
}

//<-dceag-c52-b->

	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

void Disk_Drive::CMD_Start_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c52-e->
{
    cout << "Need to implement command #52 - Start Burning" << endl;
}

//<-dceag-c53-b->

	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

void Disk_Drive::CMD_Abort_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c53-e->
{
}

//<-dceag-c54-b->

	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

void Disk_Drive::CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c54-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Got a mount media request %s", sFilename.c_str());
    string stringMRL;

	if (! m_pDisk_Drive_Functions->mountDVD(sFilename, stringMRL))
	{
		g_pPlutoLogger->Write(LV_WARNING, "Error executing the dvd mounting script (message: %s). Returning error.", stringMRL.c_str());
		sCMD_Result = "NOT_OK";
		*sMediaURL = stringMRL;
		return;
	}

	*sMediaURL += stringMRL;
	sCMD_Result = "OK";
	g_pPlutoLogger->Write(LV_STATUS, "Returning new media URL: %s", sMediaURL->c_str());
}

//<-dceag-c55-b->

	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

void Disk_Drive::CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage)
//<-dceag-c55-e->
{
	if (! m_pDisk_Drive_Functions->m_pDevice_AppServer)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot Abort rip -- no app server");
		sCMD_Result="NO App_Server";
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS,"Aborting Ripping");
	DCE::CMD_Kill_Application
		CMD_Kill_Application(m_dwPK_Device,
						m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
						"rip_" + StringUtils::itos(m_dwPK_Device) + "_0",true);

    SendCommand(CMD_Kill_Application);
}

//<-dceag-c56-b->

	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

void Disk_Drive::CMD_Format_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c56-e->
{
    cout << "Need to implement command #56 - Format Drive" << endl;
}

//<-dceag-c57-b->

	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

void Disk_Drive::CMD_Close_Tray(string &sCMD_Result,Message *pMessage)
//<-dceag-c57-e->
{
    cout << "Need to implement command #57 - Close Tray" << endl;
}


void Disk_Drive::RunMonitorLoop()
{
    m_pDisk_Drive_Functions->internal_monitor_step(false); // ignore any drive that is in the drive at the start.

    bool done = false;
    while ( ! done  && !m_bQuit )
    {
        done = ! m_pDisk_Drive_Functions->internal_monitor_step(true);
        sleep(3); // Sleep 3 seconds
    }
}

//<-dceag-sample-b->! no sample
//<-dceag-createinst-b->!
//<-dceag-c337-b->

	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #20 Format */
			/** wav, flac, ogg, etc. */
		/** @param #50 Name */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */
		/** @param #131 EK_Disc */
			/** The ID of the disc to rip */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Disk_Drive::CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	m_pDisk_Drive_Functions->CMD_Rip_Disk(iPK_Users, sFormat, sName, sTracks, iEK_Disc, iDrive_Number, sCMD_Result, pMessage);
}
