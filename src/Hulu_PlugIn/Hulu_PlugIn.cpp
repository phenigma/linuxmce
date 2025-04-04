/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Hulu_PlugIn.h"
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
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Table_Users.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Hulu_PlugIn::Hulu_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Hulu_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_HuluMediaMutex("hulu media mutex")
{
  m_HuluMediaMutex.Init(NULL);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Hulu_PlugIn::~Hulu_PlugIn()
//<-dceag-dest-e->
{
  PLUTO_SAFETY_LOCK(mm,m_HuluMediaMutex);
  pthread_mutex_destroy(&m_HuluMediaMutex.mutex);
}

//<-dceag-getconfig-b->
bool Hulu_PlugIn::GetConfig()
{
	if( !Hulu_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	return true;
}

void Hulu_PlugIn::PrepareToDelete()
{
  Command_Impl::PrepareToDelete();
  // Maybe we'll add some other destructo bits later.
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Hulu_PlugIn::Register()
//<-dceag-reg-e->
{
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Starting Hulu_Plugin::Register()");
  
  m_iPriority=DATA_Get_Priority();
  
  m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
  m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
  if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to Hulu plugin");
      return false;
    }
  
  vector<int> vectPK_DeviceTemplate;
  vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Hulu_Player_CONST);
  m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Registered device %d",DEVICETEMPLATE_Hulu_Player_CONST);
  
  RegisterMsgInterceptor(( MessageInterceptorFn )( &Hulu_PlugIn::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );
   
  return Connect(PK_DeviceTemplate_get()); 

}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Hulu_PlugIn_Command *Create_Hulu_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Hulu_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Hulu_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Hulu_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
  sCMD_Result = "UNKNOWN DEVICE";   // There is some seriously weird voodoo there. This causes
                                    // messages to be forwarded down the chain, but WHY?
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *Hulu_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Hulu Plugin CreateMediaStream Called");
  
  HuluMediaStream *pHuluMediaStream;
  MediaDevice *pMediaDevice_PassedIn;
  
  PLUTO_SAFETY_LOCK( xm, m_HuluMediaMutex );
  
  if(m_bQuit_get())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Hulu_PlugIn::CreateMediaStream with m_bQuit");
      return NULL;
    }
  
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  pMediaDevice_PassedIn = NULL;
  if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
      return NULL;
    }
  
  if ( pMediaDevice != NULL  && // test the media device only if it set
       pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Hulu_Player_CONST )
    {
      pMediaDevice_PassedIn = pMediaDevice;
      pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_Hulu_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected device (%d: %s) as playback device!",
				      pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				      pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
  
  pHuluMediaStream = new HuluMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
					  pMediaDevice,
					  iPK_Users, st_RemovableMedia, StreamID );
  
  m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;
  
  return pHuluMediaStream;
  
}

bool Hulu_PlugIn::StartMedia( MediaStream *pMediaStream,string &sError )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartMedia Called");
 
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Hulu_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	HuluMediaStream *pHuluMediaStream = NULL;
	if ( (pHuluMediaStream = ConvertToHuluMediaStream(pMediaStream, "Hulu_PlugIn::StartMedia(): ")) == NULL )
		return false;
 
	string sFileToPlay;
	MediaFile *pMediaFile = NULL;
 
	sFileToPlay = pHuluMediaStream->GetFilenameToPlay("Empty file name");
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Hulu_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
 
	string mediaURL;
	string Response;
 
	mediaURL = sFileToPlay;
 
	// send the CMD straight through.
 
	pHuluMediaStream->m_sMediaDescription = "Hulu";

	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pHuluMediaStream->m_iPK_MediaType,
						pHuluMediaStream->m_iStreamID_get( ),
						"00:00:00",mediaURL);
	SendCommand(CMD_Play_Media);
 
	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pHuluMediaStream->m_mapEntertainArea.begin( );itEA != pHuluMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
			pHuluMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return MediaHandlerBase::StartMedia(pMediaStream,sError);
 
}

bool Hulu_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StopMedia Called");
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Hulu_Plugin::StopMedia() Stopping Media Stream Playback... Pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	HuluMediaStream *pHuluMediaStream = NULL;
 
	if ((pHuluMediaStream = ConvertToHuluMediaStream(pMediaStream,"Hulu_Plugin::StopMedia():")) == NULL )
		return false;
 
	string savedPosition;
 
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pHuluMediaStream->m_iStreamID_get(),
						&savedPosition);
 
	SendCommand(CMD_Stop_Media);
 
	return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *Hulu_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Hulu_Player_CONST);
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
	if ( pMediaDevice == NULL )
	{

		return NULL;
	}
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
 
	return pMediaDevice;
}

HuluMediaStream *Hulu_PlugIn::ConvertToHuluMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}
 
	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_HULU )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a HuluMediaStream!").c_str());
		return NULL;
	}
 
	return static_cast<HuluMediaStream*>(pMediaStream);
}

/* Grabbed almost verbatim from Xine Player */
bool Hulu_PlugIn::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	/** Confirm this is from one of ours */
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Hulu_Player_CONST )
		return false; 

	int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";

	/** Find the stream */
	HuluMediaStream *pHuluMediaStream = NULL;
	MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
	if( !pMediaStream || (pHuluMediaStream = ConvertToHuluMediaStream(pMediaStream, "Hulu_Plugin::MenuOnScreen(): ")) == NULL )
		return false;

	pHuluMediaStream->m_bUseAltScreens=bOnOff;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pHuluMediaStream, pHuluMediaStream->m_iStreamID_get( ), pHuluMediaStream->m_iPK_MediaType );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pHuluMediaStream->m_mapEntertainArea.size( ) );


	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pHuluMediaStream->m_mapEntertainArea.begin( );itEA != pHuluMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
			pHuluMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return false;
}
