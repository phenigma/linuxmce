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
		class DeviceData_Router *m_pDeviceData_Router;          /**< The device in the router */
		map<int,int> m_mapMediaType_DesignObj;              /** A map of all the remotes for the various screens */
		map<int,class EntertainArea *> m_mapEntertainArea;
		int m_iLastPlaybackSpeed;
		class OH_Orbiter *m_pOH_Orbiter_OSD;    	   /** Which orbiter is the on-sceren display */
		map<int, Row_MediaType *> m_mapMediaType;
	};

	typedef list<MediaDevice *> ListMediaDevice;
}

#endif
