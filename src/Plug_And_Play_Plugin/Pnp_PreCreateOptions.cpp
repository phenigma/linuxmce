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

	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Storage_Devices_CONST) )
		return OkayToCreateDevice_NetworkStorage(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		return OkayToCreate_Cameras(pPnpQueueEntry,pRow_DeviceTemplate);

	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_AlarmPanel(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// The user must answer 2 questions before we create a network storage device: 1) use it automatically, 2) use our directory structure.  See which questions were answered
	bool bUseAutoSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Automatically_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
	bool bUseDirectorySpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Pluto_Directory_Structure_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage %d/%d",(int) bUseAutoSpecified,(int) bUseDirectorySpecified);
#endif
	if( bUseAutoSpecified && bUseDirectorySpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NAS_Options_CONST);
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified both options
	}

	if( bUseAutoSpecified && !bUseDirectorySpecified && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
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
	g_pPlutoLogger->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_Cameras queue %d",
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

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->m_pOH_Orbiter=NULL;  // The user isn't responding.  Ask on all orbiters
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	// We just store the relations as lights\tsensors in device data 0 as a temporary holding place.  Post Create options will fix this up
	if( pPnpQueueEntry->m_mapPK_DeviceData.find(0)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
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