//<-dceag-d-b->
#ifndef Powerfile_C200_h
#define Powerfile_C200_h

//	DCE Implemenation for #1737 Powerfile C200

#include "Gen_Devices/Powerfile_C200Base.h"
//<-dceag-d-e->
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"

typedef enum { RS_NOT_PROCESSED = 0, RS_RIPPING, RS_FAIL, RS_SUCCESS, RS_IDENTIFYING } enum_RipStatus;
typedef enum { PF_IDLE = 0, PF_IDENTIFYING, PF_RIPPING } enum_PFState;

struct state_RipStatus
{
	int slot;
	enum_RipStatus status;
};

//<-dceag-decl-b->
namespace DCE
{
	class Powerfile_C200 : public Powerfile_C200_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Powerfile_C200();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

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
	/** Starts ripping a DVD. */

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

	virtual void CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number) { string sCMD_Result; CMD_Rip_Disk(iPK_Users,sFormat.c_str(),sName.c_str(),sTracks.c_str(),iEK_Disc,iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc to */

	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number) { string sCMD_Result; CMD_Load_from_Slot_into_Drive(iSlot_Number,iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc from */

	virtual void CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number) { string sCMD_Result; CMD_Unload_from_Drive_into_Slot(iSlot_Number,iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #21 Force */
			/** If not empty, force=true. Else false. */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

	virtual void CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status) { string sCMD_Result; CMD_Get_Jukebox_Status(sForce.c_str(),sJukebox_Status,sCMD_Result,NULL);};
	virtual void CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #720 - Bulk Rip */
	/** Mass ripping */
		/** @param #157 Disks */
			/** Comma delimited list of slot number. For all, use "*". */

	virtual void CMD_Bulk_Rip(string sDisks) { string sCMD_Result; CMD_Bulk_Rip(sDisks.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bulk_Rip(string sDisks,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #738 - Play Disk */
	/** Play the disk from specified slot. Automatically do the behind logic. Eject a disk from drive if necessary. */
		/** @param #151 Slot Number */
			/** Slot number */

	virtual void CMD_Play_Disk(int iSlot_Number) { string sCMD_Result; CMD_Play_Disk(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Play_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #739 - Get Bulk Ripping Status */
	/** Get status/progress of bulk ripping operation */
		/** @param #158 Bulk rip status */
			/** Returns:
S1-F,S2-S,S3-R,S4-N
F = failed
S = succeeded
R = ripping
N = not processed yet
empty string when idle or reset; last status when all disks were ripped
only slots that were scheduled for ripping will appear in the string */

	virtual void CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status) { string sCMD_Result; CMD_Get_Bulk_Ripping_Status(sBulk_rip_status,sCMD_Result,NULL);};
	virtual void CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #740 - Mass identify media */
	/** Scan all loaded discs and identify each one */

	virtual void CMD_Mass_identify_media() { string sCMD_Result; CMD_Mass_identify_media(sCMD_Result,NULL);};
	virtual void CMD_Mass_identify_media(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
		private:
			// corresponding devices
			vector<string> m_vectDrive;
			string m_sChanger;
			vector<Disk_Drive_Functions *> m_vectDDF;

			Disk_Drive_Functions * GetDDF(int iDrive_Number);
			bool Get_Jukebox_Status(string * sJukebox_Status, bool bForce = false);
			
			vector<int> m_vectDriveStatus; // slot of provenience (0 = empty)
			vector<bool> m_vectSlotStatus; // occupied or not
			bool m_bStatusCached;
			vector<state_RipStatus> m_vectRipStatus;
			enum_PFState m_State;

		public:
			bool MediaIdentified(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo);
			bool RippingProgress(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo);
			pluto_pthread_mutex_t m_MediaMutex, m_ChangerMutex; // TODO: make ChangerMutex recursive and put in all places where needed
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
