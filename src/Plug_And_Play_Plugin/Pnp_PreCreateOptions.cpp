/*
 Pnp_PreCreateOptions

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "Pnp_PreCreateOptions.h"
#include "PnpQueue.h"
#include "PnpQueueEntry.h"
#include "Plug_And_Play_Plugin.h"
#include "DeviceData_Base.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Screen.h"

using namespace DCE;

Pnp_PreCreateOptions::Pnp_PreCreateOptions(class PnpQueue *pPnpQueue,class Plug_And_Play_Plugin *pPlug_And_Play_Plugin)
{
	m_pPnpQueue=pPnpQueue;
	m_pDatabase_pluto_main=pPlug_And_Play_Plugin->m_pDatabase_pluto_main;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice(PnpQueueEntry *pPnpQueueEntry)
{
	// See if there is anything special we need to do for this type of device
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get());
	if( !pRow_DeviceTemplate ) // Shouldn't happen
		return true;

	DeviceCategory *pDeviceCategory = m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceCategory_Find(pRow_DeviceTemplate->FK_DeviceCategory_get());
	if( !pDeviceCategory )
		return true;

	if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty() )
		m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,false);  // Don't block since we may not need to ask anyway
	
	if( !OkayToCreateDevice_Username(pPnpQueueEntry,pRow_DeviceTemplate) )  // It needs a username
		return false;
	if( !OkayToCreateDevice_Room(pPnpQueueEntry,pRow_DeviceTemplate) )  // It needs a room
		return false;
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Storage_Devices_CONST) )
		return OkayToCreateDevice_NetworkStorage(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		return OkayToCreate_Cameras(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Mobile_Orbiter_CONST) )
		return OkayToCreate_MobilePhone(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Capture_Cards_CONST) )
		return OkayToCreate_CaptureCard(pPnpQueueEntry,pRow_DeviceTemplate);

	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_Username(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// See if a password is required
	map<int,string>::iterator it;
	if( (it=pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Password_Required_CONST))!=pPnpQueueEntry->m_mapPK_DeviceData.end() &&
		atoi(it->second.c_str())==1 )
	{
		bool bHasUserName = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Username_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
		// It is, and it's not specified
		if( bHasUserName==false ||
			pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Password_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
		{
			if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
				return false; // No orbiters.  Skip this one for now

			// We need to get it and block until we do
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

			if( pPnpQueueEntry->m_pOH_Orbiter )
			{
				DCE::SCREEN_Get_Username_Password_For_Devices SCREEN_Get_Username_Password_For_Devices(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
					bHasUserName,pRow_DeviceTemplate->Description_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Get_Username_Password_For_Devices);
			}
			else
			{
				DCE::SCREEN_Get_Username_Password_For_Devices_DL SCREEN_Get_Username_Password_For_Devices_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
					bHasUserName,pRow_DeviceTemplate->Description_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Get_Username_Password_For_Devices_DL);
			}
			return false;
		}
	}
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_Room(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	if( pPnpQueueEntry->m_iPK_Room!=-1 )
		return true; // The user already specified this

	if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="mobile_phone" )
		return true; // Mobile phones float around the house anyway

	Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_Autoassign_to_parents_room_CONST);
	if( pRow_DeviceTemplate_DeviceData && atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str()) )
		return true; // We will automatically pick the room in create device
	
	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	// We need to ask the user for the room
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
	if( pPnpQueueEntry->m_pOH_Orbiter )
	{
		DCE::SCREEN_Pick_Room_For_Device SCREEN_Pick_Room_For_Device(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() * -1, pRow_DeviceTemplate->Description_get(),pRow_DeviceTemplate->Comments_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Pick_Room_For_Device);
	}
	else
	{
		DCE::SCREEN_Pick_Room_For_Device_DL SCREEN_Pick_Room_For_Device_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() * -1, pRow_DeviceTemplate->Description_get(),pRow_DeviceTemplate->Comments_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Pick_Room_For_Device_DL);
	}

	return false;  
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_AlarmPanel(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreate_MobilePhone(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// The user must answer 2 questions before we create a network storage device: 1) use it automatically, 2) use our directory structure.  See which questions were answered
	bool bUserSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_PK_Users_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
	bool bPhoneSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Mobile_Orbiter_Phone_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_MobilePhone %d/%d",(int) bUserSpecified,(int) bPhoneSpecified);
#endif

	if( bUserSpecified && bPhoneSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,
			 StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPhoneDetected_CONST);

		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified both options
	}

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( bUserSpecified && !bPhoneSpecified && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
	{
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		return false;  // The user specified only 1 option.  Give him TIMEOUT_PROMPTING_USER seconds to specify the other
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->m_pOH_Orbiter=NULL;  // The user isn't responding.  Ask on all orbiters

	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter )
	{
		DCE::SCREEN_NewPhoneDetected SCREEN_NewPhoneDetected(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(),m_pPnpQueue->GetDescription(pPnpQueueEntry),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPhoneDetected);
	}
	else
	{
		DCE::SCREEN_NewPhoneDetected_DL SCREEN_NewPhoneDetected_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(),m_pPnpQueue->GetDescription(pPnpQueueEntry),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPhoneDetected_DL);
	}
	return false;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// The user must answer 2 questions before we create a network storage device: 1) use it automatically, 2) use our directory structure.  See which questions were answered
	bool bUseAutoSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Automatically_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
	bool bDirectoryStructSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_PK_Users_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage %d/%d",(int) bUseAutoSpecified,(int) bDirectoryStructSpecified);
#endif
	if( bUseAutoSpecified && bDirectoryStructSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NAS_Options_CONST);
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified both options
	}

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( bUseAutoSpecified && !bDirectoryStructSpecified && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
	{
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		return false;  // The user specified only 1 option.  Give him TIMEOUT_PROMPTING_USER seconds to specify the other
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->m_pOH_Orbiter=NULL;  // The user isn't responding.  Ask on all orbiters
	
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter )
	{
		DCE::SCREEN_NAS_Options SCREEN_NAS_Options(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NAS_Options);
	}
	else
	{
		DCE::SCREEN_NAS_Options_DL SCREEN_NAS_Options_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NAS_Options_DL);
	}
	return false;
}

bool Pnp_PreCreateOptions::OkayToCreate_Cameras(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_Cameras queue %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	string sSqlSensors = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Security_Device_CONST) + " LIMIT 1";

	string sSqlLights = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST) + " LIMIT 1";

	PlutoSqlResult result_set_sensors,result_set_lights;
	bool bHasSensors = ( (result_set_sensors.r=m_pDatabase_pluto_main->mysql_query_result(sSqlSensors)) && result_set_sensors.r->row_count );
	bool bHasLights = ( (result_set_lights.r=m_pDatabase_pluto_main->mysql_query_result(sSqlLights)) && result_set_lights.r->row_count );
	if( !bHasSensors && !bHasLights )
		return true;

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->m_pOH_Orbiter=NULL;  // The user isn't responding.  Ask on all orbiters
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	// Be sure the user specified the related cameras and lights
	if( pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_sPK_Device_Relations_For_Creat_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
	{
		string sOptions = (bHasSensors && bHasLights ? "3" : (bHasLights ? "1" : "2"));
		if( pPnpQueueEntry->m_pOH_Orbiter )
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera SCREEN_Sensors_Viewed_By_Camera(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device, pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
				sOptions,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Sensors_Viewed_By_Camera);
		}
		else
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera_DL SCREEN_Sensors_Viewed_By_Camera_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device, m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,
				sOptions,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Sensors_Viewed_By_Camera_DL);
		}
		return false;
	}
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreate_CaptureCard(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	bool bUseAutoSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Automatically_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

	if( bUseAutoSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, 
			 StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_Auto_Configure_TV_CONST);
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified this option
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->m_pOH_Orbiter=NULL;  // The user isn't responding.  Ask on all orbiters
	
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter )
	{
		DCE::SCREEN_AutoConfigure_TV SCREEN_AutoConfigure_TV(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_AutoConfigure_TV);
	}
	else
	{
		DCE::SCREEN_AutoConfigure_TV_DL SCREEN_AutoConfigure_TV_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_AutoConfigure_TV_DL);
	}
	return false;
}
