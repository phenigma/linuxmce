#ifndef DCEMI_PS_Infrared_h
#define DCEMI_PS_Infrared_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"

class IRInformation
{
public:
	bool m_bTogglePower,m_bDontResendOn;
	list<int> m_listToggleInputs;
	list<int> m_listToggleDSPModes;
	bool m_bIsOn;
	int m_iPK_Input,m_iPK_Command_Input;
	list<int>::iterator m_itCurrentInput,m_itCurrentDSPMode;

	IRInformation() 
	{
		m_iPK_Input=m_iPK_Command_Input=0;
		m_bIsOn=m_bDontResendOn=false;
		m_itCurrentInput=m_listToggleInputs.end();
		m_itCurrentDSPMode=m_listToggleDSPModes.end();
	}
	~IRInformation()
	{
	}
};

class DCEMI_PS_Infrared : public PlugIn
{
public:

	// Required overrides
	DCEMI_PS_Infrared(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Infrared_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Infrared interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool LearnIR(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool LearnIRCancel(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool LearnedCode(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif

