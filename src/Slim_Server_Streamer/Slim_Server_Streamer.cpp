//<-dceag-d-b->
#include "Slim_Server_Streamer.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Slim_Server_Streamer::Slim_Server_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Slim_Server_Streamer_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
    , m_dataStructureAccessMutex("internal-data-structure-mutex"), m_stateChangedMutex("state-changed-mutex")
{
    // Usually the slim server and the application server that is going to spawn it are going to be on the same machine.
    // This is good because you can also restrict the control of the server only to connections from that machine.
    m_strSlimServerCliAddress = "127.0.0.1"; // hard code it here for now.
    m_iSlimServerCliPort = 7890;

    m_iServerSocket = -1;

    pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	m_dataStructureAccessMutex.Init( &mutexAttr );

	pthread_cond_init( &m_stateChangedCondition, NULL );
	g_pPlutoLogger->Write(LV_STATUS, "Condition as passed to the mutex: %p", &m_stateChangedCondition );
	m_stateChangedMutex.Init( &mutexAttr, &m_stateChangedCondition );
	g_pPlutoLogger->Write(LV_STATUS, "Condition as passed to the mutex: %p", m_stateChangedMutex.m_pthread_cond_t );

    pthread_create(&m_threadPlaybackCompletedChecker, NULL, checkForPlaybackCompleted, this);
	m_bShouldQuit = false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Slim_Server_Streamer::~Slim_Server_Streamer()
//<-dceag-dest-e->
{
	m_bShouldQuit = true;
	pthread_cond_signal(&m_stateChangedCondition);
	pthread_join(m_threadPlaybackCompletedChecker, NULL);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Slim_Server_Streamer::Register()
//<-dceag-reg-e->
{
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
void Slim_Server_Streamer::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Slim_Server_Streamer::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

    COMMANDS TO IMPLEMENT

*/



//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Starts streaming */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void Slim_Server_Streamer::CMD_Start_Streaming(int iStreamID,string sStreamingTargets,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
    PLUTO_SAFETY_LOCK( pm, m_dataStructureAccessMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Processing Start streaming command for target devices: %s", sStreamingTargets.c_str());

    vector<string> vectPlayersIds;
    vector<DeviceData_Base*> vectDevices;

    StringUtils::Tokenize(sStreamingTargets, ",", vectPlayersIds);

    string lastPlayerAddress = "-";
    string currentPlayerAddress = "-";

    vector<string>::iterator itPlayerIds = vectPlayersIds.begin();
    while ( itPlayerIds != vectPlayersIds.end() )
    {
        int iPlayerId = atoi((*itPlayerIds).c_str());

        if ( iPlayerId == 0 )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Player id string %s parsed to 0. Ignoring.", (*itPlayerIds).c_str() );
            continue;
        }

        DeviceData_Base *pPlayerDeviceData = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(iPlayerId);
        if ( pPlayerDeviceData == NULL )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Child with id: %d was not found. Ignoring", iPlayerId);
            continue;
        }

		vectDevices.push_back(pPlayerDeviceData);

        currentPlayerAddress = StringUtils::URLEncode(StringUtils::ToLower(pPlayerDeviceData->GetMacAddress()));

        SendReceiveCommand(currentPlayerAddress + " sync -"); // break previous syncronization;
		SendReceiveCommand(currentPlayerAddress + " stop"); // stop playback (if any)
		SendReceiveCommand(currentPlayerAddress + " playlist clear"); // clear previous playlist (if any)
        SendReceiveCommand(currentPlayerAddress + " sync " + lastPlayerAddress); // synchronize with the last one.
        lastPlayerAddress = currentPlayerAddress;
        itPlayerIds++;
    }

	// add this stream to the list of playing streams.
    m_mapStreamsToPlayers[iStreamID] = make_pair(STATE_PAUSE, vectDevices);
	pthread_cond_signal(&m_stateChangedCondition);
}

//<-dceag-c262-b->

	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void Slim_Server_Streamer::CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c262-e->
{
	PLUTO_SAFETY_LOCK( pm, m_dataStructureAccessMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Processing Stop Streaming command for target devices: %s", sStreamingTargets.c_str());

	map<int, pair<StreamStateType, vector<DeviceData_Base *> > >::iterator itPlayers;

	itPlayers = m_mapStreamsToPlayers.find(iStreamID);
	if ( itPlayers == m_mapStreamsToPlayers.end() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Stop_Streaming() got a command for a stream that is not registered here: %d", iStreamID);
		return;
	}

	string currentPlayerAddress;
    vector<string> vectPlayersIds;
    vector<DeviceData_Base*> &vectDevices = (*itPlayers).second.second;

    StringUtils::Tokenize(sStreamingTargets, ",", vectPlayersIds);

	vector<DeviceData_Base*>::iterator itPlaybackDevices;
	vector<string>::iterator itPlayerIds = vectPlayersIds.begin();
    while ( itPlayerIds != vectPlayersIds.end() )
	{
		int iPlayerId = atoi((*itPlayerIds).c_str());

        if ( iPlayerId == 0 )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Player id string %s parsed to 0. Ignoring.", (*itPlayerIds).c_str() );
            continue;
        }

        DeviceData_Base *pPlayerDeviceData = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(iPlayerId);
        if ( pPlayerDeviceData == NULL )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Child with id: %d was not found. Ignoring", iPlayerId);
            continue;
        }

		currentPlayerAddress = StringUtils::URLEncode(StringUtils::ToLower(pPlayerDeviceData->GetMacAddress()));

		// break this players syncronization.
        SendReceiveCommand(currentPlayerAddress + " sync -");
		SendReceiveCommand(currentPlayerAddress + " playlist clear");
		SendReceiveCommand(currentPlayerAddress + " stop");

		itPlaybackDevices = vectDevices.begin();
		while ( itPlaybackDevices != vectDevices.end() && *itPlaybackDevices != pPlayerDeviceData )
			itPlaybackDevices++;

		if ( itPlaybackDevices == vectDevices.end() )
			g_pPlutoLogger->Write(LV_WARNING, "Asked to stop a stream on a client that was not playing the stream!");
		else
			vectDevices.erase(itPlaybackDevices);

		itPlayerIds++;
	}

	if ( vectDevices.size() == 0 )
		m_mapStreamsToPlayers.erase(iStreamID);
}

bool Slim_Server_Streamer::ConnectToSlimServerCliCommandChannel()
{
    bool socketNotOpen  = true;
    struct sockaddr_in serverSocket;
    struct hostent *host;

    m_iServerSocket = socket(PF_INET, SOCK_STREAM, 0);
    if ( m_iServerSocket == -1 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Could create client test socket. This should not happend here. Returning empty mrl.");
        return false;
    }

    if( (host = gethostbyname(m_strSlimServerCliAddress.c_str())) == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Could not resolve IP address: %s. Not connecting.", m_strSlimServerCliAddress.c_str());
        return false;
    }

    g_pPlutoLogger->Write(LV_WARNING, "Trying to connect to SlimServer at address: %s:%d", m_strSlimServerCliAddress.c_str(), m_iSlimServerCliPort );
    serverSocket.sin_family = AF_INET;
    memcpy((char *) &serverSocket.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    serverSocket.sin_port = htons(m_iSlimServerCliPort);

    long timeSpent;
    struct timeval startTime, currentTime;

    gettimeofday(&startTime, NULL);
    while ( socketNotOpen )
    {
        gettimeofday(&currentTime, NULL);
        timeSpent = (currentTime.tv_sec - startTime.tv_sec) * 1000000 + (currentTime.tv_usec - startTime.tv_usec);

        if ( connect(m_iServerSocket, (struct sockaddr*)&serverSocket, sizeof(serverSocket)) == 0 )
        {
            g_pPlutoLogger->Write(LV_STATUS, "We have made a successful connection to the slim server on %s:%d. Life is good.", m_strSlimServerCliAddress.c_str(), m_iSlimServerCliPort);
            socketNotOpen = false;
        }
        else
        {
            g_pPlutoLogger->Write(LV_STATUS, "We can't connect yet to the server. Waiting!");
            Sleep(500);
        }

        if ( socketNotOpen && timeSpent >= 3 * 1000000 ) // wait at most three seconds;
        {
            g_pPlutoLogger->Write(LV_WARNING, "Couldn't contact the slim server for the past 3 seconds..");
            m_iServerSocket = -1;
            return false;
        }
    }

    return true;
}

string Slim_Server_Streamer::SendReceiveCommand(string command)
{
    if ( m_iServerSocket == 0 || m_iServerSocket == -1 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Connection is down. Trying to reconnect.");
        if ( ! ConnectToSlimServerCliCommandChannel() )
        {
            g_pPlutoLogger->Write(LV_STATUS, "Reconnection failed. Sending a start command to the application server.");
//             string notificationMessage = StringUtils::Format("%d %d %d %d",
//                                                             0,
//                                                             m_dwPK_Device,
//                                                             MESSAGETYPE_COMMAND,
//                                                             COMMAND_Start_Streaming_CONST
//                                                             );

			DCE::CMD_Spawn_Application_DT spawnApplication(
                    m_dwPK_Device,                      // send from here
                    DEVICETEMPLATE_App_Server_CONST,    // to an application server
                    BL_SameComputer,                    // on the same computer
                    "/usr/pluto/bin/LaunchSlimServer.sh",  // launch this
                    "slim-server",                              // reference it with this name
                    StringUtils::itos(m_iSlimServerCliPort),    // pass it the desired port number for reconnection.
                    "",                                         // execute this serialized message on exit with failure
                    "",
					false);                                        // execute this serialized message on exit with success

            spawnApplication.m_pMessage->m_bRelativeToSender = true;
            SendCommand(spawnApplication);

            Sleep(3000);
            if ( ! ConnectToSlimServerCliCommandChannel() ) // try again the connection after the command was sent.
            {
                g_pPlutoLogger->Write(LV_WARNING, "I could not connect to the streaming server after i have launched it with the application server.");
                return "";
            }
        }
    }

    g_pPlutoLogger->Write(LV_STATUS, "Sending command: %s", command.c_str());
    unsigned int nBytes;
    const char *sendBuffer;
    char *receiveBuffer;

    command += "\n";

    sendBuffer = command.c_str();

    nBytes = 0;
    while ( nBytes >= 0 && nBytes < (unsigned int)command.length() - 1  )
    {
        int result = write((int)m_iServerSocket, sendBuffer + nBytes, (unsigned int)command.length() - nBytes);

        if ( result == 0 || result == -1 )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Error while sending to socket: %s.", strerror(errno));
            close((int)m_iServerSocket);
            m_iServerSocket = -1;
        }

        nBytes += result;
    }

    receiveBuffer = new char[1024];

    int pos = 0;
    while ( read((int)m_iServerSocket, receiveBuffer + pos, 1) == 1 && pos < 1023 && receiveBuffer[pos] != '\n' ) pos++;

    receiveBuffer[pos] = '\0';
    if ( pos == 1023 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Invalid read from socket. Closing connection");
        close((int)m_iServerSocket);
        m_iServerSocket = -1;
    }

    string result(receiveBuffer);
    delete receiveBuffer;

    g_pPlutoLogger->Write(LV_STATUS, "Got response: %s", result.c_str());
    return result;
}

void *Slim_Server_Streamer::checkForPlaybackCompleted(void *pSlim_Server_Streamer)
{
    Slim_Server_Streamer *pStreamer = (Slim_Server_Streamer*)pSlim_Server_Streamer;

    while ( true )
    {
		string macAddress, strResult;

		PLUTO_SAFETY_LOCK( lock, pStreamer->m_stateChangedMutex );
		g_pPlutoLogger->Write(LV_STATUS, "The map size: %d should quit: %d", pStreamer->m_mapStreamsToPlayers.size(), pStreamer->m_bShouldQuit );
		while ( pStreamer->m_mapStreamsToPlayers.size() == 0 || pStreamer->m_bShouldQuit )
			lock.CondWait();
		lock.Release();

		g_pPlutoLogger->Write(LV_STATUS, "Looping");

		if ( pStreamer->m_bShouldQuit )
			return NULL;

        PLUTO_SAFETY_LOCK( pm, pStreamer->m_dataStructureAccessMutex);

        map<int, pair<StreamStateType, vector<DeviceData_Base *> > >::iterator itStreamsToPlayers;
        itStreamsToPlayers = pStreamer->m_mapStreamsToPlayers.begin();
        while ( itStreamsToPlayers != pStreamer->m_mapStreamsToPlayers.end() )
		{
			DeviceData_Base *pPlayerDeviceData = (*itStreamsToPlayers).second.second[0];

			macAddress = StringUtils::URLEncode(pPlayerDeviceData->GetMacAddress());
			strResult = pStreamer->SendReceiveCommand((macAddress + " mode ?").c_str());

			if ( itStreamsToPlayers->second.first == STATE_PLAY && ( strResult == macAddress + " mode stop" || strResult == macAddress + " mode %3F") )
			{
				g_pPlutoLogger->Write(LV_STATUS, "Sending playback completed event for stream %d", itStreamsToPlayers->first);
				pStreamer->SetStateForStream((*itStreamsToPlayers).first, STATE_STOP);
				pStreamer->EVENT_Playback_Completed((*itStreamsToPlayers).first);
			}
			else if ( strResult == macAddress + " mode pause" )
			{
				pStreamer->SetStateForStream((*itStreamsToPlayers).first, STATE_PAUSE);
			}
            itStreamsToPlayers++;
        }
        pm.Release();

        Sleep(500);
    }

    return NULL;
}


string Slim_Server_Streamer::FindControllingMacForStream(int iStreamID)
{
	PLUTO_SAFETY_LOCK( dataMutex, m_dataStructureAccessMutex );

    if ( m_mapStreamsToPlayers.find(iStreamID) != m_mapStreamsToPlayers.end() )
    {
        DeviceData_Base *playerDevice = m_mapStreamsToPlayers[iStreamID].second.front();
        return StringUtils::URLEncode(StringUtils::ToLower(playerDevice->GetMacAddress()));
    }

	g_pPlutoLogger->Write(LV_WARNING, "Could not find an controlling device for stream: %d", iStreamID);
	return "";
}

StreamStateType Slim_Server_Streamer::GetStateForStream(int iStreamID)
{
	PLUTO_SAFETY_LOCK(dataMutex, m_dataStructureAccessMutex);

	if ( m_mapStreamsToPlayers.find(iStreamID) == m_mapStreamsToPlayers.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Can't get the state of an invalid stream: %d!", iStreamID);
		return STATE_UNDEFINED;
	}

	return m_mapStreamsToPlayers[iStreamID].first;
}

void Slim_Server_Streamer::SetStateForStream(int iStreamID, StreamStateType newState)
{
	PLUTO_SAFETY_LOCK(dataMutex, m_dataStructureAccessMutex);

    if ( m_mapStreamsToPlayers.find(iStreamID) == m_mapStreamsToPlayers.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Can't change the state of an invalid stream: %d!", iStreamID);
		return;
	}

	m_mapStreamsToPlayers[iStreamID].first = newState;
}

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

void Slim_Server_Streamer::CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	// SendReceiveCommand(lastPlayerAddress + " playlist play " + StringUtils::URLEncode(string("file://") + sFilename).c_str());
	SendReceiveCommand(sControlledPlayerMac + " playlist play " + StringUtils::URLEncode(string("file://") + StringUtils::Replace(sFilename,"//", "/")));

	if ( iMediaPosition != 0 )
		SendReceiveCommand(sControlledPlayerMac + " gototime " + StringUtils::itos(iMediaPosition / 1000));

	SetStateForStream(iStreamID, STATE_PLAY);
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Slim_Server_Streamer::CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	PLUTO_SAFETY_LOCK( pm, m_dataStructureAccessMutex);

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	m_mapStreamsToPlayers.erase(iStreamID);
	SendReceiveCommand(sControlledPlayerMac + " playlist clear");
	SendReceiveCommand(sControlledPlayerMac + " stop");
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Slim_Server_Streamer::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
    PLUTO_SAFETY_LOCK( pm, m_dataStructureAccessMutex);

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	// SendReceiveCommand(lastPlayerAddress + " playlist play " + StringUtils::URLEncode(string("file://") + sFilename).c_str());

	if ( GetStateForStream(iStreamID) == STATE_PAUSE )
	{
		SendReceiveCommand(sControlledPlayerMac + " pause 0");
		SetStateForStream(iStreamID, STATE_PLAY);
	}
	else
	{
		SendReceiveCommand(sControlledPlayerMac + " pause 1");
		SetStateForStream(iStreamID, STATE_PAUSE);
	}
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Slim_Server_Streamer::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	CMD_Pause_Media(iStreamID,sCMD_Result,pMessage);
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

void Slim_Server_Streamer::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

void Slim_Server_Streamer::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

void Slim_Server_Streamer::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
}

//<-dceag-c243-b->

	/** @brief COMMAND: #243 - Enable Broadcasting */
	/** Enable broadcasting from here. */
		/** @param #41 StreamID */
			/** The stream to enable broadcast for */
		/** @param #59 MediaURL */
			/** The media url from which this stream can be played. */

void Slim_Server_Streamer::CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c243-e->
{

}

//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #39 Options */
			/** Other options that the player might record for this position. Usefull if we have a non standard encoding of the player position. */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** The reported media position ( in milliseconds since the beginning of the stream). */
		/** @param #106 Media Length */
			/** The complete length of the media stream. Where appliable. */

void Slim_Server_Streamer::CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	string commandResult;
	string command;
	string sPlayerMac;
	float floatValue;

	if ( (sPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	command = sPlayerMac + " gototime ";
	if ( (commandResult = SendReceiveCommand(command + "?")) == "")
	{
		sCMD_Result = "Could not get current position.";
		return;
	}

	if ( sscanf(commandResult.substr(command.length()).c_str(), "%f", &floatValue) == 0 )
	{
		sCMD_Result = "Could not parse current position";
		return;
	}

	*iMediaPosition = (int)(floatValue * 1000);

	command = sPlayerMac + " duration ";
	if ( (commandResult = SendReceiveCommand(command + "?")) == "")
	{
		sCMD_Result = "Could not get duration.";
		return;
	}

	if ( sscanf(commandResult.substr(command.length()).c_str(), "%f", &floatValue) == 0 )
	{
		sCMD_Result = "Could not parse duration.";
		return;
	}

	*iMedia_Length = (int)(floatValue * 1000);

	g_pPlutoLogger->Write(LV_STATUS, "Detected data: position %d from %d", *iMediaPosition, *iMedia_Length);
}
//<-dceag-createinst-b->!
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void Slim_Server_Streamer::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Vol_Up() Not implemented yet.");
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void Slim_Server_Streamer::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Vol_Down() Not implemented yet.");
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

void Slim_Server_Streamer::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Mute() Not implemented yet.");
}
