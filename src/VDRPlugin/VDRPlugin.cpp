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

#include "EPG.h"
#include "EPGGrid.h"
#include "VDRStateInfo.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"

void* EPG_Thread( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	VDRPlugin *p = (VDRPlugin*)param;
	p->FetchEPG();
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VDRPlugin::VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VDRPlugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_VDRMutex("vdr")
{
	m_bEPGThreadRunning = false;
	pthread_cond_init( &m_VDRCond, NULL );
	m_VDRMutex.Init(NULL,&m_VDRCond);
}
VDREPG::EPG *pEPG;
//<-dceag-getconfig-b->
bool VDRPlugin::GetConfig()
{
	if( !VDRPlugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_bEPGThreadRunning=true;
	pthread_t pt_epg;
	if(pthread_create( &pt_epg, NULL, EPG_Thread, (void*)this) )
	{
		m_bEPGThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create EPG thread" );
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VDRPlugin::~VDRPlugin()
//<-dceag-dest-e->
{
	// Wait 10 seconds for the epg thread to quit
	m_bQuit=true;
	for(int i=0;i<1000;++i)
		if( !m_bEPGThreadRunning )
			break;
		else
		{
			pthread_cond_broadcast(&m_VDRCond);
			Sleep(10);
		}

		if( m_bEPGThreadRunning )
			g_pPlutoLogger->Write(LV_CRITICAL,"Could not kill EPG thread");

	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	for(map<int,VDREPG::EPG *>::iterator it=m_mapEPG.begin();it!=m_mapEPG.end();++it)
		delete it->second;
	for(map<int,VDRStateInfo *>::iterator it=m_mapVDRStateInfo.begin();it!=m_mapVDRStateInfo.end();++it)
		delete it->second;
}

void VDRPlugin::FetchEPG()
{
    ListDeviceData_Router *pListDeviceData_Router = 
		m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_VDR_CONST);

	bool bFirstRun=true;
	while(!m_bQuit)
	{
		PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			vm.TimedCondWait(1,0);  // 1 second delay here just to slow this down so it doesn't cause too much activity all at once
			vm.Release();
			DeviceData_Router *pDevice_VDR = *it;
			DeviceData_Router *pDevice_MD = (DeviceData_Router *) pDevice_VDR->m_pDevice_ControlledVia;
			DeviceData_Router *pDevice_Router = m_pRouter->m_mapDeviceData_Router_Find( m_pRouter->iPK_Device_get() );
			if( !pDevice_MD )
			{
				g_pPlutoLogger->Write(LV_WARNING,"Skipping VDR %d because it's not on a media director",pDevice_VDR->m_dwPK_Device);
				continue;
			}

			bool bIsHybrid = pDevice_Router && pDevice_MD->m_dwPK_Device_ControlledVia==pDevice_Router->m_dwPK_Device_ControlledVia;
			string sPath = pDevice_VDR->m_mapParameters_Find(DEVICEDATA_File_Name_and_Path_CONST);
			if( sPath.size()==0 )
				sPath = "/var/cache/vdrdevel";

			if( bIsHybrid )
				sPath = "/usr/pluto/diskless/" + pDevice_MD->m_sIPAddress + "/" + sPath;
#ifdef WIN32
			sPath = "Y:/home/root/var/cache/vdrdevel";
#endif
			g_pPlutoLogger->Write(LV_STATUS,"Reading EPG from %s",sPath.c_str());
			VDREPG::EPG *pEPG = new VDREPG::EPG();
			pEPG->ReadFromFile(sPath + "/epg.data",StringUtils::Replace(sPath,"/cache/","/lib/") + "/channels.conf");
			vm.Relock();
			if( pEPG->m_listChannel.size()==0 || pEPG->m_mapEvent.size()==0 )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"EPG file %s was badly parsed",sPath.c_str());
				continue;
			}
			VDREPG::EPG *pEPG_Old = m_mapEPG[pDevice_VDR->m_dwPK_Device];
			if( pEPG_Old )
				delete pEPG_Old;
			m_mapEPG[pDevice_VDR->m_dwPK_Device] = pEPG;
			g_pPlutoLogger->Write(LV_STATUS,"Done Reading EPG from %s",sPath.c_str());
		}
		// The first time refresh after 20 minutes to be sure we at least picked up the first days stuff
		if( bFirstRun )
		{
			bFirstRun=false;
			vm.TimedCondWait(1200,0);
		}
		else
			vm.TimedCondWait(7200,0);
	}
	m_bEPGThreadRunning=false;
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
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_VDR_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::CurrentShows))
                                                ,DATAGRID_EPG_Current_Shows_CONST,PK_DeviceTemplate_get());

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::AllShows))
                                                ,DATAGRID_EPG_All_Shows_CONST,PK_DeviceTemplate_get());

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
void VDRPlugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
		g_pPlutoLogger->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
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
			g_pPlutoLogger->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
			return NULL;
		}
	}

	MediaStream *pMediaStream = new MediaStream( pMediaHandlerInfo, iPK_MediaProvider,
										pMediaDevice,
										iPK_Users, st_RemovableMedia, StreamID );

	return pMediaStream;
}

bool VDRPlugin::StartMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write( LV_STATUS, "VDRPlugin::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
	int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pMediaStream->m_iStreamID_get( );

	VDRStateInfo *pVDRStateInfo = m_mapVDRStateInfo[PK_Device];
	if( !pVDRStateInfo )
	{
		pVDRStateInfo = new VDRStateInfo(PK_Device);
		m_mapVDRStateInfo[PK_Device]=pVDRStateInfo;
	}
	else // Set all the orbiter initial positions so we will regrab them
		for(map<int,bool>::iterator it=pVDRStateInfo->m_mapOrbiter_HasInitialPosition.begin();it!=pVDRStateInfo->m_mapOrbiter_HasInitialPosition.begin();++it)
			it->second = false;

	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	VDREPG::EPG *pEPG = m_mapEPG_Find(PK_Device);
	if( !pEPG )
		pEPG = m_mapEPG.begin()->second;
	if( !pEPG )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot start without any EPG data");
		return false;
	}
	pVDRStateInfo->m_bViewing = true;
	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,PK_Device,"",pMediaStream->m_iPK_MediaType,
		pMediaStream->m_iStreamID_get(), "");

	SendCommand(CMD_Play_Media);
	return true;
}

bool VDRPlugin::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Stopping media in VDR_PlugIn!");

	string SavedPosition;
	int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	VDRStateInfo *pVDRStateInfo = m_mapVDRStateInfo_Find(PK_Device);
	if( !pVDRStateInfo )
		g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::StopMedia but no state info");
	else
		pVDRStateInfo = false;

	int StreamID = pMediaStream->m_iStreamID_get( );
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,PK_Device,pMediaStream->m_iStreamID_get(),&SavedPosition);


	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( CMD_Stop_Media ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		g_pPlutoLogger->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		g_pPlutoLogger->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

	return true;
}

MediaDevice *VDRPlugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_VDR_CONST);
	g_pPlutoLogger->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

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
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	MediaDevice *pMediaDevice = GetVDRFromOrbiter(pMessage->m_dwPK_Device_From);
	VDREPG::EPG *pEPG = NULL; 
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
    if( pMediaDevice && (pEPG=m_mapEPG_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device)) && pEPG->m_mapEvent.size() && sValue_To_Assign.size() )
	{
		VDRStateInfo *pVDRStateInfo = m_mapVDRStateInfo_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
		if( !pVDRStateInfo )
			g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist but no state info");
		else
		{
			VDREPG::Event *pEvent=NULL;
			if( sValue_To_Assign[0]=='-' || sValue_To_Assign[0]=='+' )
			{
				pEvent = pEPG->m_mapEvent_Find(pVDRStateInfo->EventID);
				if( !pEvent )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find current event!");
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
					g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist event invalid!");
					pEvent = pEPG->m_mapEvent.begin()->second;
				}
				pEvent = pEvent->ConfirmCurrentProgram(); // Confirm it's the current one
			}
			else  // Must be a channel
			{
				VDREPG::Channel *pChannel = pEPG->m_mapChannelNumber_Find( atoi(sValue_To_Assign.c_str()) );
				if( !pChannel )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find channel %s",sValue_To_Assign.c_str());
					pChannel = *pEPG->m_listChannel.begin();
				}
				pEvent = pChannel->GetCurrentEvent();
			}
			if( !pEvent || !pEvent->m_pChannel )
				g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist trying to tune to unknown event %s",sValue_To_Assign.c_str());
			else
			{
				DCE::CMD_Tune_to_channel CMD_Tune_to_channel(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
					"",StringUtils::itos(pEvent->m_pChannel->m_ChannelID));
				SendCommand(CMD_Tune_to_channel);
			}
		}
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist confused?? media device %p EPG %p events %d val: %s",
			pMediaDevice,pEPG,(int) pEPG->m_mapEvent.size(),sValue_To_Assign.c_str());
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
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
    DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pDataGridCell;

	MediaDevice *pMediaDevice = GetVDRFromOrbiter(pMessage->m_dwPK_Device_From);
	VDREPG::EPG *pEPG = NULL; 
	if( pMediaDevice )
		pEPG=m_mapEPG_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
	if( !pEPG )
		pEPG = m_mapEPG.begin()->second;
	if( !pEPG )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CurrentShows No EPG");
		return pDataGrid;
	}

	int iColumns=atoi(Parms.c_str());
	int iRow=0,iColumn=0;
	for(list<VDREPG::Channel *>::iterator it=pEPG->m_listChannel.begin();it!=pEPG->m_listChannel.end();++it)
	{
		VDREPG::Channel *pChannel = *it;
		VDREPG::Event *pEvent = pChannel->GetCurrentEvent();
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
	return pDataGrid;
}

class DataGridTable *VDRPlugin::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	int iGridResolutions = atoi(Parms.c_str());
	if( !iGridResolutions )
		iGridResolutions = 5;
	MediaDevice *pMediaDevice = GetVDRFromOrbiter(pMessage->m_dwPK_Device_From);

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	VDRStateInfo *pVDRStateInfo = m_mapVDRStateInfo[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device];
	if( !pVDRStateInfo )
	{
		pVDRStateInfo = new VDRStateInfo(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
		m_mapVDRStateInfo[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pVDRStateInfo;
	}

	return new VDREPG::EpgGrid(this,pVDRStateInfo,pMessage->m_dwPK_Device_From,iGridResolutions);  // Fallback, just return any VDR grid
}

class MediaDevice *VDRPlugin::GetVDRFromOrbiter(int PK_Device)
{
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device);
	if( pOH_Orbiter && pOH_Orbiter->m_pEntertainArea )
	{
		ListMediaDevice *pListMediaDevice = pOH_Orbiter->m_pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_VDR_CONST);
		if( pListMediaDevice && pListMediaDevice->size() )
			return *pListMediaDevice->begin();
	}
	return NULL;
}
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
	MediaDevice *pMediaDevice = GetVDRFromOrbiter(pMessage->m_dwPK_Device_From);
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	VDREPG::EPG *pEPG = NULL; 
	if( pMediaDevice && (pEPG=m_mapEPG_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device)) )
	{
		VDREPG::Event *pEvent = pEPG->m_mapEvent_Find(atoi(sProgramID.substr(1).c_str()));
		if( !pEvent || !pEvent->m_pChannel )
			g_pPlutoLogger->Write(LV_CRITICAL,"VDRPlugin::CMD_Get_Extended_Media_Data trying to tune to unknown event %s",sProgramID.c_str());
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
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable2.m_pMessage );
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable3.m_pMessage );
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable4.m_pMessage );
			if( sPK_DesignObj.size() )
			{
				DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pMessage->m_dwPK_Device_From,
					0,sPK_DesignObj,"","",false,false);
				CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Goto_Screen.m_pMessage );
			}
			else
			{
				DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device,pMessage->m_dwPK_Device_From,"");
				CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Refresh.m_pMessage );
			}
			SendCommand(CMD_Set_Variable1);
		}
	}
}
