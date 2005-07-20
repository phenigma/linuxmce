//
// C++ Implementation: VideoLan_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "VideoLanMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DesignObj.h"

namespace DCE {

	VideoLanMediaStream::VideoLanMediaStream(
							class VideoLan_PlugIn *pVideoLanPlugin,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_bIsStreaming = 0;
	}

	VideoLanMediaStream::~VideoLanMediaStream()
	{
	}

	int VideoLanMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_VIDEOLAN;
	}

	bool VideoLanMediaStream::isStreaming()
	{
		return m_bIsStreaming;
	}

	void VideoLanMediaStream::setIsStreaming(bool isStreaming)
	{
		m_bIsStreaming = isStreaming;
	}

	bool VideoLanMediaStream::ShouldUseStreaming()
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

	bool VideoLanMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}

	int VideoLanMediaStream::SpecialOsdScreen()
	{
		switch( m_iPK_MediaType )
		{
		case MEDIATYPE_pluto_DVD_CONST:
			return DESIGNOBJ_vlc_dvd_full_screen_CONST;
		}

		return MediaStream::SpecialOsdScreen();
	}

};
