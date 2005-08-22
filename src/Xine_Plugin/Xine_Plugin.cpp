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
	m_iPriority=DATA_Get_Priority();

	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Xine_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	// In our device data we will give ourselves a higher priority and set multipledestinations=false since this 
	// media handler doesn't do multipledestinations, but, if there's only 1 destination, this
	// should be chosen over another handler that does streaming since this is more efficient
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

class MediaStream *Xine_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
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

	pXineMediaStream = new XineMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
							pMediaDevice,
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

	if ( pXineMediaStream->ShouldUseStreaming() )
		return false; // We don't do streaming

	string sFileToPlay = pXineMediaStream->GetFilenameToPlay("Empty file name");

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Plugin::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	MediaFile *pMediaFile = NULL;

	mediaURL = sFileToPlay;

	if ( pXineMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST )
		mediaURL = "dvd:/" + mediaURL;

	g_pPlutoLogger->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d", 
		m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
		pMediaStream->m_iDequeMediaFile_Pos);
	DCE::CMD_Play_Media cmd(m_dwPK_Device,
							pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
							mediaURL,
							pXineMediaStream->m_iPK_MediaType,
							pXineMediaStream->m_iStreamID_get( ),
							pMediaFile ? pMediaFile->m_sStartPosition : "");

	// No handling of errors (it will in some cases deadlock the router.)
	SendCommand(cmd);

	if( pMediaFile )
		pMediaFile->m_sStartPosition=""; // Be sure to reset the start position so next time we start at the beginning of the file if this is in a queue

	g_pPlutoLogger->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);

// 	if( !SendCommand( cmd, &Response ) )
// 		g_pPlutoLogger->Write( LV_CRITICAL, "The player %d (%s) didn't respond to play media command!",
// 					pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
// 					pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_sDescription.c_str());
// 	else
// 		g_pPlutoLogger->Write(LV_STATUS, "The sources device responded to play media command!" );

	// If there are more than 1 song in the queue, we likely added to an existing queue, so we want
	// to refresh=true so any orbiters will re-render the play list
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

	if( !pXineMediaStream->m_pMediaDevice_Source )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Stopping media in Xine_Plugin but mediadevice_source is null");
		return false;
	}
	int PK_Device = pXineMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pXineMediaStream->m_iStreamID_get( );
	string SavedPosition;
	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							PK_Device,  		// Send to the device that is actually playing
							StreamID,      		// Send the stream ID that we want to actually stop
							&SavedPosition);

	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( cmd ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		g_pPlutoLogger->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		pXineMediaStream->m_sLastPosition=SavedPosition;
		if( pXineMediaStream->m_iDequeMediaFile_Pos>=0 && pXineMediaStream->m_iDequeMediaFile_Pos<pXineMediaStream->m_dequeMediaFile.size() )
		{
			pXineMediaStream->m_dequeMediaFile[pXineMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = SavedPosition;
			g_pPlutoLogger->Write( LV_STATUS, "Media stopped at %s",SavedPosition.c_str());
		}

		g_pPlutoLogger->Write( LV_STATUS, "The target device %d responded to stop media command",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

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

		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
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
	XineMediaStream *pXineMediaStream = NULL;
	if ( (pXineMediaStream = ConvertToXineMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find( StreamID ), "Xine_Plugin::MenuOnScreen(): ")) == NULL )
		return false;

	if( bOnOff )
	{
		pXineMediaStream->m_iPK_DesignObj_Remote_After_Menu=pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote;
		pXineMediaStream->m_iPK_DesignObj_RemoteOSD_After_Menu=pXineMediaStream->m_iPK_DesignObj_RemoteOSD;
		pXineMediaStream->m_iPK_DesignObj_Remote_Popup_After_Menu=pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote_Popup;

		pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote=DESIGNOBJ_mnuDVDMenu_CONST;
		pXineMediaStream->m_iPK_DesignObj_RemoteOSD=DESIGNOBJ_dvd_menu_full_screen_CONST;
		pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote_Popup=-1;  // -1 tells orbiter not to remove the popup--this is just temporary
	}
	else
	{
		pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote=pXineMediaStream->m_iPK_DesignObj_Remote_After_Menu;
		pXineMediaStream->m_iPK_DesignObj_RemoteOSD=pXineMediaStream->m_iPK_DesignObj_RemoteOSD_After_Menu;
		pXineMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote_Popup=pXineMediaStream->m_iPK_DesignObj_Remote_Popup_After_Menu;
		pXineMediaStream->m_iPK_DesignObj_Remote_After_Menu=pXineMediaStream->m_iPK_DesignObj_RemoteOSD_After_Menu=pXineMediaStream->m_iPK_DesignObj_Remote_Popup_After_Menu=0;
	}


	g_pPlutoLogger->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pXineMediaStream, pXineMediaStream->m_iStreamID_get( ), pXineMediaStream->m_iPK_MediaType );
	g_pPlutoLogger->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pXineMediaStream->m_mapEntertainArea.size( ) );


	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pXineMediaStream->m_mapEntertainArea.begin( );itEA != pXineMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		g_pPlutoLogger->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			g_pPlutoLogger->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			m_pMedia_Plugin->SetNowPlaying(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
				pXineMediaStream,false);
		}
		m_pMedia_Plugin->WaitForMessageQueue();
g_pPlutoLogger->Write(LV_WARNING, "Sent now playing to %d remoted for on: %d queue",
					  (int) pEntertainArea->m_mapBoundRemote.size( ),(int) bOnOff);
		for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
		{
			BoundRemote *pBoundRemote = ( *itBR ).second;
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
				0,"<%=NP_R%>","","",false,false);
			SendCommand(CMD_Goto_Screen);
		}
	}

	return true;
}

//<-dceag-sample-b->!
//<-dceag-createinst-b->!
