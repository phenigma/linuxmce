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

#include <sstream>
#include <pthread.h>

#ifndef WIN32
#include <sys/wait.h>

#define VDR_SOCKET_TIMEOUT	3  // SECONDS

VDR *g_pVDR = NULL;

void sh(int i) /* signal handler */
{
   if ( g_pVDR && g_pVDR->m_bQuit )
   		return;
                    
   int status = 0;
   pid_t pid = 0;
   pid = wait(&status);

   if ( g_pVDR )
   	g_pVDR->ProcessExited(pid, status);
}
#endif

#define VDR_WINDOW_NAME "VDR_Xine"



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
	m_pDevice_MediaPlugin=NULL;
	 m_pDevice_PVRCard = NULL;
}
                                


//<-dceag-getconfig-b->
bool VDR::GetConfig()
{
	if( !VDR_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	m_pDevice_PVRCard = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_PVR_Capture_Cards_CONST);
	m_pDevice_Xine = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
	m_pDevice_MediaPlugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Media_Plugins_CONST);
	if( !m_pDevice_Xine || !m_pDevice_MediaPlugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"I need a Xine player & media plugin to function");
		return false;
	}
			
	// system(("/usr/pluto/bin/Write_VDR_Conf.sh " + string(m_pDevice_PVRCard ? "1" : "0")).c_str());

	// Find all other VDR's
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_VDR_CONST )
		{
			string s=pDevice->m_sIPAddress;
		}
	return true;
	}

	
        signal(SIGCHLD, sh); /* install handler */
        return true;
                
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VDR::~VDR()
//<-dceag-dest-e->
{
        // Kill any instances we spawned
        vector<void *> data;
        ProcessUtils::KillApplication(VDR_WINDOW_NAME, data);

}

bool VDR::LaunchVDR()
{
	DCE::CMD_Play_Media cmd(m_dwPK_Device,m_pDevice_Xine->m_dwPK_Device,
			"vdr:/tmp/vdr-xine/stream#demux:mpeg_pes",
			MEDIATYPE_pluto_LiveTV_CONST,
			1,""); // Stream ID and start position not important
	return SendCommand(cmd);
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
    PLUTO_SAFETY_LOCK(mm,m_VDRMutex);
                                
    LaunchVDR();
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
void VDR::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
		/** @param #13 Filename */
			/** The file to play.  The format is specific on the media type and the media player. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */

void VDR::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
        /** dirty hack for xine testing */
        /** only to make the xine-protocol starting for developing burgi */
             
        // ProcessUtils::SpawnApplication("/usr/bin/xine", "vdr:/tmp/vdr-xine/stream#demux:mpeg_pes", "VDR_Xine");

	m_iStreamID = iStreamID;
    LaunchVDR();
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
	cout << "Need to implement command #39 - Pause Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void VDR::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void VDR::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
}

void VDR::ProcessExited(int pid, int status)
{
        PLUTO_SAFETY_LOCK(mm,m_VDRMutex);
        g_pPlutoLogger->Write(LV_STATUS, "Process exited %d %d", pid, status);
                
        void *data;
        string applicationName;
        if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
                return;
                                                       
        g_pPlutoLogger->Write(LV_STATUS, "Got application name: %s compare with %s", applicationName.c_str(), VDR_WINDOW_NAME);
                                                         
        if ( applicationName.compare(VDR_WINDOW_NAME) == 0 )
        {
                g_pPlutoLogger->Write(LV_STATUS, "Send go back to the caller!");
                DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"");
                SendCommand(CMD_MH_Stop_Media_Cat);
        }
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
	cout << "Need to implement command #84 - Get Video Frame" << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #20 - Format=" << sFormat << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #60 - Width=" << iWidth << endl;
	cout << "Parm #61 - Height=" << iHeight << endl;
}


//<-dceag-c129-b->

	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

void VDR::CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c129-e->
{
	cout << "Need to implement command #129 - PIP - Channel Up" << endl;
}

//<-dceag-c130-b->

	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

void VDR::CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c130-e->
{
	cout << "Need to implement command #130 - PIP - Channel Down" << endl;
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
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
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
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
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
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void VDR::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
}

void VDR::KillSpawnedDevices()
{
}

bool VDR::SendVDRCommand(string sCommand,string &sVDRResponse)
{
	g_pPlutoLogger->Write(LV_WARNING,"Going to send command %s",sCommand.c_str());
	PlainClientSocket _PlainClientSocket("localhost:2001");
	if( !_PlainClientSocket.Connect() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to connect to VDR client");
		return false;
	}
g_pPlutoLogger->Write(LV_STATUS,"connected");
	string sResponse;
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"VDR not ready got %s",sResponse.c_str());
		return false;
	}

	if( !_PlainClientSocket.SendString(sCommand) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="250" )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"VDR not ok with command got %s",sResponse.c_str());
		return false;
	}
	
	if( sResponse.size()>4 )
		sVDRResponse = sResponse.substr(4);
g_pPlutoLogger->Write(LV_WARNING,"VDR Responded %s",sResponse.c_str());
	if( !_PlainClientSocket.SendString("QUIT") )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="221" )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"VDR not ok with quit got %s",sResponse.c_str());
		return false;
	}
	
	_PlainClientSocket.Close();
	return true;
}

void VDR::ParseCurrentChannel(string sChannel)
{
	if( sChannel.size()<3 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Error parsing channel %s",sChannel.c_str());
		return;
	}
	string::size_type pos_space = sChannel.find(' ');
	m_iChannelNumber = atoi( sChannel.c_str() );
	if( pos_space!=string::npos )
		m_sChannelName = sChannel.substr(pos_space+1);
	g_pPlutoLogger->Write(LV_CRITICAL,"VDR processed ok, channel %d / %s",m_iChannelNumber,m_sChannelName.c_str());
	DCE::CMD_Update_Time_Code CMD_Update_Time_Code_(m_dwPK_Device,m_pDevice_MediaPlugin->m_dwPK_Device,
		m_iStreamID,"","",
		"","",
		StringUtils::itos(m_iChannelNumber) + " " + m_sChannelName);
	SendCommand(CMD_Update_Time_Code_);
}
