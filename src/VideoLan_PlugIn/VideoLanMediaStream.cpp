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

namespace DCE {

	VideoLanMediaStream::VideoLanMediaStream(
							class VideoLan_PlugIn *pVideoLanPlugin,
							class MediaHandlerInfo *pMediaHandlerInfo,
							MediaDevice *pMediaDevice,
							int PK_DesignObj_Remote,
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, pMediaDevice, PK_DesignObj_Remote, PK_Users,sourceType, iStreamID)
	{
		m_bIsStreaming = 0;
	}

	VideoLanMediaStream::~VideoLanMediaStream()
	{
		delete m_pMediaPosition;
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

	VideoLanMediaStream::VideoLanMediaPosition *VideoLanMediaStream::GetMediaPosition()
	{
		if ( m_pMediaPosition == NULL )
			m_pMediaPosition = new VideoLanMediaPosition();

		return static_cast<VideoLanMediaStream::VideoLanMediaPosition*>(m_pMediaPosition);
	}

	VideoLanMediaStream::VideoLanMediaPosition::VideoLanMediaPosition()
	{
		Reset();
	}

	VideoLanMediaStream::VideoLanMediaPosition::~VideoLanMediaPosition()
	{
		// HACK: No-op dtor. But to avoid gcc warnings.
	}

	void VideoLanMediaStream::VideoLanMediaPosition::Reset()
	{
g_pPlutoLogger->Write(LV_STATUS,"VideoLanMediaPosition reset");

		m_iSavedPosition = 0;
		m_iTotalStreamTime = 0;
		m_sSavedPosition = "";
	}

	string VideoLanMediaStream::VideoLanMediaPosition::GetID()
	{
		return "VideoLanMediaStream::VideoLanMediaPosition class";
	}

	bool VideoLanMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
