#ifndef OCROOM_H
#define OCROOM_H

#include "PlutoEvents.h" // Needed for DeviceList

class OCRoom
{
public:
	int m_PK_Room;
	string m_Description;
	int m_PK_C_RoomMode;
	bool m_bLightsLastOn,m_bClimateLastOn;
	class CommandGroup *m_pagLightsOff,*m_pagLightsOn,*m_pagClimateOff,*m_pagClimateOn;
	int PK_CommandGroup_LightsOff,PK_CommandGroup_LightsOn,PK_CommandGroup_ClimateOff,PK_CommandGroup_ClimateOn;
	int m_iTemperature;
	vector<OCController *> m_vectController;
	vector<class CommandGroup *> m_vectCommandGroups;
	DeviceList m_listDevices;

	OCRoom() : m_pagLightsOff(NULL),m_pagLightsOn(NULL),m_pagClimateOff(NULL),m_pagClimateOn(NULL),
		m_bLightsLastOn(false),m_bClimateLastOn(false), m_iTemperature(0) {}
};

#endif

