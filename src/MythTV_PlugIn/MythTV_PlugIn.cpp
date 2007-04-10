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

#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_Users.h"
#include "../pluto_media/Table_LongAttribute.h"
#include "../pluto_media/Table_MediaProvider.h"
#include "../pluto_media/Table_ProviderSource.h"
#include "../pluto_media/Table_Picture.h"
#include "../pluto_media/Define_FileFormat.h"
#include "../pluto_media/Define_MediaSubType.h"
#include "../pluto_media/Define_AttributeType.h"

#include "../Gen_Devices/AllScreens.h"

#include "DataGrid.h"
#include "EPGGrid.h"
#include "MythTvMediaStream.h"
#ifndef WIN32
//#include "MythTvWrapper.h"
//#include <libmythtv/frame.h>
#endif

#define MINIMUM_MYTH_SCHEMA		1123
#define CHECK_FOR_NEW_RECORDINGS	1
#define SYNC_PROVIDERS				2
#define RUN_BACKEND_STARTER			3

#include "../Orbiter_Plugin/OH_Orbiter.h"

// For sorting channels
static bool ChannelComparer(MythChannel *x, MythChannel *y)
{
	return x->m_dwChanNum<y->m_dwChanNum;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pEPGGrid = NULL;
	m_bFillDbRunning = m_bNeedToRunFillDb = false;
//	m_bPreProcessSpeedControl=false;  // We do some ridiculous hacks in Myth player to convert speed control commands to keystrokes
//	m_pMythWrapper = NULL;
	m_pMySqlHelper_Myth = NULL;
	m_pAlarmManager=NULL;
	m_dwPK_File_LastCheckedForNewRecordings=0;

	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
}

//<-dceag-getconfig-b->
bool MythTV_PlugIn::GetConfig()
{
	if( !MythTV_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	m_pMySqlHelper_Myth = new MySqlHelper(m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ),"mythconverg");
	m_pEPGGrid = new EPGGrid(m_pMySqlHelper_Myth);

	UpdateMythSetting("JobAllowUserJob1","1","*");
	UpdateMythSetting("AutoRunUserJob1","1","");
	UpdateMythSetting("UserJob1","/usr/pluto/bin/SaveMythRecording.sh %CHANID% %STARTTIME% %DIR% %FILE%","");
	UpdateMythSetting("UserJobDesc1","Save the recorded show into Pluto's database","");
	UpdateMythSetting("Language","EN","*",true);

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1);      //4 = number of worker threads
	m_pAlarmManager->AddRelativeAlarm(30,this,CHECK_FOR_NEW_RECORDINGS,NULL);
	m_pAlarmManager->AddRelativeAlarm(5,this,RUN_BACKEND_STARTER,NULL);
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_PlugIn::~MythTV_PlugIn()
//<-dceag-dest-e->
{
#ifndef WIN32
//	delete m_pMythWrapper;
#endif

	delete m_pEPGGrid;
	m_pEPGGrid = NULL;

	delete m_pMySqlHelper_Myth;
	m_pMySqlHelper_Myth = NULL;

	PurgeChannelList();
}

void MythTV_PlugIn::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();
    /** Get a pointer to the media plugin */
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pGeneral_Info_Plugin=( General_Info_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pMedia_Plugin || !m_pGeneral_Info_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to myth plugin");
		return false;
	}

    m_pMedia_Plugin->RegisterMediaPlugin(this, this, DEVICETEMPLATE_MythTV_Player_CONST, true);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::CurrentShows))
                                                ,DATAGRID_EPG_Current_Shows_CONST,PK_DeviceTemplate_get());

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShows))
                                                ,DATAGRID_EPG_All_Shows_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::TvProviders))
												,DATAGRID_TV_Providers_CONST,PK_DeviceTemplate_get());

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::FavoriteChannels ))
										        , DATAGRID_Favorite_Channels_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::FavoriteShows ))
										        , DATAGRID_Favorite_Shows_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::ThumbnailableAttributes ))
										        , DATAGRID_Thumbnailable_Attributes_CONST,PK_DeviceTemplate_get() );
    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::ThumbnailableAttributes ))
										        , DATAGRID_Thumbnailable_Attributes_CONST,DEVICETEMPLATE_MythTV_Player_CONST );

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_MythTV_Channel_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::NewBookmarks ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Save_Bookmark_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::ScanningProgress ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Channel_Scan_Progress_CONST );

	SetPaths();

	// Don't actually build the bookmark/channel list because Sync_Cards_Providers will fill in m_mapDevicesToSources,
	// but it's not run until about 20 seconds after everything starts so if it needs to send a message to the orbiters
	// or use AppServer, those devices will be available
	m_bBookmarksNeedRefreshing=true;

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
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::BuildAttachedInfraredTargetsMap() found a database row with null entries. Ignoring!");
				continue;
			}

			int PK_Device = atoi(row[0]);
			int mythConnectionID = atoi(row[1]);

			if ( PK_Device == 0 || mythConnectionID == 0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,
							"MythTV_PlugIn::BuildAttachedInfraredTargetsMap() one of the device ID or the mythconnection specification is 0: (device: %d, mythconnID: %d)",
							PK_Device, mythConnectionID);
				continue;
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Mapping MythTV tuner input with ID %d to device ID %d", mythConnectionID, PK_Device);
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
void MythTV_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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

bool MythTV_PlugIn::StartMedia(class MediaStream *pMediaStream,string &sError)
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	MythTvMediaStream *pMythTvMediaStream = NULL;

	if( (pMythTvMediaStream = ConvertToMythMediaStream(pMediaStream, "MythTV_PlugIn::StartMedia() ")) == NULL)
        return false;

    string Response;

	m_dwTargetDevice = pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_On(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0,"");
		}
	}

	DCE::CMD_Play_Media cmd(m_dwPK_Device, pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,pMythTvMediaStream->m_iPK_MediaType,pMythTvMediaStream->m_iStreamID_get( ),pMediaStream->m_sLastPosition,"");
	SendCommand(cmd);

#endif
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool MythTV_PlugIn::StopMedia(class MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media stream playback--sending command, waiting for response");

	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);


	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_Off(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0);
		}
	}

	string sLastPosition;
    DCE::CMD_Stop_Media cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,0,&sLastPosition);
	// this is useless here because all the message processing in the Media Plugin is done with the m_MediaMutex taken and
	// since this mutex is recursive the release here is useless and the same apply for all the media plugin processing functions.
	mm.Release();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Sending command to stop media to the player: %d", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	SendCommand(cmd);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Returning from stop media command to the player: %d last pos %s", 
		pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sLastPosition.c_str());
	pMediaStream->m_sLastPosition=sLastPosition;

	return MediaHandlerBase::StopMedia(pMediaStream);
}


class MediaStream *MythTV_PlugIn::CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo,int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeFilenames,int StreamID)
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
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "There are multiple Myth Player's (%d devices) in the ent area %d. This is an error picking only the first one!", pListMediaDevice->size(), pEntertainArea->m_iPK_EntertainArea);

		    pMediaDevice = pListMediaDevice->front();
		}
    }

    if( ! pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MythTV_Player_CONST )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Myth plugin being told to play in an entertainment area without a MythTV Player");
        return NULL;
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found playback device to be: %d %s (at IP address: %s)!",
				pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str(),
				pMediaDevice->m_pDeviceData_Router->GetIPAddress().c_str());

    MythTvMediaStream *pMediaStream = new MythTvMediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, 0, st_RemovableMedia,StreamID);

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
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_MYTHTV )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a MythTvMediaStream!").c_str());
		return NULL;
	}

	return static_cast<MythTvMediaStream*>(pMediaStream);
}

// Parms = Users,PK_EntertainmentArea
class DataGridTable *MythTV_PlugIn::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign,
	Message *pMessage)
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();
    int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	DataGridTable *pDataGridTable = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos=0;
	int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());
    
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( iPK_EntertainArea );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}

	string sProvider;
	map<int,bool> mapVideoSourcesToUse;
	// The source is 0, means all myth channels and video sources.  Otherwise, just those for the given device (like a cable box)
	int PK_Device_Source = pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ? 0 : pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	list_int *p_list_int = &(m_mapDevicesToSources[PK_Device_Source]);
	if( p_list_int->size() )
	{
		sProvider = " AND sourceid in (";
		bool bFirst=true;
		for(list_int::iterator it=p_list_int->begin();it!=p_list_int->end();++it)
		{
			if( bFirst )
				bFirst = false;
			else
				sProvider += ",";
			sProvider += StringUtils::itos( *it );
			mapVideoSourcesToUse[ *it ] = true;
		}
		sProvider += ")";
	}

	for(map<int,MythChannel *>::iterator it=m_mapMythChannel.begin();it!=m_mapMythChannel.end();++it)
	{
		it->second->m_cAddedAlready='N';
		it->second->m_pCell=NULL;
	}

	string sBookmark_Program = m_mapProgramBookmarks[0];
	string sBookmark_Program2 = m_mapProgramBookmarks[iPK_Users];
	if( sBookmark_Program.empty()==false && sBookmark_Program2.empty()==false )
		sBookmark_Program += ",";
	sBookmark_Program += sBookmark_Program2;
	if( sBookmark_Program.empty() )
		sBookmark_Program = "-999";  // Something we know is not going to happen so the sql syntax is correct

	string sBookmark_Series = m_mapSeriesBookmarks[0];
	string sBookmark_Series2 = m_mapSeriesBookmarks[iPK_Users];
	if( sBookmark_Series.empty()==false && sBookmark_Series2.empty()==false )
		sBookmark_Series += ",";
	sBookmark_Series += sBookmark_Series2;
	if( sBookmark_Series.empty() )
		sBookmark_Series = "-999";  // Something we know is not going to happen so the sql syntax is correct

	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL = "SELECT program.chanid, title, starttime, endtime, seriesid, programid, "
		"program.seriesid in (" + sBookmark_Series + ") as fav_seriesid, program.programid in (" + sBookmark_Program + ") as fav_programid "
		"FROM program JOIN channel on program.chanid=channel.chanid "
		"WHERE starttime < '" + StringUtils::SQLDateTime() + "' AND endtime>'" + StringUtils::SQLDateTime() + "' " + sProvider;

	int iRow=0;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))!=NULL )
	{
		while((row = mysql_fetch_row(result.r)))
		{
			MythChannel *pMythChannel = m_mapMythChannel_Find( atoi(row[0]) );
			if( !pMythChannel )
				continue; // Shouldn't happen

			string sChannelName = StringUtils::itos(pMythChannel->m_dwChanNum) + " " + pMythChannel->m_sShortName;
			string sDesc = sChannelName + " " + row[1];
			pCell = new DataGridCell(sDesc,row[0]);
			if( pMythChannel->m_pPic )
			{
				char *pPic = new char[ pMythChannel->m_Pic_size ];
				memcpy(pPic,pMythChannel->m_pPic,pMythChannel->m_Pic_size);
				pCell->SetImage( pPic, pMythChannel->m_Pic_size, GR_JPG );
			}

			time_t tStart = StringUtils::SQLDateTime( row[2] );
			time_t tStop = StringUtils::SQLDateTime( row[3] );
			string sTime = StringUtils::HourMinute(tStart) + " - " + StringUtils::HourMinute(tStop);
			string sNumber = NULL != row[0] ? row[0] : "";
			string sInfo = NULL != row[4] ? row[1]: "";

			pCell->m_mapAttributes["Number"] = sNumber;
			pCell->m_mapAttributes["Name"] = sChannelName;
			pCell->m_mapAttributes["Time"] = sTime;
			pCell->m_mapAttributes["Info"] = sInfo;
			if( row[4] )
				pCell->m_mapAttributes["Series"] = row[4];
			if( row[5] )
				pCell->m_mapAttributes["Program"] = row[5];

			pMythChannel->m_pCell = pCell;

			if( (row[6] && row[6][0]=='1') || (row[7] && row[7][0]=='1') )
			{
				pMythChannel->m_cAddedAlready = 'F';
				pDataGridTable->SetData(0,iRow++,pMythChannel->m_pCell);
			}
		}
	}

	ListMythChannel *pListMythChannel = &(m_map_listMythChannel[iPK_Users]);
	if( !pListMythChannel ) // Shouldn't happen
		return NULL;

	bool bAllSource = mapVideoSourcesToUse.empty();
	int iPositionInChannelList = 0;  // Keep track of how far down we are in pListMythChannel, so we know if we're still in the favorites
	int iFavorites = m_mapUserFavoriteChannels[iPK_Users];
	for(ListMythChannel::iterator it=pListMythChannel->begin();it!=pListMythChannel->end();++it)
	{
		MythChannel *pMythChannel = *it;
		if( bAllSource==false && mapVideoSourcesToUse[ pMythChannel->m_dwSource ]==false )  // Not a source for this list
			continue;

		if( pMythChannel->m_cAddedAlready=='F' && iPositionInChannelList<iFavorites )
		{
			iPositionInChannelList++;
			continue; // Don't put it in the favorites twice
		}
		if( !pMythChannel->m_pCell )  // There's nothing in the program guide for this channel
		{
			string sChannelName = StringUtils::itos(pMythChannel->m_dwChanNum) + " " + pMythChannel->m_sShortName;
			pMythChannel->m_pCell = new DataGridCell(sChannelName,StringUtils::itos(pMythChannel->m_dwID));
			pMythChannel->m_pCell->m_mapAttributes["Number"] = StringUtils::itos(pMythChannel->m_dwID);
			pMythChannel->m_pCell->m_mapAttributes["Name"] = sChannelName;
		}

		if( pMythChannel->m_cAddedAlready )
			// The cell is in the list twice (ie favorites + the main list).  Make a copy since you can't have the same cell there twice or it will get deleted twice
			pMythChannel->m_pCell = new DataGridCell( pMythChannel->m_pCell );  

		if( iPositionInChannelList<iFavorites )
			pMythChannel->m_pCell->m_mapAttributes["Favorite"]="1";

		iPositionInChannelList++;

		pMythChannel->m_cAddedAlready='Y';
		pDataGridTable->SetData(0,iRow++,pMythChannel->m_pCell);
	}

	return pDataGridTable;
}

class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();
    int nHeight = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Height_CONST].c_str());
	DataGridTable *pDataGridTable = new DataGridTable();
	
	string::size_type pos = 0;
	string sChanId = StringUtils::Tokenize( Parms, ",", pos);
	int iPK_Users = atoi(StringUtils::Tokenize( Parms, ",", pos).c_str());
	bool bOnePageOnly = pos < Parms.size(); // If there's trailing data, we're supposed to only cache one page
	
	if( atoi(sChanId.c_str())==0 )
		return pDataGridTable;

	time_t tNow = time(NULL);
	struct tm t;
	localtime_r(&tNow,&t);
	int Month = t.tm_mon;
	int Day = t.tm_mday;

	DataGridCell *pCell;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::CurrentShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());

	string sBookmark_Program = m_mapProgramBookmarks[0];
	string sBookmark_Program2 = m_mapProgramBookmarks[iPK_Users];
	if( sBookmark_Program.empty()==false && sBookmark_Program2.empty()==false )
		sBookmark_Program += ",";
	sBookmark_Program += sBookmark_Program2;
	if( sBookmark_Program.empty() )
		sBookmark_Program = "-999";  // Something we know is not going to happen so the sql syntax is correct

	string sBookmark_Series = m_mapSeriesBookmarks[0];
	string sBookmark_Series2 = m_mapSeriesBookmarks[iPK_Users];
	if( sBookmark_Series.empty()==false && sBookmark_Series2.empty()==false )
		sBookmark_Series += ",";
	sBookmark_Series += sBookmark_Series2;
	if( sBookmark_Series.empty() )
		sBookmark_Series = "-999";  // Something we know is not going to happen so the sql syntax is correct

	string sProvider;
	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL =
		"SELECT program.chanid, program.programid, program.seriesid, title, starttime, endtime, description, Picture_series.EK_Picture AS Picture_series, Picture_program.EK_Picture AS Picture_program, "
		"program.seriesid in (" + sBookmark_Series + ") as fav_seriesid, program.programid in (" + sBookmark_Program + ") as fav_programid "
		"FROM program "
		"LEFT JOIN `pluto_myth`.`Picture` AS Picture_series ON Picture_series.seriesid=program.seriesid "
		"LEFT JOIN `pluto_myth`.`Picture` AS Picture_program ON Picture_program.programid=program.programid "
		"WHERE program.chanid=" + sChanId + " AND endtime>'" + StringUtils::SQLDateTime() + "' ORDER BY endtime";

	if( bOnePageOnly )
		sSQL += " LIMIT " + StringUtils::itos(nHeight);

	PlutoSqlResult result;
	MYSQL_ROW row;
	int iRow=0;
	if( (result.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))!=NULL )
	{
		while((row = mysql_fetch_row(result.r)))
		{
			pCell = new DataGridCell(row[3] ? row[3] : "",Parms);
			pCell->m_mapAttributes["chanid"]=row[0];
			pCell->m_mapAttributes["programid"]=row[1];
			pCell->m_mapAttributes["seriesid"]=row[2];
			time_t tStart = StringUtils::SQLDateTime( row[4] );
			time_t tStop = StringUtils::SQLDateTime( row[5] );
			
			struct tm t;
			localtime_r(&tStart,&t);
			string sDate;
			if( t.tm_mon==Month && t.tm_mday==Day )
				sDate = "Today";
			else
				sDate = StringUtils::itos(t.tm_mon+1) + "/" + StringUtils::itos(t.tm_mday);

			string sTime = StringUtils::HourMinute(tStart) + "-" + StringUtils::HourMinute(tStop);
			
			string sPK_Picture;
			if( row[8] && row[8][0] )
				sPK_Picture = row[8];
			else if( row[7] && row[7][0] )
				sPK_Picture = row[7];

			if( sPK_Picture.empty()==false )
			{
				size_t size;
				char *pPic = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + sPK_Picture + "_tn.jpg",size);
				if( pPic )
					pCell->SetImage( pPic, size, GR_JPG );
			}
			if( (row[9] && row[9][0]=='1') || (row[10] && row[10][0]=='1') )
				pCell->m_mapAttributes["Favorite"]="1";

			pCell->m_mapAttributes["Date"] = sDate;
			pCell->m_mapAttributes["Time"] = sTime;
			if( row[6] )
				pCell->m_mapAttributes["Synopsis"] = row[6];

			pDataGridTable->SetData(0,iRow++,pCell);
		}
	}
 
	return pDataGridTable;
}

bool MythTV_PlugIn::MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
#ifndef WIN32
    MythTvMediaStream *pMythTvStream;

    if ( pDeviceFrom->m_dwPK_DeviceTemplate == DEVICETEMPLATE_MythTV_Backend_Proxy_CONST )
    {
		int playbackDevice = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
		int tunedChannel = atoi(pMessage->m_mapParameters[EVENTPARAMETER_MythTV_ChannelID_CONST].c_str());
		if ( playbackDevice == 0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::MediaInfoChanged() called for an event which didn't provided a proper device ID");
			return false;
		}

		if ( (pMythTvStream = ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[playbackDevice],pMessage->m_dwPK_Device_From), "MythTV_PlugIn::MediaInfoChanged() ")) == NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not detect a valid MythTV media stream based on the device %d", pDeviceFrom->m_dwPK_Device);
			return false;
		}
/*
		if ( m_pMythWrapper->GetCurrentChannelProgram(tunedChannel,
															pMythTvStream->m_sMediaDescription,
															pMythTvStream->m_sSectionDescription,
															pMythTvStream->m_sMediaSynopsis) == false )
		{
			pMythTvStream->m_sMediaDescription = "Channel: " + StringUtils::itos(tunedChannel);
			pMythTvStream->m_sSectionDescription = "Show info not available";
			pMythTvStream->m_sMediaSynopsis = "Other info not available";
		}
		*/

		pMythTvStream->m_iNextProgramChannelID = pMythTvStream->m_iCurrentProgramChannelID = tunedChannel;
		m_pMedia_Plugin->MediaInfoChanged(pMythTvStream);
    }
#endif
    return false;
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

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Jump to position called %s", sValue_To_Assign.c_str());

     MythTvMediaStream *pMediaStream =
         (MythTvMediaStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);

	 if( !pMediaStream )
         return;  

	int m_dwTargetDevice = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

	DCE::CMD_Jump_Position_In_Playlist tuneCommand(m_dwPK_Device, m_dwTargetDevice, sValue_To_Assign);
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
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Jump to position called %s", sProgramID.c_str());

    class MythTvStream *pMediaStream =
        (MythTvStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);

    if( !pMediaStream )
        return;  /** Can't do anything */

/*    switch( m_pMythWrapper->ProcessAddRecordingRequest(sProgramID) )
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
    } */
#endif
}
//<-dceag-createinst-b->!


class DataGridTable *MythTV_PlugIn::TvProviders(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
/*
	DataGridCell *pCell;
	if( !m_pMySqlHelper_Myth || !m_pMySqlHelper_Myth->m_bConnected )
		return pDataGrid;

	string::size_type pos=0;
	string sPK_PostalCode = StringUtils::Tokenize(Parms,",",pos);
	bool bInternalTuner = StringUtils::Tokenize(Parms,",",pos)=="1";
	string sSQL = "SELECT PK_Provider,Provider.Description,ProviderType.Description FROM PostalCode_Provider"
		" JOIN Provider ON FK_Provider=PK_Provider"
		" JOIN ProviderType ON FK_ProviderType=PK_ProviderType"
		" WHERE EK_PostalCode=" + sPK_PostalCode +
		" AND FK_ProviderType IN " +
		(bInternalTuner ? "(1,4)" : "(1,2,3,4)");

	PlutoSqlResult result_set;
    MYSQL_ROW row;
	pCell = new DataGridCell("None","0");
	pDataGrid->SetData(0,0,pCell);
	int iRow=1;
	if( (result_set.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) )
	{
		while ((row = mysql_fetch_row(result_set.r)))
		{
			pCell = new DataGridCell(string(row[2]) + ": " + row[1], row[0]);
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}*/

	return pDataGrid;
}

//<-dceag-c698-b->

	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

void MythTV_PlugIn::CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c698-e->
{
}

//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

void MythTV_PlugIn::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::CMD_Set_Active_Menu %s", sText.c_str());

    MythTvMediaStream *pMythTvMediaStream =
		ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[pMessage->m_dwPK_Device_From],pMessage->m_dwPK_Device_From));

    if( !pMythTvMediaStream )
	{
	    LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::CMD_Set_Active_Menu stream is NULL");
        return;  /** Can't do anything */
	}

	int PK_Screen_Remote,PK_Screen_OSD;
	if( sText=="live" )
	{
		PK_Screen_Remote=SCREEN_PVR_CONST;
		PK_Screen_OSD=SCREEN_PvrFullScreen_CONST;
	}
	else if( sText=="nonlive" )
	{
		PK_Screen_Remote=SCREEN_mnuPVRRecording_CONST;
		PK_Screen_OSD=SCREEN_mnuPVRRecording_Full_Screen_CONST;
	}
	else
	{
		PK_Screen_Remote=SCREEN_mnuPVROSD_CONST;
		PK_Screen_OSD=SCREEN_mnuPVR_OSD_Full_Screen_CONST;
	}

	/** We're going to send a message to all the orbiters that are bound to remotes in any of the entertainment areas */
	for( MapEntertainArea::iterator itEA = pMythTvMediaStream->m_mapEntertainArea.begin( );itEA != pMythTvMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
		for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
		{
			BoundRemote *pBoundRemote = ( *itBR ).second;
			if (pBoundRemote)
			{
				RemoteControlSet *pRemoteControlSet =
					pMythTvMediaStream->m_mapRemoteControlSet[pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device];
				if (pRemoteControlSet)
				{
					pRemoteControlSet->m_iPK_Screen_Remote = PK_Screen_Remote;
					pRemoteControlSet->m_iPK_Screen_OSD = PK_Screen_OSD;

					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing bound remote: for orbiter: %d", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
					m_pMedia_Plugin->SetNowPlaying(pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
						pMythTvMediaStream,false,true);
				}
			}
		}
	}
}

//<-dceag-c824-b->

	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

void MythTV_PlugIn::CMD_Sync_Providers_and_Cards(int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
 	string sNoConfig = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,m_dwPK_Device,DEVICEDATA_Dont_Auto_Configure_CONST);
	if( atoi(sNoConfig.c_str()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- skipping because Don't Auto Configure is set");
		return;
	}

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	SetPaths();
	m_bBookmarksNeedRefreshing=true;
	m_mapDevicesToSources.clear();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders");
    MYSQL_ROW row,row2;

	string sSQL = "select data from settings where value='DBSchemaVer'";
	PlutoSqlResult result_set_check;
	if( (result_set_check.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))==NULL || (row=mysql_fetch_row(result_set_check.r))==NULL || atoi(row[0])<MINIMUM_MYTH_SCHEMA )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards skipping now because I need at least schema %d insted of %s",
			MINIMUM_MYTH_SCHEMA, (NULL != result_set_check.r && row && row[0]) ? row[0] : "*none*");
		return;
	}

	sSQL = "update settings set data='xv' where data='xvmc' and value='PreferredMPEG2Decoder'";  // xvmc causes the video to go crazy when overlaid
	m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);

	// Find either inputs with a provider specified, or certain types of devices which should be added, but still need a provider
	sSQL = "select PK_Device,IK_DeviceData FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_EK_MediaProvider_CONST) " AND IK_DeviceData IS NOT NULL AND IK_DeviceData<>'' AND IK_DeviceData<>'NONE' "
		"WHERE IK_DeviceData IS NOT NULL OR (IK_DeviceData IS NULL AND FK_DeviceTemplate IN (" TOSTRING(DEVICETEMPLATE_Antenna_Port_CONST) "))";

	bool bModifiedRows=false; // Keep track of whether or not we changed anything
	PlutoSqlResult result_set;
	if( (result_set.r=m_pMedia_Plugin->m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
	{
		while ((row = mysql_fetch_row(result_set.r)))
		{
			Row_Device *pRow_Device = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow( atoi(row[0]) );
			Row_MediaProvider *pRow_MediaProvider = row[1] ? m_pMedia_Plugin->m_pDatabase_pluto_media->MediaProvider_get()->GetRow( atoi(row[1]) ) : NULL;

			if( !pRow_Device )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders cannot find device %s provider %s",row[0],row[1]);
				continue;
			}

			Row_Device_DeviceData *pRow_Device_DeviceData_UseInMyth = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Dont_Consolidate_Media_CONST);
			bool bUseInMyth = !pRow_Device_DeviceData_UseInMyth || pRow_Device_DeviceData_UseInMyth->IK_DeviceData_get()!="1";
			Row_Device *pRow_Device_Source = pRow_Device;

			string::size_type pos=0;
			string sMediaProviderID = pRow_MediaProvider ? pRow_MediaProvider->ID_get() : "";
			string sUsername = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			string sPassword = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			int PK_DeviceTemplate_MediaType = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
			int PK_ProviderSource = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
			string sLineup = StringUtils::Tokenize(sMediaProviderID,"\t",pos);

			// We only care about capture cards
			Row_Device *pRow_Device_External = NULL;
			Row_Device_DeviceData *pRow_Device_DeviceData = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_FK_Device_Capture_Card_Port_CONST);
			if( pRow_Device_DeviceData )
			{
				pRow_Device_External=pRow_Device;
				pRow_Device = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
				if( !pRow_Device )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find capture card for %d", pRow_Device_Source->PK_Device_get());
					continue;
				}
			}
			Row_Device *pRow_Device_CaptureCard = pRow_Device;
			if( !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
			{
				pRow_Device_CaptureCard = pRow_Device_CaptureCard->FK_Device_ControlledVia_getrow();
				if( !pRow_Device_CaptureCard || !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders not a capture card device %s provider %s",row[0],row[1]);
#endif
					continue;
				}
			}

			bool bTunersAsSeparateDevices = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Children_as_Separate_Tuners_CONST)=="1";

			// We need to add configure scripts for each model of card, or the sql statement to insert for each card. 

			// We have a capture card.  See if it's in the database already.  We use DEVICEDATA_Port_CONST for the port
			int cardid = atoi(DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST).c_str());
			if( cardid && (pRow_Device->Disabled_get()==1 || pRow_Device_CaptureCard->Disabled_get()==1) )
			{
				sSQL = "DELETE FROM `capturecard` where cardid=" + StringUtils::itos(cardid);
				m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
				DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST,"");
				continue;
			}

			string sPortName = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Name_CONST);
			string sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
			if( sBlockDevice.empty() )
				sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Block_Device_CONST);

			string sScanningScript = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Scanning_Script_CONST);
			if( sScanningScript.empty() )
				sScanningScript = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Scanning_Script_CONST);

			bool bNewCard=false;
				
			string sPortNumber = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_PortChannel_Number_CONST);
			if( sPortName.empty() )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders no port name for device %s provider %s",row[0],row[1]);
				continue;
			}
			if( !cardid )
			{
				bNewCard=true;
				bModifiedRows=true;
				string sHostname = "dcerouter";
				int PK_Device_PC = DatabaseUtils::GetTopMostDevice(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
				Row_Device *pRow_Device_PC = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device_PC);
				if( pRow_Device_PC )
				{
					Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_PC->FK_DeviceTemplate_getrow();
					if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Core_CONST )
						sHostname = "moon" + StringUtils::itos(pRow_Device_PC->PK_Device_get());
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards topmost for %d is %d host %s",
						pRow_Device->PK_Device_get(),pRow_Device_PC->PK_Device_get(),sHostname.c_str());
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find topmost device for %d", pRow_Device->PK_Device_get());

				sSQL = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Configuration_CONST);
				if( sSQL.empty() )
					sSQL = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->FK_Device_ControlledVia_get(),DEVICEDATA_Configuration_CONST);

				if( sSQL.empty() )
					sSQL = "INSERT INTO `capturecard`(cardtype,hostname,defaultinput) VALUES ('MPEG','" + sHostname + "','" + sPortName + "');";
				else
				{
					StringUtils::Replace(&sSQL,"<%=HOST%>",sHostname);
					StringUtils::Replace(&sSQL,"<%=PORT%>",sPortName);
					StringUtils::Replace(&sSQL,"<%=PORTNUM%>",sPortNumber);
					string sIPAddress = pRow_Device_CaptureCard->IPaddress_get();
					StringUtils::Replace(&sSQL,"<%=IP%>",sIPAddress);
				}

				cardid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
				DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST,StringUtils::itos(cardid));
			}

			if( sBlockDevice.empty()==false )
			{
				sSQL = "UPDATE `capturecard` set videodevice='" + sBlockDevice + "' WHERE cardid=" + StringUtils::itos(cardid);
				if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
					bModifiedRows=true;
			}

			if( (bNewCard || pRow_Device_CaptureCard->NeedConfigure_get()==1 || pRow_Device->NeedConfigure_get()==1) && sScanningScript.empty()==false )
			{
				StartScanningScript(pRow_Device,pRow_Device_CaptureCard,iPK_Orbiter,sScanningScript);
				continue;
			}

			int sourceid=0;
			if( pRow_MediaProvider!=NULL )
			{
				Row_ProviderSource *pRow_ProviderSource = pRow_MediaProvider->FK_ProviderSource_getrow();
				string sGrabber;
				if( pRow_ProviderSource )
					sGrabber = pRow_ProviderSource->FillCommandLine_get();

				// See if the provider is in the database already
				string sProviderName = "Provider " + StringUtils::itos(pRow_MediaProvider->PK_MediaProvider_get());
				sSQL = "SELECT sourceid FROM `videosource` WHERE name='" + sProviderName + "'";
				PlutoSqlResult result_set2;
				if( (result_set2.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row2=mysql_fetch_row(result_set2.r)) && atoi(row2[0]) )
					sourceid = atoi(row2[0]);
				else
				{
					// It's possible this was already added as an 'unknown' source.  If so convert that record instead
					sSQL = "SELECT cardinput.sourceid FROM cardinput JOIN videosource ON cardinput.sourceid=videosource.sourceid "
						"WHERE cardid=" + StringUtils::itos(cardid) + " AND inputname = '" + sPortName + "' AND videosource.name like 'UNKNOWN%'";

					PlutoSqlResult result_set2;
					if( (result_set2.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row2=mysql_fetch_row(result_set2.r)) && atoi(row2[0]) )
						sourceid = atoi(row2[0]);
					else
					{
						bModifiedRows=true;
						// The data direct shouldn't be hardcoded
						sSQL = "INSERT INTO `videosource`(name) VALUES ('" + sProviderName + "')";
						sourceid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
					}
				}

				sSQL = "UPDATE `videosource` SET name='" + sProviderName + "',xmltvgrabber='" + sGrabber + "',userid='" + sUsername + "', password='" + sPassword + "', lineupid='" + sLineup + "' WHERE sourceid=" + StringUtils::itos(sourceid);
				if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
					bModifiedRows=true;

				m_mapDevicesToSources[pRow_Device_Source->PK_Device_get()].push_back(sourceid);
				if( bUseInMyth )
					m_mapDevicesToSources[0].push_back(sourceid);

				int cardinputid=0;
				sSQL = "SELECT cardinputid FROM `cardinput` WHERE cardid='" + StringUtils::itos(cardid) + "' AND sourceid='" + StringUtils::itos(sourceid) + "' AND inputname='" + sPortName + "'";
				PlutoSqlResult result_set3;
				if( (result_set3.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row2=mysql_fetch_row(result_set3.r)) && atoi(row2[0]) )
					cardinputid = atoi(row2[0]);

				if( cardinputid && !bUseInMyth )
					m_pMySqlHelper_Myth->threaded_mysql_query("DELETE FROM `cardinput` WHERE cardinputid=" + StringUtils::itos(cardinputid));
				else if( !cardinputid && bUseInMyth )
				{
					bModifiedRows=true;
					sSQL = "INSERT INTO `cardinput`(cardid,sourceid,inputname) VALUES (" + StringUtils::itos(cardid) + "," + StringUtils::itos(sourceid) + ",'" + sPortName + "')";
					cardinputid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
				}

				if( pRow_Device_External )
				{
					sSQL = "UPDATE cardinput SET externalcommand='/usr/pluto/bin/TuneToChannel.sh " + StringUtils::itos(pRow_Device_External->PK_Device_get()) + "' WHERE cardinputid=" + StringUtils::itos(cardinputid);
					if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
						bModifiedRows=true;
				}
			}
		}
	}

	// Delete any stray rows for cards that no longer exist
	sSQL = "DELETE cardinput FROM cardinput LEFT JOIN capturecard ON capturecard.cardid=cardinput.cardid WHERE capturecard.cardid IS NULL";
	if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
		bModifiedRows=true;
	sSQL = "DELETE videosource FROM videosource LEFT JOIN cardinput on videosource.sourceid=cardinput.sourceid WHERE cardinput.sourceid IS NULL";
	if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
		bModifiedRows=true;

	if( m_mapPendingScans.empty()==false )
		return;

	// Find cards with invalid starting channels
	sSQL = "SELECT cardinputid FROM cardinput LEFT JOIN channel on startchan=channum WHERE channum IS NULL";
	PlutoSqlResult result_set2;
	if( (result_set2.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && result_set2.r->row_count>0 )
	{
		bModifiedRows=true;
		string sChanNum="1";

		sSQL = "SELECT min(channum) FROM channel";
		PlutoSqlResult result_set3;
		if( (result_set3.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set3.r)) && row[0] )
			sChanNum = row[0];

		while ((row = mysql_fetch_row(result_set2.r)))
		{
			sSQL = "UPDATE cardinput SET startchan='" + sChanNum + "' WHERE cardinputid='" + row[0] + "'";
			m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- bModifiedRows %d m_bFillDbRunning %d m_bNeedToRunFillDb %d",
		(int) bModifiedRows,(int) m_bFillDbRunning,(int) m_bNeedToRunFillDb);
	if( bModifiedRows )  // We've changed stuff.  Need to run the fill process
	{
		if( m_bFillDbRunning )
			m_bNeedToRunFillDb = true;  // A fill is already running.  Need to re-run it when we're done
		else
		{
			m_bFillDbRunning=true;
			StartFillDatabase();
		}
	}
}

void MythTV_PlugIn::UpdateMythSetting(string value,string data,string hostname,bool bOnlyIfNotExisting)
{
	if( hostname=="*" )
	{
		// For some reason mysql returns an error: Error Code : 1030, Got error 28 from table handler
		// When you run SELECT DISTINCT hostname FROM settings
		// So we'll keep a map so we can skip ones we've already done
		map<string,bool> mapExistingHosts;

		string sSQL = "SELECT hostname FROM settings";
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( (result.r = m_pMySqlHelper_Myth->mysql_query_result(sSQL)) )
		{
			while( ( row=mysql_fetch_row( result.r ) ) )
			{
				if( row[0] )
				{
					if( mapExistingHosts[row[0]] )
						continue;
					mapExistingHosts[row[0]]=true;
					UpdateMythSetting(value,data,row[0],bOnlyIfNotExisting);
				}
			}
		}
		return;
	}

	string sSQL = "SELECT value FROM settings WHERE value='" + StringUtils::SQLEscape(value) + "' AND " 
		" hostname " + (hostname.empty() ? "IS NULL" : "='" + StringUtils::SQLEscape(hostname) + "'");
	PlutoSqlResult result;
	if( (result.r = m_pMySqlHelper_Myth->mysql_query_result(sSQL))==NULL || result.r->row_count==0 )
	{
		sSQL = "INSERT INTO settings(value,hostname) VALUES('" + StringUtils::SQLEscape(value) + "',"
			+ (hostname.empty() ? "NULL" : "'" + StringUtils::SQLEscape(hostname) + "'") + ")";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
	}
	else if( bOnlyIfNotExisting )
		return;

	sSQL = "UPDATE settings set data='" + StringUtils::SQLEscape(data) + "' WHERE value='" + StringUtils::SQLEscape(value) + "' "
		" AND hostname " + (hostname.empty() ? "IS NULL" : "='" + StringUtils::SQLEscape(hostname) + "'");

	m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
}

void MythTV_PlugIn::CheckForNewRecordings()
{
	/* I haven't been able to find a clean way to get myth back end to notify us
	when it has finished making new recordings.  So we'll do it the brute force way
	and search for new files that have 'tv_shows' in the path but don't yet have any
	attributes */

	string sSQL = "SELECT max(PK_File) FROM File";
	PlutoSqlResult result;
    MYSQL_ROW row;
    
	int PK_File_Max=0;
	if( ( result.r=m_pMedia_Plugin->m_pDatabase_pluto_media->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
		PK_File_Max = atoi(row[0]);

	sSQL = "LEFT JOIN File_Attribute ON FK_File=PK_File WHERE Path like '%tv_shows_%' AND FK_File IS NULL AND PK_File>" + StringUtils::itos(m_dwPK_File_LastCheckedForNewRecordings);
	vector<Row_File *> vectRow_File;
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->GetRows( sSQL, &vectRow_File );

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CheckForNewRecordings PK_File_Max %d m_dwPK_File_LastCheckedForNewRecordings %d files: %d",
		PK_File_Max, m_dwPK_File_LastCheckedForNewRecordings, (int) vectRow_File.size());

	for(vector<Row_File *>::iterator it=vectRow_File.begin();it!=vectRow_File.end();++it)
	{
		Row_File *pRow_File = *it;
		// Find the file in mythconverg.recorded and import the attributes

		sSQL = 
			"SELECT recorded.chanid,recorded.starttime,recorded.title,recorded.subtitle,recorded.stars,recorded.category,recorded.description,"
			"recordedprogram.hdtv,recordedprogram.category_type,channel.name,recordedrating.rating FROM recorded "
			"LEFT JOIN recordedprogram ON recorded.chanid=recordedprogram.chanid and recorded.starttime=recordedprogram.starttime "
			"LEFT JOIN channel ON recorded.chanid=channel.chanid "
			"LEFT JOIN recordedrating ON recorded.chanid=recordedrating.chanid and recorded.starttime=recordedrating.starttime "
			"WHERE basename='" + pRow_File->Filename_get() + "'";

		PlutoSqlResult result;
		if( (result.r=m_pMySqlHelper_Myth->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		{
			if( row[2] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[2],0,"",ATTRIBUTETYPE_Title_CONST,"",pRow_File->PK_File_get());
			if( row[3] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[3],0,"",ATTRIBUTETYPE_Episode_CONST,"",pRow_File->PK_File_get());
			if( row[4] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[4],0,"",ATTRIBUTETYPE_Rating_CONST,"",pRow_File->PK_File_get());
			if( row[5] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[5],0,"",ATTRIBUTETYPE_Genre_CONST,"",pRow_File->PK_File_get());
			if( row[6] )
			{
				Row_LongAttribute *pRow_LongAttribute = m_pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->AddRow();
				pRow_LongAttribute->FK_File_set( pRow_File->PK_File_get() );
				pRow_LongAttribute->FK_AttributeType_set( ATTRIBUTETYPE_Synopsis_CONST );
				pRow_LongAttribute->Text_set( row[6] );
				m_pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->Commit();
			}
			if( row[7] && atoi(row[7]) )
				pRow_File->FK_FileFormat_set(FILEFORMAT_HD_1080_CONST);
			else
				pRow_File->FK_FileFormat_set(FILEFORMAT_Standard_Def_CONST);
			if( row[8] )
			{
				string sType = row[8];
				if( sType=="tvshow" || sType=="series" )
					pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_TV_Shows_CONST);
				else if( sType=="movie" )
					pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_Movies_CONST);
				else if( sType=="sports" )
					pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_Sports_Events_CONST);
			}
			if( row[9] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[9],0,"",ATTRIBUTETYPE_Channel_CONST,"",pRow_File->PK_File_get());
			if( row[10] )
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[10],0,"",ATTRIBUTETYPE_Rated_CONST,"",pRow_File->PK_File_get());

			sSQL = 
				"SELECT role,name FROM recordedcredits JOIN people on recordedcredits.person=people.person "
				"WHERE chanid='" + string(row[0]) + "' AND starttime='" + row[1] + "'";

			PlutoSqlResult result_person;
			if( (result_person.r=m_pMySqlHelper_Myth->mysql_query_result( sSQL ) ) )
			{
				while( row=mysql_fetch_row( result_person.r ) )
				{
					string sRole = row[0];
					if( sRole=="director" )
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Director_CONST,"",pRow_File->PK_File_get());
					else if( sRole=="actor" )
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Performer_CONST,"",pRow_File->PK_File_get());
					else if( sRole=="producer" )
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Producer_CONST,"",pRow_File->PK_File_get());
					else if( sRole=="executive_producer" )
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Executive_Producer_CONST,"",pRow_File->PK_File_get());
					else if( sRole=="writer" )
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_ComposerWriter_CONST,"",pRow_File->PK_File_get());

				}
			}
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CheckForNewRecordings couldn't locate file %s", pRow_File->Filename_get().c_str());
	}
	
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->Commit();
	m_pAlarmManager->AddRelativeAlarm(3600,this,CHECK_FOR_NEW_RECORDINGS,NULL);  /* check again in an hour */

	m_dwPK_File_LastCheckedForNewRecordings = PK_File_Max;
}

void MythTV_PlugIn::AlarmCallback(int id, void* param)
{
	if( id==CHECK_FOR_NEW_RECORDINGS )
		CheckForNewRecordings();
	else if( id==SYNC_PROVIDERS )
	{
		ScanJob *pScanJob = (ScanJob *) param;
		StartScanJob(pScanJob);
	}
	else if( id==RUN_BACKEND_STARTER )
		RunBackendStarter();
}

bool MythTV_PlugIn::NewBookmarks( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	m_bBookmarksNeedRefreshing=true;
	return false;
}

void MythTV_PlugIn::RefreshBookmarks()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	m_bBookmarksNeedRefreshing=false;
	map<int,string> *p_m_mapBookmark=NULL;
	m_mapChannelBookmarks.clear();
	m_mapSeriesBookmarks.clear();
	m_mapProgramBookmarks.clear();
	string sSQL = "SELECT EK_Users,Position FROM Bookmark WHERE EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r = m_pMedia_Plugin->m_pDatabase_pluto_media->mysql_query_result(sSQL)) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			char *pPos = NULL;
			if( !row[1] )
				continue;
			if( (pPos=strstr(row[1]," PROG:"))!=NULL )
			{
				p_m_mapBookmark = &m_mapProgramBookmarks;
				pPos += 6;
			}
			else if( (pPos=strstr(row[1]," CHAN:"))!=NULL )
			{
				p_m_mapBookmark = &m_mapChannelBookmarks;
				pPos += 6;
			}
			else if( (pPos=strstr(row[1]," SERIES:"))!=NULL )
			{
				p_m_mapBookmark = &m_mapSeriesBookmarks;
				pPos += 8;
			}

			if( pPos && p_m_mapBookmark )
			{
				string sCurrent = (*p_m_mapBookmark)[ row[0] ? atoi(row[0]) : 0];
				if( sCurrent.empty()==false )
					sCurrent += ",";

				if( p_m_mapBookmark == &m_mapChannelBookmarks) // Channels are int's.  Don't quote
					sCurrent += pPos;
				else
					sCurrent += string("'") + pPos + "'";
				(*p_m_mapBookmark)[ row[0] ? atoi(row[0]) : 0] = sCurrent;
			}
		}
	}
	BuildChannelList();
}

void MythTV_PlugIn::PurgeChannelList()
{
	m_map_listMythChannel.clear();
	for(map<int,MythChannel *>::iterator it=m_mapMythChannel.begin();it!=m_mapMythChannel.end();++it)
		delete it->second;
	m_mapMythChannel.clear();
}

void MythTV_PlugIn::BuildChannelList()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	PurgeChannelList();

	string sSQL = "SELECT channel.chanid, channum, callsign, name, icon, pic.EK_Picture, sourceid "
		"FROM channel "
		"LEFT JOIN `pluto_myth`.`Picture` AS pic on channel.chanid=pic.chanid";
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))!=NULL )
	{
		while((row = mysql_fetch_row(result.r)))
		{
			string sFilename;
			if( row[5] )
				sFilename = "/home/mediapics/" + string(row[5]) + "_tn.jpg";
			else if( row[4] )
				sFilename = row[4];

			size_t size=0;
			char *pData = sFilename.empty() ? NULL : FileUtils::ReadFileIntoBuffer(sFilename,size);

			MythChannel *pMythChannel = new MythChannel( atoi(row[0]), atoi(row[1]), atoi(row[6]), row[2] ? row[2] : "", row[3] ? row[3] : "", pData, size );
			m_mapMythChannel[pMythChannel->m_dwID] = pMythChannel;
		}
	}

	vector<Row_Users *> vectRow_Users;
	m_pMedia_Plugin->m_pDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users);

	for(vector<Row_Users *>::iterator it=vectRow_Users.begin();it!=vectRow_Users.end();++it)
	{
		Row_Users *pRow_Users = *it;
		ListMythChannel *pListMythChannel = &(m_map_listMythChannel[pRow_Users->PK_Users_get()]);

		string sBookmarks = m_mapChannelBookmarks[0];
		string sBookmarks2 = m_mapChannelBookmarks[pRow_Users->PK_Users_get()];
		if( sBookmarks.empty()==false && sBookmarks2.empty()==false )
			sBookmarks += ",";
		sBookmarks += sBookmarks2;

		for(map<int,MythChannel *>::iterator it=m_mapMythChannel.begin();it!=m_mapMythChannel.end();++it)
			pListMythChannel->push_back( it->second );
		pListMythChannel->sort(ChannelComparer);

		ListMythChannel listMythChannel_Favs;
		string::size_type pos=0;
		while(pos<sBookmarks.size())
		{
			int Channel = atoi( StringUtils::Tokenize( sBookmarks, ",", pos ).c_str() );
			if( m_mapMythChannel.find(Channel)!=m_mapMythChannel.end() )
			{
				MythChannel *pMythChannel = m_mapMythChannel[ Channel ];
				if( pMythChannel )
					listMythChannel_Favs.push_back( pMythChannel );
			}
		}
		m_mapUserFavoriteChannels[pRow_Users->PK_Users_get()]=(int) listMythChannel_Favs.size();
		listMythChannel_Favs.sort(ChannelComparer);

		for(ListMythChannel::reverse_iterator itri=listMythChannel_Favs.rbegin();itri!=listMythChannel_Favs.rend();++itri)
			pListMythChannel->push_front(*itri);
	}
}

class DataGridTable *MythTV_PlugIn::FavoriteChannels( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	string sPK_Users = Parms.empty() ? " AND EK_Users IS NULL " : " AND (EK_Users IS NULL OR EK_Users=" + Parms + ")";
	string sSQL = "EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST) + sPK_Users + " AND Position like '%CHAN:%' ORDER BY Description";
	vector<Row_Bookmark *> vectRow_Bookmark;
	 m_pMedia_Plugin->m_pDatabase_pluto_media->Bookmark_get()->GetRows(sSQL,&vectRow_Bookmark);

	int iRow=0;
	DataGridTable *pDataGrid = new DataGridTable();
	for(vector<Row_Bookmark *>::iterator it=vectRow_Bookmark.begin();it!=vectRow_Bookmark.end();++it)
	{
		Row_Bookmark *pRow_Bookmark = *it;
		DataGridCell *pCell = new DataGridCell( pRow_Bookmark->Description_get(), StringUtils::itos( pRow_Bookmark->PK_Bookmark_get() ) );
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *MythTV_PlugIn::FavoriteShows( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	string sPK_Users = Parms.empty() ? " AND EK_Users IS NULL " : " AND (EK_Users IS NULL OR EK_Users=" + Parms + ")";
	string sSQL = "EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST) + sPK_Users + " AND (Position like '%PROG:%' OR Position like '%SERIES:%') ORDER BY Description";
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pMedia_Plugin->m_pDatabase_pluto_media->Bookmark_get()->GetRows(sSQL,&vectRow_Bookmark);

	int iRow=0;
	DataGridTable *pDataGrid = new DataGridTable();
	for(vector<Row_Bookmark *>::iterator it=vectRow_Bookmark.begin();it!=vectRow_Bookmark.end();++it)
	{
		Row_Bookmark *pRow_Bookmark = *it;
		DataGridCell *pCell = new DataGridCell( pRow_Bookmark->Description_get(), StringUtils::itos( pRow_Bookmark->PK_Bookmark_get() ) );
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *MythTV_PlugIn::ThumbnailableAttributes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

	MythTvMediaStream *pMythTvMediaStream = NULL;
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || 
		(pMythTvMediaStream = ConvertToMythMediaStream(pEntertainArea->m_pMediaStream, "MythTV_PlugIn::ThumbnailableAttributes() ")) == NULL )
		return pDataGrid;

pMythTvMediaStream->m_iCurrentProgramChannelID=1003; // temp hack until i get this working
	MythChannel *pMythChannel = m_mapMythChannel_Find( pMythTvMediaStream->m_iCurrentProgramChannelID );
	if( !pMythChannel )
		return pDataGrid;

	int iRow=0;
	DataGridCell *pCell = new DataGridCell( "Channel: " + StringUtils::itos(pMythChannel->m_dwChanNum) + " " + pMythChannel->m_sShortName, 
		"CHAN:" + StringUtils::itos( pMythChannel->m_dwID ) );
	pDataGrid->SetData(0,iRow++,pCell);

	// Try to find what series/program is on now
	string sSQL = "SELECT seriesid, programid, title "
		"FROM program "
		"WHERE starttime < '" + StringUtils::SQLDateTime() + "' AND endtime>'" + StringUtils::SQLDateTime() + "' " 
		" AND chanid=" + StringUtils::itos(pMythTvMediaStream->m_iCurrentProgramChannelID);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))!=NULL && (row = mysql_fetch_row(result.r)) )
	{
		if( row[0] && row[0][0] )
		{
			pCell = new DataGridCell( row[2], "SERIES:" + string(row[0]) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
		else if( row[1] && row[1][0] )
		{
			pCell = new DataGridCell( row[2], "PROG:" + string(row[1]) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

//<-dceag-c846-b->

	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

void MythTV_PlugIn::CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c846-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	if( !pData || iData_Size==0 || sFilename.size()<3 )
		return;

	Row_Picture *pRow_Picture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(pData,iData_Size,"JPG","");
	if( !pRow_Picture )
		return;

	string sWhere,sInsert;
	string::size_type pos;
	if( (pos=sFilename.find("CHAN:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE chanid=" + sFilename.substr(pos+5);
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,chanid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "," + sFilename.substr(pos+5) + ")";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
		BuildChannelList();
	}
	else if( (pos=sFilename.find("SERIES:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE seriesid='" + sFilename.substr(pos+7) + "'";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,seriesid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + ",'" + sFilename.substr(pos+7) + "')";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
	}
	else if( (pos=sFilename.find("PROG:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE programid='" + sFilename.substr(pos+5) + "'";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,programid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + ",'" + sFilename.substr(pos+5) + "')";
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
	}
}

void MythTV_PlugIn::StartScanningScript(Row_Device *pRow_Device_Tuner,Row_Device *pRow_Device_CaptureCard,int iPK_Orbiter,string sScanningScript)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	map< int, ScanJob * >::iterator it=m_mapPendingScans.find( pRow_Device_Tuner->PK_Device_get() );
	if( it!=m_mapPendingScans.end() )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanningScript %d/%d already scanning", pRow_Device_Tuner->PK_Device_get(), pRow_Device_CaptureCard->PK_Device_get() );
		return;
	}

	ScanJob *pScanJob = new ScanJob( pRow_Device_CaptureCard, pRow_Device_Tuner, iPK_Orbiter, sScanningScript );
	m_mapPendingScans[ pScanJob->m_pRow_Device_Tuner->PK_Device_get() ] = pScanJob;
	StartScanJob(pScanJob);
}

void MythTV_PlugIn::StartScanJob(ScanJob *pScanJob)
{
	if( pScanJob->m_bActive )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanningScript %d/%d active", pScanJob->m_pRow_Device_Tuner->PK_Device_get(),pScanJob->m_pRow_Device_CaptureCard->PK_Device_get());
		return;
	}

	// If we're still busy downloading packages we don't want to do this yet since the files may not be installed yet
	if( m_pGeneral_Info_Plugin->ReportPendingTasks() )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanningScript %d/%d waiting for packages", pScanJob->m_pRow_Device_Tuner->PK_Device_get(),pScanJob->m_pRow_Device_CaptureCard->PK_Device_get() );
		m_pAlarmManager->AddRelativeAlarm(30,this,SYNC_PROVIDERS,(void *) pScanJob);  /* check again in 30 seconds */
		return;
	}

	// Before we start be sure the block device is in the database
	bool bTunersAsSeparateDevices = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Children_as_Separate_Tuners_CONST)=="1";
	int cardid = atoi(DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pScanJob->m_pRow_Device_Tuner->PK_Device_get() : pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST).c_str());
	string sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_Tuner->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
	if( sBlockDevice.empty() )
		sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
	if( sBlockDevice.empty()==false )
	{
		string sSQL = "UPDATE `capturecard` set videodevice='" + sBlockDevice + "' WHERE cardid=" + StringUtils::itos(cardid);
		m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
	}

	DeviceData_Base *pDevice_App_Server = (DeviceData_Router *) m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
	if( pDevice_App_Server )
	{
		string sScanFailed = "0 -1000 2 " TOSTRING(EVENT_Channel_Scan_Progress_CONST) " " 
			TOSTRING(EVENTPARAMETER_PK_Device_CONST) " " + StringUtils::itos( pScanJob->m_pRow_Device_Tuner->PK_Device_get() ) + " "
			TOSTRING(EVENTPARAMETER_Result_CONST) " -1 "
			TOSTRING(EVENTPARAMETER_Text_CONST) " \"Failed to start\" ";

		string sArguments = StringUtils::itos(pScanJob->m_pRow_Device_Tuner->PK_Device_get()) + "\t" + StringUtils::itos(pScanJob->m_pRow_Device_CaptureCard->PK_Device_get());
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/pluto/bin/" + pScanJob->m_sScanJob,"scanchannels",sArguments,sScanFailed,"",false,false,false,true);
		string sResponse;
		if( !SendCommand(CMD_Spawn_Application,&sResponse) || sResponse!="OK" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartScanningScript -- app server didn't respond");
			m_pAlarmManager->AddRelativeAlarm(30,this,SYNC_PROVIDERS,(void *) pScanJob);  /* check again in 30 seconds */
			pScanJob->m_pRow_Device_CaptureCard->NeedConfigure_set(1);
			pScanJob->m_pRow_Device_Tuner->NeedConfigure_set(1);
		}
		else
		{
			pScanJob->m_bActive=true;
			pScanJob->m_pRow_Device_CaptureCard->NeedConfigure_set(0);
			pScanJob->m_pRow_Device_Tuner->NeedConfigure_set(0);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartScanningScript -- no app server");
		pScanJob->m_pRow_Device_CaptureCard->NeedConfigure_set(1);
		pScanJob->m_pRow_Device_Tuner->NeedConfigure_set(1);
		m_mapPendingScans.erase( pScanJob->m_pRow_Device_Tuner->PK_Device_get() );
		delete pScanJob;
	}
	m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->Commit();
}

bool MythTV_PlugIn::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "checking scanning jobs %d %d",m_dwPK_Device, (int) m_mapPendingScans.size());

	if( m_mapPendingScans.size() )
	{
		if( pPendingTaskList )
		{
			for( map< int, ScanJob * >::iterator it=m_mapPendingScans.begin();it!=m_mapPendingScans.end();++it )
			{
				ScanJob *pScanJob = it->second;
				string sDesc = "Scanning for channels: " + pScanJob->m_pRow_Device_CaptureCard->Description_get() + "/" + pScanJob->m_pRow_Device_Tuner->Description_get() + " ";
				if( pScanJob->m_bActive )
					sDesc += StringUtils::itos(pScanJob->m_iPercentCompletion) + "% ";
				else
					sDesc += "waiting...";

				pPendingTaskList->m_listPendingTask.push_back(new PendingTask(pScanJob->m_pRow_Device_Tuner->PK_Device_get(),
					m_dwPK_Device,m_dwPK_Device,
					"channel_scan","Scanning Channels: " + sDesc + " " + pScanJob->m_sStatus,
					-1,0,true));
			}
		}
		return true;
	}
	return false;
}

bool MythTV_PlugIn::ScanningProgress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	int		iPK_Device = atoi( pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str( ) );
	int		iResult  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str( ) );
	int		iValue  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str( ) );
	string      sText = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	map< int, ScanJob * >::iterator it=m_mapPendingScans.find(iPK_Device);
	if( it==m_mapPendingScans.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ScanningProgress cannot find job for %d", iPK_Device);
		return false;
	}

	ScanJob *pScanJob = it->second;
	if( iResult!=0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::ScanningProgress done for job %d", iPK_Device);

		// See if we've found a tv format, or can otherwise determine the provider
		CheckForTvFormatAndProvider( iPK_Device );

		DCE::CMD_Sync_Providers_and_Cards CMD_Sync_Providers_and_Cards(m_dwPK_Device,m_dwPK_Device,pScanJob->m_iPK_Orbiter);
		QueueMessageToRouter(CMD_Sync_Providers_and_Cards.m_pMessage);  // Do this asynchronously
		m_mapPendingScans.erase(it);
		delete pScanJob;
		return false;
	}

	pScanJob->m_sStatus = sText;
	pScanJob->m_iPercentCompletion = iValue;

	return false;
}

void MythTV_PlugIn::CheckForTvFormatAndProvider( int iPK_Device )
{
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(iPK_Device,DEVICEDATA_Port_CONST);
	if( !pRow_Device_DeviceData || pRow_Device_DeviceData->IK_DeviceData_get().empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CheckForTvFormatAndProvider cannot determine anything without a port");
		return;
	}

	string sSQL = "SELECT DISTINCT modulation,tvformat FROM cardinput "
		"JOIN channel ON cardinput.sourceid=channel.sourceid "
		"JOIN dtv_multiplex ON dtv_multiplex.mplexid=channel.mplexid "
		"WHERE cardinput.cardid=" + pRow_Device_DeviceData->IK_DeviceData_get();

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r = m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && ( row=mysql_fetch_row( result.r ) ) && row[0] && row[1] )
	{
		if( result.r->row_count>1 )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CheckForTvFormatAndProvider found more than 1 tv format for %d",iPK_Device);
		DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_Type_CONST,row[0]);
/*
		// TEMP -- remove this before .44
		if( strcmp(row[0],"8vsb")==0 && strcmp(row[1],"ATSC")==0 )
			m_pMedia_Plugin->CMD_Specify_Media_Provider(iPK_Device,"paulhuber2005\tmsvirus1\t46\t2\tPC:94066");
		else if( strcmp(row[0],"qam_256")==0 )
			m_pMedia_Plugin->CMD_Specify_Media_Provider(iPK_Device,"paulhuber2005\tmsvirus1\t46\t2\tCA04684:X");
*/
	}
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void MythTV_PlugIn::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
}

void MythTV_PlugIn::SetPaths()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SetPaths");

	// Get all the backend ip's so we can match the hostname myth uses to our own device id
	map<string,int> mapIpToDevice;
	vector<Row_Device *> vectRow_Device;
	// Get the top level devices
	m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRows("FK_Device_ControlledVia IS NULL",&vectRow_Device);
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
		mapIpToDevice[ (*it)->IPaddress_get() ] = (*it)->PK_Device_get();

	int PK_Device_Storage = atoi(DATA_Get_PK_Device().c_str());
	string sFilename = PK_Device_Storage ? "/mnt/device/" + StringUtils::itos(PK_Device_Storage) + "/public/data/videos/tv_shows_" : "/home/public/data/videos/tv_shows_";

	string sSQL = "SELECT data,hostname from settings where value='BackendServerIP'";
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r = m_pMySqlHelper_Myth->mysql_query_result(sSQL)) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			if( !row[1] || !row[1][0] )
				continue;
			int PK_Device = row[0] ? mapIpToDevice[row[0]] : 0;
			string sDirectory = sFilename + StringUtils::itos(PK_Device);

			string sCmd = "mkdir -p \"" + sDirectory + "\"";
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::Register %s",sCmd.c_str());
			system(sCmd.c_str());

			sCmd = "chmod 775 \"" + sDirectory +"\"";
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_Plugin::Register %s",sCmd.c_str());
			system(sCmd.c_str());

			sCmd = "chown mythtv \"" + sDirectory +"\"";
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_Plugin::Register %s",sCmd.c_str());
			system(sCmd.c_str());

			UpdateMythSetting("RecordFilePrefix",sDirectory,row[1]);
		}
	}
}

void MythTV_PlugIn::RunBackendStarter()
{
	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/bin/screen","restart_myth","-d\t-m\t-S\tRestart_Myth_Backend\t/usr/pluto/bin/Restart_MythBackend.sh","","",false,false,false,false);
		if( SendCommand(CMD_Spawn_Application) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::RunBackendStarter ok");
			return; // We're ok
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::RunBackendStarter try again");
	m_pAlarmManager->AddRelativeAlarm(5,this,RUN_BACKEND_STARTER,NULL);
}

void MythTV_PlugIn::StartFillDatabase()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	DeviceData_Router *pDevice_App_Server=NULL,*pDevice_Us = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
	if( pDevice_Us )
		pDevice_App_Server = (DeviceData_Router *) pDevice_Us->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);

	if( pDevice_App_Server )
	{
		string sResponse = "fill done";
		DCE::CMD_Spawn_Application CMD_Spawn_Application_fill(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/pluto/bin/FillDbAndFetchIcons.sh","filldb","","","",false,false,true,false);
		if( !SendCommand(CMD_Spawn_Application_fill) )
		{
			m_bFillDbRunning=false;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartFillDatabase failed");
		}

		DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
			"It will take about 10 minutes to retrieve your channel listing.  Wait until you see the message \"MythTV is ready\" before you start using TV features.", // Main message
			"", // Command Line
			"generic message", // Description
			"0", // sPromptToResetRouter
			"300", // sTimeout
			"1"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage);
	}
}

//<-dceag-c883-b->

	/** @brief COMMAND: #883 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

void MythTV_PlugIn::CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage)
//<-dceag-c883-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Report_EPG Success %d m_bNeedToRunFillDb %d m_bFillDbRunning %d",
		bIsSuccessful,(int) m_bNeedToRunFillDb,(int) m_bFillDbRunning);
	if( m_bNeedToRunFillDb )
	{
		m_bFillDbRunning=true;
		StartFillDatabase();
	}
	else
	{
		m_bFillDbRunning=false;
	}
}
