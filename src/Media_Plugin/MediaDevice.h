#ifndef MediaDevice_h
#define MediaDevice_h

class Row_Device;
class Row_MediaType;

namespace DCE
{
	/** @brief This adds media specific information for a device, extending the DeviceData_Router */

	class MediaDevice
	{
	public:

		/** @brief constructor */
		MediaDevice(class Router *pRouter,Row_Device *pRow_Device);   /** This will store extra information about itself */
		bool m_bDontSendOffIfOSD_ON;  // This allows that only the OSD can turn it off
		class DeviceData_Router *m_pDeviceData_Router;          /**< The device in the router */
		map<int,int> m_mapMediaType_DesignObj;              /** A map of all the remotes for the various screens */
		map<int,class EntertainArea *> m_mapEntertainArea;
		int m_iLastPlaybackSpeed;
		class OH_Orbiter *m_pOH_Orbiter_OSD;    	   /** Which orbiter is the on-sceren display */
		map<int, Row_MediaType *> m_mapMediaType;
		class CommandGroup *m_pCommandGroup; // A command group used to turn this device on

		// These will only be used if this is a disk drive.  If so, we'll keep track of the orbiter
		// that reset the drive so that when the drive later fires a media inserted we know which 
		// orbiter originated the request
		class OH_Orbiter *m_pOH_Orbiter_Reset;
		time_t m_tReset;
	};

	typedef list<MediaDevice *> ListMediaDevice;
}

#endif
