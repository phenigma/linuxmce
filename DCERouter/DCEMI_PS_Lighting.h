#ifndef DCEMI_PS_Lighting_h
#define DCEMI_PS_Lighting_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

class DCEMI_PS_Lighting : public PlugIn
{
public:
	// Required overrides
	DCEMI_PS_Lighting(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Lighting_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Lighting interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Orbiters will just send a relative dim up/down (ie +10%-10%).  Not all lighting devices understand relative dimming, so this keeps track
	// of a light's dim level, and converts relative dims to absolute
	bool Dimming_RelativeOrAbsolute(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);

	// Orbiters may send an 'Adjust Lights' command, which is designed to toggle lights, turn all on or all off
	// This keeps track of each light's last state, and translates these into on/off and absolute dim
	bool AdjustLights(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
