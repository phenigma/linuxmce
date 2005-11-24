#ifndef DISK_DRIVE_FUNCTIONS
#define DISK_DRIVE_FUNCTIONS

#include "Command_Impl.h"
#include "pluto_main/Define_MediaType.h"

#ifndef WIN32
extern "C"
{
    #include <linux/cdrom.h>
}
#endif

#include <string>

using namespace std;
using namespace DCE;

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
	RIP_RESULT_STILLGOING		 = 0x07,
	RIP_RESULT_END_ENUM			 = 0x08
} rippingResult;

enum DiscTypes {
    DISCTYPE_NONE       = -1,
    DISCTYPE_CD_AUDIO   =  0,
    DISCTYPE_CD         =  1,
    DISCTYPE_DVD_VIDEO  =  2,
    DISCTYPE_DVD        =  3,
    DISCTYPE_BLANK      =  4,
    DISCTYPE_DATA       =  5,
    DISCTYPE_CD_MIXED   =  6,
    DISCTYPE_CD_VCD     =  7,
    DISCTYPE_CD_SVCD    =  8,
    DISCTYPE_DVD_AUDIO  =  9
};

class Disk_Drive_Functions
{
	private:
		string m_sDrive;
		Command_Impl * m_pCommand_Impl;
	    pluto_pthread_mutex_t m_DiskMutex;

		void EVENT_Media_Inserted(int iFK_MediaType,string sMRL,string sID,string sName, int iEventDevice = DEVICEID_EVENTMANAGER);
		void EVENT_Ripping_Progress(string sText,int iResult,string sValue,string sName,int iEK_Disc, int iEventDevice = DEVICEID_EVENTMANAGER);
		
	public:
		Disk_Drive_Functions(Command_Impl * pCommand_Impl, const string & sDrive);
		bool internal_monitor_step(bool bFireEvent);
		bool internal_reset_drive(bool bFireEvent);
		int cdrom_has_dir(int fd, const char *directory);
		int cdrom_lock(int lock);
		int cdrom_checkdrive(const char * filename, int * flag, bool bFireEvent);
		bool mountDVD(string fileName, string & strMediaUrl);
		void CMD_Rip_Disk(int iPK_Users, string sFormat, string sName, string sTracks, int iEK_Disc, int iDriveNumber, string &sCMD_Result, Message *pMessage, int iEventDevice = DEVICEID_EVENTMANAGER);
		string getTracks(string mrl); // We use cddb for nothing other than determining how many tracks there are
		void DisplayMessageOnOrbVFD(string sMessage);

		// TODO: write accessors for these
		DeviceData_Base *m_pDevice_AppServer;
	    int m_mediaDiskStatus;
	    int m_discid;		// A unique ID to indicate the insertion of this disc, will be set to the time(NULL) at insertion
		bool m_isRipping;
	    bool m_mediaInserted;
		int m_what_is_ripping;
		bool m_bTrayOpen;
};

#endif
