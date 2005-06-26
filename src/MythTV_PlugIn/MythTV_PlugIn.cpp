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
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Table_EventParameter.h"

#include "DataGrid.h"

#include "MythTvWrapper.h"
#include "../Orbiter_Plugin/OH_Orbiter.h"
#include <libmythtv/frame.h>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    m_pMythBackend_ProxyDevice = NULL;
	m_pMythWrapper = new MythTvWrapper(this);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_PlugIn::~MythTV_PlugIn()
//<-dceag-dest-e->
{
	delete m_pMythWrapper;

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();
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

/*
	string SQL = "SELECT FK_Device,IK_DeviceData FROM Device_DeviceData JOIN Device ON PK_Device=FK_Device WHERE FK_Installation="
		+ StringUtils::itos(m_pRouter->m_Installation_get()) + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_MythTV_PVR_Input_CONST);

    PlutoSqlResult result;
    MYSQL_ROW row;
	m_pMedia_Plugin->m_pDatabase_pluto_main->threaded_mysql_query(SQL);
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
			int PK_Device = atoi(row[0]);
			int PK_Device = atoi(row[0]);

		}
	}
*/

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

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_MythTV_Channel_Changed_CONST );
	// RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Info_Changed_CONST );

	BuildAttachedInfraredTargetsMap();
    return Connect(PK_DeviceTemplate_get());
}

void MythTV_PlugIn::BuildAttachedInfraredTargetsMap()
{
	string SQL = 	"SELECT FK_Device, IK_DeviceData "
					"FROM Device_DeviceData "
						"JOIN Device ON PK_Device=FK_Device "
					"WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " AND "
						"FK_DeviceData=" + StringUtils::itos(DEVICEDATA_MythTV_PVR_Input_CONST);

	PlutoSqlResult result;
	MYSQL_ROW row;

	if( ( result.r=m_pMedia_Plugin->GetMainDatabaseConnection()->mysql_query_result( SQL ) ) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			if( !row[1] || !row[0])
			{
				g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::BuildAttachedInfraredTargetsMap() found a database row with null entries. Ignoring!");
				continue;
			}

			int PK_Device = atoi(row[0]);
			int mythConnectionID = atoi(row[1]);

			if ( PK_Device == 0 || mythConnectionID == 0 )
			{
				g_pPlutoLogger->Write(LV_STATUS,
							"MythTV_PlugIn::BuildAttachedInfraredTargetsMap() one of the device ID or the mythconnection specification is 0: (device: %d, mythconnID: %d)",
							PK_Device, mythConnectionID);
				continue;
			}

			g_pPlutoLogger->Write(LV_STATUS, "Mapping MythTV tuner input with ID %d to device ID %d", mythConnectionID, PK_Device);
			m_mapMythInputsToDevices[mythConnectionID] = PK_Device;
		}
	}
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
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	MythTvMediaStream *pMythTvMediaStream = NULL;

	if( (pMythTvMediaStream = ConvertToMythMediaStream(pMediaStream, "MythTV_PlugIn::StartMedia() ")) == NULL)
        return false;

    string Response;

	m_dwTargetDevice = pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_PVR_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_On(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0,"");
		}
	}


	if ( pMythTvMediaStream->ShouldTuneToNewChannel() )
	{
		if ( WatchTVResult_Tuned == m_pMythWrapper->ProcessWatchTvRequest( pMythTvMediaStream->m_iNextProgramChannelID,
					pMythTvMediaStream->m_iNextProgramTimeYear, pMythTvMediaStream->m_iNextProgramTimeMonth, pMythTvMediaStream->m_iNextProgramTimeDay,
					pMythTvMediaStream->m_iNextProgramTimeHour, pMythTvMediaStream->m_iNextProgramTimeMinute))
		{

			// The player knows how to actually tune to the proper channel. There is no need to hit the database for the proper xmltv id here.
			DCE::CMD_Tune_to_channel tuneCommand(
					m_dwPK_Device,
					pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					"",
					StringUtils::itos(pMythTvMediaStream->m_iNextProgramChannelID));

			SendCommand(tuneCommand);
		}
		else
		{
			DCE::CMD_Goto_Screen cmdGotoScreen(
                    m_dwPK_Device, pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
                    0, StringUtils::itos(DESIGNOBJ_mnuPVROptions_CONST).c_str(),
                    "", "", false, false);
			SendCommand(cmdGotoScreen);

			return true;
		}
	}
	else
	{
		// if there is no next channel to tune then just start it.
		DCE::CMD_Start_TV cmd(m_dwPK_Device, pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
		SendCommand(cmd);
	}

	m_pMedia_Plugin->MediaInfoChanged(pMythTvMediaStream);
    return true;
}

bool MythTV_PlugIn::StopMedia(class MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Stopping media stream playback--sending command, waiting for response");

	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);


	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_PVR_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_Off(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0);
		}
	}

	int i; // report the current playback position here.

    DCE::CMD_Stop_TV cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	// this is useless here because all the message processing in the Media Plugin is done with the m_MediaMutex taken and
	// since this mutex is recursive the release here is useless and the same apply for all the media plugin processing functions.
	mm.Release();

	/** Changing send with confirmation to a simple SendCommand because of the locking issues. We can never be sure that the
		Target device will answer us.


	string Response;
    if( !SendCommand(cmd, &Response) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"MythTV player didn't respond to stop media command!");
		// TODO: See how to handle failure here
        return false;
    }

    g_pPlutoLogger->Write(LV_STATUS,"MythTV player responded to stop media command!");
    return true;
*/
	g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Sending command to stop media to the player: %d", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	SendCommand(cmd);
	g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Returning from stop media command to the player: %d", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);

	return true;
}


class MediaStream *MythTV_PlugIn::CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeFilenames,int StreamID)
{
    if ( m_pMedia_Plugin == NULL )
        return NULL;

	EntertainArea *pEntertainArea=NULL;
	if( vectEntertainArea.size() )
		pEntertainArea=vectEntertainArea[0];

    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    if ( pMediaHandlerInfo == NULL )
        return NULL;

    if( (!pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_MythTV_Player_CONST) && pEntertainArea )
    {
        ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Player_CONST);

		if( pListMediaDevice && pListMediaDevice->size())
		{
			if ( pListMediaDevice->size() > 1 )
				g_pPlutoLogger->Write(LV_STATUS, "There are multiple Myth Player's (%d devices) in the ent area %d. This is an error picking only the first one!", pListMediaDevice->size(), pEntertainArea->m_iPK_EntertainArea);

		    pMediaDevice = pListMediaDevice->front();
		}
    }

    if( ! pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MythTV_Player_CONST )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Myth plugin being told to play in an entertainment area without a MythTV Player");
        return NULL;
    }

	g_pPlutoLogger->Write(LV_STATUS, "Found playback device to be: %d %s (at IP address: %s)!",
				pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str(),
				pMediaDevice->m_pDeviceData_Router->GetIPAddress().c_str());

    MythTvMediaStream *pMediaStream = new MythTvMediaStream(pMediaHandlerInfo, pMediaDevice, 0, st_RemovableMedia,StreamID);

	if ( m_pMythBackend_ProxyDevice == NULL )
	{
		ListDeviceData_Router *pDevices;

		pDevices = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Backend_Proxy_CONST);

		if ( pDevices == NULL )
			g_pPlutoLogger->Write(LV_CRITICAL, "There need to be at least one device with template %d in the system if you want to be able to see what shows are playing on various devices", DEVICETEMPLATE_MythTV_Backend_Proxy_CONST);
		else
		{
			if ( pDevices->size() > 1 )
				g_pPlutoLogger->Write(LV_WARNING, "You only need one device of type %d in the system (it should run where the MythTVMasterBackend it running).", DEVICETEMPLATE_MythTV_Backend_Proxy_CONST);

			m_pMythBackend_ProxyDevice = pDevices->front();

			g_pPlutoLogger->Write(LV_STATUS, "Found %d as the MythTV Backend Proxy device", m_pMythBackend_ProxyDevice->m_dwPK_Device);
		}
	}

	if ( m_pMythBackend_ProxyDevice != NULL )
	{
		DCE::CMD_Track_Frontend_At_IP enableTracking(
				m_dwPK_Device,	// from me
				m_pMythBackend_ProxyDevice->m_dwPK_Device, // to the proxy
				pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->GetIPAddress());

		SendCommand(enableTracking);
	}

	pMediaStream->m_sMediaDescription = "Not available";
    pMediaStream->m_sSectionDescription = "Not available";
    pMediaStream->m_sMediaSynopsis = "Not available";
    m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

    return pMediaStream;
}


MythTvMediaStream* MythTV_PlugIn::ConvertToMythMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	if ( pMediaStream == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_MYTHTV )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a MythTvMediaStream!").c_str());
		return NULL;
	}

	return static_cast<MythTvMediaStream*>(pMediaStream);
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
        return new DataGridTable();
    }

    return m_pMythWrapper->createShowsForMobiles(GridID, QDateTime::currentDateTime());
}

class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	MythTvMediaStream *pMythTvMediaStream = NULL;

	if ( (pMythTvMediaStream = ConvertToMythMediaStream(m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From), "MythTV_PlugIn::CurrentShows() ")) == NULL)
		return new DataGridTable();

    DataGridTable *pDataGrid = new DataGridTable();

	/** @brief Find out where is this function used!! */
//     DataGridCell *pCell;

//     for(size_t s=0;s < pMythTvMediaStream->m_vectRow_Listing.size();++s)
//     {
/** @test
//         Row_Listing *pListing = pMythTvStream->m_vectRow_Listing[s];
//         pCell = new DataGridCell(StringUtils::itos(pListing->ChannelNum_get()) + " " + pListing->ChannelName_get() + " - " +                     pListing->ShowName_get(),StringUtils::itos(pListing->PK_Listing_get()));
*/
//         pCell = new DataGridCell(StringUtils::itos(1) + " " + "ChannelName" + " - " + "ShowName",StringUtils::itos(1));
//         pDataGrid->SetData(0,s,pCell);
//     }

    return pDataGrid;
}

bool MythTV_PlugIn::MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Router *pDeviceFrom, class DeviceData_Router *pDeviceTo )
{
    MythTvMediaStream *pMythTvStream;

    if ( pDeviceFrom->m_dwPK_DeviceTemplate == DEVICETEMPLATE_MythTV_Backend_Proxy_CONST )
    {
		int playbackDevice = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
		int tunedChannel = atoi(pMessage->m_mapParameters[EVENTPARAMETER_MythTV_ChannelID_CONST].c_str());
		if ( playbackDevice == 0 )
		{
			g_pPlutoLogger->Write(LV_WARNING, "MythTV_PlugIn::MediaInfoChanged() called for an event which didn't provided a proper device ID");
			return false;
		}

		if ( (pMythTvStream = ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[playbackDevice]), "MythTV_PlugIn::MediaInfoChanged() ")) == NULL)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Could not detect a valid MythTV media stream based on the device %d", pDeviceFrom->m_dwPK_Device);
			return false;
		}

		if ( m_pMythWrapper->GetCurrentChannelProgram(tunedChannel,
															pMythTvStream->m_sMediaDescription,
															pMythTvStream->m_sSectionDescription,
															pMythTvStream->m_sMediaSynopsis) == false )
		{
			pMythTvStream->m_sMediaDescription = "Channel: " + StringUtils::itos(tunedChannel);
			pMythTvStream->m_sSectionDescription = "Show info not available";
			pMythTvStream->m_sMediaSynopsis = "Other info not available";
		}

		pMythTvStream->m_iNextProgramChannelID = pMythTvStream->m_iCurrentProgramChannelID = tunedChannel;
		m_pMedia_Plugin->MediaInfoChanged(pMythTvStream);
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

//     class MythTvStream *pMediaStream =
//         (MythTvStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);
//
//     if( !pMediaStream )
//         return;  /** Can't do anything */

	vector<string> vectData;
	StringUtils::Tokenize(sValue_To_Assign, "|", vectData);

	if ( vectData.size() != 2 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Invalid format in CMD_Jump_Position_In_Playlist: if you are telling it to jump for an external device call it with 2 params connid|channelNumber");
		return;
	}

	int connectionID = atoi(vectData[0].c_str());
	int targetChannelID = atoi(vectData[1].c_str());

	if ( m_mapMythInputsToDevices.find(connectionID) == m_mapMythInputsToDevices.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "No Pluto device is connected to this myth TV according to the pluto database.");
		return;
	}

	// the player knows the internal mythtv mappings. There is no need to hit the database here for the xmltv id.
	DCE::CMD_Tune_to_channel tuneCommand(m_dwPK_Device, m_mapMythInputsToDevices[connectionID], "", StringUtils::itos(targetChannelID));
	SendCommand(tuneCommand);
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
//<-dceag-createinst-b->!
