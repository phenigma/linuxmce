#include "PlutoUtils/CommonIncludes.h"	
#include "Router.h"
#include "DCEMH_Xine.h"
#include "../pluto_main/Table_DeviceTemplate.h"

bool DCEMH_Xine::Register()
{
	return true;
	/*
	m_pDCEMI_PS_Media = (DCEMI_PS_Media *) m_pRouter->m_mapPlugIn_Find(DEVICETEMPLATE_SERVER_CONST);
	if( m_pDCEMI_PS_Media )
	{
		m_pDCEMI_PS_Media->RegisterMediaHandler(
			new MediaHandlerCallBack(this,(CreateMediaStreamFn)(&DCEMH_Xine::CreateMediaStream),
			(StartMediaFn)(&DCEMH_Xine::StartMedia),(StopMediaFn)(&DCEMH_Xine::StopMedia),
			(MoveMediaFn)(&DCEMH_Xine::MoveMedia),".DVD,.MP3,.FLAC,.WAV.,.VOB,.PPL",DEVICETEMPLATE_XINE_CONST,
			0,NULL)); // Create the real list of media types
	}
	*/
}

class DCEMediaStream *DCEMH_Xine::CreateMediaStream(listEntZone *plistEntZone,DeviceData_Router *pDevice,string sFilename,int PK_C_MediaType)
{
	return new XineMediaStream(plistEntZone,pDevice,sFilename,PK_C_MediaType);
}

bool DCEMH_Xine::StartMedia(class DCEMediaStream *,listEntZone *plistEntZone,DeviceData_Router *pDevice)
{
	return true;
}

bool DCEMH_Xine::StopMedia(class DCEMediaStream *,bool bPark)
{
	return true;
}

bool DCEMH_Xine::MoveMedia(class DCEMediaStream *,listEntZone *plistEntZone)
{
	return true;
}
