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
#include "VDR.h"
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
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_MediaType.h"
#include "Gen_Devices/AllScreens.h"
#include "VDRCommon.h"


#include <sstream>
#include <pthread.h>

#define VDR_SOCKET_TIMEOUT	3  // SECONDS
VDR *g_pVDR = NULL;

#ifndef WIN32
#include <sys/wait.h>
#define VDR_WINDOW_NAME "VDR"
#endif

void* monitorVDRThread(void* param)
{
	while(g_pVDR->m_bExiting==false)
	{
		if( g_pVDR->m_VDRStatus_get() == VDRSTATUS_DISCONNECTED )
			Sleep(500);
		else
		{
			//g_pVDR->pollVDRStatus();
			Sleep(1000);
		}
	}
	return NULL;
}


// NEW

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VDR::VDR(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VDR_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_VDRMutex("VDR")
{
  g_pVDR = this;
  m_VDRMutex.Init(NULL);
  m_iVDRWindowId = 0;
	m_iStreamID = 0;
	m_pDisplay = NULL;
	m_pDevice_MediaPlugin=NULL;
	m_pDevice_DVBCard = NULL;
	m_VDRStatus_set(VDRSTATUS_DISCONNECTED);

}
                                


//<-dceag-getconfig-b->
bool VDR::GetConfig()
{
	if( !VDR_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	m_pDevice_DVBCard = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Digital_TV_Cards_CONST);
	m_pDevice_Xine = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
	m_pDevice_MediaPlugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Media_Plugins_CONST);
	
	if( !m_pDevice_Xine || !m_pDevice_MediaPlugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need a Xine player & media plugin to function");
		return false;
	}
			
	// system("/etc/init.d/dvb start; sleep 3; /etc/init.d/vdr start");  // First just be sure vdr is running

	if( m_pDevice_DVBCard )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"This MD has a DVB card.  Will use local xine");
		m_sXineIP = "localhost";
		return true;
	}

	DeviceData_Base *pDevice_Core = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
	if( pDevice_Core )
	{
		DeviceData_Base *pDevice_DVBCard = pDevice_Core->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Digital_TV_Cards_CONST);
		DeviceData_Base *pDevice_Xine = pDevice_Core->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
		if( pDevice_DVBCard && pDevice_Xine && pDevice_Core->m_sIPAddress.size() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"This MD has no DVB card, but core does.  Will use xine on %s",pDevice_Core->m_sIPAddress.c_str());
			m_pDevice_Xine = pDevice_Xine;
			m_sXineIP = pDevice_Core->m_sIPAddress;
			return true;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Core doesn't have a DVB (%p) or Xine (%p) or IP %s",
				pDevice_DVBCard, pDevice_Xine, pDevice_Core->m_sIPAddress.c_str());
	}

	// Find all other VDR's
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_VDR_CONST )
		{
			DeviceData_Base *pDevice_DVBCard = pDevice->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Digital_TV_Cards_CONST);
			DeviceData_Base *pDevice_Xine = pDevice->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);

			if( pDevice_DVBCard && pDevice_Xine && pDevice->m_pDevice_MD && pDevice->m_pDevice_MD->m_sIPAddress.size() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDR %d has xine + dvb will use %s",
				pDevice->m_dwPK_Device,pDevice->m_pDevice_MD->m_sIPAddress.c_str());
				m_pDevice_Xine = pDevice_Xine;
				m_sXineIP = pDevice->m_pDevice_MD->m_sIPAddress;
				return true;
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDR %d doesn't have a DVB (%p) or Xine (%p) or pc %p or IP %s",
					pDevice->m_dwPK_Device, pDevice_DVBCard, pDevice_Xine, 
					pDevice->m_pDevice_MD,(pDevice->m_pDevice_MD ? pDevice->m_pDevice_MD->m_sIPAddress.c_str() : ""));
		}
	}


	return true;
                
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VDR::~VDR()
//<-dceag-dest-e->
{
    // Kill any instances we spawned
		//system("killall pluto-vdr");  // Be sure there's no front end running


}

// ######### LAUNCH
bool VDR::LaunchVDR()
/*
{
	DCE::CMD_Play_Media cmd(m_dwPK_Device,m_pDevice_Xine->m_dwPK_Device,
			MEDIATYPE_pluto_LiveTV_CONST,1,"","xvdr://" + m_sXineIP + "#nocache;demux:mpeg_block"); // Stream ID and start position not important BURGIMAN
	return SendCommand(cmd);
}
*/

{
	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
			TOSTRING(COMMAND_Application_Exited_CONST) " " 
			TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";
		//string sWidth = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenWidth_CONST);
		//string sHeight = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenHeight_CONST); 
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/bin/plutovdr", "VDR", "--reconnect\t--fullscreen\txvdr:tcp://127.0.0.1:37890",
			sMessage + "1",sMessage + "0",false,false,true,false);
		if( SendCommand(CMD_Spawn_Application) )
			m_VDRStatus_set(VDRSTATUS_LIVETV);
		 	return true;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR::LaunchVDRFrontend - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR::LaunchVDRFrontend - no app server");
	return false;
 }


//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VDR::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

void VDR::CreateChildren()
{
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
void VDR::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VDR::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}
//<-dceag-sample-b->!
//<-dceag-sample-e->


//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */
		/** @param #59 MediaURL */
			/** The file to play, or other media id.  The format is specific on the media type and the media player. */

void VDR::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	PLUTO_SAFETY_LOCK(mm,m_VDRMutex);
	string sResponse;
	
	if ( m_VDRStatus_get() == VDRSTATUS_DISCONNECTED )
	{
		LaunchVDR();
	
	}
	else
	{
		if (m_VDRStatus_get() == VDRSTATUS_LIVETV || m_VDRStatus_get() == VDRSTATUS_PLAYBACK)
			SendVDRCommand("MESG 1live", sResponse);
		else
			SendVDRCommand("MESG 2live", sResponse);
	}
	selectWindow();

	// Set the initial channel to tune to after startup
	m_sInitialChannel = sMediaPosition;
	m_iStreamID=iStreamID;

	string::size_type pos;
/*	if( (pos=sMediaPosition.find("CHAN:")!=string::npos) && sMediaPosition.size()>5 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Will start with channel %s",sMediaPosition.substr(pos+5).c_str());
		string sResponse;
		if( SendVDRCommand("CHAN " + sMediaPosition.substr(pos+5),sResponse) )
			ParseCurrentChannel(sResponse);
	}
*/
}

void VDR::selectWindow()
{
}



void VDR::updateMode(string toMode)
{
	if (toMode!=m_CurrentMode)
	{				
		m_CurrentMode = toMode;
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Changing mode: %s",toMode.c_str());
		DCE::CMD_Set_Active_Menu CMD_Set_Active_Menu_(m_dwPK_Device,m_pDevice_VDR_Plugin->m_dwPK_Device, toMode);
		SendCommand(CMD_Set_Active_Menu_);
	}
}



//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void VDR::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	system("killall plutovdr");
	m_VDRStatus_set(VDRSTATUS_DISCONNECTED);
	string SavedPosition;
        DCE::CMD_Stop_Media cmd(m_dwPK_Device,m_pDevice_Xine->m_dwPK_Device,
                        1,
			&SavedPosition);
	SendCommand(cmd);
	m_iStreamID = 0;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void VDR::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	string sResponse;
	SendVDRCommand("MESG pausemedia",sResponse);
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void VDR::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	string sResponse;
	SendVDRCommand("MESG restartmedia",sResponse);	

	//cout << "Need to implement command #40 - Restart Media" << endl;
	//cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

void VDR::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	string sResponse;
	SendVDRCommand("HITK Play",sResponse);	

}

//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Capture a Video frame */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

void VDR::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
	string sResponse;
	SendVDRCommand("MESG getvideoframe",sResponse);	

 /*
	cout << "Need to implement command #84 - Get Video Frame" << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #20 - Format=" << sFormat << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #60 - Width=" << iWidth << endl;
	cout << "Parm #61 - Height=" << iHeight << endl;
 */
}


//<-dceag-c129-b->

	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

void VDR::CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c129-e->
{
	string sResponse;
	SendVDRCommand("MESG pipchanup",sResponse);	

	//cout << "Need to implement command #129 - PIP - Channel Up" << endl;
}

//<-dceag-c130-b->

	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

void VDR::CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c130-e->
{
	string sResponse;
	SendVDRCommand("MESG pipchandown",sResponse);	

	//cout << "Need to implement command #130 - PIP - Channel Down" << endl;
}


//<-dceag-c187-b->

	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

void VDR::CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c187-e->
{
	string sResponse;
	if( SendVDRCommand("CHAN " + sProgramID,sResponse) )
		ParseCurrentChannel(sResponse);
	SendVDRCommand("MESG tunetochannel",sResponse);	
}


//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

void VDR::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
  string sResponse;
	SendVDRCommand("MESG reportplaybackposition",sResponse);	

	/*
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	*/
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void VDR::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	string sResponse;
	SendVDRCommand("MESG setmediaposition",sResponse);	

 /*
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
 */
}



//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void VDR::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
	string sResponse;
	SendVDRCommand("MESG Jumptopositioninstream",sResponse);	

}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	string sResponse;
	SendVDRCommand("MESG jumptoposinplaylist",sResponse);	

}

void VDR::KillSpawnedDevices()
{
}

bool VDR::SendVDRCommand(string sCommand,string &sVDRResponse)
{
	//LoggerWrapper::GetInstance()->Write(LV_WARNING,"Going to send command %s",sCommand.c_str());
	//PlainClientSocket _PlainClientSocket(m_sXineIP + ":2001");
	PlainClientSocket _PlainClientSocket("127.0.0.1:2001");
	if( !_PlainClientSocket.Connect() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to VDR client");
		return false;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"connected");
	string sResponse;
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ready got %s",sResponse.c_str());
		return false;
	}

	if( !_PlainClientSocket.SendString(sCommand) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="250" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %s",sResponse.c_str());
		return false;
	}
	
	if( sResponse.size()>4 )
		sVDRResponse = sResponse.substr(4);
		//LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDR Responded %s",sResponse.c_str());
	if( !_PlainClientSocket.SendString("QUIT") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="221" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with quit got %s",sResponse.c_str());
		return false;
	}
	
	_PlainClientSocket.Close();
	return true;
}

void VDR::ParseCurrentChannel(string sChannel)
{
	if( sChannel.size()<3 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Error parsing channel %s",sChannel.c_str());
		return;
	}
	string::size_type pos_space = sChannel.find(' ');
	m_iChannelNumber = atoi( sChannel.c_str() );
	if( pos_space!=string::npos )
		m_sChannelName = sChannel.substr(pos_space+1);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL," BURGI: VDR processed ok, channel %d %s",m_iChannelNumber,m_sChannelName.c_str());
	DCE::CMD_Update_Time_Code CMD_Update_Time_Code_(m_dwPK_Device,m_pDevice_MediaPlugin->m_dwPK_Device,
		m_iStreamID,"","","","", StringUtils::itos(m_iChannelNumber) + " " + m_sChannelName);
	SendCommand(CMD_Update_Time_Code_);


	
}
//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void VDR::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	string sResponse;
	string convkey;
	//Convert the raw-keys... because there is no sName...
	if(sPK_Button == "16"){ convkey = "1"; } 
	if(sPK_Button == "17"){ convkey = "2"; } 
	if(sPK_Button == "18"){ convkey = "3"; } 
	if(sPK_Button == "19"){ convkey = "4"; } 
	if(sPK_Button == "20"){ convkey = "5"; } 
	if(sPK_Button == "21"){ convkey = "6"; } 
	if(sPK_Button == "22"){ convkey = "7"; } 
	if(sPK_Button == "23"){ convkey = "8"; } 
	if(sPK_Button == "24"){ convkey = "9"; } 
	if(sPK_Button == "25"){ convkey = "0"; } 
																				
		
	if( !SendVDRCommand("HITK " + convkey,sResponse) )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to send HITK %s",sResponse.c_str());

		if( SendVDRCommand("CHAN",sResponse) )
		  ParseCurrentChannel(sResponse);

}
//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	string sResponse;
	SendVDRCommand("HITK Recordings",sResponse);	

}
//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/** position X */
		/** @param #12 Position Y */
			/** position Y */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
}
//<-dceag-c32-b->

	/** @brief COMMAND: #32 - Update Object Image */
	/** Display an image on the media player */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

void VDR::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	string sResponse;
	SendVDRCommand("MESG updateobjectimage",sResponse);	

}
//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void VDR::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
	string sResponse;
	SendVDRCommand("MESG navigatenext",sResponse);	

}
//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void VDR::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
	string sResponse;
	SendVDRCommand("MESG navigateprev",sResponse);	

}
//<-dceag-c87-b->

	/** @brief COMMAND: #87 - Goto Media Menu */
	/** Goto to the current media Root Menu. */
		/** @param #41 StreamID */
			/** The stream ID */
		/** @param #64 MenuType */
			/** The type of menu that the user want to jump to.
(For DVD handlers usually this applies)
0 - Root menu 
1 - Title menu
2 - Media menu */

void VDR::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
	string sResponse;
	SendVDRCommand("MESG gotomediamenu",sResponse);	

}
	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void VDR::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	string sResponse;
	SendVDRCommand("HITK Menu",sResponse);
}
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	string sResponse;
	SendVDRCommand("HITK Ok",sResponse);
}
//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	string sResponse;
	SendVDRCommand("HITK Up",sResponse);
	if( SendVDRCommand("CHAN ",sResponse) )
		ParseCurrentChannel(sResponse);
}
//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
		string sResponse;
	SendVDRCommand("HITK Down",sResponse);
	if( SendVDRCommand("CHAN ",sResponse) )
		ParseCurrentChannel(sResponse);

}
//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	string sResponse;
	SendVDRCommand("HITK Left",sResponse);
//	ParseCurrentChannel(sResponse);
}
//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	string sResponse;
	SendVDRCommand("HITK Right",sResponse);
//	ParseCurrentChannel(sResponse);
}
//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void VDR::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	string sResponse;
	SendVDRCommand("HITK O",sResponse);
//	ParseCurrentChannel(sResponse);
}
//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void VDR::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	string sResponse;
	SendVDRCommand("HITK 1",sResponse);
}
//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void VDR::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
		string sResponse;
	SendVDRCommand("HITK 2",sResponse);
}
//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void VDR::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
		string sResponse;
	SendVDRCommand("HITK 3",sResponse);
}
//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void VDR::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
		string sResponse;
	SendVDRCommand("HITK 4",sResponse);
}
//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void VDR::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
		string sResponse;
	SendVDRCommand("HITK 5",sResponse);
}
//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void VDR::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
		string sResponse;
	SendVDRCommand("HITK 6",sResponse);
}
//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void VDR::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
		string sResponse;
	SendVDRCommand("HITK 7",sResponse);
}
//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void VDR::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	string sResponse;
	SendVDRCommand("HITK 8",sResponse);
}
//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void VDR::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
		string sResponse;
	SendVDRCommand("HITK 9",sResponse);
}
//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	string sResponse;
	SendVDRCommand("HITK Back",sResponse);
}


//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Like play media, but it means the destination device is not the same as the source */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #42 MediaPosition */
			/** Where to start playing from */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void VDR::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	string sResponse;
	SendVDRCommand("MESG startstreaming",sResponse);	

}
//<-dceag-c367-b->

	/** @brief COMMAND: #367 - Text */
	/** Text */

void VDR::CMD_Text(string &sCMD_Result,Message *pMessage)
//<-dceag-c367-e->
{
	string sResponse;
	SendVDRCommand("MESG cmd_text",sResponse);	

}
//<-dceag-c368-b->

	/** @brief COMMAND: #368 - Help */
	/** Help */

void VDR::CMD_Help(string &sCMD_Result,Message *pMessage)
//<-dceag-c368-e->
{
	string sResponse;
	SendVDRCommand("MESG cmd_help",sResponse);	

}
//<-dceag-c455-b->

	/** @brief COMMAND: #455 - Blue */
	/** VDR blue */

void VDR::CMD_Blue(string &sCMD_Result,Message *pMessage)
//<-dceag-c455-e->
{
	string sResponse;
	SendVDRCommand("MESG Blue",sResponse);
}
//<-dceag-c476-b->

	/** @brief COMMAND: #476 - Day Down */
	/** Day + */

void VDR::CMD_Day_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c476-e->
{
	string sResponse;
	SendVDRCommand("MESG DayDown",sResponse);
}
//<-dceag-c477-b->

	/** @brief COMMAND: #477 - Day Up */
	/** Day - */

void VDR::CMD_Day_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c477-e->
{
	string sResponse;
	SendVDRCommand("MESG Dayup",sResponse);
}
//<-dceag-c528-b->

	/** @brief COMMAND: #528 - Instant Replay */
	/** Instant replay */

void VDR::CMD_Instant_Replay(string &sCMD_Result,Message *pMessage)
//<-dceag-c528-e->
{
	string sResponse;
	SendVDRCommand("MESG Instantreplay",sResponse);
}
//<-dceag-c535-b->

	/** @brief COMMAND: #535 - Last Play */
	/** Last */

void VDR::CMD_Last_Play(string &sCMD_Result,Message *pMessage)
//<-dceag-c535-e->
{
	string sResponse;
	SendVDRCommand("MESG lastplay",sResponse);
}
//<-dceag-c571-b->

	/** @brief COMMAND: #571 - Page Down */
	/** Page + */

void VDR::CMD_Page_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c571-e->
{
	string sResponse;
	SendVDRCommand("MESG PageDown",sResponse);
}
//<-dceag-c572-b->

	/** @brief COMMAND: #572 - Page Up */
	/** Page - */

void VDR::CMD_Page_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c572-e->
{
	string sResponse;
	SendVDRCommand("MESG PageUp",sResponse);
}
//<-dceag-c592-b->

	/** @brief COMMAND: #592 - PVR Menu */
	/** Show the pvr menu */

void VDR::CMD_PVR_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c592-e->
{
	string sResponse;
	SendVDRCommand("MESG PVR_menu",sResponse);
}
//<-dceag-c605-b->

	/** @brief COMMAND: #605 - Red */
	/** VDR red */

void VDR::CMD_Red(string &sCMD_Result,Message *pMessage)
//<-dceag-c605-e->
{
	string sResponse;
	SendVDRCommand("MESG RED",sResponse);
}
//<-dceag-c682-b->

	/** @brief COMMAND: #682 - Yellow */
	/** VDR yellow */

void VDR::CMD_Yellow(string &sCMD_Result,Message *pMessage)
//<-dceag-c682-e->
{
	string sResponse;
	SendVDRCommand("MESG Yellow",sResponse);
}
//<-dceag-c762-b->
//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Change channels greater (larger numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Change channels lower (smaller numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
}

//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** vol- */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void VDR::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** vol+ */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void VDR::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
}
//<-dceag-c91-b->

	/** @brief COMMAND: #91 - Input Select */
	/** toggle inputs */
		/** @param #71 PK_Command_Input */
			/** The Input to select, or 0=toggle */

void VDR::CMD_Input_Select(int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c91-e->
{
}
//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** VDR pause */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
}
//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** VDR stop */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void VDR::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** 97 */

void VDR::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
}
//<-dceag-c102-b->

	/** @brief COMMAND: #102 - Record */
	/** Record the current show */

void VDR::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{
}
//<-dceag-c123-b->

	/** @brief COMMAND: #123 - Info */
	/** Info about the currently playing program */
		/** @param #9 Text */
			/** nimic */

void VDR::CMD_Info(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c123-e->
{
}
//<-dceag-c124-b->

	/** @brief COMMAND: #124 - Favorites */
	/** Show favorites */

void VDR::CMD_Favorites(string &sCMD_Result,Message *pMessage)
//<-dceag-c124-e->
{
}
//<-dceag-c126-b->

	/** @brief COMMAND: #139 - Play */
	/** vdr play */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDR::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
}
//<-dceag-c762-b->

	/** @brief COMMAND: #762 - Live TV */
	/** Go to Live TV */

void VDR::CMD_Live_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c762-e->
{
}
//<-dceag-c763-b->

	/** @brief COMMAND: #763 - Exit */
	/** Exit guide */

void VDR::CMD_Exit(string &sCMD_Result,Message *pMessage)
//<-dceag-c763-e->
{
}
//<-dceag-c777-b->

	/** @brief COMMAND: #777 - Recorded TV Menu */
	/** go to the recorded tv menu */

void VDR::CMD_Recorded_TV_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c777-e->
{
}
//<-dceag-c809-b->

	/** @brief COMMAND: #809 - Display Alert */
	/** Displays alert on the box */
		/** @param #9 Text */
			/** The text in the alert */
		/** @param #70 Tokens */
			/** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
		/** @param #182 Timeout */
			/** Make the alert go away after this many seconds */
		/** @param #251 Interruption */
			/** How to interrupt the user if something is happening */

void VDR::CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage)
//<-dceag-c809-e->
{
}
//<-dceag-c870-b->

	/** @brief COMMAND: #870 - Get Data */
	/** Get data from a device */
		/** @param #9 Text */
			/** What data to return.  This is free form */
		/** @param #19 Data */
			/** The data being returned */

void VDR::CMD_Get_Data(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c870-e->
{
}
//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void VDR::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
}
//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void VDR::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
}
//<-dceag-c919-b->

	/** @brief COMMAND: #919 - On Demand */
	/** Go to Video on demand */

void VDR::CMD_On_Demand(string &sCMD_Result,Message *pMessage)
//<-dceag-c919-e->
{
}
//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void VDR::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
}
