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
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

	};
}
#endif
