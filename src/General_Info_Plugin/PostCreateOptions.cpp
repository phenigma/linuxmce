/*
 PostCreateOptions

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "PostCreateOptions.h"
#include "DeviceData_Base.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "Command_Impl.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_FloorplanObjectType.h"
extern class Command_Impl *g_pCommand_Impl;

PostCreateOptions::PostCreateOptions(Database_pluto_main *pDatabase_pluto_main,Router *pRouter)
{
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRouter=pRouter;
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
}

void PostCreateOptions::PostCreateDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	DeviceCategory *pDeviceCategory = m_pRouter->m_mapDeviceCategory_Find(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
	if( !pDeviceCategory )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"PostCreateOptions::PostCreateDevice category for Device %d is invalid",pRow_Device->PK_Device_get());
		return;
	}

	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Network_Storage_CONST) )
		PostCreateDevice_NetworkStorage(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		PostCreateDevice_Cameras(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Security_Device_CONST) )
		PostCreateSecurityDevice(pRow_Device,pOH_Orbiter);
}

void PostCreateOptions::PostCreateDevice_AlarmPanel(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
}

void PostCreateOptions::PostCreateSecurityDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_FloorplanObjectType=0;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
	if( pRow_Device_DeviceData )
		PK_FloorplanObjectType = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());

	int iPK_DeviceTemplate=pRow_Device->FK_DeviceTemplate_get();
	string sDefaultSecuritySetting;
	if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Door_Sensor_CONST )
		sDefaultSecuritySetting = "1,0,N1,1,1,2,N1"; // Monitor mode, security alert when armed, announcement when entertaining
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_WINDOW_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Window_Sensor_CONST )
		sDefaultSecuritySetting = "0,0,N1,2,2,0,N1"; // Make an announcement when the user is at home, security breach when away
	else if( iPK_DeviceTemplate==DEVICETEMPLATE_Glass_Break_Sensor_CONST )
		sDefaultSecuritySetting = "0,2,N1,N1,N1,2,N1"; // Always announcement or trigger an alarm
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Motion_Detector_CONST )
		sDefaultSecuritySetting = "0,0,N1,0,0,0,N1"; // Only active when away
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_SMOKE_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Smoke_Detector_CONST )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_AIRQUALITY_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Air_Quality_Sensor_CONST )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm

	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Alert_CONST,sDefaultSecuritySetting);
}

void PostCreateOptions::PostCreateDevice_NetworkStorage(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_NetworkStorage device  %d template %d",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get());
#endif

	Command_Impl *pCommand_Impl_GIP = m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP ) // Should always be there
	{
		DCE::CMD_Check_Mounts CMD_Check_Mounts(g_pCommand_Impl->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		g_pCommand_Impl->SendCommand(CMD_Check_Mounts);
	}
}

void PostCreateOptions::PostCreateDevice_Cameras(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_Cameras device  %d template %d",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get());
#endif
	string sSQL = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Security_Device_CONST) + " LIMIT 1";

	PlutoSqlResult result_set;
	if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) && result_set.r->row_count )
	{
		if( pOH_Orbiter )
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera SCREEN_Sensors_Viewed_By_Camera(g_pCommand_Impl->m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
				pRow_Device->PK_Device_get());
			g_pCommand_Impl->SendCommand(SCREEN_Sensors_Viewed_By_Camera);
		}
		else
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera_DL SCREEN_Sensors_Viewed_By_Camera_DL(g_pCommand_Impl->m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get(),
				pRow_Device->PK_Device_get());
			g_pCommand_Impl->SendCommand(SCREEN_Sensors_Viewed_By_Camera_DL);
		}
	}
}
