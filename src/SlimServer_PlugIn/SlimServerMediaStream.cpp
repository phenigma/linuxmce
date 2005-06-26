//
// C++ Implementation: SlimServer_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SlimServerMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	SlimServerMediaStream::SlimServerMediaStream(
							class SlimServer_PlugIn *pSlimServerPlugin,
							class MediaHandlerInfo *pMediaHandlerInfo,
							MediaDevice *pMediaDevice,
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_bIsStreaming = 0;
	}

	SlimServerMediaStream::~SlimServerMediaStream()
	{
		delete m_pMediaPosition;
	}

	int SlimServerMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_SLIMSERVER;
	}

	bool SlimServerMediaStream::isStreaming()
	{
		return m_bIsStreaming;
	}

	void SlimServerMediaStream::setIsStreaming(bool isStreaming)
	{
		m_bIsStreaming = isStreaming;
	}

	bool SlimServerMediaStream::ShouldUseStreaming()
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

	SlimServerMediaStream::SlimServerMediaPosition *SlimServerMediaStream::GetMediaPosition()
	{
		if ( m_pMediaPosition == NULL )
			m_pMediaPosition = new SlimServerMediaPosition();

		return static_cast<SlimServerMediaStream::SlimServerMediaPosition*>(m_pMediaPosition);
	}

	SlimServerMediaStream::SlimServerMediaPosition::SlimServerMediaPosition()
	{
		Reset();
	}

	SlimServerMediaStream::SlimServerMediaPosition::~SlimServerMediaPosition()
	{
		// HACK: No-op dtor. But to avoid gcc warnings.
	}

	void SlimServerMediaStream::SlimServerMediaPosition::Reset()
	{
g_pPlutoLogger->Write(LV_STATUS,"SlimServerMediaPosition reset");

		m_iSavedPosition = 0;
		m_iTotalStreamTime = 0;
		m_sSavedPosition = "";
	}

	string SlimServerMediaStream::SlimServerMediaPosition::GetID()
	{
		return "SlimServerMediaStream::SlimServerMediaPosition class";
	}

	bool SlimServerMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
