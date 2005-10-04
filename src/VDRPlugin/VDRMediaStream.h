//
// C++ Interface: Xine_Plugin
//
// Description:
//
//
// Author: Toader Mihai Claudiu, mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VDRMediaStream_H
#define VDRMediaStream_H

#include "../Media_Plugin/MediaStream.h"

namespace DCE
{
	/**
	 * A media stream that incapsulates TV and channel informations.
	 */
	class VDRMediaStream : public MediaStream
	{
	public:
		class VDRStateInfo *m_pVDRStateInfo;
		class VDRPlugin *m_pVDRPlugin;

		VDRMediaStream(VDRPlugin *pVDRPlugin,
						MediaHandlerInfo *pMediaHandlerInfo,
						  int iPK_MediaProvider,
		 				  MediaDevice *pMediaDevice,
						  int PK_Users,
						  enum SourceType sourceType,
						  int iStreamID);

		virtual ~VDRMediaStream();
        virtual void UpdateDescriptions(bool bAllFiles=false,MediaFile *pMediaFile_In=NULL);
	};
};

#endif
