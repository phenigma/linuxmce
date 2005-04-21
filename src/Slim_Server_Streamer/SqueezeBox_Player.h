//<-dceag-d-b->
#ifndef SqueezeBox_Player_h
#define SqueezeBox_Player_h

//	DCE Implemenation for #58 SqueezeBox Player

#include "Gen_Devices/SqueezeBox_PlayerBase.h"
//<-dceag-d-e->

// #include "Slim_Server_Streamer.h"

//<-dceag-decl-b->
namespace DCE
{
	class SqueezeBox_Player : public SqueezeBox_Player_Command
	{
//<-dceag-decl-e->
        // Private member variables
        class Slim_Server_Streamer *findSlimServerController();

        // Private methods
public:
        // Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		SqueezeBox_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~SqueezeBox_Player();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		SqueezeBox_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_FloorplanObjectType();
	string DATA_Get_Port();

			*****EVENT***** accessors inherited from base class

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


//<-dceag-h-e->
    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
