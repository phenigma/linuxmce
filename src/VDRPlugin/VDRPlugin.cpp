/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//<-dceag-d-b->
#include "VDRPlugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "VDRMediaStream.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_media/Table_Bookmark.h"
#include "../VDR/VDRCommon.h"
#include "../DCE/DataGrid.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VDRPlugin::VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VDRPlugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
, m_VDRMutex("vdr")
{
	pthread_cond_init( &m_VDRCond, NULL );
	m_VDRMutex.Init(NULL,&m_VDRCond);
	m_sVDRIp="192.168.0.100";
}
//<-dceag-getconfig-b->
bool VDRPlugin::GetConfig()
{
	if( !VDRPlugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VDRPlugin::~VDRPlugin()
//<-dceag-dest-e->
{
}

void VDRPlugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
}




//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VDRPlugin::Register()
//<-dceag-reg-e->
{	
	m_iPriority=DATA_Get_Priority();
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin || !m_pDatagrid_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_VDR_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::CurrentShows))
		,DATAGRID_EPG_Current_Shows_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::AllShows))
		,DATAGRID_EPG_All_Shows_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::FavoriteChannels))
		,DATAGRID_Favorite_Channels_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::FavoriteShows))
		,DATAGRID_Favorite_Shows_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::OtherShowtimes))
		,DATAGRID_Other_Showtimes_CONST,PK_DeviceTemplate_get());

	BuildChannelList();

	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
When you receive commands that are destined to one of your children,
then if that child implements DCE then there will already be a separate class
created for the child that will get the message.  If the child does not, then you will 
get all	commands for your children in ReceivedCommandForChild, where 
pDeviceData_Base is the child device.  If you handle the message, you 
should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void VDRPlugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VDRPlugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

class MediaStream *VDRPlugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
		pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_VDR_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_VDR_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
	}

	if ( !pMediaDevice )
	{
		for(size_t s=0;s<vectEntertainArea.size();++s)
		{
			EntertainArea *pEntertainArea = vectEntertainArea[0];
			pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea);
			if( pMediaDevice )
				break;
		}
		if( !pMediaDevice )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
			return NULL;
		}
	}

	VDRMediaStream *pVDRMediaStream = new VDRMediaStream( this, pMediaHandlerInfo, iPK_MediaProvider,
		pMediaDevice,
		iPK_Users, st_RemovableMedia, StreamID );

	return pVDRMediaStream;
}

bool VDRPlugin::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	/*
	VDRMediaStream *pVDRMediaStream = (VDRMediaStream *) pMediaStream;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "VDRPlugin::StartMedia() Starting media stream playback. pos: %d", pVDRMediaStream->m_iDequeMediaFile_Pos );
	int PK_Device = pVDRMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pVDRMediaStream->m_iStreamID_get( );

	for(map<int,bool>::iterator it=pVDRMediaStream->m_mapOrbiter_HasInitialPosition.begin();it!=pVDRMediaStream->m_mapOrbiter_HasInitialPosition.begin();++it)
		it->second = false;

	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	VDREPG::EPG *pEPG = m_mapEPG_Find(PK_Device);
	if( !pEPG && m_mapEPG.size() )
		pEPG = m_mapEPG.begin()->second;

	VDREPG::Event *pEvent = GetStartingEvent(pEPG,pVDRMediaStream->m_iPK_Users);
	if( !pEPG || !pEvent )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot start without any EPG data %p %p",pEPG,pEvent);
		sError = "Cannot start TV with no EPG Data.  Check the connection and try again in 20 minutes.";
		return false;
	}
	pVDRMediaStream->m_EventID = pEvent->m_EventID;
	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,PK_Device,pVDRMediaStream->m_iPK_MediaType,
		pVDRMediaStream->m_iStreamID_get(), " CHAN:" + StringUtils::itos(pEvent->m_pChannel->m_ChannelID),"");

	SendCommand(CMD_Play_Media);
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
	*/
	return false;
}

bool VDRPlugin::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in VDR_PlugIn!");

	string SavedPosition;
	int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pMediaStream->m_iStreamID_get( );
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,PK_Device,pMediaStream->m_iStreamID_get(),&SavedPosition);


	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( CMD_Stop_Media ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped",
			pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

	return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *VDRPlugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_VDR_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}



/*

COMMANDS TO IMPLEMENT

*/



//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void VDRPlugin::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	/*
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) &&
		pEPG->m_mapEvent.size() && sValue_To_Assign.size() )
	{
		VDREPG::Event *pEvent=NULL;
		if( sValue_To_Assign[0]=='-' || sValue_To_Assign[0]=='+' )
		{
			pEvent = pEPG->m_mapEvent_Find(pVDRMediaStream->m_EventID);
			if( !pEvent )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find current event!");
				pEvent = pEPG->m_mapEvent.begin()->second;
			}
			VDREPG::Channel *pChannel = pEvent->m_pChannel;
			if( sValue_To_Assign[0]=='-' )
				pEvent = pEvent->m_pChannel->m_pChannel_Prior->GetCurrentEvent();
			else
				pEvent = pEvent->m_pChannel->m_pChannel_Next->GetCurrentEvent();
		}
		else if( sValue_To_Assign[0]=='E' )
		{
			pEvent = pEPG->m_mapEvent_Find(atoi(sValue_To_Assign.substr(1).c_str()));
			if( !pEvent )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist event invalid!");
				pEvent = pEPG->m_mapEvent.begin()->second;
			}
			pEvent = pEvent->ConfirmCurrentProgram(); // Confirm it's the current one
		}
		else  // Must be a channel
		{
			VDREPG::Channel *pChannel = pEPG->m_mapChannelNumber_Find( atoi(sValue_To_Assign.c_str()) );
			if( !pChannel )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find channel %s",sValue_To_Assign.c_str());
				pChannel = *pEPG->m_listChannel.begin();
			}
			pEvent = pChannel->GetCurrentEvent();
		}
		if( !pEvent || !pEvent->m_pChannel )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist trying to tune to unknown event %s",sValue_To_Assign.c_str());
		else
		{
			pVDRMediaStream->m_EventID = pEvent->m_EventID;
			DCE::CMD_Tune_to_channel CMD_Tune_to_channel(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				"",StringUtils::itos(pEvent->m_pChannel->m_ChannelID));
			SendCommand(CMD_Tune_to_channel);
		}
		vm.Release();

		m_pMedia_Plugin->MediaInfoChanged(pVDRMediaStream);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist confused");
		*/
}

//<-dceag-c185-b->

	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

void VDRPlugin::CMD_Schedule_Recording(string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c185-e->
{
}

class DataGridTable *VDRPlugin::CurrentShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pDataGridCell;
	/*

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG;  VDRMediaStream *pVDRMediaStream;
	if( !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CurrentShows No EPG");
		return pDataGrid;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::CurrentShows 2");

	int iColumns=atoi(Parms.c_str());
	int iRow=0,iColumn=0;
	for(list<VDREPG::Channel *>::iterator it=pEPG->m_listChannel.begin();it!=pEPG->m_listChannel.end();++it)
	{
		VDREPG::Channel *pChannel = *it;
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::CurrentShows 3");
		VDREPG::Event *pEvent = pChannel->GetCurrentEvent();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::CurrentShows 4");
		if( pEvent )
		{
			pDataGridCell = new DataGridCell(pEvent->m_pChannel->m_sChannelName,"E" + StringUtils::itos(pEvent->m_EventID));
			pDataGrid->SetData((iColumn*5),iRow,pDataGridCell);
			pDataGridCell = new DataGridCell(pEvent->m_pProgram->m_sTitle,"E" + StringUtils::itos(pEvent->m_EventID));
			pDataGridCell->m_Colspan=3;
			pDataGrid->SetData((iColumn*5)+1,iRow,pDataGridCell);

			struct tm *tmptr = localtime(&pEvent->m_tStartTime);
			struct tm tmStart = *tmptr;
			tmptr = localtime(&pEvent->m_tStopTime);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::CurrentShows 5");

			string sDesc;
			sDesc += 
				StringUtils::itos(tmStart.tm_hour) + ":" + (tmStart.tm_min<10 ? "0" : "") +
				StringUtils::itos(tmStart.tm_min) + " - " +
				StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") +
				StringUtils::itos(tmptr->tm_min);

			pDataGridCell = new DataGridCell(sDesc,"E" + StringUtils::itos(pEvent->m_EventID));
			pDataGrid->SetData((iColumn*5)+4,iRow,pDataGridCell);
			iColumn++;
			if( iColumn>iColumns )
			{
				iColumn=0;
				iRow++;
			}
		}
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::CurrentShows 6");
	*/
	return pDataGrid;
}

class DataGridTable *VDRPlugin::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGridTable = new DataGridTable();

	string::size_type pos=0;
	int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::AllShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());
    
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( iPK_EntertainArea );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::AllShows cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}

	string sProvider;
	map<int,bool> mapVideoSourcesToUse;
	// The source is 0, means all VDR channels and video sources.  Otherwise, just those for the given device (like a cable box)
	int PK_Device_Source = pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ? 0 : pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	list_int *p_list_int = NULL;
	if( PK_Device_Source && m_mapDevicesToSources.find(PK_Device_Source)!=m_mapDevicesToSources.end() )
		p_list_int = &(m_mapDevicesToSources[PK_Device_Source]);

	// Go through all the channels, and set the cell to NULL if we're not supposed to include it
	// or to a new value if we are.  We'll update the description with the show down below
	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		VDRChannel *pVDRChannel = *it;
		if( p_list_int )
		{
			bool bOk=false;
			for(list_int::iterator it=p_list_int->begin();it!=p_list_int->end();++it)
			{
int i=*it;
				if( *it==pVDRChannel->m_pVDRSource->m_dwID )
				{
					bOk=true;
					break;
				}
			}
			if( !bOk )
			{
				m_pVDRSource->m_pCell=NULL;
				continue;
			}
		}
		string sChannelName = StringUtils::itos(pVDRChannel->m_dwChanNum) + " " + pVDRChannel->m_sShortName;
		pVDRChannel->m_pCell = new DataGridCell(sChannelName,"i" + StringUtils::itos(pVDRChannel->m_dwID));
		pVDRChannel->m_pCell->m_mapAttributes["Name"] = sChannelName;
		pVDRChannel->m_pCell->m_mapAttributes["Source"] = pVDRChannel->m_pVDRSource->m_sDescription;
	}

	string sVDRResponse;
	if( !SendVDRCommand(m_sVDRIp,"LSTC",sVDRResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::AllShows cannot get channel list");
		return pDataGridTable;
	}

	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL = "SELECT chanid, title, starttime, endtime, seriesid, programid "
		"FROM program "
		"WHERE starttime < '" + StringUtils::SQLDateTime() + "' AND endtime>'" + StringUtils::SQLDateTime() + "' " + sProvider;

	bool bAllSource = mapVideoSourcesToUse.empty();
	int iRow=0;
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r=m_pDBHelper_VDR->db_wrapper_query_result(sSQL))!=NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::AllShows rows %d map %d allsource %d sources to use %d",
			result.r->row_count, (int) m_mapVDRChannel.size(), (int) bAllSource, (int) mapVideoSourcesToUse.size() );

		while((row = db_wrapper_fetch_row(result.r)))
		{
			VDRChannel *pVDRChannel = m_mapVDRChannel_Find( atoi(row[0]) );
			if( !pVDRChannel || !pVDRChannel->m_pCell )
				continue; // Shouldn't happen that pVDRChannel is NULL, if cell is, we're not including it

			if( bAllSource==false && mapVideoSourcesToUse[ pVDRChannel->m_pVDRSource->m_dwID ]==false )  // Not a source for this list
				continue;

			pVDRChannel->m_pCell->SetText( string(pVDRChannel->m_pCell->GetText()) + row[1] );
			if( pVDRChannel->m_pPic )
			{
				char *pPic = new char[ pVDRChannel->m_Pic_size ];
				memcpy(pPic,pVDRChannel->m_pPic,pVDRChannel->m_Pic_size);
				pVDRChannel->m_pCell->SetImage( pPic, pVDRChannel->m_Pic_size, GR_JPG );
			}

			time_t tStart = StringUtils::SQLDateTime( row[2] );
			time_t tStop = StringUtils::SQLDateTime( row[3] );
			string sTime = StringUtils::HourMinute(tStart) + " - " + StringUtils::HourMinute(tStop);
			string sNumber = NULL != row[0] ? row[0] : "";
			string sInfo = NULL != row[4] ? row[1]: "";

			pVDRChannel->m_pCell->m_mapAttributes["Number"] = sNumber;
			pVDRChannel->m_pCell->m_mapAttributes["Time"] = sTime;
			pVDRChannel->m_pCell->m_mapAttributes["Info"] = sInfo;
			if( row[4] )
				pVDRChannel->m_pCell->m_mapAttributes["Series"] = row[4];
			if( row[5] )
				pVDRChannel->m_pCell->m_mapAttributes["Program"] = row[5];
			MapBookmark *pMapBookmark_Series_Or_Program;
			MapBookmark::iterator it;
			if( (it=pVDRChannel->m_mapBookmark.find(0))!=pVDRChannel->m_mapBookmark.end() ||
				(it=pVDRChannel->m_mapBookmark.find(iPK_Users))!=pVDRChannel->m_mapBookmark.end() )
			{
				// It's a user's favorited channel
				pVDRChannel->m_pCell->m_mapAttributes["PK_Bookmark"] = it->second;
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
			}
			else if( row[4] &&
				(pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(row[4])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
			}
			else if( row[5] &&
				(pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(row[5])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series.
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
			}
		}
	}

	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		VDRChannel *pVDRChannel = *it;
		if( pVDRChannel->m_pCell )
			pDataGridTable->SetData(0,iRow++,pVDRChannel->m_pCell);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::AllShows cells: %d source %d plist %p map %d",
		(int) pDataGridTable->m_MemoryDataTable.size(), PK_Device_Source, p_list_int, (int) m_mapDevicesToSources.size() );

	return pDataGridTable;
}

class DataGridTable *VDRPlugin::OtherShowtimes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	/*
	if( Parms.size()==0 )
		return pDataGrid;  // Shouldn't happen

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CurrentShows No EPG");
		return pDataGrid;
	}

	int iRow=0;
	VDREPG::Event *pEvent = pEPG->m_mapEvent_Find(atoi(Parms.substr(1).c_str()));
	pEvent->m_pProgram->m_listEvent.sort(EventsByShowtime);
	for(list<VDREPG::Event *>::iterator it=pEvent->m_pProgram->m_listEvent.begin();it!=pEvent->m_pProgram->m_listEvent.end();++it)
	{
		VDREPG::Event *pEvent_Other = *it;
		if( pEvent_Other->AlreadyOver() )
			continue;

		string sDesc = pEvent_Other->m_pChannel->m_sChannelName + " / " + pEvent_Other->GetShortShowtime();
		if( pEvent_Other->m_sDescription_Short.size() )
			sDesc += "\n" + pEvent_Other->m_sDescription_Short;
		else if( pEvent_Other->m_sDescription_Long.size() )
			sDesc += "\n" + pEvent_Other->m_sDescription_Long;

		DataGridCell *pDataGridCell = new DataGridCell(sDesc, "E" + StringUtils::itos(pEvent_Other->m_EventID));
		pDataGrid->SetData(0,iRow++,pDataGridCell);
	}
*/
	return pDataGrid;
}

class DataGridTable *VDRPlugin::FavoriteChannels(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::FavoriteChannels Called to populate: %s", Parms.c_str());

	int PK_Users = atoi(Parms.c_str());
	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND Position LIKE ' CHAN:%' AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users) + ")",&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		string sDescription = pRow_Bookmark->Description_get().size() ? pRow_Bookmark->Description_get() : pRow_Bookmark->Position_get();
		DataGridCell *pDataGridCell = new DataGridCell(sDescription, "B" + StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(0,s,pDataGridCell);
	}
*/
	return pDataGrid;
}

class DataGridTable *VDRPlugin::FavoriteShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::FavoriteShows Called to populate: %s", Parms.c_str());

	int PK_Users = atoi(Parms.c_str());
	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND Position LIKE ' PROG:%' AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users) + ")",&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		string sDescription = pRow_Bookmark->Description_get().size() ? pRow_Bookmark->Description_get() : pRow_Bookmark->Position_get();
		DataGridCell *pDataGridCell = new DataGridCell(sDescription, "B" + StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(0,s,pDataGridCell);
	}
*/
	return pDataGrid;
}
	/*

bool VDRPlugin::GetVdrAndEpgFromOrbiter(int PK_Device,MediaDevice *&pMediaDevice_VDR,VDREPG::EPG *&pEPG,VDRMediaStream *&pVDRMediaStream)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 1");
#ifdef DEBUG
	if( m_VDRMutex.m_NumLocks==0 || m_pMedia_Plugin->m_MediaMutex.m_NumLocks==0 )
	{
		// EPG is volatile and the mutex must be blocked whenever we get a pointer to it, also media plugin to protect teh stream
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter you're not holding the vdr mutex");
		return false;
	}
#endif

	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device);
	if( pOH_Orbiter && pOH_Orbiter->m_pEntertainArea )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 2");
		ListMediaDevice *pListMediaDevice = pOH_Orbiter->m_pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_VDR_CONST);
		if( pListMediaDevice && pListMediaDevice->size() )
			pMediaDevice_VDR = *pListMediaDevice->begin();
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter No VDR device");
			return false;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter not ent area");
		return false;
	}

	if( pMediaDevice_VDR->m_mapEntertainArea.size()==1 )
	{
		EntertainArea *pEntertainArea = pMediaDevice_VDR->m_mapEntertainArea.begin()->second;
		if( pEntertainArea->m_pMediaStream->GetType()!=MEDIASTREAM_TYPE_VDR )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter media stream isn't for VDR");
			return false;
		}
		pVDRMediaStream = (VDRMediaStream *) pEntertainArea->m_pMediaStream;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter VDR isn't in a single entertainment area");
		return false;
	}

LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 3");
	pEPG = m_mapEPG_Find(pMediaDevice_VDR->m_pDeviceData_Router->m_dwPK_Device);
	if( !pEPG )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter No EPG");
		return false;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 4");
	return true;
}
*/	
//<-dceag-c698-b->

	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

void VDRPlugin::CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c698-e->
{
	/*
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Get_Extended_Media_Data No EPG");
		return;
	}

	VDREPG::Event *pEvent = NULL;
	if( sProgramID.size()==0 )
		pEvent = pEPG->m_mapEvent_Find(pVDRMediaStream->m_EventID);
	else
		pEvent = pEPG->m_mapEvent_Find(atoi(sProgramID.substr(1).c_str()));
	if( !pEvent || !pEvent->m_pChannel )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Get_Extended_Media_Data trying to tune to unknown event %s",sProgramID.c_str());
	else
	{
		struct tm *tmptr = localtime(&pEvent->m_tStartTime);
		struct tm tm_start = *tmptr;
		tmptr = localtime(&pEvent->m_tStopTime);
		DCE::CMD_Set_Variable CMD_Set_Variable1(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_1_CONST,pEvent->m_pChannel->m_sChannelName + " / " +
			StringUtils::itos(tm_start.tm_hour) + ":" + (tm_start.tm_min<10 ? "0" : "") + StringUtils::itos(tm_start.tm_min) + " - " +
			StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") + StringUtils::itos(tmptr->tm_min));
		DCE::CMD_Set_Variable CMD_Set_Variable2(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_2_CONST,pEvent->m_pProgram->m_sTitle);
		DCE::CMD_Set_Variable CMD_Set_Variable3(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_3_CONST,pEvent->m_sDescription_Short);
		DCE::CMD_Set_Variable CMD_Set_Variable4(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_4_CONST,pEvent->m_sDescription_Long);
		DCE::CMD_Set_Variable CMD_Set_VariableEvent(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Datagrid_Input_CONST,"E" + StringUtils::itos(pEvent->m_EventID));
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable2.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable3.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable4.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_VariableEvent.m_pMessage );
		if( sPK_DesignObj.size() )
		{
			DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				0,sPK_DesignObj,"","",false,false);
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Goto_DesignObj.m_pMessage );
		}
		else
		{
			DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device,pMessage->m_dwPK_Device_From,"");
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Refresh.m_pMessage );
		}
		SendCommand(CMD_Set_Variable1);
	}
	*/
}

	/*
VDREPG::Event *VDRPlugin::GetStartingEvent(VDREPG::EPG *pEPG,int PK_Users)
{
	VDREPG::Event *pEvent = NULL;  // This will be the one with the highest priority
	int iPriorityLevel = 5;  // The priority of that event.  1=best match (current show) private, 2=current show public, 3=channel private, 4=channel public, 5=no match
	if( !pEPG )
		return pEvent; // Can't do anything without EPG

	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users)+")",&vectRow_Bookmark);
	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		int iPriority_Bookmark;
		VDREPG::Event *pEvent_Bookmark = GetEventForBookmark(pEPG,vectRow_Bookmark[s],iPriority_Bookmark);
		if( pEvent_Bookmark && iPriorityLevel>iPriority_Bookmark )
		{
			iPriorityLevel=iPriority_Bookmark;
			pEvent = pEvent_Bookmark;
		}
	}

	if( !pEvent )  // Just find the first show
	{
		time_t tNow = time(NULL);
		for(size_t s=0;s<pEPG->m_vectChannel.size();++s)
		{
			pEvent = pEPG->m_vectChannel[s]->GetCurrentEvent();
			if( pEvent && pEvent->m_tStartTime<tNow && pEvent->m_tStopTime>tNow)
				return pEvent;
		}
	}
	return pEvent;
	return NULL;
}
	*/

	/*
VDREPG::Event *VDRPlugin::GetEventForBookmark(VDREPG::EPG *pEPG,Row_Bookmark *pRow_Bookmark,int &iPriority_Bookmark)
{
	time_t tNow = time(NULL);
	string::size_type pos;
	bool bIsShow = (pos=pRow_Bookmark->Position_get().find(" PROG:"))!=string::npos && pRow_Bookmark->Position_get().size()-pos>6;
	if( bIsShow )
	{
		VDREPG::Program *pProgram = pEPG->m_mapProgram_Find(pRow_Bookmark->Position_get().substr(pos+6));
		if( !pProgram )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetEventForBookmark Cannot find any programs of %s",pRow_Bookmark->Position_get().c_str());
			return NULL;
		}

		VDREPG::Event *pEvent = pProgram->GetNextEvent();
		if( !pEvent )
			return NULL; // Should never happen

		if( pEvent->m_tStartTime > tNow + 600 )
			return NULL; // Ignore this it's not starting soon

		if( pRow_Bookmark->EK_Users_get() )
			iPriority_Bookmark = 1;
		else
			iPriority_Bookmark = 2;
		return pEvent;
	}

	VDREPG::Channel *pChannel;
	VDREPG::Event *pEvent;
	pos=pRow_Bookmark->Position_get().find(" CHAN:");
	if( pos==string::npos ||  pRow_Bookmark->Position_get().size()-pos<=6 || 
		(pChannel=pEPG->m_mapChannelName_Find(pRow_Bookmark->Position_get().substr(pos+6)))==NULL ||
		(pEvent=pChannel->GetCurrentEvent())==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetEventForBookmark Cannot find any channels of %s",pRow_Bookmark->Position_get().c_str());
		return NULL;
	}

	if( pRow_Bookmark->EK_Users_get() )
		iPriority_Bookmark = 3;
	else
		iPriority_Bookmark = 4;
	return pEvent;
}
	*/
//<-dceag-c409-b->

	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current channel or program as a bookmark.  Text should have CHAN: or PROG: in there */
		/** @param #39 Options */
			/** For TV, CHAN: or PROG: indicating if it's the channel or program to bookmark */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */

void VDRPlugin::CMD_Save_Bookmark(string sOptions,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c409-e->
{
	/*
	string sBookmark;
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	VDREPG::Event *pEvent = NULL;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( !pOH_Orbiter || !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) ||
		(pEvent = pEPG->m_mapEvent_Find(pVDRMediaStream->m_EventID))==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Save_Bookmark No EPG");
		return;
	}

	string sPosition,sDescription;
	if( sOptions.find("PROG")!=string::npos )
	{
		sPosition = " PROG:" + pEvent->m_pProgram->m_sTitle;
		sDescription = pEvent->m_pProgram->m_sTitle;
	}
	else
	{
		sPosition = " CHAN:" + pEvent->m_pChannel->m_sChannelName;
		sDescription = pEvent->m_pChannel->m_sChannelName;
	}

	Row_Bookmark *pRow_Bookmark = m_pMedia_Plugin->GetMediaDatabaseConnect()->Bookmark_get()->AddRow();
	pRow_Bookmark->EK_MediaType_set(MEDIATYPE_pluto_LiveTV_CONST);
	pRow_Bookmark->Description_set(sDescription);
	pRow_Bookmark->Position_set(sPosition);
	pRow_Bookmark->EK_Users_set(pOH_Orbiter->m_pOH_User->m_iPK_Users);
	pRow_Bookmark->Table_Bookmark_get()->Commit();
	DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device,pMessage->m_dwPK_Device_From,"*");
	SendCommand(CMD_Refresh);
	*/
}
//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

void VDRPlugin::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
}
//<-dceag-c824-b->

	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */

void VDRPlugin::CMD_Sync_Providers_and_Cards(string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
}

void VDRPlugin::PurgeChannelList()
{
}

VDRSource *VDRPlugin::GetNewSource(string sSource)
{
	VDRSource *pVDRSource = m_mapVDRSource_Find(sSource);
	if( pVDRSource==NULL )
	{
		pVDRSource = new VDRSource(1,sSource);
		m_mapVDRSource[sSource]=pVDRSource;
	}
	return pVDRSource;
}

void VDRPlugin::BuildChannelList()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	PurgeChannelList();

	string sVDRResponse;
	if( !SendVDRCommand(m_sVDRIp,"LSTC",sVDRResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::BuildChannelList cannot get channel list");
		return;
	}
	string::size_type pos_line=0;
	string sLine;
	while( true )
	{
		sLine = StringUtils::Tokenize(sVDRResponse,"\n",pos_line);
		if( sLine.empty()==true )
			break;

		string::size_type pos_delimit = 0;
		string sChannel = StringUtils::Tokenize(sLine,":",pos_delimit);
		StringUtils::Replace(sChannel,"|",":");
		string sC1 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC2 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sSource = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC4 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC5 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC6 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC7 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC8 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sSID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sNID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sTID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC12 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC13 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC14 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC15 = StringUtils::Tokenize(sLine,":",pos_delimit);

		VDRSource *pVDRSource = GetNewSource(sSource);
		int iChannel = atoi(sChannel.c_str());
		VDRChannel *pVDRChannel = new VDRChannel(iChannel,iChannel,pVDRSource,"","",NULL,0);
		m_ListVDRChannel.push_back(pVDRChannel);
	}
/*
	
	VDRSource *pVDRSource = m_mapVDRSource_Find( atoi(row[6]) );
			if( !pVDRSource )
			{
				string sDescription = row[6];

				sSQL = "select name from videosource where sourceid=" + string(row[6]);
				PlutoSqlResult result2;
				DB_ROW row2;
				if( (result2.r=m_pDBHelper_VDR->db_wrapper_query_result(sSQL))!=NULL && (row2 = db_wrapper_fetch_row(result2.r))!=NULL )
				{
					string s = row2[0];
					// This is probably in the format Provider X.  Try to get that better description
					if( s.size()>9 )
					{
						int Provider = atoi(s.substr(8).c_str());
						Row_MediaProvider *pRow_MediaProvider = m_pMedia_Plugin->m_pDatabase_pluto_media->MediaProvider_get()->GetRow(Provider);
						if( pRow_MediaProvider && pRow_MediaProvider->Description_get().empty()==false )
							sDescription += " " + pRow_MediaProvider->Description_get();
					}
					else
						sDescription += " " + s;
				}
				pVDRSource = new VDRSource(atoi(row[6]),sDescription);
				m_mapVDRSource[ pVDRSource->m_dwID ] =pVDRSource;
			}

			string sFilename;
			if( row[5] )
				sFilename = "/home/mediapics/" + string(row[5]) + "_tn.jpg";
			else if( row[4] )
				sFilename = row[4];

			size_t size=0;
			char *pData = sFilename.empty() ? NULL : FileUtils::ReadFileIntoBuffer(sFilename,size);
			VDRChannel *pVDRChannel = new VDRChannel( atoi(row[0]), atoi(row[1]), pVDRSource, row[2] ? row[2] : "", row[3] ? row[3] : "", pData, size );
			m_mapVDRChannel[pVDRChannel->m_dwID] = pVDRChannel;
			m_ListVDRChannel.push_back(pVDRChannel);
		}
	}
	m_ListVDRChannel.sort(ChannelComparer);
*/
}
