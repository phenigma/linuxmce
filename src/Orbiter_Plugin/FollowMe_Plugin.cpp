/*
 Orbiter_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "FollowMe_Plugin.h"
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DeviceData_Router.h"

void FollowMe_Plugin::HandleFollowMe(class DeviceData_Router *pDevice_PriorDetector,char Type, class OH_Orbiter *pOH_Orbiter)
{
g_pPlutoLogger->Write(LV_WARNING,"Handle follow me type %c orbiter %d",Type,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	if( !pOH_Orbiter->m_pDevice_CurrentDetected->m_pRoom )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter is in follow-me mode, but detected device %d isn't in a room",pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device);
		return;
	}
	if( pDevice_PriorDetector && pDevice_PriorDetector->m_pRoom )
		FollowMe_LeftRoom(pOH_Orbiter,pDevice_PriorDetector->m_pRoom, pOH_Orbiter->m_pDevice_CurrentDetected->m_pRoom);
	if( !m_bOneAtATime )  // Some things, like media, climate, etc. can only have 1 thing going.  Others like Telecom don't have that restriction
		FollowMe_EnteredRoom(pOH_Orbiter,pDevice_PriorDetector ? pDevice_PriorDetector->m_pRoom : NULL, pOH_Orbiter->m_pDevice_CurrentDetected->m_pRoom);
}

void FollowMe_Plugin::CancelPendingMovesByType(class OH_Orbiter *pOH_Orbiter,char cType)
{
}

