//
// C++ Interface: VideoLan_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEVIDEOLANMEDIASTREAM_H
#define DCEVIDEOLANMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
	using namespace std;

	class VideoLanMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			VideoLanMediaStream(class VideoLan_PlugIn *pVideoLanPlugin, class MediaHandlerInfo *pMediaHandlerInfo, MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~VideoLanMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();
			bool isStreaming();
			void setIsStreaming(bool isStreaming = true);

			bool CanPlayMore();

			int SpecialOsdScreen();
	};

};

#endif
