int g_iLastStreamIDPlayed=0;

/*
	Xine_Plugin

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/**
*
* @file Xine_Plugin.h
* @brief source file for the Xine_Plugin class
* @author
*
*/

//<-dceag-d-b->
#include "Xine_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Xine_Plugin::Xine_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Xine_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Xine_Plugin::~Xine_Plugin()
//<-dceag-dest-e->
{

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Xine_Plugin::Register()
//<-dceag-reg-e->
{
	m_pMedia_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Media_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Xine plug in cannot find media handler %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
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
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Xine_Player_CONST);
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_SqueezeBox_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	// We will give ourselves a higher priority and set multipledestinations=false since this 
	// media handler doesn't do multipledestinations, but, if there's only 1 destination, this
	// should be chosen over another handler that does streaming since this is more efficient
	m_iPriority=7;
	for(size_t s=0;s<m_vectMediaHandlerInfo.size();++s)
		m_vectMediaHandlerInfo[s]->m_bMultipleDestinations=false;

	RegisterMsgInterceptor(( MessageInterceptorFn )( &Xine_Plugin::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

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
void Xine_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Xine_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNHANDLED";
}

class MediaStream *Xine_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	XineMediaStream *pXineMediaStream;
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
		 pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Xine_Player_CONST &&
	     pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_SqueezeBox_Player_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_Xine_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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

	pXineMediaStream = new XineMediaStream( this, pMediaHandlerInfo,
							pMediaDevice,
							pMediaHandlerInfo->m_iPK_DesignObj,
							iPK_Users, st_RemovableMedia, StreamID );

	// if the source device is a disk drive then we can't move this media stream around.
	if ( pMediaDevice_PassedIn && pMediaDevice_PassedIn->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Disk_Drive_CONST )
		pXineMediaStream->setIsMovable(false);

	return pXineMediaStream;
}

XineMediaStream *Xine_Plugin::ConvertToXineMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaStream == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_XINE )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a XineMediaStream!").c_str());
		return NULL;
	}

	return static_cast<XineMediaStream*>(pMediaStream);
}

bool Xine_Plugin::StartMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
g_iLastStreamIDPlayed=pMediaStream->m_iStreamID_get();

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Plugin::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );

	XineMediaStream *pXineMediaStream = NULL;
	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::StartMedia(): ")) == NULL )
		return false;

	if ( pXineMediaStream->ShouldUseStreaming() &&
		(!pXineMediaStream->m_pMediaDevice_Source || pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Media_Streamers_CONST) )
	{
		pXineMediaStream->m_pMediaDevice_Source = FindStreamerDevice();
	}

	if( pXineMediaStream->m_pMediaDevice_Source && pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory == DEVICECATEGORY_Media_Streamers_CONST )
	{
		// Find the destinations for each ent area
		map<int, EntertainArea *>::iterator itEntertainmentAreas;
		for ( itEntertainmentAreas = pXineMediaStream->m_mapEntertainArea.begin(); itEntertainmentAreas != pXineMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
		{
			EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;
			pEntertainArea->m_pMediaDevice_ActiveDest = FindMediaDeviceForEntertainArea(pEntertainArea);
		}

		pXineMediaStream->setIsStreaming();
		StartStreaming(pXineMediaStream);
	}

	string sFileToPlay = pXineMediaStream->GetFilenameToPlay("Empty file name");

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Plugin::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	if ( pXineMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST )
	{
		string sPK_Orbiter = pMediaStream->GetAllOSD();
		if( sPK_Orbiter.size() )
			m_pOrbiter_Plugin->DisplayMessageOnOrbiter(sPK_Orbiter,"<%=T" + StringUtils::itos(TEXT_Checking_drive_CONST) + "%>",false,10,true);

		if( pXineMediaStream->m_dequeMediaFile.size() )
		{
			MediaFile *pMediaFile = pXineMediaStream->m_dequeMediaFile[0];
			pXineMediaStream->m_sMediaDescription = pMediaFile->m_sDescription.size() ? pMediaFile->m_sDescription : FileUtils::FilenameWithoutPath(pMediaFile->m_sFilename);
		}

		g_pPlutoLogger->Write(LV_STATUS, "Got pluto DVD media type");
		// Find a disk Drive in one of the entertainment areas.// Wait(true);
		map<int, EntertainArea *>::iterator itEntertainmentAreas;

		bool bFound = false ;

		for ( itEntertainmentAreas = pXineMediaStream->m_mapEntertainArea.begin(); ! bFound && itEntertainmentAreas != pXineMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
		{
			EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;

			map<int, MediaDevice *>::iterator itDevice;
			for( itDevice = pEntertainArea->m_mapMediaDevice.begin( ); ! bFound && itDevice != pEntertainArea->m_mapMediaDevice.end( ); ++itDevice )
			{
				class MediaDevice *pMediaDevice = ( *itDevice ).second;

				if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory==DEVICECATEGORY_Disc_Drives_CONST )
				{
					DCE::CMD_Mount_Disk_Image mountCommand(
								pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
								pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
								sFileToPlay,
								&mediaURL );

					// TODO: read the response ( see if the mount was succesfull ) and continue if not.
					if ( SendCommand( mountCommand, &Response ) )
					{
						bFound = true;
						break;
					}
					else
					{
						string sPK_Orbiter = pMediaStream->GetAllOSD();
						if( sPK_Orbiter.size() )
							m_pOrbiter_Plugin->DisplayMessageOnOrbiter(sPK_Orbiter,"<%=T" + StringUtils::itos(TEXT_Cannot_play_DVD_CONST) + "%>",false,20,true);

						return false;
					}

					g_pPlutoLogger->Write(LV_CRITICAL, "Disk drive mount command didn't complete succesfully (response %s). Error message: %s", Response.c_str(), mediaURL.c_str( ) );
				}
			}
			g_pPlutoLogger->Write( LV_STATUS, "Media device %d got back URL: %s", pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, mediaURL.c_str( ) );
		}

		if ( !bFound ) // we didn;t find a disk drive which was able to mount hte images
		{
			string sPK_Orbiter = pMediaStream->GetAllOSD();
			if( sPK_Orbiter.size() )
				m_pOrbiter_Plugin->DisplayMessageOnOrbiter(sPK_Orbiter,"Error -- no drive",false,30,true);
			return false;
		}
	}
	else
	{
		// HACK: -- todo: get real informations.
		if( pXineMediaStream->m_dequeMediaFile.size()>pXineMediaStream->m_iDequeMediaFile_Pos )
		{
			MediaFile *pMediaFile = pXineMediaStream->m_dequeMediaFile[pXineMediaStream->m_iDequeMediaFile_Pos];
			if( pMediaFile && pMediaFile->m_sDescription.size() )
				pXineMediaStream->m_sMediaDescription = pMediaFile->m_sDescription;
			else
				pXineMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);
		}
		else
			pXineMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);

		mediaURL = sFileToPlay;
	}

	g_pPlutoLogger->Write(LV_WARNING, "Is streaming ?: %d", pXineMediaStream->isStreaming() );

	// Establish streaming configuration first if this applies here
	// This is probably needed only if we are doing a direct playback to the target ent area.
	//	if ( pXineMediaStream->isStreaming() )
 	//		StartStreaming(pXineMediaStream);

	// If this is streaming, we already called PlayMedia within StartStreaming above
	if( !pXineMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d saved pos %d", 
			m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
			pMediaStream->m_iDequeMediaFile_Pos, pXineMediaStream->GetMediaPosition()->m_iSavedPosition);
		DCE::CMD_Play_Media cmd(m_dwPK_Device,
								pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
								mediaURL,
								pXineMediaStream->m_iPK_MediaType,
								pXineMediaStream->m_iStreamID_get( ),
								pXineMediaStream->GetMediaPosition()->m_iSavedPosition);

		// No handling of errors (it will in some cases deadlock the router.)
		SendCommand(cmd);
	}

	g_pPlutoLogger->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
// 	if( !SendCommand( cmd, &Response ) )
// 		g_pPlutoLogger->Write( LV_CRITICAL, "The player %d (%s) didn't respond to play media command!",
// 					pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
// 					pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_sDescription.c_str());
// 	else
// 		g_pPlutoLogger->Write(LV_STATUS, "The sources device responded to play media command!" );

	// If there are more than 1 song in the queue, we likely added to an existing queue, so we want
	// to refresh=true so any orbiters will re-render the play list
	m_pMedia_Plugin->MediaInfoChanged( pXineMediaStream, pXineMediaStream->m_dequeMediaFile.size()>1 );
	return true;
}

bool Xine_Plugin::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Stopping media in Xine_Plugin!");

if( pMediaStream->m_iStreamID_get()<g_iLastStreamIDPlayed )
{
int k=2;
}
	XineMediaStream *pXineMediaStream = NULL;

	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::StopMedia() ")) == NULL )
		return false;

	if( pXineMediaStream->isStreaming() )
		StopStreaming(pXineMediaStream,NULL);  // NULL=stop all
	pXineMediaStream->setIsStreaming(false);

	if( !pXineMediaStream->m_pMediaDevice_Source )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Stopping media in Xine_Plugin but mediadevice_source is null");
		return false;
	}
	int PK_Device = pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pXineMediaStream->m_iStreamID_get( );
	int SavedPosition=0;
	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							PK_Device,  		// Send to the device that is actually playing
							StreamID,      		// Send the stream ID that we want to actually stop
							&SavedPosition);


// todo -- temporary hack -- Xine can lockup while trying to stop.  
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
		pXineMediaStream->GetMediaPosition()->m_iSavedPosition = SavedPosition;
		MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(StreamID);
		if( !pMediaStream || (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::StopMedia() ")) == NULL )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Stream has vanished or was changed.");
			return false; // It's ok -- the user just stopped it
		}

		pXineMediaStream->GetMediaPosition()->m_iSavedPosition=SavedPosition;
		g_pPlutoLogger->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped at position: %d",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
											pXineMediaStream->GetMediaPosition()->m_iSavedPosition);
	}

	return true;
}

MediaDevice *Xine_Plugin::FindStreamerDevice()
{
	int iStreamerDeviceId = 0;

	if ( (iStreamerDeviceId = m_pRouter->FindClosestRelative(DEVICETEMPLATE_Slim_Server_Streamer_CONST, m_dwPK_Device)) <= 0 )
		return NULL;
	else
		return m_pMedia_Plugin->m_mapMediaDevice_Find(iStreamerDeviceId);
}

bool Xine_Plugin::StopStreaming(XineMediaStream *pXineMediaStream, vector<MediaDevice*> *stopStreamingTargets)
{
	if ( ! pXineMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Xine_Plugin::StopStreaming() Function was called for a MediaStream that is not streaming currently!");
		return false;
	}

	vector<MediaDevice*>::const_iterator itDevices;
	map<int, EntertainArea *>::iterator itEntertainmentAreas;

	if( stopStreamingTargets )
		itDevices = stopStreamingTargets->begin();
	else
		itEntertainmentAreas = pXineMediaStream->m_mapEntertainArea.begin();

	string strSqueezesToStopList = "";
	while ( true )
	{
		MediaDevice *pDevice;
		if( stopStreamingTargets )
			pDevice = (*itDevices);
		else
			pDevice = (*itEntertainmentAreas).second->m_pMediaDevice_ActiveDest;

		if ( pDevice && pDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Xine_Player_CONST )
		{
			int ignoredPosition; // we will actually query the Slim Server about this
			// send them separately since the devices can be in a very different places.
			DCE::CMD_Stop_Media stopMediaCommand(
					m_dwPK_Device,
					pDevice->m_pDeviceData_Router->m_dwPK_Device,
					pXineMediaStream->m_iStreamID_get(), &ignoredPosition);
			SendCommand(stopMediaCommand);
		}
		else if( pDevice )
			strSqueezesToStopList += StringUtils::itos(pDevice->m_pDeviceData_Router->m_dwPK_Device) + ",";

		if( stopStreamingTargets && ++itDevices == stopStreamingTargets->end() )
			break;
		else if( !stopStreamingTargets && ++itEntertainmentAreas == pXineMediaStream->m_mapEntertainArea.end() )
			break;
	}

	g_pPlutoLogger->Write(LV_WARNING, "Xine_Plugin::StopStreaming() Send stop streaming command to the Streamer with targets: %s!", strSqueezesToStopList.c_str());
	if ( strSqueezesToStopList.size() != 0 )
	{
		// send the StopStreaming command to the SlimServerStreamer
		DCE::CMD_Stop_Streaming stopStreamingCommands(
				m_dwPK_Device,
				pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pXineMediaStream->m_iStreamID_get(),
				strSqueezesToStopList);

		SendCommand(stopStreamingCommands);
	}

	return true;
}

bool Xine_Plugin::StartStreaming(XineMediaStream *pMediaStream)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	map<int, MediaDevice *>::const_iterator itPlaybackDevices;
	map<int, MediaDevice *> mapPlaybackDevices;
	string strTargetDevices = "";

	if ( ! pMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Called Xine_Plugin::StartStreaming but with a MediaStream (streamid: %d) that is non streamable.", pMediaStream->m_iStreamID_get());
		return false;
	}

	if ( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Xine_Plugin::StartStreaming() was passed a stream with a NULL source device! Ignoring request!.");
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
	while ( itPlaybackDevices != mapPlaybackDevices.end() )
	{
		MediaDevice *pMediaDevice = (*itPlaybackDevices).second;

		// when we are starting streaming on xine players we need to point them to the slim server device
		if ( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Xine_Player_CONST )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Sending CMD_Play_Media slim server connection command to the xine player");

			DCE::CMD_Play_Media playMediaCommand(
					m_dwPK_Device,
					pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
					"slim://" + pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->GetIPAddress() + "/",
					pMediaStream->m_iPK_MediaType,
					pMediaStream->m_iStreamID_get(), 0);
			SendCommand(playMediaCommand);
g_pPlutoLogger->Write(LV_CRITICAL,"Finished sending playback to xine %d",pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
		}

		itPlaybackDevices++;
	}

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

MediaDevice *Xine_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Xine_Player_CONST);

	g_pPlutoLogger->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
	if ( pMediaDevice == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Could not find a Xine Player device (with device template id: %d) in the entertainment area: %d. Looking for a squeeze box.",
				DEVICETEMPLATE_Xine_Player_CONST,
				pEntertainArea->m_iPK_EntertainArea);

		pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_SqueezeBox_Player_CONST);

		if ( pMediaDevice == NULL )
		{
			g_pPlutoLogger->Write(LV_WARNING, "No squeeze box device (device template id: %d) was found in the entertainment area: %d. Ignoring this ent area ",
				DEVICETEMPLATE_SqueezeBox_Player_CONST,
				pEntertainArea->m_iPK_EntertainArea);

			return NULL;
		}
	}

	g_pPlutoLogger->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}

bool Xine_Plugin::BroadcastMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	return true;
}

bool Xine_Plugin::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	/** Confirm this is from one of ours */
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Xine_Player_CONST )
		return false; /** Some other media player. We only know xine's menu handling */

	int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";

	/** Find the stream */
	MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID );
	if( !pMediaStream )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "DVD Menu appeared for stream %d, but I can't find it", StreamID );
		return false; /** Maybe somebody else knows what to do with it */
	}

	string sOnScreenOrbiters="", sOtherOrbiters="";
	map<int, OH_Orbiter *> mapOH_Orbiter; 				/** Use a map so we don't have duplicates */

	g_pPlutoLogger->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pMediaStream, pMediaStream->m_iStreamID_get( ), pMediaStream->m_iPK_MediaType );
	g_pPlutoLogger->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pMediaStream->m_mapEntertainArea.size( ) );

	/** We're going to send a message to all the orbiters that are bound to remotes in any of the entertainment areas */
	for( MapEntertainArea::iterator itEA = pMediaStream->m_mapEntertainArea.begin( );itEA != pMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		g_pPlutoLogger->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
		for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
		{
			BoundRemote *pBoundRemote = ( *itBR ).second;
			g_pPlutoLogger->Write(LV_STATUS, "Processing bound remote: for orbiter: %d", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			mapOH_Orbiter[pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device] = pBoundRemote->m_pOH_Orbiter;
		}
	}

	for( map<int, OH_Orbiter *>::iterator itOH=mapOH_Orbiter.begin( );itOH!=mapOH_Orbiter.end( );++itOH )
	{
		OH_Orbiter *pOH_Orbiter = ( *itOH ).second;

		// the old code assumed that the on screen orbiters were all the oribters controlled by a MediaDirector device. We will make the condition harder. See below
		// If the orbiter is the source device parent (since we know that hte source device is a Xine Player) we can assume that this is the only OnScreen Orbiter.
		if ( pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia && pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_DeviceCategory==DEVICECATEGORY_Media_Director_CONST
			&& pDeviceFrom->m_pDevice_ControlledVia && pDeviceFrom->m_pDevice_ControlledVia->m_dwPK_Device == pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device )
			sOnScreenOrbiters += StringUtils::itos( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device ) + ", ";
		else
			sOtherOrbiters += StringUtils::itos( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device ) + ", ";
	}

	g_pPlutoLogger->Write( LV_STATUS, "osd: %s other: %s",sOnScreenOrbiters.c_str(),sOtherOrbiters.c_str());

	if( sOnScreenOrbiters.size( )==0 && sOtherOrbiters.size( )==0 )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Got a menu goto, but there are no related orbiters" );
		return false;
	}

	if( bOnOff )
	{
		if( sOtherOrbiters.size( ) )
		{
			/** Send all the non onscreen orbiters to the dvd menu */
			DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOtherOrbiters, 0, StringUtils::itos( DESIGNOBJ_mnuDVDMenu_CONST ), "", "", false, true );
			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sOtherOrbiters, VARIABLE_PK_Device_CONST, StringUtils::itos( pMessage->m_dwPK_Device_From ) );

			CMD_Set_Variable_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Goto_Screen_DL.m_pMessage );
			QueueMessageToRouter( CMD_Set_Variable_DL.m_pMessage );
		}

		if( sOnScreenOrbiters.size( ) )
		{
			/** If it's an on-screen orbiter, just send it to the DVD menu full screen */
			DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOnScreenOrbiters, 0, StringUtils::itos( DESIGNOBJ_dvd_menu_full_screen_CONST ), "", "", false, true );
			SendCommand( CMD_Goto_Screen_DL );
		}
	}
	else
	{
		// send them back if they were at the dvd menu screen
		if( sOtherOrbiters.size( ) )
		{
			DCE::CMD_Go_back_DL CMD_Go_back_DL( m_dwPK_Device, sOtherOrbiters, StringUtils::itos( DESIGNOBJ_mnuDVDMenu_CONST ), "" );
			SendCommand( CMD_Go_back_DL );
		}

		if ( sOnScreenOrbiters.size() )
		{
			DCE::CMD_Go_back_DL CMD_Go_back_DL( m_dwPK_Device, sOnScreenOrbiters, StringUtils::itos( DESIGNOBJ_dvd_menu_full_screen_CONST ), "" );
			SendCommand( CMD_Go_back_DL );
		}
	}

	return true;
}

//<-dceag-sample-b->!
//<-dceag-createinst-b->!
