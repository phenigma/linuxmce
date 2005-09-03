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
#include "pluto_main/Table_MediaType_DesignObj.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"

RemoteControlSet::RemoteControlSet(Row_MediaType_DesignObj *pRow_MediaType_DesignObj)
{
	m_iPK_DesignObj_Remote=pRow_MediaType_DesignObj->FK_DesignObj_get();
	m_iPK_DesignObj_Remote_Popup=pRow_MediaType_DesignObj->FK_DesignObj_Popup_get();
	m_iPK_DesignObj_FileList=pRow_MediaType_DesignObj->FK_DesignObj_FileList_get();
	m_iPK_DesignObj_FileList_Popup=pRow_MediaType_DesignObj->FK_DesignObj_FileList_Popup_get();
}

RemoteControlSet::RemoteControlSet(Row_DeviceTemplate_MediaType_DesignObj *pRow_DeviceTemplate_MediaType_DesignObj)
{
	m_iPK_DesignObj_Remote=pRow_DeviceTemplate_MediaType_DesignObj->FK_DesignObj_get();
	m_iPK_DesignObj_Remote_Popup=pRow_DeviceTemplate_MediaType_DesignObj->FK_DesignObj_Popup_get();
	m_iPK_DesignObj_FileList=pRow_DeviceTemplate_MediaType_DesignObj->FK_DesignObj_FileList_get();
	m_iPK_DesignObj_FileList_Popup=pRow_DeviceTemplate_MediaType_DesignObj->FK_DesignObj_FileList_Popup_get();
}

MediaStream::MediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, MediaDevice *pMediaDevice, int PK_Users, enum SourceType sourceType, int iStreamID )
{
	m_dwPK_Disc=m_discid=0;
    m_iStreamID = iStreamID;
    m_pMediaHandlerInfo=pMediaHandlerInfo;
	m_iPK_DesignObj_RemoteOSD=0;
	m_pRemoteControlSet=NULL;
	m_IdentifiedPriority=0;
	m_iPK_MediaProvider=iPK_MediaProvider;
	m_bPlugInWillSetDescription=false;

    m_iPK_Users=PK_Users;
    m_eSourceType=sourceType;
    m_bResume=m_bPlaying=false;
    m_pOH_Orbiter_StartedMedia=NULL;
    m_pPictureData=NULL;
    m_iPictureSize=0;
    m_iOrder=0;
    m_iDequeMediaFile_Pos=0; m_iDequeMediaTitle_Pos=m_iDequeMediaSection_Pos=-1;

	m_iPK_Playlist=0;
    m_sPlaylistName="";
	m_bIsMovable = true; // by default all the media can move but not always.
	m_tTime = time(NULL);
	m_tTime_Parked = 0;
	m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
	m_iRepeat = 0;
	m_bStopped = false;

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
g_pPlutoLogger->Write(LV_STATUS,"SetPlaylistPosition with position %d dequepos %d size %d",
	position,m_iDequeMediaFile_Pos,(int) m_dequeMediaFile.size());

	if ( (unsigned int)position == m_iDequeMediaFile_Pos )
		return;

	if ( position < 0 )
		m_iDequeMediaFile_Pos = m_dequeMediaFile.size() - 1;
	else
		m_iDequeMediaFile_Pos = position;

    if ( m_iDequeMediaFile_Pos >= m_dequeMediaFile.size() )
        m_iDequeMediaFile_Pos = 0;
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
	for(size_t s=0;s<m_dequeMediaSection.size();++s)
		if( m_dequeMediaSection[s] )
			delete m_dequeMediaSection[s];

	for(size_t s=0;s<m_dequeMediaTitle.size();++s)
		if( m_dequeMediaTitle[s] )
			delete m_dequeMediaTitle[s];

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


bool MediaStream::OrbiterIsOSD(int PK_Orbiter,EntertainArea **ppEntertainArea)
{
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest &&
				pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD &&
				pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device==PK_Orbiter )
		{
			if( ppEntertainArea )
				(*ppEntertainArea) = pEntertainArea;
			return true;
		}
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

void MediaStream::UpdateDescriptions(bool bAllFiles,MediaFile *pMediaFile_In)
{
	if( m_bPlugInWillSetDescription )
		return;

	m_sMediaDescription="";
	Media_Plugin *pMedia_Plugin = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin;

	if( m_iPK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST || m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST || m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
	{
		MediaFile *pMediaFile=pMediaFile_In;
		if( bAllFiles )
		{
			for(size_t s=0;s<m_dequeMediaFile.size();++s)
				UpdateDescriptions(false,m_dequeMediaFile[s]);
			return;
		}
		else if( !pMediaFile && m_iDequeMediaFile_Pos>=0 && m_iDequeMediaFile_Pos<m_dequeMediaFile.size() )
			pMediaFile = m_dequeMediaFile[m_iDequeMediaFile_Pos];

		int PK_Attribute;
		Row_Attribute *pRow_Attribute_Song=NULL,*pRow_Attribute_Album=NULL,*pRow_Attribute_Performer=NULL,*pRow_Attribute_Title=NULL;

		// First try to find attributes for the particular song, otherwise look in the collection
		if( pMediaFile )
		{
			if( !pRow_Attribute_Song && (PK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Song_CONST))!=0 )
				pRow_Attribute_Song = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
			if( !pRow_Attribute_Performer && (PK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Performer_CONST))!=0 )
				pRow_Attribute_Performer = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
			if( !pRow_Attribute_Album && (PK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST))!=0 )
				pRow_Attribute_Album = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);	
			if( !pRow_Attribute_Title && (PK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST))!=0 )
				pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);	
		}

		if( !pRow_Attribute_Performer && (PK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Performer_CONST))!=0 )
			pRow_Attribute_Performer = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
		if( !pRow_Attribute_Album && (PK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST))!=0 )
			pRow_Attribute_Album = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);	
		if( !pRow_Attribute_Title && (PK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST))!=0 )
			pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);	

		if( pRow_Attribute_Performer || pRow_Attribute_Album )
		{
			if( pRow_Attribute_Performer )
			{
				m_sMediaDescription = pRow_Attribute_Performer->Name_get();
				if( pRow_Attribute_Album )
					m_sMediaDescription += "\n" + pRow_Attribute_Album->Name_get();
			}
			else
				m_sMediaDescription = pRow_Attribute_Album->Name_get();
			if( pRow_Attribute_Song )
				m_sSectionDescription = pRow_Attribute_Song->Name_get();
			else
				m_sSectionDescription = pMediaFile->m_sFilename;

			if( pMediaFile )
				pMediaFile->m_sDescription = m_sSectionDescription;
		}
		else if( pRow_Attribute_Song )
				m_sMediaDescription = pRow_Attribute_Song->Name_get();
		else if( pRow_Attribute_Title )
				m_sMediaDescription = pRow_Attribute_Title->Name_get();
		else if( pMediaFile && pMediaFile->m_sFilename.size()>6 && pMediaFile->m_sFilename.substr(0,6)=="cdda:/" )
			m_sMediaDescription = pMediaFile->m_sFilename.substr(6);
		else if( pMediaFile )
			m_sMediaDescription = pMediaFile->m_sFilename;

		if( pMediaFile && pMediaFile->m_sDescription.size()==0 )
			pMediaFile->m_sDescription = pMediaFile->m_sFilename;

		if( m_sMediaDescription.size()==0 )
		{
			if( m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST || !pMediaFile || pMediaFile->m_sFilename.size()==0 )
				m_sMediaDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%>";
			else
				m_sMediaDescription = pMediaFile->m_sFilename;
		}
	}
	else if( m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
	{
		MediaTitle *pMediaTitle=NULL;
		MediaSection *pMediaSection=NULL;
		if( m_iDequeMediaTitle_Pos>=0 && m_iDequeMediaTitle_Pos<m_dequeMediaTitle.size() )
			pMediaTitle = m_dequeMediaTitle[m_iDequeMediaTitle_Pos];
		if( pMediaTitle && m_iDequeMediaSection_Pos>=0 && m_iDequeMediaSection_Pos<pMediaTitle->m_dequeMediaSection.size() )
			pMediaSection=pMediaTitle->m_dequeMediaSection[m_iDequeMediaSection_Pos];
		else if( m_iDequeMediaSection_Pos>=0 && m_iDequeMediaSection_Pos<m_dequeMediaSection.size() )
			pMediaSection=m_dequeMediaSection[m_iDequeMediaSection_Pos];

		int PK_Attribute;
		Row_Attribute *pRow_Attribute_Title=NULL,*pRow_Attribute_Chapter=NULL;

		if( !pRow_Attribute_Title && pMediaTitle && (PK_Attribute = pMediaTitle->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST))!=0 )
			pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
		if( !pRow_Attribute_Title && (PK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST))!=0 )
			pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
		if( !pRow_Attribute_Chapter && pMediaSection && (PK_Attribute = pMediaSection->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST))!=0 )
			pRow_Attribute_Chapter = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);	

		if( pRow_Attribute_Title )
			m_sMediaDescription = pRow_Attribute_Title->Name_get();
		if( pRow_Attribute_Chapter )
			m_sSectionDescription = pRow_Attribute_Chapter->Name_get();
		else if( m_iDequeMediaTitle_Pos!=-1 && m_iDequeMediaSection_Pos!=-1 )
			m_sSectionDescription = "Title: " + StringUtils::itos(m_iDequeMediaTitle_Pos+1) + "\n"
				+ "Chapter: " + StringUtils::itos(m_iDequeMediaSection_Pos+1);
		else
			m_sSectionDescription = "";

		if( m_sMediaDescription.size()==0 )
		{
			MediaFile *pMediaFile=NULL;
			if( !pMediaFile && m_iDequeMediaFile_Pos>=0 && m_iDequeMediaFile_Pos<m_dequeMediaFile.size() )
				pMediaFile = m_dequeMediaFile[m_iDequeMediaFile_Pos];

			if( pMediaFile && !StringUtils::StartsWith(pMediaFile->m_sFilename,"/dev/") )
				m_sMediaDescription = pMediaFile->m_sFilename;
			else
				m_sMediaDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%>";
		}
	}

	if( m_sMediaDescription.size()==0 && m_dequeMediaFile.size() && m_iDequeMediaFile_Pos<m_dequeMediaFile.size() )
	{
		MediaFile *pMediaFile = m_dequeMediaFile[m_iDequeMediaFile_Pos];
		m_sMediaDescription = pMediaFile->m_sFilename;
	}
}

string MediaStream::GetEntAreasWithout(EntertainArea *pEntertainArea)
{
	string sResult;
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEA = it->second;
		if( pEA!=pEntertainArea )
			sResult += StringUtils::itos(pEA->m_iPK_EntertainArea) + ",";
	}

	return sResult;
}

string MediaStream::GetEntAreasWithout(map<int,class EntertainArea *> *p_mapEntertainArea)
{
	string sResult;
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEA = it->second;
		if( !p_mapEntertainArea || p_mapEntertainArea->find(pEA->m_iPK_EntertainArea)==p_mapEntertainArea->end() )
			sResult += StringUtils::itos(pEA->m_iPK_EntertainArea) + ",";
	}

	return sResult;
}

