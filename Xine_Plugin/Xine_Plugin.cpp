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

	m_pMedia_Plugin->RegisterMediaPlugin( this, this, DEVICETEMPLATE_Xine_Player_CONST, true );
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, DEVICETEMPLATE_SqueezeBox_Player_CONST, true );

	RegisterMsgInterceptor(( MessageInterceptorFn )( &Xine_Plugin::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

	/**
	// The constructor for this class registers itself, so we don't worry about deleting it or doing anything with it

	// We can play cd's, both discs and stored media
	MediaHandlerInfo *pMediaHandlerInfo = new MediaHandlerInfo( this, MEDIATYPE_pluto_CD_CONST, DEVICETEMPLATE_Xine_Player_CONST, canjumptrue, usesdcetrue );

	// We can play dvd's, both discs and stored media
	pMediaHandlerInfo = new MediaHandlerInfo( this, MEDIATYPE_pluto_DVD_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );

	// We can play stored audio and video of all types
	pMediaHandlerInfo = new MediaHandlerInfo( this, MEDIATYPE_pluto_StoredAudio_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );
	pMediaHandlerInfo = new MediaHandlerInfo( this, MEDIATYPE_pluto_StoredAudio_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );
	*/
	/** @todo list<int> m_PK_Broadcast_Send; // What broadcast formats it supports to send */
	/** @todo list<int> m_PK_Broadcast_Receive;  // What broadcast formats it supports to receive */

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

class MediaStream *Xine_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, class EntertainArea *pEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	XineMediaStream *pXineMediaStream;
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( pEntertainArea == NULL && pMediaDevice == NULL )
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

	if ( ! pMediaDevice && (pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea)) == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Selected device (%d) as playback device!", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);

	pXineMediaStream = new XineMediaStream( this, pMediaHandlerInfo,
							pMediaDevice->m_pDeviceData_Router,
							pMediaHandlerInfo->m_iPK_DesignObj,
							iPK_Users, st_RemovableMedia, StreamID );

	pXineMediaStream->SetPlaybackDeviceForEntArea(pEntertainArea->m_iPK_EntertainArea, pMediaDevice);
	if ( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SqueezeBox_Player_CONST )
	{
		pXineMediaStream->m_pDeviceData_Router_Source = FindStreamerDevice();
		pXineMediaStream->setIsStreaming();
	}

	// if the source device is a disk drive then we can't move this media stream around.
	if ( pMediaDevice_PassedIn && pMediaDevice_PassedIn->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Disk_Drive_CONST )
		pXineMediaStream->setIsMovable(false);

	return pXineMediaStream;
}

XineMediaStream *Xine_Plugin::ConvertToXineMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
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
	g_pPlutoLogger->Write( LV_STATUS, "Starting media stream playback--sending command, waiting for response" );

	XineMediaStream *pXineMediaStream = NULL;
	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::StartMedia(): ")) == NULL )
		return false;

	string sFileToPlay = pXineMediaStream->GetFilenameToPlay("Empty file name");

	g_pPlutoLogger->Write( LV_STATUS, "Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	if ( pXineMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST )
	{
		pXineMediaStream->m_sMediaDescription = "Media Desc";

		g_pPlutoLogger->Write(LV_STATUS, "Got pluto DVD media type");
		// Find a disk Drive in one of the entertainment areas.
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
								pXineMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
								pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
								sFileToPlay,
								&mediaURL );

					// TODO: read the response ( see if the mount was succesfull ) and continue if not.
					if ( SendCommand( mountCommand, &Response ) )
					{
						g_pPlutoLogger->Write(LV_STATUS, "And again Here ");
						bFound = true;
						break;
					}
					else
					{
						g_pPlutoLogger->Write(LV_CRITICAL, "Failed to receive response from the disk drive device!. Response was: \"%s\"!", Response.c_str( ) );
					}

					g_pPlutoLogger->Write(LV_STATUS, "Got response from the disk drive: %s", mediaURL.c_str() );
				}
			}
			g_pPlutoLogger->Write( LV_CRITICAL, "Media device %d got back URL: %s", pXineMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device, mediaURL.c_str( ) );
		}
	}
	else
	{
		// HACK: -- todo: get real informations.
		pXineMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);
		mediaURL = sFileToPlay;
	}

	// Establish streaming configuration first if this applies here
	if ( pXineMediaStream->isStreaming() )
		StartStreaming(pXineMediaStream);

	DCE::CMD_Play_Media cmd(m_dwPK_Device,
							pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
							mediaURL,
							pXineMediaStream->m_iPK_MediaType,
							pXineMediaStream->m_iStreamID_get( ),
							pXineMediaStream->GetMediaPosition()->m_iSavedPosition);

	// TODO: Handle error more gracefully.
	if( !SendCommand( cmd, &Response ) )
		g_pPlutoLogger->Write( LV_CRITICAL, "The player %d () didn't respond to play media command!",
					pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
					pMediaStream->m_pDeviceData_Router_Source->m_sDescription.c_str());
	else
		g_pPlutoLogger->Write( LV_STATUS, "Xine player responded to play media command!" );

	m_pMedia_Plugin->MediaInfoChanged( pXineMediaStream );
	return true;
}

bool Xine_Plugin::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Stopping media in Xine_Plugin!");

	XineMediaStream *pXineMediaStream = NULL;

	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::StopMedia() ")) == NULL )
		return false;

	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							pXineMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,  // Send to the device that is actually playing
							pXineMediaStream->m_iStreamID_get( ),       // Send the stream ID that we want to actually stop
							&pXineMediaStream->GetMediaPosition()->m_iSavedPosition);

	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( cmd, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		g_pPlutoLogger->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device );
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped at position: %d",
											pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
											pXineMediaStream->GetMediaPosition()->m_iSavedPosition);

	return true;
}

/**
* @brief Called by the Media Plugin when we need to move media around in entertainment areas. The media plugin will pass 3 lists:
*          < listStart: the list on which media needs to be started. >
*          < listStop: the list on which media needs to be stopped >
*          < listChange: the list on which media needs to be changed (if we move the media from one place to another it might be possible that the actual source of the stream to change). >
*/
bool Xine_Plugin::MoveMedia(class MediaStream *pMediaStream, list<EntertainArea*> &listStart, list<EntertainArea *> &listStop, list<EntertainArea *> &listChange)
{
	map<int, EntertainArea *>::const_iterator itIntToEntArea;
	list<EntertainArea *>::const_iterator itEntArea;
	list<MediaDevice *> startDevices;
	XineMediaStream *pXineMediaStream;
	MediaDevice *pMediaDevice;

	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::MoveMedia() ")) == NULL )
		return false;

	// walk the start list and put the devices in the current devices list.
	// We don't need to do the same with the change list since we already have them in the players list.
	for ( itEntArea = listStart.begin(); itEntArea != listStart.end(); itEntArea++ )
		if ( (pMediaDevice = FindMediaDeviceForEntertainArea(*itEntArea)) != NULL )
		{
			pXineMediaStream->SetPlaybackDeviceForEntArea((*itEntArea)->m_iPK_EntertainArea, pMediaDevice);

			pXineMediaStream->m_mapEntertainArea[(*itEntArea)->m_iPK_EntertainArea] = *itEntArea;
			(*itEntArea)->m_pMediaStream = pXineMediaStream;
		}

	// we will stop the media differently if we are streaming and if we have only one playing device
	// this assumes that the Media_Plugin is behaving correctly. When i get a a MoveMedia and i have a non
	// streaming XineStream it means that is playing on a single XinePlayer device and the m_pMediaSourceDevice is
	// target device also. Also it assumes that the listStop has has one.
	if ( ! pXineMediaStream->isStreaming() )
		StopMedia(pMediaStream);

	// if we are currently streaming then we need a much more complicated approach.
	// Iterate all the stop devices and tell the source device to stop streaming to them
	for ( itEntArea = listStop.begin(); itEntArea != listStop.end(); itEntArea++ )
	{
		if ( (pMediaDevice = pXineMediaStream->GetPlaybackDeviceForEntArea((*itEntArea)->m_iPK_EntertainArea)) == NULL)
			continue;

		// only send the command to the streamer if we were actually streaming.
		if ( pXineMediaStream->isStreaming() )
		{
			/** @todo: Send a message to the streamer to stop playback on this device. */
			g_pPlutoLogger->Write(LV_STATUS, "Should send a message to stop playback on device: %d!", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
		}

		// remove this media stream from the target ent area.
		pXineMediaStream->m_mapEntertainArea.erase((*itEntArea)->m_iPK_EntertainArea);
		(*itEntArea)->m_pMediaStream = NULL;

		// remove the devices also from the XineMediaStream data structure
		pXineMediaStream->SetPlaybackDeviceForEntArea((*itEntArea)->m_iPK_EntertainArea, NULL);
	}

	// in this case we need to locate the streaming device (because we need to enable streaming and the streaming device was not found in a previous call)
	if ( pXineMediaStream->ShouldUseStreaming() && ! pXineMediaStream->isStreaming() )
	{
		// int iStreamingDeviceId;
		g_pPlutoLogger->Write(LV_STATUS, "Enabling streaming for stream: %d", pXineMediaStream->m_iStreamID_get());

		pXineMediaStream->m_pDeviceData_Router_Source = FindStreamerDevice();
		pXineMediaStream->setIsStreaming();

		// This is a big error. We can't actually move media in multiple rooms if we don't have a proper streamer.
		if ( pXineMediaStream->m_pDeviceData_Router_Source == NULL )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,
				"Streaming is required but not streaming device was located. Please make sure that you have a device with template id %d as a child of the dcerouter machine",
				DEVICETEMPLATE_Slim_Server_Streamer_CONST);

			return false;
		}
		else
			g_pPlutoLogger->Write(LV_STATUS, "Found streaming device %d", pXineMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device);
	}

	// in this case we need to disable because it really is only one device in the target list
	if ( ! pXineMediaStream->ShouldUseStreaming() && pXineMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Disabling streaming for stream: %d", pXineMediaStream->m_iStreamID_get());
		pXineMediaStream->setIsStreaming(false);

		MediaDevice *pMediaDevice;
		if ( pXineMediaStream->m_mapEntertainArea.size() == 1 )
		{
			itIntToEntArea = pXineMediaStream->m_mapEntertainArea.begin();

			pMediaDevice = pXineMediaStream->GetPlaybackDeviceForEntArea((*itIntToEntArea).second->m_iPK_EntertainArea);
		}

		pXineMediaStream->m_pDeviceData_Router_Source = pMediaDevice ? pMediaDevice->m_pDeviceData_Router : NULL;
	}

	// Now there are two conditions which can be true.
	// 	Either: pXineMediaStream->ShouldUseStreaming() == true && pXineMediaStream->isStreaming() == true
	// 	Or: 	pXineMediaStream->ShouldUseStreaming() != true && pXineMediaStream->isStreaming() != true
	// 		We can't have anything else since we have dealt with those situations above.

	// if not streaming then:
	if ( ! pXineMediaStream->isStreaming() )
		// start streaming like you would normally do.
		return StartMedia(pXineMediaStream);

	return StartStreaming(pXineMediaStream);
}

DeviceData_Router *Xine_Plugin::FindStreamerDevice()
{
	int iStreamerDeviceId = 0;

	if ( (iStreamerDeviceId = m_pRouter->FindClosestRelative(DEVICETEMPLATE_Slim_Server_Streamer_CONST, m_dwPK_Device)) <= 0 )
		return NULL;
	else
		return m_pRouter->m_mapDeviceData_Router_Find(iStreamerDeviceId);
}

bool Xine_Plugin::StartStreaming(XineMediaStream *pMediaStream)
{
	map<int, MediaDevice *>::const_iterator itPlaybackDevices;
	map<int, MediaDevice *> mapPlaybackDevices;
	string strTargetDevices = "";

	if ( ! pMediaStream->isStreaming() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Called Xine_Plugin::StartStreaming but with a MediaStream (streamid: %d) )that is non streamable.", pMediaStream->m_iStreamID_get());
		return false;
	}

	if ( pMediaStream->m_pDeviceData_Router_Source == NULL )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Xine_Plugin::StartStreaming() was passed a stream with a NULL source device! Ignoring request!.");
		return false;
	}

	GetRenderDevices(pMediaStream, &mapPlaybackDevices);

	itPlaybackDevices = mapPlaybackDevices.begin();
	while ( itPlaybackDevices != mapPlaybackDevices.end() )
	{
		strTargetDevices += StringUtils::itos((*itPlaybackDevices).second->m_pDeviceData_Router->m_dwPK_Device) + ",";
		itPlaybackDevices++;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Established streaming configuration: %d -> [%s]!",
											pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
											strTargetDevices.c_str());

	string resultingURL;
	DCE::CMD_Start_Streaming startStreamingCommand(
					m_dwPK_Device,
					pMediaStream->m_pDeviceData_Router_Source->m_dwPK_Device,
					pMediaStream->m_iStreamID_get(),
					strTargetDevices,
					&resultingURL );

	return SendCommand(startStreamingCommand);
}

MediaDevice *Xine_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Xine_Player_CONST);

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

	return pMediaDevice;
}

bool Xine_Plugin::BroadcastMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	return true;
}

void Xine_Plugin::GetRenderDevices(MediaStream *pMediaStream, map<int,MediaDevice *> *pmapMediaDevice)
{
	XineMediaStream *pXineMediaStream;

	if ( (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::GetRenderDevices() ")) == NULL )
		return;

	pXineMediaStream->GetRenderDevices(pmapMediaDevice);
}

bool Xine_Plugin::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	g_pPlutoLogger->Write(LV_STATUS, "Hannibal is here");
	/** Confirm this is from one of ours */
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_Xine_Player_CONST )
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
	map<int, OH_Orbiter *> mapOH_Orbiter; /** Use a map so we don't have duplicates */

	g_pPlutoLogger->Write( LV_STATUS, "Mediastream %p on menu id: %d type %d", pMediaStream, pMediaStream->m_iStreamID_get( ), pMediaStream->m_iPK_MediaType );
	g_pPlutoLogger->Write( LV_STATUS, "Mediastream mapea size %d", pMediaStream->m_mapEntertainArea.size( ) );

	/** We're going to send a message to all the orbiters that are bound to remotes in any of the entertainment areas */
	for( MapEntertainArea::iterator itEA = pMediaStream->m_mapEntertainArea.begin( );itEA != pMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		g_pPlutoLogger->Write( LV_STATUS, "Mediastream ea %p %d", pEntertainArea, pEntertainArea->m_iPK_EntertainArea );
		for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
		{
			BoundRemote *pBoundRemote = ( *itBR ).second;
			mapOH_Orbiter[pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device] = pBoundRemote->m_pOH_Orbiter;
		}
	}

	for( map<int, OH_Orbiter *>::iterator itOH=mapOH_Orbiter.begin( );itOH!=mapOH_Orbiter.end( );++itOH )
	{// TODO: handle failure when sending the command. This is ignored now.
		OH_Orbiter *pOH_Orbiter = ( *itOH ).second;
		if( pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia && pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_DeviceCategory==DEVICECATEGORY_Media_Director_CONST )
			sOnScreenOrbiters += StringUtils::itos( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device ) + ", ";
		else
			sOtherOrbiters += StringUtils::itos( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device ) + ", ";
	}

	if( sOnScreenOrbiters.size( )==0 && sOtherOrbiters.size( )==0 )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Got a menu goto, but there are no related orbiters" );
		return false;
	}

	if( bOnOff )
	{
		if( sOtherOrbiters.size( ) )
		{
			/** Send all the orbiters to the dvd menu */
			// DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOtherOrbiters, 0, StringUtils::itos( /*DESIGNOBJ_dvd_menu_CONST HACK -- todo*/1 ), "", "", false );
			DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOtherOrbiters, 0, StringUtils::itos( DESIGNOBJ_mnuDVDMenu_CONST ), "", "", false );
			DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL( m_dwPK_Device, sOtherOrbiters, VARIABLE_PK_Device_CONST, StringUtils::itos( pMessage->m_dwPK_Device_From ) );

			CMD_Set_Variable_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Goto_Screen_DL.m_pMessage );
			QueueMessageToRouter( CMD_Set_Variable_DL.m_pMessage );
		}

		if( sOnScreenOrbiters.size( ) )
		{
			/** If it's an on-screen orbiter, just send it to the full screen menu */
//            int DESIGNOBJ_full_screen_CONST=1; /** @todo - hack */
//             DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOnScreenOrbiters, 0, StringUtils::itos( DESIGNOBJ_full_screen_CONST ), "", "", false );
			DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL( m_dwPK_Device, sOnScreenOrbiters, 0, StringUtils::itos( DESIGNOBJ_mnuDVDMenu_CONST ), "", "", false );
			SendCommand( CMD_Goto_Screen_DL );
		}
	}
	else if( sOtherOrbiters.size( ) )
	{
		DCE::CMD_Go_back_DL CMD_Go_back_DL( m_dwPK_Device, sOtherOrbiters, StringUtils::itos( DESIGNOBJ_mnuDVDMenu_CONST ), "" );
		SendCommand( CMD_Go_back_DL );
	}

	return true;
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Xine_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->
//<-dceag-createinst-b->!
