/*
	Generic_NonPluto_Media

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/**
*
* @file Generic_NonPluto_Media.h
* @brief source file for the Generic_NonPluto_Media class
* @author
*
*/

#include "Generic_NonPluto_Media.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllCommandsRequests.h"

Generic_NonPluto_Media::Generic_NonPluto_Media(Media_Plugin *pMedia_Plugin)
{
	m_pMedia_Plugin=pMedia_Plugin;
}

Generic_NonPluto_Media::~Generic_NonPluto_Media()
{

}


class MediaStream *Generic_NonPluto_Media::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}
/*
	if ( ! pMediaDevice && (pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea)) == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
		return NULL;
	}
*/
	g_pPlutoLogger->Write(LV_STATUS, "Selected device (%d) as playback device!", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);

	MediaStream *pMediaStream
		= new MediaStream( pMediaHandlerInfo,
							pMediaDevice,
							pMediaHandlerInfo->m_iPK_DesignObj,
							iPK_Users, st_RemovableMedia, StreamID );

	pMediaStream->m_sMediaDescription = pMediaDevice->m_pDeviceData_Router->m_sDescription;
	return pMediaStream;
}

bool Generic_NonPluto_Media::StartMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write( LV_STATUS, "Starting media stream playback--sending command, waiting for response" );

	m_pMedia_Plugin->MediaInfoChanged( pMediaStream );
	return true;
}

bool Generic_NonPluto_Media::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Stopping media in Generic_NonPluto_Media!");

	return true;
}

/**
* @brief Called by the Media Plugin when we need to move media around in entertainment areas. The media plugin will pass 3 lists:
*          < listStart: the list on which media needs to be started. >
*          < listStop: the list on which media needs to be stopped >
*          < listChange: the list on which media needs to be changed (if we move the media from one place to another it might be possible that the actual source of the stream to change). >
*/
bool Generic_NonPluto_Media::MoveMedia(class MediaStream *pMediaStream, list<EntertainArea*> &listStart, list<EntertainArea *> &listStop, list<EntertainArea *> &listChange)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	return true;
}
/*
MediaDevice *Generic_NonPluto_Media::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Xine_Player_CONST);

	if ( pMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Could not find a Xine Player device (with device template id: %d) in the entertainment area: %d. Looking for a squeeze box.",
				DEVICETEMPLATE_Xine_Player_CONST,
				pEntertainArea->m_iPK_EntertainArea);

		pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_SqueezeBox_Player_CONST);

		if ( pMediaDevice == NULL )
		{
			g_pPlutoLogger->Write(LV_WARNING, "No squeeze box device (device template id: %d) was found in the entertainment area: %d. Ignoring this ent area ",
				DEVICETEMPLATE_SqueezeBox_Player_CONST,
				pEntertainArea->m_iPK_EntertainArea);

			return NULL;
		}
	}

	return pMediaDevice;
}

void Generic_NonPluto_Media::GetRenderDevices(MediaStream *pMediaStream, map<int,MediaDevice *> *pmapMediaDevice)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	XineMediaStream *pXineMediaStream;

	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Generic_NonPluto_Media::GetRenderDevices() ")) == NULL )
		return;

	pXineMediaStream->GetRenderDevices(pmapMediaDevice);
}
*/
