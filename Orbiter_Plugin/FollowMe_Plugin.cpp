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

void FollowMe_Plugin::HandleFollowMe(DeviceData_Router *pDevice_PriorDetector,OH_Orbiter *pOH_Orbiter)
{
	ExecuteFollowMe(pOH_Orbiter,pDevice_PriorDetector ? pDevice_PriorDetector->m_pRoom : NULL, pOH_Orbiter->m_pDevice_CurrentDetected->m_pRoom);
}

