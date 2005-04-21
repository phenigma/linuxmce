//<-dceag-d-b->
#ifndef Slim_Server_Streamer_h
#define Slim_Server_Streamer_h

//	DCE Implemenation for #53 Slim Server Streamer

#include "Gen_Devices/Slim_Server_StreamerBase.h"
//<-dceag-d-e->

#include "SqueezeBox_Player.h"

#include <map>

using namespace std;

typedef enum _StreamState {
	STATE_UNDEFINED,
	STATE_STOP,
	STATE_PAUSE,
	STATE_PLAY
} StreamStateType;

//<-dceag-decl-b->
namespace DCE
{
	class Slim_Server_Streamer : public Slim_Server_Streamer_Command
	{
//<-dceag-decl-e->
        bool					m_bShouldQuit;

		pthread_t               m_threadPlaybackCompletedChecker;
        pluto_pthread_mutex_t   m_mutexDataStructureAccess;

		pthread_mutex_t			m_stateChangedMutex;
		pthread_cond_t			m_stateChangedCondition;

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

		string FindControllingMacForStream(int iStreamID);

		void SetStateForStream(int iStreamID, StreamStateType newState);
		StreamStateType GetStateForStream(int iStreamID);
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

	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { string sCMD_Result; CMD_Play_Media(sFilename.c_str(),iPK_MediaType,iStreamID,iMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

	virtual void CMD_Stop_Media(int iStreamID,int *iMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,iMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

	virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed. This is a multiplier of the normal speed. (If we want 2x playback this parameter will be 2 if we want half of normal speed then the parameter will be 0.5). The formula is NextSpeed = MediaPlaybackSpeed * NormalPlaybackS */

	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater() { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

	virtual void CMD_Skip_Back_ChannelTrack_Lower() { string sCMD_Result; CMD_Skip_Back_ChannelTrack_Lower(sCMD_Result,NULL);};
	virtual void CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */

	virtual void CMD_Vol_Up() { string sCMD_Result; CMD_Vol_Up(sCMD_Result,NULL);};
	virtual void CMD_Vol_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */

	virtual void CMD_Vol_Down() { string sCMD_Result; CMD_Vol_Down(sCMD_Result,NULL);};
	virtual void CMD_Vol_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

	virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
	virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #243 - Enable Broadcasting */
	/** Enable broadcasting from here. */
		/** @param #41 StreamID */
			/** The stream to enable broadcast for */
		/** @param #59 MediaURL */
			/** The media url from which this stream can be played. */

	virtual void CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL) { string sCMD_Result; CMD_Enable_Broadcasting(iStreamID,sMediaURL,sCMD_Result,NULL);};
	virtual void CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #249 - Start Streaming */
	/** Starts streaming */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

	virtual void CMD_Start_Streaming(int iStreamID,string sStreamingTargets,string *sMediaURL) { string sCMD_Result; CMD_Start_Streaming(iStreamID,sStreamingTargets.c_str(),sMediaURL,sCMD_Result,NULL);};
	virtual void CMD_Start_Streaming(int iStreamID,string sStreamingTargets,string *sMediaURL,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length) { string sCMD_Result; CMD_Report_Playback_Position(iStreamID,sOptions,iMediaPosition,iMedia_Length,sCMD_Result,NULL);};
	virtual void CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

	virtual void CMD_Stop_Streaming(int iStreamID,string sStreamingTargets) { string sCMD_Result; CMD_Stop_Streaming(iStreamID,sStreamingTargets.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!

