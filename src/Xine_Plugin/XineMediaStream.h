//
// C++ Interface: Xine_Plugin
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEXINEMEDIASTREAM_H
#define DCEXINEMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
	using namespace std;

	class XineMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			int 	m_iPK_DesignObj_Remote_After_Menu; 		/** Store the real value if we're switching to a menu */
			int 	m_iPK_DesignObj_RemoteOSD_After_Menu;
	        int 	m_iPK_DesignObj_Remote_Popup_After_Menu;

			XineMediaStream(class Xine_Plugin *pXinePlugin, class MediaHandlerInfo *pMediaHandlerInfo, MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~XineMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();

			bool CanPlayMore();
	};

};

#endif
