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
    , m_mutexDataStructureAccess("internal-data-structure-mutex")
{
    // Usually the slim server and the application server that is going to spawn it are going to be on the same machine.
    // This is good because you can also restrict the control of the server only to connections from that machine.
    m_strSlimServerCliAddress = "127.0.0.1"; // hard code it here for now.
    m_iSlimServerCliPort = 7890;

    m_iServerSocket = -1;

    pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
    m_mutexDataStructureAccess.Init( &mutexAttr );

    pthread_create(&m_threadPlaybackCompletedChecker, NULL, checkForPlaybackCompleted, this);
    pthread_detach(m_threadPlaybackCompletedChecker); // make the thread a free man..
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Slim_Server_Streamer::~Slim_Server_Streamer()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Slim_Server_Streamer::Register()
//<-dceag-reg-e->
{
    return Connect();
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

//<-dceag-sample-b->
/*      **** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Slim_Server_Streamer::SomeFunction()
{
    // If this is going to be loaded into the router as a plug-in, you can implement:   virtual bool Register();
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

/*

    COMMANDS TO IMPLEMENT

*/



//<-dceag-c249-b->

    /** @brief COMMAND: #249 - Start Streaming */
    /** Starts streaming */
        /** @param #13 Filename */
            /** The filename to stream */
        /** @param #41 StreamID */
            /** Identifier for this streaming session. */
        /** @param #59 MediaURL */
            /** The url to use to play this stream. */
        /** @param #105 StreamingDestinations */
            /** Target destinations for streaming. Semantics dependent on the target device. */

void Slim_Server_Streamer::CMD_Start_Streaming(string sFilename,int iStreamID,string sStreamingTargets,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Processing Start streaming command for target devices: %s", sStreamingTargets.c_str());
    PLUTO_SAFETY_LOCK( pm, m_mutexDataStructureAccess);

    if ( sStreamingTargets == "" &&
        m_mapStreamsToPlayers.find(iStreamID) != m_mapStreamsToPlayers.end() )
    {
        SqueezeBox_Player *player = m_mapStreamsToPlayers[iStreamID].second.front();

        string macAddress = StringUtils::URLEncode(StringUtils::ToLower(player->m_pData->GetMacAddress()));

        SendReceiveCommand(macAddress + " playlist play " + StringUtils::URLEncode(string("file://") + StringUtils::Replace(sFilename, "//", "/")).c_str());
        m_mapStreamsToPlayers[iStreamID].first = STATE_PLAY;
        return;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Processing Start streaming command for target devices: %s", sStreamingTargets.c_str());

    vector<string> vectPlayersIds;
    vector<SqueezeBox_Player *> vectPlayers;

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

        map<int, Command_Impl *>::iterator itPlayer = m_mapCommandImpl_Children.find(iPlayerId);
        if ( itPlayer == m_mapCommandImpl_Children.end() )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Child with id: %d was not found. Ignoring", iPlayerId);
            continue;
        }

        // assume all the children are Squeeze boxes.
        SqueezeBox_Player *pPlayer = static_cast<SqueezeBox_Player*>((*itPlayer).second);

        vectPlayers.push_back(pPlayer);

        currentPlayerAddress = StringUtils::URLEncode(StringUtils::ToLower(pPlayer->m_pData->GetMacAddress()));

        SendReceiveCommand(currentPlayerAddress + " sync -"); // break previous syncronization;
        SendReceiveCommand(currentPlayerAddress + " sync " + lastPlayerAddress); // synchronize with the last one.
        lastPlayerAddress = currentPlayerAddress;
        *itPlayerIds++;
    }

    // add this stream to the list of playing streams.
    m_mapStreamsToPlayers[iStreamID] = make_pair(STATE_PLAY, vectPlayers);

    SendReceiveCommand(lastPlayerAddress + " playlist play " + StringUtils::URLEncode(string("file://") + sFilename).c_str());
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
                    "/usr/pluto/bin/LaunchSlimServer",  // launch this
                    "slim-server",                              // reference it with this name
                    StringUtils::itos(m_iSlimServerCliPort),    // pass it the desired port number for reconnection.
                    "",                                         // execute this serialized message on exit with failure
                    "");                                        // execute this serialized message on exit with success

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
        PLUTO_SAFETY_LOCK( pm, pStreamer->m_mutexDataStructureAccess);

        map<int, pair<StreamStateType, vector<SqueezeBox_Player *> > >::iterator itStreamsToPlayers;

        itStreamsToPlayers = pStreamer->m_mapStreamsToPlayers.begin();
        while ( itStreamsToPlayers != pStreamer->m_mapStreamsToPlayers.end() &&
                (*itStreamsToPlayers).second.first == STATE_PLAY )
        {
            SqueezeBox_Player *pPlayer = (*itStreamsToPlayers).second.second[0];

            string strCommand = StringUtils::URLEncode(pPlayer->m_pData->GetMacAddress()) + " mode ?";
            string strResult = pStreamer->SendReceiveCommand(strCommand);

            if (strResult == StringUtils::URLEncode(pPlayer->m_pData->GetMacAddress()) + " mode stop")
            {
                (*itStreamsToPlayers).second.first = STATE_STOP;
                pStreamer->EVENT_Playback_Completed((*itStreamsToPlayers).first);
            }

            itStreamsToPlayers++;
        }

        pm.Release();

        Sleep(500);
    }

    return NULL;
}
