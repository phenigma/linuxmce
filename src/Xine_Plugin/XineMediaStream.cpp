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
		if ( m_mapEntertainmentAreasToDevices.size() > 1 )
			return true;

		if ( m_mapEntertainmentAreasToDevices.size() == 0 )
			return false;

		if ( (*m_mapEntertainmentAreasToDevices.begin()).second->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SqueezeBox_Player_CONST)
			return true;

		return false;
	}

	XineMediaStream::XineMediaPosition *XineMediaStream::GetMediaPosition()
	{
		if ( m_pMediaPosition == NULL )
			m_pMediaPosition = new XineMediaPosition();

		return static_cast<XineMediaStream::XineMediaPosition*>(m_pMediaPosition);
	}

	MediaDevice *XineMediaStream::GetPlaybackDeviceForEntArea(int entAreaId)
	{
		map<int, MediaDevice*>::const_iterator itPlaybackDevices;

		if ( (itPlaybackDevices = m_mapEntertainmentAreasToDevices.find(entAreaId)) != m_mapEntertainmentAreasToDevices.end() )
			return (*itPlaybackDevices).second;

		g_pPlutoLogger->Write(LV_WARNING, "XineMediaStream::GetPlaybackDeviceForEntArea(): Could not find a playback device for ent area: %d!", entAreaId);
		return NULL;
	}

	void XineMediaStream::SetPlaybackDeviceForEntArea(int entAreaId, MediaDevice *pMediaDevice)
	{
		if ( pMediaDevice == NULL )
			m_mapEntertainmentAreasToDevices.erase(entAreaId);
		else
			m_mapEntertainmentAreasToDevices[entAreaId] = pMediaDevice;
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
		m_iSavedPosition = 0;
		m_iTotalStreamTime = 0;
		m_sSavedPosition = "";
	}

	string XineMediaStream::XineMediaPosition::GetID()
	{
		return "XineMediaStream::XineMediaPosition class";
	}

	void XineMediaStream::GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices)
	{
		map<int, MediaDevice*> &mapRenderDevices = *pmapMediaDevices;
		map<int, MediaDevice*>::const_iterator itRenderingDevices;

		itRenderingDevices = m_mapEntertainmentAreasToDevices.begin();

		mapRenderDevices.clear();
		while ( itRenderingDevices != m_mapEntertainmentAreasToDevices.end() )
		{
			mapRenderDevices[(*itRenderingDevices).second->m_pDeviceData_Router->m_dwPK_Device] = (*itRenderingDevices).second;
			itRenderingDevices++;
		}
	}

	bool XineMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat!=-1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
