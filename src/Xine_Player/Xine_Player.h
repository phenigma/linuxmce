/**
 *
 * @file Xine_Player.h
 * @brief header file for the Xine_Player class
 * @author
 *
 */

//<-dceag-d-b->
#ifndef Xine_Player_h
#define Xine_Player_h

//	DCE Implemenation for #5 Xine Player

#include "Gen_Devices/Xine_PlayerBase.h"
//<-dceag-d-e->

#include "XineSlaveWrapper.h"
//#include "SlimServerClient.h"
#include "utilities/linux/RatpoisonHandler.h"

//<-dceag-decl-b->! custom

class SlimServerClient;

namespace DCE
{
	class Xine_Player : public Xine_Player_Command, public RatpoisonHandler<Xine_Player>
	{
	//<-dceag-decl-e->
	private:
		SlimServerClient *m_pSlimServerClient; /** < m_pSlimServerClient is the wrapper around the slim server protocol */

		pluto_pthread_mutex_t 	m_xineSlaveMutex;

		SlimServerClient *getSlimServerClient();


	//<-dceag-const-b->
public:
		// Constructors/Destructor
		Xine_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Xine_Player();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		/** Private member variables */

		XineSlaveWrapper *m_pXineSlaveControl; /** < m_pXineSlaveControl points to the XineSlaveWrapper object */

		bool Connect(int iPK_DeviceTemplate );

		Display *getDisplay();
	public:
		// Public member variables
		int m_iTitle,m_iChapter;
		string m_sCurrentFile;

	//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Output_Speaker_arrangement();
	string DATA_Get_Alsa_Output_Device();
	string DATA_Get_Subtitles();
	void DATA_Set_Subtitles(string Value);
	string DATA_Get_Audio_Tracks();
	void DATA_Set_Audio_Tracks(string Value);
	string DATA_Get_Angles();
	void DATA_Set_Angles(string Value);

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Menu_Onscreen(int iStream_ID,bool bOnOff);
	void EVENT_Playback_Completed(int iStream_ID,bool bWith_Errors);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Chapter/Track Next/Down/Forward */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater() { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Chapter/Track Back/Up/Prior */

	virtual void CMD_Skip_Back_ChannelTrack_Lower() { string sCMD_Result; CMD_Skip_Back_ChannelTrack_Lower(sCMD_Result,NULL);};
	virtual void CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Next(int iStreamID) { string sCMD_Result; CMD_Navigate_Next(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Prev(int iStreamID) { string sCMD_Result; CMD_Navigate_Prev(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's the current video frame from the media player. */
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

	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType) { string sCMD_Result; CMD_Goto_Media_Menu(iStreamID,iMenuType,sCMD_Result,NULL);};
	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #89 - Vol Up */
	/** Make the sound go up. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

	virtual void CMD_Vol_Up(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Up(iRepeat_Command,sCMD_Result,NULL);};
	virtual void CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #90 - Vol Down */
	/** Make the sound go down. */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

	virtual void CMD_Vol_Down(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Down(iRepeat_Command,sCMD_Result,NULL);};
	virtual void CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #97 - Mute */
	/** Mute the sound. */

	virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
	virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */

	virtual void CMD_Audio_Track(string sValue_To_Assign) { string sCMD_Result; CMD_Audio_Track(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Audio_Track(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */

	virtual void CMD_Subtitle(string sValue_To_Assign) { string sCMD_Result; CMD_Subtitle(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Subtitle(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */

	virtual void CMD_Angle(string sValue_To_Assign) { string sCMD_Result; CMD_Angle(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Angle(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */

	virtual void CMD_EnterGo() { string sCMD_Result; CMD_EnterGo(sCMD_Result,NULL);};
	virtual void CMD_EnterGo(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */

	virtual void CMD_Move_Up() { string sCMD_Result; CMD_Move_Up(sCMD_Result,NULL);};
	virtual void CMD_Move_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */

	virtual void CMD_Move_Down() { string sCMD_Result; CMD_Move_Down(sCMD_Result,NULL);};
	virtual void CMD_Move_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */

	virtual void CMD_Move_Left() { string sCMD_Result; CMD_Move_Left(sCMD_Result,NULL);};
	virtual void CMD_Move_Left(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */

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
			/** The media position */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

		/**
		* @brief destructor
		*/

		virtual void FireMenuOnScreen(int iDestinationDevice, int iStream_ID, bool bOnOff);
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
