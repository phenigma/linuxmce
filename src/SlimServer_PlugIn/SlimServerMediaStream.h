//
// C++ Interface: SlimServer_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCESLIMSERVERMEDIASTREAM_H
#define DCESLIMSERVERMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"
#include "../Media_Plugin/MediaPosition.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
	using namespace std;

	class SlimServerMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			SlimServerMediaStream(class SlimServer_PlugIn *pSlimServerPlugin, class MediaHandlerInfo *pMediaHandlerInfo, MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~SlimServerMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();
			bool isStreaming();
			void setIsStreaming(bool isStreaming = true);

			bool CanPlayMore();

			class SlimServerMediaPosition : public MediaPosition
			{
				public:
					// data related to stream position. I'm not sure we even need it here because we can always ask the device
					// for it. If a device crashes then we might need to keep the data here.
					int             	m_iSavedPosition;
					int					m_iTotalStreamTime;
					string 				m_sSavedPosition;

					SlimServerMediaPosition();
					virtual ~SlimServerMediaPosition();
					virtual void Reset();
					virtual string GetID();
			};

			SlimServerMediaPosition *GetMediaPosition();
	};

};

#endif
