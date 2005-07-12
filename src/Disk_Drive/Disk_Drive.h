//<-dceag-d-b->
#ifndef Disk_Drive_h
#define Disk_Drive_h

//	DCE Implemenation for #11 Disk Drive

#include "Gen_Devices/Disk_DriveBase.h"
//<-dceag-d-e->

#include <pthread.h>

typedef enum {
    MEDIA_NOT_LOADED        = 0x00,
    MEDIA_CD_ROM            = 0x08, /* also CD-ROM and Finalized CD-R */
    MEDIA_CD_R              = 0x09,
    MEDIA_CD_RW             = 0x0A,
    MEDIA_DVD_ROM           = 0x10, /* also Finalized DVD-R and DVD+R */
    MEDIA_DVD_MINUS_R       = 0x11,
    MEDIA_DVD_MINUS_RW      = 0x13,
    MEDIA_DVD_MINUS_RW_RO   = 0x14, /* _RO = restricted overwrite */
    MEDIA_DVD_PLUS_RW       = 0x1A,
    MEDIA_DVD_PLUS_R        = 0x1B,
    MEDIA_TYPE_UNKNOWN      = 0x98,
    MEDIA_TYPE_ERROR        = 0x99
} discMediaType;

// This should be in sync with the typedef in the Media_Plugin.cpp
typedef enum {
	RIP_RESULT_BEGIN_ENUM		 = 0x01,
	RIP_RESULT_ALREADY_RIPPING	 = 0x02,
	RIP_RESULT_NO_DISC			 = 0x03,
	RIP_RESULT_INVALID_DISC_TYPE = 0x04,
	RIP_RESULT_SUCCESS			 = 0x05,
	RIP_RESULT_FAILURE			 = 0x06,
	RIP_RESULT_END_ENUM			 = 0x07
} rippingResult;

//<-dceag-decl-b->
namespace DCE
{
	class Disk_Drive : public Disk_Drive_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Disk_Drive();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
    // Private member variables
    bool m_monitorEnabled;
    bool m_mediaInserted;
    int  m_mediaDiskStatus;

    bool m_isRipping;

    int  m_serverPid;
    int  m_serverPort;
    int  m_discid;		// A unique ID to indicate the insertion of this disc, will be set to the time(NULL) at insertion
    int  m_what_is_ripping;

	string m_sDrive;	// The drive, normally DATA_Get_Drive()

    char *m_args[100];

    pthread_mutex_t sqlLock;

//     Connection m_mysql;

    // Private methods
    int cdrom_lock( int lock );
    int cdrom_checkdrive( const char *filename, int *flag );

	string getTracks (string mrl);  // We use cddb for nothing other than determining how many tracks there are

    bool internal_reset_drive(bool bFireEvent);
    bool internal_monitor_step(bool bFireEvent);

    int cdrom_has_dir (int fd, const char *directory);
	bool mountDVD(string fileName, string &strMediaUrl);

public:
    // Public member variables

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
	void EVENT_Ripping_Completed(int iResult,string sName);

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

	virtual void CMD_Reset_Disk_Drive() { string sCMD_Result; CMD_Reset_Disk_Drive(sCMD_Result,NULL);};
	virtual void CMD_Reset_Disk_Drive(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */

	virtual void CMD_Eject_Disk() { string sCMD_Result; CMD_Eject_Disk(sCMD_Result,NULL);};
	virtual void CMD_Eject_Disk(string &sCMD_Result,Message *pMessage);


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
		/** @param #50 Name */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */

	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string sTracks) { string sCMD_Result; CMD_Rip_Disk(iPK_Users,sName.c_str(),sTracks.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string sTracks,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
