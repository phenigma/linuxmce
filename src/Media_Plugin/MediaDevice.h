#ifndef MediaDevice_h
#define MediaDevice_h

class Row_Device;
class Row_MediaType;
class Row_MediaProvider;

#define DONT_RESEND_POWER_WITHIN_X_SECONDS		3

namespace DCE
{
	/** @brief This adds media specific information for a device, extending the DeviceData_Router */

	class MediaDevice
	{
	public:

		/** @brief constructor */
		MediaDevice(class Router *pRouter,Row_Device *pRow_Device);   /** This will store extra information about itself */
		bool m_bDontSendOffIfOSD_ON;  // This allows that only the OSD can turn it off
		bool m_bCaptureCardActive; // True if this is an external source and m_pDevice_CaptureCard is not null, and the video/audio path are now using the capture card
		int m_iDelayForCaptureCard; // If m_pDevice_CaptureCard is not null and the stream should start by showing the content from the capture card (ie m_bCaptureCardActive=true), and switch after this many seconds to a direct connect
		class DeviceData_Router *m_pDeviceData_Router,          /**< The device in the router */
			*m_pDevice_Audio, /**< If this is inside a media director, and there is no destination device in the audio path, this will point app server to adjust the volume in the PC */
			*m_pDevice_Video, /**< The destination video device, ie the TV */
			*m_pDevice_Media_ID, /**< The device responsible for identifying media inserted if this is a disk drive */
			*m_pDevice_CaptureCard; /**< If this is an external device, like a VCR, connected to a capture card port, this is the port */
		Row_MediaProvider *m_pRow_MediaProvider;

		map<int,int> m_mapMediaType_DesignObj;              /** A map of all the remotes for the various screens */
		map<int,class EntertainArea *> m_mapEntertainArea;
		int m_iLastPlaybackSpeed,m_iPK_MediaProvider;
		class OH_Orbiter *m_pOH_Orbiter_OSD;    	   /** Which orbiter is the on-sceren display */
		map<int, Row_MediaType *> m_mapMediaType;
		class CommandGroup *m_pCommandGroup; // A command group used to turn this device on
		class Router *m_pRouter;

		// These will only be used if this is a disk drive.  If so, we'll keep track of the orbiter
		// that reset the drive so that when the drive later fires a media inserted we know which 
		// orbiter originated the request
		class OH_Orbiter *m_pOH_Orbiter_Reset;
		time_t m_tReset;

		// Keep track of the last time the media plugin turned this device on or off, so Orbiter Plugin doesn't
		// repeat the same command.  This can happen when media plugin starts some media which causes the orbiter
		// to simultaneously come on
		time_t m_tLastPowerCommand;
		int m_dwPK_Command_LastPower;
		int m_iLastVolume;  // The last known volume for this device, -1 means unknown
		bool m_bMute;  // True if the device was last muted

		int FindUltimateDestinationViaPipe(class Pipe *pPipe,int PK_Pipe);

		bool IsInEntertainArea(EntertainArea *pEntertainArea)
		{
			for(map<int,class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
			{
				if( pEntertainArea == it->second )
					return true;
			}
			return false;
		}

	};

	typedef list<MediaDevice *> ListMediaDevice;
}

#endif
