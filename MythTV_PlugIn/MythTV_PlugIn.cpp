/**
 *
 * @file MythTV_PlugIn.cpp
 * @brief source file for the MythTvStream, MythTV_PlugIn classes
 *
 */

//<-dceag-d-b->
#include "MythTV_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Table_EventParameter.h"

#include "DataGrid.h"

#include "MythTvWrapper.h"
#include <libmythtv/frame.h>

//<-dceag-const-b->
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pMythWrapper = new MythTvWrapper(this);
}

//<-dceag-dest-b->
MythTV_PlugIn::~MythTV_PlugIn()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_PlugIn::Register()
//<-dceag-reg-e->
{
    /** Get a pointer to the media plugin */
    m_pMedia_Plugin=NULL;
    ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Media_Plugin_CONST);

    if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV plug in cannot find media handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
        return false;
    }
    m_pMedia_Plugin=(Media_Plugin *) pListCommand_Impl->front();

    m_pMedia_Plugin->RegisterMediaPlugin(this, this, DEVICETEMPLATE_MythTV_Player_CONST, true);

    /** And the datagrid plug-in */
    m_pDatagrid_Plugin=NULL;

    pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

    if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"File grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
        return false;
    }
    m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::CurrentShows))
                                                ,DATAGRID_EPG_Current_Shows_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShows))
                                                ,DATAGRID_EPG_All_Shows_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShowsForMobiles))
                                                ,DATAGRID_EPG_All_Shows_Mobile_CONST);

    m_pRouter->RegisterInterceptor(
        new MessageInterceptorCallBack( this, ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged) ),
        0, 0, 0, 0,
        MESSAGETYPE_EVENT, EVENT_Playback_Info_Changed_CONST );

    return Connect();
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
void MythTV_PlugIn::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void MythTV_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

bool MythTV_PlugIn::StartMedia(class MediaStream *pMediaStream)
{
    if( pMediaStream->GetType()!=MEDIASTREAM_TYPE_MYTHTV )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Trying to start a non myth-tv stream");
        return false;
    }
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    MythTvStream *pMythTvStream = (MythTvStream *) pMediaStream;

    g_pPlutoLogger->Write(LV_STATUS, "Media type %d %s", pMythTvStream->m_iPK_MediaType, pMediaStream->GetFilenameToPlay("empty files").c_str());

    string Response;

    m_dwTargetDevice = pMythTvStream->m_dwPK_Device;

    DCE::CMD_Start_TV cmd(m_dwPK_Device, pMythTvStream->m_dwPK_Device);

//     DCE::CMD_Play_Media cmd(m_dwPK_Device,

//      pMythTvStream->m_dwPK_Device,
//         "0", // todo -- the real channel
//                 pMythTvStream->m_dequeFilename.front(),
//         pMythTvStream->m_iPK_MediaType,
//         pMythTvStream->m_iStreamID_get(),#include "pluto_main/Table_EventParameter.h"
//         0);

    m_pMedia_Plugin->MediaInfoChanged(pMythTvStream);

    if( !SendCommand(cmd, &Response) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV player didn't respond to play media command!");

    /** handle failure */

#pragma warning("Ignore this for now")
        return true;
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS,"MythTV player responded to Start TV command!");

    return true;
}

bool MythTV_PlugIn::StopMedia(class MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Stopping media stream playback--sending command, waiting for response");
int i;
    DCE::CMD_Stop_Media cmd(m_dwPK_Device,
        pMediaStream->m_dwPK_Device,
        pMediaStream->m_iStreamID_get(),&i);
    string Response;
    if( !SendCommand(cmd, &Response) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV player didn't respond to stop media command!");

    /** handle failure */

#pragma warning("Ignore this for now")
        return true;
        return false;
    }

    map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);

    g_pPlutoLogger->Write(LV_STATUS,"MythTV player responded to stop media command!");
    return true;
}

bool MythTV_PlugIn::BroadcastMedia(class MediaStream *pMediaStream)
{
    return true;
}

class MediaStream *MythTV_PlugIn::CreateMediaStream(class MediaPluginInfo *pMediaPluginInfo, class EntertainArea *pEntertainArea, MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeFilenames,int StreamID)
{
    if ( m_pMedia_Plugin == NULL )
        return NULL;

    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    if ( pMediaPluginInfo == NULL )
        return NULL;

    MythTvStream *pMediaStream = new MythTvStream(this, pMediaPluginInfo, pMediaDevice, pMediaPluginInfo->m_iPK_DesignObj, 0, st_RemovableMedia,StreamID);

    pMediaStream->m_sMediaDescription = "Not available";
    pMediaStream->m_sSectionDescription = "Not available";
    pMediaStream->m_sMediaSynopsis = "Not available";

    if( !PK_Device_Source && pMediaPluginInfo->m_listMediaDevice.size() )
    {
        MediaDevice *pMediaDevice = pMediaPluginInfo->m_listMediaDevice.front();
        PK_Device_Source=pMediaDevice->m_pDeviceData_Router->m_dwPK_Device;
    }

    DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Source);

    bool bFoundDevice=false;
    if( pDeviceData_Router->m_dwPK_DeviceTemplate==DEVICETEMPLATE_MythTV_Player_CONST )
    {
        pMediaStream->m_dwPK_Device = PK_Device_Source;
        m_mapDevicesToStreams[PK_Device_Source] = StreamID;
        bFoundDevice=true;
    }

    if( !bFoundDevice )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Couldn't find myth tv player for create media stream");
    }

    return pMediaStream;
}

bool MythTV_PlugIn::isValidStreamForPlugin(class MediaStream *pMediaStream)
{
    return pMediaStream->GetType() == MEDIASTREAM_TYPE_MYTHTV;
}

class DataGridTable *MythTV_PlugIn::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign
                        , Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());

    if ( ! m_pMythWrapper )
    {
        g_pPlutoLogger->Write(LV_STATUS, "The myth wrapper object wasn't constructed at MythTV_PlugIn contructor time. Ignoring datagrid request!");

        return NULL;
    }

    QDateTime currentTime = QDateTime::currentDateTime();

    currentTime.setTime(
                QTime(
                    currentTime.time().hour(),
                    currentTime.time().minute() >= 30 ? 30 : 0,
                    0));

    return m_pMythWrapper->createShowsDataGrid(GridID, currentTime, currentTime.addDays(14));
}

class DataGridTable *MythTV_PlugIn::AllShowsForMobiles(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);

    g_pPlutoLogger->Write(LV_STATUS, "Getting all shows");
    if ( ! m_pMythWrapper )
    {
        g_pPlutoLogger->Write(LV_STATUS, "The myth wrapper object wasn't constructed at MythTV_PlugIn contructor time. Ignoring datagrid request!");
        return NULL;
    }

    return m_pMythWrapper->createShowsForMobiles(GridID, QDateTime::currentDateTime());
}

class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    class MediaStream *pMediaStream = m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);
    if( !pMediaStream || pMediaStream->GetType()!=MEDIASTREAM_TYPE_MYTHTV )

    /** @todo probably should have a virtual GetID method to confirm it's the right type to cast */

        return NULL;

    class MythTvStream *pMythTvStream = (MythTvStream *) pMediaStream;

    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    for(size_t s=0;s<pMythTvStream->m_vectRow_Listing.size();++s)
    {
/** @test
//         Row_Listing *pListing = pMythTvStream->m_vectRow_Listing[s];
//         pCell = new DataGridCell(StringUtils::itos(pListing->ChannelNum_get()) + " " + pListing->ChannelName_get() + " - " +                     pListing->ShowName_get(),StringUtils::itos(pListing->PK_Listing_get()));

*/
        pCell = new DataGridCell(StringUtils::itos(1) + " " + "ChannelName" + " - " + "ShowName",StringUtils::itos(1));
        pDataGrid->SetData(0,s,pCell);
    }

    return pDataGrid;
}


bool MythTV_PlugIn::MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Router *pDeviceFrom, class DeviceData_Router *pDeviceTo )
{
    MediaStream *pMediaStream;
    MythTvStream *pMythStream;

    if ( pDeviceFrom->m_dwPK_DeviceTemplate == DEVICETEMPLATE_MythTV_Player_CONST )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Got event from Myth Player device: %d, associated stream is %d", pDeviceFrom->m_dwPK_Device, m_mapDevicesToStreams[pDeviceFrom->m_dwPK_Device]);
        pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[pDeviceFrom->m_dwPK_Device]);

        if ( pMediaStream == NULL || pMediaStream->GetType() != MEDIASTREAM_TYPE_MYTHTV )
        {
            if ( pMediaStream == NULL )
                g_pPlutoLogger->Write(LV_WARNING, "Got a MediaInfoChanged but there is no stream in the media plugin associated with the device %d", pDeviceFrom->m_dwPK_Device );
            else
                g_pPlutoLogger->Write(LV_STATUS, "This device %d claims to be a MythTV Player but the stream assciated with it is not a Myth Stream", pDeviceFrom->m_dwPK_Device);

            return false;
        }

        pMythStream = (MythTvStream *) pMediaStream;

        pMythStream->m_sMediaDescription = pMessage->m_mapParameters[EVENTPARAMETER_MediaDescription_CONST];
        pMythStream->m_sSectionDescription = pMessage->m_mapParameters[EVENTPARAMETER_SectionDescription_CONST];
        pMythStream->m_sMediaSynopsis = pMessage->m_mapParameters[EVENTPARAMETER_SynposisDescription_CONST];

        m_pMedia_Plugin->MediaInfoChanged(pMythStream);
    }

    return true;
}
/*

COMMANDS TO IMPLEMENT

*/

//<-dceag-sample-b->!
//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void MythTV_PlugIn::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    g_pPlutoLogger->Write(LV_STATUS, "Jump to position called %s", sValue_To_Assign.c_str());

    class MythTvStream *pMediaStream =
        (MythTvStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);

    if( !pMediaStream )
        return;  /** Can't do anything */


    WatchTVRequestResult result;
    result = m_pMythWrapper->ProcessWatchTvRequest(sValue_To_Assign);
    switch ( result )
    {
        case WatchTVResult_Tuned:
            return;
        case WatchTVResult_InTheFuture:
        {
            DCE::CMD_Goto_Screen
                cmdGotoScreen(
                    m_dwPK_Device,
                    pMessage->m_dwPK_Device_From,
                    0,
                    StringUtils::itos(DESIGNOBJ_mnuPVROptions_CONST).c_str(),
                    "", "", false);

            SendCommand(cmdGotoScreen);
        }

        default:
            return;
        }
}

//<-dceag-c185-b->

	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

void MythTV_PlugIn::CMD_Schedule_Recording(string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c185-e->
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    g_pPlutoLogger->Write(LV_STATUS, "Jump to position called %s", sProgramID.c_str());

    class MythTvStream *pMediaStream =
        (MythTvStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);

    if( !pMediaStream )
        return;  /** Can't do anything */

    switch( m_pMythWrapper->ProcessAddRecordingRequest(sProgramID) )
    {
        case ScheduleRecordTVResult_Failed:
            EVENT_Error_Occured("The Recording of the event failed");
            sCMD_Result = "Failure";
            break;
        case ScheduleRecordTVResult_WithConflicts:
            EVENT_PVR_Rec_Sched_Conflict();
            sCMD_Result = "Conflicts";
            break;
        default:
            break;
    }
}
