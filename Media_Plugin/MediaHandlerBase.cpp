/*
 Media_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "MediaHandlerBase.h"
#include "Media_Plugin.h"

#include <iostream>
using namespace std;
using namespace DCE;

void MediaHandlerBase::GetRenderDevices(MediaStream *pMediaStream, map<int,MediaDevice *> *pmapMediaDevice)
{
	if( pMediaStream && pMediaStream->m_pDeviceData_Router_Source )
	{
		g_pPlutoLogger->Write(LV_STATUS, " Setting the device data source: to %d (%s)", pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device, pMediaStream->m_pDeviceData_Router_Source->m_sDescription.c_str());
		(*pmapMediaDevice)[pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device] = m_pMedia_Plugin->m_mapMediaDevice_Find(pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device);
	}
}
