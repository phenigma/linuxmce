/*
 MediaPluginInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaPluginInfo.h"
#include "Media_Plugin.h"

#include "DCERouter/DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional)
{
    for(size_t s=0;s<dAdditional.size();++s)
        dTarget.push_back(dAdditional[s]);
}

MediaPluginInfo::MediaPluginInfo( class MediaPluginBase *pMediaPluginBase, class Command_Impl *pCommand_Impl, int PK_MediaType, int PK_DeviceTemplate, bool bCanJumpPosition, bool bUsesDevices, bool bDontRegister )
{
    m_iPK_DesignObj=0;
    m_pMediaPluginBase=pMediaPluginBase;
    m_pCommand_Impl=pCommand_Impl;
    m_PK_MediaType=PK_MediaType;
    m_bCanJumpPosition=bCanJumpPosition;
    m_bUsesDevices=bUsesDevices;
    m_bUsesRemovableMedia=true;

    if( PK_DeviceTemplate )
    {
        ListDeviceData_Router *pListDeviceData_Router=m_pMediaPluginBase->m_pMedia_Plugin->m_pRouter->m_mapDeviceByTemplate_Find( PK_DeviceTemplate );
        for( ListDeviceData_Router::iterator it=pListDeviceData_Router->begin( );it!=pListDeviceData_Router->end( );++it )
        {
            DeviceData_Router *pDeviceData_Router = *it;
            MediaDevice *pMediaDevice = m_pMediaPluginBase->m_pMedia_Plugin->m_mapMediaDevice_Find( pDeviceData_Router->m_dwPK_Device );
            if( !pMediaDevice )
                g_pPlutoLogger->Write( LV_CRITICAL, "Device %d (%s) isn't in an entertainment area", pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_sDescription.c_str() );
            else
                m_listMediaDevice.push_back( pMediaDevice );
        }
    }

    Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType=NULL;
    if( PK_DeviceTemplate )
    {
        vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
        pMediaPluginBase->m_pMedia_Plugin->m_pDatabase_pluto_main->DeviceTemplate_MediaType_get( )->GetRows(
                DEVICETEMPLATE_MEDIATYPE_FK_DEVICETEMPLATE_FIELD + string( "=" ) + StringUtils::itos( PK_DeviceTemplate ) + " AND " +
                DEVICETEMPLATE_MEDIATYPE_FK_MEDIATYPE_FIELD + "=" + StringUtils::itos( PK_MediaType ), &vectRow_DeviceTemplate_MediaType );

        if( vectRow_DeviceTemplate_MediaType.size( ) ) // there should only be 1 anyway
            pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[0];
    }

    // See if we have information specific to this media type on this device. Otherwise we'll use the defaults for the media type
    if( pRow_DeviceTemplate_MediaType )
    {
        string::size_type pos=0;
        while( true )
        {
            string Extension_list = pRow_DeviceTemplate_MediaType->Extensions_get( );
            string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
            if( !Extension.size( ) )
                break;
            m_listExtensions.push_back( Extension );
        }
    }
    else
    {
        Row_MediaType *pRow_MediaType=m_pMediaPluginBase->m_pMedia_Plugin->m_pDatabase_pluto_main->MediaType_get( )->GetRow( m_PK_MediaType );
        if( pRow_MediaType )
        {
    //todo      m_bUsesRemovableMedia = pRow_MediaType->UsesRemoveableMedia_get( )==1;
    //todo      m_bCanStoreOnServer = pRow_MediaType->CanStoreOnServer_get( )==1;
    // todo     m_bCanBroadcastInHouse = pRow_MediaType->CanBroadcastInHouse_get( )==1;
    //todo hack     m_bIsExternalTransmission = pRow_MediaType->IsExternalTransmission_get( )==1;
            string::size_type pos=0;
            while( true )
            {
                string Extension_list = pRow_MediaType->Extensions_get( );
                string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
                if( !Extension.size( ) )
                    break;
                m_listExtensions.push_back( Extension );
            }
        }
    }

    // Now find all the entertainment areas served by those devices, and put ourselves as a media handler there
    for( list<class MediaDevice *>::iterator itListMD=m_listMediaDevice.begin( );itListMD!=m_listMediaDevice.end( );++itListMD )
    {
        MediaDevice *pMediaDevice = *itListMD;
        for( list<class EntertainArea *>::iterator itEntArea=pMediaDevice->m_listEntertainArea.begin( );itEntArea!=pMediaDevice->m_listEntertainArea.end( );++itEntArea )
        {
            EntertainArea *pEntertainArea = *itEntArea;
            List_MediaPluginInfo *pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_MediaType_Find( m_PK_MediaType );
            if( !pList_MediaPluginInfo )
            {
                pList_MediaPluginInfo = new List_MediaPluginInfo( );
                pEntertainArea->m_mapMediaPluginInfo_MediaType[m_PK_MediaType] = pList_MediaPluginInfo;
            }
            pList_MediaPluginInfo->push_back( this );

            for( list<string>::iterator itExt=m_listExtensions.begin( );itExt!=m_listExtensions.end( );++itExt )
            {
                string Extension = *itExt;

                pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_Extension_Find( Extension );
                if( !pList_MediaPluginInfo )
                {
                    pList_MediaPluginInfo = new List_MediaPluginInfo( );
                    pEntertainArea->m_mapMediaPluginInfo_Extension[Extension] = pList_MediaPluginInfo;
                }
                pList_MediaPluginInfo->push_back( this );
            }
        }
    }
// todo m_pMediaPluginBase->m_pMedia_Plugin->RegisterMediaPlugin( this );
}


MediaStream::MediaStream( class MediaPluginInfo *pMediaPluginInfo, MediaDevice *pMediaDevice, int PK_DesignObj_Remote, int PK_Users, enum SourceType sourceType, int iStreamID )
{
    m_iPK_MediaType = 0; // No media type specified here. The plugin should put the proper media type in here.
    m_iStreamID = iStreamID;
    m_pMediaPluginInfo=pMediaPluginInfo;
    m_iPK_DesignObj_Remote=PK_DesignObj_Remote;
    m_iPK_Users=PK_Users;
    m_eSourceType=sourceType;
    m_bPlaying=false;
    m_pMediaPosition=NULL;
    m_iStoppedAtPosition = 0;
    m_pOH_Orbiter=NULL;
    m_pPictureData=NULL;
    m_iPictureSize=0;
    m_iOrder=0;
    m_iDequeMediaFile_Pos=-1;
    m_iPK_Playlist=0;
    m_sPlaylistName="";

    m_pMediaDevice=pMediaDevice;

    if ( m_pMediaPluginInfo ) // If this stream is a "valid stream only"
        m_pMediaPluginInfo->m_pMediaPluginBase->m_pMedia_Plugin->m_mapMediaStream[m_iStreamID] = this;

    if( !m_pMediaDevice || !m_pMediaPluginInfo )
        g_pPlutoLogger->Write( LV_CRITICAL, "Media stream is invalid because of NULL pointers! %p %p",m_pMediaDevice, m_pMediaPluginInfo);

g_pPlutoLogger->Write( LV_STATUS, "create Mediastream %p on menu id: %d type %d", this, m_iStreamID, m_iPK_MediaType );
g_pPlutoLogger->Write( LV_STATUS, "Mediastream mapea size %d", m_mapEntertainArea.size( ) );
}

void MediaStream::SetPlaylistPosition(int position)
{
    m_iDequeMediaFile_Pos = position;

    if ( m_iDequeMediaFile_Pos >= (int)m_dequeMediaFile.size() )
        m_iDequeMediaFile_Pos = m_dequeMediaFile.size() - 1;

    if ( m_iDequeMediaFile_Pos < 0 )
        m_iDequeMediaFile_Pos = 0;

    m_iStoppedAtPosition = 0; // reset the file pointer also.

    DumpPlaylist();
}


void MediaStream::ChangePositionInPlaylist(int iHowMuch)
{
    SetPlaylistPosition(m_iDequeMediaFile_Pos + iHowMuch);
}

string MediaStream::GetFilenameToPlay(string sNoFilesFileName )
{
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
                    (m_iDequeMediaFile_Pos == itPlaylist - m_dequeMediaFile.begin()) ? '*' : ' ',
                    itPlaylist - m_dequeMediaFile.begin(),
                    (*itPlaylist)->FullyQualifiedFile().c_str());
        itPlaylist++;
    }
}

void MediaStream::ClearPlaylist()
{
    m_iDequeMediaFile_Pos = -1;
    MediaAttributes::PurgeDequeMediaFile(m_dequeMediaFile);
    m_dequeMediaFile.clear();
}

bool MediaStream::HaveMoreInQueue()
{
    g_pPlutoLogger->Write(LV_WARNING, "HaveMoreInQueue: position %d, size: %d, result %d", m_iDequeMediaFile_Pos, m_dequeMediaFile.size(), m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1));
    DumpPlaylist();
    return m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1);
}

MediaStream::~MediaStream( )
{
    if ( m_pMediaPluginInfo )
        m_pMediaPluginInfo->m_pMediaPluginBase->m_pMedia_Plugin->m_mapMediaStream_Remove( m_iStreamID );
    ClearPlaylist();
}

void BoundRemote::UpdateOrbiter( MediaStream *pMediaStream )
{
    // TODO -- Figure out the media information, like track, timecode, picture, etc. For now just update the text object. Also need to update the pictures
//  size_t size; char *pPic = FileUtils::ReadFileIntoBuffer( "/image.jpg", size );
    DCE::CMD_Update_Object_Image CMD_Update_Object_Image( 0, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, m_sPK_DesignObj_GraphicImage, "jpg", pMediaStream->m_pPictureData, pMediaStream->m_iPictureSize, "0" );

    if( m_iPK_Text_Description )
    {
        DCE::CMD_Set_Text CMD_Set_Text( m_pMedia_Plugin->m_dwPK_Device, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "",
            ( pMediaStream ? pMediaStream->m_sMediaDescription : "*no media*" ), m_iPK_Text_Description );
        CMD_Update_Object_Image.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Text.m_pMessage );
    }

    if( m_iPK_Text_Section )
    {
        DCE::CMD_Set_Text CMD_Set_Text( m_pMedia_Plugin->m_dwPK_Device, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "",
            ( pMediaStream ? pMediaStream->m_sSectionDescription : "* no media *" ), m_iPK_Text_Section );
        CMD_Update_Object_Image.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Text.m_pMessage );
    }

    if( m_iPK_Text_Synopsis )
    {
        DCE::CMD_Set_Text CMD_Set_Text( m_pMedia_Plugin->m_dwPK_Device, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "",
            ( pMediaStream ? pMediaStream->m_sMediaSynopsis : "* no media *" ), m_iPK_Text_Synopsis );
        CMD_Update_Object_Image.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Text.m_pMessage );
    }

    m_pMedia_Plugin->QueueMessageToRouter( CMD_Update_Object_Image.m_pMessage );
    // TODO -- Need a real way to send multiple messages to the same device in one package. This gets them to the
}
