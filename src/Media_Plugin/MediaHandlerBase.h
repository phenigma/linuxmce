#ifndef MediaHandlerBase_h
#define MediaHandlerBase_h

#include "EntertainArea.h"

/**
* A Media Handler is derived from the Media Handler abstract class.  When it registers, it passes in a MediaHandlerInfo pointer indicating
* what type of media it can play.  It may register several times with different types of media and different capabilities.
*/

namespace DCE
{

	/** All media plugins must implement this class */
	class MediaHandlerBase
	{
	public:
		class Media_Plugin *m_pMedia_Plugin;

		/** @brief constructor */
		MediaHandlerBase() { m_iPriority=5; }

		int m_iPriority; // If multiple media handlers can handle the same type of media in an area the one with the highest priority will be chosen
		vector<class MediaHandlerInfo *> m_vectMediaHandlerInfo; // The media handler info's we have

		/** @brief Each Plugin will create its own instance of MediaStream, so it can create a derived version with extra information */
		virtual class MediaStream *CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo,vector<class EntertainArea *> &vectEntertainArea,MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeMediaFile,int StreamID)=0;
		virtual bool StartMedia(class MediaStream *pMediaStream)=0;
		virtual bool StopMedia(class MediaStream *pMediaStream)=0;
		virtual bool BroadcastMedia(class MediaStream *pMediaStream)=0;

		// Given a stream, what is the rendering device(s).  The source device is stored in the stream.  Normally the source and rendering are the same (dvd player, for example).
		// But sometimes the source may be a back-end streamer, and the rendering device(s) some network audio players.  The framework needs
		// to know the rendering devices so it can send on/off's to them.  If the source & dest aren't the same, the plug-in must override this
		virtual void GetRenderDevices(EntertainArea *pEntertainArea,map<int,MediaDevice *> *pmapMediaDevice);
		void GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices);  // In all entertainment areas

		// it can't virtual and static at the same time.
		virtual MediaDevice *GetMediaDeviceForEntertainArea(EntertainArea *pEntertainArea, int PK_DeviceTemplate)
		{
			list<MediaDevice*> *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(PK_DeviceTemplate);
			if( pListMediaDevice && pListMediaDevice->size())
				return pListMediaDevice->front();
			else
				return NULL;
		}
	};

}

#endif
