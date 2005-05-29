/*
 MediaHandlerInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "EntertainArea.h"
#include "MediaPosition.h"
#include "Media_Plugin.h"

#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Define_AttributeType.h"


MediaStream::MediaStream( class MediaHandlerInfo *pMediaHandlerInfo, MediaDevice *pMediaDevice, int PK_DesignObj_Remote, int PK_Users, enum SourceType sourceType, int iStreamID )
{
	m_dwPK_Disc=m_discid=0;
    m_iStreamID = iStreamID;
    m_pMediaHandlerInfo=pMediaHandlerInfo;
    m_iPK_DesignObj_Remote=PK_DesignObj_Remote;
	m_iPK_DesignObj_RemoteOSD=0;
    m_iPK_Users=PK_Users;
    m_eSourceType=sourceType;
    m_bResume=m_bPlaying=false;
    m_pMediaPosition=NULL;
    m_pOH_Orbiter_StartedMedia=NULL;
    m_pPictureData=NULL;
    m_iPictureSize=0;
    m_iOrder=0;
    m_iDequeMediaFile_Pos=0;
    m_iPK_Playlist=0;
    m_sPlaylistName="";
	m_bIsMovable = true; // by default all the media can move but not always.
	m_tTime = time(NULL);
	m_tTime_Parked = 0;
	m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
	m_iRepeat = 0;

    m_pMediaDevice_Source=pMediaDevice;

    if ( m_pMediaHandlerInfo ) // If this stream is a "valid stream only"
        m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream[m_iStreamID] = this;

    if( !m_pMediaDevice_Source || !m_pMediaHandlerInfo )
        g_pPlutoLogger->Write( LV_CRITICAL, "Media stream is invalid because of NULL pointers! %p %p", m_pMediaDevice_Source, m_pMediaHandlerInfo);

	m_pMediaDevice_Source->m_iLastPlaybackSpeed = 1000;
g_pPlutoLogger->Write( LV_STATUS, "create Mediastream %p on stream id: %d type %d", this, m_iStreamID, m_iPK_MediaType );
g_pPlutoLogger->Write( LV_STATUS, "Mediastream mapEntArea size %d", m_mapEntertainArea.size( ) );
}

bool MediaStream::isMovable()
{
	return m_bIsMovable;
}

void MediaStream::setIsMovable(bool bIsMovable)
{
	m_bIsMovable = bIsMovable;
}

void MediaStream::SetPlaylistPosition(int position)
{
g_pPlutoLogger->Write(LV_STATUS,"SetPlaylistPosition with position %d dequepos %d size %d m_pmediaposition %p",
	position,m_iDequeMediaFile_Pos,(int) m_dequeMediaFile.size(),m_pMediaPosition);

	if ( (unsigned int)position == m_iDequeMediaFile_Pos )
		return;

	if ( position < 0 )
		m_iDequeMediaFile_Pos = m_dequeMediaFile.size() - 1;
	else
		m_iDequeMediaFile_Pos = position;

    if ( m_iDequeMediaFile_Pos >= m_dequeMediaFile.size() )
        m_iDequeMediaFile_Pos = 0;

	// reset the stream position
	if ( m_pMediaPosition )
		m_pMediaPosition->Reset();
}


void MediaStream::ChangePositionInPlaylist(int iHowMuch)
{
g_pPlutoLogger->Write(LV_STATUS,"ChangePositionInPlaylist with position %d how much %d",
	m_iDequeMediaFile_Pos,iHowMuch);

	SetPlaylistPosition(m_iDequeMediaFile_Pos + iHowMuch);
}

string MediaStream::GetFilenameToPlay(string sNoFilesFileName )
{
g_pPlutoLogger->Write(LV_STATUS,"GetFilenameToPlay called with size: %d pos: %d",
	(int) m_dequeMediaFile.size(), m_iDequeMediaFile_Pos);

    if ( m_dequeMediaFile.size() == 0 )
        return sNoFilesFileName;

    // ensure the playlist position is accurate
    SetPlaylistPosition(m_iDequeMediaFile_Pos);

    return m_dequeMediaFile[m_iDequeMediaFile_Pos]->FullyQualifiedFile();
}

void MediaStream::DumpPlaylist()
{
    deque<MediaFile *>::iterator itPlaylist;

    itPlaylist = m_dequeMediaFile.begin();
    g_pPlutoLogger->Write(LV_STATUS, "Position: %d", m_iDequeMediaFile_Pos);
    while ( itPlaylist != m_dequeMediaFile.end() )
    {
        g_pPlutoLogger->Write(LV_STATUS, "File%c%d: %s",
                    (m_iDequeMediaFile_Pos == (unsigned int)(itPlaylist - m_dequeMediaFile.begin())) ? '*' : ' ',
                    itPlaylist - m_dequeMediaFile.begin(),
                    (*itPlaylist)->FullyQualifiedFile().c_str());
        itPlaylist++;
    }
}

void MediaStream::ClearPlaylist()
{
    m_iDequeMediaFile_Pos = 0;
    MediaAttributes::PurgeDequeMediaFile(m_dequeMediaFile);
    m_dequeMediaFile.clear();
}

bool MediaStream::CanPlayMore()
{
    g_pPlutoLogger->Write(LV_WARNING, "HaveMoreInQueue: position %d, size: %d, result %d", m_iDequeMediaFile_Pos, m_dequeMediaFile.size(), m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1));
    DumpPlaylist();

    return m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1);
}

MediaStream::~MediaStream( )
{
    if ( m_pMediaHandlerInfo )
        m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream_Remove( m_iStreamID );
    ClearPlaylist();
}

bool MediaStream::ProcessJumpPosition(string sJumpSpecification)
{
    switch ( sJumpSpecification[0] )
    {
        case '-':
            ChangePositionInPlaylist(-atoi(sJumpSpecification.substr(1).c_str()));
            break;
        case '+':
            ChangePositionInPlaylist(atoi(sJumpSpecification.substr(1).c_str()));
            break;
        default:
            SetPlaylistPosition(atoi(sJumpSpecification.c_str()));
            break;
    }

	return true;
}

void MediaStream::DeleteEntryFromPlaylist(int position)
{
	if ( position >= 0 && position < m_dequeMediaFile.size() )
		m_dequeMediaFile.erase(m_dequeMediaFile.begin() + position);

	DumpPlaylist();
}

void MediaStream::MoveEntryInPlaylist(int position, int displacement)
{
	if ( position >= 0 && position < m_dequeMediaFile.size() &&
		 position + displacement >= 0 && position + displacement < m_dequeMediaFile.size() )
	{
		MediaFile *pTargetFile = m_dequeMediaFile[position];
		m_dequeMediaFile[position] = m_dequeMediaFile[position + displacement];
		m_dequeMediaFile[position + displacement ] = pTargetFile;

		if ( m_iDequeMediaFile_Pos == position )
			m_iDequeMediaFile_Pos += displacement;
	}

	DumpPlaylist();
}

bool MediaStream::ContainsVideo()
{
	return m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_Pictures_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_LiveTV_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_DVD_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_VideoTape_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_LaserDisc_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_Game_CONST;
}


bool MediaStream::OrbiterIsOSD(int PK_Orbiter)
{
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest &&
				pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD &&
				pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device==PK_Orbiter )
			return true;
	}
	return false;
}

int MediaStream::SpecialOsdScreen()
{
	switch( m_iPK_MediaType )
	{
	case MEDIATYPE_pluto_DVD_CONST:
		return DESIGNOBJ_dvd_full_screen_CONST;
	case MEDIATYPE_pluto_LiveTV_CONST:
		return DESIGNOBJ_pvr_full_screen_CONST;
	case MEDIATYPE_pluto_StoredVideo_CONST:
		return DESIGNOBJ_storedvideos_full_screen_CONST;
	}

	return 0;
}

string MediaStream::GetAllOSD()
{
	string s;
	for( map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest && 
			pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD )
		{
			if( s.size() )
				s += ",";
			s += StringUtils::itos(pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device);
		}
	}
	return s;
}

void MediaStream::GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices)
{
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest )
			(*pmapMediaDevices)[ pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device ] = pEntertainArea->m_pMediaDevice_ActiveDest;
	}
}

void MediaStream::UpdateDescriptionsFromAttributes()
{
	Media_Plugin *pMedia_Plugin = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin;
	for(size_t s=0;s<m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = m_dequeMediaFile[s];
		int PK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Song_CONST);
		if( PK_Attribute )
		{
			Row_Attribute *pRow_Attribute = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
			if( pRow_Attribute ) // should always be valid
				pMediaFile->m_sDescription = pMedia_Plugin->m_pMediaAttributes->GetPrintableName(pRow_Attribute);
		}
	}

	if( m_dequeMediaFile.size() && m_iDequeMediaFile_Pos<m_dequeMediaFile.size() )
	{
		MediaFile *pMediaFile = m_dequeMediaFile[m_iDequeMediaFile_Pos];
		m_sMediaDescription = pMediaFile->m_sDescription;
	}
	else
	{
		m_sMediaDescription = "";
		Row_Attribute *pRow_Attribute_Group = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(m_mapPK_Attribute_Find(ATTRIBUTETYPE_Group_CONST));
		Row_Attribute *pRow_Attribute_Album = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST));

		if( pRow_Attribute_Group )
			m_sMediaDescription = pMedia_Plugin->m_pMediaAttributes->GetPrintableName(pRow_Attribute_Group);

		if( pRow_Attribute_Album && pRow_Attribute_Group )
			m_sMediaDescription += " / " + pMedia_Plugin->m_pMediaAttributes->GetPrintableName(pRow_Attribute_Album);
		else if( pRow_Attribute_Album )
			m_sMediaDescription = pMedia_Plugin->m_pMediaAttributes->GetPrintableName(pRow_Attribute_Album);
	}
}
