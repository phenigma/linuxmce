/*
 Media_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "Media_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <algorithm>

#include "MediaPluginInfo.h"
#include "DCERouter/DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"

#define MAX_MEDIA_COLORS	5 // We need some unique colors to color-code the media streams
int UniqueColors[MAX_MEDIA_COLORS];

MediaDevice::MediaDevice( class Router *pRouter, class Row_Device *pRow_Device )
{
UniqueColors[0] = PlutoColor(128,0,0).m_Value;
UniqueColors[1] = PlutoColor(0,128,0).m_Value;
UniqueColors[2] = PlutoColor(0,0,128).m_Value;
UniqueColors[3] = PlutoColor(0,128,128).m_Value;
UniqueColors[4] = PlutoColor(128,128,0).m_Value;
    m_pDeviceData_Router = pRouter->m_mapDeviceData_Router_Find( pRow_Device->PK_Device_get( ) );
    // do stuff with this
}

//<-dceag-const-b->! custom
Media_Plugin::Media_Plugin( int DeviceID, string ServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter )
    : Media_Plugin_Command( DeviceID, ServerAddress, bConnectEventHandler, bLocalMode, pRouter ),
    m_MediaMutex( "Media Plugin" )
//<-dceag-const-e->
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_MediaMutex.Init( &m_MutexAttr );

    m_iStreamID=0;

    m_pDatabase_pluto_main = new Database_pluto_main( );
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit=true;
        return;
    }

    if( !MySQLConnect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to main database!" );
        m_bQuit=true;
        return;
    }

    m_pDatabase_pluto_media = new Database_pluto_media( );
    if( !m_pDatabase_pluto_media->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit=true;
        return;
    }

    if( !m_MySqlHelper_Media.MySQLConnect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to media database!" );
        m_bQuit=true;
        return;
    }

    m_pMediaAttributes = new MediaAttributes( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) );

    // Get all the entertainment areas and populate them with all the devices in those areas
    Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get( )->GetRow( m_pRouter->iPK_Installation_get( ) );
    vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
    pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
    for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
    {
        Row_Room *pRow_Room=vectRow_Room[iRoom];

        vector<Row_EntertainArea *> vectRow_EntertainArea;
        pRow_Room->EntertainArea_FK_Room_getrows( &vectRow_EntertainArea );
        for( size_t s=0;s<vectRow_EntertainArea.size( );++s )
        {
            Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];
            EntertainArea *pEntertainArea = new EntertainArea( pRow_EntertainArea->PK_EntertainArea_get( ), ( pRow_EntertainArea->Only1Stream_get( )==1 ) );
            m_mapEntertainAreas[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
            // Now find all the devices in the ent area
            vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
            pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows( &vectRow_Device_EntertainArea );
            for( size_t s2=0;s2<vectRow_Device_EntertainArea.size( );++s2 )
            {
                Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s2];
                // This will recurse all the other devices in the path, and add them too
                AddDeviceToEntertainArea( pEntertainArea, pRow_Device_EntertainArea->FK_Device_getrow( ) );

                // If this is a media director, be sure all the device's children, like the media players, disc drives, etc. are also in the same entertainment area
                // We don't do this for other devices because some devices, like whole house amps, have children that are distinct zones in their own entertainment areas
                if( pRow_Device_EntertainArea->FK_Device_getrow( )->FK_DeviceTemplate_getrow( )->FK_DeviceCategory_get( )==DEVICECATEGORY_Media_Director_CONST )
                {
                    vector<Row_Device *> vectRow_Device;
                    pRow_Device_EntertainArea->FK_Device_getrow( )->Device_FK_Device_ControlledVia_getrows( &vectRow_Device ); // all it's children
                    for( size_t child=0;child<vectRow_Device.size( );++child )
                        AddDeviceToEntertainArea( pEntertainArea, vectRow_Device[child] );
                }
            }
        }
    }

    // Now go through all the devices in all the ent areas, and add the reverse match so all devices have a list of the entertain areas they're in
    for( map<int, EntertainArea *>::iterator itEntArea=m_mapEntertainAreas.begin( );itEntArea!=m_mapEntertainAreas.end( );++itEntArea )
    {
        class EntertainArea *pEntertainArea = ( *itEntArea ).second;
        for( map<int, class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin( );itDevice!=pEntertainArea->m_mapMediaDevice.end( );++itDevice )
        {
            class MediaDevice *pMediaDevice = ( *itDevice ).second;
            pMediaDevice->m_listEntertainArea.push_back( pEntertainArea );
        }
    }
}

//<-dceag-dest-b->
Media_Plugin::~Media_Plugin()
//<-dceag-dest-e->
{
    delete m_pMediaAttributes;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Plugin::Register()
//<-dceag-reg-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Orbiter_Plugin_CONST );
    if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Media handler plug in cannot find orbiter handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
        return false;
    }

    m_pOrbiter_Plugin=( Orbiter_Plugin * ) pListCommand_Impl->front( );

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaInserted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Inserted_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::PlaybackCompleted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );

    // And the datagrid plug-in
    m_pDatagrid_Plugin=NULL;
    pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Datagrid_Plugin_CONST );
    if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "File grids cannot find datagrid handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
        return false;
    }

    m_pDatagrid_Plugin=( Datagrid_Plugin * ) pListCommand_Impl->front( );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::CurrentMediaSections) )
        , DATAGRID_Current_Media_Sections_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::AvailablePlaylists) )
        , DATAGRID_Playlists_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaSearchAutoCompl ) )
        , DATAGRID_Media_Search_Auto_Compl_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrFiles ) )
        , DATAGRID_Media_Attr_Files_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrCollections ) )
        , DATAGRID_Media_Attr_Collections_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrXref ) )
        , DATAGRID_Media_Attr_Xref_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaItemAttr ) )
        , DATAGRID_Media_Item_Attr_CONST );

    // datagrids to support the floorplans
    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::FloorplanMediaChoices ) )
        , DATAGRID_Floorplan_Media_Choices_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ActiveMediaStreams ) )
        , DATAGRID_Floorplan_Media_Streams_CONST );

    //  m_pMediaAttributes->ScanDirectory( "/home/public/data/music/" );
//  m_pMediaAttributes->ScanDirectory( "Z:\\" );

    return Connect( );
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Media_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Media_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

void Media_Plugin::AddDeviceToEntertainArea( EntertainArea *pEntertainArea, Row_Device *pRow_Device )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    if( pEntertainArea->m_mapMediaDevice_Find( pRow_Device->PK_Device_get( ) ) )
        return; // We already added this one

    // See if we already have this media device in our master list
    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pRow_Device->PK_Device_get( ) );
    if( !pMediaDevice )
    {
        pMediaDevice = new MediaDevice( m_pRouter, pRow_Device ); // Nope, create it
        m_mapMediaDevice[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pMediaDevice;
    }
    pEntertainArea->m_mapMediaDevice[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pMediaDevice;

    map<int, ListMediaDevice *> m_mapMediaDeviceByTemplate;  /** All the media devices in the area by device template */
    ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(pRow_Device->FK_DeviceTemplate_get());
    if( !pListMediaDevice )
    {
        pListMediaDevice = new ListMediaDevice();
        pEntertainArea->m_mapMediaDeviceByTemplate[pRow_Device->FK_DeviceTemplate_get()]=pListMediaDevice;
    }
    pListMediaDevice->push_back(pMediaDevice);
}

// Our message interceptor
bool Media_Plugin::MediaInserted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int PK_MediaType = atoi( pMessage->m_mapParameters[EVENTPARAMETER_FK_MediaType_CONST].c_str( ) );
    string MRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];

    // First figure out what entertainment area this corresponds to. We are expecting that whatever media player is running on this pc will have
    // added the disc drive to it's entertainment area when it registered
    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDeviceFrom->m_dwPK_Device );
    if( !pMediaDevice )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got a media inserted event from a drive that isn't a media device" );
        return false; // Let someone else handle it
    }

    if( !pMediaDevice->m_listEntertainArea.size( ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got a media inserted event from a drive that doesn't exist in an entertainment area" );
        return false; // Let someone else handle it
    }

    // If there are more than one entertainment areas for this drive there's nothing we can do since we can't know the
    // destination based on the media inserted event. No matter what, we'll just pick the first one
    EntertainArea *pEntertainArea = pMediaDevice->m_listEntertainArea.front( );

    // See if there's a media handler for this type of media in this area
    List_MediaPluginInfo *pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_MediaType_Find( PK_MediaType );
    if( !pList_MediaPluginInfo || pList_MediaPluginInfo->size( )==0 )
    {
        g_pPlutoLogger->Write( LV_WARNING, "Media inserted from device %d but nothing to handle it", pDeviceFrom->m_dwPK_Device );
        return false; // Let someone else handle it
    }

    // We've got all the plug-ins that can handle this type of media
    for( List_MediaPluginInfo::iterator itMPI=pList_MediaPluginInfo->begin( );itMPI!=pList_MediaPluginInfo->end( );++itMPI )
    {
        MediaPluginInfo *pMediaPluginInfo = *itMPI;
        if( pMediaPluginInfo->m_bUsesRemovableMedia ) // todo - hack --- this should be a list of removable media devices
        {
            deque<MediaFile *> dequeMediaFile;
            dequeMediaFile.push_back(new MediaFile(m_pMediaAttributes,MRL));
            StartMedia(pMediaPluginInfo,0,pEntertainArea,pDeviceFrom->m_dwPK_Device,0,&dequeMediaFile);
            return true;
        }
    }

    g_pPlutoLogger->Write( LV_WARNING, "Nothing knows how to handle removable media here. We have %d Plugins registered", ( int ) pList_MediaPluginInfo->size( ) );
    return false; // Couldn't handle it//                     g_pPlutoLogger->Write(LV_STATUS, "Filename: %s", sFilename.c_str());

}

bool Media_Plugin::PlaybackCompleted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );

    MediaStream * pMediaStream = m_mapMediaStream_Find( iStreamID );

    if ( pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }

    if ( pMediaStream->m_pOH_Orbiter == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The stream object mapped to the stream ID %d does not have an associated orbiter object.", iStreamID);
        return false;
    }

    EntertainArea *pEntertainArea = pMediaStream->m_pOH_Orbiter->m_pEntertainArea;

//     pMediaStream->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device
    if ( pEntertainArea == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The orbiter %d which created this stream %d is not in a valid entertainment area",
                    pMediaStream->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
                    iStreamID );
        return false;
    }

#pragma warning("implement this");
    /*
    if ( pMediaStream->HaveMoreInQueue() )
        return StartMediaByPositionInPlaylist(pEntertainArea, pMediaStream->m_iDequeMediaFile_Pos + 1, m_dwPK_Device, 0);
*/
    g_pPlutoLogger->Write(LV_STATUS, "Playback completed. At the end of the queue");
    return true;
}

bool Media_Plugin::StartMedia( MediaPluginInfo *pMediaPluginInfo, int PK_Device_Orbiter, EntertainArea *pEntertainArea, int PK_Device_Source, int PK_DesignObj_Remote, deque<MediaFile *> *dequeMediaFile )
{
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);
    // Normally we'll add new files to the queue if we're playing stored audio/video.  However, if the current media is not playing files, or if it's playing a mounted dvd, or if the new
    // file to play is a mounted dvd, then we can't add to the queue, and will need to create a new media stream
    if( pEntertainArea->m_pMediaStream )
    {
        if( pEntertainArea->m_pMediaStream->m_pMediaPluginInfo!=pMediaPluginInfo )
        {
            // We can't queue this.
			pEntertainArea->m_pMediaStream->m_pMediaPluginInfo->m_pMediaPluginBase->StopMedia( pEntertainArea->m_pMediaStream );
			delete pEntertainArea->m_pMediaStream;
			pEntertainArea->m_pMediaStream=NULL;
        }
    }

    MediaDevice *pMediaDevice=NULL;
    if( PK_Device_Source )
        pMediaDevice = m_mapMediaDevice_Find(PK_Device_Source);

    // If this pointer is still valid, then we'll just add this file to the queue
    MediaStream *pMediaStream;
    if( pEntertainArea->m_pMediaStream )
    {
        pMediaStream = pEntertainArea->m_pMediaStream;
        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
        pMediaStream->m_iDequeMediaFile_Pos = pMediaStream->m_dequeMediaFile.size()-1;
    }
    else
    {
        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device_Orbiter);
        pMediaStream = pMediaPluginInfo->m_pMediaPluginBase->CreateMediaStream(pMediaPluginInfo,pEntertainArea,pMediaDevice,(pOH_Orbiter ? pOH_Orbiter->m_iPK_Users : 0),dequeMediaFile,++m_iStreamID);
        pEntertainArea->m_pMediaStream=pMediaStream;
        pMediaStream->m_pOH_Orbiter = pOH_Orbiter;
        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
    }

    pMediaStream->m_pMediaPluginInfo = pMediaPluginInfo;
    pMediaStream->m_iPK_MediaType = pMediaPluginInfo->m_PK_MediaType;
    pMediaStream->m_mapEntertainArea[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;

    // hack get the user if the message originated from an orbiter!

    // Todo -- get the real remote
    if( PK_DesignObj_Remote )
        pMediaStream->m_iPK_DesignObj_Remote = PK_DesignObj_Remote;

    g_pPlutoLogger->Write(LV_STATUS,"Calling Plug-in's start media");
    if( pMediaPluginInfo->m_pMediaPluginBase->StartMedia(pMediaStream) )
    {
        g_pPlutoLogger->Write(LV_STATUS,"Plug-in started media");
        if( pMediaStream->m_iPK_DesignObj_Remote )
        {
            for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
            {
                OH_Orbiter *pOH_Orbiter = (*it).second;
                if( !pOH_Orbiter->m_pEntertainArea || pMediaStream->m_mapEntertainArea.find(pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea)==pMediaStream->m_mapEntertainArea.end() )
                    continue;  // Don't send an orbiter to the remote if it's not linked to an entertainment area where we're playing this stream

                int iPK_DesignObj_Remote = pMediaStream->m_iPK_DesignObj_Remote;

                DeviceData_Router *pController = m_pRouter->m_mapDeviceData_Router_Find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device_ControlledVia);
                if (  pController && pController->m_dwPK_DeviceCategory == DEVICECATEGORY_Media_Director_CONST &&
                    ( pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST || pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_LiveTV_CONST ) )
                    iPK_DesignObj_Remote = DESIGNOBJ_screen_app_desktop_CONST;

                // Only send the orbiter if it's at the main menu, unless it's the orbiter that started the stream in the first place
#pragma warning("implement bound to media remote")
                // this IF is identical ?? Why ?
                if( pMediaStream->m_pOH_Orbiter==pOH_Orbiter ) //|| pOH_Orbiter->boundtomediaremote )
                {
                    DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,
                        StringUtils::itos(iPK_DesignObj_Remote),"","",false);
                    SendCommand(CMD_Goto_Screen);
                }
                else
                {
                    DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,
                        StringUtils::itos(iPK_DesignObj_Remote),"","",false);
                    SendCommand(CMD_Goto_Screen);
                }
            }
        }
        else
            g_pPlutoLogger->Write(LV_CRITICAL,"Media Plug-in's call to Start Media failed.");
    }
    else
        g_pPlutoLogger->Write(LV_CRITICAL,"Media Plug-in's call to Start Media failed.");

    g_pPlutoLogger->Write(LV_WARNING, "We have called the CreateMediaStream");
    return true; // hack -- handle errors
}

class DataGridTable *Media_Plugin::CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    class MediaStream *pMediaStream = DetermineStreamOnOrbiter( pMessage->m_dwPK_Device_From, true );

    if ( pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The message was not from an orbiter or there is not media stream on it");
        return NULL;
    }

    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    deque<MediaFile *>::iterator itFiles;
    string sCurrentFile;

    int currentPos = 0;
    for ( itFiles = pMediaStream->m_dequeMediaFile.begin(); itFiles != pMediaStream->m_dequeMediaFile.end(); itFiles++ )
    {
        int index = itFiles - pMediaStream->m_dequeMediaFile.begin();

        sCurrentFile = (*itFiles)->FullyQualifiedFile();

        pDataGrid->SetData(0, currentPos++,
                new DataGridCell((*itFiles)->m_sFilename, StringUtils::itos(itFiles - pMediaStream->m_dequeMediaFile.begin())));
    }
    return pDataGrid;
}

bool Media_Plugin::ReceivedMessage( class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    g_pPlutoLogger->Write( LV_STATUS, "Media plug in received message id: %d", pMessage->m_dwID );
    // Give it to our base class to see if we have a handler
    if( !Media_Plugin_Command::ReceivedMessage( pMessage ) )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Media plug base class didn't handle message id: %d", pMessage->m_dwID );
        // We didn't handle it. Give it to the appropriate plug-in. Assume it's coming from an orbiter
        class EntertainArea *pEntertainArea;
        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
        if( !pOH_Orbiter || !pOH_Orbiter->m_pEntertainArea || !pOH_Orbiter->m_pEntertainArea->m_pMediaStream  )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "An orbiter sent the media handler message type: %d id: %d, but it's not for me and I can't find a stream in it's entertainment area", pMessage->m_dwMessage_Type, pMessage->m_dwID );
            return false;
        }
		pEntertainArea=pOH_Orbiter->m_pEntertainArea;

        // We know it's derived from CommandImpl
        class Command_Impl *pPlugIn = pEntertainArea->m_pMediaStream->m_pMediaPluginInfo->m_pCommand_Impl;
        pMessage->m_dwPK_Device_To=pPlugIn->m_dwPK_Device;
        if( !pPlugIn->ReceivedMessage( pMessage ) )
        {
            g_pPlutoLogger->Write( LV_STATUS, "Media plug in did not handled message id: %d forwarding to %d", pMessage->m_dwID, pEntertainArea->m_pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device );
            // The plug-in doesn't know what to do with it either. Send the message to the actual media device
            pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device;

            #pragma warning( "received dcemessage should take a bool bdon't delete in or something so we don't need to copy he message" );
            Message *pNewMessage = new Message( pMessage );
            QueueMessageToRouter( pNewMessage );
        }

        g_pPlutoLogger->Write( LV_STATUS, "Media plug in handled message id: %d", pMessage->m_dwID );
    }

    g_pPlutoLogger->Write( LV_STATUS, "Media plug base handled message id: %d", pMessage->m_dwID );

    return true;
}

void Media_Plugin::MediaInfoChanged( MediaStream *pMediaStream )
{
	delete pMediaStream->m_pPictureData;
	pMediaStream->m_iPictureSize=0;

	if( pMediaStream->m_dequeMediaFile.size() )
	{
		MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[pMediaStream->m_iDequeMediaFile_Pos];
		pMediaStream->m_pPictureData = FileUtils::ReadFileIntoBuffer(pMediaFile->m_sPath + "/" +
			FileUtils::FileWithoutExtension(pMediaFile->m_sFilename) + ".jpg", pMediaStream->m_iPictureSize);

		if( !pMediaStream->m_pPictureData )
		{
			pMediaStream->m_pPictureData = FileUtils::ReadFileIntoBuffer(pMediaFile->m_sPath + "/cover.jpg", pMediaStream->m_iPictureSize);
			if( !pMediaStream->m_pPictureData && pMediaFile->m_dwPK_File )
			{
				int PK_Picture=0;
				Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(pMediaFile->m_dwPK_File);
				vector<Row_Picture_File *> vectRow_Picture_File;
				if( pRow_File )
				{
					pRow_File->Picture_File_FK_File_getrows(&vectRow_Picture_File);
					if( vectRow_Picture_File.size() )
						PK_Picture = vectRow_Picture_File[0]->FK_Picture_get();
					else
					{
						vector<Row_File_Attribute *> vectRow_File_Attribute;
						pRow_File->File_Attribute_FK_File_getrows(&vectRow_File_Attribute);
						for(size_t s=0;s<vectRow_File_Attribute.size();++s)
						{
							vector<Row_Picture_Attribute *> vectRow_Picture_Attribute;
							vectRow_File_Attribute[s]->FK_Attribute_getrow()->Picture_Attribute_FK_Attribute_getrows(&vectRow_Picture_Attribute);
							if( vectRow_Picture_Attribute.size() )
							{
								PK_Picture = vectRow_Picture_Attribute[0]->FK_Picture_get();
								break;
							}
						}
					}
				}
				if( PK_Picture )
					pMediaStream->m_pPictureData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + ".jpg", pMediaStream->m_iPictureSize);
			}
		}
	}

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
        for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
        {
            BoundRemote *pBoundRemote = ( *itBR ).second;
            pBoundRemote->UpdateOrbiter( pMediaStream );
        }
		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
				m_pOrbiter_Plugin->SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pMediaStream->m_sMediaDescription );
		}
    }
}

/*

    COMMANDS TO IMPLEMENT

*/
//<-dceag-c44-b->

	/** @brief COMMAND: #44 - MH Stop Media */
	/** Stop media. All parameters are optional. The Media handler must find out what media to stop. */
		/** @param #2 PK_Device */
			/** The specific device to stop media on. */
		/** @param #29 PK_MediaType */
			/** The type of media to stop. */
		/** @param #44 PK_DeviceTemplate */
			/** The type of device to stop the media on. */
		/** @param #45 PK_EntertainArea */
			/** This is the location on which we need to stop the media. This is optional. If not specified the orbiter will decide the location based on the controlled area. */

void Media_Plugin::CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c44-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Only an Orbiter will tell us to play media
    EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, iPK_EntertainArea );
    if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
        return; // Don't know what area it should be played in, or there's no media playing there

    g_pPlutoLogger->Write( LV_STATUS, "Got MH_stop media" );
    pEntertainArea->m_pMediaStream->m_pMediaPluginInfo->m_pMediaPluginBase->StopMedia( pEntertainArea->m_pMediaStream );

    for( MapEntertainArea::iterator it=pEntertainArea->m_pMediaStream->m_mapEntertainArea.begin( );it!=pEntertainArea->m_pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
				m_pOrbiter_Plugin->SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "" );
		}
    }

    delete pEntertainArea->m_pMediaStream;
    pEntertainArea->m_pMediaStream=NULL;
}

//<-dceag-c73-b->

	/** @brief COMMAND: #73 - MH Send Me To Remote */
	/** An Orbiter sends this command when it wants to go to the active remote control.  This device will send the sender of the command a 'goto' command with the current remote. */
		/** @param #93 Not Full Screen */
			/** Normally all Orbiters that are on screen displays (ie running on a media director) are always sent to a 'full screen'  application desktop as the remote whenever video is playing, since the user will have another remote control.  Setting this to 1 explici */

void Media_Plugin::CMD_MH_Send_Me_To_Remote(bool bNot_Full_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c73-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Only an Orbiter will send this
    EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, 0, 0 );

    g_pPlutoLogger->Write(LV_STATUS, "Found entertainment area: (%d) %p %p", pEntertainArea->m_iPK_EntertainArea, pEntertainArea, pEntertainArea->m_pMediaStream);
    if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
    {
        DCE::CMD_Goto_Screen CMD_Goto_Screen( m_dwPK_Device, pMessage->m_dwPK_Device_From, 0, StringUtils::itos( DESIGNOBJ_mnuNoMedia_CONST ), "", "", false );
        SendCommand( CMD_Goto_Screen );
        return; // Don't know what area it should be played in, or there's no media playing there
    }

    DCE::CMD_Goto_Screen CMD_Goto_Screen( m_dwPK_Device, pMessage->m_dwPK_Device_From, 0, StringUtils::itos( pEntertainArea->m_pMediaStream->m_iPK_DesignObj_Remote ), "", "", false );
    SendCommand( CMD_Goto_Screen );
}
//<-dceag-c74-b->

	/** @brief COMMAND: #74 - Bind to Media Remote */
	/** When an orbiter goes to a media remote control screen, it fires this command so that the media plug-in knows it is sitting at a remote, and needs to be notified when the media changes or the cover art changes.  This should be in the onLoad commands of eve */
		/** @param #2 PK_Device */
			/** The device the orbiter is controlling. */
		/** @param #3 PK_DesignObj */
			/** The object where the remote displays the graphical image of the cover art.  It will get update object images when the cover art changes. */
		/** @param #8 On/Off */
			/** If 1, bind (the Orbiter is sitting at the remote screen).  If 0, the orbiter has left the remote screen, and does not need media changed commands. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The current screen. */
		/** @param #25 PK_Text */
			/** The text object that contains the media description.  This will get set whenever the media changes, such as changing discs or channels. */
		/** @param #39 Options */
			/** Miscellaneous options.  These are not pre-defined, but are specific to a remote and the plug-in.  For example, the PVR plug-in needs to know what tuning device is active. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area the orbiter is controlling. */
		/** @param #56 PK_Text_Timecode */
			/** If the remote wants time code information, the object is stored here.  This can include an optional |n at the end, where n is the number of seconds to update (ie how often to update the counter). */
		/** @param #62 PK_Text_SectionDesc */
			/** The text object for the section description.  The section is tracks on a cd, or shows on a tv channel. */
		/** @param #63 PK_Text_Synopsis */
			/** The text object for the synopsis, a full description.  Examples are a DVD synopsis, or a description of a tv show. */

void Media_Plugin::CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,string sPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis,string &sCMD_Result,Message *pMessage)
//<-dceag-c74-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Only an Orbiter will send this
    OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
    EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, iPK_EntertainArea );
    if( !pEntertainArea || !pOH_Orbiter )
    {
        sCMD_Result="Can't find ent area/orbiter";
        return; // Don't know what area it should be played in, or there's no media playing there
    }

    if( !atoi( sOnOff.c_str( ) ) ) // Off
        pEntertainArea->m_mapBoundRemote_Remove( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
    else
    {

        BoundRemote *pBoundRemote=pEntertainArea->m_mapBoundRemote_Find( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
        if( !pBoundRemote )
        {
            pBoundRemote = new BoundRemote( this );
            pEntertainArea->m_mapBoundRemote[pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device]=pBoundRemote;
        }
        pBoundRemote->m_pOH_Orbiter = pOH_Orbiter;
        pBoundRemote->m_pEntertainArea = pEntertainArea;
        pBoundRemote->m_sPK_DesignObj_GraphicImage = sPK_DesignObj;
        pBoundRemote->m_sPK_DesignObj_Remote = sPK_DesignObj_CurrentScreen;
        pBoundRemote->m_sOptions = sOptions;

        pBoundRemote->m_iPK_Text_Description = iPK_Text;
        pBoundRemote->m_iPK_Text_Section = iPK_Text_SectionDesc;
        pBoundRemote->m_iPK_Text_Synopsis = iPK_Text_Synopsis;
        pBoundRemote->m_iPK_Text_Timecode = iPK_Text_Timecode;
        pBoundRemote->UpdateOrbiter( pEntertainArea->m_pMediaStream ); // So that it will put the picture back on this remote
    }
}


#pragma warning( "put this in the mythtv plugin" )
/*

if( atoi( pEGO->Screen.c_str( ) )==OBJECT_MNUPVR_CONST )
            {
                pEntGroup->m_bPlutoTV=true;
                if( !pEntGroup->m_pWatchingStream->pProvider_Station__Schedule )
                {
                    // This must be an md which defaults to non-pluto tv, and we're switching
                    // to pluto tv for the first time
                    // Stop this media, and restart it with PlutoTV
                    pEntGroup->FindDefaultShowForController( pEGO->m_pDevice_Controller->m_ptrController_Iam, pEntGroup->m_pWatchingStream );
                    if( pEntGroup->m_pWatchingStream->pProvider_Station__Schedule )
                    {
                        pEntGroup->m_pWatchingStream->m_Object_RemoteScreen=OBJECT_MNUPVR_CONST;
                        pEntGroup->TurnOnAndTune( pEGO->m_pDevice_Controller->m_ptrController_Iam, pEntGroup->m_pWatchingStream->pProvider_Station__Schedule );
                    }
                    else
                    {
                        // Send the controller back to non-pluto tv
                        // Do this within the queue because this can be called from within a mobile orbiter
                        // show menu, and we don't want to loop right back around again
                        pEntGroup->m_pWatchingStream->m_Object_RemoteScreen=OBJECT_MNUSATELLITECABLEBOX_CONST;
                        AddMessageToQueue( new OCMessage( DEVICEID_SERVER, ( *SafetyMessage )->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
                            ACTION_NAV_GOTO_CONST, 1, C_ACTIONPARAMETER_PK_OBJECT_HEADER_CONST, StringUtils::itos( OBJECT_MNUSATELLITECABLEBOX_CONST ).c_str( ) ) );
                    }
                }
                pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_REMOTE_1_DEVICE_CONST, StringUtils::itos( pEntGroup->m_PK_EntGroup ) );
                pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_PK_DEVICE_CONST, StringUtils::itos( pEntGroup->m_PK_EntGroup ) );
            }
            else if( atoi( pEGO->Screen.c_str( ) )==OBJECT_MNUSATELLITECABLEBOX_CONST ) // Non-pluto TV
            {
                if( pEntGroup->m_pWatchingStream )
                    pEntGroup->m_pWatchingStream->m_Object_RemoteScreen=OBJECT_MNUSATELLITECABLEBOX_CONST;

                pEntGroup->m_bPlutoTV=false;
                pEGO->PVRInfo=( *SafetyMessage )->m_mapParameters[C_ACTIONPARAMETER_ID_PVR_SESSION_CONST];
                m_pOCLogger->Write( LV_WARNING, "Switching to pvr with %s", pEGO->PVRInfo.c_str( ) );
                if( pEGO->PVRInfo.length( )>1 ) // a tuner specified
                {
                    // Be sure the right inputs are set
                    int PK_Device_Tuning = atoi( pEGO->PVRInfo.c_str( ) );
                    if( pEntGroup->m_pWatchingStream )
                        pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo=PK_Device_Tuning;
                    ReceivedOCMessage( NULL, new OCMessage( ( *SafetyMessage )->m_dwPK_Device_From, PK_Device_Tuning,
                        PRIORITY_NORMAL, MESSAGETYPE_COMMAND, ACTION_GEN_ON_CONST, 0 ) );

                    // Help out the controller
                    pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_REMOTE_1_DEVICE_CONST, StringUtils::itos( pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo ) );
                    pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_PK_DEVICE_CONST, StringUtils::itos( pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo ) );
                    if( pDeviceFrom->m_ptrController_Iam->m_pMobileOrbiter )
                    {
                        // Help out the mobile orbiter and send it back to it's menu
                        pDeviceFrom->m_ptrController_Iam->GotoScreen( OBJECT_MNUSATELLITECABLEBOX_CONST );
                    }
                    return; // Nothing else to do--it's just a general notification
                }
                else
                {
                    if( pEntGroup->m_pWatchingStream )
                    {
                        Device *pDevice_Tuning=m_pPlutoEvents->m_mapDevice_Find( pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo );
                        if( pDevice_Tuning && pDevice_Tuning->m_iPK_MasterDeviceList==MASTERDEVICELIST_ENTERTAIN_UNIT_CONST )
                        {
                            pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo=0; // This is no good anymore
                            // We're coming to here after having been watching pluto tv
                            // Figure out what tuning device to use
                            if( pEntGroup->m_pWatchingStream->pProvider_Station__Schedule &&
                                pEntGroup->m_pWatchingStream->pProvider_Station__Schedule->m_PK_Provider )
                            {
                                EPGProvider *pProvider = pEntGroup->m_mapProviders[pEntGroup->m_pWatchingStream->pProvider_Station__Schedule->m_PK_Provider];
                                // The user was watching Pluto TV. Use that tuning device
                                if( pProvider )
                                    pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo = pProvider->m_PK_Device_TuningLive;
                            }
                        }
                    }
                    if( pEntGroup->m_pWatchingStream && !pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo )
                    {
m_pOCLogger->Write( LV_WARNING, "not watching anything, find a tuning device" );

                        for( int i=0;i<( int ) pEntGroup->m_vectDevice_AV.size( );++i )
                        {
                            Device *pDevice = pEntGroup->m_vectDevice_AV[i];
                            if( pDevice->m_iPK_DeviceCategory==DEVICECATEGORY_SATELLITE_CONST )
                            {
                                pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo = pDevice->m_dwPK_Device;
                            }
                        }
                    }
                    if( pEntGroup->m_pWatchingStream && pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo )
                    {
m_pOCLogger->Write( LV_WARNING, "setting tuning device to: %d", pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo );
                        pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_REMOTE_1_DEVICE_CONST, StringUtils::itos( pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo ) );
                        pDeviceFrom->m_ptrController_Iam->SetVariable( VARIABLE_PK_DEVICE_CONST, StringUtils::itos( pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo ) );
                    }
                }
            }
*/

//<-dceag-sample-b->!

/*
    else if( ( *SafetyMessage )->m_MessageType==MESSAGETYPE_COMMAND && ( *SafetyMessage )->m_ID==ACTION_CREATED_MEDIA_CONST )
    {
        listMediaAttribute *listMA = MediaAttributes::AttributesFromString( ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_TEXT_CONST] );
        listMediaPicture *listMP = MediaAttributes::PicturesFromString( ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_GRAPHIC_IMAGE_CONST] );
        m_pMediaAttributes->CreatedMedia( atoi( ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_TYPE_CONST].c_str( ) ),
            ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_PATH_CONST],
            listMA, listMP );
        delete listMA;
        delete listMP;
    }
*/

/*
    string sMRL = ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_MEDIA_URL_CONST];
        string sFilename = ( *SafetyMessage )->m_Parameters[C_ACTIONPARAMETER_FILE_NAME_CONST];
        if( sFilename.length( )>0 )
        {
            if( sFilename[0]=='*' )
            {
                bPreview=true;
                sFilename = sFilename.substr( 1 );
            }
            else if( sFilename[0]=='+' )
            {
                bAppend=true;
                sFilename = sFilename.substr( 1 );
            }
            if( sFilename[0]=='#' )
            {
                int ID = atoi( sFilename.substr( 2 ).c_str( ) );
                if( sFilename[1]=='F' )
                    sFilename = m_pMediaAttributes->GetFilePathFromFileID( ID );
                else if( sFilename[1]=='A' )
                    sFilename = m_pMediaAttributes->GetFilePathsFromAttributeID( ID );
            }
        }
*/

class DataGridTable *Media_Plugin::MediaSearchAutoCompl( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    FileListGrid *pDataGrid = new FileListGrid( m_pDatagrid_Plugin, this );
    DataGridCell *pCell;

    if( Parms.length( )==0 )
        return pDataGrid; // Nothing passed in yet

    string AC = Parms;

    string SQL = "select PK_Attribute, Name, FirstName, Description FROM Attribute " \
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_AttributeType=PK_AttributeType "\
        "WHERE ( Name Like '" + AC + "%' OR FirstName Like '" + AC + "%' ) AND Identifier>0 "\
        "ORDER BY Name limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_MySqlHelper_Media.mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = /* string( "~`S24`" ) + */ row[1];
            if( row[2] && *row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( "", row[0] );
            pDataGrid->SetData( 0, RowCount, pCell );

            pCell = new DataGridCell( label, row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }

    SQL = "select PK_Attribute, Name, FirstName, Description FROM SearchToken "\
        "JOIN SearchToken_Attribute ON PK_SearchToken=FK_SearchToken "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON FK_AttributeType=PK_AttributeType "\
        "WHERE Token like '" + AC + "%' "\
        "limit 30;";
    if( ( result.r=m_MySqlHelper_Media.mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = /*string( "~`S24`" ) + */ row[1];
            if( row[2] && *row[2]  )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( "", row[0] );
            pDataGrid->SetData( 0, RowCount, pCell );

            pCell = new DataGridCell( label, row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrFiles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

    string PK_Attribute = Parms;

    if( PK_Attribute.substr( 0, 2 )=="#A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="#F" )
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );

    if ( PK_Attribute == "" )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return NULL;
    }

    string SQL="select DISTINCT Dest.FK_File, Attribute.Name, Attribute.FirstName, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + PK_Attribute +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 ORDER BY Name limit 200;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_MySqlHelper_Media.mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = row[1];
            if( row[2] && *row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrCollections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

    string PK_Attribute = Parms;
    if( PK_Attribute.substr( 0, 2 )=="#A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="#F" )
        PK_Attribute = StringUtils::itos(
            m_pMediaAttributes->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) )
         );

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, Attribute.FirstName, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + PK_Attribute +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=2 ORDER BY Name limit 100;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_MySqlHelper_Media.mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = row[1];
            if( row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrXref( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

    string PK_Attribute = Parms;
    g_pPlutoLogger->Write(LV_STATUS, "Got this PK_Attributte: %s", PK_Attribute.c_str());

    if( PK_Attribute.substr( 0, 2 )=="#A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="#F" )
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );

    g_pPlutoLogger->Write(LV_STATUS, "Transformed PK_Attributte: %s", PK_Attribute.c_str());

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, Attribute.FirstName, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + PK_Attribute +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=3 ORDER BY Name limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_MySqlHelper_Media.mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = row[1];
            if( row[2] && *row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaItemAttr( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    string SQL;

    int PK_File=0;
    string PK_Attribute = Parms;
    if( PK_Attribute.length()==0 )
        return NULL;
    if( PK_Attribute.substr(0,2)=="#F" )
        PK_File = atoi(PK_Attribute.substr(2).c_str());
    else
    {
        if( PK_Attribute.substr(0,2)=="#A" )
            PK_Attribute = PK_Attribute.substr(2);

        PK_File = m_pMediaAttributes->GetFileIDFromAttributeID(atoi(PK_Attribute.c_str()));

        if( !PK_File )
        {
            // They chose an attribute other than a file, like an album
            SQL = "select DISTINCT PK_Attribute,Description,Name,FirstName FROM File_Attribute AS Source "\
                "JOIN File_Attribute As Dest ON Source.FK_File=Dest.FK_File "\
                "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute  "\
                "JOIN C_AttributeType ON Attribute.FK_C_AttributeType=PK_C_AttributeType  "\
                "JOIN File ON Dest.FK_File=PK_File "\
                "JOIN C_Type_C_AttributeType ON C_Type_C_AttributeType.FK_C_Type=File.FK_C_Type "\
                "AND C_Type_C_AttributeType.FK_C_AttributeType=Attribute.FK_C_AttributeType "\
                "WHERE Identifier>2 AND Source.FK_Attribute=" + PK_Attribute + " ORDER BY Description";
        }
    }

    string::size_type pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    int PK_Picture;
    string Extension;
    if( PK_File )
        Extension = m_pMediaAttributes->GetPictureFromFileID(PK_File,&PK_Picture);
    else
        Extension = m_pMediaAttributes->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

    // We may have already built the sql statement
    if( SQL.length()==0 )
    {
        SQL="SELECT PK_Attribute,Description,Name,FirstName FROM File "\
        "JOIN File_Attribute ON FK_File=PK_File "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN C_AttributeType ON FK_C_AttributeType=PK_C_AttributeType "\
        "WHERE PK_File=" + StringUtils::itos(PK_File) + " ORDER By Description";
    }

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( (result.r=m_MySqlHelper_Media.mysql_query_result(SQL)) )
    {
        while( (row=mysql_fetch_row(result.r)) )
        {
            string label = row[1];
            label += ": ";
            label += row[2];
            if( row[3] && *row[3] )
                label += string(",") + row[3];
            pCell = new DataGridCell(label,row[0]);
            pDataGrid->SetData(0,RowCount++,pCell);
        }
    }
    return pDataGrid;
}

EntertainArea *Media_Plugin::DetermineEntArea( int iPK_Device_Orbiter, int iPK_Device, int iPK_EntertainArea )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // If we don't have an entertainment area, but we do have a device that is a media device we can find it there
    if( !iPK_EntertainArea && iPK_Device )
    {
        MediaDevice *pMediaDevice = m_mapMediaDevice_Find( iPK_Device );
        if( pMediaDevice && pMediaDevice->m_listEntertainArea.size( ) )
        {
            EntertainArea *pEntertainArea = pMediaDevice->m_listEntertainArea.front( );
            iPK_EntertainArea = pEntertainArea->m_iPK_EntertainArea;
        }
    }

    // See if we need to figure out the entertainment area on our own. If so, the only way to do this is if the message came from an orbiter
    if( !iPK_EntertainArea )
    {
        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( iPK_Device_Orbiter );
        if( !pOH_Orbiter )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "Received a play media with no entertainment area from a non-orbiter" );
            return NULL; // Don't know what area it should be played in
        }

		iPK_EntertainArea = pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0;
        if( !iPK_EntertainArea )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "Received a DetermineEntArea from an orbiter with no entertainment area" );
            return NULL; // Don't know what area it should be played in
        }
    }

    g_pPlutoLogger->Write(LV_STATUS, "Found the proper ent area: %d", iPK_EntertainArea);
    EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( iPK_EntertainArea );
    if( !pEntertainArea )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Received a play media for an invalid entertainment area" );
        return NULL; // Don't know what area it should be played in
    }
    return pEntertainArea;
}

int Media_Plugin::DetermineUserOnOrbiter(int iPK_Device_Orbiter)
{
    if ( !m_pOrbiter_Plugin )
    {
        g_pPlutoLogger->Write(LV_WARNING, "There is no valid orbiter_plugin in the media plugin. (While trying to find a user on the orbiter: %d)", iPK_Device_Orbiter );
        return 0;
    }

    class OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Device_Orbiter);
    if ( ! pOH_Orbiter )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Not orbiter found for id %d while trying to find the active user.", iPK_Device_Orbiter);
        return 0;
    }

    return pOH_Orbiter->m_iPK_Users;
}

//<-dceag-c43-b->

	/** @brief COMMAND: #43 - MH Play Media */
	/** The Orbiters send the play media command to the actual media handler. rnrnThe Orbiter can send anyone or a combination of parameters. rnrnIt's up to media handler to figure out how to handle it. The media handler must find out if the media is already pla */
		/** @param #2 PK_Device */
			/** The ID of the actual device to start playing. */
		/** @param #3 PK_DesignObj */
			/** The Remote Control to use for playing this media. */
		/** @param #13 Filename */
			/** The filename to play or a pipe delimited list of filenames. */
		/** @param #29 PK_MediaType */
			/** The ID of the media type descriptor (if it is TV, movie, audio, etc ..  ) */
		/** @param #44 PK_DeviceTemplate */
			/** The DeviceTemplate ID. */
		/** @param #45 PK_EntertainArea */
			/** The desired target area for the playback. If this is missing then the orbiter should decide the target based on his controlled area. */

void Media_Plugin::CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c43-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str()); // TODO: handle multiple entertainment areas
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);
    int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    // Only an Orbiter will tell us to play media
    EntertainArea *pEntertainArea = DetermineEntArea(iPK_Device_Orbiter,iPK_Device,iPK_EntertainArea);
    if( !pEntertainArea )
        return;  // Don't know what area it should be played in

    deque<MediaFile *> dequeMediaFile;
    if( sFilename.length()>0 )
        m_pMediaAttributes->TransformFilenameToDeque(sFilename,dequeMediaFile);  // This will convert any #a, #f, etc.

    // What is the media?  It must be a master device, or a media type, or filename
    if( iPK_DeviceTemplate )
    {
        // todo
    }
    else if( iPK_MediaType )
    {
        // See if there's a media handler for this type of media in this area
        List_MediaPluginInfo *pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_MediaType_Find(iPK_MediaType);
        if( !pList_MediaPluginInfo || pList_MediaPluginInfo->size()==0 )
        {
            g_pPlutoLogger->Write(LV_WARNING,"Play media type %d in entertain area %d but nothing to handle it",iPK_MediaType,pEntertainArea->m_iPK_EntertainArea);
        }
        else
        {
            MediaPluginInfo *pMediaPluginInfo = pList_MediaPluginInfo->front();
            StartMedia(pMediaPluginInfo,iPK_Device_Orbiter,pEntertainArea,iPK_Device,0,&dequeMediaFile);  // We'll let the plug-in figure out the source, and we'll use the default remote
        }
    }
    else if( dequeMediaFile.size() )
    {
        string Extension = StringUtils::ToUpper(FileUtils::FindExtension(dequeMediaFile[0]->m_sFilename));
        List_MediaPluginInfo *pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_Extension_Find(Extension);
        if( !pList_MediaPluginInfo || pList_MediaPluginInfo->size()==0 )
            g_pPlutoLogger->Write(LV_WARNING,"Play media file %s in entertain area %d but nothing to handle it",sFilename.c_str(),iPK_EntertainArea);
        else
        {
            MediaPluginInfo *pMediaPluginInfo = pList_MediaPluginInfo->front();
            StartMedia(pMediaPluginInfo,iPK_Device_Orbiter,pEntertainArea,iPK_Device,0,&dequeMediaFile);  // We'll let the plug-in figure out the source, and we'll use the default remote
        }
    }
    else  // We got nothing -- find a disk drive within the entertainment area and send it a reset
    {
        for(map<int,class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin();itDevice!=pEntertainArea->m_mapMediaDevice.end();++itDevice)
        {
            class MediaDevice *pMediaDevice = (*itDevice).second;
            if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory==DEVICECATEGORY_Disc_Drives_CONST )
            {
                DCE::CMD_Reset_Disk_Drive CMD_Reset_Disk_Drive(m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
                SendCommand(CMD_Reset_Disk_Drive);
            }
        }
    }
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** This will allow an orbiter to change the current playing position in the playlist */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void Media_Plugin::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;

    // Only an Orbiter will tell us to jump to next position
    EntertainArea *pEntertainArea = DetermineEntArea( iPK_Device_Orbiter, 0, 0);
    if( !pEntertainArea )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got jump position but i couldn't find an entertainment area for it!");
        return;
    }

    if ( pEntertainArea->m_pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Can't jump in an empty queue");
        return;
    }

    switch ( sValue_To_Assign[0] )
    {
        case '-':
            pEntertainArea->m_pMediaStream->ChangePositionInPlaylist(-atoi(sValue_To_Assign.substr(1).c_str()));
            break;
        case '+':
            pEntertainArea->m_pMediaStream->ChangePositionInPlaylist(atoi(sValue_To_Assign.substr(1).c_str()));
            break;
        default:
            pEntertainArea->m_pMediaStream->SetPlaylistPosition(atoi(sValue_To_Assign.c_str()));
            break;
    }

    pEntertainArea->m_pMediaStream->m_pMediaPluginInfo->m_pMediaPluginBase->StartMedia(pEntertainArea->m_pMediaStream);

//  StartMediaByPositionInPlaylist(pEntertainArea, pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos, 0, 0);
}
//<-dceag-c214-b->

	/** @brief COMMAND: #214 - Save playlist */
	/** This will instruct the device to save the currently playing list */
		/** @param #17 PK_Users */
			/** The user that will own the new playlist. Can be missing. It will pick the current user then. */
		/** @param #45 PK_EntertainArea */
			/** Which playlist to save. You can direct the command to save a specific entertainment area's playlist or you can leave it blank to pick the current entertainment area's playlist */
		/** @param #50 Name */
			/** It will use the this name when saving. If it is not specified it will either use the name of the loaded playlist in the database or it will generate a new one. */
		/** @param #77 Save as new */
			/** Save the playlist as a new playlist. This will override the default behaviour. (If this playlist was not loaded from the database it will be saved as new. If it was loaded it will be overridded). This will make it always save it as new. */

void Media_Plugin::CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage)
//<-dceag-c214-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Find the corrent entertainment area to use.
    EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, 0, iPK_EntertainArea );

    if ( pEntertainArea == NULL || pEntertainArea->m_pMediaStream == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't find a valid entertainment area with a valid stream in it. Can't save the playlist");
        return;
    }

    if ( sName == "" )
        sName = pEntertainArea->m_pMediaStream->m_sPlaylistName;

    if ( sName == "" )
        sName = "New Playlist";

    int iPK_Playlist = bSave_as_new ? 0 : pEntertainArea->m_pMediaStream->m_iPK_Playlist;
    if( !m_pMediaAttributes->SavePlaylist(pEntertainArea->m_pMediaStream->m_dequeMediaFile, iPK_Users, iPK_Playlist, sName) )
    {
        m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to save playlist");
        return;
    }

    pEntertainArea->m_pMediaStream->m_iPK_Playlist = iPK_Playlist;
    pEntertainArea->m_pMediaStream->m_sPlaylistName = sName;
}
//<-dceag-c231-b->

	/** @brief COMMAND: #231 - Load Playlist */
	/** This will instruct the device to load the specific playlist. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to load */

void Media_Plugin::CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage)
//<-dceag-c231-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Find the corrent entertainment area to use.
    EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, 0, iPK_EntertainArea );
    if ( pEntertainArea == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't find a valid entertainment area to load the playlist into.");
        return;
    }

    string sPlaylistName;
    deque<MediaFile *> dequeMediaFile;
    if( !m_pMediaAttributes->LoadPlaylist( iEK_Playlist, dequeMediaFile, sPlaylistName) || dequeMediaFile.size()==0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "I was unable to load playlist entries");
        m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to load playlist");
        return;
    }

    string Extension = StringUtils::ToUpper(FileUtils::FindExtension(dequeMediaFile[0]->m_sFilename));
    List_MediaPluginInfo *pList_MediaPluginInfo = pEntertainArea->m_mapMediaPluginInfo_Extension_Find(Extension);
    if( !pList_MediaPluginInfo || pList_MediaPluginInfo->size()==0 )
    {
        g_pPlutoLogger->Write(LV_WARNING,"Play playlist %s in entertain area %d but nothing to handle it",sPlaylistName.c_str(),iPK_EntertainArea);
        return;
    }
    MediaPluginInfo *pMediaPluginInfo = pList_MediaPluginInfo->front();
    StartMedia(pMediaPluginInfo,pMessage->m_dwPK_Device_From,pEntertainArea,0,0,&dequeMediaFile);  // We'll let the plug-in figure out the source, and we'll use the default remote
}

class DataGridTable *Media_Plugin::FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(atoi(Parms.c_str()));
    if( !pEntertainArea )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find entertainment area for flooplan");
        return new DataGridTable();
    }

    string sWhereClause = string("JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup ") +
        "JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup " +
        "WHERE FK_EntertainArea=" + StringUtils::itos(pEntertainArea->m_iPK_EntertainArea) + " AND FK_Command=" + StringUtils::itos(COMMAND_MH_Play_Media_CONST);
    vector<Row_CommandGroup *> vectRow_CommandGroup;
    m_pDatabase_pluto_main->CommandGroup_get()->GetRows(sWhereClause,&vectRow_CommandGroup);

    for(size_t s=0;s<vectRow_CommandGroup.size();++s)
    {
        Row_CommandGroup *pRow_CommandGroup = vectRow_CommandGroup[s];
        DataGridCell *pDataGridCell = new DataGridCell(pRow_CommandGroup->Description_get(),StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()));
        pDataGrid->SetData(0,s,pDataGridCell);
    }

    return pDataGrid;
}

class DataGridTable *Media_Plugin::ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    int iRow=0;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMediaStream = (*it).second;
		pMediaStream->m_iOrder = iRow;
        DataGridCell *pDataGridCell = new DataGridCell(pMediaStream->m_sMediaDescription,StringUtils::itos(pMediaStream->m_iStreamID_get()));
		pDataGridCell->m_AltColor = UniqueColors[iRow];
        pDataGrid->SetData(0,iRow++,pDataGridCell);
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    g_pPlutoLogger->Write(LV_STATUS, "Called to populate: %s", Parms.c_str());

    if( Parms.length( )==0 )
        return NULL; // Nothing passed in yet

    // select PK_Playlist, Name from Playlist where EK_USER IN ( 0, 33128 ) LIMIT 30;;

    int userID = atoi(Parms.c_str());

    string SQL = "SELECT PK_Playlist, Name from Playlist where EK_USER IN ( 0, " + StringUtils::itos(userID) + " ) ORDER BY NAME LIMIT 30";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    DataGridTable *pDataGrid = new DataGridTable();
    if( (result.r=m_MySqlHelper_Media.mysql_query_result(SQL)) )
        while( (row=mysql_fetch_row(result.r)) )
            pDataGrid->SetData(0,RowCount++,new DataGridCell(row[1], row[0]));

    return pDataGrid;
}


void Media_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD)
{
	MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDeviceData_Router->m_dwPK_Device);
	if( !pMediaDevice )
		return;
	for(list<EntertainArea *>::iterator it=pMediaDevice->m_listEntertainArea.begin();it!=pMediaDevice->m_listEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = *it;
		if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_iOrder>=0 && pEntertainArea->m_pMediaStream->m_iOrder<MAX_MEDIA_COLORS )
		{
			Color = UniqueColors[pEntertainArea->m_pMediaStream->m_iOrder];
			sDescription = pEntertainArea->m_pMediaStream->m_sMediaDescription;
			return;
		}
	}
}//<-dceag-c241-b->

	/** @brief COMMAND: #241 - MH Move Media */
	/** Moves an existing media stream to the specified entertainment areas. */
		/** @param #41 StreamID */
			/** The ID of the media stream to move */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area(s) to send the Media Stream to. */

void Media_Plugin::CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c241-e->
{
	int iPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
}
