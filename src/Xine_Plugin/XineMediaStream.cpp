//
// C++ Implementation: Xine_Plugin
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "XineMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	XineMediaStream::XineMediaStream(
							class Xine_Plugin *pXinePlugin,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
	}

	XineMediaStream::~XineMediaStream()
	{
		if( m_pRemoteControlSet )
		{
			if( m_iPK_DesignObj_Remote_After_Menu )
				m_pRemoteControlSet->m_iPK_DesignObj_Remote=m_iPK_DesignObj_Remote_After_Menu;
			if( m_iPK_DesignObj_RemoteOSD_After_Menu )
				m_iPK_DesignObj_RemoteOSD=m_iPK_DesignObj_RemoteOSD_After_Menu;
			if( m_iPK_DesignObj_Remote_Popup_After_Menu )
				m_pRemoteControlSet->m_iPK_DesignObj_Remote_Popup=m_iPK_DesignObj_Remote_Popup_After_Menu;
		}
	}

	int XineMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_XINE;
	}

	bool XineMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;
		if ( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SqueezeBox_Player_CONST)
			return true;

		return false;
	}

	bool XineMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
