//<-dceag-d-b->
#include "MythTV_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../FakeEPG/Database_FakeEPG.h"
#include "../FakeEPG/Table_Listing.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "DataGrid.h"

#include "MythTvEPGWrapper.h"


//<-dceag-const-b->
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pDatabase_FakeEPG = new Database_FakeEPG();
    if(!m_pDatabase_FakeEPG->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),"FakeEPG",m_pRouter->iDBPort_get()) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
        m_bQuit=true;
        return;
    }
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
    // Get a pointer to the media plugin
    m_pMedia_Plugin=NULL;
    ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Media_Plugin_CONST);
    if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV plug in cannot find media handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
        return false;
    }
    m_pMedia_Plugin=(Media_Plugin *) pListCommand_Impl->front();
    m_pMedia_Plugin->RegisterMediaPlugin(this,this,DEVICETEMPLATE_MythTV_Player_CONST,true);

    // And the datagrid plug-in
    m_pDatagrid_Plugin=NULL;
    pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);
    if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"File grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
        return false;
    }
    m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();
    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::CurrentShows))
        ,DATAGRID_EPG_Current_Shows_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShows))
        ,DATAGRID_EPG_All_Shows_CONST);

    return Connect();
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

    g_pPlutoLogger->Write(LV_STATUS, "Starting media stream playback--sending command, waiting for response");

    g_pPlutoLogger->Write(LV_CRITICAL, "Media type %d %s", pMythTvStream->m_iPK_MediaType, pMythTvStream->m_dequeFilename.size() ? pMythTvStream->m_dequeFilename.front().c_str() : "empty files");

    string Response;

    DCE::CMD_Play_Media cmd(m_dwPK_Device,
        pMythTvStream->m_dwPK_Device,
        "0", // todo -- the real channel
        //                 pMythTvStream->m_dequeFilename.front(),
        pMythTvStream->m_iPK_MediaType,
        pMythTvStream->m_iStreamID_get(),
        0);

    m_pMedia_Plugin->MediaInfoChanged(pMythTvStream);

    if( !SendCommand(cmd, 1, &Response) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV player didn't respond to play media command!");
        // handle failure
#pragma warning("Ignore this for now")
        return true;
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS,"MythTV player responded to play media command!");
    return true;
}

bool MythTV_PlugIn::StopMedia(class MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Stopping media stream playback--sending command, waiting for response");

    DCE::CMD_Stop_Media cmd(m_dwPK_Device,
        pMediaStream->m_dwPK_Device,
        pMediaStream->m_iStreamID_get());
    string Response;
    if( !SendCommand(cmd, 1, &Response) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV player didn't respond to stop media command!");
        // handle failure
#pragma warning("Ignore this for now")
        return true;
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS,"MythTV player responded to stop media command!");
    return true;
}

bool MythTV_PlugIn::BroadcastMedia(class MediaStream *pMediaStream)
{
    return true;
}

class MediaStream *MythTV_PlugIn::CreateMediaStream(class MediaPluginInfo *pMediaPluginInfo,int PK_Device_Source,string Filename,int StreamID)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    MythTvStream *pMediaStream = new MythTvStream(this,pMediaPluginInfo, pMediaPluginInfo->m_iPK_DesignObj, 0, st_RemovableMedia,StreamID);  // hack hack hack
    m_pDatabase_FakeEPG->Listing_get()->GetRows("1=1",&pMediaStream->m_vectRow_Listing); // temporary epg hack
    pMediaStream->m_iCurrentShow=0;
    pMediaStream->UpdateDescriptions();

    if( !PK_Device_Source && pMediaPluginInfo->m_listMediaDevice.size() )
    {
        MediaDevice *pMediaDevice = pMediaPluginInfo->m_listMediaDevice.front();
        PK_Device_Source=pMediaDevice->m_pDeviceData_Router->m_dwPK_Device;
    }
    DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Source);

    bool bFoundDevice=false;
    if( pDeviceData_Router->m_dwPK_DeviceTemplate==DEVICETEMPLATE_MythTV_PlugIn_CONST )
    {
        pMediaStream->m_dwPK_Device = PK_Device_Source;
        bFoundDevice=true;
    }

    if( !bFoundDevice )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Couldn't find myth tv player for create media stream");
    }

    return pMediaStream;
}

void MythTvStream::UpdateDescriptions()
{
    Row_Listing *pRow_Listing = m_vectRow_Listing[m_iCurrentShow];
    m_sMediaDescription=pRow_Listing->ChannelName_get();
    m_sSectionDescription=pRow_Listing->ShowName_get();
    m_sMediaSynopsis=pRow_Listing->Synopsis_get();
}

class DataGridTable *MythTV_PlugIn::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());

    if ( ! m_pAllShowsDataGrid )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Building a new MythTvEPGWrapper object as the datagrid");
        m_pAllShowsDataGrid = new MythTvEPGWrapper();
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    m_pAllShowsDataGrid->rebuildDataGrid(currentTime, currentTime.addDays(3)); // i want 3 days.)
    return m_pAllShowsDataGrid;
}

class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    class MediaStream *pMediaStream = m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);
    if( !pMediaStream || pMediaStream->GetType()!=MEDIASTREAM_TYPE_MYTHTV )  // TODO -- probably should have a virtual GetID method to confirm it's the right type to cast
        return NULL;

    class MythTvStream *pMythTvStream = (MythTvStream *) pMediaStream;

    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    for(size_t s=0;s<pMythTvStream->m_vectRow_Listing.size();++s)
    {
        Row_Listing *pListing = pMythTvStream->m_vectRow_Listing[s];
        pCell = new DataGridCell(StringUtils::itos(pListing->ChannelNum_get()) + " " + pListing->ChannelName_get() + " - " + pListing->ShowName_get(),StringUtils::itos(pListing->PK_Listing_get()));
        pDataGrid->SetData(0,s,pCell);
    }

    return pDataGrid;
}

/*

COMMANDS TO IMPLEMENT

*/

//<-dceag-sample-b->!
//<-dceag-c65-b->
/*
    COMMAND: #65 - Jump Position In Playlist
    COMMENTS: Change channels.  +1 and -1 mean up and down 1 channel.
    PARAMETERS:
        #5 Value To Assign
            The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.
*/
void MythTV_PlugIn::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    class MythTvStream *pMediaStream = (MythTvStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);
    if( !pMediaStream )
        return;  // Can't do anything

    if( sValue_To_Assign=="+1" )
    {
        pMediaStream->m_iCurrentShow++;
        if( pMediaStream->m_iCurrentShow >= (int) pMediaStream->m_vectRow_Listing.size() )
            pMediaStream->m_iCurrentShow=0;
    }
    else
    {
        pMediaStream->m_iCurrentShow--;
        if( pMediaStream->m_iCurrentShow<0  )
            pMediaStream->m_iCurrentShow= (int) pMediaStream->m_vectRow_Listing.size()-1;
    }
    pMediaStream->UpdateDescriptions();
    m_pMedia_Plugin->MediaInfoChanged(pMediaStream);
}

