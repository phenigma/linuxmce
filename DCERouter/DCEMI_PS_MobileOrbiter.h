#ifndef DCEMI_PS_MobileOrbiter_h
#define DCEMI_PS_MobileOrbiter_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

typedef enum {
		dgrt_none,dgrt_CameraQuadView,dgrt_CameraSingleView,
		dgrt_SecurityPanel,dgrt_UserStatus,dgrt_MakeACall,dgrt_NoCommands
} enumRecognizedCommandGroups;


class DCEMI_PS_MobileOrbiter : public PlugIn
{
public:
	class DCEMI_PS_Orbiter *m_pDCEMI_PS_Orbiter;

	map<int,class MobileOrbiter *> m_mapMobileOrbiter;
	MobileOrbiter *m_mapMobileOrbiter_Find(int PK_Device)
	{
		map<int,MobileOrbiter *>::iterator it = m_mapMobileOrbiter.find(PK_Device);
		return it==m_mapMobileOrbiter.end() ? NULL : (*it).second;
	}

	map<string,class MobileOrbiter *> m_mapMobileOrbiter_Mac;
	MobileOrbiter *m_mapMobileOrbiter_Mac_Find(string MacAddress)
	{
		map<string,MobileOrbiter *>::iterator it = m_mapMobileOrbiter_Mac.find(MacAddress);
		return it==m_mapMobileOrbiter_Mac.end() ? NULL : (*it).second;
	}

	// Required overrides
	DCEMI_PS_MobileOrbiter(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Mobile_Orbiter_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Mobile Orbiter interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool ToggleFollowMe(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool LockOntoLocation(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool LinkMediaRemote(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif

