#ifndef ALERTNOTIFICATION_H
#define ALERTNOTIFICATION_H

#include <set>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"

#include "DeviceCreator.h"
#include "Gen_Devices/DCEAllDeviceEvents.h"
#include "DeviceData_Impl.h"
#include "Gen_Devices/EventManagerCommand.h"
#include "../pluto_main/Table_Command.h"
#include "../pluto_main/Table_Event.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_CommandParameter.h"
#include "../pluto_main/Table_Variable.h"

class PhoneNotification
{
public:
	string PhoneNumber;
	bool bMonitorMode,bSecurity,bFire,bIntercom;
};

class Neighbor
{
public:
	string Name;
	string Number;
};

class AlertNotification : public AlarmEvent
{
	class PlutoServerCore *m_pCore;
	class DeviceData_Router *m_pDevice_WAPServer;
	int m_MobileOrbiterDelay,m_OtherPhoneDelay;
	enum enumCallOrder { Orbiters_Then_Other, Other_Then_Orbiters, Both_At_Same_Time } m_enumCallOrder;
	vector<PhoneNotification *> m_vectMobileOrbiters,m_vectOtherPhones;
	vector<Neighbor *> m_vectNeighbors;

public:
	AlertNotification(class PlutoServerCore *pCore,class DeviceData_Router *pDevice_WAPServer);
	~AlertNotification();
	void Notify(int TypeOfBreach,class DeviceData_Router *pDevice);
	void AlarmCallback(int id, void* param);
};

#endif

