#include "DCEMI_PS_Security.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"

//temp
#define COMMAND_SECURITY_RESET_CONST 999


bool DCEMI_PS_Security::Register()
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Security::SecurityReset))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_SECURITY_RESET_CONST);

	return true;
}

bool DCEMI_PS_Security::SecurityReset(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
g_pPlutoLogger->Write(LV_CRITICAL,"Got security reset command");
	for(int i=(int) m_pPlutoEvents->m_vectSecurityEvent.size()-1;i>=0;--i)
	{
		SecurityEvent *pSecurityEvent = m_pPlutoEvents->m_vectSecurityEvent[i];
		delete pSecurityEvent;
	}
	m_pPlutoEvents->m_vectSecurityEvent.clear();
	int PK_Device_From = pMessage->m_DeviceIDFrom;
	if( !PK_Device_From )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Security reset from no device");
		PK_Device_From = m_pPlutoEvents->m_TTSDevice; // Just pick something otherwise the event manager won't process it
	}
g_pPlutoLogger->Write(LV_CRITICAL,"Forwarding action security reset to device: %d / %d / %d",PK_Device_From,m_pPlutoEvents->m_TelecomServer,m_pPlutoEvents->m_TTSDevice);
	m_pRouter->DispatchMessage(new Message(PK_Device_From,0,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_RESET_SECURITY_CONST,0));
	*/
	return true;
}
