#ifndef DCEMI_PS_Floorplan_h
#define DCEMI_PS_Floorplan_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

class DCEMI_PS_Floorplan : public PlugIn
{
public:
	map<int,class IRInformation *> m_mapIRInformation;
	IRInformation *m_mapIRInformation_Find(int PK_Device)
	{
		map<int,IRInformation *>::iterator it = m_mapIRInformation.find(PK_Device);
		return it==m_mapIRInformation.end() ? NULL : (*it).second;
	}
	class DCEMI_PS_Orbiter *m_pDCEMI_PS_Orbiter;
	class DCEMI_PS_DataGrid *m_pDCEMI_PS_DataGrid;
	class DCEMI_PS_Media *m_pDCEMI_PS_Media;

	// Required overrides
	DCEMI_PS_Floorplan(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Floorplan_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Floorplan interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool FloorplanStatus(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool FloorplanLayout(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
