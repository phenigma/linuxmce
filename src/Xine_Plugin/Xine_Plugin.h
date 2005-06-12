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
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "../Media_Plugin/MediaPosition.h"

#include "XineMediaStream.h"

//<-dceag-decl-b->! custom
namespace DCE
{

	class Xine_Plugin : public Xine_Plugin_Command, public MediaHandlerBase
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


//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
