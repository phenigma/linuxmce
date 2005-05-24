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
							class MediaHandlerInfo *pMediaHandlerInfo,
							MediaDevice *pMediaDevice,
							int PK_DesignObj_Remote,
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, pMediaDevice, PK_DesignObj_Remote, PK_Users,sourceType, iStreamID)
	{
		m_bIsStreaming = 0;
	}

	XineMediaStream::~XineMediaStream()
	{
		delete m_pMediaPosition;
	}

	int XineMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_XINE;
	}

	bool XineMediaStream::isStreaming()
	{
		return m_bIsStreaming;
	}

	void XineMediaStream::setIsStreaming(bool isStreaming)
	{
		m_bIsStreaming = isStreaming;
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

	XineMediaStream::XineMediaPosition *XineMediaStream::GetMediaPosition()
	{
		if ( m_pMediaPosition == NULL )
			m_pMediaPosition = new XineMediaPosition();

		return static_cast<XineMediaStream::XineMediaPosition*>(m_pMediaPosition);
	}

	XineMediaStream::XineMediaPosition::XineMediaPosition()
	{
		Reset();
	}

	XineMediaStream::XineMediaPosition::~XineMediaPosition()
	{
		// HACK: No-op dtor. But to avoid gcc warnings.
	}

	void XineMediaStream::XineMediaPosition::Reset()
	{
g_pPlutoLogger->Write(LV_STATUS,"XineMediaPosition reset");

		m_iSavedPosition = 0;
		m_iTotalStreamTime = 0;
		m_sSavedPosition = "";
	}

	string XineMediaStream::XineMediaPosition::GetID()
	{
		return "XineMediaStream::XineMediaPosition class";
	}

	bool XineMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
