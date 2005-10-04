#ifndef VDRStateInfo_h
#define VDRStateInfo_h

namespace DCE
{
	class VDRMediaStream;
};

class VDRStateInfo
{
public:
	int m_dwPK_Device;
	int EventID; // The current event
	int m_dwPK_Users;
	bool m_bViewing,m_bRecording;
	VDRMediaStream *m_pVDRMediaStream;

	// When a new VDR player is started, these are all set to false
	// so we know the first time the orbiter requests the guide data
	// it hasn't yet been set to the initial position.  After we set
	// the initial position we won't reset it again so the user can
	// stay where he last left off
	map<int,bool> m_mapOrbiter_HasInitialPosition;
	VDRStateInfo(int dwPK_Device,int dwPK_Users,VDRMediaStream *pVDRMediaStream) { m_dwPK_Device=dwPK_Device; EventID=0; m_bViewing=m_bRecording=false; m_dwPK_Users=dwPK_Users; m_pVDRMediaStream=pVDRMediaStream; }
};

#endif

