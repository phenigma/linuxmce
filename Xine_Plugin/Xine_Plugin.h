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
        virtual bool StartStreaming(class XineMediaStream *m_pMediaStream, string strTargetDevices);

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

	XineMediaStream *ConvertToXineMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");
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


//<-dceag-h-e->
};


class XineMediaStream : public MediaStream
{
    private:
        map<int, MediaDevice *> m_mapEntertainmentAreasToDevices;
		bool					m_bIsStreaming;

    public:
        class Xine_Plugin *m_pXinePlugin;

        XineMediaStream(class Xine_Plugin *pXinePlugin, class MediaPluginInfo *pMediaPluginInfo, DeviceData_Router *pDeviceData_Router, int PK_DesignObj_Remote, int PK_Users,enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaPluginInfo, pDeviceData_Router, PK_DesignObj_Remote, PK_Users,sourceType, iStreamID) { m_pXinePlugin = pXinePlugin; m_bIsStreaming = 0; }

		virtual ~XineMediaStream();
        virtual int GetType() { return MEDIASTREAM_TYPE_XINE; }

		bool ShouldUseStreaming();
		bool isStreaming();
		void setIsStreaming(bool isStreaming = true);

		MediaDevice *GetPlaybackDeviceForEntArea(int entAreaId);
		void SetPlaybackDeviceForEntArea(int entAreaId, MediaDevice *pMediaDevice);

		class XineMediaPosition : public MediaPosition
		{
			public:
				// data related to stream position. I'm not sure we even need it here because we can always ask the device
				// for it. If a device crashes then we might need to keep the data here.
        		int             	m_iSavedPosition;
				int					m_iTotalStreamTime;
				string 				m_sSavedPosition;

				XineMediaPosition();

				virtual ~XineMediaPosition();

				virtual void Reset();

				virtual string GetID();
		};

		XineMediaPosition *GetMediaPosition();
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
