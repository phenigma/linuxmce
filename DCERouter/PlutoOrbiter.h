#ifndef PlutoOrbiter_H
#define PlutoOrbiter_H

#include "Floorplan.h"
#include "DCEMI_PS_Orbiter.h"

class OCVariable
{
public:
	int PK_Variable;
	string m_sCurrentValue;
	string m_sDefaultValue;
	OCVariable()
	{
		m_sDefaultValue="";
	}
};

class PlutoOrbiter
{
public:
	// Keep track of various states so we don't need to unnecessarily reset them each time
	bool m_bState_TV_Realtime;
	string m_sState_NowPlaying;

	string m_sCurrentScreen;
	class DeviceData_Router *m_pDeviceData_Router;
	class Router *m_pRouter;

	map<int,OCVariable *> m_mapVariables;
	vector<class DCERoom *> m_vectRooms;
	map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;

	virtual ~PlutoOrbiter() {};

	class EntZone *m_pEntZone;
	class OCUser *m_pOCUser;
	class DeviceData_Router *m_pCurrentDevice;
	class MobileOrbiter *m_pMobileOrbiter;
//	class MediaStream *m_pMediaStream_Remote1,*m_pMediaStream_Remote2,*m_pGuideStream; //,*m_pOpenPlaylist;
	class DCEMI_PS_Orbiter *m_pDCEMI_PS_Orbiter;
	// If the user touches the button on the guide, leaves and comes back (without tuning to something else)
	// We will return to this
	class EPGProvider_Station__Schedule *m_pProvider_Station__Schedule_LastEPGTouch;
	FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page)
	{
		map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page);
		return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second;
	}

	PlutoOrbiter(class DCEMI_PS_Orbiter *pDCEMI_PS_Orbiter)
	{
		m_pDCEMI_PS_Orbiter=pDCEMI_PS_Orbiter;
		m_pRouter = m_pDCEMI_PS_Orbiter->m_pRouter;
		m_sCurrentScreen="";
		m_pEntZone=NULL;
		m_pOCUser=NULL;
		m_pCurrentDevice=NULL;
		m_pProvider_Station__Schedule_LastEPGTouch=NULL;
		m_pMobileOrbiter = NULL;
	}

	// When a controller is registered, we will store the defaults
	void SetDefaultFlags()
	{
		m_bState_TV_Realtime=true;
	}

	string CreateDesignObj(int DesignObjID)
	{
		if( m_sCurrentScreen.length()<0 )
			return "";
		else
			return m_sCurrentScreen + "." + StringUtils::itos(DesignObjID);
	}

	string CreateScreen(int DesignObjID)
	{
		if( m_sCurrentScreen.empty() )
			return StringUtils::itos(DesignObjID) + ".0.0";
		else
		{
			string::size_type version = m_sCurrentScreen.find(".");
			string::size_type versionEnd = m_sCurrentScreen.find(".",version+1);
			if( version==string::npos || versionEnd==string::npos )
				return StringUtils::itos(DesignObjID) + ".0.0";  // TODO -- if a device is sent a nav goto with a screen without the version/page, then received Message
			// will change the current screen to that number (without version and page), and this will assume the version is 0 even though it may not be
			else
				return StringUtils::itos(DesignObjID) + m_sCurrentScreen.substr(version,versionEnd-version) + ".0";
		}
	}

	void Refresh(bool bReacquireGrids=false);
	void Regen();
	void Beep();
	void GotoScreen(string DesignObj);
	void GotoScreen(int DesignObj);
	void SetVariable(int PK_Variable,string Value);
	void SetNowPlaying(string s);
};

#endif

