#include "PlutoUtils/CommonIncludes.h"	
#include "DCEMI_PS_Infrared.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Event.h"

// temp
#define COMMAND_LEARN_IR_CONST 999
#define COMMAND_LEARN_IR_CANCEL_CONST 999
#define EVENT_LEARNED_CODE_CONST 999
#define COMMAND_NAV_HIDE_OBJECT_CONST 999
#define DESIGNOBJ_MNUDEVICECONTROL_CONST 999
#define DESIGNOBJ_GRPLEARNCANCELCODE_CONST 999
#define DESIGNOBJ_GRPLEARNCANCELCODE_CONST 999
#define DESIGNOBJ_GRPLEARNCANCELCODE_CONST 999
#define DESIGNOBJ_GRPLEARNCANCELCODE_CONST 999
#define DESIGNOBJ_GRPLEARNCANCELCODE_CONST 999

bool DCEMI_PS_Infrared::Register()
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Infrared::LearnIR))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_LEARN_IR_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Infrared::LearnIRCancel))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_LEARN_IR_CANCEL_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Infrared::LearnedCode))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_LEARNED_CODE_CONST);

	return true;
}
bool DCEMI_PS_Infrared::LearnIR(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;
	m_pRouter->DispatchMessage( new Message(0,pDeviceFrom->m_iPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_NAV_HIDE_OBJECT_CONST,
		2,COMMANDPARAMETER_PK_DesignObj_CONST,(StringUtils::itos(DESIGNOBJ_MNUDEVICECONTROL_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_GRPLEARNCANCELCODE_CONST)).c_str(),
		COMMANDPARAMETER_OnOff_CONST,"0"));
	return true;
}


bool DCEMI_PS_Infrared::LearnIRCancel(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;
	m_pRouter->DispatchMessage( new Message(0,pDeviceFrom->m_iPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_NAV_HIDE_OBJECT_CONST,
		1,COMMANDPARAMETER_PK_DesignObj_CONST,(StringUtils::itos(DESIGNOBJ_MNUDEVICECONTROL_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_GRPLEARNCANCELCODE_CONST)).c_str()));
	return true;
}

bool DCEMI_PS_Infrared::LearnedCode(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	int PK_PlutoOrbiter = atoi(pMessage->m_mapParameters[C_EVENTPARAMETER_PK_CONTROLLER_CONST].c_str());
	string Pronto = pMessage->m_mapParameters[C_EVENTPARAMETER_TEXT_CONST];
	string PlutoIR = pMessage->m_mapParameters[C_EVENTPARAMETER_TOKEN_CONST];
	int PK_Command = atoi(pMessage->m_mapParameters[C_EVENTPARAMETER_ID_CONST].c_str());

	DeviceData_Router *pDevice = m_pPlutoEvents->m_mapDevice_Find(pMessage->m_dwPK_Device_From);
	if( !pDevice || !PK_Command || (Pronto=="" && PlutoIR=="") )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to learn code");
		return true;
	}
	string URL = "http://plutohome.com/utils/updateIR.php?fkid_installation=" + StringUtils::itos(m_pRouter->m_PK_Installation) + 
		"&fkid_masterdevicelist=" + StringUtils::itos(pDevice->m_iPK_DeviceTemplate) +
		"&fkid_action=" + StringUtils::itos(PK_Command);

	if( Pronto!="" )
		URL += "&prontocode=" + StringUtils::URLEncode(Pronto);
	if( PlutoIR!="" )
		URL += "&plutoir=" + StringUtils::URLEncode(PlutoIR);

	g_pPlutoLogger->Write(LV_STATUS,"Sending to pluto: %s",URL.c_str());
#ifndef WIN32
	char buf[20];
	FILE* f = popen(( string("wget -q -O - '") + URL + "'").c_str(), "r");
	if(f == NULL)
	{
#endif
		g_pPlutoLogger->Write(LV_CRITICAL, "failed to open web site URL (won't work in Windows)");
		return true;
#ifndef WIN32
	}
	else
	{
        	PlutoOrbiter *pController = m_mapPlutoOrbiter_Find(pMessage->m_dwPK_Device_From);
	        if( !pController )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find controller for floorplan layout: %d",pMessage->m_dwPK_Device_From);
	                return true;
	        }
		
		
		fgets(buf, sizeof(buf), f);
		fclose(f);
		size_t len = strlen(buf);
		if(buf[len-1] == '\n');
			buf[len-1] = 0;
		g_pPlutoLogger->Write(LV_STATUS, "Response was '%s'",buf);
		string Description = "Code learned OK";
		if( len<2 || buf[0]!='O' || buf[1]!='K' )
		{
			Description = string("Error: ") + buf;

			m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,pController->m_pDeviceData_Router->m_iPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_TEXT_CONST,
				3,COMMANDPARAMETER_PK_DesignObj_CONST,(StringUtils::itos(DESIGNOBJ_MNUPOPUPMESSAGE_CONST) + ".0.0").c_str(),COMMANDPARAMETER_ID_CONST,StringUtils::itos(TEXT_STATUS_CONST).c_str(),
				COMMANDPARAMETER_Text_CONST,Description.c_str()));

			m_pRouter->DispatchMessage(new Message(DEVICEID_EVENTMANAGER,pController->m_pDeviceData_Router->m_iPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
				COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,(StringUtils::itos(DESIGNOBJ_MNUPOPUPMESSAGE_CONST)+".0.0").c_str()));
		}
		else
		{
			m_pRouter->DispatchMessage( new Message(0,pController->m_pDeviceData_Router->m_iPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_NAV_HIDE_OBJECT_CONST,
				1,COMMANDPARAMETER_PK_DesignObj_CONST,
				(StringUtils::itos(DESIGNOBJ_MNUDEVICECONTROL_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_GRPLEARNCANCELCODE_CONST)).c_str()));
		}
	}
#endif
	*/
	return true;
}

