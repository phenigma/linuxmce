#ifndef DCEMI_PS_Security_h
#define DCEMI_PS_Security_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

class DCEMI_PS_Security : public PlugIn
{
public:
	// Required overrides
	DCEMI_PS_Security(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Security_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Security interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an action to reset all security sensors
	bool SecurityReset(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
