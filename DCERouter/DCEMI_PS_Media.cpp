#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCEMI_PS_Media.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "MediaAttributes.h"

// temp
#define COMMAND_CREATED_MEDIA_CONST 9999
#define COMMAND_START_MEDIA_CONST 9999
#define COMMAND_STOP_STREAM_CONST 9999
#define COMMAND_MOVE_MEDIA_CONST 9999
#define C_COMMANDPARAMETER_GRAPHIC_IMAGE_CONST 9999
#define C_COMMANDPARAMETER_PATH_CONST 9999
#define C_COMMANDPARAMETER_FILE_NAME_CONST 99999
#define C_COMMANDPARAMETER_MEDIA_URL_CONST 9999

bool DCEMI_PS_Media::Register()
{
	m_pDCEMI_PS_Main = (class DCEMI_PS_Main *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Main_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Media::CreatedMedia))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_CREATED_MEDIA_CONST);
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Media::StartMedia))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_START_MEDIA_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Media::StopMedia))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_STOP_STREAM_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Media::MoveMedia))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_MOVE_MEDIA_CONST);

	m_pMediaAttributes = new MediaAttributes(this);

	return true;
}


bool DCEMI_PS_Media::CreatedMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	listMediaAttribute *listMA = MediaAttributes::AttributesFromString(pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST]);
	listMediaPicture *listMP = MediaAttributes::PicturesFromString(pMessage->m_mapParameters[C_COMMANDPARAMETER_GRAPHIC_IMAGE_CONST]);
	m_pMediaAttributes->CreatedMedia(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Type_CONST].c_str()),
		pMessage->m_mapParameters[C_COMMANDPARAMETER_PATH_CONST],
		listMA,listMP);
	delete listMA;
	delete listMP;
	return true;
}


bool DCEMI_PS_Media::StartMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	PLUTO_SAFETY_LOCK(mm,m_MediaMutex);

	// See what type of media we're supposed to start
	int MediaType=0,PK_Device=0;
	string FileName="";
	map<long, string>::iterator it;
	for(it=pMessage->m_mapParameters.begin();it!=pMessage->m_mapParameters.end();++it)
	{
		if( (*it).first==COMMANDPARAMETER_Type_CONST )
			MediaType = atoi((*it).second.c_str());
		else if( ((*it).first==C_COMMANDPARAMETER_FILE_NAME_CONST || (*it).first==C_COMMANDPARAMETER_MEDIA_URL_CONST)
			&& (*it).second!="")
			FileName = (*it).second;
		else if( (*it).first==COMMANDPARAMETER_PK_Device_CONST )
			PK_Device = atoi((*it).second.c_str());
	}

	// When designer is working again, we need a consistent way to specify multiple ent zones.  For now hack it in
	listEntZone *plistEntZone = new listEntZone();
//	controllers_register_their_ent_group();

	MediaHandlerCallBack *pMHCB=NULL,*pMHCB_Type=NULL,*pMHCB_Filename=NULL;
	DeviceData_Router *pDevice = PK_Device ? m_pRouter->m_mapDeviceData_Router_Find(PK_Device) : NULL;

	// Now find the best media handler.  If there's an exact match on the DeviceData_Router, that's perfect
	// Otherwise match the media type, and lastly the file name
	list<class MediaHandlerCallBack *>::iterator itmh;
	for(itmh=m_listMediaHandlerCallBack.begin();
		itmh!=m_listMediaHandlerCallBack.end();++itmh)
	{
		MediaHandlerCallBack *p = (*itmh);
		if( pDevice && p->m_PK_DeviceTemplate==pDevice->m_iPK_DeviceTemplate )
		{
			pMHCB=p;
			break;
		}
		if( MediaType && p->ContainsMediaType(MediaType) )
			pMHCB_Type=p;
		else if( FileName.length()>3 && p->m_sFileExtensions.find(StringUtils::ToUpper(FileUtils::FindExtension(FileName)))!=string::npos )
			pMHCB_Filename=p;
	}
	if( !pMHCB )
	{
		if( pMHCB_Type )
			pMHCB=pMHCB_Type;
		else
			pMHCB=pMHCB_Filename;
	}

	if( !pMHCB )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got Request to play media: %s type: %d device: %d, but have no handler",
			FileName.c_str(),PK_Device,MediaType);
		return true;
	}

	// Got the media handler
	DCEMediaStream *pStream = 
		CALL_MEMBER_FN(*pMHCB->m_pMediaHandlerPlugIn,pMHCB->m_pCreateMediaStreamFn) (plistEntZone,pDevice,FileName,MediaType);

	if( !pStream )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got Request to play media: %s type: %d device: %d, but handler didn't create stream",
			FileName.c_str(),PK_Device,MediaType);
		return true;
	}

	if( !CALL_MEMBER_FN(*pMHCB->m_pMediaHandlerPlugIn,pMHCB->m_pStartMediaFn)(pStream,plistEntZone,pDevice) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got Request to play media: %s type: %d device: %d, but handler didn't start stream",
			FileName.c_str(),PK_Device,MediaType);
		return true;
	}

	// The media is playing
	pStream->m_bPlaying=true;
	m_listDCEMediaStream.push_back(pStream);

	return true;
}

bool DCEMI_PS_Media::StopMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	return true;
}

bool DCEMI_PS_Media::MoveMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	return true;
}
