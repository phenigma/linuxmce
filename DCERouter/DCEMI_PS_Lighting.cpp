#include "PlutoUtils/CommonIncludes.h"	
#include "DCEMI_PS_Lighting.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"

// temp
#define C_COMMANDPARAMETER_REL_LEVEL_CONST 9999
#define C_COMMANDPARAMETER_ABS_LEVEL_CONST 9999
#define COMMAND_LIT_DIM_CONST 9999
#define DEVICECATEGORY_LIGHTS_CONST 9999
#define COMMAND_ADJUST_LIGHTS_CONST 9999

bool DCEMI_PS_Lighting::Register()
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Lighting::Dimming_RelativeOrAbsolute))
		,0,0,0,DEVICECATEGORY_LIGHTS_CONST,MESSAGETYPE_COMMAND,COMMAND_LIT_DIM_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Lighting::AdjustLights))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_ADJUST_LIGHTS_CONST);

	return true;
}

bool DCEMI_PS_Lighting::Dimming_RelativeOrAbsolute(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	map<long, string>::iterator it = pMessage->m_mapParameters.find(C_COMMANDPARAMETER_ABS_LEVEL_CONST);
	if( it!=pMessage->m_mapParameters.end() )
	{
		string s = (*it).second;
		pDeviceTo->m_iLastState=atoi(s.c_str());
	}
	it = pMessage->m_mapParameters.find(C_COMMANDPARAMETER_REL_LEVEL_CONST);
	if( it!=pMessage->m_mapParameters.end() )
	{
		string s = (*it).second;
		pDeviceTo->m_iLastState += atoi(s.c_str());
	}
	return true;
}

bool DCEMI_PS_Lighting::AdjustLights(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	if( !pDeviceFrom )
		return true;
	// 0=ALL OFF, 1=ALL ON, 2=TOGGLE, -X reduce by X, +X = increase by X
	int PK_Room = atoi(pMessage->m_mapParameters[C_COMMANDPARAMETER_ROOM_CONST].c_str());
	class DCERoom *pRoom=NULL;
	if( PK_Room )
		pRoom = m_pRouter->m_mapRoom[PK_Room];
	if( !pRoom && pDeviceFrom )
		pRoom = pDeviceFrom->m_pRoom;
	if( pRoom )
	{
		string ID = pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];

		if( ID=="0" )
			pRoom->m_bLightsLastOn=false;
		else if( ID=="1" )
			pRoom->m_bLightsLastOn=true;
		else if( ID=="2" )
		{
			pRoom->m_bLightsLastOn=!pRoom->m_bLightsLastOn;
			ID = StringUtils::itos(pRoom->m_bLightsLastOn);
		}

		list<DeviceData_Router *>::iterator itDevices;
		for(itDevices=pRoom->m_listDevices.begin();itDevices!=pRoom->m_listDevices.end();++itDevices)
		{
			DeviceData_Router *pDevice = (*itDevices);
			if( !pDevice->m_pLightingInformation )
				continue;

			Message *pMessage=NULL;
			if( ID=="0" )
			{
				pMessage = new Message(pMessage->m_dwPK_Device_From,pDevice->m_dwPK_Device,PRIORITY_NORMAL,
					MESSAGETYPE_COMMAND,COMMAND_GEN_OFF_CONST,0);
			}
			else if( ID=="1" )
			{
				pMessage = new Message(pMessage->m_dwPK_Device_From,pDevice->m_dwPK_Device,PRIORITY_NORMAL,
					MESSAGETYPE_COMMAND,COMMAND_GEN_ON_CONST,0);
			}
			else if( ID.substr(0)=="+" )
			{
				int Level = pDevice->m_pLightingInformation->m_iLastState==-1 ? 100 :
					pDevice->m_pLightingInformation->m_iLastState + atoi(ID.substr(1).c_str());
				if( Level>100 )
					Level=100;
				pMessage = new Message(pMessage->m_dwPK_Device_From,pDevice->m_dwPK_Device,PRIORITY_NORMAL,
					MESSAGETYPE_COMMAND,COMMAND_LIT_DIM_CONST,1,C_COMMANDPARAMETER_ABS_LEVEL_CONST,StringUtils::itos(Level).c_str());
			}
			else if( ID.substr(0)=="-" )
			{
				int Level = pDevice->m_pLightingInformation->m_iLastState==-1 ? 0 :
					pDevice->m_pLightingInformation->m_iLastState - atoi(ID.substr(1).c_str());
				if( Level<0 )
					Level=0;
				pMessage = new Message(pMessage->m_dwPK_Device_From,0,PRIORITY_NORMAL,
					MESSAGETYPE_COMMAND,COMMAND_LIT_DIM_CONST,1,C_COMMANDPARAMETER_ABS_LEVEL_CONST,StringUtils::itos(Level).c_str());
			}
			if( pMessage )
				m_pRouter->DispatchMessage(pMessage);
		}
	}
*/
	return true;
}
