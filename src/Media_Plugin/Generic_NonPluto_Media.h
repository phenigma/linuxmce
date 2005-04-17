/**
 *
 * @file Generic_NonPluto_Media.h
 * @brief header file for the Generic_NonPluto_Media class
 * @author
 *
 */


#ifndef Generic_NonPluto_Media_h
#define Generic_NonPluto_Media_h


#include "Media_Plugin.h"
#include "MediaStream.h"
#include "MediaHandlerBase.h"
#include "MediaPosition.h"

namespace DCE
{

	class Generic_NonPluto_Media : public MediaHandlerBase
	{
public:
		// Constructors/Destructor
		Generic_NonPluto_Media(Media_Plugin *pMedia_Plugin);
		virtual ~Generic_NonPluto_Media();

	protected:

	/** Mandatory implementations */

		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, class EntertainArea *pEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual bool BroadcastMedia(class MediaStream *pMediaStream) { return true; }

		/**
		* @brief Called by the Media Plugin when we need to move media around in entertainment areas. The media plugin will pass 3 lists:
		*          < listStart: the list on which media needs to be started. >
		*          < listStop: the list on which media needs tobe stopped >
		*          < listChange: the list on which media needs to be changed (if we move the media from one place to another it might be possible that the actual source of the stream to change). >
		*/
		virtual bool MoveMedia(class MediaStream *pMediaStream, list<EntertainArea*> &listStart, list<EntertainArea *> &listStop, list<EntertainArea *> &listChange);

//		virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);
		/**
		* @brief We need to see all media inserted events so we can start the appropriate media devices
		*/

	//	void GetRenderDevices(MediaStream *pMediaStream, map<int,MediaDevice *> *pmapMediaDevice);
	};
}
#endif
