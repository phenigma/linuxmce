//<-dceag-d-b->
#include "SlimServer_PlugIn.h"
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

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SlimServer_PlugIn::SlimServer_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SlimServer_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SlimServer_PlugIn::SlimServer_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: SlimServer_PlugIn_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
SlimServer_PlugIn::~SlimServer_PlugIn()
//<-dceag-dest-e->
{
	vector<void *> vectvoid;
	ProcessUtils::KillApplication("slimserver",vectvoid);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SlimServer_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();
	m_pMedia_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Media_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "SlimServer plug in cannot find media handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}

	m_pMedia_Plugin=( Media_Plugin * ) pListCommand_Impl->front( );

    pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Orbiter_Plugin_CONST );
    if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Media handler plug in cannot find orbiter handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
        return false;
    }

    m_pOrbiter_Plugin=( Orbiter_Plugin * ) pListCommand_Impl->front( );

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_SqueezeBox_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	// In our device data we will give ourselves a lower priority and set multipledestinations=true 
	// so we yield priority for single destinations to a non-streaming type of media
	for(size_t s=0;s<m_vectMediaHandlerInfo.size();++s)
		m_vectMediaHandlerInfo[s]->m_bMultipleDestinations=true;

	string sCmdParms="--audiodir\t/home/public/data/music\t-cliaddr\t127.0.0.1\t--d_protocol\t--d_cli\t--d_server";
	if( !ProcessUtils::SpawnApplication("/usr/pluto/servers/SlimServer/slimserver.pl",sCmdParms,"slimserver") )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to spawn slimserver");

	return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SlimServer_PlugIn_Command *Create_SlimServer_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new SlimServer_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void SlimServer_PlugIn::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void SlimServer_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

class MediaStream *SlimServer_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	SlimServerMediaStream *pSlimServerMediaStream;
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
	     pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_SqueezeBox_Player_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_SqueezeBox_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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

	g_pPlutoLogger->Write(LV_STATUS, "Selected device (%d: %s) as playback device!",
			pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
			pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	pSlimServerMediaStream = new SlimServerMediaStream( this, pMediaHandlerInfo,
							pMediaDevice,
							pMediaHandlerInfo->m_iPK_DesignObj,
							iPK_Users, st_RemovableMedia, StreamID );

	return pSlimServerMediaStream;
}

SlimServerMediaStream *SlimServer_PlugIn::ConvertToSlimServerMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaStream == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_SLIMSERVER )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a SlimServerMediaStream!").c_str());
		return NULL;
	}

	return static_cast<SlimServerMediaStream *>(pMediaStream);
}

bool SlimServer_PlugIn::StartMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write( LV_STATUS, "SlimServer_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );

	SlimServerMediaStream *pSlimServerMediaStream = NULL;
	if ( (pSlimServerMediaStream = ConvertToSlimServerMediaStream(pMediaStream, "SlimServer_PlugIn::StartMedia(): ")) == NULL )
		return false;

	if ( !pSlimServerMediaStream->m_pMediaDevice_Source || pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Media_Streamers_CONST )
	{
		pSlimServerMediaStream->m_pMediaDevice_Source = FindStreamerDevice();
	}

	if( pSlimServerMediaStream->m_pMediaDevice_Source && pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory == DEVICECATEGORY_Media_Streamers_CONST )
	{
		// Find the destinations for each ent area
		map<int, EntertainArea *>::iterator itEntertainmentAreas;
		for ( itEntertainmentAreas = pSlimServerMediaStream->m_mapEntertainArea.begin(); itEntertainmentAreas != pSlimServerMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
		{
			EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;
			pEntertainArea->m_pMediaDevice_ActiveDest = FindMediaDeviceForEntertainArea(pEntertainArea);
		}

		pSlimServerMediaStream->setIsStreaming();
		StartStreaming(pSlimServerMediaStream);
	}

	string sFileToPlay = pSlimServerMediaStream->GetFilenameToPlay("Empty file name");

	g_pPlutoLogger->Write( LV_STATUS, "SlimServer_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	// HACK: -- todo: get real informations.
	if( pSlimServerMediaStream->m_dequeMediaFile.size()>pSlimServerMediaStream->m_iDequeMediaFile_Pos )
	{
		MediaFile *pMediaFile = pSlimServerMediaStream->m_dequeMediaFile[pSlimServerMediaStream->m_iDequeMediaFile_Pos];
		if( pMediaFile && pMediaFile->m_sDescription.size() )
			pSlimServerMediaStream->m_sMediaDescription = pMediaFile->m_sDescription;
		else
			pSlimServerMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);
	}
	else
		pSlimServerMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);

	mediaURL = sFileToPlay;

	g_pPlutoLogger->Write(LV_WARNING, "Is streaming ?: %d", pSlimServerMediaStream->isStreaming() );

	// Establish streaming configuration first if this applies here
	// This is probably needed only if we are doing a direct playback to the target ent area.
	//	if ( pSlimServerMediaStream->isStreaming() )
 	//		StartStreaming(pSlimServerMediaStream);

	g_pPlutoLogger->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
// 	if( !SendCommand( cmd, &Response ) )
// 		g_pPlutoLogger->Write( LV_CRITICAL, "The player %d (%s) didn't respond to play media command!",
// 					pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
// 					pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_sDescription.c_str());
// 	else
// 		g_pPlutoLogger->Write(LV_STATUS, "The sources device responded to play media command!" );

	// If there are more than 1 song in the queue, we likely added to an existing queue, so we want
	// to refresh=true so any orbiters will re-render the play list
	m_pMedia_Plugin->MediaInfoChanged( pSlimServerMediaStream, pSlimServerMediaStream->m_dequeMediaFile.size()>1 );
	return true;
}

bool SlimServer_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Stopping media in SlimServer_PlugIn!");

	SlimServerMediaStream *pSlimServerMediaStream = NULL;

	if ( (pSlimServerMediaStream = ConvertToSlimServerMediaStream(pMediaStream, "SlimServer_PlugIn::StopMedia() ")) == NULL )
		return false;

	if( pSlimServerMediaStream->isStreaming() )
		StopStreaming(pSlimServerMediaStream,NULL);  // NULL=stop all
	pSlimServerMediaStream->setIsStreaming(false);

	if( !pSlimServerMediaStream->m_pMediaDevice_Source )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Stopping media in SlimServer_PlugIn but mediadevice_source is null");
		return false;
	}
	int PK_Device = pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pSlimServerMediaStream->m_iStreamID_get( );
	int SavedPosition=0;
	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							PK_Device,  		// Send to the device that is actually playing
							StreamID,      		// Send the stream ID that we want to actually stop
							&SavedPosition);


// todo -- temporary hack -- Maybe slim can lockup while trying to stop.  
// Ignore the out paramater until we fix this
delete cmd.m_pcResponse;
cmd.m_pcResponse=NULL;

	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( cmd ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		g_pPlutoLogger->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		pSlimServerMediaStream->GetMediaPosition()->m_iSavedPosition = SavedPosition;
		MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(StreamID);
		if( !pMediaStream || (pSlimServerMediaStream = ConvertToSlimServerMediaStream(pMediaStream, "SlimServer_PlugIn::StopMedia() ")) == NULL )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Stream has vanished or was changed.");
			return false; // It's ok -- the user just stopped it
		}

		pSlimServerMediaStream->GetMediaPosition()->m_iSavedPosition=SavedPosition;
		g_pPlutoLogger->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped at position: %d",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
											pSlimServerMediaStream->GetMediaPosition()->m_iSavedPosition);
	}

	return true;
}

MediaDevice *SlimServer_PlugIn::FindStreamerDevice()
{
	int iStreamerDeviceId = 0;

	if ( (iStreamerDeviceId = m_pRouter->FindClosestRelative(DEVICETEMPLATE_Slim_Server_Streamer_CONST, m_dwPK_Device)) <= 0 )
		return NULL;
	else
		return m_pMedia_Plugin->m_mapMediaDevice_Find(iStreamerDeviceId);
}

bool SlimServer_PlugIn::StopStreaming(SlimServerMediaStream *pSlimServerMediaStream, vector<MediaDevice*> *stopStreamingTargets)
{
	if ( ! pSlimServerMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "SlimServer_PlugIn::StopStreaming() Function was called for a MediaStream that is not streaming currently!");
		return false;
	}

	vector<MediaDevice*>::const_iterator itDevices;
	map<int, EntertainArea *>::iterator itEntertainmentAreas;

	if( stopStreamingTargets )
		itDevices = stopStreamingTargets->begin();
	else
		itEntertainmentAreas = pSlimServerMediaStream->m_mapEntertainArea.begin();

	string strSqueezesToStopList = "";
	while ( true )
	{
		MediaDevice *pDevice;
		if( stopStreamingTargets )
			pDevice = (*itDevices);
		else
			pDevice = (*itEntertainmentAreas).second->m_pMediaDevice_ActiveDest;

		if( pDevice )
			strSqueezesToStopList += StringUtils::itos(pDevice->m_pDeviceData_Router->m_dwPK_Device) + ",";

		if( stopStreamingTargets && ++itDevices == stopStreamingTargets->end() )
			break;
		else if( !stopStreamingTargets && ++itEntertainmentAreas == pSlimServerMediaStream->m_mapEntertainArea.end() )
			break;
	}

	g_pPlutoLogger->Write(LV_WARNING, "SlimServer_PlugIn::StopStreaming() Send stop streaming command to the Streamer with targets: %s!", strSqueezesToStopList.c_str());
	if ( strSqueezesToStopList.size() != 0 )
	{
		// send the StopStreaming command to the SlimServerStreamer
		DCE::CMD_Stop_Streaming stopStreamingCommands(
				m_dwPK_Device,
				pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pSlimServerMediaStream->m_iStreamID_get(),
				strSqueezesToStopList);

		SendCommand(stopStreamingCommands);
	}

	return true;
}

bool SlimServer_PlugIn::StartStreaming(SlimServerMediaStream *pMediaStream)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	map<int, MediaDevice *>::const_iterator itPlaybackDevices;
	map<int, MediaDevice *> mapPlaybackDevices;
	string strTargetDevices = "";

	if ( ! pMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Called SlimServer_PlugIn::StartStreaming but with a MediaStream (streamid: %d) that is non streamable.", pMediaStream->m_iStreamID_get());
		return false;
	}

	if ( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimServer_PlugIn::StartStreaming() was passed a stream with a NULL source device! Ignoring request!.");
		return false;
	}

	for( MapEntertainArea::iterator itEA = pMediaStream->m_mapEntertainArea.begin( );itEA != pMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		GetRenderDevices(pEntertainArea, &mapPlaybackDevices);
	}

	// First get the list of playback devices
	for ( itPlaybackDevices = mapPlaybackDevices.begin(); itPlaybackDevices != mapPlaybackDevices.end(); itPlaybackDevices++ )
		strTargetDevices += StringUtils::itos((*itPlaybackDevices).second->m_pDeviceData_Router->m_dwPK_Device) + ",";

	// virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage);
	itPlaybackDevices = mapPlaybackDevices.begin();

	string resultingURL;
	DCE::CMD_Start_Streaming startStreamingCommand(
					m_dwPK_Device,
					pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					pMediaStream->m_iStreamID_get(),
					strTargetDevices,
					&resultingURL );

	if( !SendCommand(startStreamingCommand) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Send StartStreaming Command failed");
		return false;
	}

g_pPlutoLogger->Write(LV_CRITICAL,"About to call CMD_Play_Media sole master to %d play media within start streaming",pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);

	DCE::CMD_Play_Media cmd(m_dwPK_Device,
							pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
							pMediaStream->GetFilenameToPlay("Empty file name"),
							pMediaStream->m_iPK_MediaType,
							pMediaStream->m_iStreamID_get( ),
							0);//Mihai look into this please pMediaStream->GetMediaPosition()->m_iSavedPosition);

	// No handling of errors (it will in some cases deadlock the router.)
	SendCommand(cmd);
	//QueueMessageToRouter(cmd.m_pMessage);

	g_pPlutoLogger->Write(LV_STATUS, "Established streaming configuration: %d -> [%s]!",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
											strTargetDevices.c_str());
	return true;
}

MediaDevice *SlimServer_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_SqueezeBox_Player_CONST);
	g_pPlutoLogger->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}
