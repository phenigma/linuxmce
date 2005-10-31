#ifndef RegenMonitor_h
#define RegenMonitor_h

class RegenMonitor
{
	class OrbiterGenerator *m_pOrbiterGenerator;
public:

	RegenMonitor(OrbiterGenerator *pOrbiterGenerator) { m_pOrbiterGenerator=pOrbiterGenerator; }

	bool CachedVersionOK(string sString);

	string QueryAsModString(string sSQL);

	// This returns a string that shows the current status of this array so it can be 
	// compared to see if the array has changed.  For example, if the array is lighting scenarios (1), and 
	// the current room is 3 and the last psc_mod (timestamp) for rooms is 20050320 and for commandgroup_d is 20050321
	// the string might be: 1,3,20050320,20050321
	// If it returns a single *, that means the string cannot be determined for some reason and the screen
	// be regenerated
	string GetModInfo_Array(int PK_Array);

	// Some specific helper functions for the various types, called by GetModInfo
	string GetModInfo_RoomScenario(int PK_Array,int PK_Room);
	string GetModInfo_EntAreaScenario(int PK_Array,int PK_EntArea);
	string GetModInfo_DeviceCategory(int PK_DeviceCategory);
	string GetModInfo_Users();
	string GetModInfo_Locations();
	string GetModInfo_FloorplanArray();
	string GetModInfo_Floorplan(int PK_FloorplanType);
};

#endif
