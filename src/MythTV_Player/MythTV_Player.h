/**
 *
 * @file MythTV_Player.h
 * @brief header file for the MythTV_Player class
 *
 */


//<-dceag-d-b->
#ifndef MythTV_Player_h
#define MythTV_Player_h

//	DCE Implemenation for #35 MythTV Player

#include "Gen_Devices/MythTV_PlayerBase.h"
//<-dceag-d-e->

class RatPoisonWrapper;

//<-dceag-decl-b->
namespace DCE
{
	class MythTV_Player : public MythTV_Player_Command
	{
//<-dceag-decl-e->
    private:
        long unsigned int            m_iMythFrontendWindowId;
	    pluto_pthread_mutex_t m_MythMutex;

        /** Private member variables */
        int                          m_iControllingDevice;
        pthread_t                    m_qApplicationThreadId;
        RatPoisonWrapper            *m_pRatWrapper;

        void selectWindow();
        bool checkWindowName(long unsigned int window, string windowName);

    protected:
        bool LaunchMythFrontend();

        void processKeyBoardInputRequest(int iXKeySym);

        // This should be Window but if i put #include <X11/Xlib.h>  in this it will break the compilation.
        bool locateMythTvFrontendWindow(long unsigned int window);

		bool checkXServerConnection();
        /** Private methods */
    public:
        /** Public member variables */

//<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_Player();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void KillSpawnedDevices();
		virtual void CreateChildren();

		virtual void ProcessExited(int pid, int status);
    //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Error_Occured(string sError_Message);

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

	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Play_Media(sFilename.c_str(),iPK_MediaType,iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,sMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


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
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */

	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

	virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_to_Position_in_Stream(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

	virtual void CMD_PIP_Channel_Up() { string sCMD_Result; CMD_PIP_Channel_Up(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

	virtual void CMD_PIP_Channel_Down() { string sCMD_Result; CMD_PIP_Channel_Down(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID) { string sCMD_Result; CMD_Tune_to_channel(sOptions.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */

	virtual void CMD_EnterGo() { string sCMD_Result; CMD_EnterGo(sCMD_Result,NULL);};
	virtual void CMD_EnterGo(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Up */

	virtual void CMD_Move_Up() { string sCMD_Result; CMD_Move_Up(sCMD_Result,NULL);};
	virtual void CMD_Move_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Down */

	virtual void CMD_Move_Down() { string sCMD_Result; CMD_Move_Down(sCMD_Result,NULL);};
	virtual void CMD_Move_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Left */

	virtual void CMD_Move_Left() { string sCMD_Result; CMD_Move_Left(sCMD_Result,NULL);};
	virtual void CMD_Move_Left(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Right */

	virtual void CMD_Move_Right() { string sCMD_Result; CMD_Move_Right(sCMD_Result,NULL);};
	virtual void CMD_Move_Right(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #204 - 0 */
	/** 0 */

	virtual void CMD_0() { string sCMD_Result; CMD_0(sCMD_Result,NULL);};
	virtual void CMD_0(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #205 - 1 */
	/** 1 */

	virtual void CMD_1() { string sCMD_Result; CMD_1(sCMD_Result,NULL);};
	virtual void CMD_1(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #206 - 2 */
	/** 2 */

	virtual void CMD_2() { string sCMD_Result; CMD_2(sCMD_Result,NULL);};
	virtual void CMD_2(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #207 - 3 */
	/** 3 */

	virtual void CMD_3() { string sCMD_Result; CMD_3(sCMD_Result,NULL);};
	virtual void CMD_3(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #208 - 4 */
	/** 4 */

	virtual void CMD_4() { string sCMD_Result; CMD_4(sCMD_Result,NULL);};
	virtual void CMD_4(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #209 - 5 */
	/** 5 */

	virtual void CMD_5() { string sCMD_Result; CMD_5(sCMD_Result,NULL);};
	virtual void CMD_5(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #210 - 6 */
	/** 6 */

	virtual void CMD_6() { string sCMD_Result; CMD_6(sCMD_Result,NULL);};
	virtual void CMD_6(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #211 - 7 */
	/** 7 */

	virtual void CMD_7() { string sCMD_Result; CMD_7(sCMD_Result,NULL);};
	virtual void CMD_7(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #212 - 8 */
	/** 8 */

	virtual void CMD_8() { string sCMD_Result; CMD_8(sCMD_Result,NULL);};
	virtual void CMD_8(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #213 - 9 */
	/** 9 */

	virtual void CMD_9() { string sCMD_Result; CMD_9(sCMD_Result,NULL);};
	virtual void CMD_9(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */

	virtual void CMD_Back_Prior_Menu() { string sCMD_Result; CMD_Back_Prior_Menu(sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

	virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition) { string sCMD_Result; CMD_Report_Playback_Position(iStreamID,sText,sMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->


//<-dceag-const2-b->!
