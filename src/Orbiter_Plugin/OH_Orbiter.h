#ifndef OH_Orbiter_h
#define OH_Orbiter_h

#include "OH_User.h"
#include "Orbiter/Floorplan.h"

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

		OH_User *m_pOH_User;  // The current user
		class EntertainArea *m_pEntertainArea,*m_pEntertainArea_Locked;  // The current entertain area
		int m_dwPK_Room,m_dwPK_Room_Locked;  // The current room

		int m_iLastSignalStrength;
		int m_iFailedToConnectCount;
		bool m_bRegistered,m_bDisplayOn;
        string m_sUpdateVMCFile;
		string m_sVersion;

		time_t m_tRegenTime,m_tSendAppTime;

		int PK_Users_get() { return m_pOH_User ? m_pOH_User->m_iPK_Users : 0; }  // Lots of places just want a 0 when there is no user

		OH_Orbiter(class DeviceData_Router *pDeviceData_Router)
		{
			m_pDeviceData_Router = pDeviceData_Router;
			m_dwPK_Room = m_dwPK_Room_Locked = 0;
			m_pEntertainArea = m_pEntertainArea_Locked = NULL;
			m_iLastSignalStrength = 0;
			m_pDevice_CurrentDetected = NULL;
			m_iFailedToConnectCount = 0;
			m_bDisplayOn=m_bRegistered=false;
			m_tSendAppTime=m_tRegenTime=0;
			m_pOH_User=NULL;
		}

        bool NeedVMC();
        void NeedVMC(bool bTrue);
        bool NeedApp();
        void NeedApp(bool bTrue);

    protected:
        void SetState(bool bNeedVmc, bool bNeedApp);
	};
}

#endif
