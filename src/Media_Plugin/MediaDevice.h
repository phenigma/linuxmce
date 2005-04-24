#ifndef MediaDevice_h
#define MediaDevice_h

class Row_Device;

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
		list<class EntertainArea *> m_listEntertainArea;
		int m_iLastPlaybackSpeed;

	};

	typedef list<MediaDevice *> ListMediaDevice;
}

#endif
