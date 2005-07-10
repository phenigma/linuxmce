//<-dceag-d-b->
#include "VideoLan_Server.h"
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
#include "pluto_main/Define_CommandParameter.h"

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

VideoLan_Server *g_pVideoLan_Server = NULL;

void sh(int i) /* signal handler */
{
	if ( g_pVideoLan_Server && g_pVideoLan_Server->m_bQuit )
		return;

	int status = 0;
	pid_t pid = 0;


	pid = wait(&status);

	if ( g_pVideoLan_Server )
		g_pVideoLan_Server->ProcessExited(pid, status);
}
#endif


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VideoLan_Server::VideoLan_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VideoLan_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
, m_VideoLanMutex("videolan")
{
	m_VideoLanMutex.Init(NULL);
	m_bUseMultiCast=false;
#ifndef WIN32
	g_pVideoLan_Server = this;
	signal(SIGCHLD, sh); /* install handler */
#endif
	/* TODO -- DATA_Get_Network_Interfaces
	will give us a multicast address and range.  if specified,
	change GetVlanStream to use multicast */
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VideoLan_Server::~VideoLan_Server()
//<-dceag-dest-e->
{
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);
	for(map<int,class VideoLanServerInstance *>::iterator it=m_mapVideoLanServerInstance.begin();it!=m_mapVideoLanServerInstance.end();++it)
	{
		// Kill any instances we spawned
		vector<void *> data;
		ProcessUtils::KillApplication(it->second->m_sSpawnName, data);
	}
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VideoLan_Server::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

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
void VideoLan_Server::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void VideoLan_Server::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

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

void VideoLan_Server::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt play nonexistant stream");
		return;
	}

	vector<string> vectIPs;
	vector<int> vectDevices;
	string::size_type pos=0;
	while(pos!=string::npos && pos<pVideoLanServerInstance->m_sStreamingTargets.size())
	{
		int PK_Device = atoi(StringUtils::Tokenize(pVideoLanServerInstance->m_sStreamingTargets,",",pos).c_str());
		DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(PK_Device);
		g_pPlutoLogger->Write(LV_STATUS,"Device %d found at %p",PK_Device,pDeviceData_Base);
		while( pDeviceData_Base )
		{
			if( pDeviceData_Base->m_sIPAddress.size() )
			{
				vectIPs.push_back(pDeviceData_Base->m_sIPAddress);
				vectDevices.push_back(PK_Device);
				break;
			}
			pDeviceData_Base = pDeviceData_Base->m_pDevice_ControlledVia;
		}
	}

	g_pPlutoLogger->Write(LV_STATUS,"Found %d IP's",(int) vectIPs.size());

	if( vectIPs.size()==0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"No IP's to stream to");
		return;
	}

	pVideoLanServerInstance->m_sFilename = sFilename;
	pVideoLanServerInstance->m_sCommandLine = "--intf\trc\t" + sFilename + 
		"\t--sout\t" + GetVlanStream(vectIPs,iStreamID);
	pVideoLanServerInstance->m_sSpawnName = "vlc_s_" + StringUtils::itos(iStreamID);


	if( !ProcessUtils::SpawnApplication("vlc",pVideoLanServerInstance->m_sCommandLine,pVideoLanServerInstance->m_sSpawnName) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to spawn server");
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS,"Found %d Devices",(int) vectDevices.size());
	for(size_t s=0;s<vectDevices.size();++s)
	{
		DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,vectDevices[s],"udp:",iPK_MediaType,iStreamID,"");
		SendCommand(CMD_Play_Media);
	}

	if( sMediaPosition.size() )
		CMD_Set_Media_Position(iStreamID,sMediaPosition);
}

string VideoLan_Server::GetVlanStream(vector<string> &vectIPs,int iStreamID)
{
	string sStream;
	if( vectIPs.size()==1 )
		sStream = "#standard{access=udp,mux=ts,url=" + vectIPs[0] + ",sap,name=\"s" + StringUtils::itos(iStreamID) + "\"}";
	else if( m_bUseMultiCast )
		sStream = "MultiCast";
	else
	{
		sStream = "#duplicate{";
		for(size_t s=0;s<vectIPs.size();++s)
		{
			if( s!=0 )
				sStream += ",";
			sStream += "dst=standard{access=udp,mux=ts,url=" + vectIPs[s] + ",sap,name=\"s" + StringUtils::itos(iStreamID) + "\"}";
		}
		sStream += "}";
	}

	g_pPlutoLogger->Write(LV_STATUS,"Returning stream %s",sStream.c_str());
	return sStream;
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void VideoLan_Server::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void VideoLan_Server::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt change playback on nonexistant stream");
		return;
	}

	string cmd="title_n\n";
	if( iMediaPlaybackSpeed<0 )
		cmd="title_p\n";

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"next\n")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to stop VideoLan Server");
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

void VideoLan_Server::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	int iStreamID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());

	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter_n\n")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send chapter_n");
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

void VideoLan_Server::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	int iStreamID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());

	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter_p\n")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send chapter_p");
}

//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void VideoLan_Server::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	cout << "Need to implement command #89 - Vol Up" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;
}

//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void VideoLan_Server::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	cout << "Need to implement command #90 - Vol Down" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;
}

//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

void VideoLan_Server::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	cout << "Need to implement command #97 - Mute" << endl;
}

//<-dceag-c243-b->

	/** @brief COMMAND: #243 - Enable Broadcasting */
	/** Enable broadcasting from here. */
		/** @param #41 StreamID */
			/** The stream to enable broadcast for */
		/** @param #59 MediaURL */
			/** The media url from which this stream can be played. */

void VideoLan_Server::CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c243-e->
{
	cout << "Need to implement command #243 - Enable Broadcasting" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
}

//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Starts streaming */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void VideoLan_Server::CMD_Start_Streaming(int iStreamID,string sStreamingTargets,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	cout << "Need to implement command #249 - Start Streaming" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = new VideoLanServerInstance(this,iStreamID,sStreamingTargets,*sMediaURL);
	m_mapVideoLanServerInstance[iStreamID]=pVideoLanServerInstance;
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

void VideoLan_Server::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
}

//<-dceag-c262-b->

	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void VideoLan_Server::CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c262-e->
{
	cout << "Need to implement command #262 - Stop Streaming" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt play nonexistant stream");
		return;
	}

	vector<void *> data;
	if( ProcessUtils::KillApplication(pVideoLanServerInstance->m_sSpawnName,data)==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to stop VideoLan Server");

	m_mapVideoLanServerInstance.erase(iStreamID);

	string::size_type pos=0;
	while( pos<pVideoLanServerInstance->m_sStreamingTargets.size() )
	{
		string MediaPosition;
		int PK_Device = atoi(StringUtils::Tokenize(pVideoLanServerInstance->m_sStreamingTargets,",",pos).c_str());
		DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,PK_Device,iStreamID,&MediaPosition);
		SendCommand(CMD_Stop_Media);
	}
}

void VideoLan_Server::ProcessExited(int pid, int status)
{
	string applicationName;
	void *data;

	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
		g_pPlutoLogger->Write(LV_WARNING, "VideoLan_Server::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
	else
		g_pPlutoLogger->Write(LV_WARNING, "VideoLan_Server::ProcessExited() Child with pid %d terminated", pid);
}
//<-dceag-c140-b->

	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */

void VideoLan_Server::CMD_Audio_Track(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
}

//<-dceag-c141-b->

	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */

void VideoLan_Server::CMD_Subtitle(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
}

//<-dceag-c142-b->

	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */

void VideoLan_Server::CMD_Angle(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position */

void VideoLan_Server::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"Set media position %d %s",iStreamID,sMediaPosition.c_str());
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	int Pos=-2,Title=-2,Chapter=2,Subtitle=-2,Angle=-2,AudioTrack=-2;
	Pos = CalculatePosition(sMediaPosition,&Title,&Chapter,&Subtitle,&Angle,&AudioTrack);

	g_pPlutoLogger->Write(LV_STATUS,"VideoLan_Server::CMD_Set_Media_Position %d %d",Title,Chapter);

	if( Title!=-2 && ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"title " + StringUtils::itos(Title) + "\n")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send chapter_p");
	if( Chapter!=-2 && ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter " + StringUtils::itos(Chapter) + "\n")==false )
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send chapter_p");
}

int VideoLan_Server::CalculatePosition(string &sMediaPosition,int *Title,int *Chapter,int *Subtitle,int *Angle,int *AudioTrack)
{
	string::size_type pos=0;
	int iPos=0;

	if( Title )
	{
		pos = sMediaPosition.find(" TITLE:");
		if( pos!=string::npos )
			*Title = atoi( sMediaPosition.substr(pos+7).c_str() )+1;
	}

	if( Chapter )
	{
		pos = sMediaPosition.find(" CHAPTER:");
		if( pos!=string::npos )
			*Chapter = atoi( sMediaPosition.substr(pos+9).c_str() )+1;  // 
	}

	if( Subtitle )
	{
		pos = sMediaPosition.find(" SUBTITLE:");
		if( pos!=string::npos )
			*Subtitle = atoi( sMediaPosition.substr(pos+10).c_str() );
	}

	if( AudioTrack )
	{
		pos = sMediaPosition.find(" AUDIO:");
		if( pos!=string::npos )
			*AudioTrack = atoi( sMediaPosition.substr(pos+7).c_str() );
	}

	pos = sMediaPosition.find(" POS:");
	if( pos!=string::npos )
		iPos = atoi( sMediaPosition.substr(pos+5).c_str() );

	return iPos;
}

