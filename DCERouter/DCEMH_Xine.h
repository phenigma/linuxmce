#ifndef DCEMH_Xine_h
#define DCEMH_Xine_h

#include "MediaStream.h"
#include "DCEMI_PS_Media.h"

class XineMediaStream : public DCEMediaStream
{
public:
	XineMediaStream(listEntZone *plistEntZone,DeviceData_Router *pDeviceData_Router,
		string sFilename,int PK_C_MediaType) 
		: DCEMediaStream(plistEntZone,pDeviceData_Router,sFilename,PK_C_MediaType) {}
};

class DCEMH_Xine : public MediaHandlerPlugIn
{
public:
	class DCEMI_PS_Media *m_pDCEMI_PS_Media;
	DCEMH_Xine(class Router *pRouter);
	bool Register();

	class DCEMediaStream *CreateMediaStream(listEntZone *plistEntZone,DeviceData_Router *pDevice,string sFilename,int PK_C_MediaType);
	bool StartMedia(class DCEMediaStream *,listEntZone *plistEntZone,DeviceData_Router *pDevice);
	bool StopMedia(class DCEMediaStream *,bool bPark);
	bool MoveMedia(class DCEMediaStream *,listEntZone *plistEntZone);
};

#endif
