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

#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "Generic_NonPluto_Media.h"
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_AttributeType.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"

#define MAX_MEDIA_COLORS    5 // We need some unique colors to color-code the media streams
int UniqueColors[MAX_MEDIA_COLORS];

// For sorting by priority
static bool MediaHandlerComparer(MediaHandlerInfo *x, MediaHandlerInfo *y)
{
	// Reverse since we want highest priorities first
	return x->m_pMediaHandlerBase->m_iPriority>y->m_pMediaHandlerBase->m_iPriority;
}

MediaDevice::MediaDevice( class Router *pRouter, class Row_Device *pRow_Device )
{
	UniqueColors[0] = PlutoColor(128,0,0).m_Value;
	UniqueColors[1] = PlutoColor(0,128,0).m_Value;
	UniqueColors[2] = PlutoColor(0,0,128).m_Value;
	UniqueColors[3] = PlutoColor(0,128,128).m_Value;
	UniqueColors[4] = PlutoColor(128,128,0).m_Value;
	m_iLastPlaybackSpeed = 1000;
	m_pDeviceData_Router = pRouter->m_mapDeviceData_Router_Find( pRow_Device->PK_Device_get( ) );
	m_pOH_Orbiter_OSD = NULL;
	m_bDontSendOffIfOSD_ON=false;
	m_pOH_Orbiter_Reset=NULL;
	m_pCommandGroup=NULL;
	m_tReset=0;
	// do stuff with this
}

//<-dceag-const-b->! custom// i get
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

    m_pDatabase_pluto_media = new Database_pluto_media( );
    if( !m_pDatabase_pluto_media->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit=true;
        return;
    }

    m_pMediaAttributes = new MediaAttributes( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) );

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
            EntertainArea *pEntertainArea = new EntertainArea( pRow_EntertainArea->PK_EntertainArea_get( ),
				( pRow_EntertainArea->Only1Stream_get( )==1 ), pRow_EntertainArea->Description_get(),
				m_pRouter->m_mapRoom_Find(pRow_Room->PK_Room_get()) );
            m_mapEntertainAreas[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
            // Now find all the devices in the ent area
            vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
            pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows( &vectRow_Device_EntertainArea );
            for( size_t s2=0;s2<vectRow_Device_EntertainArea.size( );++s2 )
            {
                Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s2];
if( !pRow_Device_EntertainArea || !pRow_Device_EntertainArea->FK_Device_getrow( ) )
{
g_pPlutoLogger->Write(LV_CRITICAL,"Device_EntertainArea refers to a NULL device %d %d",pRow_Device_EntertainArea->FK_EntertainArea_get(),pRow_Device_EntertainArea->FK_Device_get());
//pRow_Device_EntertainArea->Delete();
//TODO: add the delete above and also do a commit.
continue;
}


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

	// Streamers are often not in entertainment areas.  But we need them to have an entry as
	// a MediaDevice so they can be a media source
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Media_Streamers_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDeviceData_Router = (*it);
		    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDeviceData_Router->m_dwPK_Device );
			if( !pMediaDevice && m_mapEntertainAreas.size() )
			{
				EntertainArea *pEntertainArea = (*m_mapEntertainAreas.begin()).second;
				AddDeviceToEntertainArea(pEntertainArea,pDeviceData_Router->m_pRow_Device);
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
            pMediaDevice->m_mapEntertainArea[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
        }
    }

	// Go through all the command groups, looking for ones that are used to turn a device on.  If the device
	// is turned on manually, we'll execute that command group
    for(map<int,CommandGroup *>::const_iterator it=m_pRouter->m_mapCommandGroup_get()->begin();it!=m_pRouter->m_mapCommandGroup_get()->end();++it)
    {
		CommandGroup *pCommandGroup = it->second;
		for(size_t s=0;s<pCommandGroup->m_vectCommandGroup_Command.size();++s)
		{
			CommandGroup_Command *pCommandGroup_Command = pCommandGroup->m_vectCommandGroup_Command[s];
			if( pCommandGroup_Command->m_pCommand->m_dwPK_Command==COMMAND_MH_Play_Media_CONST &&
				pCommandGroup_Command->m_mapParameter.find(COMMANDPARAMETER_PK_Device_CONST)!=pCommandGroup_Command->m_mapParameter.end() )
			{
				int PK_Device = atoi(pCommandGroup_Command->m_mapParameter[COMMANDPARAMETER_PK_Device_CONST].c_str());
				MediaDevice *pMediaDevice = m_mapMediaDevice_Find(PK_Device);
				if( pMediaDevice )
				{
					pMediaDevice->m_pCommandGroup = pCommandGroup;
					break;
				}
			}
		}
	}

	m_pGeneric_NonPluto_Media = new Generic_NonPluto_Media(this);
    m_pGenericMediaHandlerInfo = new MediaHandlerInfo(m_pGeneric_NonPluto_Media,this,0,0,false,false);
}



//<-dceag-const2-b->!

//<-dceag-dest-b->
Media_Plugin::~Media_Plugin()
//<-dceag-dest-e->
{
    delete m_pMediaAttributes;
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_media;
	for(map<int,MediaDevice *>::iterator it=m_mapMediaDevice.begin();it!=m_mapMediaDevice.end();++it)
		delete (*it).second;
	for(map<int,EntertainArea *>::iterator it=m_mapEntertainAreas.begin();it!=m_mapEntertainAreas.end();++it)
		delete (*it).second;

	for(size_t s=0;s<m_vectMediaHandlerInfo.size();++s)
	{
		MediaHandlerInfo *pMediaHandlerInfo = m_vectMediaHandlerInfo[s];
		delete pMediaHandlerInfo;
	}

    delete m_pGenericMediaHandlerInfo;
	delete m_pGeneric_NonPluto_Media;
    pthread_mutexattr_destroy(&m_MutexAttr);
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

	for( map<int, class MediaDevice *>::iterator itDevice=m_mapMediaDevice.begin( );itDevice!=m_mapMediaDevice.end( );++itDevice )
    {
        class MediaDevice *pMediaDevice = ( *itDevice ).second;

		if ( pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia &&
			pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
		{
			// store the orbiter which is directly controlling the target device as the target on-screen display.
			pMediaDevice->m_pOH_Orbiter_OSD = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device_ControlledVia);
		}
	}

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaInserted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Inserted_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaIdentified ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Identified_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::PlaybackCompleted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaFollowMe ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Follow_Me_Media_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::RippingCompleted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Ripping_Completed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::DeviceOnOff ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_OnOff_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::AvInputChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_AV_Input_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaDescriptionChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Description_Changed_CONST );

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

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DevicesPipes ) )
        , DATAGRID_Devices__Pipes_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrCurStream ) )
        , DATAGRID_Media_Attr_Cur_Stream_CONST );

	// datagrids to support the floorplans
    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::FloorplanMediaChoices ) )
        , DATAGRID_Floorplan_Media_Choices_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ActiveMediaStreams ), true )
        , DATAGRID_Floorplan_Media_Streams_CONST );

    //  m_pMediaAttributes->ScanDirectory( "/home/public/data/music/" );
//  m_pMediaAttributes->ScanDirectory( "Z:\\" );

    return Connect(PK_DeviceTemplate_get());
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

		vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
		pMediaDevice->m_pDeviceData_Router->m_pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);
		for(size_t mt=0;mt<vectRow_DeviceTemplate_MediaType.size();++mt)
		{
			Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[mt];
			pMediaDevice->m_mapMediaType[pRow_DeviceTemplate_MediaType->FK_MediaType_get()]=pRow_DeviceTemplate_MediaType->FK_MediaType_getrow();
		}
	}
    pEntertainArea->m_mapMediaDevice[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pMediaDevice;

g_pPlutoLogger->Write( LV_STATUS, "adding device %d %s to map ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),
					  pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str());

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
	int discid = atoi( pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST].c_str( ) );
    string MRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];

    // First figure out what entertainment area this corresponds to. We are expecting that whatever media player is running on this pc will have
    // added the disc drive to it's entertainment area when it registered
    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDeviceFrom->m_dwPK_Device );
    if( !pMediaDevice )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got a media inserted event from %d -- a drive that isn't a media device", pDeviceFrom->m_dwPK_Device );
        return false; // Let someone else handle it
    }

    if( !pMediaDevice->m_mapEntertainArea.size( ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got a media inserted event from a drive that doesn't exist in an entertainment area" );
        return false; // Let someone else handle it
    }

    // If there are more than one entertainment areas for this drive there's nothing we can do since we can't know the
    // destination based on the media inserted event. No matter what, we'll just pick the first one
    EntertainArea *pEntertainArea = pMediaDevice->m_mapEntertainArea.begin( )->second;

    // See if there's a media handler for this type of media in this area
    List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find( PK_MediaType );
    if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size( )==0 )
    {
        g_pPlutoLogger->Write( LV_WARNING, "Media inserted from device %d but nothing to handle it", pDeviceFrom->m_dwPK_Device );
        return false; // Let someone else handle it
    }

    // We've got all the plug-ins that can handle this type of media
    for( List_MediaHandlerInfo::iterator itMPI=pList_MediaHandlerInfo->begin( );itMPI!=pList_MediaHandlerInfo->end( );++itMPI )
    {
        MediaHandlerInfo *pMediaHandlerInfo = *itMPI;
        if( pMediaHandlerInfo->m_bUsesRemovableMedia ) // todo - hack --- this should be a list of removable media devices
        {
            deque<MediaFile *> dequeMediaFile;

			// we split the MRL by \n first and then pass this to the StartMedia.
			deque<string> dequeFileNames;
			deque<string>::const_iterator itFileNames;

			StringUtils::Tokenize(MRL, "\n", dequeFileNames, false);
			for(size_t s=0;s<dequeFileNames.size();++s)
			{
				MediaFile *pMediaFile = new MediaFile(dequeFileNames[s]);
				if( dequeFileNames.size()>1 )
					pMediaFile->m_sDescription = "Unknown track " + StringUtils::itos(s+1);
				else
					pMediaFile->m_sDescription = "Unknown disc";

				dequeMediaFile.push_back(pMediaFile);
			}

			vector<EntertainArea *> vectEntertainArea;
			vectEntertainArea.push_back(pEntertainArea);
			int PK_Orbiter=0;
			if( pMediaDevice->m_pOH_Orbiter_Reset && time(NULL)-pMediaDevice->m_tReset<60 )
				PK_Orbiter = pMediaDevice->m_pOH_Orbiter_Reset->m_pDeviceData_Router->m_dwPK_Device;

			g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from Media Inserted with orbiter %d",PK_Orbiter);

			MediaStream *pMediaStream = StartMedia(pMediaHandlerInfo,PK_Orbiter,vectEntertainArea,pDeviceFrom->m_dwPK_Device,0,&dequeMediaFile,false,0);
			if( pMediaStream )
				pMediaStream->m_discid = discid;

			// Notify the identifiers that it's time to ID what this is
			DCE::CMD_Identify_Media_Cat CMD_Identify_Media_Cat(pDeviceFrom->m_dwPK_Device,DEVICECATEGORY_Media_Identifiers_CONST,
				false,BL_SameComputer,pDeviceFrom->m_dwPK_Device,StringUtils::itos(discid),pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST]);
			SendCommand(CMD_Identify_Media_Cat);

			return true;
        }
    }

    g_pPlutoLogger->Write( LV_WARNING, "Nothing knows how to handle removable media here. We have %d Plugins registered", ( int ) pList_MediaHandlerInfo->size( ) );
    return false; // Couldn't handle it//                     g_pPlutoLogger->Write(LV_STATUS, "Filename: %s", sFilename.c_str());

}

bool Media_Plugin::MediaIdentified( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	int discid = atoi( pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST].c_str( ) );
    string sFormat = pMessage->m_mapParameters[40];
    string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
	int PK_Device_Disk_Drive = atoi( pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str( ) );
	string sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];

	DeviceData_Router *pDevice_Disk_Drive = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Disk_Drive);
	if( !pDevice_Disk_Drive )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find the disk drive device identified");
		return false;
	}

	// Find the media stream
	MediaStream *pMediaStream=NULL;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMS = (*it).second;
		// If the disk matches, and the drive is either the source, or a sibbling of the source, this is the stream
		if( pMS->m_discid==discid &&
			(pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device==PK_Device_Disk_Drive ||
			pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_ControlledVia==pDevice_Disk_Drive->m_pDevice_ControlledVia ) )
		{
			pMediaStream=pMS;
			break;
		}
	}

	if( !pMediaStream )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find media identified");
		return false;
	}
	if( sFormat=="CDDB-TAB" )
		Parse_CDDB_Media_ID(pMediaStream,sValue);

	return true;
}

bool Media_Plugin::PlaybackCompleted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	bool bWithErrors = pMessage->m_mapParameters[EVENTPARAMETER_With_Errors_CONST]=="1";
    MediaStream * pMediaStream = m_mapMediaStream_Find( iStreamID );

    if ( pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }

/*
    if ( pMediaStream->m_pOH_Orbiter_StartedMedia == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The stream object mapped to the stream ID %d does not have an associated orbiter object.", iStreamID);
        return false;
    }
*/

//     EntertainArea *pEntertainArea = pMediaStream->m_pOH_Orbiter_StartedMedia->m_pEntertainArea;
//     if ( pEntertainArea == NULL )
//     {
//         g_pPlutoLogger->Write(LV_WARNING, "The orbiter %d which created this stream %d is not in a valid entertainment area",
//                     pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
//                     iStreamID );
//         return false;
//     }

	g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::PlaybackCompleted() Checking conditions: canPlayMore: %d, shouldResume %d", pMediaStream->CanPlayMore(), pMediaStream->m_bResume);
    if ( !bWithErrors && pMediaStream->CanPlayMore() && !pMediaStream->m_bResume )
    {
        pMediaStream->ChangePositionInPlaylist(1);
		g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from playback completed");
        pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pMediaStream);
    }
    else
    {
		if( !pMediaStream->m_bStopped )
			pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia(pMediaStream);

		StreamEnded(pMediaStream);

		g_pPlutoLogger->Write(LV_STATUS, "Playback completed. The stream can't play anything more.");
    }

    return true;
}

void Media_Plugin::StartMedia( int iPK_MediaType, unsigned int iPK_Device_Orbiter, vector<EntertainArea *> &vectEntertainArea, int iPK_Device, string sPK_DesignObj, deque<MediaFile *> *p_dequeMediaFile, bool bResume, int iRepeat, vector<MediaStream *> *p_vectMediaStream)
{
	if( !iPK_MediaType && p_dequeMediaFile->size() )
	{
        string Extension = StringUtils::ToUpper(FileUtils::FindExtension((*p_dequeMediaFile)[0]->m_sFilename));

		map<int,MediaHandlerInfo *> mapMediaHandlerInfo;

		for(size_t s=0;s<vectEntertainArea.size();++s)
		{
			EntertainArea *pEntertainArea=vectEntertainArea[s];
			List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find(Extension);
		    if( pList_MediaHandlerInfo && pList_MediaHandlerInfo->size() )
			{
			    MediaHandlerInfo *pMediaHandlerInfo = pList_MediaHandlerInfo->front();
				iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
				break;
	        }
		}
	}

	if( !iPK_MediaType )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"StartMedia - MediaType==0");
		return;
	}

	// Find the media handlers we're going to need.  If everything can be handled by one
	// handler, vectEA_to_MediaHandler will only have 1 element.  Otherwise it will have an
	// element for each handler we need to use (which will translate to a stream for each)
	// and the list of entertainment areas
	vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > vectEA_to_MediaHandler;

	GetMediaHandlersForEA(iPK_MediaType, vectEntertainArea, vectEA_to_MediaHandler);

	// If there are 2 or more stream and we have a deque of mediafiles, make copies of them
	// so each stream will have it's own and won't share, causing duplicate deletes when the stream
	// is closed
	deque<MediaFile *> *p_dequeMediaFile_Copy=NULL;
	if( vectEA_to_MediaHandler.size()>1 && p_dequeMediaFile && p_dequeMediaFile->size() )
		p_dequeMediaFile_Copy = p_dequeMediaFile;

	for(size_t s=0;s<vectEA_to_MediaHandler.size();++s)
	{
		MediaHandlerInfo *pMediaHandlerInfo = vectEA_to_MediaHandler[s].first;
		g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from MH Play Media2");

		if( p_dequeMediaFile_Copy )
		{
			p_dequeMediaFile = new deque<MediaFile *>;
			for(size_t sFile=0;sFile<p_dequeMediaFile_Copy->size();++sFile)
				(*p_dequeMediaFile).push_back( new MediaFile((*p_dequeMediaFile_Copy)[sFile]) );

		}

		StartMedia(pMediaHandlerInfo,iPK_Device_Orbiter,vectEA_to_MediaHandler[s].second,
			iPK_Device,sPK_DesignObj.size() ? atoi(sPK_DesignObj.c_str()) : 0,p_dequeMediaFile,bResume,iRepeat);  // We'll let the plug-in figure out the source, and we'll use the default remote

		if( p_dequeMediaFile_Copy )
			delete p_dequeMediaFile;
	}

	if( p_dequeMediaFile_Copy )
		for(size_t s=0;s<p_dequeMediaFile_Copy->size();++s)
			delete (*p_dequeMediaFile_Copy)[s];
}

MediaStream *Media_Plugin::StartMedia( MediaHandlerInfo *pMediaHandlerInfo, unsigned int PK_Device_Orbiter, vector<EntertainArea *> &vectEntertainArea, int PK_Device_Source, int PK_DesignObj_Remote, deque<MediaFile *> *dequeMediaFile, bool bResume,int iRepeat)
{
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);

    MediaDevice *pMediaDevice=NULL;
    if( PK_Device_Source )
        pMediaDevice = m_mapMediaDevice_Find(PK_Device_Source);

    // If this pointer is still valid, then we'll just add this file to the queue
    MediaStream *pMediaStream = NULL;

    // If all EA's are playing the same stream, it might be possible to queue it
    MediaStream *pMediaStream_AllEAsPlaying = NULL;
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		if( !vectEntertainArea[s]->m_pMediaStream ||
			(pMediaStream_AllEAsPlaying && vectEntertainArea[s]->m_pMediaStream!=pMediaStream_AllEAsPlaying) )
		{
			pMediaStream_AllEAsPlaying=NULL;  // they're not all playing the same stream
			break;
		}
		pMediaStream_AllEAsPlaying=vectEntertainArea[s]->m_pMediaStream;
	}

	// See if we can queue it
    if( !bResume && pMediaStream_AllEAsPlaying && !pMediaStream_AllEAsPlaying->m_bResume &&
		pMediaStream_AllEAsPlaying->m_pMediaHandlerInfo->m_pMediaHandlerBase == pMediaHandlerInfo->m_pMediaHandlerBase &&
		pMediaStream_AllEAsPlaying->m_iPK_MediaType == pMediaHandlerInfo->m_PK_MediaType &&
		pMediaHandlerInfo->m_PK_MediaType!=MEDIATYPE_pluto_DVD_CONST )
    {
        pMediaStream = pMediaStream_AllEAsPlaying;
        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
        pMediaStream->m_iDequeMediaFile_Pos = pMediaStream->m_dequeMediaFile.size()-1;
	    pMediaStream->m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
    }
    else
    {

        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device_Orbiter);
		if ( (pMediaStream = pMediaHandlerInfo->m_pMediaHandlerBase->CreateMediaStream(pMediaHandlerInfo,vectEntertainArea,pMediaDevice,(pOH_Orbiter ? pOH_Orbiter->PK_Users_get() : 0),dequeMediaFile,++m_iStreamID)) == NULL )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "The plugin %s (%d) returned a NULL media stream object",
													pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_sName.c_str(),
													pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_dwPK_Device);
			return pMediaStream;
		}

		// ContainsVideo needs this too
	    pMediaStream->m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;

        pMediaStream->m_pOH_Orbiter_StartedMedia = pOH_Orbiter;
		if( pOH_Orbiter && pOH_Orbiter->m_pOH_User )
			pMediaStream->m_iPK_Users = pOH_Orbiter->m_pOH_User->m_iPK_Users;

        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
    }

    pMediaStream->m_pMediaHandlerInfo = pMediaHandlerInfo;
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
	    pMediaStream->m_mapEntertainArea[pEntertainArea->m_iPK_EntertainArea] = pEntertainArea;
	}
	pMediaStream->m_iRepeat=iRepeat;

    // HACK: get the user if the message originated from an orbiter!

    // Todo -- get the real remote
	if( PK_DesignObj_Remote && PK_DesignObj_Remote!=-1 )
        pMediaStream->m_iPK_DesignObj_Remote = PK_DesignObj_Remote;

	pMediaStream->m_bResume=bResume;
	g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from StartMedia");
	if( StartMedia(pMediaStream) )
		return pMediaStream;
	return NULL;
}

class OldStreamInfo
{
public:
	EntertainArea *m_pEntertainArea;
	bool m_bNoChanges;
	int m_PK_MediaType_Prior;
	map<int,MediaDevice *> m_mapMediaDevice_Prior;
	OldStreamInfo(EntertainArea *pEntertainArea) { m_pEntertainArea=pEntertainArea; m_bNoChanges=false; m_PK_MediaType_Prior=0; }
};

bool Media_Plugin::StartMedia(MediaStream *pMediaStream)
{
	map<int,class OldStreamInfo *> mapOldStreamInfo;

	if( !pMediaStream->m_pMediaDevice_Source )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot start media without a source");
		return false;
	}

	for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;

		if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream!=pMediaStream )
		{
			OldStreamInfo *pOldStreamInfo = new OldStreamInfo(pEntertainArea);
			mapOldStreamInfo[pEntertainArea->m_iPK_EntertainArea]=pOldStreamInfo;
			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&pOldStreamInfo->m_mapMediaDevice_Prior);
			pOldStreamInfo->m_PK_MediaType_Prior = pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType;

			// this will reset the current media stream if the pMediaHandlerBase is different from the original media Handler.
			// We should also look at the media types. If the current Media type is different then we will also do a new media stream.
			if ( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase == pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase &&
				pEntertainArea->m_pMediaStream->m_iPK_MediaType == pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType )
				pOldStreamInfo->m_bNoChanges = true;

			// If Resume is set, then this media is just a temporary stream, like an announcement, and if something
			// is currently playing, we will store that stream here and resume playing it when the temporary
			// stream is done.  But don't resume another stream that also has a resume, since it too was just 
			// an announcement.
			if( pMediaStream->m_bResume && !pEntertainArea->m_pMediaStream->m_bResume )
				pEntertainArea->m_vectMediaStream_Interrupted.push_back(pEntertainArea->m_pMediaStream);

			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pEntertainArea->m_pMediaStream );
			g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::StartMedia(): Calling Stream ended after the Stop Media");
			StreamEnded(pEntertainArea->m_pMediaStream,false,pMediaStream->m_bResume ? false : true,pMediaStream);  // Don't delete it if we're going to resume
			g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::StartMedia(): Call completed.");

		}

		pEntertainArea->m_pMediaStream=pMediaStream;
		if( pMediaStream->ContainsVideo() )
			EVENT_Watching_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
		else
			EVENT_Listening_to_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
	}

	// If the media is only playing in 1 entertainment area and the source device is in the same entertainment
	// area where we are playing, we will assume the source device is also the rendering device.  In other words,
	// if we have chosen to play a movie with Xine #1 in Ent Area #2, and if Xine #1 is in Ent Area #2, normally
	// Xine #1 is both the source and the destination.  However, if we are playing media on a device in a different
	// entertainment area, or in multiple ones, then we must be streaming.  So the media handler's start media
	// will be responsible for determining the rendering device for each entertainment area.
	if( pMediaStream->m_mapEntertainArea.size()==1 )
	{
		EntertainArea *pEntertainArea = pMediaStream->m_mapEntertainArea.begin()->second;
		if( pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.find( pEntertainArea->m_iPK_EntertainArea )!=
			pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.end() )
			pEntertainArea->m_pMediaDevice_ActiveDest=pMediaStream->m_pMediaDevice_Source;
	}
	else
		g_pPlutoLogger->Write(LV_STATUS,"Calling Plug-in's start media");

	g_pPlutoLogger->Write(LV_STATUS,"Ready to call plugin's startmedia");

	if( pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pMediaStream) )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Plug-in started media");

		for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
		{
			EntertainArea *pEntertainArea = ( *it ).second;
			OldStreamInfo *pOldStreamInfo = mapOldStreamInfo[pEntertainArea->m_iPK_EntertainArea];
			if( !pOldStreamInfo || !pOldStreamInfo->m_bNoChanges )
			{
				// We need to get the current rendering devices so that we can send on/off commands
				map<int,MediaDevice *> mapMediaDevice_Current;
				// only do stuff with valid objects
				pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&mapMediaDevice_Current);
				HandleOnOffs(pOldStreamInfo ? pOldStreamInfo->m_PK_MediaType_Prior : 0,
					pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType,
					pOldStreamInfo ? &pOldStreamInfo->m_mapMediaDevice_Prior : NULL,
					&mapMediaDevice_Current);
			}
		}

		// If this is just an announcement don't bother sending the orbiters to the remote screen
		if( pMediaStream->m_bResume )
		{
			g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::StartMedia() done - not sending to remote since stream is marked resumte");
			return true;
		}

		// See if there's a special screen for the OSD
		pMediaStream->m_iPK_DesignObj_RemoteOSD = pMediaStream->SpecialOsdScreen();

		if( pMediaStream->m_iPK_DesignObj_Remote>0 )
		{
			for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
			{
				OH_Orbiter *pOH_Orbiter = (*it).second;
				if( !pOH_Orbiter->m_pEntertainArea || pMediaStream->m_mapEntertainArea.find(pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea)==pMediaStream->m_mapEntertainArea.end() )
					continue;  // Don't send an orbiter to the remote if it's not linked to an entertainment area where we're playing this stream

				// We don't want to change to the remote screen on the orbiter that started playing this if it's audio, so that they can build a playlist
				if( pOH_Orbiter && pOH_Orbiter == pMediaStream->m_pOH_Orbiter_StartedMedia && pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST )
					continue;

				int iPK_DesignObj_Remote = pMediaStream->m_iPK_DesignObj_Remote;

				// If there's another user in the entertainment area that is in the middle of something (ie the Orbiter is not at the main menu),
				// we don't want to forcibly 'take over' and change to the remote screen. So we are only goind to send the orbiters controlling this ent area
				// to the correct remote iff thery are the main menu EXCEPT for the originating device which will always be send to the remote.
				// If this is the OSD and there's a special screen, don't switch, we'll do it below
				if( !pMediaStream->m_iPK_DesignObj_RemoteOSD || !pMediaStream->OrbiterIsOSD(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device) )
				{
					if( pOH_Orbiter && pOH_Orbiter == pMediaStream->m_pOH_Orbiter_StartedMedia )
					{
						DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,
							StringUtils::itos(iPK_DesignObj_Remote),"","",false, false);
						SendCommand(CMD_Goto_Screen);
					}
					else
					{
						DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,
							StringUtils::itos(iPK_DesignObj_Remote),"","-1",false, false);
						SendCommand(CMD_Goto_Screen);
					}
				}
			}
		}
		else
			g_pPlutoLogger->Write(LV_CRITICAL,"Media Plug-in -- there's no remote control for screen.");


		if ( pMediaStream->m_iPK_DesignObj_RemoteOSD ) // this needs special handling for the OSD
		{
			for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
			{
				EntertainArea *pEntertainArea = ( *it ).second;

				// finally send the OnScreenDisplay orbiter (the one that is on the target media director) to the app_desktop screen.
				if( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD )
				{
					DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device,0,
						StringUtils::itos(pMediaStream->m_iPK_DesignObj_RemoteOSD),"","",false, false);
					SendCommand(CMD_Goto_Screen);
				}
			}
		}
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Media Plug-in's call to Start Media failed 2.");

	g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::StartMedia() function call completed with honors!");

	return true;
}

class DataGridTable *Media_Plugin::CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    class MediaStream *pMediaStream = DetermineStreamOnOrbiter( pMessage->m_dwPK_Device_From, true );

    if ( pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The message was not from an orbiter or it doesn't have a media stream on it!");
        return NULL;
    }

    DataGridTable *pDataGrid = new DataGridTable();
    // DataGridCell *pCell = NULL;

    deque<MediaFile *>::iterator itFiles;
    string sCurrentFile;

    int currentPos = 0;
    for ( itFiles = pMediaStream->m_dequeMediaFile.begin(); itFiles != pMediaStream->m_dequeMediaFile.end(); itFiles++ )
    {
		MediaFile *pMediaFile = *itFiles;
        // int index = itFiles - pMediaStream->m_dequeMediaFile.begin();
		sCurrentFile = pMediaFile->m_sDescription.size() ? pMediaFile->m_sDescription : FileUtils::FilenameWithoutPath(pMediaFile->FullyQualifiedFile());

        pDataGrid->SetData(0, currentPos++,new DataGridCell(sCurrentFile, StringUtils::itos(itFiles - pMediaStream->m_dequeMediaFile.begin())));
		g_pPlutoLogger->Write(LV_STATUS, "Returning data: (%d) -> %s", itFiles - pMediaStream->m_dequeMediaFile.begin(), ((*itFiles)->m_sFilename).c_str());
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

		// Add some stuff to the message parameters
		pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST] = StringUtils::itos(pEntertainArea->m_pMediaStream->m_iStreamID_get());

		if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source &&
			pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND &&
			(pMessage->m_dwID==COMMAND_Change_Playback_Speed_CONST ||
			pMessage->m_dwID==COMMAND_Pause_Media_CONST ||
			pMessage->m_dwID==COMMAND_Play_Media_CONST) )
		{
			MediaDevice *pMediaDevice = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source;
			if( pMessage->m_dwID==COMMAND_Pause_Media_CONST && pMediaDevice->m_iLastPlaybackSpeed!=0 )
			{
				OverrideNowPlaying(pEntertainArea->m_pMediaStream,"(pause) " + pEntertainArea->m_pMediaStream->m_sMediaDescription);
				pMediaDevice->m_iLastPlaybackSpeed = 0;
			}
			else if( pMessage->m_dwID==COMMAND_Pause_Media_CONST )
			{
				// It's the second pause in a row.  Change it to a play
				pMessage->m_dwID=COMMAND_Change_Playback_Speed_CONST;
				pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] = "1000";
				pMediaDevice->m_iLastPlaybackSpeed = 1000;
				OverrideNowPlaying(pEntertainArea->m_pMediaStream,pEntertainArea->m_pMediaStream->m_sMediaDescription);
			}
			else if( pMessage->m_dwID==COMMAND_Play_Media_CONST )
			{
				OverrideNowPlaying(pEntertainArea->m_pMediaStream,pEntertainArea->m_pMediaStream->m_sMediaDescription);
				pMediaDevice->m_iLastPlaybackSpeed = 1000;
			}
			else if( pMessage->m_dwID==COMMAND_Change_Playback_Speed_CONST )
			{
				string sValue = pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST];
				if( sValue.size() )
				{
					int iValue=1;
					if( sValue[0]=='+' )
						iValue = (sValue.size()>1 ? atoi(sValue.substr(1).c_str()) : 1);
					else if( sValue[0]=='-' )
						iValue = (sValue.size()>1 ? atoi(sValue.substr(1).c_str()) : 1) * -1;
					else
						iValue = atoi(sValue.c_str());

					// Relative speed
					if( abs(iValue)<10 && iValue )
					{
						// Nothing does more than 64x, so if we're already more than 32, go back to normal
						if( abs(pMediaDevice->m_iLastPlaybackSpeed)>32000 )
							pMediaDevice->m_iLastPlaybackSpeed = 1000;

						// We're changing directions to reverse, start at -1000
						else if( iValue<0 && pMediaDevice->m_iLastPlaybackSpeed>0 )
							pMediaDevice->m_iLastPlaybackSpeed=-1000;

						// We're changing directions to forward, start at 1000
						else if( iValue>0 && pMediaDevice->m_iLastPlaybackSpeed<0 )
							pMediaDevice->m_iLastPlaybackSpeed=1000;

						// We were paused and now got a forward.  Start out in slow motion
						else if( iValue>0 && pMediaDevice->m_iLastPlaybackSpeed==0 )
							pMediaDevice->m_iLastPlaybackSpeed=250;

						// We were paused and now got a reverse.  Start out in slow motion
						else if( iValue<0 && pMediaDevice->m_iLastPlaybackSpeed==0 )
							pMediaDevice->m_iLastPlaybackSpeed=-250;

						else if( iValue<0 )
							pMediaDevice->m_iLastPlaybackSpeed =
								-1 * abs(pMediaDevice->m_iLastPlaybackSpeed * iValue);
						else
							pMediaDevice->m_iLastPlaybackSpeed =
								abs(pMediaDevice->m_iLastPlaybackSpeed * iValue);
g_pPlutoLogger->Write(LV_STATUS,"relative Playback speed now %d for device %d %s",
pMediaDevice->m_iLastPlaybackSpeed,
pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

						pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] =
							StringUtils::itos(pMediaDevice->m_iLastPlaybackSpeed);
					}
					else
						pMediaDevice->m_iLastPlaybackSpeed = pMediaDevice->m_iLastPlaybackSpeed;
				}
				if( pMediaDevice->m_iLastPlaybackSpeed==1 )
					OverrideNowPlaying(pEntertainArea->m_pMediaStream,pEntertainArea->m_pMediaStream->m_sMediaDescription);
				else
					OverrideNowPlaying(pEntertainArea->m_pMediaStream,"(" +
						StringUtils::itos(pMediaDevice->m_iLastPlaybackSpeed/1000) + "x) "+ pEntertainArea->m_pMediaStream->m_sMediaDescription);
			}
g_pPlutoLogger->Write(LV_STATUS,"Playback speed now %d for device %d %s",
pMediaDevice->m_iLastPlaybackSpeed,
pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

		}

        // We know it's derived from CommandImpl
        class Command_Impl *pPlugIn = pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl;
        g_pPlutoLogger->Write( LV_STATUS, "Checking to see if the plugin %s will handle it!", pPlugIn->m_sName.c_str());
        pMessage->m_dwPK_Device_To=pPlugIn->m_dwPK_Device;
		// Don't forward to the generic handler--it's just ourself
        if( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo || !pPlugIn->ReceivedMessage( pMessage ) )
        {
            g_pPlutoLogger->Write( LV_STATUS, "Media plug in did not handled message id: %d forwarding to %d",
				pMessage->m_dwID, pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device );

			// TODO "Warning: received dcemessage should take a bool but don't delete in or something so we don't need to copy the message!"

			// If it's a command, it could be something for the media player (like a Pause), or something for the a/v equipment
			// that is connected to the media director (like vol up/down going to the receiver).  Don't bother with generic media
			if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo!=m_pGenericMediaHandlerInfo )
			{
			    Command *pCommand = m_pRouter->m_mapCommand_Find(pMessage->m_dwID);
				if( !pCommand )
					g_pPlutoLogger->Write(LV_CRITICAL,"Got a command in media plugin that we can't identify");
				else
				{
				    if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_MD &&
						(pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST || pCommand->m_dwPK_Command==COMMAND_Generic_On_CONST || pCommand->m_listPipe.size()) )
					{
g_pPlutoLogger->Write(LV_STATUS,"It's an on/off");
						pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_MD;
						Message *pNewMessage = new Message( pMessage );
						QueueMessageToRouter( pNewMessage );
					}

					if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapCommands.find(pMessage->m_dwID) !=
						pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapCommands.end() )
					{
g_pPlutoLogger->Write(LV_STATUS,"It's a valid command");
						pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
						Message *pNewMessage = new Message( pMessage );
						QueueMessageToRouter( pNewMessage );
					}

					// For streaming media the source and destination are often different.  Send it to the dest as well
				    if( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest!=pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
					{
						Message *pNewMessage = new Message( pMessage );
						pNewMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device;
						QueueMessageToRouter( pNewMessage );

						// It could be an a/v command that is routed through the m/d to a receiver/tv
						if( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD &&
							pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD != pNewMessage->m_dwPK_Device_To )
						{
							pNewMessage = new Message( pMessage );
							pNewMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD;
							QueueMessageToRouter( pNewMessage );
						}
					}

				}
			}
			else
			{
g_pPlutoLogger->Write(LV_STATUS,"Just send it to the media device");
				// Just send it to the media device.  We don't know what it is
                pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	            Message *pNewMessage = new Message( pMessage );
				QueueMessageToRouter( pNewMessage );
			}
        }

        g_pPlutoLogger->Write( LV_STATUS, "Media plug in done with message id: %d", pMessage->m_dwID );
    }

    g_pPlutoLogger->Write( LV_STATUS, "Media plug base done with id: %d", pMessage->m_dwID );

    return true;
}

void Media_Plugin::MediaInfoChanged( MediaStream *pMediaStream, bool bRefreshScreen )
{
    delete pMediaStream->m_pPictureData;
	pMediaStream->m_pPictureData = NULL;
    pMediaStream->m_iPictureSize = 0;
g_pPlutoLogger->Write(LV_STATUS, "Delete m_pPictureData - info changed");

	if( pMediaStream->m_dwPK_Disc )
	{
		vector<Row_Picture_Disc *> vectRow_Picture_Disc;
		m_pDatabase_pluto_media->Picture_Disc_get()->GetRows("FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc),&vectRow_Picture_Disc);

g_pPlutoLogger->Write(LV_STATUS, "Getting m_pPictureData for disc %d size %d",pMediaStream->m_dwPK_Disc,(int) vectRow_Picture_Disc.size());
		if( vectRow_Picture_Disc.size() )
	        pMediaStream->m_pPictureData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(vectRow_Picture_Disc[0]->FK_Picture_get()) + ".jpg", pMediaStream->m_iPictureSize);
	}

    if( !pMediaStream->m_pPictureData && pMediaStream->m_dequeMediaFile.size() )
    {
g_pPlutoLogger->Write(LV_STATUS, "We have %d media entries in the playback list", pMediaStream->m_dequeMediaFile.size());
		MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[pMediaStream->m_iDequeMediaFile_Pos];

g_pPlutoLogger->Write(LV_STATUS, "Got 2 picture data %p (FK_File: %d)", pMediaStream->m_pPictureData, pMediaFile->m_dwPK_File);
		if( pMediaFile->m_dwPK_File )
		{
			int PK_Picture=0;

	g_pPlutoLogger->Write(LV_STATUS, "Looking got media database file with ID: %d", pMediaFile->m_dwPK_File );
			Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(pMediaFile->m_dwPK_File);
			vector<Row_Picture_File *> vectRow_Picture_File;
			if ( pRow_File )
			{
	g_pPlutoLogger->Write(LV_STATUS, "We found a file row %p", pRow_File );
				pRow_File->Picture_File_FK_File_getrows(&vectRow_Picture_File);

	g_pPlutoLogger->Write(LV_STATUS, "We got %d rows.", vectRow_Picture_File.size() );
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
g_pPlutoLogger->Write(LV_STATUS, "Found PK_Picture to be: %d.", PK_Picture);
	        if( PK_Picture )
		        pMediaStream->m_pPictureData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + ".jpg", pMediaStream->m_iPictureSize);
		}
    }

g_pPlutoLogger->Write(LV_STATUS, "Ready to update bound remotes with %p %d",pMediaStream->m_pPictureData,pMediaStream->m_iPictureSize);

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
        for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
        {
            BoundRemote *pBoundRemote = ( *itBR ).second;
            pBoundRemote->UpdateOrbiter( pMediaStream );
			if( bRefreshScreen )
			{
				DCE::CMD_Regen_Screen CMD_Regen_Screen(m_dwPK_Device,pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
				SendCommand(CMD_Regen_Screen);
			}
        }
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
            if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
                m_pOrbiter_Plugin->SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pMediaStream->m_sMediaDescription );
        }
    }
}

void Media_Plugin::OverrideNowPlaying(MediaStream *pMediaStream,string sNowPlaying)
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
            if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
                m_pOrbiter_Plugin->SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, sNowPlaying );
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
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to play media
    DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, sPK_EntertainArea, vectEntertainArea );
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
		if( !pEntertainArea->m_pMediaStream )
			continue; // Don't know what area it should be played in, or there's no media playing there
		pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pEntertainArea->m_pMediaStream );
		g_pPlutoLogger->Write( LV_STATUS, "Called StopMedia" );
		StreamEnded(pEntertainArea->m_pMediaStream);
	}
}

void Media_Plugin::StreamEnded(MediaStream *pMediaStream,bool bSendOff,bool bDeleteStream,MediaStream *pMediaStream_Replacement,vector<EntertainArea *> *p_vectEntertainArea)
{
	if ( pMediaStream == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::StreamEnded() called with NULL MediaStream in it! Ignoring");
		return;
	}

	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// This could have been playing in lots of entertainment areas
    g_pPlutoLogger->Write( LV_STATUS, "Stream %s ended with %d ent areas", pMediaStream->m_sMediaDescription.c_str(), (int) pMediaStream->m_mapEntertainArea.size() );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
		map<int,MediaDevice *> mapMediaDevice_Prior;
		g_pPlutoLogger->Write( LV_STATUS, "Getting Render Devices" );
		pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&mapMediaDevice_Prior);

		bool bFireEvent=true;
		if( pMediaStream_Replacement &&
				pMediaStream_Replacement->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaStream_Replacement->m_mapEntertainArea.end() &&
				pMediaStream_Replacement->ContainsVideo()==pMediaStream->ContainsVideo() )
			bFireEvent=false;

		for(size_t s=0;p_vectEntertainArea && s<p_vectEntertainArea->size();++s)
			if( (*p_vectEntertainArea)[s]==pEntertainArea )
			{
				bFireEvent=false;
				break;
			}

		MediaInEAEnded(pEntertainArea,bFireEvent);

		// If the stream ended because we're starting a new one, don't bother resuming any pending streams.
		// We'll do that when this replacement stream ends
		if( pEntertainArea->m_vectMediaStream_Interrupted.size() && !pMediaStream_Replacement )
		{
			MediaStream *pMediaStream_Resume = pEntertainArea->m_vectMediaStream_Interrupted.back();
			// Don't pay attention if it's the stream we just interrupted that ended
			if( pMediaStream_Resume!=pMediaStream )
			{
				pEntertainArea->m_vectMediaStream_Interrupted.pop_back();
				g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from StreamEnded");
				StartMedia(pMediaStream_Resume);
			}
		}

		if( bSendOff )
			HandleOnOffs(pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType,0,&mapMediaDevice_Prior,NULL);
	}

	if( bDeleteStream )
	{
		// Just be sure this stream isn't still pending somewhere
		for( MapEntertainArea::iterator it=m_mapEntertainAreas.begin( );it!=m_mapEntertainAreas.end( );++it )
		{
			EntertainArea *pEntertainArea = ( *it ).second;

			for(vector<MediaStream *>::iterator itMSI=pEntertainArea->m_vectMediaStream_Interrupted.begin();itMSI!=pEntertainArea->m_vectMediaStream_Interrupted.end();)
			{
				if( *itMSI==pMediaStream )
				{
					itMSI=pEntertainArea->m_vectMediaStream_Interrupted.erase(itMSI);
				}
				else
					itMSI++;
			}
		}
	    delete pMediaStream;
	}
	else 
		pMediaStream->m_bStopped=true;
}

void Media_Plugin::MediaInEAEnded(EntertainArea *pEntertainArea,bool bFireEvent)
{
	if( bFireEvent )
	{
		if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->ContainsVideo() )
			EVENT_Stopped_Watching_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
		else
			EVENT_Stopped_Listening_To_Medi(pEntertainArea->m_pRoom->m_dwPK_Room);
	}

g_pPlutoLogger->Write( LV_STATUS, "Stream in ea %s ended %d remotes bound", pEntertainArea->m_sDescription.c_str(), (int) pEntertainArea->m_mapBoundRemote.size() );
	pEntertainArea->m_pMediaStream = NULL;
	pEntertainArea->m_pMediaDevice_ActiveDest=NULL;

	// Set all the now playing's to nothing
    for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
		{
g_pPlutoLogger->Write( LV_STATUS, "Orbiter %d %s in this ea to stop", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_sDescription.c_str());
            m_pOrbiter_Plugin->SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "" );
		}
    }

	// Send all bound remotes back home
	for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
    {
        BoundRemote *pBoundRemote = ( *itBR ).second;
g_pPlutoLogger->Write( LV_STATUS, "Orbiter %d %s is bound", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_sDescription.c_str());
		DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,"<%=M%>","","",false, false);
		SendCommand(CMD_Goto_Screen);
    }
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

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will send this
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );

   if( vectEntertainArea.size()!=1 || !vectEntertainArea[0]->m_pMediaStream )
    {
        g_pPlutoLogger->Write(LV_WARNING, "NULL entertainment area or NULL stream in the entertainment area for device %d", pMessage->m_dwPK_Device_From);
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"<%=T" + StringUtils::itos(TEXT_No_Media_CONST) + "%>",false,10,true);
        return; // Don't know what area it should be played in, or there's no media playing there
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];
   g_pPlutoLogger->Write(LV_STATUS, "Found entertainment area: (%d) %p %p", pEntertainArea->m_iPK_EntertainArea, pEntertainArea, pEntertainArea->m_pMediaStream);

   // Is the requesting orbiter the OSD?
	if( pEntertainArea->m_pMediaDevice_ActiveDest &&
		pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD &&
		pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device == (unsigned long)pMessage->m_dwPK_Device_From &&
		pEntertainArea->m_pMediaStream->m_iPK_DesignObj_RemoteOSD )
	{
	   g_pPlutoLogger->Write(LV_STATUS, "Stream media type: %d. This (%d) is an OSD Orbiter. Sending him to screen: %d",
               pMessage->m_dwPK_Device_From,
               pEntertainArea->m_pMediaStream->m_iPK_MediaType,
               pEntertainArea->m_pMediaStream->m_iPK_DesignObj_RemoteOSD);

	    DCE::CMD_Goto_Screen CMD_Goto_Screen( m_dwPK_Device, pMessage->m_dwPK_Device_From, 0, StringUtils::itos( pEntertainArea->m_pMediaStream->m_iPK_DesignObj_RemoteOSD ), "", "", false, false );
		SendCommand( CMD_Goto_Screen );
	}
	else
	{
       g_pPlutoLogger->Write(LV_STATUS, "Stream media type: %d. This (%d) is NOT an OSD Orbiter. Sending to screen: %d",
               pMessage->m_dwPK_Device_From,
               pEntertainArea->m_pMediaStream->m_iPK_MediaType,
               pEntertainArea->m_pMediaStream->m_iPK_DesignObj_Remote);

		if( pEntertainArea->m_pMediaStream->m_iPK_DesignObj_Remote>0 )
		{
		    DCE::CMD_Goto_Screen CMD_Goto_Screen( m_dwPK_Device, pMessage->m_dwPK_Device_From, 0, StringUtils::itos( pEntertainArea->m_pMediaStream->m_iPK_DesignObj_Remote ), "", "", false, false );
			SendCommand( CMD_Goto_Screen );
		}
	}
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

g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::CMD_Bind_to_Media_Remote(). Binding (%s) orbiter %d to device %d with cover art on the object: %s",
			sOnOff.c_str(),
			pMessage->m_dwPK_Device_From,
			iPK_Device, sPK_DesignObj.c_str());

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Only an Orbiter will send this
    OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );


	vector<EntertainArea *> vectEntertainArea;
	DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, sPK_EntertainArea, vectEntertainArea );
    if( vectEntertainArea.size()!=1 || !pOH_Orbiter )
    {
        sCMD_Result="Can't find ent area/orbiter";
        return; // Don't know what area it should be played in, or there's no media playing there
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];

	if( !atoi( sOnOff.c_str( ) ) ) // Off
        pEntertainArea->m_mapBoundRemote_Remove( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
    else
    {
		if( !pEntertainArea->m_pMediaStream )
		{
			sCMD_Result="No media stream";
			g_pPlutoLogger->Write(LV_CRITICAL,"Attempt to bind to a remote in an entertainment area with no media stream");
			return; // Don't know what area it should be played in, or there's no media playing there
		}

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
g_pPlutoLogger->Write(LV_STATUS,"EA %d %s bound %d remotes",pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str(),(int) pEntertainArea->m_mapBoundRemote.size());
    }
}


// #pragma warning( "put this in the mythtv plugin" )
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

	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,"|",pos).c_str());
	string AC = StringUtils::Tokenize(Parms,"|",pos);
	string sPK_Type="";
	if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
		sPK_Type="1"; // TYPE_Music_CONST;
	else if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
		sPK_Type="2,3,5"; // TYPE_Movie_CONST, TYPE_Home_Video_CONST, TYPE_TV_Show_CONST;

    string SQL = "select PK_Attribute, Name, FirstName, Description FROM Attribute " \
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_AttributeType=PK_AttributeType "\
        "WHERE ( Name Like '" + AC + "%' OR FirstName Like '" + AC + "%' ) AND Identifier>0 " +
		(sPK_Type.length() ? " AND FK_Type IN (" + sPK_Type + ") " : "") +
        "ORDER BY Name limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    string AttributesFirstSearch; // Because we're going to search twice and want to exclude any attributes we hit the first search

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = /* string( "~`S24`" ) + */ row[1];
            if( row[2] && *row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( "", row[0] );
            pDataGrid->SetData( 0, RowCount, pCell );

            if( AttributesFirstSearch.length() )
                AttributesFirstSearch += ",";
            AttributesFirstSearch += row[0];

            pCell = new DataGridCell( label, row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }

    SQL = "select PK_Attribute, Name, FirstName, Description FROM SearchToken "\
        "JOIN SearchToken_Attribute ON PK_SearchToken=FK_SearchToken "\
        "JOIN Attribute ON SearchToken_Attribute.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_AttributeType=PK_AttributeType "\
        "WHERE Token like '" + AC + "%' " +
		(AttributesFirstSearch.length() ? "AND PK_Attribute NOT IN (" + AttributesFirstSearch + ") " : "") +
		(sPK_Type.length() ? " AND FK_Type IN (" + sPK_Type + ") " : "") +
		" ORDER BY Name "\
        "limit 30;";

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
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
        return new DataGridTable();;
    }

    string SQL="select DISTINCT Dest.FK_File, Attribute.Name, Attribute.FirstName, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
		"ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 ORDER BY Name limit 200;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
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

	if( Parms.length()==0 )
		return pDataGrid;

    string PK_Attribute = Parms;
    if( PK_Attribute.substr( 0, 2 )=="#A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="#F" )
        PK_Attribute = StringUtils::itos(
            m_pMediaAttributes->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) )
         );

    if ( PK_Attribute == "" )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return new DataGridTable();
    }

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, Attribute.FirstName, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute='" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        "' JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=2 ORDER BY Name limit 100;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
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

	string::size_type pos=0;
	string PK_MediaType = StringUtils::Tokenize(Parms,"|",pos);
	string PK_Attribute = StringUtils::Tokenize(Parms,"|",pos);
    g_pPlutoLogger->Write(LV_STATUS, "Got this PK_Attributte: %s", PK_Attribute.c_str());

	if( PK_Attribute.length()==0 )
		return pDataGrid;

	if( PK_Attribute.substr( 0, 2 )=="#A" )
        PK_Attribute = PK_Attribute.substr( 2 );

    int PK_Picture;
    string Extension;

	if( PK_Attribute.substr( 0, 2 )=="#F" )
	{
        Extension = m_pMediaAttributes->GetPictureFromFileID(atoi(PK_Attribute.substr(2).c_str()),&PK_Picture);
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );
	}
	else
        Extension = m_pMediaAttributes->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

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
        "WHERE Identifier>=2 ORDER BY AttributeType.Description limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
g_pPlutoLogger->Write(LV_STATUS, "Transformed 2 PK_Attributte: %s", PK_Attribute.c_str());
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
g_pPlutoLogger->Write(LV_STATUS, "Transformed 4 PK_Attributte: %s", PK_Attribute.c_str());
            string label = row[1];
            if( row[2] && *row[2] )
                label += string( ", " ) + row[2];
            label += string( "\n" ) + row[3];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
g_pPlutoLogger->Write(LV_STATUS, "Transformed 3 PK_Attributte: %s", PK_Attribute.c_str());

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

	pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

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
//
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
                "WHERE Identifier>2 AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) + " ORDER BY Description";
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

    if( (result.r=m_pDatabase_pluto_media->mysql_query_result(SQL)) )
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

class DataGridTable *Media_Plugin::DevicesPipes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaDevice_ActiveDest )
		return pDataGrid;

	int iRow=0;
	DevicesPipes_Loop( pMessage->m_dwPK_Device_From, pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router,
		pDataGrid,iRow);
	return pDataGrid;
}


void Media_Plugin::DevicesPipes_Loop(int PK_Orbiter,DeviceData_Router *pDevice,DataGridTable *&pDataGrid,int &iRow,int PK_Command_Input,int PK_Command_Output,vector<int> *p_vectDevice)
{
	if( !pDevice )
		return;

	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDevice->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDevice->m_dwPK_Device);

	for(map<int, class Pipe *>::iterator it=pDevice->m_mapPipe_Active.begin();
		it!=pDevice->m_mapPipe_Active.end();++it)
	{
		Pipe *pPipe = (*it).second;
		DeviceData_Router *pDevice_Pipe = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get());
		if( pDevice_Pipe && pDevice_Pipe!=pDevice )
			DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *)pDevice_Pipe,pDataGrid,iRow,pPipe->m_pRow_Device_Device_Pipe->FK_Command_Input_get(),pPipe->m_pRow_Device_Device_Pipe->FK_Command_Output_get(),p_vectDevice);
	}

	if( pDevice->m_pDevice_MD && pDevice!=pDevice->m_pDevice_MD )
		DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *) pDevice->m_pDevice_MD,pDataGrid,iRow,0,0,p_vectDevice);

	DataGridCell *pCell = new DataGridCell( pDevice->m_sDescription);
	pCell->m_Colspan = 4;
	pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Pipe_CONST,"-1");
	pDataGrid->SetData(0,iRow,pCell);

	if( PK_Command_Input )
	{
		Command *pCommand = m_pRouter->m_mapCommand_Find( PK_Command_Input );
		if( pCommand )
		{
			pCell = new DataGridCell( pCommand->m_sDescription );
			pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Input_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Input_CONST,StringUtils::itos(pCommand->m_dwPK_Command).c_str());
			pDataGrid->SetData(4,iRow,pCell);
		}
	}
	if( PK_Command_Output )
	{
		Command *pCommand = m_pRouter->m_mapCommand_Find( PK_Command_Output );
		if( pCommand )
		{
			pCell = new DataGridCell( pCommand->m_sDescription );
			pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Output_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Output_CONST,StringUtils::itos(pCommand->m_dwPK_Command).c_str());
			pDataGrid->SetData(5,iRow,pCell);
		}
	}

	pCell = new DataGridCell( "Advanced" );
    pCell->m_pMessage = new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Goto_Screen_CONST,
		1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuDeviceControl_CONST).c_str());
	pCell->m_pMessage->m_vectExtraMessages.push_back(
		new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
		COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_Datagrid_Input_CONST).c_str(),
		COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(pDevice->m_dwPK_Device).c_str())
	);

	pDataGrid->SetData(6,iRow++,pCell);

	if( bCreatedVect )
		delete p_vectDevice;
}

class DataGridTable *Media_Plugin::MediaAttrCurStream( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
		return pDataGrid;

	int iRow=0;
	for( map<int,int>::iterator it=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.begin();it!=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.end();++it)
	{
		Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
		if( pRow_AttributeType && pRow_Attribute )
		{
			string sName = pRow_AttributeType->Description_get() + ": " + m_pMediaAttributes->GetPrintableName(pRow_Attribute);
			DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}
	for(size_t s=0;s<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->m_dequeMediaFile[s];
		for( map<int,int>::iterator it=pMediaFile->m_mapPK_Attribute.begin();it!=pMediaFile->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
			if( pRow_AttributeType && pRow_Attribute )
			{
				string sName = "(" + pMediaFile->m_sFilename + ") ";
				sName += pRow_AttributeType->Description_get() + ": " + m_pMediaAttributes->GetPrintableName(pRow_Attribute);
				DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	return pDataGrid;
}

void Media_Plugin::DetermineEntArea( int iPK_Device_Orbiter, int iPK_Device, string sPK_EntertainArea, vector<EntertainArea *> &vectEntertainArea )
{
    g_pPlutoLogger->Write(LV_STATUS, "DetermineEntArea1");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // If we don't have an entertainment area, but we do have a device that is a media device we can find it there
    if( sPK_EntertainArea.size()==0 && iPK_Device )
    {
        MediaDevice *pMediaDevice = m_mapMediaDevice_Find( iPK_Device );
        if( pMediaDevice && pMediaDevice->m_mapEntertainArea.size( ) )
        {
            EntertainArea *pEntertainArea = pMediaDevice->m_mapEntertainArea.begin()->second;
            vectEntertainArea.push_back(pEntertainArea);
        }
    }

    g_pPlutoLogger->Write(LV_STATUS, "DetermineEntArea2");
    // See if we need to figure out the entertainment area on our own. If so, the only way to do this is if the message came from an orbiter
    if( sPK_EntertainArea.size()==0 )
    {
        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( iPK_Device_Orbiter );
		MediaDevice *pMediaDevice;
		if( iPK_Device && (pMediaDevice = m_mapMediaDevice_Find(iPK_Device))!=NULL )
		{
			// Maybe there's a specific media device we should use
			for( MapEntertainArea::iterator it=pMediaDevice->m_mapEntertainArea.begin( );it!=pMediaDevice->m_mapEntertainArea.end( );++it )
				vectEntertainArea.push_back(( *it ).second);
		}
        else if( !pOH_Orbiter )
        {
			g_pPlutoLogger->Write( LV_CRITICAL, "Received a play media with no entertainment area/device from a non-orbiter %d %d %s",iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
			return; // Don't know what area it should be played in
        }
        else if( !pOH_Orbiter->m_pEntertainArea )
        {
			// It could be an orbiter that was recently added
			for( map<int, EntertainArea *>::iterator itEntArea=m_mapEntertainAreas.begin( );itEntArea!=m_mapEntertainAreas.end( );++itEntArea )
			{
				class EntertainArea *pEntertainArea = ( *itEntArea ).second;
				if( pEntertainArea->m_pRoom && pEntertainArea->m_pRoom->m_dwPK_Room == pOH_Orbiter->m_dwPK_Room )
				{
		            vectEntertainArea.push_back(pEntertainArea);
					break;
				}
			}

			g_pPlutoLogger->Write(LV_STATUS, "DetermineEntArea4");
			if( !vectEntertainArea.size()==0 )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Received a DetermineEntArea from an orbiter %p %d with no entertainment area (%p) %d %d %s",
					pOH_Orbiter,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pEntertainArea,
					iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
				return; // Don't know what area it should be played in
			}
        }
		else
			vectEntertainArea.push_back(pOH_Orbiter->m_pEntertainArea);
    }

    g_pPlutoLogger->Write(LV_STATUS, "Found the proper ent area: %d", (int) vectEntertainArea.size());
	string::size_type pos=0;
	while( pos<sPK_EntertainArea.size() )
	{
		string s = StringUtils::Tokenize(sPK_EntertainArea,",",pos);
		EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(s.c_str()) );
		if( !pEntertainArea )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Received a play media for an invalid entertainment area %d %d %s",iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
			return; // Don't know what area it should be played in
		}
		vectEntertainArea.push_back(pEntertainArea);
	}
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

    return pOH_Orbiter->PK_Users_get();
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
		/** @param #116 Resume */
			/** If true, when this media finishes, resume whatever was playing previously.  Useful for making announcements and similar. */
		/** @param #117 Repeat */
			/** 0=default for media type, 1=loop, -1=do not loop */

void Media_Plugin::CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,string &sCMD_Result,Message *pMessage)
//<-dceag-c43-e->
{
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);

	int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
	vector<EntertainArea *> vectEntertainArea;

	// Only an Orbiter will tell us to play media
    DetermineEntArea(iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea,vectEntertainArea);
    if( vectEntertainArea.size()==0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Got a play media for no ent area");
        return;  // Don't know what area it should be played in
	}

	// i get the media type here when it's global
    deque<MediaFile *> dequeMediaFile;
    if( sFilename.length() > 0 )
 	{
		if ( sFilename[sFilename.length() - 1] == '*' )
		{
			list<string> allFilesList;
			list<string>::const_iterator itFileName;

			if ( iPK_MediaType == 0 )
			{
				g_pPlutoLogger->Write(LV_WARNING, "When you want a recursive listing of a folder you should also provide a target media type.");
				return;
			}

			Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(iPK_MediaType);
			if( !pRow_MediaType )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Invalid media type PK provided: %d", iPK_MediaType);
				return;
			}

			string Extensions = pRow_MediaType->Extensions_get();
			g_pPlutoLogger->Write(LV_STATUS, "Using there extensions to filter the results: %s", Extensions.c_str());
			if ( FileUtils::FindFiles(allFilesList, sFilename.substr(0, sFilename.length()-1), Extensions.c_str(), true, true) ) // we have hit the bottom
			{
				m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter, "You have more than 100 pictures in this folder. Only the first 100 of them have been added to the queue.");
			}

			itFileName = allFilesList.begin();
			while ( itFileName != allFilesList.end() )
				dequeMediaFile.push_back(new MediaFile(m_pMediaAttributes, *itFileName++));
		}
		else
			m_pMediaAttributes->TransformFilenameToDeque(sFilename, dequeMediaFile);  // This will convert any #a, #f, etc.
 	}

    // What is the media?  It must be a Device, DeviceTemplate, or a media type, or filename
    if( !iPK_MediaType && (iPK_Device || iPK_DeviceTemplate) )
    {
		vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
		Row_DeviceTemplate *pRow_DeviceTemplate=NULL;
		if( iPK_Device )
		{
			Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
			if( pRow_Device )
				pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
		}

		if( !pRow_DeviceTemplate && iPK_DeviceTemplate )
			pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);

		if( pRow_DeviceTemplate )
			pRow_DeviceTemplate->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);

		if( vectRow_DeviceTemplate_MediaType.size()!=1 )
			g_pPlutoLogger->Write(LV_STATUS,"Cannot decisively determine media type for %d/%d, rows: %d",
				iPK_Device,iPK_DeviceTemplate,(int) vectRow_DeviceTemplate_MediaType.size());

		if( vectRow_DeviceTemplate_MediaType.size() )
		{
			Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[0];
			iPK_MediaType = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
		}
    }

    if( vectEntertainArea.size()==1 && dequeMediaFile.size()==0 && iPK_MediaType==0 ) // We got nothing -- find a disk drive within the entertainment area and send it a reset
    {
		EntertainArea *pEntertainArea = vectEntertainArea[0];
		bool bDiskIsRipping = false;
		bool bDiskWasReset  = false;
        for(map<int,class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin();itDevice!=pEntertainArea->m_mapMediaDevice.end();++itDevice)
        {
            class MediaDevice *pMediaDevice = (*itDevice).second;
            if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory==DEVICECATEGORY_Disc_Drives_CONST )
            {
				if ( ! DiskDriveIsRipping(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device) )
				{
					pMediaDevice->m_pOH_Orbiter_Reset = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Device_Orbiter);
					pMediaDevice->m_tReset = time(NULL);

					DCE::CMD_Reset_Disk_Drive CMD_Reset_Disk_Drive(m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
                	SendCommand(CMD_Reset_Disk_Drive);
					bDiskWasReset = true;
				}
				else
				{
					bDiskIsRipping = true;
				}
            }
        }

		if ( !bDiskWasReset && bDiskIsRipping ) // we weren't able to reset any drives and at least one of them was ripping.
		{
			m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From, "There is a ripping operation in progress at this moment. Can't reset the disk.",false,10,true);
		}
    }
	else
		StartMedia(iPK_MediaType,iPK_Device_Orbiter,vectEntertainArea,iPK_Device,sPK_DesignObj,&dequeMediaFile,bResume,iRepeat);  // We'll let the plug-in figure out the source, and we'll use the default remote
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

	vector<EntertainArea *> vectEntertainArea;
    // Only an Orbiter will tell us to jump to next position
    DetermineEntArea( iPK_Device_Orbiter, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()!=1 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got jump position but i couldn't find an entertainment area for it!");
        return;
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];

    if ( pEntertainArea->m_pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Can't jump in an empty queue");
        return;
    }

	if( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo )
	{
        g_pPlutoLogger->Write(LV_STATUS, "It's a generic stream--just forward it to the device");
        pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	    Message *pNewMessage = new Message( pMessage );
		QueueMessageToRouter( pNewMessage );
		return;
	}

	// update the orbiter object to be the one that commanded this.
	pEntertainArea->m_pMediaStream->m_pOH_Orbiter_StartedMedia = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( iPK_Device_Orbiter );

	// Made the Jump to position to be the responsibility of hte Media stream. Most of them will just inherit the previous behaviour
	// +1 next in playlist
	// -1 prev in playlist
	// number select that channel
	//
	// but a few specific ones (like the MythTV MediaStream need to so special handling here)
	if ( pEntertainArea->m_pMediaStream->ProcessJumpPosition(sValue_To_Assign) ) // if the specification was valid for this stream then continue processing in the media plugin
	{
		g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from JumpPos (handler)");
    	pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pEntertainArea->m_pMediaStream);
	}

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
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Find the corrent entertainment area to use.
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );

    if ( vectEntertainArea.size()!=1 || vectEntertainArea[0]->m_pMediaStream == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't find a valid entertainment area with a valid stream in it. Can't save the playlist");
        return;
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];
    if ( sName == "" )
        sName = pEntertainArea->m_pMediaStream->m_sPlaylistName;

    if ( sName == "" )
        sName = "New Playlist";

    int iPK_Playlist = bSave_as_new ? 0 : pEntertainArea->m_pMediaStream->m_iPK_Playlist;
    if( !m_pMediaAttributes->SavePlaylist(pEntertainArea->m_pMediaStream->m_dequeMediaFile, iPK_Users, iPK_Playlist, sName) )
    {
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to save playlist");
        m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to save playlist",false,10,true);
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
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Find the corrent entertainment area to use.
	vector<EntertainArea *> vectEntertainArea;

	DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );
    if ( vectEntertainArea.size()==0 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't find a valid entertainment area to load the playlist into.");
        return;
    }

    string sPlaylistName;
    deque<MediaFile *> dequeMediaFile;
    if( !m_pMediaAttributes->LoadPlaylist( iEK_Playlist, dequeMediaFile, sPlaylistName) || dequeMediaFile.size()==0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "I was unable to load playlist entries");
        m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to load playlist",false,10,true);
        return;
    }

    string Extension = StringUtils::ToUpper(FileUtils::FindExtension(dequeMediaFile[0]->m_sFilename));
	map<int,MediaHandlerInfo *> mapMediaHandlerInfo;

	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea=vectEntertainArea[s];

		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find(Extension);
		if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size()==0 )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Play playlist %s in entertain area %s but nothing to handle it",sPlaylistName.c_str(),sPK_EntertainArea.c_str());
			return;
		}
		MediaHandlerInfo *pMediaHandlerInfo = pList_MediaHandlerInfo->front();
		if( pMediaHandlerInfo )
			mapMediaHandlerInfo[pMediaHandlerInfo->m_MediaHandlerID]=pMediaHandlerInfo;
	}
	for(map<int,MediaHandlerInfo *>::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it)
	{
		g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from Load Playlist");
	    StartMedia(it->second,pMessage->m_dwPK_Device_From,vectEntertainArea,0,0,&dequeMediaFile,false,0);  // We'll let the plug-in figure out the source, and we'll use the default remote
	}
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
    g_pPlutoLogger->Write(LV_STATUS, "Ready to populate active media stream...");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    int iRow=0;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMediaStream = (*it).second;
		bool bPrivate=false;
		for( map<int,class EntertainArea *>::iterator itEntAreas=pMediaStream->m_mapEntertainArea.begin(); itEntAreas != pMediaStream->m_mapEntertainArea.end(); itEntAreas++)
		{
			if( itEntAreas->second->m_bMediaIsPrivate )
			{
				bPrivate=true;
				continue;
			}
		}
		if( bPrivate )
			continue;
        pMediaStream->m_iOrder = iRow;
        DataGridCell *pDataGridCell = new DataGridCell(pMediaStream->m_sMediaDescription,StringUtils::itos(pMediaStream->m_iStreamID_get()));
        pDataGridCell->m_AltColor = UniqueColors[iRow];
        pDataGrid->SetData(0,iRow++,pDataGridCell);
    }
    g_pPlutoLogger->Write(LV_STATUS, "Done with active media streams...");
    return pDataGrid;
}

class DataGridTable *Media_Plugin::AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::AvailablePlaylists Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    if( Parms.length( )==0 )
        return NULL; // Nothing passed in yet

    // select PK_Playlist, Name from Playlist where EK_USER IN ( 0, 33128 ) LIMIT 30;;

    int userID = atoi(Parms.c_str());

    string SQL = "SELECT PK_Playlist, Name from Playlist where EK_USER IN ( 0, " + StringUtils::itos(userID) + " ) ORDER BY NAME LIMIT 30";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    DataGridTable *pDataGrid = new DataGridTable();
    if( (result.r=m_pDatabase_pluto_media->mysql_query_result(SQL)) )
        while( (row=mysql_fetch_row(result.r)) )
		{
            // 	g_pPlutoLogger->Write(LV_CRITICAL, "Adding this entry \"%s\" to the position %d", row[1], RowCount);
			pDataGrid->SetData(0,RowCount++,new DataGridCell(row[1], row[0]));
		}

    return pDataGrid;
}


void Media_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	if( pEntertainArea && pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_iOrder>=0 && pEntertainArea->m_pMediaStream->m_iOrder<MAX_MEDIA_COLORS )
	{
		Color = UniqueColors[pEntertainArea->m_pMediaStream->m_iOrder];
		sDescription = pEntertainArea->m_pMediaStream->m_sMediaDescription;
	}
	else
	{
		Color = PlutoColor::Black().m_Value;
		sDescription = "off";
	}
}
//<-dceag-c241-b->

	/** @brief COMMAND: #241 - MH Move Media */
	/** Moves an existing media stream to the specified entertainment areas. */
		/** @param #41 StreamID */
			/** The ID of the media stream to move */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area(s) to send the Media Stream to. */

void Media_Plugin::CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c241-e->
{
	MediaStream *pMediaStream;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	if ( (pMediaStream = m_mapMediaStream_Find(iStreamID)) == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "No media stream with ID %d available", iStreamID );
		return;
	}

	g_pPlutoLogger->Write(LV_WARNING,"Move Media, stream %d  ea: %s",iStreamID,sPK_EntertainArea.c_str());


	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea(0,0,sPK_EntertainArea,vectEntertainArea);

	bool bNothingMoreToPlay = vectEntertainArea.size()==0;
	g_pPlutoLogger->Write( LV_STATUS, "Calling StopMedia" );
	pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pMediaStream );
	g_pPlutoLogger->Write( LV_STATUS, "Called StopMedia" );
	StreamEnded(pMediaStream,true,bNothingMoreToPlay,NULL,&vectEntertainArea);

	if( !bNothingMoreToPlay )
	{

g_pPlutoLogger->Write(LV_WARNING,"ready to restart %d eas",(int) vectEntertainArea.size());
		// Be sure all outgoing stop messages are flushed before we proceed
		WaitForMessageQueue();
		pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->WaitForMessageQueue();

		// TODO --- Hmmm... I was passing in pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device
		// instead of 0, but that means that when moving from 1 xine to another, it thinks the source isn't the dest
		// and it doesn't play.  Change this to 0, but a better solution is probably that the plugin figures this out
		// since sometimes the source will be preserved across moves maybe???
		StartMedia( pMediaStream->m_iPK_MediaType, (pMediaStream->m_pOH_Orbiter_StartedMedia ? pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device : 0),
			vectEntertainArea, 0,
			StringUtils::itos(pMediaStream->m_iPK_DesignObj_Remote), 
			&pMediaStream->m_dequeMediaFile, pMediaStream->m_bResume, pMediaStream->m_iRepeat);

		pMediaStream->m_dequeMediaFile.clear();  // We don't want to delete the media files since we will have re-used the same pointers above
		delete pMediaStream; // We will have started with new copies
	}
}

void Media_Plugin::HandleOnOffs(int PK_MediaType_Prior,int PK_MediaType_Current, map<int,MediaDevice *> *pmapMediaDevice_Prior,map<int,MediaDevice *> *pmapMediaDevice_Current)
{// Is a specific pipe used?  If this is an audio stream only, the media type will have the pipe set to 1
	Row_MediaType *pRow_MediaType_Prior = PK_MediaType_Prior ? m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType_Prior) : NULL;
	Row_MediaType *pRow_MediaType_Current = PK_MediaType_Current ? m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType_Current) : NULL;

	int PK_Pipe_Prior = pRow_MediaType_Prior && pRow_MediaType_Prior->FK_Pipe_isNull()==false ? pRow_MediaType_Prior->FK_Pipe_get() : 0;
	int PK_Pipe_Current = pRow_MediaType_Current && pRow_MediaType_Current->FK_Pipe_isNull()==false ? pRow_MediaType_Current->FK_Pipe_get() : 0;

	map<int,MediaDevice *> mapMediaDevice_Current;
	// We don't want just the top-level devices, we want all the devices in the 'current' map so we don't
	// turn off something we're going to turn on again.  But we don't want to add them to pmapMediaDevice_Current
	// and then have to handle the pipes ourselves.  But this only matters if there are prior devices to turn off
	if( pmapMediaDevice_Prior && pmapMediaDevice_Prior->size() && pmapMediaDevice_Current && pmapMediaDevice_Current->size() )
	{
		mapMediaDevice_Current = *pmapMediaDevice_Current;
		AddOtherDevicesInPipesToRenderDevices(PK_Pipe_Current,&mapMediaDevice_Current);
	}

	if( pmapMediaDevice_Prior )
	{
		// If we watching a DVD, and the pipe was from the dvd player to a video scaler to the tv, and we are now watching
		// TV with the path from the media director to the tv, we want to turn off the scaler, but not the tv.  We will just
		// send the media director an on and let the framework propagate the input selection automatically.  However, we don't
		// want to just send the DVD player an off because then the framework will turn the tv off too.
		for(map<int,MediaDevice *>::iterator it=pmapMediaDevice_Prior->begin();it!=pmapMediaDevice_Prior->end();++it)
		{
			// We need a recursive function to propagate the off's along the pipe, but not turning off any devices
			// that we're still going to use in the current map
			if ( (*it).second )  // Obs: I got a crash here while testing mihai.t
				TurnDeviceOff(PK_Pipe_Prior,(*it).second->m_pDeviceData_Router,&mapMediaDevice_Current);
			else
			{
				g_pPlutoLogger->Write(LV_WARNING, "Found a NULL associated device in the HandleOnOff: %d", (*it).first);
			}
		}
	}

	if( !pmapMediaDevice_Current || !pmapMediaDevice_Current->size() )
		return; // Nothing to turn on -- we already turned everything off

	for(map<int,MediaDevice *>::iterator it=pmapMediaDevice_Current->begin();it!=pmapMediaDevice_Current->end();++it)
	{
		MediaDevice *pMediaDevice = (*it).second;

		if ( pMediaDevice == NULL )
		{
			g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::HandleOnOffs() There is a null device associated with the deviceID: %d. Ignoring device in HandleOnOff", (*it).first);
			continue;
		}

		DCE::CMD_On CMD_On(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,PK_Pipe_Current,"");
		SendCommand(CMD_On);

		if( pMediaDevice->m_pDeviceData_Router->m_pDevice_MD && pMediaDevice->m_pDeviceData_Router!=pMediaDevice->m_pDeviceData_Router->m_pDevice_MD )
		{
			DCE::CMD_On CMD_On(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device,PK_Pipe_Current,"");
			SendCommand(CMD_On);
		}
	}
}

void Media_Plugin::TurnDeviceOff(int PK_Pipe,DeviceData_Router *pDeviceData_Router,map<int,MediaDevice *> *pmapMediaDevice_Current,vector<int> *p_vectDevice)
{
	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDeviceData_Router->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDeviceData_Router->m_dwPK_Device);

	if( !pmapMediaDevice_Current || pmapMediaDevice_Current->find( pDeviceData_Router->m_dwPK_Device ) == pmapMediaDevice_Current->end() )
	{
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDeviceData_Router->m_dwPK_Device);
		// Don't turn the device off the OSD needs it on
		if( !pMediaDevice || !pMediaDevice->m_bDontSendOffIfOSD_ON || (pMediaDevice->m_pOH_Orbiter_OSD && pMediaDevice->m_pOH_Orbiter_OSD->m_bDisplayOn==false))
		{
			DCE::CMD_Off CMD_Off(m_dwPK_Device,pDeviceData_Router->m_dwPK_Device,-1);  // -1 means don't propagate to any pipes
			SendCommand(CMD_Off);
		}
	}

	if( pDeviceData_Router->m_pDevice_MD && pDeviceData_Router!=pDeviceData_Router->m_pDevice_MD )
		TurnDeviceOff(PK_Pipe,(DeviceData_Router *) pDeviceData_Router->m_pDevice_MD,pmapMediaDevice_Current,p_vectDevice);

    for(map<int,Pipe *>::iterator it=pDeviceData_Router->m_mapPipe_Available.begin();it!=pDeviceData_Router->m_mapPipe_Available.end();++it)
    {
        Pipe *pPipe = (*it).second;
		if( PK_Pipe && pPipe->m_pRow_Device_Device_Pipe->FK_Pipe_get()!=PK_Pipe )
			continue;

		DeviceData_Router *pDeviceData_RouterChild = m_pRouter->m_mapDeviceData_Router_Find( pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get() );
		if( pDeviceData_RouterChild )
			TurnDeviceOff(PK_Pipe,pDeviceData_RouterChild,pmapMediaDevice_Current,p_vectDevice);
	}

	if( bCreatedVect )
		delete p_vectDevice;
}
//<-dceag-createinst-b->!

//<-dceag-c269-b->

	/** @brief COMMAND: #269 - Move Playlist entry Up */
	/** Moves a entry up in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved up. */

void Media_Plugin::CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c269-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to Mode media
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()==0 )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Up() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Up(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	// the playlist is reversed
	pEntertainArea->m_pMediaStream->MoveEntryInPlaylist(iValue, -1);
}

//<-dceag-c270-b->

	/** @brief COMMAND: #270 - Move Playlist entry Down */
	/** Moves a entry down in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved down in the playlist. */

void Media_Plugin::CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c270-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Only an Orbiter will tell us to Mode media
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()!=0 )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Down() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Down(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	// the playlist is reversed
	pEntertainArea->m_pMediaStream->MoveEntryInPlaylist(iValue, +1);
}

//<-dceag-c271-b->

	/** @brief COMMAND: #271 - Remove playlist entry. */
	/** Removes an entry from the playlist. */
		/** @param #48 Value */
			/** The Id of the entry that needs to be removed from the playlist. */

void Media_Plugin::CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c271-e->
{	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Only an Orbiter will tell us to Mode media
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()!=1 )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Remove_playlist_entry() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Remove_playlist_entry(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	pEntertainArea->m_pMediaStream->DeleteEntryFromPlaylist(iValue);
}
//<-dceag-c331-b->

	/** @brief COMMAND: #331 - Get EntAreas For Device */
	/** Returns the EntArea(s) that a given device is in. */
		/** @param #2 PK_Device */
			/** The Device */
		/** @param #9 Text */
			/** A comma delimted list of EntAreas */

void Media_Plugin::CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c331-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	MediaDevice *pMediaDevice = NULL;
	while( pDevice && !pMediaDevice )
	{
		pMediaDevice = m_mapMediaDevice_Find( pDevice->m_dwPK_Device );
		pDevice = (DeviceData_Router *) pDevice->m_pDevice_ControlledVia;
	}

	if( !pMediaDevice )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot get ent area for non media device: %d", iPK_Device);
		return;
	}

	for(map<int,class EntertainArea *>::iterator it=pMediaDevice->m_mapEntertainArea.begin();it!=pMediaDevice->m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		(*sText) += StringUtils::itos(pEntertainArea->m_iPK_EntertainArea) + ",";
	}
}

void Media_Plugin::FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
	// See if we have any pending media for this user
	MediaStream *pMediaStream = NULL;
	time_t tStarted = 0;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Find the last media stream this user started
	for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
	{
		MediaStream *pMS = (*it).second;
		if( pMS->m_iPK_Users && pMS->m_iPK_Users==iPK_Users && pMS->m_tTime > tStarted )
			pMediaStream = pMS;
	}

	if( !pMediaStream )
		g_pPlutoLogger->Write(LV_STATUS,"Move Media, but user %d isn't listening to anything.  Open Streams: %d",iPK_Users,(int) m_mapMediaStream.size());
	else
	{
		g_pPlutoLogger->Write(LV_WARNING,"Move Media, user %d -- stream %d %s",iPK_Users,pMediaStream->m_iStreamID_get(),pMediaStream->m_sMediaDescription.c_str());
		// Only move the media if it's not already there.  Maybe the user just turned follow me on
		if( pMediaStream->m_mapEntertainArea.find(iPK_RoomOrEntArea)==pMediaStream->m_mapEntertainArea.end() )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),StringUtils::itos(iPK_RoomOrEntArea));
	}
}

void Media_Plugin::FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
	if( iPK_RoomOrEntArea && iPK_RoomOrEntArea_Left )
		return; // It's just a move, we're going to get EnteredRoom called immediately hereafter

	// See if we have any pending media for this user
	MediaStream *pMediaStream = NULL;
	time_t tStarted = 0;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Find the last media stream this user started
	for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
	{
		MediaStream *pMS = (*it).second;
		if( pMS->m_iPK_Users && pMS->m_iPK_Users==iPK_Users && pMS->m_tTime > tStarted )
			pMediaStream = pMS;
	}

	if( !pMediaStream )
		g_pPlutoLogger->Write(LV_WARNING,"Move Media left, but user %d isn't listening to anything.  Open Streams: %d",iPK_Users,(int) m_mapMediaStream.size());
	else
	{
		g_pPlutoLogger->Write(LV_WARNING,"Before Move Media, user %d -- parking stream %d %s",
			iPK_Users,pMediaStream->m_iStreamID_get(),pMediaStream->m_sMediaDescription.c_str());
		CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),"");  // Park it for the time being
		g_pPlutoLogger->Write(LV_WARNING,"After Move Media");
	}
}

bool Media_Plugin::MediaFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	return HandleFollowMe(pMessage);
}
//<-dceag-c337-b->

	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #50 Name */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */

void Media_Plugin::CMD_Rip_Disk(int iPK_Users,string sName,string sTracks,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	// we only have the sources device. This should be an orbiter
	// but it should be ok if we get this message from a different device in the same ent area.
	// (this is not here yet, we can put this to have the above functionality. Don't know about the usefullness yet.
	//
	// EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_From, 0);
	// is this required here ?!
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	vector<EntertainArea *> vectEntertainArea;
	DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
	if ( vectEntertainArea.size()!=1 )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): The source device ID in the message is not in an ent area or is not an orbiter. Ignoring request");
		return;
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];

	// If it's a cd and no tracks were specified, prompt the user, otherwise fill in the file names
	if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
	{
		if( sTracks.size()==0 )
		{
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pMessage->m_dwPK_Device_From,0,StringUtils::itos(DESIGNOBJ_mnuCDTrackCopy_CONST),"","",false,true);
			SendCommand(CMD_Goto_Screen);
			return;
		}
		else 
		{
			string sNewTracks="";
			if( sTracks=="A" )
			{
				for(size_t s=0;s<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size();++s)
				{
					MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->m_dequeMediaFile[s];
					sNewTracks += StringUtils::itos(s+1) +
						"," + pMediaFile->m_sDescription + "|";
				}
			}
			else
			{
				string::size_type pos=0;
				while( pos<sTracks.size() && pos!=string::npos )
				{
					string sTrack = StringUtils::Tokenize(sTracks,"|",pos);
					int iTrack = atoi(sTrack.c_str());
					sNewTracks += StringUtils::itos(iTrack+1);
					if( iTrack<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() && pEntertainArea->m_pMediaStream->m_dequeMediaFile[iTrack]->m_sDescription.size() )
						sNewTracks += "," + pEntertainArea->m_pMediaStream->m_dequeMediaFile[iTrack]->m_sDescription + "|";
					else
						sNewTracks += ",Unknown " + StringUtils::itos(iTrack+1) + "|";
	g_pPlutoLogger->Write(LV_STATUS,"%s %d %s",sTrack.c_str(),iTrack,sNewTracks.c_str());
				}
			}
g_pPlutoLogger->Write(LV_STATUS,"Transformed %s into %s",sTracks.c_str(),sNewTracks.c_str());
			sTracks=sNewTracks;
		}
	}

	// Validate the name and be sure it's unique
	if( sName.size()==0 )
		sName = "Unknown disc";

	if( FileUtils::DirExists(sName) )
	{
		int Counter=1;
		string sNewName = sName + "_" + StringUtils::itos(Counter++);
		while( FileUtils::DirExists(sNewName) )
			sNewName = sName + "_" + StringUtils::itos(Counter++);
		sName = sNewName;
	}

	string sSubDir = pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST ? "movies" : "music";
	if( iPK_Users==0 )
		sName = "/home/public/data/" + sSubDir + "/" + sName;
	else
		sName = "/home/user_" + StringUtils::itos(iPK_Users) + "/data/" + sSubDir + "/" + sName;

	// If we have a proper name (aka. non empty one) we need to look in the current entertainment area for the disk drive and forward the received command to him.
	MediaDevice *pDiskDriveMediaDevice = NULL;
	map<int, class MediaDevice *>::const_iterator itMediaDevicesInEntArea;
	for ( itMediaDevicesInEntArea = pEntertainArea->m_mapMediaDevice.begin(); itMediaDevicesInEntArea != pEntertainArea->m_mapMediaDevice.end(); itMediaDevicesInEntArea++ )
	{
		MediaDevice * pMediaDevice = (*itMediaDevicesInEntArea).second;
		if ( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory == DEVICECATEGORY_Disc_Drives_CONST )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Found a disk drive in the target ent area");
			pDiskDriveMediaDevice = pMediaDevice;
			break;
		}
	}

	if ( pDiskDriveMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): No disk drive was found in the target ent. area with id: %d. Not ripping anything.", pEntertainArea->m_iPK_EntertainArea);
		return;
	}

	if( pEntertainArea->m_pMediaStream )
	{
		MediaStream *pTmpMediaStream = pEntertainArea->m_pMediaStream;
		mm.Release();

		g_pPlutoLogger->Write( LV_STATUS, "Sending stop media before rip" );
		pTmpMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pTmpMediaStream );
		g_pPlutoLogger->Write( LV_STATUS, "Called StopMedia begfore rip" );
		StreamEnded(pTmpMediaStream);
	}

	DCE::CMD_Rip_Disk cmdRipDisk(m_dwPK_Device, pDiskDriveMediaDevice->m_pDeviceData_Router->m_dwPK_Device, iPK_Users, sName, sTracks);
	SendCommand(cmdRipDisk);
	m_mapRippingJobsToRippingDevices[sName] = make_pair<int, int>(pDiskDriveMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMessage->m_dwPK_Device_From);
	return;
}

// This should be in sync with the typedef in the Disk_Drive.h
typedef enum {
	RIP_RESULT_BEGIN_ENUM		 = 0x01,
	RIP_RESULT_ALREADY_RIPPING,
	RIP_RESULT_NO_DISC,
	RIP_RESULT_INVALID_DISC_TYPE,
	RIP_RESULT_SUCCESS,
	RIP_RESULT_FAILURE,
	RIP_RESULT_END_ENUM
} rippingResult;

bool Media_Plugin::RippingCompleted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    string 	sJobName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];
	int		iResult  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str( ) );

	// See Disk_Drive.h for the defines
	if ( iResult <= RIP_RESULT_BEGIN_ENUM || iResult >= RIP_RESULT_END_ENUM )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Invalid result parameters. Ignoring event.");
		return true;
	}

	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Got a ripping completed event for job named \"%s\" from device: \"%d\"", sJobName.c_str(), pMessage->m_dwPK_Device_From);
	map<string, pair<int, int> >::const_iterator itRippingJobs;

	if ( (itRippingJobs = m_mapRippingJobsToRippingDevices.find(sJobName)) == m_mapRippingJobsToRippingDevices.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Unrecognized ripping job: %s. Ignoring event.", sJobName.c_str());
		return true;
	}

	string sMessage;
	switch ( iResult )
	{
		case RIP_RESULT_ALREADY_RIPPING: 	sMessage = "There is already a ripping job in this entertainment area!"; 	break;
		case RIP_RESULT_NO_DISC:			sMessage = "There is no disk in the Media Director which is controlling this entertainment area!";	break;
		case RIP_RESULT_INVALID_DISC_TYPE:	sMessage = "Can't rip the disk that is in the unit at this moment (unknown format)!";	break;
		case RIP_RESULT_FAILURE:			sMessage = "Unspecified error while ripping the disk.";	break;
		case RIP_RESULT_SUCCESS:			sMessage = "The disk was ripped succesfully.";	break;
		case RIP_RESULT_BEGIN_ENUM:
		case RIP_RESULT_END_ENUM:
			break;
	}

	int sourceOrbiter = m_mapRippingJobsToRippingDevices[sJobName].second;
	m_mapRippingJobsToRippingDevices.erase(sJobName);

	m_pOrbiter_Plugin->DisplayMessageOnOrbiter(sourceOrbiter,sMessage,false,1200,true);   // Leave the message on screen for 20 mins

	return true;
}

bool Media_Plugin::DeviceOnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	bool bIsOn = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";
	MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pMessage->m_dwPK_Device_From);
	if( !pMediaDevice )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Got an on/off from a non-media device");
		return true;  // It's not for us
	}

	return HandleDeviceOnOffEvent(pMediaDevice,bIsOn);
}

bool Media_Plugin::HandleDeviceOnOffEvent(MediaDevice *pMediaDevice,bool bIsOn)
{
	// First figure out if this device is involved in any streams
	MediaDevice *pMediaDevice_Source=NULL,*pMediaDevice_Dest=NULL;
	EntertainArea *pEntertainArea=NULL;
	MediaStream *pMediaStream=NULL;

	int iIsSource_OrDest = DetermineInvolvement(pMediaDevice, pMediaDevice_Source,pMediaDevice_Dest,
		pEntertainArea,pMediaStream);

	if( bIsOn && iIsSource_OrDest )  // Nothing to report, we turned on something we already knew was on
	{
		g_pPlutoLogger->Write(LV_STATUS,"Nothing to report, we turned on something we already knew was on");
		return true;
	}
	if( !bIsOn && !iIsSource_OrDest )  // Nothing to report, we turned off something we already knew was off
	{
		g_pPlutoLogger->Write(LV_STATUS,"Nothing to report, we turned off something we already knew was off");
		return true;
	}

	if( bIsOn==false )
	{
		// We're only playing in 1 EA.  Just shut it off
		if( pMediaStream->m_mapEntertainArea.size()<2 )
			StreamEnded(pMediaStream);

		// We turned off the destination in an entertainment area
		else if( pEntertainArea )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),pMediaStream->GetEntAreasWithout(pEntertainArea));

		// We're streaming to multiple destinations, and the device we turned off isn't the source
		// so we'll just do a move media removing this
		else if( pMediaDevice_Source && pMediaStream->m_mapEntertainArea.size()>1 )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),pMediaStream->GetEntAreasWithout(&pMediaDevice->m_mapEntertainArea));

		// We can't turn off just 1 area -- shut down the whole stream
		else
			StreamEnded(pMediaStream);
	}
	else if( pMediaDevice->m_pCommandGroup )
		QueueMessageToRouter(new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,
			MESSAGETYPE_EXEC_COMMAND_GROUP,pMediaDevice->m_pCommandGroup->m_PK_CommandGroup,0));
	else
	{
		// We don't have a specific command to do this, but since watching tv is such a common task
		// we've got a special case for it.  Don't bother if it's not just a simple tv in a single
		// entertainment area
		if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_TVs_CONST) &&
			pMediaDevice->m_mapEntertainArea.size()==1 )
		{
			vector<EntertainArea *> vectEntertainArea;
			vectEntertainArea.push_back( pMediaDevice->m_mapEntertainArea.begin()->second );
/*
			map<int,MediaHandlerInfo *> mapMediaHandlerInfo;
			GetMediaHandlersForEA(MEDIATYPE_pluto_LiveTV_CONST, vectEntertainArea, mapMediaHandlerInfo);

			for(map<int,MediaHandlerInfo *>::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it)
			{
				g_pPlutoLogger->Write(LV_STATUS,"Calling Start Media from Auto start TV");
				StartMedia(it->second,0,vectEntertainArea,0, 0,NULL,false,0);  // We'll let the plug-in figure out the source, and we'll use the default remote
			}
*/
		}
	}
	return true;
}

bool Media_Plugin::MediaDescriptionChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pMessage->m_dwPK_Device_From);
	if( !pMediaDevice )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Got an on/off from a non-media device");
		return true;  // It's not for us
	}

	// First figure out if this device is involved in any streams
	MediaDevice *pMediaDevice_Source=NULL,*pMediaDevice_Dest=NULL;
	EntertainArea *pEntertainArea=NULL;
	MediaStream *pMediaStream=NULL;

	int iIsSource_OrDest = DetermineInvolvement(pMediaDevice, pMediaDevice_Source,pMediaDevice_Dest,
		pEntertainArea,pMediaStream);

	if( pMediaStream )
	{
		pMediaStream->m_sMediaDescription = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
		MediaInfoChanged(pMediaStream,false);
	}
	return true;
}

bool Media_Plugin::AvInputChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( !pDeviceFrom )
		return true;

	DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceFrom;
	int PK_Command = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Command_CONST].c_str());

	// See what media device is using that input
	for(size_t s=0;s<pDevice->m_vectDevices_SendingPipes.size();++s)
	{
		DeviceData_Router *pDevice_Child = pDevice->m_vectDevices_SendingPipes[s];
        for(map<int,Pipe *>::iterator it=pDevice_Child->m_mapPipe_Available.begin();it!=pDevice_Child->m_mapPipe_Available.end();++it)
        {
            Pipe *pPipe = (*it).second;
			if( pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get()==pDevice->m_dwPK_Device &&
				pPipe->m_pRow_Device_Device_Pipe->FK_Command_Input_get()==PK_Command )
			{
				MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pPipe->m_pRow_Device_Device_Pipe->FK_Device_From_get());
				if( pMediaDevice )
					HandleDeviceOnOffEvent(pMediaDevice,true);
			}
		}
	}

	return true;
}

bool Media_Plugin::DiskDriveIsRipping(int iPK_Device)
{
	map<string, pair<int, int> >::const_iterator itRippingJobs;

	for ( itRippingJobs = m_mapRippingJobsToRippingDevices.begin(); itRippingJobs != m_mapRippingJobsToRippingDevices.end(); itRippingJobs++ )
	{
		if ( (*itRippingJobs).second.first == iPK_Device)
			return true;
	}

	return false;
}

bool Media_Plugin::SafeToReload(string *sPendingTasks)
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	if( m_mapRippingJobsToRippingDevices.size() )
	{
		string sJobs;
		if( sPendingTasks )
		{
			for(map<string, pair<int, int> >::iterator it=m_mapRippingJobsToRippingDevices.begin();it!=m_mapRippingJobsToRippingDevices.end();++it)
				(*sPendingTasks) += "Ripping: " + (*it).first + "\n";
		}
		return false;
	}
	return true;
}

void Media_Plugin::AddOtherDevicesInPipesToRenderDevices(int PK_Pipe, map<int,MediaDevice *> *pmapMediaDevice)
{
	for(map<int,MediaDevice *>::iterator it=pmapMediaDevice->begin();it!=pmapMediaDevice->end();++it)
		AddOtherDevicesInPipes_Loop(PK_Pipe,(*it).second->m_pDeviceData_Router,pmapMediaDevice);
}

void Media_Plugin::AddOtherDevicesInPipes_Loop(int PK_Pipe, DeviceData_Router *pDevice,map<int,MediaDevice *> *pmapMediaDevice,vector<int> *p_vectDevice)
{
	if( !pDevice )
		return;

	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDevice->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDevice->m_dwPK_Device);

	for(map<int, class Pipe *>::iterator it=pDevice->m_mapPipe_Available.begin();
		it!=pDevice->m_mapPipe_Available.end();++it)
	{
		Pipe *pPipe = (*it).second;
		if( !PK_Pipe || pPipe->m_pRow_Device_Device_Pipe->FK_Pipe_get()==PK_Pipe )
		{
			DeviceData_Router *pDevice_Pipe = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get());
			if( pDevice_Pipe )
			{
				MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDevice_Pipe->m_dwPK_Device);
				if( pMediaDevice )
					(*pmapMediaDevice)[pDevice_Pipe->m_dwPK_Device] = pMediaDevice;
				if( pmapMediaDevice->find(pDevice_Pipe->m_dwPK_Device)==pmapMediaDevice->end() )
					AddOtherDevicesInPipes_Loop(PK_Pipe,pDevice_Pipe,pmapMediaDevice,p_vectDevice);
			}
			else
				g_pPlutoLogger->Write(LV_CRITICAL,"AddOtherDevicesInPipes_Loop - Device %d isn't a media device",pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get());
		}
	}

	if( pDevice->m_pDevice_MD && pDevice!=pDevice->m_pDevice_MD )
	{
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDevice->m_pDevice_MD->m_dwPK_Device);
		if( pMediaDevice )
			(*pmapMediaDevice)[pDevice->m_pDevice_MD->m_dwPK_Device] = pMediaDevice;
		AddOtherDevicesInPipes_Loop( PK_Pipe, (DeviceData_Router *) pDevice->m_pDevice_MD, pmapMediaDevice, p_vectDevice );
	}
	if( bCreatedVect )
		delete p_vectDevice;
}

void Media_Plugin::GetMediaHandlersForEA(int iPK_MediaType,vector<EntertainArea *> &vectEntertainArea, vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > &vectEA_to_MediaHandler)
{
	// This function needs to find a media handler for every entertainment area.  This map will store all our possibilities
	// of handlers and what entertainment areas they can support.  We'll first populate the map, then pick the best matches
	// to store in vectEA_to_MediaHandler
	map<MediaHandlerInfo *, vector<EntertainArea *> > mapMediaHandlerInfo;
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea=vectEntertainArea[s];

		// See if there's a media handler for this type of media in this area
		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find(iPK_MediaType);
		if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size()==0 )
		{
			Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(iPK_MediaType);
			if( pRow_MediaType && pRow_MediaType->DCEAware_get()==0 )
			{
				g_pPlutoLogger->Write( LV_STATUS, "Play media type %d in entertain area %d with generic handler", iPK_MediaType, pEntertainArea->m_iPK_EntertainArea);
				m_pGenericMediaHandlerInfo->m_PK_MediaType = iPK_MediaType; // Just temporary for start media.  We're in a mutex
				mapMediaHandlerInfo[m_pGenericMediaHandlerInfo].push_back(pEntertainArea);
			}
			else
				g_pPlutoLogger->Write( LV_WARNING, "Play media type %d in entertain area %d but nothing to handle it", iPK_MediaType, pEntertainArea->m_iPK_EntertainArea);
		}
		else
		{
			for( List_MediaHandlerInfo::iterator it=pList_MediaHandlerInfo->begin();it!=pList_MediaHandlerInfo->end();++it )
			{
				MediaHandlerInfo *pMediaHandlerInfo = *it;
				mapMediaHandlerInfo[pMediaHandlerInfo].push_back(pEntertainArea);
			}
		}
	}

	if( mapMediaHandlerInfo.size()==1 && (vectEntertainArea.size()==1 || mapMediaHandlerInfo.begin()->first->m_bMultipleDestinations) )
	{
		// It's easy, save ourselves the trouble of searching we've got 1 handler and it can take care of it
		vectEA_to_MediaHandler.push_back( make_pair< MediaHandlerInfo *,vector<EntertainArea *> >(mapMediaHandlerInfo.begin()->first,mapMediaHandlerInfo.begin()->second) );
		return;
	}

	// It's more work, we need to prioritize the handlers and start making assignments from the top
	list<MediaHandlerInfo *> listMediaHandlerInfo;
	for( map<MediaHandlerInfo *, vector<EntertainArea *> >::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it )
		listMediaHandlerInfo.push_back( it->first );
	listMediaHandlerInfo.sort(MediaHandlerComparer);

	// Now a map to keep track of which EA's we have matched
	map<int,bool> mapMatched;
	for(size_t s=0;s<vectEntertainArea.size();++s)
		mapMatched[ vectEntertainArea[s]->m_iPK_EntertainArea ] = false;

	vector<EntertainArea *> vEmpty;  // Just an empty vect since we're creating new ones on the stack

	// Now, we're going to make 2 passes if we have more than 1 EA.  First we'll find handlers that can play multiple
	// areas, reducing the number of redundant streams, and then on the 2nd pass pick up EA's not yet serviced
	bool bMultipleEA = vectEntertainArea.size()>1;
	for(int pass=0;pass< ( bMultipleEA ? 2 : 1); ++pass)
	{
		for( list<MediaHandlerInfo *>::iterator it=listMediaHandlerInfo.begin();it!=listMediaHandlerInfo.end();++it )
		{
			MediaHandlerInfo *pMediaHandlerInfo = *it;
			if( bMultipleEA && pass==0 && !pMediaHandlerInfo->m_bMultipleDestinations )
				continue;

			bool bAlreadyUsed=false;  // We'll set this to true when we assign a media handler to allow for splitting into multiple entries if the device can't handle more than 1 EA
			bool bNeedToStartWithEmpty=true;  // The first time we find a match we know we need to start with a new empty vect

			// Get all the possible EA's
			vector<EntertainArea *> *pvect_EntertainArea = &mapMediaHandlerInfo[pMediaHandlerInfo];
			for(size_t s=0;s<pvect_EntertainArea->size();++s)
			{
				// And fill in the ones we didn't match
				if( !mapMatched[ (*pvect_EntertainArea)[s]->m_iPK_EntertainArea ] )
				{
					if( bAlreadyUsed || bNeedToStartWithEmpty ) // Start with a new empty vector if we already used it and can't handle multiple EA's
					{
						vectEA_to_MediaHandler.push_back( make_pair< MediaHandlerInfo *,vector<EntertainArea *> >(
							pMediaHandlerInfo,vEmpty) );
						bNeedToStartWithEmpty=false;
					}

					// Mark it as matched, and add it to the vect we just created
					mapMatched[ (*pvect_EntertainArea)[s]->m_iPK_EntertainArea ]=true;
					vectEA_to_MediaHandler.back().second.push_back( (*pvect_EntertainArea)[s] );
					if( !pMediaHandlerInfo->m_bMultipleDestinations )
						bAlreadyUsed=true;  // Don't re-use this it can't handle multiple destinations

				}
			}
		}
	}
}

MediaDevice *Media_Plugin::GetMediaDeviceForEA(int iPK_MediaType,EntertainArea *pEntertainArea)
{
	for(map<int, class MediaDevice *>::iterator it=pEntertainArea->m_mapMediaDevice.begin();it!=pEntertainArea->m_mapMediaDevice.end();++it)
	{
		MediaDevice *pMediaDevice = it->second;
		if( pMediaDevice->m_mapMediaType.find(iPK_MediaType)!=pMediaDevice->m_mapMediaType.end() )
			return pMediaDevice;
	}
	return NULL;
}

//<-dceag-c372-b->

	/** @brief COMMAND: #372 - MH Set Volume */
	/**  */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area(s) */
		/** @param #76 Level */
			/** The level as an abolute value from 0-100, or a relative value like -1 or +1. */

void Media_Plugin::CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c372-e->
{
g_pPlutoLogger->Write(LV_STATUS,"CMD_MH_Set_Volume");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string::size_type pos=0;
	while(pos<sPK_EntertainArea.size() && pos!=string::npos)
	{
		string s = StringUtils::Tokenize(sPK_EntertainArea,",",pos);
		EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(atoi(s.c_str()));
		if( pEntertainArea && pEntertainArea->m_pMediaDevice_ActiveDest )
		{
g_pPlutoLogger->Write(LV_STATUS,"For EA %s found active device %d",s.c_str(),pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device);
			if( sLevel=="-1" )
			{
				DCE::CMD_Vol_Down CMD_Vol_Down(m_dwPK_Device,pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device,1);
				SendCommand(CMD_Vol_Down);
			}
			else if( sLevel=="+1" )
			{
				DCE::CMD_Vol_Up CMD_Vol_Up(m_dwPK_Device,pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device,1);
				SendCommand(CMD_Vol_Up);
			}
			else
			{
				DCE::CMD_Set_Volume CMD_Set_Volume(m_dwPK_Device,pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device,sLevel);
				SendCommand(CMD_Set_Volume);
			}
		}
	}
}

void Media_Plugin::Parse_CDDB_Media_ID(MediaStream *pMediaStream,string sValue)
{
	string::size_type pos=0,pos2=0;

	string sCDDBID = StringUtils::Tokenize(sValue,"\t",pos);
	Row_Attribute *pRow_Attribute;

	// Before proceeding, see if this disc is already in the database
	if( !IsDiscAlreadyIdentified(sCDDBID,pMediaStream) )
	{
		// The cddb info is space delimited
	    pRow_Attribute = m_pMediaAttributes->GetAttributeFromDescription(ATTRIBUTETYPE_CDDB_CONST,StringUtils::Tokenize(sCDDBID," ",pos2)); 
		pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_CDDB_CONST] = pRow_Attribute->PK_Attribute_get();
		int NumTracks=atoi(StringUtils::Tokenize(sCDDBID," ",pos2).c_str()); // cddb id

		// The actual data is now tab delimited.
		string s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = m_pMediaAttributes->GetAttributeFromDescription(ATTRIBUTETYPE_Group_CONST,s); 
			pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Group_CONST] = pRow_Attribute->PK_Attribute_get();
		}

		s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = m_pMediaAttributes->GetAttributeFromDescription(ATTRIBUTETYPE_Album_CONST,s); 
			pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Album_CONST] = pRow_Attribute->PK_Attribute_get();
		}

		s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = m_pMediaAttributes->GetAttributeFromDescription(ATTRIBUTETYPE_Genre_CONST,s); 
			pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Genre_CONST] = pRow_Attribute->PK_Attribute_get();
		}

		// Read the info for the tracks.  NumTracks normally should = pMediaStream->m_dequeMediaFile.size()
		for(size_t s=0;s<NumTracks && s<pMediaStream->m_dequeMediaFile.size();++s)
		{
			MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[s];
			string str = StringUtils::Tokenize(sValue,"\t\n",pos);
			if( str.size() )
			{
				pRow_Attribute = m_pMediaAttributes->GetAttributeFromDescription(ATTRIBUTETYPE_Song_CONST,str); 
				pMediaFile->m_mapPK_Attribute[ATTRIBUTETYPE_Song_CONST] = pRow_Attribute->PK_Attribute_get();
			}
		}
		AddIdentifiedDiscToDB(sCDDBID,pMediaStream);
	}

	pMediaStream->UpdateDescriptionsFromAttributes();
	MediaInfoChanged(pMediaStream,true);
}
//<-dceag-c388-b->

	/** @brief COMMAND: #388 - Set Media Private */
	/** Indicate if the media in an entertainment area is private or not. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #119 True/False */
			/** True if the media in this area is public (the default), false if it's not */

void Media_Plugin::CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c388-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(sPK_EntertainArea.c_str()) );
	if( !pEntertainArea )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Tried to set private flag on non-existant ea");
		return;
	}
	pEntertainArea->m_bMediaIsPrivate=!bTrueFalse;

	for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;
		if( pOH_Orbiter->m_pEntertainArea != pEntertainArea )
			continue;

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconl(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
			StringUtils::itos((int) pEntertainArea->m_bMediaIsPrivate),"media_private");
		SendCommand(CMD_Set_Bound_Iconl);
	}
}
//<-dceag-c390-b->

	/** @brief COMMAND: #390 - Get Default Ripping Name */
	/** Gets the default name for ripping the media in the given entertainment area. */
		/** @param #13 Filename */
			/** The default filename */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */

void Media_Plugin::CMD_Get_Default_Ripping_Name(string sPK_EntertainArea,string *sFilename,string &sCMD_Result,Message *pMessage)
//<-dceag-c390-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(atoi(sPK_EntertainArea.c_str()));
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
	{
		*sFilename = "***ERROR*** cannot rip";  // should not happen

		return;
	}
	if( pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
	{
		(*sFilename) = pEntertainArea->m_pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Group_CONST];
		if( sFilename->size() )
			(*sFilename) += "/"; // We got a gropu

		(*sFilename) += pEntertainArea->m_pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Album_CONST];
	}
	else
		(*sFilename) = pEntertainArea->m_pMediaStream->m_sMediaDescription;

	// Validate the name and be sure it's unique
	if( sFilename->size()==0 )
		(*sFilename) = "Unknown disc";
}

//<-dceag-c391-b->

	/** @brief COMMAND: #391 - Add Media Attribute */
	/** Changes the media attribute */
		/** @param #5 Value To Assign */
			/** The new value of the attribute */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #121 Tracks */
			/** If empty, the attribute is for the disc.  If specified, it is for this track number */
		/** @param #122 EK_AttributeType */
			/** The type of attribute to set */

void Media_Plugin::CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string &sCMD_Result,Message *pMessage)
//<-dceag-c391-e->
{
}

bool Media_Plugin::IsDiscAlreadyIdentified(string sIdentifiedDisc,MediaStream *pMediaStream)
{
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);
	if( vectRow_Disc.size()==0 )
		return false;

	Row_Disc *pRow_Disc = vectRow_Disc[0];
	pMediaStream->m_dwPK_Disc = pRow_Disc->PK_Disc_get();

	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	pRow_Disc->Disc_Attribute_FK_Disc_getrows(&vectRow_Disc_Attribute);

	if( vectRow_Disc_Attribute.size()<2 )
		return false; // If it was identified successfully, we'd have at least 2 entries.  It could have been a failed identify before, so we'll retry
	for(size_t s=0;s<vectRow_Disc_Attribute.size();++s)
	{
		Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[s];
		Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
		if( !pRow_Attribute )
			continue; // Should never happen
		if( pRow_Disc_Attribute->Track_get()==0 )
			pMediaStream->m_mapPK_Attribute[pRow_Attribute->FK_AttributeType_get()] = pRow_Attribute->PK_Attribute_get();
		else
		{
			if( pRow_Disc_Attribute->Track_get()>=1 && pRow_Disc_Attribute->Track_get()<pMediaStream->m_dequeMediaFile.size() )
			{
				MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[pRow_Disc_Attribute->Track_get()-1];
				pMediaFile->m_mapPK_Attribute[pRow_Attribute->FK_AttributeType_get()] = pRow_Attribute->PK_Attribute_get();
			}
		}
	}
	return true;
}

int Media_Plugin::AddIdentifiedDiscToDB(string sIdentifiedDisc,MediaStream *pMediaStream)
{
	Row_Disc *pRow_Disc;
	
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);
	if( vectRow_Disc.size() )
		pRow_Disc = vectRow_Disc[0];
	else
	{
		pRow_Disc = m_pDatabase_pluto_media->Disc_get()->AddRow();
		pRow_Disc->ID_set(sIdentifiedDisc);
		pRow_Disc->Table_Disc_get()->Commit();
	}
	pMediaStream->m_dwPK_Disc = pRow_Disc->PK_Disc_get();

	for( map<int,int>::iterator it=pMediaStream->m_mapPK_Attribute.begin();it!=pMediaStream->m_mapPK_Attribute.end();++it)
	{
		if( it->first==0 )
			continue;  // Don't store the recognition type

		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
		Row_Disc_Attribute *pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->GetRow(pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),0);
		if( !pRow_Disc_Attribute )
		{
			pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->AddRow();
			pRow_Disc_Attribute->FK_Disc_set(pRow_Disc->PK_Disc_get());
			pRow_Disc_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get()); 
			pRow_Disc_Attribute->Track_set(0);
			pRow_Disc_Attribute->Table_Disc_Attribute_get()->Commit();
		}
	}

	for(size_t s=0;s<pMediaStream->m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[s];
		for( map<int,int>::iterator it=pMediaFile->m_mapPK_Attribute.begin();it!=pMediaFile->m_mapPK_Attribute.end();++it)
		{
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
			Row_Disc_Attribute *pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->GetRow(pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),s);
			if( !pRow_Disc_Attribute )
			{
				pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->AddRow();
				pRow_Disc_Attribute->FK_Disc_set(pRow_Disc->PK_Disc_get());
				pRow_Disc_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get()); 
				pRow_Disc_Attribute->Track_set(s+1);
				pRow_Disc_Attribute->Table_Disc_Attribute_get()->Commit();
			}
		}
	}
	return pRow_Disc->PK_Disc_get();
}
//<-dceag-c392-b->

	/** @brief COMMAND: #392 - Set Media Attribute Text */
	/** Adds a new attribute */
		/** @param #5 Value To Assign */
			/** The new value.  If it's a name, LastName^Firstname format */
		/** @param #123 PK_Attribute */
			/** The attribute */

void Media_Plugin::CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iPK_Attribute,string &sCMD_Result,Message *pMessage)
//<-dceag-c392-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(iPK_Attribute);
	if( pRow_Attribute )
	{
		string sFirstName;
		string::size_type posFirstName = sValue_To_Assign.find('^');
		if( posFirstName!=string::npos )
		{
			sFirstName = sValue_To_Assign.substr(posFirstName-1);
			sValue_To_Assign = sValue_To_Assign.substr(0,posFirstName);
		}
		pRow_Attribute->Name_set(sValue_To_Assign);
		pRow_Attribute->FirstName_set(sFirstName);
		pRow_Attribute->Table_Attribute_get()->Commit();

		for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
		{
			it->second->UpdateDescriptionsFromAttributes();
			MediaInfoChanged(it->second,true);
		}
	}
}
//<-dceag-c393-b->

	/** @brief COMMAND: #393 - Get Attribute */
	/** Returns the attribute text for the given ID */
		/** @param #9 Text */
			/** The value */
		/** @param #123 PK_Attribute */
			/** The attribute */

void Media_Plugin::CMD_Get_Attribute(int iPK_Attribute,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c393-e->
{
	Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(iPK_Attribute);
	if( pRow_Attribute )
	{
		(*sText) = m_pMediaAttributes->GetTabbedName(pRow_Attribute);
		StringUtils::Replace(sText,"\t","&");
	}
}

int Media_Plugin::DetermineInvolvement(MediaDevice *pMediaDevice, MediaDevice *&pMediaDevice_Source,MediaDevice *&pMediaDevice_Dest,
	EntertainArea *&pEntertainArea,MediaStream *&pMediaStream)
{
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMS = (*it).second;

		// See if the device turned off was the source for some media
		if( pMS->m_pMediaDevice_Source==pMediaDevice || pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD==pMediaDevice->m_pDeviceData_Router )
		{
			pMediaStream = pMS;
			return 1;  // This is the source device
		}

		// If not, maybe it's being fed the signal from the source
		for(size_t s=0;s<pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size();++s)
		{
			DeviceData_Base *pDevice = pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes[s];
			if( pMS->m_pMediaDevice_Source->m_pDeviceData_Router==pDevice || pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD==pDevice )
			{
				pMediaStream = pMS;
				pMediaDevice_Source = pMS->m_pMediaDevice_Source;
				return 1;
			}
		}

		for( MapEntertainArea::iterator it=pMS->m_mapEntertainArea.begin( );it!=pMS->m_mapEntertainArea.end( );++it )
		{
			EntertainArea *pEA = ( *it ).second;
			if( pEA->m_pMediaDevice_ActiveDest==pMediaDevice || pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD==pMediaDevice->m_pDeviceData_Router )
			{
				pMediaStream = pMS;
				pEntertainArea = pEA;
				return 2;  // This is the dest device
			}
			// If not, maybe it's being fed the signal from the dest
			for(size_t s=0;s<pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size();++s)
			{
				DeviceData_Base *pDevice = pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes[s];
				if( pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router==pDevice || pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD==pDevice )
				{
					pMediaStream = pMS;
					pEntertainArea = pEA;
					pMediaDevice_Dest = pEntertainArea->m_pMediaDevice_ActiveDest;
					return 2;  // This is the dest device
				}
			}
		}
	}
	return 0; // No involvement
}
