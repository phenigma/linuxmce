#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"

#include "DCEMI_PS_MobileOrbiter.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_EventParameter.h"
#include "MobileOrbiter.h"
#include "PlutoOrbiter.h"

// temp
#define COMMAND_TOGGLE_FOLLOW_ME_CONST 99
#define EVENT_MOBILE_ORBITER_LINKED_CONST 99
#define EVENT_MOBILE_ORBITER_DETECTED_CONST 99
#define EVENT_MOBILE_ORBITER_LOST_CONST 99
#define COMMAND_LOCK_ONTO_LOCATION_CONST 99
#define COMMAND_LINK_MEDIA_REMOTE_CONST 99
#define C_COMMANDPARAMETER_ROOM_CONST 99
#define C_EVENTPARAMETER_STRENGTH_CONST 99
#define C_EVENTPARAMETER_PK_DEVICE_CONST 99
#define C_EVENTPARAMETER_ID_CONST 99
#define COMMAND_LINK_WITH_MOBILE_ORBITER_CONST 99

bool DCEMI_PS_MobileOrbiter::Register()
{
	m_pDCEMI_PS_Orbiter = (class DCEMI_PS_Orbiter *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Orbiter_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::ToggleFollowMe))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_TOGGLE_FOLLOW_ME_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::MobileOrbiterLinked))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_MOBILE_ORBITER_LINKED_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::MobileOrbiterDetected))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_MOBILE_ORBITER_DETECTED_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::MobileOrbiterLost))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_MOBILE_ORBITER_LOST_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::LockOntoLocation))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_LOCK_ONTO_LOCATION_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_MobileOrbiter::LinkMediaRemote))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_LINK_MEDIA_REMOTE_CONST);

	return true;
}

bool DCEMI_PS_MobileOrbiter::ToggleFollowMe(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;
	PlutoOrbiter *ptrController = m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter_Find(pDeviceFrom->m_iPK_Device);
	if( ptrController )
	{
		MobileOrbiter *pMobileOrbiter = ptrController->m_pMobileOrbiter;
		switch(atoi(pMessage->m_Parameters[COMMANDPARAMETER_ID_CONST].c_str()))
		{
		case MENUTYPE_LIGHTING:
			pMobileOrbiter->m_bFollowMeLights = !pMobileOrbiter->m_bFollowMeLights;
			break;
		case MENUTYPE_CLIMATE:
			pMobileOrbiter->m_bFollowMeClimate = !pMobileOrbiter->m_bFollowMeClimate;
			break;
		case MENUTYPE_MEDIA:
			pMobileOrbiter->m_bFollowMeMedia = !pMobileOrbiter->m_bFollowMeMedia;
			break;
		case MENUTYPE_TELECOM:
			pMobileOrbiter->m_bFollowMeTelecom = !pMobileOrbiter->m_bFollowMeTelecom;
			break;
		case MENUTYPE_SECURITY:
			pMobileOrbiter->m_bFollowMeSecurity = !pMobileOrbiter->m_bFollowMeSecurity;
			break;
		}
		pMobileOrbiter->ShowMenu(MENUTYPE_REFRESH,pMessage);
	}
	return true;
}


bool DCEMI_PS_MobileOrbiter::MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Got orbiter detected, but pDeviceFrom is NULL");
		return true;
	}
	string ID = pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST];

	MobileOrbiter *pMobileOrbiter = m_mapMobileOrbiter_Mac_Find(ID);
	if( !pMobileOrbiter )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Linked unknown bluetooth device %s",pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST].c_str());
	}
	else
	{
		pMobileOrbiter->m_pPlutoOrbiter->SetDefaultFlags();
		// Remove any old association
		if( pMobileOrbiter->m_pDevice_CurrentDetected && pMobileOrbiter->m_pDevice_CurrentDetected!=pDeviceFrom )
		{
			pMobileOrbiter->RemovingAssocation();
			
			m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPK_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_WITH_MOBILE_ORBITER_CONST,2,COMMANDPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
				COMMANDPARAMETER_OnOff_CONST,"0"));
		}

		DeviceData_Router *pDevice_PriorDetected = pMobileOrbiter->m_pDevice_CurrentDetected;
		// Associated with a new media director.  Show the corresponding menu
		pMobileOrbiter->m_pDevice_CurrentDetected=pDeviceFrom;
		pMobileOrbiter->m_pDevice_This->m_bIsReady=true;
		// See if there's an ent group involved

		if( pMobileOrbiter->m_pEntGroupAudioZone_LockedOn )
		{
// TODO			pMobileOrbiter->m_pPlutoOrbiter->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
		}
		else
		{
			pMobileOrbiter->m_pPlutoOrbiter->m_pEntZone=NULL;
			if( pDeviceFrom->m_pDevice_ControlledVia )
			{
/*
				DeviceData_Base *pEntGroup = m_pPlutoEvents->FindSibling(pDeviceFrom->m_pDevice_ControlledVia,DEVICETEMPLATE_Entertain_Unit_CONST);
				if( pEntGroup )
				{
					pMobileOrbiter->m_pPlutoOrbiter->m_pEntGroup = m_pRouter->m_pDataGridDevice->m_mapEntGroup_Find(pEntGroup->m_iPK_Device);
				}
*/
			}
		}

		if( pMobileOrbiter->m_pRoom_LockedOn )
			pMobileOrbiter->m_pDevice_This->m_pRoom=pMobileOrbiter->m_pRoom_LockedOn;
		else 
			pMobileOrbiter->m_pDevice_This->m_pRoom=pDeviceFrom->m_pRoom;

		pMobileOrbiter->NewAssociation(pDevice_PriorDetected);

		pMobileOrbiter->ShowMenu(0,pMessage); // 0=main menu
	}
	return true;
}


bool DCEMI_PS_MobileOrbiter::MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
g_pPlutoLogger->Write(LV_WARNING, "Mobile orbiter detected");
	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Got orbiter detected, but pDeviceFrom is NULL");
		return true;
	}
	MobileOrbiter *pMobileOrbiter = m_mapMobileOrbiter_Mac_Find(pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST]);

	if( !pMobileOrbiter )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Detected unknown bluetooth device %s",pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST].c_str());
	}
	else
	{
		pMessage->m_Parameters[C_EVENTPARAMETER_PK_DEVICE_CONST] = StringUtils::itos(pMobileOrbiter->m_pDevice_This->m_iPK_Device);
		int SignalStrength = atoi(pMessage->m_Parameters[C_EVENTPARAMETER_STRENGTH_CONST].c_str());

		if( pMobileOrbiter->m_pDevice_CurrentDetected==pDeviceFrom )
		{
			pMobileOrbiter->m_iLastSignalStrength=SignalStrength;
			g_pPlutoLogger->Write(LV_WARNING,"Redetecting the same device?");
#ifdef ANNOUNCE_MOD
if( time(NULL)-tLastTime>5 )
{
tLastTime=time(NULL);
string t = "wrong " + StringUtils::itos(SignalStrength) + " x.x.";
m_pRouter->DispatchMessage( new Message(DEVICEID_DCEROUTER,m_pPlutoEvents->m_TTSDevice,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
COMMAND_SAY_TO_DEVICE_CONST,2,COMMANDPARAMETER_Text_CONST,t.c_str(),
COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(Controller).c_str()));
}
#endif
		}
		else
		{
			if( pMobileOrbiter->m_pDevice_CurrentDetected )
			{
/*
				pMobileOrbiter->m_iLastSignalStrength=0;
				PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
				int RouteToDevice = DEVICEID_NULL;
				map<int,int>::iterator iRoute;
				iRoute = m_Routing_DeviceToController.find(pMobileOrbiter->m_pDevice_CurrentDetected->m_iPK_Device);
				if (iRoute!=m_Routing_DeviceToController.end())
				{
					RouteToDevice = (*iRoute).second;
				}

				DeviceClientMap::iterator iDeviceConnection = m_mapCommandHandlers.find(RouteToDevice);
				ServerSocket *pSocket = (*iDeviceConnection).second;
				if (iDeviceConnection != m_mapCommandHandlers.end())
				{
					PLUTO_SAFETY_LOCK(slConn,((*iDeviceConnection).second->m_ConnectionMutex));
					if (!pSocket->SendMessage(new Message(0, pMobileOrbiter->m_pDevice_CurrentDetected->m_iPK_Device, 
						PRIORITY_NORMAL, MESSAGETYPE_SIGNALSTRENGTH, pMobileOrbiter->m_pDevice_This->m_iPK_Device, 1,
						COMMANDPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str())))
					{
						g_pPlutoLogger->Write(LV_CRITICAL, "Socket %p failure sending signal strength request to device %d", 
							(*iDeviceConnection).second, pMobileOrbiter->m_pDevice_CurrentDetected->m_iPK_Device);

						// TODO :  The socket failed, core needs to remove client socket

					}
					string sResult;
					if (pSocket->ReceiveString(sResult) && sResult.substr(0,7)=="MESSAGE")
					{
						Message *pMessage=pSocket->ReceiveMessage(atoi(sResult.substr(8).c_str()));
						if (pMessage)
						{
							pMobileOrbiter->m_iLastSignalStrength = pMessage->m_ID;
							delete pMessage;
						}
					}
				}
*/
			}

			if( pMobileOrbiter->m_pDevice_CurrentDetected && pMobileOrbiter->m_iLastSignalStrength>SignalStrength )
			{
				g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter %s already has a strong association with %d (%d/%d)",
					pMobileOrbiter->m_sID.c_str(),pMobileOrbiter->m_pDevice_CurrentDetected->m_sDescription.c_str(),pMobileOrbiter->m_iLastSignalStrength,
					SignalStrength);
#ifdef ANNOUNCE_MOD
if( time(NULL)-tLastTime>5 )
{
tLastTime=time(NULL);
string t = "weak " + pMobileOrbiter->m_pDevice_This->m_sDescription + ". " + pDeviceFrom->m_sDescription + ".";
m_pRouter->DispatchMessage( new Message(DEVICEID_DCEROUTER,m_pPlutoEvents->m_TTSDevice,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
COMMAND_SAY_TO_DEVICE_CONST,2,COMMANDPARAMETER_Text_CONST,t.c_str(),
COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(Controller).c_str()));
}
#endif
			}
			else
			{
				m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,pDeviceFrom->m_iPK_Device,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_WITH_MOBILE_ORBITER_CONST,2,COMMANDPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
					COMMANDPARAMETER_OnOff_CONST,"1"));

string t = " detected " + pMobileOrbiter->m_pDevice_This->m_sDescription + ", " + pDeviceFrom->m_sDescription + ".";
/*
m_pRouter->DispatchMessage( new Message(DEVICEID_DCEROUTER,m_pPlutoEvents->m_TTSDevice,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
COMMAND_SAY_TO_DEVICE_CONST,2,COMMANDPARAMETER_Text_CONST,t.c_str(),
COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(Controller).c_str()));
*/
			}
		}
	}
g_pPlutoLogger->Write(LV_WARNING, "finsihed processing");
	return true;
}


bool DCEMI_PS_MobileOrbiter::MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;
	/*
	MobileOrbiter *pMobileOrbiter = NULL;
	string ID = pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST];
	DeviceData_Router *pD = m_pPlutoEvents->m_mapMobileOrbiters_Find(ID);
	if( pD )
	{
		pMobileOrbiter = pD->m_pMobileOrbiter;
		pMessage->m_Parameters[C_EVENTPARAMETER_PK_DEVICE_CONST] = StringUtils::itos(pD->m_iPK_Device);
	}

	if( !pMobileOrbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Lost unknown bluetooth device %s",pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST].c_str());
	}
	else if( pMobileOrbiter->m_pDevice_CurrentDetected==pDeviceFrom )
	{
		if( pMobileOrbiter->m_pPlutoOrbiter->m_pEntZone )
		{
			m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,DEVICEID_DCEROUTER,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_MEDIA_REMOTE_CONST,2,
				COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(pMobileOrbiter->m_pPlutoOrbiter->m_pEntZone->m_PK_Device).c_str(),
				COMMANDPARAMETER_OnOff_CONST,"0"));
		}
		m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPK_Device,
			PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_WITH_MOBILE_ORBITER_CONST,2,COMMANDPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
			COMMANDPARAMETER_OnOff_CONST,"0"));

		pMobileOrbiter->RemovingAssocation();
		pMobileOrbiter->m_pDevice_CurrentDetected=NULL; 
	}
	*/
	return true;
}

bool DCEMI_PS_MobileOrbiter::LockOntoLocation(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom ) 
		return true;
	PlutoOrbiter *ptrController = m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter_Find(pDeviceFrom->m_iPK_Device);
	if( !ptrController || !ptrController->m_pMobileOrbiter )
		return true;

	MobileOrbiter *pMO = ptrController->m_pMobileOrbiter;
	int PK_Room=atoi(pMessage->m_Parameters[C_COMMANDPARAMETER_ROOM_CONST].c_str());
	int PK_EntGroup=atoi(pMessage->m_Parameters[COMMANDPARAMETER_PK_Device_CONST].c_str());

	if( PK_Room )
	{
		pMO->m_pRoom_LockedOn = m_pRouter->m_mapDCERoom_Find(PK_Room);
		pMO->m_pDevice_This->m_pRoom = pMO->m_pRoom_LockedOn;
	}
	else
		pMO->m_pRoom_LockedOn = NULL;

	if( PK_EntGroup )
	{
		pMO->m_pEntGroupAudioZone_LockedOn = m_pRouter->m_mapDeviceData_Router_Find(PK_EntGroup);
// TODO		pMO->m_pPlutoOrbiter->m_pEntGroup = pMO->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
	}
	else
		pMO->m_pEntGroupAudioZone_LockedOn = NULL;
	pMO->ShowMenu(MENUTYPE_MAINMENU,pMessage);
	return true;
}

bool DCEMI_PS_MobileOrbiter::LinkMediaRemote(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;

	PlutoOrbiter *ptrController = m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter_Find(pDeviceFrom->m_iPK_Device);
	if( !ptrController )
		return true;
/*
	int PK_EntGroup = atoi(pMessage->m_Parameters[COMMANDPARAMETER_PK_Device_CONST].c_str());
	int On_Off = atoi(pMessage->m_Parameters[COMMANDPARAMETER_OnOff_CONST].c_str());
	class EntGroup *pEntGroup = m_pRouter->m_pDataGridDevice->m_mapEntGroup_Find(PK_EntGroup);
	if( !pEntGroup )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got LinkMediaRemote for unknown ent group: %d",PK_EntGroup);
	}
	else if( On_Off==0 )
	{
		bool bRemoved=false;
		PLUTO_SAFETY_LOCK(vm,m_pRouter->m_pDataGridDevice->m_pCore->m_VariableMutex);
		list<class EntGroup_Orbitter_Remote *>::iterator it;
		for(it=pEntGroup->m_listEntGroup_Orbitter_Remote.begin();it!=pEntGroup->m_listEntGroup_Orbitter_Remote.end();++it)
		{
			EntGroup_Orbitter_Remote *pEGO = (*it);
			if( pEGO->m_pDevice_Controller==pDeviceFrom )
			{
				bRemoved=true;
				pEntGroup->m_listEntGroup_Orbitter_Remote.erase(it);
				break;
			}
		}
		if( !bRemoved )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Couldn't remove remote control %d from ent group %d",pDeviceFrom->m_iPK_Device,pEntGroup->m_PK_EntGroup);
		}
	}
	else
	{
		bool bAlreadyHave=false;
		EntGroup_Orbitter_Remote *pEGO=NULL;
		list<class EntGroup_Orbitter_Remote *>::iterator it;
		PLUTO_SAFETY_LOCK(vm,m_pRouter->m_pDataGridDevice->m_pCore->m_VariableMutex);
		for(it=pEntGroup->m_listEntGroup_Orbitter_Remote.begin();it!=pEntGroup->m_listEntGroup_Orbitter_Remote.end();++it)
		{
			pEGO = (*it);
			if( pEGO->m_pDevice_Controller==pDeviceFrom )
			{
				bAlreadyHave=true;
				break;
			}
		}
		if( !bAlreadyHave )
		{
			pEGO = new EntGroup_Orbitter_Remote();
			pEGO->m_pDevice_Controller = pDeviceFrom;
			pEGO->m_pEntGroup = pEntGroup;
			pEGO->PVRInfo = pMessage->m_Parameters[C_COMMANDPARAMETER_ID_PVR_SESSION_CONST];
			pEGO->Text = pMessage->m_Parameters[COMMANDPARAMETER_Text_CONST];
			pEGO->GraphicImage = pMessage->m_Parameters[C_COMMANDPARAMETER_GRAPHIC_IMAGE_CONST];
			pEntGroup->m_listEntGroup_Orbitter_Remote.push_back(pEGO);
		}
		pEGO->Screen = pMessage->m_Parameters[COMMANDPARAMETER_PK_DesignObj_CONST];
		vm.Release();
*/
		/*
		if( atoi(pEGO->Screen.c_str())==DESIGNOBJ_MNUPVR_CONST )
		{
			pEntGroup->m_bPlutoTV=true;
			if( !pEntGroup->m_pWatchingStream->pProvider_Station__Schedule )
			{
				// This must be an md which defaults to non-pluto tv, and we're switching
				// to pluto tv for the first time
				// Stop this media, and restart it with PlutoTV
				pEntGroup->FindDefaultShowForController(pEGO->m_pDevice_Controller->m_ptrController_Iam,pEntGroup->m_pWatchingStream);
				if( pEntGroup->m_pWatchingStream->pProvider_Station__Schedule )
				{
					pEntGroup->m_pWatchingStream->m_DesignObj_RemoteScreen=DESIGNOBJ_MNUPVR_CONST;
					pEntGroup->TurnOnAndTune(pEGO->m_pDevice_Controller->m_ptrController_Iam,pEntGroup->m_pWatchingStream->pProvider_Station__Schedule);
				}
				else
				{
					// Send the controller back to non-pluto tv
					// Do this within the queue because this can be called from within a mobile orbiter
					// show menu, and we don't want to loop right back around again
					pEntGroup->m_pWatchingStream->m_DesignObj_RemoteScreen=DESIGNOBJ_MNUSATELLITECABLEBOX_CONST;
					AddMessageToQueue(new Message(DEVICEID_DCEROUTER,pMessage->m_DeviceIDFrom,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
						COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_MNUSATELLITECABLEBOX_CONST).c_str()));
				}
			}
			pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_REMOTE_1_DEVICE_CONST,StringUtils::itos(pEntGroup->m_PK_EntGroup));
			pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_PK_DEVICE_CONST,StringUtils::itos(pEntGroup->m_PK_EntGroup));
		}
		else if( atoi(pEGO->Screen.c_str())==DESIGNOBJ_MNUSATELLITECABLEBOX_CONST ) // Non-pluto TV
		{
			if( pEntGroup->m_pWatchingStream )
				pEntGroup->m_pWatchingStream->m_DesignObj_RemoteScreen=DESIGNOBJ_MNUSATELLITECABLEBOX_CONST;

			pEntGroup->m_bPlutoTV=false;
			pEGO->PVRInfo=pMessage->m_Parameters[C_COMMANDPARAMETER_ID_PVR_SESSION_CONST];
			g_pPlutoLogger->Write(LV_WARNING,"Switching to pvr with %s",pEGO->PVRInfo.c_str());
			if( pEGO->PVRInfo.length()>1 ) // a tuner specified
			{
				// Be sure the right inputs are set
				int PK_Device_Tuning = atoi(pEGO->PVRInfo.c_str());
				if( pEntGroup->m_pWatchingStream )
					pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo=PK_Device_Tuning;
				m_pRouter->DispatchMessage(new Message(pMessage->m_DeviceIDFrom,PK_Device_Tuning,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GEN_ON_CONST,0));

				// Help out the controller 
				pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_REMOTE_1_DEVICE_CONST,StringUtils::itos(pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo));
				pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_PK_DEVICE_CONST,StringUtils::itos(pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo));
				if( pDeviceFrom->m_ptrController_Iam->m_pMobileOrbiter )
				{
					// Help out the mobile orbiter and send it back to it's menu
					pDeviceFrom->m_ptrController_Iam->GotoScreen(DESIGNOBJ_MNUSATELLITECABLEBOX_CONST);
				}
				return; // Nothing else to do--it's just a general notification
			}
			else
			{
				if( pEntGroup->m_pWatchingStream )
				{
					DeviceData_Router *pDevice_Tuning=m_pPlutoEvents->m_mapDevice_Find(pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo);
					if( pDevice_Tuning && pDevice_Tuning->m_iPK_DeviceTemplate==DEVICETEMPLATE_Entertain_Unit_CONST )
					{
						pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo=0; // This is no good anymore
						// We're coming to here after having been watching pluto tv
						// Figure out what tuning device to use
						if( pEntGroup->m_pWatchingStream->pProvider_Station__Schedule &&
							pEntGroup->m_pWatchingStream->pProvider_Station__Schedule->m_PK_Provider )
						{
							EPGProvider *pProvider = pEntGroup->m_mapProviders[pEntGroup->m_pWatchingStream->pProvider_Station__Schedule->m_PK_Provider];
							// The user was watching Pluto TV.  Use that tuning device
							if( pProvider )
								pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo = pProvider->m_PK_Device_TuningLive;
						}
					}
				}
				if( pEntGroup->m_pWatchingStream && !pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo )
				{
g_pPlutoLogger->Write(LV_WARNING,"not watching anything, find a tuning device");

					for(int i=0;i<(int) pEntGroup->m_vectDevice_AV.size();++i)
					{
						DeviceData_Router *pDevice = pEntGroup->m_vectDevice_AV[i];
						if( pDevice->m_iPK_DeviceCategory==DEVICECATEGORY_SATELLITE_CONST )
						{
							pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo = pDevice->m_iPK_Device;
						}
					}
				}
				if( pEntGroup->m_pWatchingStream && pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo )
				{
g_pPlutoLogger->Write(LV_WARNING,"setting tuning device to: %d",pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo);
					pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_REMOTE_1_DEVICE_CONST,StringUtils::itos(pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo));
					pDeviceFrom->m_ptrController_Iam->SetVariable(VARIABLE_PK_DEVICE_CONST,StringUtils::itos(pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo));
				}
			}
		}
	}
*/
// TODO		pEntGroup->PutMediaPictureOnRemote(NULL,ptrController);
	return true;
}
