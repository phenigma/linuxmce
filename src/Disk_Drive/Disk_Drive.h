/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Disk_Drive_h
#define Disk_Drive_h

//	DCE Implemenation for #11 Disk Drive

#include "Gen_Devices/Disk_DriveBase.h"
//<-dceag-d-e->

#include <pthread.h>
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "JobHandler/JobHandler.h"

using namespace nsJobHandler;

class Database_pluto_media;

//<-dceag-decl-b->
namespace DCE
{
	class Disk_Drive : public Disk_Drive_Command
	{
//<-dceag-decl-e->
		Database_pluto_media *m_pDatabase_pluto_media;
		MediaAttributes_LowLevel *m_pMediaAttributes_LowLevel;
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Disk_Drive();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		
		// Private member variables

		JobHandler *m_pJobHandler;
		bool m_monitorEnabled;
		int  m_serverPid;
		int  m_serverPort;

		char *m_args[100];

public:
    // Public member variables
	virtual void PostConnect();

    virtual void RunMonitorLoop();
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Drive();
	void DATA_Set_Drive(string Value);

			*****EVENT***** accessors inherited from base class
	void EVENT_Media_Inserted(int iFK_MediaType,string sMRL,string sID,string sName);
	void EVENT_Ripping_Progress(string sText,int iResult,string sValue,string sName,int iEK_Disc,string sDrive_Number);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

	virtual void CMD_Disk_Drive_Monitoring_ON() { string sCMD_Result; CMD_Disk_Drive_Monitoring_ON(sCMD_Result,NULL);};
	virtual void CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

	virtual void CMD_Disk_Drive_Monitoring_OFF() { string sCMD_Result; CMD_Disk_Drive_Monitoring_OFF(sCMD_Result,NULL);};
	virtual void CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

	virtual void CMD_Reset_Disk_Drive(int iDrive_Number) { string sCMD_Result; CMD_Reset_Disk_Drive(iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Reset_Disk_Drive(int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

	virtual void CMD_Eject_Disk(int iDrive_Number) { string sCMD_Result; CMD_Eject_Disk(iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Eject_Disk(int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

	virtual void CMD_Start_Burn_Session() { string sCMD_Result; CMD_Start_Burn_Session(sCMD_Result,NULL);};
	virtual void CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

	virtual void CMD_Start_Ripping_Session() { string sCMD_Result; CMD_Start_Ripping_Session(sCMD_Result,NULL);};
	virtual void CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

	virtual void CMD_Add_File_To_Burning_Session() { string sCMD_Result; CMD_Add_File_To_Burning_Session(sCMD_Result,NULL);};
	virtual void CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

	virtual void CMD_Start_Burning() { string sCMD_Result; CMD_Start_Burning(sCMD_Result,NULL);};
	virtual void CMD_Start_Burning(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

	virtual void CMD_Abort_Burning() { string sCMD_Result; CMD_Abort_Burning(sCMD_Result,NULL);};
	virtual void CMD_Abort_Burning(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

	virtual void CMD_Mount_Disk_Image(string sFilename,string *sMediaURL) { string sCMD_Result; CMD_Mount_Disk_Image(sFilename.c_str(),sMediaURL,sCMD_Result,NULL);};
	virtual void CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

	virtual void CMD_Abort_Ripping() { string sCMD_Result; CMD_Abort_Ripping(sCMD_Result,NULL);};
	virtual void CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

	virtual void CMD_Format_Drive() { string sCMD_Result; CMD_Format_Drive(sCMD_Result,NULL);};
	virtual void CMD_Format_Drive(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

	virtual void CMD_Close_Tray() { string sCMD_Result; CMD_Close_Tray(sCMD_Result,NULL);};
	virtual void CMD_Close_Tray(string &sCMD_Result,Message *pMessage);


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
		/** @param #233 DriveID */
			/** The ID of the storage drive. Can be the ID of the core. */
		/** @param #234 Directory */
			/** The relative directory for the file to rip */

	virtual void CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number,int iDriveID,string sDirectory) { string sCMD_Result; CMD_Rip_Disk(iPK_Users,sFormat.c_str(),sName.c_str(),sTracks.c_str(),iEK_Disc,iDrive_Number,iDriveID,sDirectory.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #742 - Media Identified */
	/** Media has been identified */
		/** @param #2 PK_Device */
			/** The disk drive */
		/** @param #5 Value To Assign */
			/** The identified data */
		/** @param #10 ID */
			/** The ID of the disc */
		/** @param #19 Data */
			/** The picture/cover art */
		/** @param #20 Format */
			/** The format of the data */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #59 MediaURL */
			/** The URL for the disc drive */
		/** @param #131 EK_Disc */
			/** If a disc was added accordingly, this reports the disc id */
		/** @param #193 URL */
			/** The URL for the picture */

	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc) { string sCMD_Result; CMD_Media_Identified(iPK_Device,sValue_To_Assign.c_str(),sID.c_str(),pData,iData_Size,sFormat.c_str(),iPK_MediaType,sMediaURL.c_str(),sURL.c_str(),iEK_Disc,sCMD_Result,NULL);};
	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #817 - Get Default Ripping Info */
	/** Get default ripping info: default filename, id and name of the storage device with most free space. */
		/** @param #13 Filename */
			/** Default ripping name. */
		/** @param #219 Path */
			/** Base path for ripping. */
		/** @param #233 DriveID */
			/** The id of the storage device with most free space. */
		/** @param #235 Storage Device Name */
			/** The name of the storage device with most free space. */

	virtual void CMD_Get_Default_Ripping_Info(string *sFilename,string *sPath,int *iDriveID,string *sStorage_Device_Name) { string sCMD_Result; CMD_Get_Default_Ripping_Info(sFilename,sPath,iDriveID,sStorage_Device_Name,sCMD_Result,NULL);};
	virtual void CMD_Get_Default_Ripping_Info(string *sFilename,string *sPath,int *iDriveID,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	private:
		Disk_Drive_Functions * m_pDisk_Drive_Functions;
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
