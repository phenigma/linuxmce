#ifndef OH_Orbiter_h
#define OH_Orbiter_h


namespace DCE
{
	// Store an orbiter list within the handler
	class OH_Orbiter
	{
	public:
		map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;
		FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page) { map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page); return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second; }

		class DeviceData_Router *m_pDeviceData_Router;
		class DeviceData_Router *m_pDevice_CurrentDetected;

		int m_iPK_Users;  // The current user
		class EntertainArea *m_pEntertainArea,*m_pEntertainArea_Locked;  // The current entertain area
		int m_dwPK_Room,m_dwPK_Room_Locked;  // The current room

		int m_iLastSignalStrength;
		int m_iFailedToConnectCount;
		bool m_bRegistered;

		time_t m_tRegenTime;

		bool m_bFollowMe_Lighting,m_bFollowMe_Media,m_bFollowMe_Climate,m_bFollowMe_Telecom,m_bFollowMe_Security;

		OH_Orbiter(class DeviceData_Router *pDeviceData_Router)
		{
			m_pDeviceData_Router = pDeviceData_Router;
			m_iPK_Users = m_dwPK_Room = m_dwPK_Room_Locked = 0;
			m_pEntertainArea = m_pEntertainArea_Locked = NULL;
			m_iLastSignalStrength = 0;
			m_pDevice_CurrentDetected = NULL;
			m_iFailedToConnectCount = 0;
			m_bRegistered=false;
			m_bFollowMe_Lighting=m_bFollowMe_Media=m_bFollowMe_Climate=m_bFollowMe_Telecom=m_bFollowMe_Security=false;
			m_tRegenTime=0;
		}
	};
}

#endif
