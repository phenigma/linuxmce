#ifndef VDRStateInfo_h
#define VDRStateInfo_h

class VDRStateInfo
{
public:
	int m_dwPK_Device;
	int EventID; // The current event
	bool m_bViewing,m_bRecording;

	// When a new VDR player is started, these are all set to false
	// so we know the first time the orbiter requests the guide data
	// it hasn't yet been set to the initial position.  After we set
	// the initial position we won't reset it again so the user can
	// stay where he last left off
	map<int,bool> m_mapOrbiter_HasInitialPosition;
	VDRStateInfo(int dwPK_Device) { m_dwPK_Device=dwPK_Device; EventID=0; m_bViewing=m_bRecording=false; }
};

#endif

