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
Xine_Plugin::Xine_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Xine_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

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

  RegisterMsgInterceptor(( MessageInterceptorFn )( &Xine_Plugin::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );
   
  /**
  // The constructor for this class registers itself, so we don't worry about deleting it or doing anything with it

  // We can play cd's, both discs and stored media
  MediaPluginInfo *pMediaPluginInfo = new MediaPluginInfo( this, MEDIATYPE_pluto_CD_CONST, DEVICETEMPLATE_Xine_Player_CONST, canjumptrue, usesdcetrue );

  // We can play dvd's, both discs and stored media
  pMediaPluginInfo = new MediaPluginInfo( this, MEDIATYPE_pluto_DVD_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );

  // We can play stored audio and video of all types
  pMediaPluginInfo = new MediaPluginInfo( this, MEDIATYPE_pluto_StoredAudio_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );
  pMediaPluginInfo = new MediaPluginInfo( this, MEDIATYPE_pluto_StoredAudio_CONST, DEVICETEMPLATE_Xine_Player_CONST, true, true );
*/
  /** @todo list<int> m_PK_Broadcast_Send; // What broadcast formats it supports to send */
  /** @todo list<int> m_PK_Broadcast_Receive;  // What broadcast formats it supports to receive */

  return Connect( );
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
    sCMD_Result = "UNHANDLED CHILD";
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
    sCMD_Result = "UNKNOWN DEVICE";
}

class MediaStream *Xine_Plugin::CreateMediaStream( class MediaPluginInfo *pMediaPluginInfo, class EntertainArea *pEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	if( (!pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_Xine_Player_CONST) && pEntertainArea )
	{
		ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_Xine_Player_CONST);
		if( pListMediaDevice && pListMediaDevice->size())
			pMediaDevice = pListMediaDevice->front();
	}

	if( !pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_Xine_Player_CONST )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Xine plugin being told to play in an entertainment area without a xine player");
		return NULL;
	}

	return new MediaStream( pMediaPluginInfo, pMediaDevice, pMediaPluginInfo->m_iPK_DesignObj, iPK_Users, st_RemovableMedia, StreamID ); // hack hack hack
}

bool Xine_Plugin::StartMedia( class MediaStream *pMediaStream )
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
    g_pPlutoLogger->Write( LV_STATUS, "Starting media stream playback--sending command, waiting for response" );

    string sFileToPlay = pMediaStream->GetFilenameToPlay("Empty file name");

    g_pPlutoLogger->Write( LV_STATUS, "Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

    string mediaURL;
    string Response;

    if ( pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST )
    {
        pMediaStream->m_sMediaDescription = "Media Desc";

        g_pPlutoLogger->Write(LV_STATUS, "Got pluto DVD media type");
        // Find a disk Drive in one of the entertainment areas.
        map<int, EntertainArea *>::iterator itEntertainmentAreas;

        bool bFound = false ;

        for ( itEntertainmentAreas = pMediaStream->m_mapEntertainArea.begin(); ! bFound && itEntertainmentAreas != pMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
        {
            EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;

            map<int, MediaDevice *>::iterator itDevice;
            for( itDevice = pEntertainArea->m_mapMediaDevice.begin( ); ! bFound && itDevice != pEntertainArea->m_mapMediaDevice.end( ); ++itDevice )
            {
                class MediaDevice *pMediaDevice = ( *itDevice ).second;

                if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory==DEVICECATEGORY_Disc_Drives_CONST )
                {
                    DCE::CMD_Mount_Disk_Image mountCommand( pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, sFileToPlay, &mediaURL );

                    g_pPlutoLogger->Write(LV_STATUS, "And Here ");
                    // TODO: read the response ( see if the mount was succesfull ) and continue if not.
                    if ( SendCommand( mountCommand, &Response ) )
                    {
                        g_pPlutoLogger->Write(LV_STATUS, "And again Here ");
                        bFound = true;
                        break; // exit the loop
                    }
                    else
                    {
                        g_pPlutoLogger->Write(LV_STATUS, "And again Here with failure");
                        g_pPlutoLogger->Write( LV_CRITICAL, "Failed to receive response from the disk drive device!. Response was: \"%s\"!", Response.c_str( ) );
                    }

                    g_pPlutoLogger->Write(LV_STATUS, "Got response from the disk drive: %s", mediaURL.c_str() );
                }
            }
            g_pPlutoLogger->Write( LV_CRITICAL, "Media device %d got back URL: %s", pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, mediaURL.c_str( ) );
        }
    }
    else
    {
        // HACK: -- todo: get real informations.
        pMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(sFileToPlay);
        mediaURL = sFileToPlay;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Calling play command with media URL: %s", mediaURL.c_str());
    DCE::CMD_Play_Media cmd( m_dwPK_Device,
                             pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
                             mediaURL,
                             pMediaStream->m_iPK_MediaType,
                             pMediaStream->m_iStreamID_get( ),
                            0 );

    m_pMedia_Plugin->MediaInfoChanged( pMediaStream );

    if( !SendCommand( cmd, &Response ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Xine player didn't respond to play media command!" );
        // handle failure
#pragma warning( "Ignore this for now" )
        return true;
        return false;
    }

    g_pPlutoLogger->Write( LV_STATUS, "Xine player responded to play media command!" );
    return true;
}

bool Xine_Plugin::StopMedia( class MediaStream *pMediaStream )
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  g_pPlutoLogger->Write( LV_STATUS, "Stopping media stream playback--sending command, waiting for response" );

  DCE::CMD_Stop_Media cmd(  m_dwPK_Device,                          // Send from us
                            pMediaStream->m_pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,	// Send to the device that is actually playing
                            pMediaStream->m_iStreamID_get( ),       // Send teh stream ID that we want to actually stop
                            &pMediaStream->m_iStoppedAtPosition);   // Get Back the position at which this stream was stopped.
  string Response;
  if( !SendCommand( cmd, &Response ) )
  {
    g_pPlutoLogger->Write( LV_CRITICAL, "Xine player didn't respond to stop media command!" );
    // handle failure
#pragma warning( "Ignore this for now" )
    return true;
    return false;
  }
  g_pPlutoLogger->Write( LV_STATUS, "Xine player responded to stop media command!" );
  return true;
}

bool Xine_Plugin::isValidStreamForPlugin(class MediaStream *pMediaStream)
{
    return pMediaStream->GetType() == MEDIASTREAM_TYPE_GENERIC;
}

bool Xine_Plugin::BroadcastMedia( class MediaStream *pMediaStream )
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  return true;
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
    {
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


/*

  COMMANDS TO IMPLEMENT

*/


//<-dceag-c36-b->

	/** @brief COMMAND: #36 - Create Media */
	/** Create a media stream descriptor. */
		/** @param #13 Filename */
			/** The filename of the media stream. */
		/** @param #41 StreamID */
			/** The media descriptor which will be associated with the current media. */

void Xine_Plugin::CMD_Create_Media(string sFilename,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c36-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
}

//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** Play a media stream descriptor. */
		/** @param #13 Filename */
			/** The file to play.  The format is specific on the media type and the media player. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */

void Xine_Plugin::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** Stop playing a media stream descriptor. */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Xine_Plugin::CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** Pause a media playback. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Xine_Plugin::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  Message *pNewMessage = new Message( pMessage );
  pNewMessage->m_dwPK_Device_To = 6; // hack this in. need to lookup streams
  QueueMessageToRouter( pNewMessage );
}
//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** Restart a media playback. */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Xine_Plugin::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Change the playback speed of a media stream. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed. This is a multiplier of the normal speed. (If we want 2x playback this parameter will be 2 if we want half of normal speed then the parameter will be 0.5). The formula is NextSpeed = MediaPlaybackSpeed * NormalPlaybackS */

void Xine_Plugin::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  Message *pNewMessage = new Message( pMessage );
  pNewMessage->m_dwPK_Device_To = 6; /** @warning hack this in. need to lookup streams */
  QueueMessageToRouter( pNewMessage );
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jumps to a position within some media, such as songs in a playlist, tracks on a cd, etc.  It will assume the sender is an orbiter, and find the entertainment area and stream associated with it.  The track can be an absolute or relative position. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void Xine_Plugin::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  Message *pNewMessage = new Message( pMessage );
  pNewMessage->m_dwPK_Device_To = 6; /** @warning hack this in. need to lookup streams   */
  QueueMessageToRouter( pNewMessage );
}

//<-dceag-sample-b->! no sample

