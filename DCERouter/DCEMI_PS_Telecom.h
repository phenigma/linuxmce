#ifndef DCEMI_PS_Telecom_h
#define DCEMI_PS_Telecom_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

class DCEMI_PS_Telecom : public PlugIn
{
public:
	// Required overrides
	DCEMI_PS_Telecom(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Telecom_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Telecom interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool NewNumMessages(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
