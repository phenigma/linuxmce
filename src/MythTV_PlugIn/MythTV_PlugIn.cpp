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
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_media/Table_MediaProvider.h"
#include "../Gen_Devices/AllScreens.h"

#include "DataGrid.h"
#include "EPGGrid.h"
#include "MythTvMediaStream.h"
#ifndef WIN32
//#include "MythTvWrapper.h"
//#include <libmythtv/frame.h>
#endif

#define MINIMUM_MYTH_SCHEMA		1123

#include "../Orbiter_Plugin/OH_Orbiter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pEPGGrid = NULL;
//	m_bPreProcessSpeedControl=false;  // We do some ridiculous hacks in Myth player to convert speed control commands to keystrokes
//	m_pMythWrapper = NULL;
	m_pMySqlHelper_Myth = NULL;
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
	if( !m_pDatagrid_Plugin || !m_pMedia_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

    m_pMedia_Plugin->RegisterMediaPlugin(this, this, DEVICETEMPLATE_MythTV_Player_CONST, true);

/*
	string SQL = "SELECT FK_Device,IK_DeviceData FROM Device_DeviceData JOIN Device ON PK_Device=FK_Device WHERE FK_Installation="
		+ StringUtils::itos(m_pRouter->m_Installation_get()) + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_MythTV_PVR_Input_CONST);

    PlutoSqlResult result;
    MYSQL_ROW row;
	m_pMedia_Plugin->m_pMedia_Plugin->m_pDatabase_pluto_main->threaded_mysql_query(SQL);
    if( ( result.r=m_pMedia_Plugin->m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
			int PK_Device = atoi(row[0]);
			int PK_Device = atoi(row[0]);

		}
	}
*/

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::CurrentShows))
                                                ,DATAGRID_EPG_Current_Shows_CONST,PK_DeviceTemplate_get());

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShows))
                                                ,DATAGRID_EPG_All_Shows_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::TvProviders))
												,DATAGRID_TV_Providers_CONST,PK_DeviceTemplate_get());

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_MythTV_Channel_Changed_CONST );
	// RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Info_Changed_CONST );

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
			g_pPlutoLogger->Write(LV_STATUS,"MythTV_PlugIn::Register %s",sCmd.c_str());
			system(sCmd.c_str());

			sCmd = "chmod 775 \"" + sDirectory +"\"";
			g_pPlutoLogger->Write(LV_STATUS,"MythTV_Plugin::Register %s",sCmd.c_str());
			system(sCmd.c_str());

			UpdateMythSetting("RecordFilePrefix",sDirectory,row[1]);
		}
	}
	
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

	g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Sending command to stop media to the player: %d", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	SendCommand(cmd);
	g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Returning from stop media command to the player: %d last pos %s", 
		pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sLastPosition.c_str());
	pMediaStream->m_sLastPosition=sLastPosition;

	return true;
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

class DataGridTable *MythTV_PlugIn::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign,
	Message *pMessage)
{
    int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	DataGridTable *pDataGridTable = new DataGridTable();
	DataGridCell *pCell;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::AllShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());
    
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
	    g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::AllShows cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}

	string sProvider;

	// TODO -- must fix this.  ID_get is a big string and not an id
//	if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pRow_MediaProvider && 
//		pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pRow_MediaProvider->ID_get().empty()==false )
//			sProvider = " AND sourceid='" + pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pRow_MediaProvider->ID_get() + "'";

	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL = "SELECT c.chanid, c.channum, c.name, c.icon, p.title, p.starttime, p.endtime "
		"FROM program p "
		"INNER JOIN channel c ON c.chanid = p.chanid "
		"WHERE '" + StringUtils::SQLDateTime() + "' BETWEEN p.starttime and p.endtime " + sProvider +
		" ORDER BY cast(c.channum as unsigned)";
	PlutoSqlResult result;
	MYSQL_ROW row;
	int iRow=0;
	if( (result.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))!=NULL )
	{
		while((row = mysql_fetch_row(result.r)))
		{
			string sDesc = string( row[1] ) + " " + row[2] + " " + row[4];
			pCell = new DataGridCell(sDesc,row[1]);

			string sChannelName = string(row[1]) + " " + row[2];
			if( row[3] && row[3][0] )  // There's an icon
				pCell->m_mapAttributes["Icon"] = row[3];

			time_t tStart = StringUtils::SQLDateTime( row[5] );
			time_t tStop = StringUtils::SQLDateTime( row[6] );
			string sTime = StringUtils::HourMinute(tStart) + " - " + StringUtils::HourMinute(tStop);
			string sNumber = NULL != row[0] ? row[0] : "";
			string sInfo = NULL != row[4] ? row[4]: "";

			pCell->m_mapAttributes["Number"] = sNumber;
			pCell->m_mapAttributes["Name"] = sChannelName;
			pCell->m_mapAttributes["Time"] = sTime;
			pCell->m_mapAttributes["Info"] = sInfo;

			//g_pPlutoLogger->Write(LV_WARNING, "Number %s, name %s, time %s, info %s", 
			//	sNumber.c_str(), sChannelName.c_str(), sDesc.c_str(), sInfo.c_str());
			pDataGridTable->SetData(0,iRow++,pCell);
		}
	}
 
	return pDataGridTable;
}

class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	return NULL;
	return m_pEPGGrid;

//	g_pPlutoLogger->Write(LV_STATUS, "Current Shows datagrid called");
	
//s	MythTvMediaStream *pMythTvMediaStream = NULL;

/*	if ( (pMythTvMediaStream = ConvertToMythMediaStream(m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From), "MythTV_PlugIn::CurrentShows() ")) == NULL)
	    return new DataGridTable();

    	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	if( !m_pMySqlHelper_Myth || !m_pMySqlHelper_Myth->m_bConnected )
		return pDataGrid;

	string::size_type pos=0;
	string sPK_PostalCode = StringUtils::Tokenize(Parms,",",pos);
	bool bInternalTuner = StringUtils::Tokenize(Parms,",",pos)=="1";
	string sSQL = "SELECT channel.chanid, channum, callsign, title, description, icon FROM channel LEFT JOIN program on (channel.chanid=program.chanid) WHERE NOW() > program.starttime AND NOW() < program.endtime ORDER BY CAST(channum AS SIGNED)";
	PlutoSqlResult result_set;
	MYSQL_ROW row;
	pCell = new DataGridCell("None","0");
	pDataGrid->SetData(0,0,pCell);
	int iRow=0;
	if( (result_set.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) )
	{
		while ((row = mysql_fetch_row(result_set.r)))
		{
			pCell = new DataGridCell(row[1]+string("\n")+row[2], row[1]);
			size_t fSize;
			char *fLogo;
			fLogo=FileUtils::ReadFileIntoBuffer(row[5], fSize);
			pDataGrid->SetData(0,iRow,pCell);
			pCell = new DataGridCell("");
			if (fLogo)		
				pCell->SetImage(fLogo, fSize, GR_JPG);
	
			pDataGrid->SetData(1,iRow,pCell);
			pCell = new DataGridCell(row[3]);
			pDataGrid->SetData(2,iRow++,pCell);
		}
	}
	
    return pDataGrid;*/
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
			g_pPlutoLogger->Write(LV_WARNING, "MythTV_PlugIn::MediaInfoChanged() called for an event which didn't provided a proper device ID");
			return false;
		}

		if ( (pMythTvStream = ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[playbackDevice],pMessage->m_dwPK_Device_From), "MythTV_PlugIn::MediaInfoChanged() ")) == NULL)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Could not detect a valid MythTV media stream based on the device %d", pDeviceFrom->m_dwPK_Device);
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

    g_pPlutoLogger->Write(LV_STATUS, "Jump to position called %s", sValue_To_Assign.c_str());

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

    g_pPlutoLogger->Write(LV_STATUS, "Jump to position called %s", sProgramID.c_str());

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
//<-dceag-c409-b->

	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current channel or program as a bookmark.  Text should have CHAN: or PROG: in there */
		/** @param #39 Options */
			/** For TV, CHAN: or PROG: indicating if it's the channel or program to bookmark */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */

void MythTV_PlugIn::CMD_Save_Bookmark(string sOptions,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c409-e->
{
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

    g_pPlutoLogger->Write(LV_STATUS, "MythTV_PlugIn::CMD_Set_Active_Menu %s", sText.c_str());

    MythTvMediaStream *pMythTvMediaStream =
		ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[pMessage->m_dwPK_Device_From],pMessage->m_dwPK_Device_From));

    if( !pMythTvMediaStream )
	{
	    g_pPlutoLogger->Write(LV_CRITICAL, "MythTV_PlugIn::CMD_Set_Active_Menu stream is NULL");
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
		g_pPlutoLogger->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
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

					g_pPlutoLogger->Write(LV_STATUS, "Processing bound remote: for orbiter: %d", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
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

void MythTV_PlugIn::CMD_Sync_Providers_and_Cards(string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders");
    MYSQL_ROW row,row2;

	string sSQL = "select data from settings where value='DBSchemaVer'";
	PlutoSqlResult result_set_check;
	if( (result_set_check.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL))==NULL || (row=mysql_fetch_row(result_set_check.r))==NULL || atoi(row[0])<MINIMUM_MYTH_SCHEMA )
	{
		g_pPlutoLogger->Write(LV_WARNING,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards skipping now because I need at least schema %d insted of %s",
			MINIMUM_MYTH_SCHEMA, (NULL != result_set_check.r && row && row[0]) ? row[0] : "*none*");
		return;
	}

	sSQL = "select PK_Device,IK_DeviceData FROM Device JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_EK_MediaProvider_CONST) " AND IK_DeviceData IS NOT NULL AND IK_DeviceData<>'' AND IK_DeviceData<>'NONE'";

	bool bModifiedRows=false; // Keep track of whether or not we changed anything
	PlutoSqlResult result_set;
	if( (result_set.r=m_pMedia_Plugin->m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
	{
		while ((row = mysql_fetch_row(result_set.r)))
		{
			Row_Device *pRow_Device = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow( atoi(row[0]) );
			Row_MediaProvider *pRow_MediaProvider = m_pMedia_Plugin->m_pDatabase_pluto_media->MediaProvider_get()->GetRow( atoi(row[1]) );

			if( !pRow_Device || !pRow_MediaProvider )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders cannot find device %s provider %s",row[0],row[1]);
				continue;
			}

			string::size_type pos=0;
			string sMediaProviderID = pRow_MediaProvider->ID_get();
			string sUsername = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			string sPassword = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			int PK_DeviceTemplate_MediaType = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
			int PK_ProviderSource = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
			string sLineup = StringUtils::Tokenize(sMediaProviderID,"\t",pos);

			// We only care about capture cards
			Row_Device *pRow_Device_CaptureCard = pRow_Device;
			if( !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
			{
				pRow_Device_CaptureCard = pRow_Device_CaptureCard->FK_Device_ControlledVia_getrow();
				if( !pRow_Device_CaptureCard || !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
				{
#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders not a capture card device %s provider %s",row[0],row[1]);
#endif
					continue;
				}
			}

			// We need to add configure scripts for each model of card, or the sql statement to insert for each card. 

			// We have a capture card.  See if it's in the database already.  We use DEVICEDATA_Port_CONST for the port
			int cardid = atoi(DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Port_CONST).c_str());
			string sPortName = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Name_CONST);
			string sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
			if( sPortName.empty() )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders no port name for device %s provider %s",row[0],row[1]);
				continue;
			}
			if( !cardid )
			{
				bModifiedRows=true;
				string sHostname = "dcerouter";
				int PK_Device_PC = DatabaseUtils::GetTopMostDevice(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
				Row_Device *pRow_Device_PC = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device_PC);
				if( pRow_Device_PC )
				{
					Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_PC->FK_DeviceTemplate_getrow();
					if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Core_CONST )
						sHostname = "moon" + StringUtils::itos(pRow_Device_PC->PK_Device_get());
					g_pPlutoLogger->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards topmost for %d is %d host %s",
						pRow_Device->PK_Device_get(),pRow_Device_PC->PK_Device_get(),sHostname.c_str());
				}
				else
					g_pPlutoLogger->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find topmost device for %d", pRow_Device->PK_Device_get());

				sSQL = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Configuration_CONST);
				if( sSQL.empty() )
					sSQL = "INSERT INTO `capturecard`(cardtype,hostname) VALUES ('MPEG','" + sHostname + "');";
				else
					StringUtils::Replace(&sSQL,"<%=HOST%>",sHostname);

				cardid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
				DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Port_CONST,StringUtils::itos(cardid));
			}

			sSQL = "UPDATE `capturecard` set videodevice='" + sBlockDevice + "', defaultinput='" + sPortName + "'";
			if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
				bModifiedRows=true;

			int sourceid=0;
			// See if the provider is in the database already
			string sProviderName = "Provider " + StringUtils::itos(pRow_MediaProvider->PK_MediaProvider_get());
			sSQL = "SELECT sourceid FROM `videosource` WHERE name='" + sProviderName + "'";
			PlutoSqlResult result_set2;
			if( (result_set2.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row2=mysql_fetch_row(result_set2.r)) && atoi(row2[0]) )
				sourceid = atoi(row2[0]);
			else
			{
				bModifiedRows=true;
				// The data direct shouldn't be hardcoded
				sSQL = "INSERT INTO `videosource`(name,xmltvgrabber) VALUES ('" + sProviderName + "','datadirect')";
				sourceid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
			}

			sSQL = "UPDATE `videosource` SET userid='" + sUsername + "', password='" + sPassword + "', lineupid='" + sLineup + "'";
			if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
				bModifiedRows=true;

			int cardinputid=0;
			sSQL = "SELECT cardinputid FROM `cardinput` WHERE cardid='" + StringUtils::itos(cardid) + "' AND sourceid='" + StringUtils::itos(sourceid) + "'";
			PlutoSqlResult result_set3;
			if( (result_set3.r=m_pMySqlHelper_Myth->mysql_query_result(sSQL)) && (row2=mysql_fetch_row(result_set3.r)) && atoi(row2[0]) )
				cardinputid = atoi(row2[0]);
			else
			{
				bModifiedRows=true;
				sSQL = "INSERT INTO `cardinput`(cardid,sourceid,inputname) VALUES (" + StringUtils::itos(cardid) + "," + StringUtils::itos(sourceid) + ",'')";
				cardinputid = m_pMySqlHelper_Myth->threaded_mysql_query_withID(sSQL);
			}

			sSQL = "UPDATE `cardinput` set inputname='" + sPortName + "'";
			if( m_pMySqlHelper_Myth->threaded_mysql_query(sSQL)>0 )
				bModifiedRows=true;
		}
	}

	// We create /usr/pluto/bin/FillDbAndFetchIcons.start when we first start, and delete it when 
	// the script finishes.  So if the file is still there, that means the user reloaded the router
	// without letting the script finish, so we should restart it
	bool bLockFileExists = FileUtils::FileExists("/usr/pluto/bin/FillDbAndFetchIcons.start");
	if( bModifiedRows || bLockFileExists )
	{
		g_pPlutoLogger->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders records changed %d/%d",(int) bModifiedRows, (int) bLockFileExists);
		// Create the file
		system("touch /usr/pluto/bin/FillDbAndFetchIcons.start");
		DeviceData_Router *pDevice_App_Server=NULL,*pDevice_Us = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
		if( pDevice_Us )
			pDevice_App_Server = (DeviceData_Router *) pDevice_Us->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);

		if( pDevice_App_Server )
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application_fill(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				"/usr/pluto/bin/FillDbAndFetchIcons.sh","filldb","","","",false,false,true,false);
			SendCommand(CMD_Spawn_Application_fill);

			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
				"It will take about 10 minutes to retrieve your guide data before you can start using the TV features.", // Main message
				"", // Command Line
				"generic message", // Description
				"0", // sPromptToResetRouter
				"300", // sTimeout
				"1"); // sCannotGoBack
			SendCommand(SCREEN_PopupMessage);
		}
	}
}

void MythTV_PlugIn::UpdateMythSetting(string value,string data,string hostname)
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
					UpdateMythSetting(value,data,row[0]);
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

	sSQL = "UPDATE settings set data='" + StringUtils::SQLEscape(data) + "' WHERE value='" + StringUtils::SQLEscape(value) + "' "
		" AND hostname " + (hostname.empty() ? "IS NULL" : "='" + StringUtils::SQLEscape(hostname) + "'");

	m_pMySqlHelper_Myth->threaded_mysql_query(sSQL);
}

