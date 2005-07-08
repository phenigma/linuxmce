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

	};

};

#endif
