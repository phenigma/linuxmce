/**
 *
 * @file Xine_Plugin.h
 * @brief header file for the Xine_Plugin class
 * @author
 *
 */


//<-dceag-d-b->
#ifndef Xine_Plugin_h
#define Xine_Plugin_h

//	DCE Implemenation for #6 Xine Plug-in

#include "Gen_Devices/Xine_PluginBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"

//<-dceag-decl-b->! custom

namespace DCE
{

class Xine_Plugin : public Xine_Plugin_Command, public MediaPluginBase
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Xine_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Xine_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
  // Private member variables

protected:
        virtual void StartStreaming(class XineMediaStream *m_pMediaStream, string strTargetSqueezeBoxesMacAddresses);

  // Private methods
public:
  // Public member variables

  /** Mandatory implementations */

    /**
    * @brief
    */
    virtual class MediaStream *CreateMediaStream( class MediaPluginInfo *pMediaPluginInfo, class EntertainArea *pEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

    /**
    * @brief Start media playback
    */
    virtual bool StartMedia( class MediaStream *pMediaStream );

    /**
    * @brief Stop media playback
    */
    virtual bool StopMedia( class MediaStream *pMediaStream );

    /**
    * @brief This will set the media to broadcast mode. (Is this still needed) ?
    */
    virtual bool BroadcastMedia( class MediaStream *pMediaStream );

    /**
    * @brief Called by the Media Plugin when we need to move media around in entertainment areas. The media plugin will pass 3 lists:
    *          < listStart: the list on which media needs to be started. >
    *          < listStop: the list on which media needs tobe stopped >
    *          < listChange: the list on which media needs to be changed (if we move the media from one place to another it might be possible that the actual source of the stream to change). >
    */
    virtual bool MoveMedia(class MediaStream *pMediaStream, list<EntertainArea*> &listStart, list<EntertainArea *> &listStop, list<EntertainArea *> &listChange);
    /**
    * @brief Test if a Media Stream is compatible with me.
    */
    virtual bool isValidStreamForPlugin(class MediaStream *pMediaStream);

    virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);

    /**
    * @brief We need to see all media inserted events so we can start the appropriate media devices
    */

    bool MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #36 - Create Media */
	/** Create a media stream descriptor. */
		/** @param #13 Filename */
			/** The filename of the media stream. */
		/** @param #41 StreamID */
			/** The media descriptor which will be associated with the current media. */

	virtual void CMD_Create_Media(string sFilename,int iStreamID) { string sCMD_Result; CMD_Create_Media(sFilename.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Create_Media(string sFilename,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #37 - Play Media */
	/** Play a media stream descriptor. */
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
	/** Stop playing a media stream descriptor. */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

	virtual void CMD_Stop_Media(int iStreamID,int *iMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,iMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #39 - Pause Media */
	/** Pause a media playback. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

	virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #40 - Restart Media */
	/** Restart a media playback. */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Change the playback speed of a media stream. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed. This is a multiplier of the normal speed. (If we want 2x playback this parameter will be 2 if we want half of normal speed then the parameter will be 0.5). The formula is NextSpeed = MediaPlaybackSpeed * NormalPlaybackS */

	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jumps to a position within some media, such as songs in a playlist, tracks on a cd, etc.  It will assume the sender is an orbiter, and find the entertainment area and stream associated with it.  The track can be an absolute or relative position. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};


class XineMediaStream : public MediaStream
{
    private:
        int      m_dwStreamer;

    public:
        class Xine_Plugin *m_pXinePlugin;

        XineMediaStream(class Xine_Plugin *pXinePlugin, class MediaPluginInfo *pMediaPluginInfo, MediaDevice *pMediaDevice, int PK_DesignObj_Remote, int PK_Users,enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaPluginInfo, pMediaDevice, PK_DesignObj_Remote, PK_Users,sourceType, iStreamID) { m_pXinePlugin = pXinePlugin; m_dwStreamer = 0; }

        virtual int GetType() { return MEDIASTREAM_TYPE_XINE; }

        int getStreamerDeviceID();
        void setStreamerDeviceID(int deviceID);
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
