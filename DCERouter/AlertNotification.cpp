#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "../pluto_main/Table_DeviceTemplate.h"
#include "../pluto_main/Table_HouseMode.h"
#include "../pluto_main/Table_C_UserMode.h"
#include "../pluto_main/Table_Command.h"
#include "../pluto_main/Table_CommandParameter.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_FloorplanObjectType.h"
#include "../pluto_main/Table_DeviceCategory.h"
#include "../pluto_main/Table_Event.h"
#include "../pluto_main/Table_Variable.h"
#include "../pluto_main/Table_DesignObj.h"
#include "../pluto_main/Table_Text.h"
#include "PlutoEvents.h"
#include "PlutoServerCore.h"
#include "DataGridDevice.h"
#include "OCUser.h"
#include "AlertNotification.h"
#include "Router.h"

class AlarmCallBackData
{
public:
	AlarmCallBackData(int TypeOfBreach,Device *Device)
	{
		iTypeOfBreach=TypeOfBreach;
		pDevice=Device;
		iVectorCount=0;
	}
	int iVectorCount;
	int iTypeOfBreach;
	Device *pDevice;
};

AlertNotification::AlertNotification(class PlutoServerCore *pCore,class Device *pDevice_WAPServer)
{
	m_pCore=pCore;
	m_pDevice_WAPServer=pDevice_WAPServer;

	string s = m_pDevice_WAPServer->m_mapDeviceParameters_Find(C_DEVICEDATA_MOBILE_ORBITERS_CONST);
	string::size_type pos=0;
	m_MobileOrbiterDelay = atoi(StringUtils::Tokenize(s,",",pos).c_str());

	string Number;
	while( (Number=StringUtils::Tokenize(s,",",pos))!="" )
	{
		PhoneNotification *pNotification = new PhoneNotification();
		pNotification->PhoneNumber = Number;
		pNotification->bMonitorMode = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bSecurity = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bFire = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bIntercom = (StringUtils::Tokenize(s,",",pos)=="1");
		m_vectMobileOrbiters.push_back(pNotification);
	}

	s = m_pDevice_WAPServer->m_mapDeviceParameters_Find(C_DEVICEDATA_EXTRA_PHONE_NUMS_CONST);
	pos=0;
	m_OtherPhoneDelay = atoi(StringUtils::Tokenize(s,",",pos).c_str());
	m_enumCallOrder = (enumCallOrder) atoi(StringUtils::Tokenize(s,",",pos).c_str());

	while( (Number=StringUtils::Tokenize(s,",",pos))!="" )
	{
		PhoneNotification *pNotification = new PhoneNotification();
		pNotification->PhoneNumber = Number;
		pNotification->bMonitorMode = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bSecurity = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bFire = (StringUtils::Tokenize(s,",",pos)=="1");
		pNotification->bIntercom = (StringUtils::Tokenize(s,",",pos)=="1");
		m_vectOtherPhones.push_back(pNotification);
	}

	string Name;
	s = m_pDevice_WAPServer->m_mapDeviceParameters_Find(C_DEVICEDATA_NEIGHBORS_TO_CALL_CONST);
	pos=0;
	while( (Name=StringUtils::Tokenize(s,",",pos))!="" )
	{
		Neighbor *pNeighbor = new Neighbor();
		pNeighbor->Name=Name;
		pNeighbor->Number=StringUtils::Tokenize(s,",",pos);
		m_vectNeighbors.push_back(pNeighbor);
	}
}

AlertNotification::~AlertNotification()
{
	for(int i=0;i<(int) m_vectMobileOrbiters.size();++i)
	{
		PhoneNotification *p = m_vectMobileOrbiters[i];
		delete p;
	}
	m_vectMobileOrbiters.clear();
	
	for(int i=0;i<(int) m_vectOtherPhones.size();++i)
	{
		PhoneNotification *p = m_vectOtherPhones[i];
		delete p;
	}
	m_vectOtherPhones.clear();

	for(int i=0;i<(int) m_vectNeighbors.size();++i)
	{
		Neighbor *n = m_vectNeighbors[i];
		delete n;
	}
	m_vectNeighbors.clear();
}

void AlertNotification::Notify(int TypeOfBreach,class Device *pDevice)
{
	/*
	static int s_AlertID=1; // Need an alert ID that's unique .. the number itself is unimportant
	s_AlertID++;
	g_pPlutoLogger->Write(LV_STATUS,"AlertNotify: %d %d",TypeOfBreach,pDevice->m_dwPK_Device);

	if( TypeOfBreach == WAP_EVENT_NOTIFICATION )
	{
		SecurityDeviceEvent sde;
		sde.GetSnapshots(pDevice,m_pCore,s_AlertID);
		m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,m_pDevice_WAPServer->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_ALERT_CONST,
			2,COMMANDPARAMETER_Text_CONST,pDevice->m_sDescription.c_str(),
			COMMANDPARAMETER_ID_CONST,StringUtils::itos(s_AlertID).c_str()));
	}
	else if( TypeOfBreach == WAP_EVENT_FIRE )
	{
		SecurityDeviceEvent sde;
		sde.GetSnapshots(pDevice,m_pCore,s_AlertID);
		m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,m_pDevice_WAPServer->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_ALERT_CONST,
			2,COMMANDPARAMETER_Text_CONST,pDevice->m_sDescription.c_str(),
			COMMANDPARAMETER_ID_CONST,StringUtils::itos(s_AlertID).c_str()));
	}
	else if( TypeOfBreach == WAP_EVENT_SECURITY_BREACH )
	{
		string BreachInfo="";
		int iNumBreaches=0;
		for(size_t i=m_pCore->m_pPlutoEvents->m_vectSecurityEvent.size();i>0;--i)
		{
			SecurityEvent *pSecurityEvent = m_pCore->m_pPlutoEvents->m_vectSecurityEvent[i-1];
			if( !pSecurityEvent->m_bPendingDisable )
			{
				iNumBreaches++;
				BreachInfo += pSecurityEvent->ReportEventAsText(iNumBreaches);

				pSecurityEvent->m_WAPAlertID=s_AlertID;  // This way we can add to the WAP message after the fact

				for(size_t i2=0;i2<pSecurityEvent->m_vectDeviceEvents.size();i2++)
				{
					SecurityDeviceEvent *pSecurityDeviceEvent = pSecurityEvent->m_vectDeviceEvents[i2];
					for( size_t i3=0;i3<pSecurityDeviceEvent->m_vectImages.size();++i3 )
					{
						void *ImageData = pSecurityDeviceEvent->m_vectImages[i3];
						int ImageLength = pSecurityDeviceEvent->m_vectLengths[i3];
                        
						m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,m_pDevice_WAPServer->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_ADD_ALERT_IMAGE_CONST,
							3,-COMMANDPARAMETER_Data_CONST, ImageData, ImageLength,
							COMMANDPARAMETER_Text_CONST,pSecurityDeviceEvent->m_vectDescriptions[i3].c_str(),
							COMMANDPARAMETER_ID_CONST,StringUtils::itos(s_AlertID).c_str())); 
					}
				}
			}
		}

		m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,m_pDevice_WAPServer->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_ALERT_CONST,
			2,COMMANDPARAMETER_Text_CONST,BreachInfo.c_str(),
//			C_COMMANDPARAMETER_ACTIONGROUP_LIST_CONST,apAGList ? apAGList->m_sValue.c_str() : "",
			COMMANDPARAMETER_ID_CONST,StringUtils::itos(s_AlertID).c_str()));
	}
	else if( TypeOfBreach == WAP_EVENT_INTERCOM )
	{
		m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER,m_pDevice_WAPServer->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_ALERT_CONST,
			2,COMMANDPARAMETER_Text_CONST,pDevice->m_sDescription.c_str(),
			COMMANDPARAMETER_ID_CONST,StringUtils::itos(s_AlertID).c_str()));
	}

	if( m_enumCallOrder==Orbiters_Then_Other || m_enumCallOrder==Both_At_Same_Time )
		AlarmCallback(TIMER_NOTIFY_MOBILE_ORBITERS,(void *)new AlarmCallBackData(TypeOfBreach,pDevice));

	if( m_enumCallOrder==Other_Then_Orbiters || m_enumCallOrder==Both_At_Same_Time )
		AlarmCallback(TIMER_NOTIFY_OTHER_PHONES,(void *)new AlarmCallBackData(TypeOfBreach,pDevice));
		*/
}

void AlertNotification::AlarmCallback(int id, void* param)
{
	if(id != TIMER_NOTIFY_MOBILE_ORBITERS && id != TIMER_NOTIFY_OTHER_PHONES )
		return;

	int Delay = (id == TIMER_NOTIFY_MOBILE_ORBITERS ? m_MobileOrbiterDelay : m_OtherPhoneDelay);

	vector<PhoneNotification *> vectNumbers;
	if(id == TIMER_NOTIFY_MOBILE_ORBITERS )
		vectNumbers = m_vectMobileOrbiters;
	else
		vectNumbers = m_vectOtherPhones;

	AlarmCallBackData *pData = (AlarmCallBackData *) param;
	for(;pData->iVectorCount<(int) vectNumbers.size();++pData->iVectorCount)
	{
		PhoneNotification *pn = vectNumbers[pData->iVectorCount];
		if( (pData->iTypeOfBreach==WAP_EVENT_FIRE && !pn->bFire) ||
			(pData->iTypeOfBreach==WAP_EVENT_SECURITY_BREACH && !pn->bSecurity) ||
			(pData->iTypeOfBreach==WAP_EVENT_NOTIFICATION && !pn->bMonitorMode) )
				continue;

		if( pData->pDevice->m_WAPEventNum )
		{
			char CallerID[50];
			sprintf(CallerID,"555%d%06d",pData->iTypeOfBreach,pData->pDevice->m_dwPK_Device);
			if( pData->iTypeOfBreach==WAP_EVENT_INTERCOM )
			{
				m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER, m_pCore->m_pPlutoEvents->m_TelecomServer, PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_XFER_OR_CONF_CONST,
					5,C_COMMANDPARAMETER_CALL_ID_CONST,"123" /* ??? TODO what was this?? ptrEventInstance->pMessage->m_mapParameters[C_EVENTPARAMETER_CALL_ID_CONST].c_str()*/,
					C_COMMANDPARAMETER_BOOLEAN_CONST,"0",
					C_COMMANDPARAMETER_PHONE_NUM_CONST,pn->PhoneNumber.c_str(),C_COMMANDPARAMETER_CALLER_ID_CONST,CallerID,
					COMMANDPARAMETER_Text_CONST,"Pluto"));
			}
			else
			{
				m_pCore->m_pRouter->DispatchMessage( new Message(DEVICEID_EVENTMANAGER, m_pCore->m_pPlutoEvents->m_TelecomServer, PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GENERATE_OUTSIDE_CALL_CONST,
					2,C_COMMANDPARAMETER_PHONE_NUM_CONST,pn->PhoneNumber.c_str(),C_COMMANDPARAMETER_CALLER_ID_CONST,CallerID));
			}
		}

		// If we've got to wait before notifying the other numbers, we'll return and finish up later
		if( pData->iVectorCount<((int) vectNumbers.size())-1 )
		{
			if( Delay )
			{
				++pData->iVectorCount;
				m_pCore->m_pAlarmManager->AddRelativeAlarm(Delay,this,id,pData);
				return;
			}
		}
	}

	// See if we're supposed to continue notifying the other
	if( id == TIMER_NOTIFY_MOBILE_ORBITERS && m_enumCallOrder==Orbiters_Then_Other )
	{
		pData->iVectorCount=0;
		m_pCore->m_pAlarmManager->AddRelativeAlarm(Delay,this,TIMER_NOTIFY_OTHER_PHONES,pData);
	}
	else if( id == TIMER_NOTIFY_OTHER_PHONES && m_enumCallOrder==Other_Then_Orbiters )
	{
		pData->iVectorCount=0;
		m_pCore->m_pAlarmManager->AddRelativeAlarm(Delay,this,TIMER_NOTIFY_MOBILE_ORBITERS,pData);
	}
	else
		delete pData;
}
