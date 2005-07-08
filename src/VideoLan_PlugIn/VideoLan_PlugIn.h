//<-dceag-d-b->
#ifndef VideoLan_PlugIn_h
#define VideoLan_PlugIn_h

//	DCE Implemenation for #1696 VideoLan Plug-In

#include "Gen_Devices/VideoLan_PlugInBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"

#include "VideoLanMediaStream.h"

//<-dceag-decl-b->!
namespace DCE
{
	class VideoLan_PlugIn : public VideoLan_PlugIn_Command, public MediaHandlerBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VideoLan_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VideoLan_PlugIn();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		VideoLan_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->


			protected:
		virtual bool StartStreaming(class VideoLanMediaStream *m_pMediaStream);
		virtual bool StopStreaming(class VideoLanMediaStream *pVideoLanMediaStream, std::vector<MediaDevice*> *stopStreamingTargets);

		class Orbiter_Plugin *m_pOrbiter_Plugin;

	// Private methods
	public:
	// Public member variables

	/** Mandatory implementations */

		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);

		VideoLanMediaStream *ConvertToVideoLanMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

		MediaDevice *FindStreamerDevice();


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
