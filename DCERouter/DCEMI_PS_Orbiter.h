#ifndef DCEMI_PS_Orbiter_h
#define DCEMI_PS_Orbiter_h

#include "PlugIn.h"
#include "pluto_main/Define_PlugIn.h"

class DCEMI_PS_Orbiter : public PlugIn
{
public:
	class DCEMI_PS_MobileOrbiter *m_pDCEMI_PS_MobileOrbiter;
	map<int,class PlutoOrbiter *> m_mapPlutoOrbiter;
	class PlutoOrbiter *m_mapPlutoOrbiter_Find(int PK_Device)
	{
		map<int,PlutoOrbiter *>::iterator it = m_mapPlutoOrbiter.find(PK_Device);
		return it==m_mapPlutoOrbiter.end() ? NULL : (*it).second;
	}

	// Required overrides
	DCEMI_PS_Orbiter(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Orbiter_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Orbiter interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool UpdateDesignObjImage(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool NavGoto(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool SimulateKeypress(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool VerifyPin(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool BindToDevice(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool BindToHouse(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool SetUserMode(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool PinRequired(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool CurrentController(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
