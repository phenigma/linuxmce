//<-dceag-d-b->
#ifndef Xine_Player_h
#define Xine_Player_h

//	DCE Implemenation for #5 Xine Player

#include "Gen_Devices/Xine_PlayerBase.h"
//<-dceag-d-e->

#include "XineSlaveWrapper.h"
#include "utilities/linux/RatpoisonHandler.h"

//<-dceag-decl-b->! custom
namespace DCE
{
	
class Xine_Player : public Xine_Player_Command, public RatpoisonHandler<Xine_Player>
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Xine_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Xine_Player();
		virtual bool Register();
//<-dceag-const-e->
    // Private member variables
    XineSlaveWrapper *m_pXineSlaveControl;

    Display *getDisplay();
    // Private methods
public:
    // Public member variables

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Events();
	void EVENT_Menu_Onscreen(int iStream_ID,bool bOnOff);

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #37 - Play Media
	COMMENTS: This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command.
	PARAMETERS:
		#13 Filename
			The file to play.  The format is specific on the media type and the media player.
		#29 PK_MediaType
			The type of media
		#41 StreamID
			The media that we need to play.
		#42 MediaPosition
			The position at which we need to start playing.
*/
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { string sCMD_Result; CMD_Play_Media(sFilename.c_str(),iPK_MediaType,iStreamID,iMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #38 - Stop Media
	COMMENTS: This will instruct the media player to stop the playback of a media started with the "Play Media" Command
	PARAMETERS:
		#41 StreamID
			The media needing to be stopped.
*/
	virtual void CMD_Stop_Media(int iStreamID) { string sCMD_Result; CMD_Stop_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #39 - Pause Media
	COMMENTS: This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device.
	PARAMETERS:
		#41 StreamID
			The media stream for which we need to pause playback.
*/
	virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #40 - Restart Media
	COMMENTS: This will restart a media was paused with the above command
	PARAMETERS:
		#41 StreamID
			The media stream that we need to restart playback for.
*/
	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #41 - Change Playback Speed
	COMMENTS: Will make the playback to FF with a configurable amount of speed.
	PARAMETERS:
		#41 StreamID
			The media needing the playback speed change.
		#43 MediaPlaybackSpeed
			The requested media playback speed. This is a multiplier of the normal speed. (If we want 2x playback this parameter will be 2 if we want half of normal speed then the parameter will be 0.5). The formula is NextSpeed = MediaPlaybackSpeed * NormalPlaybackS
*/
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #63 - Skip Forward
	COMMENTS: Chapter/Track Next/Down/Forward
	PARAMETERS:
*/
	virtual void CMD_Skip_Forward() { string sCMD_Result; CMD_Skip_Forward(sCMD_Result,NULL);};
	virtual void CMD_Skip_Forward(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #64 - Skip Back
	COMMENTS: Chapter/Track Back/Up/Prior
	PARAMETERS:
*/
	virtual void CMD_Skip_Back() { string sCMD_Result; CMD_Skip_Back(sCMD_Result,NULL);};
	virtual void CMD_Skip_Back(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #81 - Navigate Next
	COMMENTS: Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device)
	PARAMETERS:
		#41 StreamID
			The stream on which to do the navigation.
*/
	virtual void CMD_Navigate_Next(int iStreamID) { string sCMD_Result; CMD_Navigate_Next(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #82 - Navigate Prev
	COMMENTS: Nagivate the previous possible navigable area. (The actual outcome depends on the specific device).
	PARAMETERS:
		#41 StreamID
			The stream on which to do the navigation.
*/
	virtual void CMD_Navigate_Prev(int iStreamID) { string sCMD_Result; CMD_Navigate_Prev(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #83 - Select Current Navigable Area
	COMMENTS: Mark the selected area as "clicked".
	PARAMETERS:
		#41 StreamID
			The stream on which to do the navigation.
*/
	virtual void CMD_Select_Current_Navigable_Area(int iStreamID) { string sCMD_Result; CMD_Select_Current_Navigable_Area(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Select_Current_Navigable_Area(int iStreamID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #84 - Get Video Frame
	COMMENTS: Get's the current video frame from the media player.
	PARAMETERS:
		#19 Data
			The video frame
		#20 Format
			One of the following: "jpg", "png"
		#23 Disable Aspect Lock
			If true, don't worry about the aspect ratio.  Try to get the requested width and height.
		#41 StreamID
			Optional.  For multi stream devices, like media players, this identifies the stream.
		#60 Width
			The desired width of the video frame.  The sender need not respect this.
		#61 Height
			The desired height of the video frame.  The sender need not respect this.
*/
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #87 - Goto Media Menu
	COMMENTS: Goto to the current media Root Menu.
	PARAMETERS:
		#41 StreamID
			The stream ID
		#64 MenuType
			The type of menu that the user want to jump to.
(For DVD handlers usually this applies)
0 - Root menu 
1 - Title menu
2 - Media menu
*/
	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType) { string sCMD_Result; CMD_Goto_Media_Menu(iStreamID,iMenuType,sCMD_Result,NULL);};
	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

    virtual void FireMenuOnScreen(int iDestinationDevice, int iStream_ID, bool bOnOff);
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
