#ifndef DCEMediaStream_h
#define DCEMediaStream_h

typedef list<class EntZone *> listEntZone;
typedef map<int,class EntZone *> mapEntZone;

class EntZone
{
public:
	class DCEMediaStream *m_pDCEMediaStream;
	int m_PK_Device;
	EntZone(int PK_Device)
	{
		m_PK_Device=PK_Device;
		m_pDCEMediaStream=NULL;
	}
};

class MediaStream
{
public:
	listEntZone *m_plistEntZone;
	DeviceData_Router *m_pDeviceData_Router;
	string m_sFilename;
	int m_PK_C_MediaType;
	int m_PK_DesignObj_Remote;  // What screen to use as the remote control
	bool m_bPlaying;

	MediaStream(listEntZone *plistEntZone,DeviceData_Router *pDeviceData_Router,
		string sFilename,int PK_C_MediaType)
	{
		m_plistEntZone=plistEntZone;
		m_pDeviceData_Router=pDeviceData_Router;
		m_sFilename=sFilename;
		m_PK_C_MediaType=PK_C_MediaType;
		m_bPlaying=false;
	}
};

#endif
