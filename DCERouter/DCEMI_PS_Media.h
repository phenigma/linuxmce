#ifndef DCEMI_PS_Media_h
#define DCEMI_PS_Media_h

#include "PlugIn.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include "MediaStream.h"

class MediaHandlerPlugIn {
};

// Before creating a new DCEMediaStream, the framework will call CreateMediaStream so that the plug-in
// can create instead a derived class extending the information within a DCEMediaStream
// Parameters: listEntZone are all the entertainment zones where the media will initially be started,
//		pDevice is the source device.  Filename and MediaType are optional.
typedef class DCEMediaStream *(MediaHandlerPlugIn::*CreateMediaStreamFn)(listEntZone *plistEntZone,DeviceData_Router *pDevice,string sFilename,int PK_C_MediaType);

// The following functions will be called to start the media, stop it, and move it
// Note that if StopMedia is called with bPark=true, the DCEMediaStream pointer will not be destroyed, and 
// there may be a subsequent call to StartMedia to restart the same media in a different location.
// Therefore, StopMedia with bPark=true, must save the current position of the media so that it can
// be resumed at the same point.  All functions return true to indicate success.
typedef  bool (MediaHandlerPlugIn::*StartMediaFn)(class DCEMediaStream *,listEntZone *plistEntZone,DeviceData_Router *pDevice);
typedef  bool (MediaHandlerPlugIn::*StopMediaFn)(class DCEMediaStream *,bool bPark);
typedef  bool (MediaHandlerPlugIn::*MoveMediaFn)(class DCEMediaStream *,listEntZone *plistEntZone);


// Contains a pointer to the Media callback
class MediaHandlerCallBack
{
public:
	class MediaHandlerPlugIn *m_pMediaHandlerPlugIn;
	CreateMediaStreamFn m_pCreateMediaStreamFn;
	StartMediaFn m_pStartMediaFn;
	StopMediaFn m_pStopMediaFn;
	MoveMediaFn m_pMoveMediaFn;
	
	string m_sFileExtensions;
	int m_PK_DeviceTemplate,m_PK_DeviceCategory;
	listMediaTypes *m_pListMediaTypes;

	MediaHandlerCallBack(class MediaHandlerPlugIn *pMediaHandlerPlugIn,CreateMediaStreamFn pCreateMediaStreamFn,
		StartMediaFn pStartMediaFn, StopMediaFn pStopMediaFn, MoveMediaFn pMoveMediaFn,
		string sFileExtensions,int PK_DeviceTemplate,
		int PK_DeviceCategory,listMediaTypes *pListMediaTypes)
	{
		m_pMediaHandlerPlugIn=pMediaHandlerPlugIn;
		m_pCreateMediaStreamFn=pCreateMediaStreamFn;
		m_pStartMediaFn=pStartMediaFn;
		m_pStopMediaFn=pStopMediaFn;
		m_pMoveMediaFn=pMoveMediaFn;
		m_sFileExtensions=StringUtils::ToUpper(sFileExtensions);
		m_PK_DeviceTemplate=PK_DeviceTemplate;
		m_PK_DeviceCategory=PK_DeviceCategory;
		m_pListMediaTypes=pListMediaTypes;
	}

	bool ContainsMediaType(int PK_C_MediaType)
	{
		if( !m_pListMediaTypes )
			return false;

		listMediaTypes::iterator it;
		for(it=m_pListMediaTypes->begin();it!=m_pListMediaTypes->end();++it)
		{
			class MediaType *p = (*it);
			if( p->m_PK_C_MediaType==PK_C_MediaType )
				return true;
		}
		return false;
	}
};


class DCEMI_PS_Media : public PlugIn
{
public:
	class DCEMI_PS_Main *m_pDCEMI_PS_Main;
	pluto_pthread_mutex_t m_MediaMutex;
	class MediaAttributes *m_pMediaAttributes;

	mapEntZone m_mapEntZone;
	class EntZone *m_mapEntZone_Find(int PK_Device)
	{
		map<int,EntZone *>::iterator it = m_mapEntZone.find(PK_Device);
		return it==m_mapEntZone.end() ? NULL : (*it).second;
	}

	list<class DCEMediaStream *> m_listDCEMediaStream;
	list<class MediaHandlerCallBack *> m_listMediaHandlerCallBack;

	void RegisterMediaHandler(class MediaHandlerCallBack *pCallBack);
	mapMediaType m_mapMediaType;
	class MediaType *m_mapMediaType_Find(int PK_C_MediaType)
	{
		map<int,class MediaType *>::iterator it = m_mapMediaType.find(PK_C_MediaType);
		return it==m_mapMediaType.end() ? NULL : (*it).second;
	}

	// Required overrides
	DCEMI_PS_Media(class Router *pRouter) : PlugIn(pRouter), m_MediaMutex("Media") {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Media_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Media interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool CreatedMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool StartMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool StopMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool MoveMedia(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif
