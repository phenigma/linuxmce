#ifndef EntertainArea_h
#define EntertainArea_h

#include "MediaDevice.h"
#include "MediaHandlerInfo.h"
#include "BoundRemote.h"

namespace DCE
{

	/** @brief An entertainment area is a cluster of devices.  It's analogous to a Room, except you can have multiple entertainment areas in a room, like
	* a 'his' and 'hers' tv in the bedroom.
	*/

	class EntertainArea
	{
	public:

		/** @brief constructor*/

		EntertainArea(int iPK_EntertainArea,bool bOnly1Stream,string sDescription)
		{ m_iPK_EntertainArea=iPK_EntertainArea; m_bOnly1Stream=bOnly1Stream; m_pMediaStream=NULL; m_sDescription=sDescription; }

		int m_iPK_EntertainArea;
		string m_sDescription;

		class MediaStream  *m_pMediaStream;   /** The current media streams in this entertainment area */

		map<int, class MediaDevice *> m_mapMediaDevice;  /** All the media devices in the area */
		MediaDevice *m_mapMediaDevice_Find(int PK_Device) { map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(PK_Device); return it==m_mapMediaDevice.end() ? NULL : (*it).second; }
		map<int, ListMediaDevice *> m_mapMediaDeviceByTemplate;  /** All the media devices in the area by device template */
		ListMediaDevice *m_mapMediaDeviceByTemplate_Find(int PK_DeviceTemplate) { map<int,ListMediaDevice *>::iterator it = m_mapMediaDeviceByTemplate.find(PK_DeviceTemplate); return it==m_mapMediaDeviceByTemplate.end() ? NULL : (*it).second; }

		// These are the Orbiters that are currently controling this entertainment area.  When an
		// orbiter goes to a remote control for an entertainment area it becomes "bound".
		MapBoundRemote m_mapBoundRemote;

		BoundRemote *m_mapBoundRemote_Find(int iPK_Orbiter)
		{
			map<int,class BoundRemote *>::iterator it = m_mapBoundRemote.find(iPK_Orbiter);
			return it==m_mapBoundRemote.end() ? NULL : (*it).second;
		}

		void m_mapBoundRemote_Remove(int iPK_Orbiter)
		{
			MapBoundRemote::iterator it = m_mapBoundRemote.find(iPK_Orbiter);
			if( it!=m_mapBoundRemote.end() ) m_mapBoundRemote.erase(it);
		}

		bool m_bOnly1Stream;
		/** If true, stop any other media streams in this area when a new one starts.  There can be only 1.  This is the default behavior */

		map<int,List_MediaHandlerInfo *> m_mapMediaHandlerInfo_MediaType; /** The key is the media type */

		List_MediaHandlerInfo *m_mapMediaHandlerInfo_MediaType_Find(int PK_MediaType)
		{
			map<int,List_MediaHandlerInfo *>::iterator it = m_mapMediaHandlerInfo_MediaType.find(PK_MediaType);
			return it==m_mapMediaHandlerInfo_MediaType.end() ? NULL : (*it).second;
		}

		map<string,List_MediaHandlerInfo *> m_mapMediaHandlerInfo_Extension; // The key is a file extension
		List_MediaHandlerInfo *m_mapMediaHandlerInfo_Extension_Find(string Extension) {
			map<string,List_MediaHandlerInfo *>::iterator it = m_mapMediaHandlerInfo_Extension.find(Extension);
			return it==m_mapMediaHandlerInfo_Extension.end() ? NULL : (*it).second;
		}
	};

	typedef map<int,class EntertainArea *> MapEntertainArea;
}

#endif
