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
#ifndef DCEMYTHTVMEDIASTREAM_H
#define DCEMYTHTVMEDIASTREAM_H

#include "../Media_Plugin/MediaStream.h"

namespace DCE
{
	/**
	 * A media stream that incapsulates TV and channel informations.
	 */
	class MythTvMediaStream : public MediaStream
	{
		//int m_iCurrentShow;

		// class MythTV_PlugIn *m_pMythTV_PlugIn;

	public:
		MythTvMediaStream(MediaHandlerInfo *pMediaHandlerInfo,
		 				  DeviceData_Router *pDeviceData_Router,
						  int PK_DesignObj_Remote, int PK_Users,
						  enum SourceType sourceType,
						  int iStreamID);

		virtual ~MythTvMediaStream();

		virtual int GetType();

		void GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices);
	};
};

#endif
