//
// C++ Implementation: VDR_Plugin (the Media stream )
//
// Description:
//
//

//
#include "VDRPlugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include <map>

#include <iostream>
using namespace std;
using namespace DCE;
#include "VDRMediaStream.h"
#include "VDRStateInfo.h"
#include "EPG.h"

namespace DCE
{
 	VDRMediaStream::VDRMediaStream(VDRPlugin *pVDRPlugin,
								MediaHandlerInfo *pMediaHandlerInfo,
								int iPK_MediaProvider,
								MediaDevice *pMediaDevice,
								int PK_Users,
								enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users, sourceType, iStreamID)
	{
		m_pVDRStateInfo = NULL;
		m_pVDRPlugin=pVDRPlugin;
	}

	VDRMediaStream::~VDRMediaStream()
	{
		// No op. Is here to keep the compiler happy.
	}

	void VDRMediaStream::UpdateDescriptions(bool bAllFiles,MediaFile *pMediaFile_In)
	{
		PLUTO_SAFETY_LOCK(vm,m_pVDRPlugin->m_VDRMutex);
		VDREPG::EPG *pEPG = m_pVDRPlugin->m_mapEPG_Find(m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
		if( !pEPG )
			pEPG = m_pVDRPlugin->m_mapEPG.begin()->second;

		VDREPG::Event *pEvent;
		if( !pEPG || !m_pVDRStateInfo || (pEvent=pEPG->m_mapEvent_Find(m_pVDRStateInfo->EventID))==NULL )
			m_sMediaDescription = "Unknown";
		else
		{
			m_sMediaDescription = StringUtils::itos(pEvent->m_pChannel->m_ChannelID) + " " + pEvent->m_pChannel->m_sChannelName;
			m_sSectionDescription = pEvent->m_pProgram->m_sTitle;
			if( pEvent->m_sDescription_Long.size() )
				m_sMediaSynopsis = pEvent->m_sDescription_Long;
			else
				m_sMediaSynopsis = pEvent->m_sDescription_Short;
			m_sTimecode = pEvent->GetShortShowtime();
		}
	}
};
