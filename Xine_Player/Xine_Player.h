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

//  DCE Implemenation for #5 Xine Player

#include "Gen_Devices/Xine_PlayerBase.h"
//<-dceag-d-e->

#include "XineSlaveWrapper.h"
#include "utilities/linux/RatpoisonHandler.h"

//<-dceag-decl-b->! custom

namespace DCE
{

/**
 * @brief
 */

class Xine_Player : public Xine_Player_Command, public RatpoisonHandler<Xine_Player>
{
//<-dceag-decl-e->
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

    /**
     * @brief destructor
     */

    XineSlaveWrapper *m_pXineSlaveControl; /**< points to the XineSlaveWrapper object */

    /**
     * @brief destructor
     */
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
    void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
    void EVENT_Menu_Onscreen(int iStream_ID,bool bOnOff);
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


    /** @brief COMMAND: #63 - Skip Forward */
    /** Chapter/Track Next/Down/Forward */

    virtual void CMD_Skip_Forward() { string sCMD_Result; CMD_Skip_Forward(sCMD_Result,NULL);};
    virtual void CMD_Skip_Forward(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #64 - Skip Back */
    /** Chapter/Track Back/Up/Prior */

    virtual void CMD_Skip_Back() { string sCMD_Result; CMD_Skip_Back(sCMD_Result,NULL);};
    virtual void CMD_Skip_Back(string &sCMD_Result,Message *pMessage);


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


    /** @brief COMMAND: #83 - Select Current Navigable Area */
    /** Mark the selected area as "clicked". */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

    virtual void CMD_Select_Current_Navigable_Area(int iStreamID) { string sCMD_Result; CMD_Select_Current_Navigable_Area(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Select_Current_Navigable_Area(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #84 - Get Video Frame */
    /** Get's the current video frame from the media player. */
        /** @param #19 Data */
            /** The video frame */
        /** @param #20 Format */
            /** One of the following: "jpg", "png" */
        /** @param #23 Disable Aspect Lock */
            /** If true, don't worry about the aspect ratio.  Try to get the requested width and height. */
        /** @param #41 StreamID */
            /** Optional.  For multi stream devices, like media players, this identifies the stream. */
        /** @param #60 Width */
            /** The desired width of the video frame.  The sender need not respect this. */
        /** @param #61 Height */
            /** The desired height of the video frame.  The sender need not respect this. */

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


    /** @brief COMMAND: #243 - Enable Broadcasting */
    /** Enable broadcasting from here. */
        /** @param #41 StreamID */
            /** The stream to enable broadcast for */
        /** @param #59 MediaURL */
            /** The media url from which this stream can be played. */

    virtual void CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL) { string sCMD_Result; CMD_Enable_Broadcasting(iStreamID,sMediaURL,sCMD_Result,NULL);};
    virtual void CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage);


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
