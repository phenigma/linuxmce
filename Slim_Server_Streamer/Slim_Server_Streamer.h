//<-dceag-d-b->
#ifndef Slim_Server_Streamer_h
#define Slim_Server_Streamer_h

//  DCE Implemenation for #53 Slim Server Streamer

#include "Gen_Devices/Slim_Server_StreamerBase.h"
//<-dceag-d-e->

#include "SqueezeBox_Player.h"

#include <map>

using namespace std;

typedef enum _StreamState {
    STATE_PLAY,
    STATE_STOP
} StreamStateType;

//<-dceag-decl-b->
namespace DCE
{
    class Slim_Server_Streamer : public Slim_Server_Streamer_Command
    {
//<-dceag-decl-e->
        pthread_t               m_threadPlaybackCompletedChecker;
        pthread_cond_t          m_condPlaybackCompletedChecker;
        pluto_pthread_mutex_t   m_mutexDataStructureAccess;

        // map<int, list<string> > m_mapStreamsToSqueezeBoxesPlayers;
        string  m_strSlimServerCliAddress;
        int m_iSlimServerCliPort;

        // Semantics:
        //   streamID ->
        //              [
        //                  state { playing, stopped }
        //                  a vector of SqueezeBox_Player objects ( the devices on which this stream is playing ).
        //              ]
        map<int, pair<StreamStateType, vector<DeviceData_Base *> > > m_mapStreamsToPlayers;

        // the socket used to communicate with the server
        SOCKET m_iServerSocket;

        // Private member variables

        // Private methods
        bool ConnectToSlimServerCliCommandChannel();
        string SendReceiveCommand(string command);

        static void *checkForPlaybackCompleted(void *pSlim_Server_Streamer);

    public:
        // Public member variables

    //<-dceag-const-b->
public:
        // Constructors/Destructor
        Slim_Server_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
        virtual ~Slim_Server_Streamer();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-h-b->
    /*
                AUTO-GENERATED SECTION
                Do not change the declarations
    */

    /*
            *****DATA***** accessors inherited from base class

            *****EVENT***** accessors inherited from base class
    void EVENT_Playback_Completed(int iStream_ID);

            *****COMMANDS***** we need to implement
    */


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

    virtual void CMD_Start_Streaming(string sFilename,int iStreamID,string sStreamingDestinations,string *sMediaURL) { string sCMD_Result; CMD_Start_Streaming(sFilename.c_str(),iStreamID,sStreamingDestinations.c_str(),sMediaURL,sCMD_Result,NULL);};
    virtual void CMD_Start_Streaming(string sFilename,int iStreamID,string sStreamingDestinations,string *sMediaURL,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!

