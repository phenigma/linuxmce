//
// C++ Implementation: MythTV_Plugin (the Media stream )
//
// Description:
//
//
// Author: Toader Mihai Claudiu, mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <map>

using namespace std;

#include "MythTvMediaStream.h"

namespace DCE
{
 	MythTvMediaStream::MythTvMediaStream(
								MediaHandlerInfo *pMediaHandlerInfo,
								DeviceData_Router *pDeviceData_Router,
								int PK_DesignObj_Remote,
								int PK_Users,
								enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaHandlerInfo, pDeviceData_Router, PK_DesignObj_Remote, PK_Users, sourceType, iStreamID)
	{
		// m_pMythTV_PlugIn=pMythTV_PlugIn;
	}

	MythTvMediaStream::~MythTvMediaStream()
	{
		// No op. Is here to keep the compiler happy.
	}

	int MythTvMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_MYTHTV;
	}
};
