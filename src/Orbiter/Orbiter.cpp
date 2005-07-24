/*
 Orbiter

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

/*
	Shortcut keys:
	F1 = help
	F2 = main menu, or 'now playing'
	F3 = power off
*/


//<-dceag-d-b->
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DesignObj_Orbiter.h"
#include "DataGrid.h"
#include "SerializeClass/ShapesColors.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "pluto_main/Define_Direction.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_VertAlignment.h"
#include "pluto_main/Define_DesignObj.h"
#include "Floorplan.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_Text.h"
#include "PlutoUtils/minilzo.h"
#include "../Orbiter/RendererMNG.h"
#include "OrbiterFileBrowser.h"

#include "GraphicBuilder.h"
#include "Simulator.h"

#include "PlutoUtils/Profiler.h"
PlutoProfiler *g_PlutoProfiler=new PlutoProfiler();

#define  VERSION "<=version=>"
extern const char *g_szCompile_Date;
extern Command_Impl *g_pCommand_Impl;

#define PROFILING_GRID


#ifdef WINCE
    #include "wince.h"

    //very nasty hack :) (this will ignore cout's. could be improved to actually log all these in a file)
    class cout_dummy
    {
    public:
        static cout_dummy cout_dummy_;
        cout_dummy& operator<<(const string& s) { return cout_dummy_;}
        cout_dummy& operator<<(const char& c) { return cout_dummy_;}
        cout_dummy& operator<<(const int& l) { return cout_dummy_;}
        cout_dummy& operator<<(const char* s) { return cout_dummy_;}
    };
    cout_dummy cout_dummy::cout_dummy_;
    #define cout cout_dummy::cout_dummy_

#endif

// For DontRender
namespace DCE
{
    int g_iDontRender=0;
    clock_t g_cLastTime=clock(); // debug only
}

//------------------------------------------------------------------------
// Stuff the callback routines need
int iCallbackCounter=0;
class CallBackInfo
{
public:
	CallBackInfo(bool bPurgeTaskWhenScreenIsChanged = true)
	{
		m_iCounter=iCallbackCounter++;
		m_bStop=false;
		m_bPurgeTaskWhenScreenIsChanged = bPurgeTaskWhenScreenIsChanged;
	}

	Orbiter *m_pOrbiter;
    OrbiterCallBack m_fnCallBack;
	timespec m_abstime;
    void *m_pData;
	int m_iCounter; // A unique ID
	bool m_bStop; // Don't execute after all, we've decided to stop it (probably started another one of same type)
	bool m_bPurgeTaskWhenScreenIsChanged;
};
map<int,CallBackInfo *> mapPendingCallbacks;

//------------------------------------------------------------------------
// Stuff for the delayed select object
class DelayedSelectObjectInfo
{
public:
	DesignObj_Orbiter *m_pObj;
	string m_sPK_DesignObj_CurrentScreen;
	timespec m_tsTime;

	DelayedSelectObjectInfo(DesignObj_Orbiter *pObj,string sPK_DesignObj_CurrentScreen,timespec tsTime)
	{
		m_pObj=pObj;
		m_sPK_DesignObj_CurrentScreen=sPK_DesignObj_CurrentScreen;
		m_tsTime=tsTime;
	}
};

//------------------------------------------------------------------------
// Stuff for the continuous refresh
class ContinuousRefreshInfo
{
public:
	DesignObj_Orbiter *m_pObj;
	int m_iInterval;

	ContinuousRefreshInfo(DesignObj_Orbiter *pObj,int Interval)
	{
		m_pObj=pObj;
		m_iInterval=Interval;
	}
};

//------------------------------------------------------------------------
template<class T> inline static T Dist( T x,  T y ) { return x * x + y * y; }
//------------------------------------------------------------------------
void *MaintThread(void *p);
static bool bMaintThreadIsRunning = false;
//------------------------------------------------------------------------


/*
	EXTRAINFO codes for datagrids:
	'H' means allow the user to highlight cells without selecting them.
	'R' or 'C' means only highlight/select whole rows/columns
	Either 'A' means arrows scroll the grid.  If 'H' is not specified, the scrolling will be selecting cells.  Otherwise it will be highlighting them
	'As' means arrows scroll the grid 1 cell at a time
	'Ap' means arrows scroll the grid 1 page at a time
	'P' means make the first/last cell a 'page up'/'page down'
	'h' means that moving the highlight should de-select any selected cells so you don't have both a highlighted and selected cell.  Only meaningfull with 'H'.
	'X' means that clear the selected cell entry whenever the user scrolls
	'S' means that on the initial acquire it should scroll to the highlighted cell
	'cx' means if this is a phone on Bluetooth dongle, render this grid as a single column on the phone using the column x
*/

/*

Constructors/Destructor

*/

//<-dceag-const-b->!
Orbiter::Orbiter( int DeviceID,  string ServerAddress,  string sLocalDirectory,  bool bLocalMode,  int iImageWidth,  int iImageHeight )
: Orbiter_Command( DeviceID,  ServerAddress, true, bLocalMode ),
m_ScreenMutex( "rendering" ), m_VariableMutex( "variable" ), m_DatagridMutex( "datagrid" ),
m_MaintThreadMutex("MaintThread"), m_NeedRedrawVarMutex( "need redraw variables" )
//<-dceag-const-e->

{
g_pPlutoLogger->Write(LV_STATUS,"Orbiter %p constructor",this);

	m_bStartingUp=true;
	m_pLocationInfo = NULL;
	m_dwPK_Users = 0;
	m_dwPK_Device_NowPlaying = 0;
	m_pOrbiterFileBrowser_Collection=NULL;

	m_sLocalDirectory=sLocalDirectory;
    m_iImageWidth=iImageWidth;
    m_iImageHeight=iImageHeight;
	if( DATA_Get_Leave_Monitor_on_for_OSD() )
	    m_bDisplayOn=false;  // So the first touch will turn it on
	else
	    m_bDisplayOn=true;  // The display should already be on

	m_bForward_local_kb_to_OSD=false;
	m_bYieldScreen=false;
    m_bYieldInput=false;
    m_bRerenderScreen=false;
	m_bWeCanRepeat=false;
	m_bRepeatingObject=false;
    m_bShowShortcuts = false;
	m_iPK_DesignObj_Remote=m_iPK_DesignObj_Remote_Popup=m_iPK_DesignObj_FileList=m_iPK_DesignObj_FileList_Popup=m_iPK_DesignObj_RemoteOSD=0;

    m_pScreenHistory_Current=NULL;
    m_pObj_LastSelected=m_pObj_Highlighted=NULL;
    m_LastActivityTime=time( NULL );
    m_iLastEntryInDeviceGroup=-1;

    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
    m_VariableMutex.Init( &m_MutexAttr );
    m_ScreenMutex.Init( &m_MutexAttr );
    m_DatagridMutex.Init( &m_MutexAttr );
	m_NeedRedrawVarMutex.Init( &m_MutexAttr );
	m_pActivePopup=NULL;

    m_dwIDataGridRequestCounter=0;

    if(  !m_bLocalMode  )
    {
        GetEvents(  )->m_pClientSocket->SetReceiveTimeout( 15 );
        m_pcRequestSocket->m_pClientSocket->SetReceiveTimeout( 15 );
    }

    m_bCaptureKeyboard_OnOff = false;
    m_bCaptureKeyboard_Reset = false;
    m_iCaptureKeyboard_EditType = 0;
    m_bCaptureKeyboard_DataGrid = false;
    m_iCaptureKeyboard_PK_Variable = 0;
    m_sCaptureKeyboard_Text = "";
    m_sCaptureKeyboard_InternalBuffer = "";
    m_pCaptureKeyboard_Text = NULL;
	m_bBypassScreenSaver = false;
	DeviceData_Base *pDevice_Parent = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_pData->m_dwPK_Device_ControlledVia);
	m_bIsOSD = pDevice_Parent && pDevice_Parent->WithinCategory(DEVICECATEGORY_Media_Director_CONST);

	string::size_type pos=0;
	string sTimeout = DATA_Get_Timeout();
	m_iTimeoutScreenSaver = atoi(StringUtils::Tokenize(sTimeout,",",pos).c_str());
	m_iTimeoutBlank = atoi(StringUtils::Tokenize(sTimeout,",",pos).c_str());
	m_sCacheFolder = DATA_Get_CacheFolder();
	m_iCacheSize = DATA_Get_CacheSize();

    if(DATA_Get_ScreenWidth())
        m_iImageWidth = DATA_Get_ScreenWidth();

    if(DATA_Get_ScreenHeight())
        m_iImageHeight = DATA_Get_ScreenHeight();

	m_sScreenSize=PlutoSize(m_iImageWidth,m_iImageHeight);

	m_pCacheImageManager = NULL;
	if(m_iCacheSize > 0)
		m_pCacheImageManager = new CacheImageManager(m_sCacheFolder, m_iCacheSize);

    m_iVideoFrameInterval = DATA_Get_VideoFrameInterval();
    if(!m_iVideoFrameInterval) //this device data doesn't exist or it's 0
        m_iVideoFrameInterval = 6000; //6 sec

	pthread_cond_init(&m_MaintThreadCond, NULL);
	m_MaintThreadMutex.Init(NULL,&m_MaintThreadCond);
	pthread_create(&m_MaintThreadID, NULL, MaintThread, (void*)this);

    m_bShiftDown = false;
    m_bControlDown = false;
    m_bAltDown = false;
    m_bCapsLock = false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter::~Orbiter()
//<-dceag-dest-e->
{
g_pPlutoLogger->Write(LV_STATUS,"Orbiter  %p is exiting",this);

    //stop the simulator, if running
    if(Simulator::GetInstance()->IsRunning())
        StopSimulatorThread();

	// Be sure we get the maint thread to cleanly exit
	KillMaintThread();

	PLUTO_SAFETY_LOCK( pm, m_MaintThreadMutex );
	map<int,CallBackInfo *>::iterator itCallBackInfo;
	for(itCallBackInfo = mapPendingCallbacks.begin(); itCallBackInfo != mapPendingCallbacks.end(); itCallBackInfo++)
		delete (*itCallBackInfo).second;
	mapPendingCallbacks.clear();
	pm.Release();

g_pPlutoLogger->Write(LV_STATUS,"Maint thread dead");

	char *pData=NULL;int iSize=0;
	DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "0", 0, "", 0, &pData, &iSize );
    SendCommand( CMD_Orbiter_Registered );

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
	DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
	for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
	{
		DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
		string dummy = (*itDesignObjOrbiter).first;

		delete pObj;
		pObj = NULL;
	}
	m_mapObj_All.clear();

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	m_vectTexts_NeedRedraw.clear();
	m_vectObjs_NeedRedraw.clear();
	nd.Release();

	m_vectObjs_TabStops.clear();
	m_vectObjs_Selected.clear();
	m_ScreenMap.clear(); //shared with mapObj_All
	m_mapDeviceGroups.clear();
	m_mapUserIcons.clear();

	map < string, DesignObj_DataList * >::iterator itSuffix; //
	for(itSuffix = m_mapObj_AllNoSuffix.begin(); itSuffix != m_mapObj_AllNoSuffix.end(); itSuffix++)
	{
		delete (*itSuffix).second;
		(*itSuffix).second = NULL;
	}
	m_mapObj_AllNoSuffix.clear();

	//clearing device groups map
	map<int,class DeviceGroup *>::iterator itDeviceGroups;
	for(itDeviceGroups = m_mapDeviceGroups.begin(); itDeviceGroups != m_mapDeviceGroups.end(); itDeviceGroups++)
	{
		delete (*itDeviceGroups).second;
		(*itDeviceGroups).second = NULL;
	}

	//clearing floor plan object map
	map<int, FloorplanObjectVectorMap *>::iterator itFPObjVM;
	for(itFPObjVM = m_mapFloorplanObjectVector.begin(); itFPObjVM != m_mapFloorplanObjectVector.end(); itFPObjVM++)
	{
		FloorplanObjectVectorMap *fpObjVM = (*itFPObjVM).second;
		if( fpObjVM )
		{
			map<int,FloorplanObjectVector *>::iterator itFPObjVect;
			for(itFPObjVect=fpObjVM->begin();itFPObjVect!=fpObjVM->end();++itFPObjVect)
			{
				FloorplanObjectVector *fpObjVect = (*itFPObjVect).second;
				if( fpObjVect )
				{
					for(int i=0;i<(int) fpObjVect->size();++i)
					{
						FloorplanObject *fpObj = (*fpObjVect)[i];
						delete fpObj;
						fpObj = NULL;
					}
					delete fpObjVect;
					fpObjVect = NULL;
				}
			}
			delete fpObjVM;
			fpObjVM = NULL;
		}
	}
	m_mapFloorplanObjectVector.clear();

	//clearing screen history
	list < ScreenHistory * >::iterator itScreenHistory;
	for(itScreenHistory = m_listScreenHistory.begin(); itScreenHistory != m_listScreenHistory.end(); itScreenHistory++)
	{
        ScreenHistory * psh = *itScreenHistory;
		if( *itScreenHistory==m_pScreenHistory_Current )
			m_pScreenHistory_Current=NULL;
		delete *itScreenHistory;
		*itScreenHistory = NULL;
	}
	m_listScreenHistory.clear();

	if(NULL != m_pScreenHistory_Current)
	{
		delete m_pScreenHistory_Current;
		m_pScreenHistory_Current = NULL;
	}

	//clearing device data map
	map< string, class DesignObj_DataGrid * >::iterator itDesignObjDataGrid;
	for(itDesignObjDataGrid = m_mapObjs_AllGrids.begin(); itDesignObjDataGrid != m_mapObjs_AllGrids.end(); itDesignObjDataGrid++)
	{
		//delete (*itDesignObjDataGrid).second;
		(*itDesignObjDataGrid).second = NULL;
	}
	m_mapObjs_AllGrids.clear();

	map< string, DesignObj_DataList * >::iterator itObj_Bound;
	for(itObj_Bound = m_mapObj_Bound.begin(); itObj_Bound != m_mapObj_Bound.end(); itObj_Bound++)
	{
		delete (*itObj_Bound).second;
		(*itObj_Bound).second = NULL;
	}
	m_mapObj_Bound.clear();

	if(NULL != m_pCacheImageManager)
	{
		delete m_pCacheImageManager;
		m_pCacheImageManager = NULL;
	}

	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		delete *it;
    m_listPopups.clear();

	vm.Release();
	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_ScreenMutex.mutex);
	pthread_mutex_destroy(&m_VariableMutex.mutex);
	pthread_mutex_destroy(&m_DatagridMutex.mutex);
	pthread_mutex_destroy(&m_MaintThreadMutex.mutex);
	pthread_mutex_destroy(&m_NeedRedrawVarMutex.mutex);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter::Register()
//<-dceag-reg-e->
{
    return Connect(0);  // Don't validate the device template, since the same binary is used for lots of devices
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Orbiter::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Orbiter::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

/*

Protected methods

*/
//------------------------------------------------------------------------------------------------------------

/*
GENERAL PURPOSE METHODS
*/

//-----------------------------------------------------------------------------------------------------------
void Orbiter::RenderScreen( )
{
g_PlutoProfiler->Start("renderscreen");

	if( !m_pScreenHistory_Current || !m_pScreenHistory_Current->m_pObj )
{
    g_pPlutoLogger->Write( LV_CRITICAL, "Got attempt to render null screen: %s", m_pScreenHistory_Current );
	return;
}
#ifdef DEBUG
    g_pPlutoLogger->Write( LV_CRITICAL, "Render screen: %s", m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ) );
#endif

#if ( defined( PROFILING ) )
    clock_t clkStart = clock();
#endif
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

    if ( m_pScreenHistory_Current  )
    {
        RenderObject( m_pScreenHistory_Current->m_pObj,  m_pScreenHistory_Current->m_pObj);
    }

	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		RenderPopup(*it, (*it)->m_Position);

	if( m_pScreenHistory_Current  )
	{
		for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->m_pObj->m_listPopups.begin();it!=m_pScreenHistory_Current->m_pObj->m_listPopups.end();++it)
			RenderPopup(*it, (*it)->m_Position);
	}

	cm.Release(  );
#if ( defined( PROFILING ) )
    clock_t clkFinished = clock();
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "Render screen: %s took %d ms",
            m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ), clkFinished-clkStart );
    }
#endif
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s finished", m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ) );
#endif
	g_PlutoProfiler->Stop("renderscreen");
	g_PlutoProfiler->DumpResults();
	g_PlutoProfiler->ClearResults();
}

//-----------------------------------------------------------------------------------------------------------
void Orbiter::RedrawObjects(  )
{
	PLUTO_SAFETY_LOCK(rm,m_NeedRedrawVarMutex);
    if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0 && m_bRerenderScreen==false)
		return; // Nothing to do anyway
    // TODO --- hack
    // There appears to be a problem with SDL_Flip sometimes calling _XRead to wait for an event,  causing the thread
    // to block until there's an event,  so that our loop no longer gets back to the SDL_WaitEvent in the main pump
    // So for now just do the redraw's always on a separate thread
	CallMaintenanceInMiliseconds( 0, &Orbiter::RealRedraw, NULL, pe_ALL );
}

void Orbiter::ScreenSaver( void *data )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"::Screen saver Bypass screen saver now: %d",(int)m_bBypassScreenSaver);
#endif
	if( m_bBypassScreenSaver || !m_pDesignObj_Orbiter_ScreenSaveMenu )
		return;

	NeedToRender render( this, "Screen saver" );

	if( m_pScreenHistory_Current->m_pObj == m_pDesignObj_Orbiter_ScreenSaveMenu )
	{
		CMD_Display_OnOff("0",false);
	}
	else
	{
		CMD_Set_Main_Menu("V");
		GotoScreen(m_sMainMenu);
	}
}

void Orbiter::Timeout( void *data )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Screen %s timed out data: %p",m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(),data);
#endif
	if( !data || data!=(void *) m_pScreenHistory_Current->m_pObj )
		return;

	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;
    if(  pObj->m_Action_TimeoutList.size(  )>0  )
        ExecuteCommandsInList( &pObj->m_Action_TimeoutList, pObj );
}

void Orbiter::ReselectObject( void *data )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;

	// Not used
	DesignObjZoneList::iterator iZone;
	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
    {
        DesignObjZone *pDesignObjZone = ( *iZone );
		ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, -1, -1, pObj->m_iRepeatParm+1 );
    }
}

void Orbiter::RedrawObject( void *iData )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) iData;
	if( !pObj || !pObj->m_bOnScreen )
		return;

    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	NeedToRender render( this, "RedrawObject" );

	if( pObj->Data.m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) pObj;
		pGrid->bReAcquire=true;
	}

    PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
    m_vectObjs_NeedRedraw.push_back(pObj);
    nd.Release();


	CallMaintenanceInMiliseconds(pObj->Data.m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);
}

void Orbiter::RealRedraw( void *data )
{
	if( m_bQuit )
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "In Redraw Objects: rerender %d",(int) m_bRerenderScreen );
#endif
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	// We don't have a good solution for rendering objects under popups--so re-render the whole screen if there are popups and we're going to render something else anyway
    if(  m_bRerenderScreen || 
		((m_listPopups.size() || (m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj->m_listPopups.size())) &&
		(m_vectObjs_NeedRedraw.size() || m_vectTexts_NeedRedraw.size()) ))
    {
		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        m_vectObjs_NeedRedraw.clear();
		m_vectTexts_NeedRedraw.clear();
		nd.Release();

        RenderScreen(  );
		m_bRerenderScreen = false;
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "Exiting Redraw Objects" );
#endif
        return;
    }

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
#ifdef DEBUG
	g_pPlutoLogger->Write( LV_CONTROLLER, "I won't render the whole screen. Objects to be rendered: %d, texts to be rendered: %d",	m_vectObjs_NeedRedraw.size(), m_vectTexts_NeedRedraw.size()	);
#endif

    if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0)
        return;

	BeginPaint();
	size_t s;

    PlutoPoint AbsolutePosition = NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0);

	//render objects
	for( s = 0; s < m_vectObjs_NeedRedraw.size(); ++s )
    {
		class DesignObj_Orbiter *pObj = m_vectObjs_NeedRedraw[s];

		if(pObj)
		{
            RenderObject( pObj, m_pScreenHistory_Current->m_pObj, AbsolutePosition );
		    UpdateRect(pObj->Data.m_rPosition, AbsolutePosition);
		}
    }

	//render texts
	for( s = 0; s < m_vectTexts_NeedRedraw.size(); ++s )
	{
		DesignObjText *pText = m_vectTexts_NeedRedraw[s];
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		if( pTextStyle )
		{
            if(NULL != m_pActivePopup)
                SolidRectangle( m_pActivePopup->m_Position.X + pText->Data.m_rPosition.Left(),  m_pActivePopup->m_Position.Y + pText->Data.m_rPosition.Top(), pText->Data.m_rPosition.Width,  pText->Data.m_rPosition.Height,  pTextStyle->m_BackColor);
            else
                SolidRectangle( pText->Data.m_rPosition.Left(),  pText->Data.m_rPosition.Top(), pText->Data.m_rPosition.Width,  pText->Data.m_rPosition.Height,  pTextStyle->m_BackColor);

		    string TextToDisplay = SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0).c_str();
			RenderText(TextToDisplay,pText, pTextStyle, AbsolutePosition);    
			UpdateRect(pText->Data.m_rPosition, AbsolutePosition);
		}
		else
		{
			m_vectObjs_NeedRedraw.push_back( pText->m_pObject);
		}
	}

    //See if maybe we just added an object that's a tab stop that wasn't there before
    //if(!m_pObj_Highlighted && m_vectObjs_TabStops.size(  )  )
	//    HighlightFirstObject();

    if(NULL != m_pObj_Highlighted)
    	HighlightObject( m_pObj_Highlighted );

    m_vectObjs_NeedRedraw.clear();
	m_vectTexts_NeedRedraw.clear();
	EndPaint();
}
//-----------------------------------------------------------------------------------------------------------
void Orbiter::RenderObject( DesignObj_Orbiter *pObj,  DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
if( pObj->m_ObjectID.find("2355")!=string::npos) //&& this->m_pScreenHistory_Current && this->m_pScreenHistory_Current->m_pObj->m_ObjectID.find("1255")!=string::npos )
{
int k=2;
}
    if(  pObj->m_pDesignObj_Orbiter_TiedTo  )
    {
        pObj->m_bHidden = pObj->m_pDesignObj_Orbiter_TiedTo->IsHidden(  );
        if(  ( pObj->m_iRowTiedTo==-1 && pObj->m_iColumnTiedTo==-1 ) || pObj->m_pDesignObj_Orbiter_TiedTo->Data.m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST  )
        {
            if(  pObj->m_pDesignObj_Orbiter_TiedTo==m_pObj_Highlighted  )
                pObj->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
            else
                pObj->m_GraphicToDisplay = pObj->m_pDesignObj_Orbiter_TiedTo->m_GraphicToDisplay;
        }
        else
        {
            DesignObj_DataGrid *pObjGrid = ( DesignObj_DataGrid * ) pObj->m_pDesignObj_Orbiter_TiedTo;
            // We must be pointing to a datagrid,  and we need to bind to the highlighted row
            if(  ( pObj->m_iRowTiedTo==-1 || pObj->m_iRowTiedTo==pObjGrid->m_iHighlightedRow ) && ( pObj->m_iColumnTiedTo==-1 || pObj->m_iColumnTiedTo==pObjGrid->m_iHighlightedColumn )  )
                pObj->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
            else
                pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
        }
    }

    // See if this object is only visible for a given state
    if(  pObj->m_sVisibleState.size(  )  )
    {
        if(  (  ( pObj==m_pObj_Highlighted || pObj->m_GraphicToDisplay==GRAPHIC_HIGHLIGHTED ) && pObj->m_sVisibleState.find( "H" )==string::npos ) ||
            (  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED && pObj->m_sVisibleState.find( "S" )==string::npos  ) ||
            (  pObj->m_GraphicToDisplay==GRAPHIC_NORMAL && pObj->m_sVisibleState.find( "N" )==string::npos  ) ||
            (  pObj->m_GraphicToDisplay>0 && pObj->m_sVisibleState.find( StringUtils::itos( pObj->m_GraphicToDisplay ) )==string::npos  )  )
            return;
    }

    if ( pObj->m_bHidden || pObj->Data.m_rPosition.Width==0 || pObj->Data.m_rPosition.Height==0 )
    {
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "object: %s  not visible: %d", pObj->m_ObjectID.c_str(), (int) pObj->m_bHidden );
#endif
        return;
	}

    PROFILE_START( ctObj )

    PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
    PlutoRectangle rectBackground = pObj->Data.m_rBackgroundPosition;
    PlutoRectangle rectTotal = pObj->Data.m_rPosition;
    vm.Release(  );

	if( (pObj == m_pObj_Highlighted || pObj->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ) && pObj->m_vectHighlightedGraphic.size() )
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectHighlightedGraphic);
    else if(pObj->m_GraphicToDisplay == GRAPHIC_SELECTED && pObj->m_vectSelectedGraphic.size())
        pObj->m_pvectCurrentGraphic = &(pObj->m_vectSelectedGraphic);
    else if(  pObj->m_GraphicToDisplay >= 1 && pObj->m_GraphicToDisplay <= int(pObj->m_vectAltGraphics.size()))
        pObj->m_pvectCurrentGraphic = &(pObj->m_vectAltGraphics[pObj->m_GraphicToDisplay - 1]);
    else // Something that was invalid was chosen,  or just the normal version
        pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

    if(  !pObj->m_pvectCurrentGraphic && pObj->m_GraphicToDisplay != GRAPHIC_NORMAL  )
        pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

g_PlutoProfiler->Start("rendergraphic");
    // This is somewhat of a hack, but we don't have a clean method for setting the graphics on the user & location buttons to
    if( pObj->m_bIsBoundToUser )
    {
        vector<PlutoGraphic*> *pvectGraphic = m_mapUserIcons[m_dwPK_Users];
		//vector<PlutoGraphic*> *p = pvectGraphic;

        if( pvectGraphic )
			pObj->m_pvectCurrentGraphic = pvectGraphic;
        if( pObj->m_pvectCurrentGraphic )
            RenderGraphic(pObj, rectTotal, pObj->m_bDisableAspectLock, point);
    }
    else if( pObj->m_bIsBoundToLocation )
    {
        if( m_pLocationInfo && m_pLocationInfo->m_pvectGraphic )
            pObj->m_pvectCurrentGraphic = m_pLocationInfo->m_pvectGraphic;
        if( pObj->m_pvectCurrentGraphic )
            RenderGraphic(pObj, rectTotal, pObj->m_bDisableAspectLock, point);
    }
    // Maybe we're showing a non standard state
    else if(  pObj->m_pGraphicToUndoSelect && pObj->m_GraphicToDisplay==GRAPHIC_NORMAL  )
    {
        vector<PlutoGraphic*> *pvectGraphic_Hold = pObj->m_pvectCurrentGraphic;

		vector<PlutoGraphic*> vectGraphicToUndoSelect;
		vectGraphicToUndoSelect.push_back(pObj->m_pGraphicToUndoSelect);
		pObj->m_pvectCurrentGraphic = &vectGraphicToUndoSelect;

if( pObj->m_ObjectID.find("2355")!=string::npos) //&& this->m_pScreenHistory_Current && this->m_pScreenHistory_Current->m_pObj->m_ObjectID.find("1255")!=string::npos )
{
int k=2;
}
		RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );

        pObj->m_pvectCurrentGraphic = pvectGraphic_Hold;

		vectGraphicToUndoSelect.clear();
        delete pObj->m_pGraphicToUndoSelect;
        pObj->m_pGraphicToUndoSelect=NULL;
    }
    else if(  pObj->m_pvectCurrentGraphic  )
    {
if( pObj->m_ObjectID.find("2355")!=string::npos) //&& this->m_pScreenHistory_Current && this->m_pScreenHistory_Current->m_pObj->m_ObjectID.find("1255")!=string::npos )
{
int k=2;
}
        RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );
    }
g_PlutoProfiler->Stop("rendergraphic");

    if(pObj == m_pObj_Highlighted)
        HighlightObject(pObj, point);

    // Matt is going to pass through the text before/after children
    /*  moving this block to the end to see if it puts text on top of icons
    VectorDesignObjText::iterator iText;
    for( iText=pObj->m_vectDesignObjText.begin(  ); iText != pObj->m_vectDesignObjText.end(  ); ++iText )
    {
    PROFILE_START( ctText )
    RenderText( *iText );
    PROFILE_STOP( ctText,  "Text ( obj below )" )
    }
    */
    switch( pObj->Data.m_ObjectType )
    {
    case DESIGNOBJTYPE_Rectangle_CONST:
        // todo 2.0     SolidRectangle( rectTotal.X,  rectTotal.Y,  rectTotal.Width,  rectTotal.Height,  atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Cell_Color_CONST ).c_str(  ) ),  atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Transparency_CONST ).c_str(  ) ) );
        break;
    case DESIGNOBJTYPE_Datagrid_CONST:
g_PlutoProfiler->Start("renderdg");
        RenderDataGrid( ( DesignObj_DataGrid * )pObj, point );
g_PlutoProfiler->Stop("renderdg");
        break;
    case DESIGNOBJTYPE_Floorplan_CONST:
        RenderFloorplan(pObj,pObj_Screen, point);
        break; // Render the child objects with their text
    case DESIGNOBJTYPE_Web_Browser_CONST:
        //      LocalHandleWebWindow( pObj,  rectTotal );
        break;
    case DESIGNOBJTYPE_App_Desktop_CONST:
        if ( m_bYieldScreen )
            RenderDesktop( pObj, PlutoRectangle( 0, 0, -1, -1 ), point );  // Full screen
        else
            RenderDesktop( pObj, pObj->Data.m_rPosition, point );
        break;
#ifdef PRONTO
    case DESIGNOBJTYPE_Pronto_File_CONST:
        if ( pObj->Data.m_pCCF == NULL )
        {
            if ( !m_bInitialPaint )
            {
                string ccf = pObj->GetParameterValue( C_PARAMETER_PRONTO_FILE_CONST );
                if( ccf.length(  ) == 0 && m_bDeleteThis || ccf.find( '~' )==string::npos ) ccf = "3365~I49/ccf/1.ccf";
                string::size_type pos = 0;
                int IRDevice = atoi( StringUtils::Tokenize( ccf,  "~",  pos ).c_str(  ) );
                string fn = ccf.substr( pos );

                Message *pMessage=m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device,  DEVICEID_DCEROUTER,  PRIORITY_NORMAL,  MESSAGETYPE_FILEREQUEST,  0,  1,  1,  fn.c_str(  ) ) );
                string sResult="";
                if ( pMessage )
                {
                    if ( pMessage->m_dwID==FILERESULT_SUCCESS )
                    {

                        pObj->Data.m_pCCF = new ProntoCCF( this,  pMessage->m_mapData_Parameters[1],  pMessage->m_mapData_Lengths[1],  pObj,  IRDevice );
                        // Clear the data parameters so the Message's destructor doesn't delete the data.
                        pMessage->m_mapData_Parameters.clear(  );
                    }
                    delete pMessage;
                }
            }
            else
            {
                m_AutoInvalidateTime = clock();
            }
        }
        if ( pObj->Data.m_pCCF )
        {
            pObj->Data.m_pCCF->RenderPronto( XOffset,  YOffset, point );
        }
        break;
#endif
    case DESIGNOBJTYPE_Broadcast_Video_CONST:
		if ( pObj->m_bOnScreen && !m_bAlreadyQueuedVideo )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Scheduling object @%p: %s", pObj, pObj->m_ObjectID.c_str());
#endif

			// If we've got no current graphic, such as during the first redraw since ObjectOnScreen
			// deletes it, then request a video frame right away, otherwise we're redrawing
			// and should wait the correct interval

			// Don't purge existing callbacks since there can be multiple frames on the screen
			CallMaintenanceInMiliseconds( pObj->m_vectGraphic.size()==0 ? 0 : m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
			m_bAlreadyQueuedVideo=true;  // Only schedule once -- that will redraw all video frames
		}
		else
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Not Scheduling object @%p: %s because is not on screen.", pObj, pObj->m_ObjectID.c_str());
#endif
		}

        break;

        // Grabbing up to four video frames can take some time.  Draw the rest of the
        // objects first.
        /* todo 2.0
        if ( !m_bInitialPaint )
        {
        if(  pObj->Data.m_iBaseObjectID==DESIGNOBJ_OBJPREVIEWWINDOW1_CONST || pObj->Data.m_iBaseObjectID==DESIGNOBJ_OBJPREVIEWWINDOW2_CONST  )
        m_AutoInvalidateTime = clock(  ) + ( CLOCKS_PER_SEC*4 );
        else
        m_AutoInvalidateTime = clock(  ) + ( ( 3 * CLOCKS_PER_SEC )/4 );
        */
        // TODO!! hardcoded to motion device.  Should get that info from vidmgr

        //          ::OutputDebugString( ( pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST )+"\n" ).c_str(  ) );
        /*  todo
        int PK_Device_VideoSource = atoi( pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST ).c_str(  ) );
        if(  PK_Device_VideoSource  )
        {
        Message *pMessage = m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device,  PK_Device_VideoSource,  PRIORITY_LOW,  MESSAGETYPE_REQUEST,  REQUESTTYPE_VIDEO_FRAME,  3,
        C_DEVICEDATA_DEVICELIST_CONST,  pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST ).c_str(  ),
        COMMANDPARAMETER_Position_X_CONST,  StringUtils::itos( pObj->Data.m_rPosition.Width ).c_str(  ),
        COMMANDPARAMETER_Position_Y_CONST,  StringUtils::itos( pObj->Data.m_rPosition.Height ).c_str(  ) ) );
        string sResult;
        if ( pMessage ) //LACA_B4_0( "About to decompress cam video" );
        LocalDecompressCamVideo( pObj,  pMessage,  rectTotal );
        }
        */
        /* todo 2.0
        }
        else
        {
        m_AutoInvalidateTime = clock(  );
        }
        */
        break;
    }

    PROFILE_STOP( ctObj,  pObj->m_ObjectID.c_str(  ) )

        DesignObj_DataList::reverse_iterator iHao;

#pragma warning( "todo - nasty hack -- on the hard drive screen,  you select music,  then when you go to video,  it redraws the video section,  but then redraws the deselected music with m_pGraphicToUndoSelect,  which includes a snapshot of the prior highlighters,  that are overwriting the new ones.  Redraw these first!  After render object won't draw anything" )
    for( iHao=pObj->m_ChildObjects.rbegin(  ); iHao != pObj->m_ChildObjects.rend(  ); ++iHao )
    {
        DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
        if(  !pObj->m_pGraphicToUndoSelect || pObj->m_GraphicToDisplay!=GRAPHIC_NORMAL  )
            continue;
        RenderObject( pDesignObj_Orbiter,  pObj_Screen, point );
    }


#pragma warning( "todo - nasty hack -- the datagrid highlighters need to be drawn after the music button.  Until I can implement layering,  scan twice and do the tied to's last" )
    for( iHao=pObj->m_ChildObjects.rbegin(  ); iHao != pObj->m_ChildObjects.rend(  ); ++iHao )
    {
        // Hold back the tied to's and datagrids
        DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
        if(  pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
            continue;
        RenderObject( pDesignObj_Orbiter,  pObj_Screen, point );
    }
    for( iHao=pObj->m_ChildObjects.rbegin(  ); iHao != pObj->m_ChildObjects.rend(  ); ++iHao )
    {
        // Let the to's go through,  as long as there's no datagrid
        DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
        if(  !pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
            continue;
        RenderObject( pDesignObj_Orbiter,  pObj_Screen, point );
    }
    for( iHao=pObj->m_ChildObjects.rbegin(  ); iHao != pObj->m_ChildObjects.rend(  ); ++iHao )
    {
        DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
        if(  !pDesignObj_Orbiter->m_bContainsDataGrid  )
            continue;
        RenderObject( pDesignObj_Orbiter,  pObj_Screen, point );
    }

g_PlutoProfiler->Start("rendertext");
    VectorDesignObjText::iterator iText;
    for( iText=pObj->m_vectDesignObjText.begin(  ); iText != pObj->m_vectDesignObjText.end(  ); ++iText )
    {
        DesignObjText *pText = *iText;
        if(  pText->Data.m_bPreRender  )
            continue;
        PROFILE_START( ctText );
            TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
	    string TextToDisplay = SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0).c_str();
        RenderText( TextToDisplay, pText, pTextStyle, point );
        PROFILE_STOP( ctText,  "Text ( obj below )" );
    }
g_PlutoProfiler->Stop("rendertext");
    if( pObj->m_pFloorplanObject && m_mapDevice_Selected.find(pObj->m_pFloorplanObject->PK_Device)!=m_mapDevice_Selected.end() )
    {
		int i;
        for(i = 0; i < 4; ++i)
            HollowRectangle(point.X + pObj->Data.m_rBackgroundPosition.X-i, point.Y + pObj->Data.m_rBackgroundPosition.Y-i, pObj->Data.m_rBackgroundPosition.Width+i+i, pObj->Data.m_rBackgroundPosition.Height+i+i,
			    (i==1 || i==2 ? PlutoColor::Black() : PlutoColor::White()));

        //force an update because the object boundaries are not respected
        PlutoRectangle rect(point.X + pObj->Data.m_rBackgroundPosition.X-i, point.Y + pObj->Data.m_rBackgroundPosition.Y-i, pObj->Data.m_rBackgroundPosition.Width+i+i, pObj->Data.m_rBackgroundPosition.Height+i+i);
        UpdateRect(rect, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));
    }

    if(m_bShowShortcuts)
    {
        string sCharToRender;
        if(pObj->Data.m_iPK_Button >= BUTTON_1_CONST && pObj->Data.m_iPK_Button <= BUTTON_9_CONST)
            sCharToRender += '0' + pObj->Data.m_iPK_Button - BUTTON_1_CONST + 1;
        else if(pObj->Data.m_iPK_Button == BUTTON_0_CONST)
            sCharToRender += '0';
                
        if(sCharToRender != "")
        {
            PlutoPoint AbsPos = NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0);
            PlutoPoint textPos(AbsPos.X + pObj->Data.m_rPosition.X + 5, AbsPos.Y + pObj->Data.m_rPosition.Y + 5);

            PlutoRectangle rect(textPos.X, textPos.Y, 20, 20);
            DesignObjText text(m_pScreenHistory_Current->m_pObj);
            text.Data.m_rPosition = rect;
            TextStyle *pTextStyle = m_mapTextStyle_Find(1);
            PlutoColor OldColor = pTextStyle->m_ForeColor;
            pTextStyle->m_ForeColor.m_Value = 0x808080;
            RenderText(sCharToRender,&text, pTextStyle);
            pTextStyle->m_ForeColor = OldColor;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------
bool Orbiter::RenderCell( class DesignObj_DataGrid *pObj,  class DataGridTable *pT,  class DataGridCell *pCell,  int j,  int i,  int GraphicToDisplay, PlutoPoint point )
{
g_PlutoProfiler->Start("render cell");
    TextStyle *pTextStyle = pObj->m_pTextStyle;
    bool bTransparentCell = false; // todo,  is transparency in PlutoColor? ( strchr( pObj->GetParameterValue( C_PARAMETER_CELL_COLOR_CONST ).c_str(  ),  ', ' )==NULL );

    if ( j==0 && pT->m_bKeepColumnHeader )
    {
        pTextStyle = pObj->m_pTextStyle_FirstCol;
        bTransparentCell = false;
    }
    if ( i==0 && pT->m_bKeepRowHeader )
    {
        pTextStyle = pObj->m_pTextStyle_FirstRow;
        bTransparentCell = false;
    }
    if ( pCell->m_AltColor > 0 && pCell->m_AltColor<( int ) pObj->m_vectTextStyle_Alt.size(  ) )
    {
        pTextStyle = pObj->m_vectTextStyle_Alt[pCell->m_AltColor];
        bTransparentCell = false;
    }
    if( GraphicToDisplay==GRAPHIC_SELECTED )
    {
        pTextStyle = pObj->m_pTextStyle_Selected;
        bTransparentCell = false;
    }
    else if ( GraphicToDisplay==GRAPHIC_HIGHLIGHTED )
    {
        pTextStyle = pObj->m_pTextStyle_Highlighted;
        bTransparentCell = false;
    }

    if(  !pTextStyle  )
        pTextStyle = m_mapTextStyle_Find( 0 );
    if(  !pTextStyle  )
        pTextStyle = m_mapTextStyle_Find( 1 );

    if(  !pTextStyle  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Trying to render cell,  but cannot find any style" );
        return false;
    }
    int x, y, w, h;
    GetGridCellDimensions( pObj,  pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );
    if ( w>4 && h >4 )
    {
		if ( !bTransparentCell )
			SolidRectangle( point.X + x,  point.Y + y,  w,  h,  pCell->m_AltColor ? pCell->m_AltColor : pTextStyle->m_BackColor);

		/*
		if ( pObj->BorderColor2.m_Value!=pObj->BorderColor.m_Value )
        {
			SolidRectangle( x+w-pObj->BorderWidth,  y,  pObj->BorderWidth,  h,  pObj->BorderColor2 );
			SolidRectangle( x,  y+h-pObj->BorderWidth,  w,  pObj->BorderWidth,  pObj->BorderColor2 );
			SolidRectangle( x,  y,  w,  pObj->BorderWidth,  pObj->BorderColor );
			SolidRectangle( x,  y,  pObj->BorderWidth,  h,  pObj->BorderColor );
        }
        else
		*/

		//SolidRectangle( x,  y,  w,  h,  pObj->BorderColor  );
		if ( pCell->m_pGraphicData )
		{
			PlutoGraphic *pPlutoGraphic = CreateGraphic();
			pPlutoGraphic->m_GraphicFormat = pCell->m_GraphicFormat;
			pPlutoGraphic->LoadGraphic(pCell->m_pGraphicData,  pCell->m_GraphicLength, m_iRotation);
			RenderGraphic(pPlutoGraphic, PlutoRectangle(x,  y,  w,  h), pObj->m_bDisableAspectLock, point );
			delete pPlutoGraphic;
		}

        DesignObjText Text( pObj );
        // todo         Text.m_Rect = PlutoRectangle( x+pObj->BorderWidth,  y+pObj->BorderWidth,  w-( 2*pObj->BorderWidth ),  h-( 2*pObj->BorderWidth ) );
        Text.Data.m_rPosition = PlutoRectangle( x,  y,  w,  h );
        //pTextStyle->m_BackColor = PlutoColor( 0, 0, 0, 255 );
        //pTextStyle->m_ForeColor = PlutoColor( 255, 255, 255, 255 );
        //pTextStyle->m_iPK_VertAlignment=2;

		Text.Data.m_iPK_HorizAlignment = pTextStyle->m_iPK_HorizAlignment;
		Text.Data.m_iPK_VertAlignment = pTextStyle->m_iPK_VertAlignment;

        string sText = pCell->GetText(  );
        RenderText( sText, &Text, pTextStyle, point );
    }
    else
        g_pPlutoLogger->Write( LV_WARNING,  "Datagrid width or height is too small" );

g_PlutoProfiler->Stop("render cell");
    return bTransparentCell;
}
//------------------------------------------------------------------------
void Orbiter::RenderDataGrid( DesignObj_DataGrid *pObj, PlutoPoint point )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"RenderDataGrid %s %p",pObj->m_ObjectID.c_str(),pObj->m_pDataGridTable);
#endif
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    string delSelections;
    if ( !pObj->sSelVariable.empty(  ) )
    {
        PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
        delSelections = "|"+m_mapVariable[atoi( pObj->sSelVariable.c_str(  ) )]+"|";
        vm.Release(  );
    }

#if ( defined( PROFILING_GRID ) )
    clock_t clkStart = clock(  );
#endif

g_PlutoProfiler->Start("prepare render dg");
    PrepareRenderDataGrid( pObj,  delSelections );
g_PlutoProfiler->Stop("prepare render dg");

#if ( defined( PROFILING_GRID ) )
    clock_t clkAcquired = clock(  );
#endif

    if( !pObj->m_pDataGridTable )
        return;

	SolidRectangle( point.X + pObj->Data.m_rPosition.X, point.Y + pObj->Data.m_rPosition.Y, pObj->Data.m_rPosition.Width, pObj->Data.m_rPosition.Height, PlutoColor( 0, 0, 0 ) );

    // short for "number of ARRow ROWS": ArrRows
    // last screen exception: we consider one up arrow as not being there so we don't skip a row when we scroll up
    int ArrRows = 0;

    DataGridTable *pT = pObj->m_pDataGridTable;
    int i,  j; //indexes

    bool bAddedUpButton=false, bAddedDownButton=false;

	// See if we should add page up/down cells -- see notes at top of file
    if(  pObj->m_sExtraInfo.find( 'P' )!=string::npos  )
    {
        ArrRows = pObj->CanGoDown(  ) + ( pObj->CanGoUp(  ) && pObj->CanGoDown(  ) );
        if ( pObj->CanGoUp(  ) )
        {
            pObj->m_iUpRow = 0;
            DataGridCell * pCell = new DataGridCell( "<Scroll up>" );
			pCell->m_Colspan = pObj->m_pDataGridTable->m_ColumnCount;
            RenderCell( pObj,  pT,  pCell,  0,  0,  GRAPHIC_NORMAL, point );
            delete pCell;
            bAddedUpButton=true;
        }

        if ( pObj->CanGoDown(  ) )
        {
            pObj->m_dwIDownRow = pObj->m_pDataGridTable->m_RowCount - 1;
            DataGridCell * pCell = new DataGridCell( "<Scroll down>" );
			pCell->m_Colspan = pObj->m_pDataGridTable->m_ColumnCount;
            RenderCell( pObj,  pT,  pCell,  0,  pObj->m_dwIDownRow,  GRAPHIC_NORMAL, point );
            delete pCell;
            bAddedDownButton=true;
        }
    }

    for ( i = 0; i < pObj->m_pDataGridTable->m_RowCount - ArrRows; i++ )
    {
        for ( j = 0; j < pObj->m_pDataGridTable->m_ColumnCount; j++ )
        {
            int DGRow = ( ( i == 0 && pT->m_bKeepRowHeader ) ? 0 : i + pT->m_StartingRow );
            int DGColumn = ( j == 0 && pT->m_bKeepColumnHeader ) ? 0 : j + pT->m_StartingColumn;

            DataGridCell * pCell = pT->GetData( DGColumn,  DGRow );

            if ( pCell )
            {
                if ( !pObj->m_bDontShowSelection )
                {
                    int GraphicType = GRAPHIC_NORMAL;
                    if(  delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos  )
                        GraphicType = GRAPHIC_SELECTED;
                    else if(  ( pObj->m_iHighlightedRow!=-1 || pObj->m_iHighlightedColumn!=-1 ) &&
                        ( pObj->m_iHighlightedColumn==-1 || pObj->m_iHighlightedColumn==j ) &&
                        ( pObj->m_iHighlightedRow==-1 || pObj->m_iHighlightedRow==i )  )
                        GraphicType = GRAPHIC_HIGHLIGHTED;

                    RenderCell( pObj,  pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GraphicType, point );
                }
                else
                    RenderCell( pObj,  pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GRAPHIC_NORMAL, point );

                delete pCell;
                pCell = NULL;
            }
        }
    }

    pObj->m_pDataGridTable->m_RowCount = i + ArrRows;

#if ( defined( PROFILING_GRID ) )
    clock_t clkFinished = clock(  );

    g_pPlutoLogger->Write( LV_CONTROLLER, "Grid: %s took %d ms to acquire and %d ms to render",
        pObj->m_sGridID.c_str(), int(clkAcquired-clkStart), int(clkFinished-clkAcquired));
#endif
}
//------------------------------------------------------------------------
void Orbiter::PrepareRenderDataGrid( DesignObj_DataGrid *pObj,  string& delSelections )
{
    AcquireGrid( pObj,  pObj->m_GridCurCol,  pObj->m_GridCurRow,  pObj->m_pDataGridTable );
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable is now %p",pObj->m_ObjectID.c_str(),pObj->m_pDataGridTable);
#endif

	/* todo 2.0
    if(  pObj->GetParameterValue( C_PARAMETER_DATAGRID_ID_CONST ).substr( 0, 4 )=="EPG_" && pObj->m_pDataGridTable )
    {
    // Set to cache this grid,  currently disabled for anything other than the EPG
    m_GridCaching[pObj]=1;
    }
    */

    if ( pObj->m_pDataGridTable )
    {
        // Hide arrow if not on the first row.
        if(  pObj->m_pObjUp  )
            CMD_Show_Object( pObj->m_pObjUp->m_ObjectID, 0, "", "",  pObj->CanGoUp(  ) ? "1" : "0" );
        // Hide down arrow if the curren row + rows < the total rows in the grid
        if(  pObj->m_pObjDown  )
            CMD_Show_Object( pObj->m_pObjDown->m_ObjectID, 0, "", "",  pObj->CanGoDown(  ) ? "1" : "0" );
        // Hide arrow if not on the first column.
        if(  pObj->m_pObjLeft  )
            CMD_Show_Object( pObj->m_pObjLeft->m_ObjectID, 0, "", "",  pObj->CanGoLeft(  ) ? "1" : "0" );
        // Hide down arrow if the curren row + rows < the total rows in the grid
        if(  pObj->m_pObjRight  )
            CMD_Show_Object( pObj->m_pObjRight->m_ObjectID, 0, "", "",  pObj->CanGoRight(  ) ? "1" : "0" );

        // number of the row that has the respective scroll arrows
        pObj->m_dwIDownRow = -1;
        pObj->m_iUpRow = -1;
    }
}
//-----------------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::NeedToChangeScreens( ScreenHistory *pScreenHistory, bool bAddToHistory )
{
	if(m_bQuit)
		return;

#ifdef DEBUG
if(pScreenHistory)
g_pPlutoLogger->Write(LV_STATUS,"Need to change screens executed to %s (%d)",
					  pScreenHistory->m_pObj->m_ObjectID.c_str(),(int) bAddToHistory);
#endif

    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    m_vectObjs_GridsOnScreen.clear(  );
	m_vectObjs_VideoOnScreen.clear(  );
	m_pObj_NowPlayingOnScreen=NULL;
    dg.Release(  );

    PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );
    m_bRerenderScreen=true;
	m_bShowShortcuts=false;
    // The framework will call this when it's time to change screens.  This immediately calls RenderScreen.

	// ATTEN: The siganture was changed from RegionUp() -> RegionUp(int x, int y);
	// TODO: Make sure this is called properly (if the implementation will actually take the params into
	// account it will probably break something). )))
	RegionUp ( 0, 0);

    //  m_listDevice_Selected=""; // Nothing is selected anymore
    //  m_pLastSelectedObject=NULL;

    // todo 2.0?    m_bDrawBackFirst=true;
    // todo 2.0?    int Dim=100;

    if ( m_pScreenHistory_Current )
    {

		if( m_pScreenHistory_Current->m_pObj==m_pDesignObj_Orbiter_ScreenSaveMenu && m_pDesignObj_Orbiter_MainMenu!=m_pDesignObj_Orbiter_ScreenSaveMenu )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"Goto Screen -- wakign up from screen saver");
#endif
			CMD_Set_Main_Menu("N");
}

		m_pScreenHistory_Current->m_pObj->m_bActive=false;
        ObjectOffScreen( m_pScreenHistory_Current->m_pObj );
		for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->m_pObj->m_listPopups.begin();it!=m_pScreenHistory_Current->m_pObj->m_listPopups.end();++it)
		{
			PlutoPopup *pPlutoPopup = *it;
			ObjectOffScreen( pPlutoPopup->m_pObj );
			if( m_pActivePopup==pPlutoPopup )
				m_pActivePopup=NULL;
		}

        if(m_pScreenHistory_Current != pScreenHistory)
        {
            //update the list only if the screen is changed
            PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
            if(  bAddToHistory  )
                m_listScreenHistory.push_back( m_pScreenHistory_Current );
            DumpScreenHistory();
            if ( m_listScreenHistory.size(  ) > 64 )
            {
                delete m_listScreenHistory.front();
                m_listScreenHistory.pop_front(  );
            }
        }
    }

	// todo 2.0 SelectFirstObject(  );

	if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && pScreenHistory->m_pObj == m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutBlank )
	{
		m_tTimeoutTime = time(NULL) + m_iTimeoutBlank;

#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "@@@ About to call maint for display off with timeout: %d ms", m_iTimeoutBlank * 1000);
#endif

		CallMaintenanceInMiliseconds( m_iTimeoutBlank * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}
	else if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && pScreenHistory->m_pObj != m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutScreenSaver )
	{
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "@@@ About to call maint for screen saver with timeout: %d ms", m_iTimeoutScreenSaver * 1000);
#endif
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}

    m_pScreenHistory_Current=pScreenHistory;
    m_pScreenHistory_Current->m_pObj->m_bActive=true;
	if( m_pScreenHistory_Current->m_pObj->Data.m_dwTimeoutSeconds )
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->m_pObj->Data.m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->m_pObj, pe_ALL );

#ifdef DEBUG
    g_pPlutoLogger->Write( LV_STATUS, "Changing screen to %s", m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ) );
#endif
    ObjectOnScreenWrapper(  );
}
//------------------------------------------------------------------------
void Orbiter::ObjectOnScreenWrapper(  )
{
    PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );

    // ObjectOnSCreen will reset the hidden flags,  and CheckSpecial may set an object to hidden,  so do them in this order
    VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;
    PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
    m_vectObjs_NeedRedraw.clear(  );
	nd.Release();

    m_vectObjs_Selected.clear(  );
    m_vectObjs_TabStops.clear(  );
    m_mapDevice_Selected.clear(  );
    m_pObj_Highlighted=NULL;
    vm.Release(  );

    // I used to call DoLoadUnload from within ObjectOnScreen,  but that means some of the
    // load/unload actions and the initialize grid's are getting called before the objects
;    // are on the screen.  This caused a problem when there is an onload that sets
    // a variable and a datagrid that uses it.  PopulateDataGrid can get called before the
    // variable is set.  10-15-2003
    ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, m_pScreenHistory_Current->m_pObj );
	for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->m_pObj->m_listPopups.begin();it!=m_pScreenHistory_Current->m_pObj->m_listPopups.end();++it)
	{
		PlutoPopup *pPlutoPopup = *it;
	    ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pPlutoPopup->m_pObj );
	}

	// Do the on load actions for the screen itself,  and objects on it
    ExecuteCommandsInList( &m_pScreenHistory_Current->m_pObj->m_Action_LoadList, m_pScreenHistory_Current->m_pObj, 0, 0 );

	HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );

    //if(  m_vectObjs_TabStops.size(  )  )
    //   HighlightFirstObject();

    //if(NULL != m_pObj_Highlighted &&  m_pObj_Highlighted->m_vectHighlightedGraphic.size())
	//	HighlightObject( m_pObj_Highlighted );
}
//------------------------------------------------------------------------
// If an object has the don't reset state true,  it won't reset to normal,  and it's children won't reset either
void Orbiter::ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj )
{
	// Do this again since sometimes there will be several grids with the same name within the application and if
	// we're going to do a lookup, such as with seek grid, we want to find the one most recently on screen
	if( pObj->Data.m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pObj_Datagrid = (DesignObj_DataGrid *) pObj;
		m_mapObjs_AllGrids[pObj_Datagrid->m_sGridID] = pObj_Datagrid;
	}
	if( pObj->Data.m_iRegenInterval )
		CallMaintenanceInMiliseconds(pObj->Data.m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);

    pVectDesignObj_Orbiter->push_back( pObj );
    pObj->m_bOnScreen=true;

    // Add it to the list of tab stops whether it's visible or not.  The findfirst/next will skip of hidden objects anyway
    // And this way we don't need to worry about the changing state of objects that are hidden/shown
    if(  pObj->Data.m_bTabStop  ) 
        m_vectObjs_TabStops.push_back( pObj );

	if( pObj->Data.m_ObjectType==DESIGNOBJTYPE_Broadcast_Video_CONST )
	{
        pObj->m_pvectCurrentGraphic = NULL;
		for(size_t i = 0; i < pObj->m_vectGraphic.size(); i++)
		{
			delete pObj->m_vectGraphic[i];
		}
		pObj->m_vectGraphic.clear();
		pObj->m_iCurrentFrame = 0;
		m_vectObjs_VideoOnScreen.push_back(pObj);
		m_bAlreadyQueuedVideo=false;
	}

    if(  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED  )
        m_vectObjs_Selected.push_back( pObj );

    // Move InitializeGrid down so it gets called after the LoadActions,  which may set variables that it will use

    //  Move this down because there's a hide object in the onload
    //  actions that is getting executed before the object's state is reset
    //  DoLoadUnloadActions( pObj->m_Action_LoadList, pObj, CTRLCOMMAND_UNLOAD );

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
        ObjectOnScreen( pVectDesignObj_Orbiter, ( DesignObj_Orbiter * )( *iHao ) );
}
//------------------------------------------------------------------------
void Orbiter::GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic)
{
	size_t size = (*pvectGraphic).size();
	for(size_t i = 0; i < size; i++)
		(*pvectGraphic)[i]->Clear();
}
//------------------------------------------------------------------------
void Orbiter::ObjectOffScreen( DesignObj_Orbiter *pObj )
{
#ifdef DEBUG
    if(  pObj->m_ObjectID.find( "2715" )!=string::npos  )
        //if(  pObj->Data.m_iBaseObjectID == 2707  )
    {
        g_pPlutoLogger->Write( LV_STATUS, "control off screen state: %d undo: %p", pObj->m_GraphicToDisplay, pObj->m_pGraphicToUndoSelect );
    }
#endif

    pObj->m_bOnScreen=false;
    if(  pObj->m_pGraphicToUndoSelect  )
    {
        delete pObj->m_pGraphicToUndoSelect;
        pObj->m_pGraphicToUndoSelect=NULL;
    }

    GraphicOffScreen( &(pObj->m_vectGraphic) );
    GraphicOffScreen( &(pObj->m_vectSelectedGraphic) );
	GraphicOffScreen( &(pObj->m_vectHighlightedGraphic) );

    size_t i;
    for(i = 0; i < pObj->m_vectAltGraphics.size(); ++i)
        GraphicOffScreen(&(pObj->m_vectAltGraphics[i]));

    pObj->m_pvectCurrentGraphic = NULL;
	pObj->m_pvectCurrentPlayingGraphic = NULL;

    ExecuteCommandsInList( &pObj->m_Action_UnloadList, pObj, 0, 0 );

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        ObjectOffScreen( ( DesignObj_Orbiter * )*iHao );
    }
}

//------------------------------------------------------------------------
void Orbiter::SelectedObject( DesignObj_Orbiter *pObj,  int X,  int Y)
{
    PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

    if ( pObj->Data.m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST && X>=0 && Y>=0  )
    {
#ifdef DEBUG
        g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33mSelected datagrid: %s\x1b[0m", pObj->m_ObjectID.c_str(  ) );
#endif
        if ( !SelectedGrid( ( DesignObj_DataGrid * ) pObj,  X-pObj->Data.m_rPosition.X,  Y-pObj->Data.m_rPosition.Y ) )
            return;
    }

#ifdef DEBUG
    g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33mSelected objs: %s\x1b[0m with %d zones", pObj->m_ObjectID.c_str(  ), ( int ) pObj->m_ZoneList.size(  ) );
#endif

    // There's a problem that we draw the selected state before we show or hide other objects,  and this causes
    // the other objects to be drawn on top of the selected state.  We'll execute the commands first so that
    // show/hides are executed before setting the selected state
    DesignObjZoneList::iterator iZone;

	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
    {
        DesignObjZone *pDesignObjZone = ( *iZone );
        if(  pDesignObjZone->m_Rect.Width==0 || pDesignObjZone->m_Rect.Height==0 || pDesignObjZone->m_Rect.Contains( X, Y )  )
        {
            ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, X, Y );
        }
    }

    if(  pObj->m_vectSelectedGraphic.size() && pObj->m_GraphicToDisplay != GRAPHIC_SELECTED ) // TODO 2.0 && m_ChangeToScreen.length(  ) == 0 )
    {
        pObj->m_GraphicToDisplay=GRAPHIC_SELECTED;

		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        m_vectObjs_NeedRedraw.push_back( pObj );
		nd.Release();

		if(pObj->m_vectSelectedGraphic.size())
		{
			PlutoGraphic *pPlutoGraphic = pObj->m_vectSelectedGraphic[0];

			if(pPlutoGraphic->m_GraphicFormat != GR_MNG)
			{
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
				if(  !pObj->Data.m_bDontResetState  )
					CallMaintenanceInMiliseconds( 500, &Orbiter::DeselectObjects, ( void * ) pObj, pe_NO );
			}
		}

        // Unless the screen's don't reset state is set,  we'll clear any other selected graphics
        if(  !m_pScreenHistory_Current->m_pObj->Data.m_bDontResetState  )
        {
            for( size_t s=0;s<m_vectObjs_Selected.size(  );++s )
            {
                DesignObj_Orbiter *pObj_Sel = m_vectObjs_Selected[s];
                if(  pObj_Sel->m_GraphicToDisplay==GRAPHIC_SELECTED  )
                {
#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS, "About to reset state for object with id %s",
						pObj_Sel->m_ObjectID.c_str());
#endif
					//if it is playing, cancel this
					pObj_Sel->m_pvectCurrentPlayingGraphic = NULL;
					pObj_Sel->m_iCurrentFrame = 0;
                    pObj_Sel->m_GraphicToDisplay = GRAPHIC_NORMAL;

#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS, "State reseted for object with id %s",
						pObj_Sel->m_ObjectID.c_str());
#endif
					PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
                    m_vectObjs_NeedRedraw.push_back( pObj_Sel );
					nd.Release();
                }
            }
            m_vectObjs_Selected.clear(  );
        }
        m_vectObjs_Selected.push_back( pObj );
    }

#ifdef PRONTO
    if ( pObj->Data.m_ObjectType == DESIGNOBJTYPE_Pronto_File_CONST )
    {
        if ( pObj->Data.m_pCCF )
            pObj->Data.m_pCCF->Touch( X-pObj->Data.m_rPosition.X,  Y-pObj->Data.m_rPosition.Y );
        return;
    }
#endif

#pragma warning( "bad,  bad hack" )
    if(  pObj->Data.m_iBaseObjectID==2706  )
    {
        // There's a problem when you select the video button.  The music button was already selected,  and saved a snapshop in the 'savebackgroundfordeselect'.
        // The new video objects are drawn,  then the deselected music object,  which includes a snapshot of the music datagrid.  If I put refresh in the video object
        // it has no effect,  since the commands are executed before the deselect state.  Until we find a solution,  hack in a redraw after resetting the video state
        PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
		m_vectObjs_NeedRedraw.push_back( m_pScreenHistory_Current->m_pObj );
    }
    /*
    string VariableData = "";
    map<string, string>::iterator ivv;
    for( ivv=pObj->Data.m_VariableValue.begin(  );ivv!=pObj->Data.m_VariableValue.end(  );++ivv )
    {
    if ( VariableData.length(  )==0 )
    VariableData = StringUtils::itos( pObj->Data.m_VariableValue.size(  ) );

    VariableData+="|";
    VariableData+=( *ivv ).first+"|"+( *ivv ).second;
    m_mapVariable[atoi( ( *ivv ).first.c_str(  ) )] = ( *ivv ).second;
    }
    */

    /* todo 2.0
    if( pObj->Data.m_bProcessActionsAtServer )
    {
    Message *pThisMessage = new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_PROCESS_BUTTON_AT_SERVER_CONST, 3,
    COMMANDPARAMETER_PK_DesignObj_CONST, StringUtils::itos( pObj->Data.m_iBaseObjectID ),
    C_COMMANDPARAMETER_ONLY_IF_ON_SCREEN_CONST, m_pVisibleScreen->m_ObjectID,
    COMMANDPARAMETER_Text_CONST,  m_mapVariable[VARIABLE_USER_INPUT_CONST] );
    if(  pButtonTouchMessage==NULL  )
    pButtonTouchMessage=pThisMessage;
    else
    pButtonTouchMessage->m_vectExtraMessages.push_back( pThisMessage );
    }
    */

    /* todo 2.0
    if(  pButtonTouchMessage  )
    {
    #ifdef DEBUG
    g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33;1m#%d Selected object: %s # \x1b[0m  click: %d, %d", m_dwPK_Device, pObj->m_ObjectID.c_str(  ), X, Y );
    #endif
    Message *pThisMessage = new Message( m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Orbiter_BUTTON_TOUCHED_CONST, 4,
    C_EVENTPARAMETER_PK_OBJECT_CONST, pObj->m_ObjectID.c_str(  ),
    C_EVENTPARAMETER_ID_CONST, StringUtils::itos( CTRLCOMMAND_ACTIVATE ).c_str(  ),
    C_EVENTPARAMETER_XPOS_CONST, StringUtils::itos( X-pObj->Data.m_rPosition.X ).c_str(  ),
    C_EVENTPARAMETER_YPOS_CONST, StringUtils::itos( Y-pObj->Data.m_rPosition.Y ).c_str(  ) );

    pButtonTouchMessage->m_vectExtraMessages.push_back( pThisMessage );

    QueueMessageToRouter( pButtonTouchMessage );
    //      GetEvents(  )->Orbiter_Object_Actions( StringUtils::itos( NumActions ) + "\t" +ActionData,
    //          pObj->m_ObjectID,  StringUtils::itos( CTRLCOMMAND_ACTIVATE ),  StringUtils::itos( X-pObj->Data.m_rPosition.X ),
    //          StringUtils::itos( Y-pObj->Data.m_rPosition.Y ) );
    }
    */
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  int X,  int Y )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    if ( !pDesignObj_DataGrid->m_pDataGridTable )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"Selected grid %s but m_pDataGridTable is NULL",pDesignObj_DataGrid->m_ObjectID.c_str());
#endif
		return false;
}

    bool bFoundSelection = false,  bFinishLoop = false;

    DataGridCell *pLastCell = NULL;

    int LastI,  LastJ;

    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    string delSelections = "|"+m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable]+"|";
    string NewValue;

    vm.Release(  );

    // There's a variable associated with this,  so it's a selectable grid.  Which cell was touched?
    // Go through the cells on the screen
    int i, j;
    DataGridTable *pT = pDesignObj_DataGrid->m_pDataGridTable;

    // Let's say we have six rows,  and we are on the last page.  We'll only show 5 plus an
    // up,  so datagrid will set the row count to be 5 ( you can't past the number of rows left )
    // So,  if we have an up,  we need to loop one more time for the last row only if we
    // are showing all the rows that are left.  Otherwise,  row count will be correct
    for( i=0;i<=pT->m_RowCount && !bFinishLoop ;i++ )
    {
        for( j=pT->m_ColumnCount-1;j>=0 && !bFinishLoop;j-- )
        {
            int x, y, w, h;
            if(  i==pDesignObj_DataGrid->m_iUpRow  || i==pDesignObj_DataGrid->m_dwIDownRow  )
            {
                if(  pDesignObj_DataGrid->m_iUpRow!=-1 && i!=pDesignObj_DataGrid->m_dwIDownRow && i!=pDesignObj_DataGrid->m_iUpRow  )
                    GetGridCellDimensions( pDesignObj_DataGrid,  1,  1,  j,  i+1,  x,  y,  w,  h );
                else
                    GetGridCellDimensions( pDesignObj_DataGrid,  1,  1,  j,  i,  x,  y,  w,  h );
                PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
                    int ContainsX=X+pDesignObj_DataGrid->Data.m_rPosition.X;
                int ContainsY=Y+pDesignObj_DataGrid->Data.m_rPosition.Y;
                vm.Release(  );
                if ( PlutoRectangle( x,  y,  w,  h ).Contains( ContainsX,  ContainsY ) )
                {
                    if(  i==pDesignObj_DataGrid->m_dwIDownRow  )
                    {
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
					    if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
							m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable] = "";
						}
                        CMD_Scroll_Grid( "",  pDesignObj_DataGrid->m_ObjectID,  DIRECTION_Down_CONST );
                        return false;  // No more processing
                    }
                    else
                    {
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
					    if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
							m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable] = "";
						}
                        CMD_Scroll_Grid( "",  pDesignObj_DataGrid->m_ObjectID,  DIRECTION_Up_CONST );
                        return false;  // No more processing
                    }
                }
                else
                    continue;
            }

            int DGColumn = ( j==0 && pT->m_bKeepColumnHeader ) ? 0 : j+pT->m_StartingColumn;
            int DGRow = ( i==0 && pT->m_bKeepRowHeader ) ? 0 : i+pT->m_StartingRow;
            if(  pDesignObj_DataGrid->m_iUpRow!=-1  )
                DGRow--; // Everything is shifted up
            DataGridCell *pCell=pT->GetData( DGColumn,  DGRow );
            if ( pCell )
            {
                GetGridCellDimensions( pDesignObj_DataGrid,  pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );
                PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
                    int ContainsX=X+pDesignObj_DataGrid->Data.m_rPosition.X;
                int ContainsY=Y+pDesignObj_DataGrid->Data.m_rPosition.Y;
                vm.Release(  );
                if ( ( delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos ) )
                {
                    if ( bFoundSelection )
                    {
                        if ( RenderCell( ( DesignObj_DataGrid * )pDesignObj_DataGrid,  pT,  pCell,  j,  i ,  GRAPHIC_NORMAL ) )
						{
							PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
                            m_vectObjs_NeedRedraw.push_back( pDesignObj_DataGrid );
						}
                        bFinishLoop = true;
                    }
                    delete pLastCell;
                    pLastCell = pCell;
                    LastI = i;
                    LastJ = j;
                }

                if ( PlutoRectangle( x,  y,  w,  h ).Contains( ContainsX,  ContainsY ) )
                {
					pDesignObj_DataGrid->m_iHighlightedColumn=DGColumn;
					pDesignObj_DataGrid->m_iHighlightedRow=DGRow;
                    SelectedGrid( pDesignObj_DataGrid,  pCell );
                    bFinishLoop = true;
                    bFoundSelection = true; // Is this correct????  Hacked in this time
				    m_vectObjs_NeedRedraw.push_back(pDesignObj_DataGrid);
                }

                if ( pLastCell != pCell )
                    delete pCell;
            }
        }
    }
    delete pLastCell;

    return bFoundSelection;
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  DataGridCell *pCell )
{
	pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
	m_sLastSelectedDatagrid=pCell->GetText();

    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    if(  pCell->m_pMessage  )
    {
        Message *pMessage = new Message( pCell->m_pMessage );
        // See if we are re-rendering this grid.  If so
        if(  pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND &&
            pMessage->m_dwID==COMMAND_Populate_Datagrid_CONST &&
            pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST]==pDesignObj_DataGrid->m_sGridID  )
        {
#ifdef DEBUG
            g_pPlutoLogger->Write( LV_CONTROLLER, "re-rendering grid: ( %s", pDesignObj_DataGrid->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ).c_str(  ) );
#endif
            bool bRefreshGrids=false;
            string GridID = pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST];
            DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen=NULL;
            // See if this grid is onscreen
            for( size_t s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
            {
                DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[s];
                if(  pDesignObj_DataGrid->m_sGridID == GridID  )
                {
                    pDesignObj_DataGrid_OnScreen=pDesignObj_DataGrid;
                    pDesignObj_DataGrid_OnScreen->m_iHighlightedRow=0;
                    pDesignObj_DataGrid_OnScreen->m_iHighlightedColumn=0;
                    bRefreshGrids=true;
                }
            }

            string Response;

            // This message has out parameters with a variable to assign
            Message *pMessage_Out = m_pcRequestSocket->SendReceiveMessage( pMessage );
            if(  pMessage_Out  )
            {
                int iPK_Variable = atoi( pMessage_Out->m_mapParameters[COMMANDPARAMETER_PK_Variable_CONST].c_str(  ) );
                if(  iPK_Variable  )
				{
					PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
                    m_mapVariable[iPK_Variable] = pMessage_Out->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST];
				}
                delete pMessage_Out;
            }
		    pDesignObj_DataGrid->m_GridCurCol = pDesignObj_DataGrid->m_iInitialColNum;
	        pDesignObj_DataGrid->m_GridCurRow = pDesignObj_DataGrid->m_iInitialRowNum;
            pDesignObj_DataGrid->bReAcquire=true;
            if(  bRefreshGrids  )
			{
				PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
                m_vectObjs_NeedRedraw.push_back( pDesignObj_DataGrid );
			}
        }
        else if( pMessage->m_dwPK_Device_To==m_dwPK_Device || pMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
		{
			pMessage->m_dwPK_Device_To=m_dwPK_Device;
			ReceivedMessage(pMessage);
		}
        else
        {
#pragma warning( "Look into this and the sendreceive above.  one deletes,  the other not.  if it deletes,  you can't select the grid twice.  if not,  the framework will delete.  should make a copy I think" );
            QueueMessageToRouter( pMessage );

            //          QueueMessageToRouter( pMessage );  // I think this caused some grids not to immediately refresh  **TODO** look into this
            //          pMessage=NULL;  // Send message will delete the messages.  It's okay--it's just a copy anyway
        }
    }

    // WAS: selection on "click to select cell,  set variable,  click play button,  play what in the just set var" interface
    // Side-effect on single select interface: the clicked row was redrawn after drawing the new grid
    if ( !pDesignObj_DataGrid->m_iPK_Variable || !pCell->m_bSelectable )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "No datagrid variable was updated");
#endif
        return true;
	}

    string NewValue;
    int PK_Variable = pDesignObj_DataGrid->m_iPK_Variable;
    
    //handle multi select
    if(pDesignObj_DataGrid->m_bIsMultiSelect)
    {
        DataGridTable *pT = pDesignObj_DataGrid->m_pDataGridTable;

        PLUTO_SAFETY_LOCK(vm, m_VariableMutex);
        string delSelections = "|"+m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable]+"|";
        string Selections = m_mapVariable[PK_Variable];
        vm.Release();
        string::size_type valpos;

        int i, j;
        i = pCell->m_Colspan;
        j = pCell->m_Rowspan;

        if((valpos = delSelections.find("|" + string(pCell->GetValue()) + "|")) != string::npos)
        {
            NewValue = delSelections.substr(0, valpos) + Selections.substr(valpos + strlen(pCell->GetValue()));
            
            if(NewValue.length() > 0)
                NewValue = NewValue.substr(1);
        }
        else
        {
            if(Selections.length() > 0)
                NewValue = Selections + "|" + pCell->GetValue();
            else
                NewValue = pCell->GetValue();
        }
    }
    else
    {
        // Not multi-select.  Just replace it.
        NewValue =  pCell->GetValue(  );
    }

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_WARNING, "Need to update variable %d of the datagrid with value %s",
        pDesignObj_DataGrid->m_iPK_Variable, NewValue.c_str());
#endif

    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    m_mapVariable[PK_Variable] = NewValue;

    return true;
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( int DGRow )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    //used in BD_PC_SelectedFromList
    //
    //this method selects the DGRow-st item from the grid
    //( should to be only one on current screen ) -> for mobile

    if( m_vectObjs_GridsOnScreen.size(  ) != 1 )
    {
        g_pPlutoLogger->Write( LV_WARNING,
            "It should be one and only one visible on this screen!" );

        return false;
    }

    vector<class DesignObj_DataGrid *>::iterator it = m_vectObjs_GridsOnScreen.begin(  );
    DesignObj_DataGrid *pDesignObj_DataGrid = *it;

	int iSelectedColumn = pDesignObj_DataGrid->m_iInitialColNum;
	//if 'c' - column  extraoption is specified, we'll send to phone the specified column
	size_t sPos;
	if((sPos = pDesignObj_DataGrid->m_sExtraInfo.find( 'c' )) != string::npos)
	{
		if(sPos + 1 < pDesignObj_DataGrid->m_sExtraInfo.size())
			iSelectedColumn = pDesignObj_DataGrid->m_sExtraInfo[sPos + 1] - '0';
	}

    DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData( iSelectedColumn,  DGRow );
	pDesignObj_DataGrid->m_iHighlightedRow = DGRow;
	pDesignObj_DataGrid->m_GridCurCol = iSelectedColumn;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "Selected row: %d, selected column: %d",
		DGRow, iSelectedColumn);
#endif

	if( pCell )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable] = pCell->GetValue(  );
		vm.Release();
		delete pCell;
	}

	return true;
}
void Orbiter::SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter)
{
    if( pDesignObj_Orbiter->m_pFloorplanObject )
        SelectedFloorplan(pDesignObj_Orbiter);

	if( pDesignObj_Orbiter->Data.m_iBaseObjectID==DESIGNOBJ_butPINGo_CONST)
	{
		if( m_bRestrictedOp_IsUser )
		{
			bool bValid=false;
			DCE::CMD_Verify_PIN CMD_Verify_PIN(m_dwPK_Device, m_dwPK_Device_SecurityPlugIn,
				m_iRestrictedOp_ID,m_mapVariable[VARIABLE_Datagrid_Input_CONST],&bValid);
			if( SendCommand(CMD_Verify_PIN) && bValid )
			{
				CMD_Set_Current_User(m_iRestrictedOp_ID);
				CMD_Goto_Screen(0,m_sMainMenu,"","",false,false);
			}
			else
			{
				m_mapVariable[VARIABLE_Datagrid_Input_CONST]="";
				CMD_Set_Text(m_pScreenHistory_Current->m_pObj->m_ObjectID,"***INVALID***",TEXT_PIN_Code_CONST);
			}
		}
	}
	else if( pDesignObj_Orbiter->Data.m_iBaseObjectID==DESIGNOBJ_mnuDisplayPower_CONST)
	{
		if( !m_pLocationInfo->m_dwPK_Device_MediaDirector || m_pLocationInfo->m_dwPK_Device_MediaDirector==DEVICEID_NULL )
		{
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayON_OtherControlling_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
		}
		else
		{
			// We know we've got an m/d and it's not us.  Find out the state and status
			string sState = GetState(m_pLocationInfo->m_dwPK_Device_MediaDirector);
			string sStatus = GetStatus(m_pLocationInfo->m_dwPK_Device_MediaDirector);

			if( sStatus.length()>1 && sStatus.substr(0,2)=="MD" )
			{
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
				// See if we've got media playing
				if( m_sNowPlaying.length() )
					CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "1" );
				else
					CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			}
			else
			{
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			}

			// See if it's on.  If so, enable the halt and displayoff buttons
			if( sStatus.length()>3 && (sStatus.substr(3) == "ON" || sStatus.substr(3) == "BLACK") )
			{
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "1" );
			}
			else
			{
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
				CMD_Show_Object( m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
			}
		}
	}
	else if( pDesignObj_Orbiter->Data.m_iBaseObjectID==DESIGNOBJ_mnuScreenSaver_CONST )
	{
		CMD_Set_Text(pDesignObj_Orbiter->m_ObjectID, StringUtils::itos(	int(m_tTimeoutTime - time(NULL)) ) + " seconds",TEXT_USR_ENTRY_CONST);
		CMD_Continuous_Refresh("10");
	}
	else if( pDesignObj_Orbiter->Data.m_iBaseObjectID==DESIGNOBJ_objHouseStatusIndicator_CONST )
	{
		// This is actually at startup time.  Figure out what device group this is controlling
		int PK_DeviceGroup=0;
		for(DesignObjZoneList::iterator itZ=pDesignObj_Orbiter->m_ZoneList.begin();itZ!=pDesignObj_Orbiter->m_ZoneList.end();++itZ)
		{
			DesignObjZone *pDesignObjZone = *itZ;

			for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
			{
				DesignObjCommand *pDesignObjCommand = *it;
				if( pDesignObjCommand->Data.m_PK_Command==COMMAND_Set_Variable_CONST )
				{
					int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
					if( PK_Variable == VARIABLE_PK_DeviceGroup_CONST )
						PK_DeviceGroup = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				}
			}
		}
		CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "housemode" + StringUtils::itos(PK_DeviceGroup),true);
	}
}

void Orbiter::SelectedFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter)
{
    m_pScreenHistory_Current->m_dwPK_Device = pDesignObj_Orbiter->m_pFloorplanObject->PK_Device;
    CMD_Set_Variable(VARIABLE_Array_ID_CONST,pDesignObj_Orbiter->m_pFloorplanObject->ObjectTypeDescription);
    CMD_Set_Variable(VARIABLE_Array_Desc_CONST,pDesignObj_Orbiter->m_pFloorplanObject->DeviceDescription);
    CMD_Set_Variable(VARIABLE_Status_CONST,pDesignObj_Orbiter->m_pFloorplanObject->Status);

	// If this has a toolbar, and either (1) there is no previously selected object, 
	// (2) there was a previously selected object but it wasn't a floorplan or (3) used a different toolbar
	// then we need to activate this toolbar and remove any previous one
	int PK_DesignObj_Toolbar_ToTurnOn=0,PK_DesignObj_Toolbar_ToTurnOff=0;
	if( pDesignObj_Orbiter->m_pFloorplanObject && pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar &&
		(!m_pObj_LastSelected || m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar!=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar) )
	{
		PK_DesignObj_Toolbar_ToTurnOn=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;
		if( m_pObj_LastSelected && m_pObj_LastSelected->m_pFloorplanObject && m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar )
			PK_DesignObj_Toolbar_ToTurnOff=m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;
	}
	else if( m_pObj_LastSelected && m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar &&
			pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar!=m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar )
		PK_DesignObj_Toolbar_ToTurnOff=m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;


	// We've selected this object twice, cycle through the vector of device groups
    if( m_pObj_LastSelected==pDesignObj_Orbiter )
    {
        // We went past the end, select nothing.  Or this is an ent area with no device pointer, so there are no groups anyway
        if( !pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base ||
			m_iLastEntryInDeviceGroup>=(int) pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup.size()-1 )
        {
			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
			m_vectObjs_NeedRedraw.push_back((DesignObj_Orbiter *) pDesignObj_Orbiter->m_pParentObject);
			nd.Release();

            m_mapDevice_Selected.clear();
            m_pObj_LastSelected=NULL;
            m_iLastEntryInDeviceGroup=-1;  // Start at the beginning
			PK_DesignObj_Toolbar_ToTurnOn=0;
			PK_DesignObj_Toolbar_ToTurnOff=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;
		}
        else
        {
            // Maybe this doesn't belong to any device groups, just toggle on and off
            if( pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup.size()==0 )
            {
                m_mapDevice_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base;
                m_iLastEntryInDeviceGroup=0;  // If we touch it again, it will trigger the block above
            }
            else
            {
                // Select all the items in the group
                ++m_iLastEntryInDeviceGroup;
                DeviceGroup *pDG = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup[m_iLastEntryInDeviceGroup];
                for(int i=0;i<(int) pDG->m_vectDeviceData_Base.size();++i)
                {
                    DeviceData_Base *pD = pDG->m_vectDeviceData_Base[i];
                    m_mapDevice_Selected[pD->m_dwPK_Device] = pD;
                }
            }
        }

    }
    else
    {
		if( m_mapDevice_Selected.find(pDesignObj_Orbiter->m_pFloorplanObject->PK_Device)!=m_mapDevice_Selected.end() )
			m_mapDevice_Selected.erase(pDesignObj_Orbiter->m_pFloorplanObject->PK_Device);
		else
	        m_mapDevice_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base;
        m_pObj_LastSelected=pDesignObj_Orbiter;
    }

	// The toolbar will be a direct child of the topmost object, whether it's a screen or a popup
	if( PK_DesignObj_Toolbar_ToTurnOn )
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOn),0,"","","1");
	if( PK_DesignObj_Toolbar_ToTurnOff )
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOff),0,"","","0");

/*
    if( pDesignObj_Orbiter->m_pParentObject->Data.m_iBaseObjectID==OBJECT_FPENTERTAINMENT_CONST )
    {
        bool bResponse;
        int iPK_Variable=0;
        string sValue_To_Assign;
        DCE::CMD_Populate_Datagrid_DT CMD_Populate_Datagrid_DT(m_dwPK_Device, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse, StringUtils::itos(m_dwIDataGridRequestCounter),"HAGA_" + StringUtils::itos(m_dwPK_Device),
            pObj->m_iPK_Datagrid,SubstituteVariables(pObj->m_sOptions,pObj,0,0),&iPK_Variable,&sValue_To_Assign,&bResponse);
        if( !SendCommand(CMD_Populate_Datagrid_DT) || !bResponse ) // wait for a response
            g_pPlutoLogger->Write(LV_CRITICAL,"Populate datagrid: %d failed",pObj->m_iPK_Datagrid);

        // We're on the house-at-a-glance floorplan.  These are the activies at the bottom, not the streams
        m_pRequestSocket->SendOCMessage(new OCMessage(m_DeviceID, DEVICEID_DATAGRID, PRIORITY_NORMAL,
            MESSAGETYPE_COMMAND, ACTION_POPULATE_DATAGRID_CONST, 2,
            C_ACTIONPARAMETER_DATAGRID_ID_CONST, ("HAGA_" + StringUtils::itos(m_DeviceID)).c_str(),
            C_ACTIONPARAMETER_TYPE_CONST, (StringUtils::itos(DGTYPE_HAG_ACTIVITIES) + "," + m_sSelectedDevices).c_str()));
    }

    Invalidate();
    return;
*/
	CMD_Refresh("");
}
//------------------------------------------------------------------------
bool Orbiter::ClickedButton( DesignObj_Orbiter *pObj, int PK_Button )
{
    bool bFoundHandler=false;

    if ( pObj->IsHidden(  ) )
        return false;

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
		DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
        bool bResult=ClickedButton( pDesignObj_Orbiter, PK_Button );
        if(  !bFoundHandler && bResult  )
            bFoundHandler=true;
    }
    if( pObj->Data.m_iPK_Button==PK_Button || pObj->Data.m_iPK_Button==BUTTON_Any_key_CONST  )
    {
        SelectedObject( pObj );
        return true;
    }
    return bFoundHandler;
}
//------------------------------------------------------------------------
bool Orbiter::ClickedRegion( DesignObj_Orbiter *pObj, int X, int Y, DesignObj_Orbiter *&pTopMostAnimatedObject )
{
    PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

    if(pObj->IsHidden())
        return false;

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        ClickedRegion( ( DesignObj_Orbiter * )( *iHao ), X, Y, pTopMostAnimatedObject );
    }

    if( pObj->Data.m_rPosition.Contains( X, Y ) )
    {
        if(  pObj->Data.m_bAnimate && !pTopMostAnimatedObject  )  // These get called from topmost down,  so don't overwrite if a higher object already did
        {
            pTopMostAnimatedObject = pObj;
        }

		if( pObj->m_dbHitTest.m_dwSize )
		{
			int LineWidth = pObj->Data.m_rBackgroundPosition.Width / 8 + 1; // The width of each line
			int x = X - pObj->Data.m_rPosition.Left();
			int y = Y - pObj->Data.m_rPosition.Top();
			char c = pObj->m_dbHitTest.m_pBlock[ y * LineWidth + x/8 ];
			if( !(c & 1 << (x%8)) )
				return false;
		}

		SelectedObject( pObj, X, Y );
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::HighlightObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
    int x = point.X + pObj->Data.m_rBackgroundPosition.X;
    int y = point.Y + pObj->Data.m_rBackgroundPosition.Y;
    int w = pObj->Data.m_rBackgroundPosition.Width;
    int h = pObj->Data.m_rBackgroundPosition.Height;

    PlutoColor WhiteColor(255, 255, 255, 100);
    PlutoColor RedColor(255, 0, 0, 100);

    HollowRectangle(x    , y    , w - 1, h - 1, RedColor);
    HollowRectangle(x + 1, y + 1, w - 3, h - 3, RedColor);
    HollowRectangle(x + 2, y + 2, w - 5, h - 5, WhiteColor);
    HollowRectangle(x + 3, y + 3, w - 7, h - 7, WhiteColor);
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
    int x = point.X + pObj->Data.m_rBackgroundPosition.X;
    int y = point.Y + pObj->Data.m_rBackgroundPosition.Y;
    int w = pObj->Data.m_rBackgroundPosition.Width;
    int h = pObj->Data.m_rBackgroundPosition.Height;

    PlutoColor WhiteColor(255, 255, 255, 100);
    PlutoColor BlueColor(0, 0, 255, 100);

    HollowRectangle(x    , y    , w - 1, h - 1, BlueColor);
    HollowRectangle(x + 1, y + 1, w - 3, h - 3, BlueColor);
    HollowRectangle(x + 2, y + 2, w - 5, h - 5, WhiteColor);
    HollowRectangle(x + 3, y + 3, w - 7, h - 7, WhiteColor);
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::HighlightFirstObject(  )
{
	if(m_bQuit)
		return;

    if( !m_ScreenMap.size(  )  || !m_vectObjs_TabStops.size(  )  )
        return;

    DesignObj_OrbiterMap::iterator iter;
    for( iter = m_ScreenMap.begin(  ); iter != m_ScreenMap.end(  ); iter++ )
        if( HighlightFirstChildObject( ( *iter ).second ) )
            break;
}
//------------------------------------------------------------------------
/*virtual*/ bool Orbiter::HighlightFirstChildObject( DesignObj_Orbiter* pObj )
{
	if(m_bQuit)
		return false;

    DesignObj_DataList::iterator iter;
    for( iter = pObj->m_ChildObjects.begin(  ); iter != pObj->m_ChildObjects.end(  ); iter++ )
    {
        DesignObj_Data *p = ( *iter );
/*      if(  p->m_ObjectID.find( "3016" )!=string::npos  )
        {
            int k=2;
        }
*/
        if( ( *iter )->Data.m_bTabStop && ( ( DesignObj_Orbiter * ) ( *iter ) )->m_bOnScreen &&
            ( ( DesignObj_Orbiter * ) ( *iter ) )->IsHidden(  ) == false && ( *iter )->Data.m_bTabStop  )
        {
            m_pObj_Highlighted = ( DesignObj_Orbiter * ) *iter;
            return true;
        }
        else
        {
            if( HighlightFirstChildObject( ( DesignObj_Orbiter * ) *iter ) )
                return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------
void Orbiter::FindObjectToHighlight(
                                    DesignObj_Orbiter **ppNextObjectToRight,
                                    DesignObj_Orbiter *pObj,
                                    int PK_Direction )
{
    int nXDistNew,  nYDistNew;
    int nXDistOld,  nYDistOld;

    DesignObj_DataList::iterator iter;
    for( iter = pObj->m_ChildObjects.begin(  ); iter != pObj->m_ChildObjects.end(  ); iter++ )
    {
        DesignObj_Orbiter *pCurrentObj = ( DesignObj_Orbiter * ) *iter;

        if( pCurrentObj->Data.m_bTabStop && pCurrentObj->m_bOnScreen && !pCurrentObj->IsHidden(  ) )
        {
            nXDistNew = m_pObj_Highlighted->Data.m_rBackgroundPosition.X - pCurrentObj->Data.m_rBackgroundPosition.X;
            nYDistNew = m_pObj_Highlighted->Data.m_rBackgroundPosition.Y - pCurrentObj->Data.m_rBackgroundPosition.Y;

            nXDistOld = m_pObj_Highlighted->Data.m_rBackgroundPosition.X - ( *ppNextObjectToRight )->Data.m_rBackgroundPosition.X;
            nYDistOld = m_pObj_Highlighted->Data.m_rBackgroundPosition.Y - ( *ppNextObjectToRight )->Data.m_rBackgroundPosition.Y;

            switch( PK_Direction )
            {
            case DIRECTION_Right_CONST:
                if(
                    ( nXDistNew < 0 && Dist( nXDistNew,  nYDistNew ) < Dist( nXDistOld,  nYDistOld ) &&
                    abs( nXDistNew ) >= abs( nYDistNew ) ) ||
                    ( nXDistNew < 0 && nXDistOld == 0 )
                     )
                    *ppNextObjectToRight = pCurrentObj;
                break;

            case DIRECTION_Left_CONST:
                if(
                    ( nXDistNew > 0 && Dist( nXDistNew,  nYDistNew ) < Dist( nXDistOld,  nYDistOld ) &&
                    abs( nXDistNew ) >= abs( nYDistNew ) ) ||
                    ( nXDistNew > 0 && nXDistOld == 0 )
                     )
                    *ppNextObjectToRight = pCurrentObj;
                break;

            case DIRECTION_Up_CONST:
                if(
                    (
                    nYDistNew > 0 &&
                    Dist( nXDistNew,  nYDistNew ) < Dist( nXDistOld,  nYDistOld ) &&
                    abs( nXDistNew ) <= abs( nYDistNew )
                     )
                    ||
                    ( nYDistNew > 0 && nYDistOld == 0 )
                     )
                    *ppNextObjectToRight = pCurrentObj;

                break;

            case DIRECTION_Down_CONST:
                if(
                    ( nYDistNew < 0 && Dist( nXDistNew,  nYDistNew ) < Dist( nXDistOld,  nYDistOld ) &&
                    abs( nXDistNew ) <= abs( nYDistNew ) ) ||
                    ( nYDistNew < 0 && nYDistOld == 0 )
                     )
                    *ppNextObjectToRight = pCurrentObj;
                break;


            }
        }

        FindObjectToHighlight( ppNextObjectToRight,  pCurrentObj,  PK_Direction );
    }
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::HighlightNextObject( int PK_Direction )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	vector<DesignObj_DataGrid *> vectObj_SelectedGrids;
	vector < class DesignObj_Orbiter * > vectObjs_NeedRedraw;  // Add them outside the datagrid mutex since realrender can lock redraw, then datagrid
	size_t s;
    for( s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
    {
        DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
        if(  pDesignObj_DataGrid->IsHidden(  )  )
            continue;
        if(  pDesignObj_DataGrid->m_pDataGridTable && pDesignObj_DataGrid->m_sExtraInfo.find( 'A' )!=string::npos  )
        {
            // We've got a datagrid that is capturing scrolling.
            if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'p' )!=string::npos  ) // P means page
                CMD_Scroll_Grid( "", "", PK_Direction );
            else  // Otherwise we move 1 row at a time
            {
//int r=pDesignObj_DataGrid->m_pDataGridTable->GetRows(  );
                switch( PK_Direction )
                {
                case DIRECTION_Up_CONST:
                    if(  pDesignObj_DataGrid->m_iHighlightedRow>0 || pDesignObj_DataGrid->m_GridCurRow>0  )
                    {
                        pDesignObj_DataGrid->m_iHighlightedRow--;
                        if(  pDesignObj_DataGrid->m_iHighlightedRow<0  )
                        {
                            // Save which row in the actual table we are pointing to,  so we can point there again after doing the scroll
                            int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
                            CMD_Scroll_Grid( "", "", PK_Direction );
                            pDesignObj_DataGrid->m_iHighlightedRow=iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;
                        }
                    }
                    break;
                case DIRECTION_Down_CONST:
                    // Continue only if we're not already highlighting the last cell
					if(  pDesignObj_DataGrid->m_GridCurRow+pDesignObj_DataGrid->m_iHighlightedRow + 1 < pDesignObj_DataGrid->m_pDataGridTable->GetRows(  )  )  // Add 1 since the highlight is 0 based and get rows is not, add 2 if the last row is just a 'scroll down'
                    {
                        pDesignObj_DataGrid->m_iHighlightedRow++;
                        // See if we've scrolled past the visible end, in which case we need to page.  Subtract 1 or 2 cells for the scroll up/down cells if any
                        if(  pDesignObj_DataGrid->m_iHighlightedRow>=pDesignObj_DataGrid->m_MaxRow - (pDesignObj_DataGrid->m_dwIDownRow >= 0 ? 1 : 0) - (pDesignObj_DataGrid->m_iUpRow >= 0 ? 1 : 0) )
                        {
                            int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
                            CMD_Scroll_Grid( "", "", PK_Direction );
                            pDesignObj_DataGrid->m_iHighlightedRow=iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;
                        }
                    }
                    break;
                case DIRECTION_Left_CONST:
                    if(  pDesignObj_DataGrid->m_iHighlightedColumn>0 || pDesignObj_DataGrid->m_GridCurCol>0  )
                    {
                        pDesignObj_DataGrid->m_iHighlightedColumn--;
                        if(  pDesignObj_DataGrid->m_iHighlightedColumn<0  )
                        {
                            CMD_Scroll_Grid( "", "", PK_Direction );
                            pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_MaxCol;
                        }
                    }
                    break;
                case DIRECTION_Right_CONST:
                    // See if we scrolled past the physical end of all the columns
                    if(  pDesignObj_DataGrid->m_GridCurCol+pDesignObj_DataGrid->m_iHighlightedColumn+1 < pDesignObj_DataGrid->m_pDataGridTable->GetCols(  )  ) // Add 1 since the highlight is 0 based and get cols is not
                    {
                        pDesignObj_DataGrid->m_iHighlightedColumn++;
                        // See if we've scrolled past the visible end, in which case we need to page
                        if(  pDesignObj_DataGrid->m_iHighlightedColumn>=pDesignObj_DataGrid->m_MaxCol  )
                        {
                            CMD_Scroll_Grid( "", "", PK_Direction );
                            pDesignObj_DataGrid->m_iHighlightedColumn=0;
                        }
                    }
                    break;
                };

				// Check that pDesignObj_DataGrid->m_pDataGridTable wasn't deleted
	            if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'H' )==string::npos )
				{
					if( pDesignObj_DataGrid->m_pDataGridTable )
					{
						// We don't want the user to be able to just highlight cells without selecting, so select this cell
						DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(
							pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : 0,
							pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow : 0);
						if( pCell )
						{
							PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
							m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable] = pCell->GetValue(  );
							vm.Release();
							delete pCell;
						}
					}
					else
					{
						// Be sure nothing is selected since we moved the highlight
						PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
						m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable] = "";
					}
					// Do a select object on this grid so we reset any hidden/shown objects like buttons.  But do it outside
					// this loop since we're holding the datagrid mutex
					vectObj_SelectedGrids.push_back(pDesignObj_DataGrid);
				}
            }
			vectObjs_NeedRedraw.push_back(pDesignObj_DataGrid);
        }
    }
    dg.Release(  );
	if( vectObjs_NeedRedraw.size() )
	{
		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
		for(size_t s=0;s<vectObjs_NeedRedraw.size();++s)
	        m_vectObjs_NeedRedraw.push_back( vectObjs_NeedRedraw[s] );
		nd.Release();
	}

	for(s=0;s<vectObj_SelectedGrids.size();++s)
		SelectedObject(vectObj_SelectedGrids[s]);

    if(NULL == m_pObj_Highlighted || (m_pObj_Highlighted && (!m_pObj_Highlighted->m_bOnScreen || m_pObj_Highlighted->IsHidden())))
    {
        HighlightFirstObject();

        PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        if(m_pObj_Highlighted)
            m_vectObjs_NeedRedraw.push_back( m_pObj_Highlighted );
        nd.Release();

        return;
    }

    DesignObj_Orbiter *pDesignObj_Orbiter_OriginallyHighlight=m_pObj_Highlighted;
    DesignObj_Orbiter *pNextObject = m_pObj_Highlighted;
    switch( PK_Direction )
    {
        // Find the next ones,  skipping over any hidden ones
    case DIRECTION_Up_CONST:
        while(  pNextObject->m_pDesignObj_Orbiter_Up && pNextObject->m_pDesignObj_Orbiter_Up->IsHidden(  )  )
            pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Up;
        pNextObject = pNextObject->m_pDesignObj_Orbiter_Up;
        break;
    case DIRECTION_Down_CONST:
        while(  pNextObject->m_pDesignObj_Orbiter_Down && pNextObject->m_pDesignObj_Orbiter_Down->IsHidden(  )  )
            pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Down;
        pNextObject = pNextObject->m_pDesignObj_Orbiter_Down;
        break;
    case DIRECTION_Left_CONST:
        while(  pNextObject->m_pDesignObj_Orbiter_Left && pNextObject->m_pDesignObj_Orbiter_Left->IsHidden(  )  )
            pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Left;
        pNextObject = pNextObject->m_pDesignObj_Orbiter_Left;
        break;
    case DIRECTION_Right_CONST:
        while(  pNextObject->m_pDesignObj_Orbiter_Right && pNextObject->m_pDesignObj_Orbiter_Right->IsHidden(  )  )
            pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Right;
        pNextObject = pNextObject->m_pDesignObj_Orbiter_Right;
        break;
    }

    if(!pNextObject || pNextObject == m_pObj_Highlighted) 
    {
        if(!pNextObject)
            pNextObject = m_pObj_Highlighted;

        FindObjectToHighlight( &pNextObject, m_pScreenHistory_Current->m_pObj, PK_Direction );
    }

    m_pObj_Highlighted = pNextObject;

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
    if(pDesignObj_Orbiter_OriginallyHighlight)
        if(pDesignObj_Orbiter_OriginallyHighlight->m_vectGraphic.size())
            m_vectObjs_NeedRedraw.push_back( pDesignObj_Orbiter_OriginallyHighlight );
        else //this button is embedded in the background, we must rerender all
            m_vectObjs_NeedRedraw.push_back( m_pScreenHistory_Current->m_pObj );

    if(m_pObj_Highlighted)
        m_vectObjs_NeedRedraw.push_back( m_pObj_Highlighted );

	nd.Release();
}


/*
INITIALIZATION
*/
//-----------------------------------------------------------------------------------------------------------
void Orbiter::Initialize( GraphicType Type, int iPK_Room, int iPK_EntertainArea )
{
    if ( !m_bQuit )
	{
		m_bStartingUp=true;
		{
			NeedToRender render( this, "Initial config" );
			Message *pMessage=NULL;
			string Filename = "C" + StringUtils::itos( m_dwPK_Device ) + ".info";
			bool bFileExists;
			if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
			{
				if(  m_sLocalDirectory.length(  )==0  )
				{
#ifdef WIN32
					m_sLocalDirectory = "/pluto/orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#else
					m_sLocalDirectory = "/usr/pluto/orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#endif
					if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
					{
						m_sLocalDirectory = "orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
						if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
						{
							m_sLocalDirectory = "../orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
							if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
							{
								m_sLocalDirectory = "C" + StringUtils::itos( m_dwPK_Device ) + "/";
								bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename );
							}
						}
					}
				}
			}
			if(  bFileExists  )
			{
				size_t size;
				char *buffer = FileUtils::ReadFileIntoBuffer( m_sLocalDirectory + "/" + Filename, size );

				if(  !buffer || !SerializeRead( ( long ) size, buffer, ( void * ) this ) || !ParseConfigurationData( Type ) )
				{
					delete[] buffer;
					g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to read Orbiter data from file: %s/%s", m_sLocalDirectory.c_str(  ), Filename.c_str(  ) );
                    m_bQuit = true;
					m_bReload = true;
					exit( 1 );
				}
				delete [] buffer;
			}
			else
			{
				if(  m_sLocalDirectory.length(  )  )
				{
					g_pPlutoLogger->Write( LV_CRITICAL, "Directory: %s doesn't contain the orbiter's .info file.  Requesting files from the server.", m_sLocalDirectory.c_str(  ) );
					m_sLocalDirectory="";  // It's not valid
				}

				// Request our config info
				char *pConfigFile=NULL;
				int iSizeConfigFile=0;
g_PlutoProfiler->Start("getfile");
				// We can't send it to the General Info virtual device since we won't have that until we get our config data
				DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false,  BL_SameHouse, "orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/" + Filename + ".lzo",
					&pConfigFile, &iSizeConfigFile );
				SendCommand( CMD_Request_File_Cat );
g_PlutoProfiler->Stop("getfile");

g_PlutoProfiler->Start("decompressfile");
				if ( iSizeConfigFile )  // Decompress it
				{
					int UncompressedLength = *((int *)pConfigFile);
					char *UncompressedData = new char[UncompressedLength];
					lzo_uint new_len;
					lzo1x_decompress((lzo_byte *)pConfigFile+4,iSizeConfigFile - 4,(lzo_byte *)UncompressedData,&new_len,NULL);
					delete[] pConfigFile;
					pConfigFile = UncompressedData;
					iSizeConfigFile=UncompressedLength;
				}
g_PlutoProfiler->Stop("decompressfile");

				if ( !iSizeConfigFile )
				{
					g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to get Orbiter data" );
					Sleep( 5000 );
					m_bReload = true;
					return;
				}
g_PlutoProfiler->Start("serialize");

				// The framework needs a pointer to us when it's creating Orbiter objects,  pass in this as a void *
				if(  !SerializeRead( iSizeConfigFile, pConfigFile, ( void * ) this ) )
				{
					delete pMessage;
					g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to parse Orbiter data" );
					Sleep( 5000 );  // Sleep a bit before re-attempting
					m_bReload = true;
					return;
				}
g_PlutoProfiler->Stop("serialize");
g_PlutoProfiler->Start("parse");
ParseConfigurationData( Type );
g_PlutoProfiler->Stop("parse");
				delete pConfigFile;
			}

			if(  !m_bLocalMode  )
			{
				PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, GetEvents(  )->m_pClientSocket->m_SocketMutex );  // Don't log anything but failures
				string sResult = GetEvents(  )->SendReceiveString( "TIME" );

				if(  sResult.length(  )>5  )
				{
					char *Buffer = new char[sResult.length(  )+1];
					strcpy( Buffer,  sResult.c_str(  ) );
					SetTime( Buffer );
					delete [] Buffer;
				}
				sSM.Release(  );
			}

			if(NULL != m_pCacheImageManager && !m_pCacheImageManager->VerifyCache(StringUtils::ltos((long)m_tGenerationTime)))
			{
				delete m_pCacheImageManager;
				m_pCacheImageManager = NULL;
			}

			//      PreloadObjects(  );

			/*
			if(  !m_bLocalMode  )
			Event_Orbiter_Started( m_sMainMenu );
			*/

			m_pLocationInfo_Initial=NULL;
			for( size_t s=0;s<m_dequeLocation.size();++s)
			{
				class LocationInfo *pLocationInfo = m_dequeLocation[s];
#ifdef DEBUG
				g_pPlutoLogger->Write(LV_STATUS,"Checing %d/%d room: %d ea %d against %d %d",
					(int) s,pLocationInfo->iLocation,pLocationInfo->PK_Room,pLocationInfo->PK_EntertainArea,iPK_Room,iPK_EntertainArea);
#endif
				// If a room/ent area is passed in use that location instead of m_iLocation_Initial
				if( (pLocationInfo->iLocation==m_iLocation_Initial && !m_pLocationInfo_Initial) ||
					(pLocationInfo->PK_Room==iPK_Room && pLocationInfo->PK_EntertainArea==0) ||
					(iPK_EntertainArea && pLocationInfo->PK_EntertainArea==iPK_EntertainArea) )
				{
#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS,"using location in size: %d",(int) m_dequeLocation.size());
#endif
					m_pLocationInfo_Initial = pLocationInfo;
				}
			}

			if(  !m_pLocationInfo_Initial  )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "No initial Location" );
				exit( 1 );
			}
			CMD_Set_Current_Location(m_pLocationInfo_Initial->iLocation);

			DesignObj_OrbiterMap::iterator iHao=m_ScreenMap.begin(  );
			if ( iHao==m_ScreenMap.end(  ) )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "No screens found." );
				exit( 1 );
			}
			else
			{
				if( m_sInitialScreen.size() )
					GotoScreen( m_sInitialScreen );
				else
					GotoScreen( m_sMainMenu );
			}

			m_pLocationInfo = m_pLocationInfo_Initial;
			m_dwPK_Users = m_dwPK_Users_Default;

			char *pData=NULL; int iSize=0;
			DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "1",
				m_dwPK_Users,StringUtils::itos(m_pLocationInfo->PK_EntertainArea),m_pLocationInfo->PK_Room, &pData, &iSize);
			SendCommand( CMD_Orbiter_Registered );
			m_pOrbiterFileBrowser_Collection = new OrbiterFileBrowser_Collection;
			m_pOrbiterFileBrowser_Collection->SerializeRead(iSize,pData);
			delete pData;
		}

		if( !m_pScreenHistory_Current )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "No initial screen" );
			exit( 1 );
		}

        AdjustWindowSize(m_pScreenHistory_Current->m_pObj->Data.m_rPosition.Width, 
            m_pScreenHistory_Current->m_pObj->Data.m_rPosition.Height);

		DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
		for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
		{
			DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
			if(  pObj->m_Action_StartupList.size(  )>0  )
				ExecuteCommandsInList( &pObj->m_Action_StartupList, pObj );
		}
		m_bStartingUp=false;
	}
}

void Orbiter::InitializeGrid( DesignObj_DataGrid *pObj )
{
g_PlutoProfiler->Start("init grid");
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    /* todo 2.0
    bool bEPG = ( GridID.substr( 0, 3 )=="EPG" || GridID.substr( 0, 5 )=="CURSH" );
    */
    // Initially the first row and column will be highlighted
    string::size_type posH;
    if(  (posH=pObj->m_sExtraInfo.find( 'H' ))!=string::npos && pObj->sSelVariable.length()==0 )
    {
        pObj->m_iHighlightedRow = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='C') ? 0 : -1;
        pObj->m_iHighlightedColumn = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='R') ? 0 : -1;
    }
    else
    {
        pObj->m_iHighlightedRow=-1;
        pObj->m_iHighlightedColumn=-1;
    }

    if ( pObj->m_pDataGridTable )
    {
        delete pObj->m_pDataGridTable;
        pObj->m_pDataGridTable = NULL;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s deleting m_pDataGridTable 1",pObj->m_ObjectID.c_str());
#endif
    }

    if(  !pObj->m_bPersistXY || pObj->m_GridCurCol==-1 || pObj->m_GridCurRow==-1  )
    {
        pObj->m_GridCurCol = pObj->m_iInitialColNum;
        pObj->m_GridCurRow = pObj->m_iInitialRowNum;
    }

    ++m_dwIDataGridRequestCounter;
#ifdef DEBUG
    g_pPlutoLogger->Write( LV_CONTROLLER, "Initializing grid: %d ( %s ) dev %d options: %s ( # %d )", pObj->m_iPK_Datagrid, pObj->m_sGridID.c_str(  ), m_dwPK_Device, pObj->m_sOptions.c_str(  ), m_dwIDataGridRequestCounter );
#endif

    // Don't populate if we're not passing in anything at this point

    if(  pObj->m_iPK_Datagrid  )
    {
        bool bResponse;
        int iPK_Variable=0;
        string sValue_To_Assign;
		WaitForMessageQueue();  // There might still be some messages in the queue which will affect this grid
        DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID,
            pObj->m_iPK_Datagrid, SubstituteVariables( pObj->m_sOptions, pObj, 0, 0 ), &iPK_Variable, &sValue_To_Assign, &bResponse, &pObj->m_iPopulatedWidth, &pObj->m_iPopulatedHeight  );
        if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
            g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid: %d failed", pObj->m_iPK_Datagrid );
        else if(  iPK_Variable  )
		{
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			m_mapVariable[iPK_Variable] = sValue_To_Assign;
		}
    }
    /* todo 2.0
    if ( !sSelVariable.empty(  ) && !bEPG )
    {
    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    m_mapVariable[atoi( sSelVariable.c_str(  ) )] = "";
    vm.Release(  );
    }
    */
    /* todo 2.0
    if(  GridID.substr( 0, 4 )=="EPG_"  )
    {
    time_t EPGStartingTime = atoi( m_mapVariable[VARIABLE_EPG_START_TIME_CONST].c_str(  ) );
    if(  EPGStartingTime>0  )
    {
    int CurrentMinutes = int( time( NULL ) - EPGStartingTime ) / 60;

    //      time_t t_t = time( NULL );
    //      struct tm *t = localtime( &t_t );
    pObj->m_GridCurCol = CurrentMinutes/MINS_PER_COLUMN-NUM_COLUMNSPAN_HEADER;
    PositionEPGMarker( pObj );
    }
    g_pPlutoLogger->Write( LV_WARNING, "Marker set: start time: %d column: %d", ( int ) EPGStartingTime, pObj->m_GridCurCol );
    }
    */
g_PlutoProfiler->Stop("init grid");
}
//------------------------------------------------------------------------
bool Orbiter::ParseConfigurationData( GraphicType Type )
{
    m_LastActivityTime = time( NULL );

    for( ScreenMap::iterator it=m_ScreenMap.begin(  );it!=m_ScreenMap.end(  );++it )
    {
        DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) ( *it ).second;
        ParseObject( pObj, pObj, pObj, Type, 0 );
    }


    for( DesignObj_OrbiterMap::iterator oi = m_mapObj_All.begin(  ); oi != m_mapObj_All.end(  ); ++oi  )
    {
        DesignObj_Orbiter *pObj = ( *oi ).second;

        // Now match up all the up/down/left/right
        if(  pObj->Data.m_PK_DesignObj_Up  )
            pObj->m_pDesignObj_Orbiter_Up = FindObject( StringUtils::itos( pObj->Data.m_PK_DesignObj_Up ), pObj );
        if(  pObj->Data.m_PK_DesignObj_Down  )
            pObj->m_pDesignObj_Orbiter_Down = FindObject( StringUtils::itos( pObj->Data.m_PK_DesignObj_Down ), pObj );
        if(  pObj->Data.m_PK_DesignObj_Left  )
            pObj->m_pDesignObj_Orbiter_Left = FindObject( StringUtils::itos( pObj->Data.m_PK_DesignObj_Left ), pObj );
        if(  pObj->Data.m_PK_DesignObj_Right  )
            pObj->m_pDesignObj_Orbiter_Right = FindObject( StringUtils::itos( pObj->Data.m_PK_DesignObj_Right ), pObj );
        if(  pObj->m_sPK_DesignObj_TiedTo.length(  )  )
        {
            // This may be in the form object:column:row
            string::size_type pos=0;
            string Object = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
            string Column = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
            string Row = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
            pObj->m_pDesignObj_Orbiter_TiedTo = FindObject( Object, pObj );
            pObj->m_iColumnTiedTo = Column=="" ? -1 : atoi( Column.c_str(  ) );
            pObj->m_iRowTiedTo = Row=="" ? -1 : atoi( Row.c_str(  ) );
        }
    }

    m_pDesignObj_Orbiter_MainMenu=m_ScreenMap_Find( m_sMainMenu );
    m_pDesignObj_Orbiter_SleepingMenu=m_ScreenMap_Find( m_sSleepingMenu );
    m_pDesignObj_Orbiter_ScreenSaveMenu=m_ScreenMap_Find( m_sScreenSaveMenu );

    if(  !m_pDesignObj_Orbiter_MainMenu  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find main menu: %s", m_sMainMenu.c_str(  ) );
        throw "Cannot find main menu";
    }

    if(  m_bLocalMode  )
        return true;

/*
    // Send through event manager so it updates its value.  Event manager will pick a default user if this doesn't exist
    Message *pMessage = new Message(m_dwPK_Device,m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
    COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_PK_USERS_CONST).c_str(),COMMANDPARAMETER_Value_To_Assign_CONST,Data_Get_PK_Users().c_str());
    GetEvents()->SendMessage(pMessage);

    m_mapVariable[VARIABLE_MEDIA_CENTRAL_ENTGROUP_CONST]=m_mapVariable[VARIABLE_PK_ENTERTAIN_GROUP_CONST];

    if( m_mapVariable[VARIABLE_EPG_GUIDE_VIEW_CONST]=="" )
    {
    Message *pMessage = new Message(m_dwPK_Device,m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
    COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_EPG_GUIDE_VIEW_CONST).c_str(),
    COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(DESIGNOBJ_MNUTVEPG1_CONST).c_str());
    GetEvents()->SendMessage(pMessage);
    }
*/
	string sResult;
    DCE::CMD_Get_Floorplan_Layout CMD_Get_Floorplan_Layout(m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, &sResult);
    SendCommand(CMD_Get_Floorplan_Layout);

      string::size_type pos=0;
    int NumPages = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Get Floorplan layout found %d pages", NumPages);
#endif
    for(int PageCount=0;PageCount<NumPages;++PageCount)
    {
        int PageNum = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());

        FloorplanObjectVectorMap *pFloorplanObjectVectorMap = new FloorplanObjectVectorMap();
        m_mapFloorplanObjectVector[PageNum]=pFloorplanObjectVectorMap ;

        int NumTypes = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Page %d had %d types", PageNum, NumTypes);
#endif
        for(int TypeCount=0;TypeCount<NumTypes;++TypeCount)
        {
            int TypeNum = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());

            FloorplanObjectVector *fpObjVector = new FloorplanObjectVector();
            (*pFloorplanObjectVectorMap)[TypeNum] = fpObjVector;

			string sNumObjects = StringUtils::Tokenize(sResult, "|", pos);
            int NumObjects = atoi(sNumObjects.c_str());
            for(int ObjectCount=0;ObjectCount<NumObjects;++ObjectCount)
            {
                FloorplanObject *fpObj = new FloorplanObject();

                fpObj->ObjectTypeDescription=StringUtils::Tokenize(sResult, "|", pos);
                fpObj->FillX=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
                fpObj->FillY=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
                fpObj->ObjectID=StringUtils::Tokenize(sResult, "|", pos);
                fpObj->Page=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
                fpObj->PK_Device=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
                fpObj->m_pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(fpObj->PK_Device);
                fpObj->DeviceDescription=StringUtils::Tokenize(sResult, "|", pos);
                fpObj->Type=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
                fpObj->pObj = FindObject(fpObj->ObjectID);
				if( !fpObj->pObj )
					g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find floorplan object: %s",fpObj->ObjectID.c_str());

#ifdef DEBUG
				g_pPlutoLogger->Write(LV_STATUS,"Adding device %d %s to floorplan", fpObj->PK_Device, fpObj->DeviceDescription.c_str());
#endif

				fpObjVector->push_back(fpObj);
                if( fpObj->pObj )
                {
                    fpObj->pObj->m_pFloorplanObject=fpObj;
                    // Be sure there is a 'requires special handling' action on this object
                    if( fpObj->pObj->m_ZoneList.size()==0 )
                    {
                        DesignObjZone *pDesignObjZone = new DesignObjZone();
                        pDesignObjZone->m_Rect.Width=pDesignObjZone->m_Rect.Height=0;
                        DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
                        pDesignObjCommand->Data.m_PK_Command=COMMAND_Requires_Special_Handling_CONST;
                        pDesignObjCommand->Data.m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
                        pDesignObjZone->m_Commands.push_back(pDesignObjCommand);
                        fpObj->pObj->m_ZoneList.push_back(pDesignObjZone);
                    }
                }
            }
        }
    }

	pos=0;
	string sKeymapping=DATA_Get_Hard_Keys_mapping();
	while(pos<DATA_Get_Hard_Keys_mapping().size())
	{
		string sToken=StringUtils::Tokenize(sKeymapping,"\n",pos);
		string::size_type pos2=0;
		int iKey = atoi( StringUtils::Tokenize(sToken,"\t",pos2).c_str() );
		if( !iKey )
			continue;
		Message *pMessage = new Message(StringUtils::Tokenize(sToken,"\t",pos2));
		if( pMessage->m_dwPK_Device_To<0 )
			pMessage->m_dwPK_Device_To = TranslateVirtualDevice(pMessage->m_dwPK_Device_To);
		m_mapHardKeys[iKey] = pMessage;
	}

    return true;
}
//------------------------------------------------------------------------
void Orbiter::ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type,  int Lev )
{
	ShowProgress();
    if(  pObj->m_ObjectID.find( "1255.0.0" )!=string::npos )// || pObj->m_ObjectID.find( "2071" )!=string::npos )
    {
        int k=2;
    }
    if(  pObj->m_ObjectID.find( "1255.0.0.1687" )!=string::npos )// || pObj->m_ObjectID.find( "2071" )!=string::npos )
    {
        int k=2;
    }

	for(size_t s=0;s<pObj->m_vectDesignObjText.size();++s)
		pObj->m_vectDesignObjText[s]->m_pObject = pObj;

	/*


    if(  pObj->m_ObjectID.find( "3290." )!=string::npos  )
        //if(  ocDesignObj->m_drDesignObj->PK_DesignObj_get(  )==2790  )
    {
        int k=2;
        //  DesignObjZone *pz = pObj->m_ZoneList.front(  );
        //  DesignObjCommand *pc = pz->m_Commands.front(  );
        //  string screen = pc->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST];

    }
*/
    m_mapObj_All[pObj->m_ObjectID] = pObj;  // Do this earlier since the bind to action may require looking this up.
	pObj->m_pObj_Screen=pObj_Screen;

    if(  pObj->Data.m_bHideByDefault  )
        pObj->m_bHidden = true;

    enum eGraphicManagement eMgmt = GR_DISCARDONCHANGE;
    if(  pObj->m_sBackgroundFile.length(  )>0  )
        CreateVectorGraphic(pObj->m_vectGraphic, Type,  pObj->m_sBackgroundFile,  eMgmt,  this );
    if(  pObj->m_sHighlightGraphicFilename.length(  )>0  )
        CreateVectorGraphic(pObj->m_vectHighlightedGraphic, Type,  pObj->m_sHighlightGraphicFilename,  eMgmt,  this );
    if(  pObj->m_sSelectedFile.length(  )>0  )
        CreateVectorGraphic(pObj->m_vectSelectedGraphic, Type,  pObj->m_sSelectedFile,  eMgmt,  this );
    for( size_t salt=0;salt<pObj->m_vectAltGraphicFilename.size(  );++salt )
    {
		VectorPlutoGraphic vectPlutoGraphic;
		CreateVectorGraphic(vectPlutoGraphic, Type,  pObj->m_vectAltGraphicFilename[salt],  eMgmt,  this);
		pObj->m_vectAltGraphics.push_back( vectPlutoGraphic );
    }

    if(  pObj->Data.m_bTabStop  )
        pObj_Screen->m_vectObj_TabStops.push_back( pObj );

    /*
    if(  pAction->m_PK_Command==COMMAND_USE_DEVICE_CONST  )
    {
    string ID = pAction->m_ParameterList[COMMANDPARAMETER_ID_CONST];
    string Device = pAction->m_ParameterList[COMMANDPARAMETER_PK_Device_CONST];
    GetEvents(  )->SendMessage( new Message( m_dwPK_Device,  DEVICEID_DATAGRID,  PRIORITY_NORMAL,
    MESSAGETYPE_COMMAND,  COMMAND_BIND_TO_DEVICE_STATUS_CONST,  4,
    C_COMMANDPARAMETER_Orbiter_ID_CONST,  StringUtils::itos( m_dwPK_Device ).c_str(  ),
    COMMANDPARAMETER_PK_DesignObj_CONST,  pObj->m_ObjectID.c_str(  ),
    COMMANDPARAMETER_ID_CONST,  ID.c_str(  ),
    COMMANDPARAMETER_PK_Device_CONST,  Device.c_str(  ) ) );
    }
    */

    // Child objects have the format screen.version.page.objectid.x where x is an arbitrary increment to insure uniqueness
    // Some commands need to send messages to an object not knowing the value of x.  So,  we create another map without the suffix
    int CountDots=0;
    int DotPos=0, LastDotPos=-1, PriorToLastDotPos=-1;
    while(  ( DotPos=( int ) pObj->m_ObjectID.find( ".", DotPos+1 ) )>0  )
    {
        PriorToLastDotPos=LastDotPos;
        LastDotPos=DotPos;
        CountDots++;
    }

    if(  CountDots==4  )
    {
        string IDWithoutSuffix = pObj->m_ObjectID.substr( 0, LastDotPos );
        DesignObj_DataList *pObjectList;
        map<string, DesignObj_DataList *>::iterator it = m_mapObj_AllNoSuffix.find( IDWithoutSuffix );
        if(  it==m_mapObj_AllNoSuffix.end(  )  )
        {
            pObjectList = new DesignObj_DataList(  );
            m_mapObj_AllNoSuffix[IDWithoutSuffix] = pObjectList;
        }
        else
            pObjectList = ( *it ).second;
        pObjectList->push_back( pObj );
        string x=pObj->m_ObjectID.substr( PriorToLastDotPos+1 );
        pObj->Data.m_iBaseObjectID = atoi( x.c_str(  ) );
    }
    else
    {
        pObj->Data.m_iBaseObjectID = atoi( pObj->m_ObjectID.c_str(  ) );
    }

	// On any screen all child objects should inherit the screen's priority so the whole screen is cached
	pObj->Data.m_Priority = pObj_Screen->Data.m_Priority;

	if ( pObj->Data.m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
    {
        pObj->m_bContainsDataGrid=true;
        DesignObj_DataGrid *pObj_Datagrid = ( DesignObj_DataGrid * ) pObj;
        // Fix up the parameters
        pObj_Datagrid->m_sGridID = SubstituteVariables( pObj->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ), pObj, 0, 0 );
        pObj_Datagrid->m_sExtraInfo = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Extra_Info_CONST];
        pObj_Datagrid->m_FixedRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Row_Height_CONST ).c_str(  ) );
        pObj_Datagrid->m_FixedColumnWidth =  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Column_Width_CONST ).c_str(  ) );
        pObj_Datagrid->m_RowSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Row_Spacing_CONST ).c_str(  ) );
        pObj_Datagrid->m_ColumnSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Column_Spacing_CONST ).c_str(  ) );
        pObj_Datagrid->m_FirstRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Row_Height_CONST ).c_str(  ) );
        pObj_Datagrid->m_FirstColumnWidth =  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Column_Width_CONST ).c_str(  ) );
        pObj_Datagrid->m_bKeepRowHeader = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Keep_Row_Header_CONST )=="1";
        pObj_Datagrid->m_bKeepColHeader = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Keep_Column_Header_CONST )=="1";
        pObj_Datagrid->m_bPersistXY = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Persist_XY_Position_CONST )=="1";
        pObj_Datagrid->m_iInitialRowNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Row_Number_CONST ).c_str(  ) );
        pObj_Datagrid->m_iInitialColNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Column_Numb_CONST ).c_str(  ) );
        pObj_Datagrid->m_MaxCol = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Columns_CONST ).c_str(  ) );
        pObj_Datagrid->m_sSeek = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Seek_Value_CONST );
        pObj_Datagrid->m_iSeekColumn = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Seek_Column_CONST ).c_str(  ) );
        if ( pObj_Datagrid->m_MaxCol == 0 )
        {
            if(  pObj_Datagrid->m_FixedColumnWidth==0  )
                pObj_Datagrid->m_MaxCol=1;
            else if( m_iRotation==90 || m_iRotation==270 )
                pObj_Datagrid->m_MaxCol =1+ ( pObj_Datagrid->Data.m_rPosition.Height - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
			else
                pObj_Datagrid->m_MaxCol =1+ ( pObj_Datagrid->Data.m_rPosition.Width - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
        }
        else if ( pObj_Datagrid->m_FixedColumnWidth == 0 ) // Do we know the number of columns but not their size?
        {
            if( m_iRotation==90 || m_iRotation==270 )
				pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->Data.m_rPosition.Height / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
			else
				pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->Data.m_rPosition.Width / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
        }
        pObj_Datagrid->m_MaxRow = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Rows_CONST ).c_str(  ) );
        if ( pObj_Datagrid->m_MaxRow == 0 )
        {
            if(  pObj_Datagrid->m_FixedRowHeight==0  )
                pObj_Datagrid->m_MaxRow=1;
            else if( m_iRotation==90 || m_iRotation==270 )
                pObj_Datagrid->m_MaxRow = 1+ ( pObj_Datagrid->Data.m_rPosition.Width - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
			else
                pObj_Datagrid->m_MaxRow = 1+ ( pObj_Datagrid->Data.m_rPosition.Height - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
        }
        else if ( pObj_Datagrid->m_FixedRowHeight == 0 ) // Do we know the number of columns but not their size?
        {
            if( m_iRotation==90 || m_iRotation==270 )
				pObj_Datagrid->m_FixedRowHeight = ( pObj_Datagrid->Data.m_rPosition.Width / pObj_Datagrid->m_MaxRow ) - ( ( pObj_Datagrid->m_MaxRow-1 ) * pObj_Datagrid->m_RowSpacing );
			else
				pObj_Datagrid->m_FixedRowHeight = ( pObj_Datagrid->Data.m_rPosition.Height / pObj_Datagrid->m_MaxRow ) - ( ( pObj_Datagrid->m_MaxRow-1 ) * pObj_Datagrid->m_RowSpacing );
        }
        if(  ( pObj_Datagrid->m_FixedColumnWidth == 0 && pObj_Datagrid->m_MaxCol == 0 ) || ( pObj_Datagrid->m_FixedRowHeight == 0 && pObj_Datagrid->m_MaxRow == 0 )  )
        {
            g_pPlutoLogger->Write( LV_CRITICAL,  "Neither the column/row size,  nor the the column/row count are specified in the parameters on grid %s",  pObj_Datagrid->m_ObjectID.c_str(  ) );
        }

        // For purposes of calculation,  take the bigger of the two here when not fixed
        if(  !pObj_Datagrid->m_bKeepRowHeader  )
        {
            pObj_Datagrid->m_FirstRowHeight = max( pObj_Datagrid->m_FirstRowHeight,  pObj_Datagrid->m_FixedRowHeight );
        }
        else
        {
            if ( pObj_Datagrid->m_FirstRowHeight == 0 )
                pObj_Datagrid->m_FirstRowHeight = pObj_Datagrid->m_FixedRowHeight;
        }
        if (  !pObj_Datagrid->m_FirstColumnWidth  )
        {
            pObj_Datagrid->m_FirstColumnWidth = max( pObj_Datagrid->m_FirstColumnWidth,  pObj_Datagrid->m_FixedColumnWidth );
        }
        else
        {
            if ( pObj_Datagrid->m_FirstColumnWidth == 0 )
                pObj_Datagrid->m_FirstColumnWidth = pObj_Datagrid->m_FixedColumnWidth;
        }

        pObj_Datagrid->m_iPK_Variable = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST ).c_str(  ) );
        pObj_Datagrid->m_sOptions = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Options_CONST];  // No substitution since this usually has tokens in it to parse at runtime
        pObj_Datagrid->m_iPK_Datagrid = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Datagrid_CONST ).c_str(  ) );
        pObj_Datagrid->m_bDontShowSelection = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST )=="-1";
        pObj_Datagrid->m_bIsMultiSelect = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST )=="1";

        string::size_type pos = 0;
        /* todo 2.0 pObj_Datagrid->BorderWidth = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ", ",  pos ).c_str(  ) );
        pObj_Datagrid->BorderColor = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ";",  pos ).c_str(  ) );
        if ( pos<pObj_Datagrid->BorderStyle.length(  ) )
        pObj_Datagrid->BorderColor2 = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ";",  pos ).c_str(  ) );
        else
        pObj_Datagrid->BorderColor2 = pObj_Datagrid->BorderColor;
        */
        pObj_Datagrid->sSelVariable = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST );

        pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_CONST ).c_str(  ) )  );
        if(  !pObj_Datagrid->m_pTextStyle  )
            pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find(  1  );
        pObj_Datagrid->m_pTextStyle_FirstCol = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstColumn_CONST ).c_str(  ) )  );
        pObj_Datagrid->m_pTextStyle_FirstRow = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstRow_CONST ).c_str(  ) )  );
        pObj_Datagrid->m_pTextStyle_Selected = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Selected_CONST ).c_str(  ) )  );
        pObj_Datagrid->m_pTextStyle_Highlighted = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Highlighted_CONST ).c_str(  ) )  );

        if(  !pObj_Datagrid->m_pTextStyle_FirstCol  )
            pObj_Datagrid->m_pTextStyle_FirstCol = pObj_Datagrid->m_pTextStyle;
        if(  !pObj_Datagrid->m_pTextStyle_FirstRow  )
            pObj_Datagrid->m_pTextStyle_FirstRow = pObj_Datagrid->m_pTextStyle;
        if(  !pObj_Datagrid->m_pTextStyle_Selected  )
            pObj_Datagrid->m_pTextStyle_Selected = pObj_Datagrid->m_pTextStyle;

        pos=0;
        string AltStyles = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Alt_CONST );
        int Counter=0;
        while( true )
        {
            string Style = StringUtils::Tokenize( AltStyles, ", ", pos );
            if(  Style.length(  )==0  )
                break;
            TextStyle *pTextStyle = m_mapTextStyle_Find(  atoi( Style.c_str(  ) )  );
            if(  pTextStyle  )
            {
				while(int(pObj_Datagrid->m_vectTextStyle_Alt.size()) < Counter)
                    pObj_Datagrid->m_vectTextStyle_Alt.push_back(NULL);
                pObj_Datagrid->m_vectTextStyle_Alt.push_back(pTextStyle);
                //              pObj_Datagrid->m_vectTextStyle_Alt[Counter] = pTextStyle;
            }
            Counter++;
        }
        m_mapObjs_AllGrids[pObj_Datagrid->m_sGridID] = pObj_Datagrid;
    }

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        DesignObj_Orbiter *p = ( DesignObj_Orbiter * )( *iHao );
        ParseObject( p, pObj_Screen, pObj_Parent, Type, Lev );
    }
    if(  pObj->m_bContainsDataGrid==true && pObj_Parent  )
        pObj_Parent->m_bContainsDataGrid=true;

    /*
    Floorplan
    else if(  pObj->Data.m_iBaseObjectID==DESIGNOBJ_GRPHOUSEINDICATORS_CONST  )
    {
    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
    DesignObj_Orbiter *p = ( *iHao );
    if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAWINDOW0_CONST  )
    m_FloorplanObjects.m_Windows->m_objBypassed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAWINDOW1_CONST  )
    m_FloorplanObjects.m_Windows->m_objArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAWINDOW2_CONST  )
    m_FloorplanObjects.m_Windows->m_objNotArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAWINDOW3_CONST  )
    m_FloorplanObjects.m_Windows->m_objTriggered = p;

    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRADOOR0_CONST  )
    m_FloorplanObjects.m_Doors->m_objBypassed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRADOOR1_CONST  )
    m_FloorplanObjects.m_Doors->m_objArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRADOOR2_CONST  )
    m_FloorplanObjects.m_Doors->m_objNotArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRADOOR3_CONST  )
    m_FloorplanObjects.m_Doors->m_objTriggered = p;

    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAMOTION0_CONST  )
    m_FloorplanObjects.m_Motions->m_objBypassed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAMOTION1_CONST  )
    m_FloorplanObjects.m_Motions->m_objArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAMOTION2_CONST  )
    m_FloorplanObjects.m_Motions->m_objNotArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRAMOTION3_CONST  )
    m_FloorplanObjects.m_Motions->m_objTriggered = p;

    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRACAMERA0_CONST  )
    m_FloorplanObjects.m_Cameras->m_objBypassed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRACAMERA1_CONST  )
    m_FloorplanObjects.m_Cameras->m_objArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRACAMERA2_CONST  )
    m_FloorplanObjects.m_Cameras->m_objNotArmed = p;
    else if(  p->Data.m_iBaseObjectID==DESIGNOBJ_GRACAMERA3_CONST  )
    m_FloorplanObjects.m_Cameras->m_objTriggered = p;
    }
    }
    */
}
//------------------------------------------------------------------------
bool Orbiter::RenderDesktop( class DesignObj_Orbiter *pObj,  PlutoRectangle rectTotal, PlutoPoint point )
{
#ifdef DEBUG
    g_pPlutoLogger->Write( LV_STATUS, "Render desktop orb" );
#endif
    SolidRectangle( point.X + pObj->Data.m_rPosition.X, point.Y + pObj->Data.m_rPosition.Y, pObj->Data.m_rPosition.Width, pObj->Data.m_rPosition.Height, PlutoColor( 0, 0, 255 ) );
    return true;
}

/*
ACCEPT OUTSIDE INPUT
*/

void Orbiter::QueueEventForProcessing( void *eventData )
{
	Orbiter::Event *pEvent = (Orbiter::Event*)eventData;

	PreprocessEvent(*pEvent);
	ProcessEvent(*pEvent);
}

bool Orbiter::PreprocessEvent(Orbiter::Event &event)
{
	// nothing. This is here because i need a prepropcessing phase in key translation ( the XRecord callback doesn't allow XLib calls inside it so i need to make in another thread );
	return true;
}

bool Orbiter::ProcessEvent( Orbiter::Event &event )
{
#ifdef DEBUG
	// a switch would be good but kdevelop somehow doesn't like the syntax and mekes it red :-(
 	if ( event.type != Orbiter::Event::QUIT && event.type != Orbiter::Event::NOT_PROCESSED )
 	{
		g_pPlutoLogger->Write(LV_STATUS, "Processing event type: %d", event.type);

		if ( event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::BUTTON_UP )
			g_pPlutoLogger->Write(LV_WARNING, "Button %s: button ID: %d", (event.type == Orbiter::Event::BUTTON_DOWN) ? "down" : "up", event.data.button.m_iPK_Button );

		if ( event.type == Orbiter::Event::REGION_DOWN || event.type == Orbiter::Event::REGION_UP )
			g_pPlutoLogger->Write(LV_WARNING, "Region %s: position: [%d, %d]", (event.type == Orbiter::Event::REGION_DOWN) ? "down" : "up", event.data.region.m_iX, event.data.region.m_iY);
 	}
#endif
	if ( event.type == Orbiter::Event::BUTTON_DOWN )
		return ButtonDown(event.data.button.m_iPK_Button);

	if ( event.type == Orbiter::Event::BUTTON_UP )
		return ButtonUp(event.data.button.m_iPK_Button);

	if ( event.type == Orbiter::Event::REGION_DOWN )
		return RegionDown(event.data.region.m_iX, event.data.region.m_iY);

	if ( event.type == Orbiter::Event::REGION_UP )
		return  RegionUp(event.data.region.m_iX, event.data.region.m_iY); // Shouldn't this be like the above?

	return false;
}

/*virtual*/ bool Orbiter::HandleButtonEvent(int PK_Button)
{
    if( !PK_Button || !m_pScreenHistory_Current )
        return false;

    NeedToRender render( this, "Button Down" );  // Redraw anything that was changed by this command
    class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

    bool bHandled=false;
    if( !GotActivity(  ) )
        return true;

    if(  PK_Button == BUTTON_Enter_CONST  )
    {
		// See if we're capturing arrows for a grid.  That takes precedence
	    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		vector<DesignObj_DataGrid *> vectObj_SelectedGrids;
		size_t s;
		for( s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
		{
			DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
			if(  pDesignObj_DataGrid->IsHidden(  )  )
				continue;
			if(  pDesignObj_DataGrid->m_pDataGridTable && pDesignObj_DataGrid->m_sExtraInfo.find( 'A' )!=string::npos  )
			{
//                    if(  pDesignObj_DataGrid->m_iHighlightedRow>0 || pDesignObj_DataGrid->m_GridCurRow>0  )
  //                  if(  pDesignObj_DataGrid->m_iHighlightedColumn>0 || pDesignObj_DataGrid->m_GridCurColumn>0  )
				DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(
                    pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol,
                    pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow : 0);

                if( pCell )
                {
                    SelectedGrid( pDesignObj_DataGrid,  pCell );
                    SelectedObject( pDesignObj_DataGrid, -1, -1 ); //also select the grid as a object
                }
				return true;
			}
		}
		
		if(  m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden(  )  )
        {
            SelectedObject( m_pObj_Highlighted );
            bHandled=true;
        }
        else
        {
            vector<DesignObj_DataGrid *> vectSelectedGrids; // We don't want to fire the selected objects while holding the datagrid mutex
            PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
            size_t s;
            for( s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
            {
                DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
                // We're using the arrows, so use this to activate the cell
                if(  !pDesignObj_DataGrid->IsHidden(  ) && pDesignObj_DataGrid->m_pDataGridTable && pDesignObj_DataGrid->m_sExtraInfo.find( 'A' )!=string::npos )
                {
                    // We've got a datagrid that allows the user to highlight cells without selecting them.  Now that enter was pressed it needs to select the cell
#ifdef DEBUG
                    g_pPlutoLogger->Write(LV_STATUS, "Enter key press. Status: iHighlightedColumn %d, GridCurCol %d, iHighlightedRow %d, GridCurRow %d",
                        pDesignObj_DataGrid->m_iHighlightedColumn, pDesignObj_DataGrid->m_GridCurCol,
                        pDesignObj_DataGrid->m_iHighlightedRow, pDesignObj_DataGrid->m_GridCurRow);
#endif
                    DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(
                        pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol,
                        pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow : 0);
                    if(  pCell  )
                    {
                        // First select the cell,  then select the grid object,  passing in -1 values so it won't reselect the cell
                        SelectedGrid( pDesignObj_DataGrid, pCell );
                        vectSelectedGrids.push_back( pDesignObj_DataGrid );
                    }
                }
            }
#ifdef DEBUG
            g_pPlutoLogger->Write(LV_STATUS,"Before dg release.  %d",(int) vectSelectedGrids.size(  ));
#endif
            dg.Release(  );
            for( s=0;s<vectSelectedGrids.size(  );++s )
            {
#ifdef DEBUG
                g_pPlutoLogger->Write(LV_STATUS,"in for loop: %d",(int) s);
#endif
                DesignObj_DataGrid *pDesignObj_DataGrid = vectSelectedGrids[s];
                SelectedObject( pDesignObj_DataGrid, -1, -1 );
            }
#ifdef DEBUG
            g_pPlutoLogger->Write(LV_STATUS,"after for loop");
#endif
        }
    }
    if(  !( bHandled=ClickedButton( m_pScreenHistory_Current->m_pObj,  PK_Button ) )  )
    {
        if(  PK_Button>=BUTTON_Up_Arrow_CONST && PK_Button<=BUTTON_Right_Arrow_CONST  ) // up,  down,  left or right
        {
            if(  PK_Button==BUTTON_Up_Arrow_CONST  )
            {
                HighlightNextObject( DIRECTION_Up_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Down_Arrow_CONST  )
            {
                HighlightNextObject( DIRECTION_Down_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Left_Arrow_CONST  )
            {
                HighlightNextObject( DIRECTION_Left_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Right_Arrow_CONST  )
            {
                HighlightNextObject( DIRECTION_Right_CONST );
                PK_Button = -1;
            }
            bHandled=true;
        }
        else if(  PK_Button>=BUTTON_Shift_Up_Arrow_CONST && PK_Button<=BUTTON_Shift_Right_Arrow_CONST  )
        {
            if(  PK_Button==BUTTON_Shift_Up_Arrow_CONST  )
            {
                CMD_Scroll_Grid( "", "", DIRECTION_Up_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Shift_Down_Arrow_CONST  )
            {
                CMD_Scroll_Grid( "", "", DIRECTION_Down_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Shift_Left_Arrow_CONST  )
            {
                CMD_Scroll_Grid( "", "", DIRECTION_Left_CONST );
                PK_Button = -1;
            }
            else if(  PK_Button==BUTTON_Shift_Right_Arrow_CONST  )
            {
                CMD_Scroll_Grid( "", "", DIRECTION_Right_CONST );
                PK_Button = -1;
            }
        }
        else if(  PK_Button==BUTTON_F4_CONST  )  // home
        {
#ifdef DEBUG
            g_pPlutoLogger->Write(LV_STATUS,"Got an F4, sending to %s",m_sMainMenu.c_str());
#endif
            //LACA_B4_0( "Orbitercore::respond to input home" );
            GotoScreen( m_sMainMenu );
            bHandled=true;
        }
    }

#ifndef PHONEKEYS
    if( m_bCaptureKeyboard_OnOff )
    {
        if( m_iCaptureKeyboard_PK_Variable )
        {
            if( PK_Button == BUTTON_space_CONST )
                bHandled |= CaptureKeyboard_EditText_AppendChar( ' ' );
            else if( PK_Button == BUTTON_comma_CONST )
                bHandled |= CaptureKeyboard_EditText_AppendChar( ',' );
            else if(
                ( PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST ) ||
                ( PK_Button >= BUTTON_A_CONST && PK_Button <= BUTTON_Z_CONST ) ||
                ( PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST )
                )
            {
                char ch;

                if( PK_Button == BUTTON_0_CONST )
                    ch = '0';
                else if ( PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST )
                    ch = '1' + PK_Button - BUTTON_1_CONST;
                else
                    ch = ( PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST ) ?
                    'a' + PK_Button - BUTTON_a_CONST :
                'A' + PK_Button - BUTTON_A_CONST;

                bHandled |= CaptureKeyboard_EditText_AppendChar( ch );
            }
            else if( PK_Button == BUTTON_Back_CONST )
                bHandled |= CaptureKeyboard_EditText_DeleteLastChar(  );
            else if( PK_Button == BUTTON_Enter_CONST )
            {
                //TODO: send this message
                //CaptureKeyboard_UpdateVariableAndText( m_iCaptureKeyboard_PK_Variable,
                //  m_sCaptureKeyboard_InternalBuffer );
            }
            else if( PK_Button == BUTTON_left_shift_CONST || PK_Button == BUTTON_right_shift_CONST  )
                int k=2;// TODO: implement this
            else if( PK_Button == BUTTON_caps_lock_CONST )
                int k=2;// TODO: implement this

            if(bHandled)
            {
                if(NULL != m_pCaptureKeyboard_Text && NULL != m_pCaptureKeyboard_Text->m_pObject)
                {
                    PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
                    m_vectObjs_NeedRedraw.push_back(m_pCaptureKeyboard_Text->m_pObject);
                }
            }
        }

        if( m_bCaptureKeyboard_DataGrid )
        {
            if(
                ( PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST ) ||
                ( PK_Button >= BUTTON_A_CONST && PK_Button <= BUTTON_Z_CONST ) ||
                ( PK_Button == BUTTON_Back_CONST )
                )
            {
                PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
                if( m_vectObjs_GridsOnScreen.size(  ) > 0 )
                {
                    //m_sCaptureKeyboard_InternalBuffer;

                    vector<DesignObj_DataGrid *>::iterator it = m_vectObjs_GridsOnScreen.begin(  );
                    DesignObj_DataGrid *pDataGrid = *it;

                    for( int i = 0; i < pDataGrid->m_pDataGridTable->m_RowCount; i++ )
                    {
                        DataGridCell * pCell = pDataGrid->m_pDataGridTable->GetData( 0,  i );
                        string Text = "";

                        if( pCell )
                            Text = pCell->GetText(  );

                        if( 0 == Text.find( m_sCaptureKeyboard_InternalBuffer ) ) //we have a match,  Text starts with m_sCaptureKeyboard_InternalBuffer
                        {
                            pDataGrid->m_iHighlightedRow = i;

                            PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
                            m_vectObjs_NeedRedraw.push_back( pDataGrid );
                            nd.Release();

                            //selected
                            bHandled = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    //TODO: seek in data grid

#else
    if( m_bCaptureKeyboard_OnOff && m_iCaptureKeyboard_PK_Variable )
    {
        //TODO: if keys = 0,  1,  ... 9,  transform this to abc,  def,  etc.

        if( PK_Button == BUTTON_Phone_C_CONST )
            bHandled |= CaptureKeyboard_EditText_DeleteLastChar(  );
    }
#endif

    if(  pScreenHistory != m_pScreenHistory_Current  )
        m_bRerenderScreen = true;

    return bHandled;
}

bool Orbiter::ButtonDown( int iPK_Button )
{
    //if this is a repeated button, we'll handle it right away
    if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->m_pObj, iPK_Button))
    {
        return HandleButtonEvent(iPK_Button);
    }

    //this is not a repeated button
    //it will be handled in ButtonUp
    gettimeofday(&m_tButtonDown,NULL);
    return false;
}

bool Orbiter::ButtonUp( int iPK_Button )
{
	if( m_mapHardKeys.find(iPK_Button)!=m_mapHardKeys.end() )
	{
		Message *pMessage = m_mapHardKeys[iPK_Button];
		if( pMessage->m_dwPK_Device_To==m_dwPK_Device )
			ReceivedMessage( pMessage );
		else
			QueueMessageToRouter(new Message(pMessage));
	}
	if( m_bForward_local_kb_to_OSD && m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_Orbiter )
	{
		DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_dwPK_Device,m_pLocationInfo->m_dwPK_Device_Orbiter,StringUtils::itos(iPK_Button),"");
		SendCommand(CMD_Simulate_Keypress);
	}

	//if this was a repeated button, we might want to stop all repeat related events
    if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->m_pObj, iPK_Button)) 
    {
        StopRepeatRelatedEvents();
        return false;
    }

    //this is not a repeated button
    timespec m_tButtonUp;  
    gettimeofday(&m_tButtonUp,NULL);
    timespec m_tInterval = m_tButtonUp - m_tButtonDown;
    long tMilisecondsPassed = m_tInterval.tv_sec * 1000 + m_tInterval.tv_nsec / 1000000;

    if(tMilisecondsPassed > 500) //more then half of second
    {
        //it is a long key - let's get the right button code
        switch(	iPK_Button)
        {
            case BUTTON_0_CONST:        iPK_Button = BUTTON_Rept_0_CONST; break;
            case BUTTON_1_CONST:        iPK_Button = BUTTON_Rept_1_CONST; break;
            case BUTTON_2_CONST:        iPK_Button = BUTTON_Rept_2_CONST; break;
            case BUTTON_3_CONST:        iPK_Button = BUTTON_Rept_3_CONST; break;
            case BUTTON_4_CONST:        iPK_Button = BUTTON_Rept_4_CONST; break;
            case BUTTON_5_CONST:        iPK_Button = BUTTON_Rept_5_CONST; break;
            case BUTTON_6_CONST:        iPK_Button = BUTTON_Rept_6_CONST; break;
            case BUTTON_7_CONST:        iPK_Button = BUTTON_Rept_7_CONST; break;
            case BUTTON_8_CONST:        iPK_Button = BUTTON_Rept_8_CONST; break;
            case BUTTON_9_CONST:        iPK_Button = BUTTON_Rept_9_CONST; break;
            case BUTTON_Asterisk_CONST: iPK_Button = BUTTON_Rept_Asterisk_CONST;  break;
            case BUTTON_Pound_CONST:    iPK_Button = BUTTON_Rept_Pound_CONST;     break;

#ifdef PHONEKEYS
                case BUTTON_C_CONST:
                case BUTTON_c_CONST:
                    iPK_Button = BUTTON_Rept_Phone_C_CONST;         
                    break;
                case BUTTON_P_CONST:
                case BUTTON_p_CONST:
                    iPK_Button = BUTTON_Rept_Phone_Pencil_CONST;    
                    break;
                case BUTTON_T_CONST:
                case BUTTON_t_CONST:
                    iPK_Button = BUTTON_Rept_Phone_Talk_CONST;      
                    break;
                case BUTTON_E_CONST:
                case BUTTON_e_CONST:
                    iPK_Button = BUTTON_Rept_Phone_End_CONST;       
                    break;
                case BUTTON_L_CONST:
                case BUTTON_l_CONST:
                    iPK_Button = BUTTON_Rept_Phone_Soft_left_CONST; 
                    break;
                case BUTTON_R_CONST:
                case BUTTON_r_CONST:
                    iPK_Button = BUTTON_Rept_Phone_Soft_right_CONST;
                    break;
#endif 
        }

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "Long key %d", iPK_Button);
#endif
    }
#ifdef DEBUG
else
g_pPlutoLogger->Write(LV_STATUS, "Short key %d", iPK_Button);
#endif        
    return HandleButtonEvent(iPK_Button);
}

/*virtual*/ void Orbiter::StopRepeatRelatedEvents()
{
    m_bWeCanRepeat=true;
    if( m_bRepeatingObject )
    {
        PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
        for(map<int,CallBackInfo *>::iterator it=mapPendingCallbacks.begin();it!=mapPendingCallbacks.end();++it)
        {
            CallBackInfo *pCallBackInfo = (*it).second;
            if( pCallBackInfo->m_fnCallBack== &Orbiter::ReselectObject )
                pCallBackInfo->m_bStop=true;
        }
        m_bRepeatingObject=false;
    }
}

bool Orbiter::RegionUp( int x,  int y )
{
    StopRepeatRelatedEvents();
	return false;
}
bool Orbiter::RegionDown( int x,  int y )
{
	NeedToRender render( this, "Region Down" );  // Redraw anything that was changed by this command
    if( !GotActivity(  ) )
	{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Ignoring click %d,%d",x,y);
#endif
		return true;
	}

    class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

    bool bHandled=false;
    if(  !m_pScreenHistory_Current   )
    {
        // todo 2.0     if(  m_NumTouchesBeforeScreenAppears++<3  )
        //          return;
        //      exit( 1 );  // Die an immediate death
    }

    if ( m_pScreenHistory_Current  && m_pScreenHistory_Current->m_pObj->Data.m_rPosition.Contains( x,  y ) )
    {
#ifdef LOG_MOUSE_CLICKS
        g_pPlutoLogger->Write( LV_MOUSE_CLICKS,  "./MessageSend localhost 0 %d 1 305 57 %d 58 %d", m_dwPK_Device, x, y );
#endif
        DesignObj_Orbiter *pTopMostAnimatedObject=NULL;

        //LACA_B4_0( "contains it,  calling clicked region" )
#if ( defined( PROFILING ) )
    clock_t clkStart = clock(  );
#endif

    //any popup in this location ?
    DesignObj_Orbiter* pTopMostObject = m_pScreenHistory_Current->m_pObj;
    PlutoPoint RelativePoint(x, y);
    m_pActivePopup = NULL;

	if( m_pScreenHistory_Current )
	{
		for(list<class PlutoPopup*>::reverse_iterator it=m_pScreenHistory_Current->m_pObj->m_listPopups.rbegin();it!=m_pScreenHistory_Current->m_pObj->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if( pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->Data.m_rPosition.Right() &&
				pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->Data.m_rPosition.Bottom()
			)
			{
				pTopMostObject = pPopup->m_pObj;
				RelativePoint = PlutoPoint(x - pPopup->m_Position.X, y - pPopup->m_Position.Y);
				m_pActivePopup = pPopup;
				break;
			}
		}
	}

	if( !m_pActivePopup )
	{
		for(list<class PlutoPopup*>::reverse_iterator it=m_listPopups.rbegin();it!=m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(
				pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->Data.m_rPosition.Right()    &&
				pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->Data.m_rPosition.Bottom())
			{
				pTopMostObject = pPopup->m_pObj;
				RelativePoint = PlutoPoint(x - pPopup->m_Position.X, y - pPopup->m_Position.Y);
				m_pActivePopup = pPopup;
				break;
			}
		}
	}

    bHandled=ClickedRegion( pTopMostObject, RelativePoint.X, RelativePoint.Y, pTopMostAnimatedObject );

#if ( defined( PROFILING ) )
    clock_t clkFinished = clock(  );
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "$$$$$$$$$$$$$ SelectedObject took %d ms $$$$$$$$$$$$$$", clkFinished - clkStart );
    }
#endif

        /*
        if(  pTopMostAnimatedObject  )
        {
        PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
        if(  m_ChangeToScreen!=""  )
        {
        //LACA_B4_0( "fill rectangle" )
        SolidRectangle( pTopMostAnimatedObject->Data.m_rBackgroundPosition.X+XOffset, pTopMostAnimatedObject->Data.m_rBackgroundPosition.Y+YOffset,  pTopMostAnimatedObject->Data.m_rBackgroundPosition.Width,  pTopMostAnimatedObject->Data.m_rBackgroundPosition.Height,  RGB( 255, 255, 255 ),  75 );
        }
        else
        {
        CHAEffect *curEffect = CreateEffect( 1,  pTopMostAnimatedObject,  XOffset,  YOffset );
        if(  !curEffect  )
        g_pPlutoLogger->Write( LV_CRITICAL, "Create effect returned NULL" );
        else
        {
        m_EffectList.push_back( curEffect );
        m_NextEffectTime = ( m_NextEffectTime == 0 ) ? ( curEffect )->m_NextFrameTime : min( m_NextEffectTime,  ( curEffect )->m_NextFrameTime );
        }
        }
        // AnimateObject pTopMostAnimatedObject
        }
        */
    }

    if(  pScreenHistory != m_pScreenHistory_Current  )
        m_bRerenderScreen = true;

    return bHandled;
}
//------------------------------------------------------------------------
bool Orbiter::GotActivity(  )
{
    m_LastActivityTime=time( NULL );

	if( !m_bDisplayOn ||
		(m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj==m_pDesignObj_Orbiter_ScreenSaveMenu &&
			m_pDesignObj_Orbiter_MainMenu!=m_pDesignObj_Orbiter_ScreenSaveMenu) )
    {
		if( !m_bDisplayOn )
	        CMD_Display_OnOff( "1",false );
		if( m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->m_pObj == m_pDesignObj_Orbiter_ScreenSaveMenu )
		{
			CMD_Set_Main_Menu("N");
			CMD_Go_back("","1");
		}
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Ignoring click because screen saver was active");
#endif
		return false; // Don't do anything with this
    }

	if(  m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj->Data.m_dwTimeoutSeconds  )
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->m_pObj->Data.m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->m_pObj, pe_ALL );

	if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && m_pScreenHistory_Current->m_pObj != m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutScreenSaver )
	{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Got activity - calling ScreenSaver in %d ",m_iTimeoutScreenSaver);
#endif
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}
	return true;
}

/*
UTILITIES
*/
//------------------------------------------------------------------------
DesignObj_Orbiter *Orbiter::FindObject( string PK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter )
{
	PK_DesignObj = SubstituteVariables(PK_DesignObj, NULL, 0, 0);
    string OriginalValue;
    bool bSingleNumber=false;

    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	DesignObj_OrbiterMap::iterator oi;

	// Could be a screen,  find the current version and page
    if(  PK_DesignObj.find( "." )==string::npos  )
    {
        OriginalValue=PK_DesignObj;
        bSingleNumber=true;
		// AB 1 Jul 05 - check for version & page first, otherwise popups use the wrong version
		if( pDesignObj_Orbiter )
			PK_DesignObj += "." + StringUtils::itos(pDesignObj_Orbiter->Data.m_iVersion) + "." + StringUtils::itos(pDesignObj_Orbiter->Data.m_iPage);
		else 
			PK_DesignObj += "." + StringUtils::itos(m_pLocationInfo->iLocation) + ".0";

		if( (oi=m_mapObj_All.find(PK_DesignObj))!=m_mapObj_All.end(  ) )
			return oi->second;
		PK_DesignObj = OriginalValue + ".0.0";
    }

	oi=m_mapObj_All.find( PK_DesignObj );
    if ( oi==m_mapObj_All.end(  ) )
    {
        if(  bSingleNumber  )
        {
            // We were passed in another object to look at,  and assume this one is on the same screen
            if(  pDesignObj_Orbiter  )
            {
                string::size_type pos=0;
                for( int iDotCount=0;iDotCount<2 && pos!=string::npos;iDotCount++, pos++ )
                    pos=pDesignObj_Orbiter->m_ObjectID.find( ".", pos );

                // pos should now be at teh third dot,  unless we were passed in a screen which only has 2 dots
                PK_DesignObj = ( pos==string::npos ? pDesignObj_Orbiter->m_ObjectID : pDesignObj_Orbiter->m_ObjectID.substr( 0, pos+1 ) ) + "." + OriginalValue;

            }
            else if(  m_pScreenHistory_Current  )
            {
                // Maybe it's an object on the current screen
                PK_DesignObj = m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + OriginalValue;
            }
        }

        DesignObj_DataList *pObjectList;
        map<string, DesignObj_DataList *>::iterator it = m_mapObj_AllNoSuffix.find( PK_DesignObj );
        if(  it!=m_mapObj_AllNoSuffix.end(  )  )
        {
            pObjectList = ( *it ).second;
            if(  pObjectList->size(  )==1  )
                return ( DesignObj_Orbiter * )pObjectList->front(  );

            DesignObj_DataList::iterator itObjList;
            for( itObjList=pObjectList->begin(  );itObjList!=pObjectList->end(  );++itObjList )
            {
                DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * )( *itObjList );
                if(  pObj->m_bOnScreen && !pObj->IsHidden(  ) )
                {
                    return pObj;
                }
            }

            // So we didn't find anything on screen,  just return the first
            if(  pObjectList->size(  )>0  )
                return ( DesignObj_Orbiter * ) pObjectList->front(  );
        }
        // We're going to have to hunt for it
        else if(  bSingleNumber  )
        {
            // Release the variable mutex first.  We don't want variables locked while we wait for overlay to become
            // available.
            vm.Release(  );
            // Now re-lock the variable mutex
            PLUTO_SAFETY_LOCK( vm2, m_VariableMutex );
                if(  pDesignObj_Orbiter  )
                    FindSingleNumberObject( atoi( OriginalValue.c_str(  ) ), pDesignObj_Orbiter );
                else if(  m_pScreenHistory_Current  )
                    FindSingleNumberObject( atoi( OriginalValue.c_str(  ) ), m_pScreenHistory_Current->m_pObj );
                else
                    return NULL;
        }
    }
    else
    {
        return ( ( *oi ).second );
    }

    return NULL;
}
//------------------------------------------------------------------------
DesignObj_Orbiter* Orbiter::FindSingleNumberObject( int PK_Object,  DesignObj_Orbiter *pObj )
{
    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
        if(  pChild->Data.m_iBaseObjectID==PK_Object  )
            return pChild;
        pChild = FindSingleNumberObject( PK_Object, pChild );
        if(  pChild  )
            return pChild;
    }

    for( iHao=pObj->m_OverlayObjects.begin(  ); iHao != pObj->m_OverlayObjects.end(  ); ++iHao )
    {
        DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
        if(  pChild->Data.m_iBaseObjectID==PK_Object  )
            return pChild;

        pChild = FindSingleNumberObject( PK_Object, pChild );
        if(  pChild  )
            return pChild;
    }
    return NULL;
}
//------------------------------------------------------------------------
void Orbiter::ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList,
                                    DesignObj_Orbiter *pObj,
                                    int X,  int Y, int Repeat )
{
	if(m_bQuit)
		return;

	if(  pDesignObjCommandList->size(  )==0  )
        return;

	if( pObj->Data.m_iRepeatIntervalInMS && m_bWeCanRepeat )
	{
		m_bRepeatingObject=true;
		pObj->m_iRepeatParm = Repeat;
		// Arbitrary decision that there should be a bit of a delay before the object starts repeating that
		// is 3 times the repeat interval
		CallMaintenanceInMiliseconds( (Repeat==0 ? pObj->Data.m_iRepeatIntervalInMS * 3 : pObj->Data.m_iRepeatIntervalInMS),
			&Orbiter::ReselectObject, pObj, pe_ALL, true );
	}

#ifdef DEBUG
    g_pPlutoLogger->Write( LV_STATUS, "Executing %d commands in object: %s", ( int ) pDesignObjCommandList->size(  ),  pObj->m_ObjectID.c_str(  ) );
#endif

    bool bRefreshGrids=false;

    Message *pMessage=NULL;

    //          g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33;1mSelected object: %s zone: %d, %d-%d, %d,  actions in zone: %d,  obj %p zone %p", pObj->m_ObjectID.c_str(  ),
    //              ( *iZone )->m_Rect.X, ( *iZone )->m_Rect.Y, ( *iZone )->m_Rect.Width, ( *iZone )->m_Rect.Height,
    //              ( *iZone )->m_Commands.size(  ), pObj, ( *iZone ) );
    // The commands are in the list backwards, so use a reverse iterator so that they are executed in the same order they appear within designer
    DesignObjCommandList::reverse_iterator iCommand;
	bool bGetConfirmation=false;
    for(iCommand=pDesignObjCommandList->rbegin();iCommand!=pDesignObjCommandList->rend();++iCommand)
    {
        DesignObjCommand *pCommand = *iCommand;
        int PK_Command = pCommand->Data.m_PK_Command;
		if( pCommand->Data.m_bDeliveryConfirmation )
			bGetConfirmation=true;

#ifdef DEBUG
        g_pPlutoLogger->Write( LV_STATUS, "Executing command %d in object: %s", PK_Command,  pObj->m_ObjectID.c_str(  ) );
#endif

        if(  PK_Command==COMMAND_Restart_DCERouter_CONST && Simulator::GetInstance()->IsRunning() )
			continue;  // While in stress-test simulator, don't try to reset the router
        else if(  PK_Command==COMMAND_Requires_Special_Handling_CONST  )
        {
            SpecialHandlingObjectSelected( pObj );
            continue;
        }
        else if( PK_Command==COMMAND_Set_Current_User_CONST && m_vectPK_Users_RequiringPIN.size() )
		{
			int PK_Users = atoi(pCommand->m_ParameterList[COMMANDPARAMETER_PK_Users_CONST].c_str());
			for(size_t s=0;s<m_vectPK_Users_RequiringPIN.size();++s)
			{
				if( PK_Users==m_vectPK_Users_RequiringPIN[s] )
				{
					// This user requires a PIN.  Change there and return
					m_bRestrictedOp_IsUser=true;
					m_iRestrictedOp_ID=PK_Users;
					CMD_Goto_Screen(0,StringUtils::itos(DESIGNOBJ_mnuUserPinCode_CONST),"","",false,true);
					return;
				}
			}
		}
		else if( PK_Command==COMMAND_Set_Current_Location_CONST )
		{
			// Only cc
			int iLocationID = atoi(pCommand->m_ParameterList[COMMANDPARAMETER_LocationID_CONST].c_str());
	        LocationInfo *pLocationInfo = m_dequeLocation[iLocationID];
			if( pLocationInfo->m_vectAllowedUsers.size() )
			{
				// Only some users can switch to this location
				size_t s;
				for(s=0;s<pLocationInfo->m_vectAllowedUsers.size();++s)
				{
					if( m_dwPK_Users==pLocationInfo->m_vectAllowedUsers[s] )
						break; // this one's ok
				}
				if( s>=pLocationInfo->m_vectAllowedUsers.size() )
				{
					CMD_Set_Text( StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "You are not allowed to access this location", TEXT_STATUS_CONST);
					CMD_Goto_Screen( 0, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "", "", false, true );
					return;
				}
			}
		}

        // We may attach a simulate keypress action to an onscreen button, like a keyboard key, which simulates pressing
        // that key if the user touches it.  In this case, we don't want to execute the simulate keypress command
        // if the button was selected because the user hit the button associated with it (ie x==-1 and y==-1 when input was ButtonDown)
        // and create an endless loop
        else if( PK_Command==COMMAND_Simulate_Keypress_CONST && X==-1 && Y==-1 )
            continue;


// HACK! HACK!
// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
// HACK! HACK!
// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
// HACK! HACK!
// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
// HACK! HACK!
// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT

        if(  pCommand->Data.m_PK_DeviceTemplate  )
        {
            if(  pCommand->Data.m_PK_DeviceTemplate<0  )
            {
                pCommand->Data.m_PK_Device=DEVICEID_HANDLED_INTERNALLY;  // Set this to a temporary value,  so we'll know if it's changes at teh end of the if
				if( pCommand->Data.m_PK_DeviceTemplate!=DEVICEID_HANDLED_INTERNALLY )  // Just leave it there
				{
					pCommand->Data.m_PK_Device = TranslateVirtualDevice(pCommand->Data.m_PK_DeviceTemplate);
					if(  pCommand->Data.m_PK_Device==DEVICEID_NULL  )
					{
						// We recognized it as one of the known cases in the switch block,  but apparently this option isn't sent
						g_pPlutoLogger->Write( LV_CRITICAL, "Sending command to unidentified virtual device: %d", pCommand->Data.m_PK_DeviceTemplate );
						pCommand->Data.m_PK_Device = pCommand->Data.m_PK_DeviceTemplate; // Maybe there's some interceptor that will know what to do with it
					}
					else if(  pCommand->Data.m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
						pCommand->Data.m_PK_Device = pCommand->Data.m_PK_DeviceTemplate; // It's not a virtual device we know how to handle.  Let the interceptors have it
				}
            }
            else
                pCommand->Data.m_PK_Device=DEVICEID_MASTERDEVICE;
        }
        else if(  pCommand->Data.m_PK_DeviceCategory  )
            pCommand->Data.m_PK_Device=DEVICEID_CATEGORY;

        if(  pCommand->Data.m_PK_Device==0 && pCommand->Data.m_PK_DeviceGroup==0  )
        {
            // See if it's one we handle locally
            if(  m_bLocalMode )
            {
                pCommand->Data.m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
            }
            else if( m_mapDevice_Selected.size() )
                pCommand->Data.m_PK_Device = DEVICEID_LIST;
            else
            {
                PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
                pCommand->Data.m_PK_Device = atoi( m_mapVariable[VARIABLE_PK_Device_CONST].c_str(  ) );
                vm.Release(  );
            }
        }

        if(  pCommand->Data.m_PK_Device==DEVICEID_NULL  )
        {
            g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33;1m****ACTION %d GOING TO DEVICEID_NULL***%d\x1b[0m\n", PK_Command );
        }
        else
        {
            if(  PK_Command==COMMAND_Populate_Datagrid_CONST  )
            {
				// If we already added messages to the queue, go ahead and send them out so that the populate datagrid is executed in the proper order
				if(  pMessage && !m_bLocalMode  )
				{
					if( !m_pcRequestSocket->SendMessage( pMessage ) )
						g_pPlutoLogger->Write( LV_CRITICAL,  "Send Message failed");
					pMessage = NULL;
				}

                PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
                string GridID = pCommand->m_ParameterList[COMMANDPARAMETER_DataGrid_ID_CONST];
                DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen=NULL;
                // See if this grid is onscreen
                for( size_t s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
                {
                    DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[s];
                    if(  pDesignObj_DataGrid->m_sGridID == GridID  )
                    {
                        pDesignObj_DataGrid_OnScreen=pDesignObj_DataGrid;
                        pDesignObj_DataGrid_OnScreen->m_iHighlightedRow=0;
                        pDesignObj_DataGrid_OnScreen->m_iHighlightedColumn=0;
                        bRefreshGrids=true;
                    }
                }

                bool bResponse;
                int iPK_Variable=0;
                string sValue_To_Assign;
				int iWidth,iHeight;
                DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ),
                    GridID, atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ),
                    SubstituteVariables( pCommand->m_ParameterList[COMMANDPARAMETER_Options_CONST], pObj, 0, 0 ), &iPK_Variable, &sValue_To_Assign, &bResponse, &iWidth, &iHeight );
                if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
                    g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid from command: %d failed", atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ) );
                else if(  iPK_Variable  )
				{
					PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
                    m_mapVariable[iPK_Variable] = sValue_To_Assign;
				}

                continue; // processed this one ourselves
            }

            Message *pThisMessage = new Message( m_dwPK_Device, pCommand->Data.m_PK_Device, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, PK_Command, 0 );
            if(  pCommand->Data.m_PK_Device==DEVICEID_MASTERDEVICE  )
                pThisMessage->m_dwPK_Device_Template=pCommand->Data.m_PK_DeviceTemplate;
            else if(  pCommand->Data.m_PK_Device==DEVICEID_CATEGORY  )
                pThisMessage->m_dwPK_Device_Category_To=pCommand->Data.m_PK_DeviceCategory;
            else if(  pCommand->Data.m_PK_Device==DEVICEID_LIST  )
			{
				for(map<int,class DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				{
					pThisMessage->m_sPK_Device_List_To += StringUtils::itos((*it).first) + ",";
				}
			}

			bool bAlreadySetRepeat=false;
            map<int,  string>::iterator iap;
            for( iap=pCommand->m_ParameterList.begin(  );iap!=pCommand->m_ParameterList.end(  );++iap )
            {
                string Value = SubstituteVariables( ( *iap ).second, pObj, X, Y );
                pThisMessage->m_mapParameters[( *iap ).first]=Value;
				if( ( *iap ).first==COMMANDPARAMETER_Repeat_Command_CONST )
					bAlreadySetRepeat=true;

#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "Parm %d = %s",( *iap ).first,Value.c_str());
#endif
            }
			if( Repeat && !bAlreadySetRepeat )
				pThisMessage->m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST]=StringUtils::itos(Repeat);


			pThisMessage->m_dwPK_Device_Group_ID_To=pCommand->Data.m_PK_DeviceGroup;

            if(  pCommand->Data.m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
            {
                pThisMessage->m_dwPK_Device_To = m_dwPK_Device; // So the handler will loop back to ourselves
                ReceivedMessage( pThisMessage );
				delete pThisMessage;
				pThisMessage = NULL;

                // Don't bother sending grid movements either
                if(  PK_Command==COMMAND_Simulate_Keypress_CONST ||
                    PK_Command==COMMAND_Scroll_Grid_CONST ||
                    PK_Command==COMMAND_Show_Object_CONST  )
                {
                    continue;
                }
            }
            else
				if(  pMessage==NULL  )
					pMessage=pThisMessage;
				else
					pMessage->m_vectExtraMessages.push_back( pThisMessage );
        }
    }
    if(  pMessage && !m_bLocalMode  )
    {
		if( bGetConfirmation )
		{
			pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
			string sResponse; // We'll use this only if a response wasn't passed in
			bool bResult = m_pcRequestSocket->SendMessage( pMessage, sResponse );
			if( !bResult || sResponse != "OK" )
				g_pPlutoLogger->Write( LV_CRITICAL,  "Send Message failed with result %d Response %s",(int) bResult,sResponse.c_str());
		}
		else if( pMessage->m_dwID==COMMAND_Bind_to_Media_Remote_CONST )
		{
			pMessage->m_eExpectedResponse = ER_ReplyMessage;  // a set now playing command
			Message *pResponse = m_pcRequestSocket->SendReceiveMessage( pMessage );
			if( pResponse && pResponse->m_dwID )
			{
                ReceivedMessage( pResponse );
				delete pResponse;
			}
		}
		else
		{
			if( !m_pcRequestSocket->SendMessage( pMessage ) )
				g_pPlutoLogger->Write( LV_CRITICAL,  "Send Message failed");
		}
		pMessage = NULL;
    }
    if(  bRefreshGrids  )
    {
        // This includes a populate datagrid.  We are probably changing the contents of a
        // grid that's on screen,  so send this with confirmation to be sure the thread blocks
        // until it goes through,  and then refresh our grids
        PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
        for( size_t grid=0;grid<m_vectObjs_GridsOnScreen.size(  );++grid )
        {
            DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[grid];
            pDesignObj_DataGrid->bReAcquire=true;
        }

		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        m_vectObjs_NeedRedraw.push_back( m_pScreenHistory_Current->m_pObj );
    }
}

//------------------------------------------------------------------------
void Orbiter::GetGridCellDimensions( DesignObj_DataGrid *pDesignObj_DataGrid,  int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h )
{
    if ( Colspan < 1 )
    {
        g_pPlutoLogger->Write( LV_WARNING,  "Data grid cell has column span of zero!" );
        x=y=w=h=0;
        return;
    }

    // These deltas make adjustments to the rest of the coodrinates when there's a first column
    // on the grid that's a different size from the rest.

    int DeltaX = 0,  DeltaY = 0;

    // Is the first row on the screen?
    if ( Row > 0 && pDesignObj_DataGrid->m_FirstRowHeight > 0 && ( pDesignObj_DataGrid->m_GridCurRow == 0 || pDesignObj_DataGrid->m_bKeepRowHeader )  )
    {
        DeltaY = pDesignObj_DataGrid->m_FirstRowHeight - pDesignObj_DataGrid->m_FixedRowHeight;
    }
    // Is the first column on the screen?
    if ( Column > 0 && pDesignObj_DataGrid->m_FirstColumnWidth > 0 && ( pDesignObj_DataGrid->m_GridCurCol == 0 || pDesignObj_DataGrid->m_bKeepColHeader )  )
    {
        DeltaX = pDesignObj_DataGrid->m_FirstColumnWidth - pDesignObj_DataGrid->m_FixedColumnWidth;
    }

    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	if( m_iRotation==90 )
	{
	    x = pDesignObj_DataGrid->Data.m_rPosition.Right()-( Row*( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX - pDesignObj_DataGrid->m_FixedRowHeight;
		y = pDesignObj_DataGrid->Data.m_rPosition.Y+( Column *( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY;
	}
	else if( m_iRotation==180 )
	{
	    x = pDesignObj_DataGrid->Data.m_rPosition.Right()-( (Column+Colspan-1)*( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX - pDesignObj_DataGrid->m_FixedColumnWidth;
		y = pDesignObj_DataGrid->Data.m_rPosition.Bottom()-( (Row+Rowspan-1) *( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY - pDesignObj_DataGrid->m_FixedRowHeight;
	}
	else if( m_iRotation==270 )
	{
	    x = pDesignObj_DataGrid->Data.m_rPosition.X+( Row*( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX;
		y = pDesignObj_DataGrid->Data.m_rPosition.Bottom()-( (Column+Colspan-1) *( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY - pDesignObj_DataGrid->m_FixedColumnWidth;
	}
	else
	{
	    x = pDesignObj_DataGrid->Data.m_rPosition.X+( Column*( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX;
		y = pDesignObj_DataGrid->Data.m_rPosition.Y+( Row *( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY;
	}

    if ( Column == 0 && pDesignObj_DataGrid->m_FirstColumnWidth > 0 && ( pDesignObj_DataGrid->m_GridCurCol == 0 || pDesignObj_DataGrid->m_bKeepColHeader ))
    {
        w = pDesignObj_DataGrid->m_FirstColumnWidth + (Colspan-1) * pDesignObj_DataGrid->m_FixedColumnWidth + ( Colspan-1 );
    }
    else
    {
        w = pDesignObj_DataGrid->m_FixedColumnWidth * Colspan + ( Colspan-1 );
    }
    if ( Row == 0 && pDesignObj_DataGrid->m_FirstRowHeight > 0  && ( pDesignObj_DataGrid->m_GridCurRow == 0 || pDesignObj_DataGrid->m_bKeepRowHeader ) )
    {
        h = pDesignObj_DataGrid->m_FirstRowHeight + (Rowspan-1) * pDesignObj_DataGrid->m_FixedRowHeight + ( Rowspan-1 );
    }
    else
    {
        h = pDesignObj_DataGrid->m_FixedRowHeight * Rowspan+ ( Rowspan-1 );
    }

	if( m_iRotation==0 )
	{
	    if ( x+w > pDesignObj_DataGrid->Data.m_rPosition.X + pDesignObj_DataGrid->Data.m_rPosition.Width )
		    // Oops,  width is greater than the size of the object.  Truncate.
	        w = ( pDesignObj_DataGrid->Data.m_rPosition.X+pDesignObj_DataGrid->Data.m_rPosition.Width ) - x;
	    if ( y+h > pDesignObj_DataGrid->Data.m_rPosition.Y + pDesignObj_DataGrid->Data.m_rPosition.Height )
	        // Oops,  height is greater than the size of the object.  Truncate.
		    h = ( pDesignObj_DataGrid->Data.m_rPosition.Y+pDesignObj_DataGrid->Data.m_rPosition.Height ) - y;
    }

    vm.Release(  );
    if ( w<0 || h<0 )
    {
        g_pPlutoLogger->Write( LV_WARNING,  "Data grid cell has negative size values."  );
        x=y=w=h=0;
        return;
    }

	if( m_iRotation==90 || m_iRotation==270 )
	{
		int ww = w;
		w=h;
		h=ww;
	}
}
//------------------------------------------------------------------------
string Orbiter::SubstituteVariables( string Input,  DesignObj_Orbiter *pObj,  int X,  int Y )
{
    string Output;

    size_t pos=0;
    for( ;; )
    {
        size_t foundbegin;
        foundbegin=Input.find( "<%=",  pos );
        if ( foundbegin==string::npos )
            break;
        Output+=Input.substr( pos,  ( foundbegin-pos ) );
        size_t foundend=Input.find( "%>",  foundbegin );
        pos=foundbegin+3;
        if ( foundend==string::npos )
            break;

        int PK_Variable=0;
        string Variable = Input.substr( pos,  foundend-pos );
        if(  Variable=="M"  )
            Output += m_sMainMenu;
        else if(  Variable=="!"  )
            Output += StringUtils::itos( m_dwPK_Device );
        else if(  Variable=="RIP"  )
            Output += m_sIPAddress;
        else if(  Variable[0]=='!'  )  // It starts with ! -- that's a not followed by a variable, if the variable is 1 we return 0, anything else we return 1
		{
			int PK_Variable = atoi( Variable.substr(1).c_str() );
            PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			Output+= (m_mapVariable[PK_Variable]!="1" ? "1" : "0");
		}
        else if(  Variable=="E" && m_pLocationInfo  )
            Output += StringUtils::itos( m_pLocationInfo->PK_EntertainArea );
        else if(  Variable=="MD" && m_pLocationInfo  )
            Output += StringUtils::itos( m_pLocationInfo->m_dwPK_Device_MediaDirector );
        else if(  Variable=="MDH"  )
            Output += StringUtils::itos( m_pLocationInfo_Initial->m_dwPK_Device_MediaDirector );
        else if(  Variable=="LD" && m_pLocationInfo  )
            Output += m_pLocationInfo->Description;
        else if(  Variable=="LDGC"  )
            Output += m_sLastSelectedDatagrid;
        else if(  Variable=="V" )
			Output += string(VERSION) + "(" + g_szCompile_Date + ")";
        else if(  Variable=="NP" )
		{
            Output += m_sNowPlaying;
			m_pObj_NowPlayingOnScreen=pObj;
			if( !m_pObj_NowPlayingOnScreen->m_pvectCurrentGraphic || m_pObj_NowPlayingOnScreen->m_pvectCurrentGraphic->size()==0 )
				SaveBackgroundForDeselect( m_pObj_NowPlayingOnScreen, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
        else if(  Variable=="NPD" )
		{
			Output += StringUtils::itos(m_dwPK_Device_NowPlaying);
		}
        else if(  Variable=="NP_R" )
		{
			if( m_bIsOSD && m_iPK_DesignObj_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
				Output += m_iPK_DesignObj_RemoteOSD ? StringUtils::itos(m_iPK_DesignObj_RemoteOSD) : "**NOMEDIA**";
			else
				Output += m_iPK_DesignObj_Remote ? StringUtils::itos(m_iPK_DesignObj_Remote) : "**NOMEDIA**";
		}
        else if(  Variable=="NP_PR" )
			Output += StringUtils::itos(m_iPK_DesignObj_Remote_Popup);
        else if(  Variable=="NP_FL" )
			Output += StringUtils::itos(m_iPK_DesignObj_FileList);
        else if(  Variable=="NP_PFL" )
			Output += StringUtils::itos(m_iPK_DesignObj_FileList_Popup);
        else if(  Variable=="NP_RNF" )
			Output += StringUtils::itos(m_iPK_DesignObj_Remote);
        else if(  Variable=="B" )
			Output += "\t";
        else if(  Variable=="ND" )
			Output += StringUtils::itos((int) m_mapDevice_Selected.size());
        else if(  Variable=="DFN"  )
			Output += m_sDefaultRippingName;
        else if(  Variable.length()>1 && Variable[0]=='G' && Variable[1]=='D' )
		{
			DesignObj_Orbiter *pObjGD = pObj;
			if( Variable.length()>2 )
			{
				pObjGD = FindObject(Variable.substr(2));
				if( !pObjGD )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find GD%s",Variable.substr(2).c_str());
					pObjGD = pObj;
				}
			}
			Output += StringUtils::itos(pObjGD->m_GraphicToDisplay);
		}
        else if(  Variable.length()>1 && Variable[0]=='A' )
		{
			int PK_Attribute;
			string sAttribute;
			if( Variable[1]=='V' )
				PK_Attribute = atoi(m_mapVariable[atoi(Variable.substr(2).c_str())].c_str());
			else
				PK_Attribute = atoi(Variable.substr(2).c_str());
			DCE::CMD_Get_Attribute CMD_Get_Attribute(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,PK_Attribute,&sAttribute);
			SendCommand(CMD_Get_Attribute);
			Output += sAttribute;
		}
        else if(  Variable.length()>1 && Variable[0]=='E' )
		{
			Output += "<%=" + Variable.substr(1) + "%>";
		}
        else if(  Variable=="SKIN" )
			Output += m_sSkin;
        else if(  Variable=="SG" )
		{
			time_t t = m_tGenerationTime;
			struct tm *p_tm = localtime( &t );
			Output += StringUtils::itos( p_tm->tm_mon+1 ) + "/" + StringUtils::itos( p_tm->tm_mday) + "/" + StringUtils::itos( p_tm->tm_year+1900 ) + " " +
				StringUtils::itos( p_tm->tm_hour ) + ":" + StringUtils::itos( p_tm->tm_min );
		}
        else if(  Variable=="CD" )
		{
			// Find if there are any selected object timeout's pending
			PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
			for(map<int,CallBackInfo *>::iterator it=mapPendingCallbacks.begin();it!=mapPendingCallbacks.end();++it)
			{
				CallBackInfo *pCallBackInfo = (*it).second;
				if( pCallBackInfo->m_fnCallBack == &Orbiter::DelayedSelectObject && !pCallBackInfo->m_bStop )
				{
					DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) pCallBackInfo->m_pData;
					if( TestCurrentScreen(pDelayedSelectObjectInfo->m_sPK_DesignObj_CurrentScreen) )
					{
						timespec ts_Current,ts;
						gettimeofday(&ts_Current,NULL);
						ts = pDelayedSelectObjectInfo->m_tsTime - ts_Current;

						Output += StringUtils::itos(ts.tv_sec);
						break;
					}
				}
			}
		}
        else if(  Variable=="SD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				Output += StringUtils::itos((*it).first) + ",";
        else if(  Variable=="SDD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				Output += it->second->m_sDescription + ",";
        else if(  Variable=="R" && m_pLocationInfo  )
            Output += StringUtils::itos( m_pLocationInfo->PK_Room );
        else if(  Variable=="U" )
            Output += StringUtils::itos( m_dwPK_Users );
        else if(  Variable=="X"  )
            Output += StringUtils::itos( X );
        else if(  Variable=="Y"  )
            Output += StringUtils::itos( Y );
        else if(  Variable=="XS"  )
        {
            if(pObj->m_vectGraphic.size() && pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Width > 0)
            {
                int ClickX = X - pObj->Data.m_rPosition.X;
                Output+=StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Width * ClickX / pObj->Data.m_rPosition.Width );
            }
        }
        else if(  Variable=="YS"  )
        {
            if(pObj->m_vectGraphic.size() && pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Height > 0)
            {
                int ClickY = Y - pObj->Data.m_rPosition.Y;
                Output += StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Height * ClickY / pObj->Data.m_rPosition.Height );
            }
        }
        else if(  Variable=="S"  )
            Output += m_pScreenHistory_Current->m_pObj->m_ObjectID;
        else if(  Variable=="O"  )
            Output += pObj->m_ObjectID;
        else if( Variable.length()>1 && Variable[0]=='T'  )
		{
			int PK_Text=atoi(Variable.substr(1).c_str());
			Output += m_mapTextString[PK_Text];
		}
        else if( Variable.length()>1 && Variable[0]=='V'  )
		{
			int VirtDev=atoi(Variable.substr(1).c_str());
			Output += StringUtils::itos(TranslateVirtualDevice(VirtDev));
		}
        else if( Variable.length()>1 && Variable[0]=='D'  )
		{
			int PK_Variable=atoi(Variable.substr(1).c_str());
            PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			int PK_Device = atoi(m_mapVariable[PK_Variable].c_str());
            vm.Release(  );

			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find( PK_Device );
			if( pDevice )
			{
				Output += pDevice->m_sDescription;
			}
		}
        else
            PK_Variable = atoi( Variable.c_str(  ) );

        if(  PK_Variable>0  )
        {
            PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
            Output+=m_mapVariable[PK_Variable];
            vm.Release(  );
        }
        pos=foundend+2;
    }
    Output+=Input.substr( pos );
    return Output;
}

//------------------------------------------------------------------------
bool Orbiter::DoComparisson( int PK_Variable,  string oComparisson,  string oValue )
{
    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    string Variable = m_mapVariable[PK_Variable];
    vm.Release(  );

    if(  Variable.length(  )==0 && oValue.length(  )==0 && oComparisson=="="  )
        return true;

    if(  Variable.length(  )==0 && oValue.length(  )==0  )
        return false;

    if(  ( Variable.length(  )==0 || oValue.length(  )==0 ) && ( oComparisson=="<>" || oComparisson=="<>" )  )
        return true;

    if(  Variable.length(  )==0 || oValue.length(  )==0  )
        return false;

    if(  oComparisson=="="  )
        return Variable == oValue;

    else if(  oComparisson=="<"  )
        return atoi( Variable.c_str(  ) ) < atoi( oValue.c_str(  ) );

    else if(  oComparisson==">"  )
        return atoi( Variable.c_str(  ) ) > atoi( oValue.c_str(  ) );

    else if(  oComparisson=="!=" || oComparisson=="<>"  )
        return Variable != oValue;

    return false;

}

//------------------------------------------------------------------------
void Orbiter::FindDGArrows( DesignObj_Orbiter *pObj, DesignObj_DataGrid *pDGObj )
{
    DesignObjZoneList::iterator iZone;
    for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
    {
        DesignObjCommandList::iterator iCommand;
        for( iCommand=( *iZone )->m_Commands.begin(  );iCommand!=( *iZone )->m_Commands.end(  );++iCommand )
        {
            DesignObjCommand *pCommand = ( *iCommand );

            if(  pCommand->Data.m_PK_Command == COMMAND_Scroll_Grid_CONST  )
            {
                if(  pCommand->m_ParameterList[COMMANDPARAMETER_Level_CONST]=="-1"  )
                    continue; // This is just a move to the top/bottom
                // If this Command points to this grid or to nothing,  it's a match
                if(  atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str(  ) ) == pDGObj->Data.m_iBaseObjectID ||
                    atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str(  ) ) == 0  )
                {
                    int PK_Direction = atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_Direction_CONST].c_str(  ) );
                    if(  PK_Direction == DIRECTION_Up_CONST  )
                        pDGObj->m_pObjUp=pObj;
                    else if(  PK_Direction == DIRECTION_Down_CONST  )
                        pDGObj->m_pObjDown=pObj;
                    else if(  PK_Direction == DIRECTION_Left_CONST  )
                        pDGObj->m_pObjLeft=pObj;
                    else if(  PK_Direction == DIRECTION_Right_CONST  )
                        pDGObj->m_pObjRight=pObj;
                }
            }
        }
    }
    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        FindDGArrows( ( DesignObj_Orbiter * )( *iHao ), pDGObj );
    }
}
bool Orbiter::AcquireGrid( DesignObj_DataGrid *pObj,  int GridCurCol,  int &GridCurRow,  DataGridTable* &pDataGridTable )
{
g_PlutoProfiler->Start("acquire grid");
    bool bLoadedSomething = false;

    if (  pObj->bReAcquire || !pDataGridTable || pDataGridTable->m_StartingColumn != GridCurCol || pDataGridTable->m_StartingRow != GridCurRow || pObj->m_sSeek.length() )
    {
		pObj->m_iHighlightedColumn=pObj->m_iHighlightedRow=-1;
        if ( pDataGridTable )
        {
            delete pDataGridTable;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable deleted indirectly",pObj->m_ObjectID.c_str());
#endif
            pDataGridTable = NULL;
        }
		else if( pObj->m_sExtraInfo.find('S')!=string::npos && pObj->m_sSeek.length()==0 && !pObj->sSelVariable.empty(  ) && !pObj->bReAcquire )
	    {
		    PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
            pObj->m_sSeek = "~" + m_mapVariable[atoi( pObj->sSelVariable.c_str(  ) )];
	    }
		pObj->bReAcquire=false;

        ++m_dwIDataGridRequestCounter;

#ifdef DEBUG
        g_pPlutoLogger->Write( LV_STATUS,  "requesting grid ( # %d ),  %s.",  m_dwIDataGridRequestCounter,
            pObj->m_sGridID.c_str(  ) );
#endif
        int size = 0;
        char *data = NULL;

        DCE::CMD_Request_Datagrid_Contents CMD_Request_Datagrid_Contents( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,
            StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID, GridCurCol,
            pObj->m_MaxRow, pObj->m_MaxCol, pObj->m_bKeepRowHeader, pObj->m_bKeepColHeader, true, pObj->m_sSeek, pObj->m_iSeekColumn, &data, &size, &GridCurRow );

        if(  !SendCommand( CMD_Request_Datagrid_Contents )  )
            g_pPlutoLogger->Write( LV_CRITICAL, "Request datagrid: %s failed", pObj->m_ObjectID.c_str(  ) );
        else
        {
            if ( size && data )
            {
                pDataGridTable = new DataGridTable( size,  data );

				delete[] data; 
				data = NULL;

#ifdef DEBUG
				g_pPlutoLogger->Write( LV_STATUS, "Got %d rows %d cols in new %s m_pDataGridTable %p", pDataGridTable->GetRows(  ), pDataGridTable->GetCols(  ), pObj->m_ObjectID.c_str(), pDataGridTable );
#endif
                if(  !pDataGridTable->GetRows(  ) || !pDataGridTable->GetCols(  )  )
                {
                    // Initialize grid will set these to 0,  assuming there will be data.  If the grid is empty,  change that
                    pObj->m_iHighlightedRow=-1;
                    pObj->m_iHighlightedColumn=-1;
                }
                pObj->m_sSeek="";  // Only do the seek 1 time
g_PlutoProfiler->Stop("acquire grid");
                return pDataGridTable!=NULL;
            }
            else
            {
                g_pPlutoLogger->Write( LV_WARNING,  "Error loading grid ID: %d",  m_dwIDataGridRequestCounter );
            }
        }
    }
g_PlutoProfiler->Stop("acquire grid");
    return false;
}

void *MaintThread(void *p)
{
	bMaintThreadIsRunning = true;
	Orbiter* pOrbiter = (Orbiter *)p;

	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, pOrbiter->m_MaintThreadMutex);// Keep this locked to protect the map
	while(!pOrbiter->m_bQuit)
	{
		if(mapPendingCallbacks.size() == 0)
		{
			//nothing to process. let's sleep...
			cm.CondWait(); // This will unlock the mutex and lock it on awakening
		}
		else
		{
			// We've got stuff to check out
			CallBackInfo *pCallBackInfoGood = NULL;
			timespec ts_NextCallBack,ts_now;
			ts_NextCallBack.tv_sec=0;
			gettimeofday(&ts_now,NULL);

			//let's choose the one which must be processed first
			for(map<int,CallBackInfo *>::iterator it=mapPendingCallbacks.begin();it!=mapPendingCallbacks.end();)
			{
				CallBackInfo *pCallBackInfo = (*it).second;

				if( pCallBackInfo->m_bStop )
				{
					mapPendingCallbacks.erase( it++ );  // This is dead anyway
					delete pCallBackInfo;
					continue;
				}
				else if(pCallBackInfo->m_abstime <= ts_now)
				{
					mapPendingCallbacks.erase( it );
					pCallBackInfoGood = pCallBackInfo;
					break;  // We got one to execute now
				}
				else if( ts_NextCallBack.tv_sec==0 || pCallBackInfo->m_abstime<ts_NextCallBack )
					ts_NextCallBack = pCallBackInfo->m_abstime;  // This is the next one to call
				it++;
			}

			if( pCallBackInfoGood )
			{
				cm.Release(); // Don't keep the mutex locked while executing
				CALL_MEMBER_FN(*(pCallBackInfoGood->m_pOrbiter), pCallBackInfoGood->m_fnCallBack)(pCallBackInfoGood->m_pData);
				cm.Relock();
                delete pCallBackInfoGood;
			}
			else if( ts_NextCallBack.tv_sec!=0 ) // Should be the case
			{
				cm.TimedCondWait(ts_NextCallBack);
			}
		}
	}

	bMaintThreadIsRunning = false;
	return NULL;
}

//------------------------------------------------------------------------
void Orbiter::CallMaintenanceInMiliseconds( clock_t milliseconds, OrbiterCallBack fnCallBack,
										   void *data, ePurgeExisting e_PurgeExisting,
										   bool bPurgeTaskWhenScreenIsChanged/*= true*/
										   )
{
    PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "CallMaintenanceInMiliseconds started with mapPendingCallbacks size: %d",
        mapPendingCallbacks.size());
#endif

	if( e_PurgeExisting!=pe_NO )
	{
		for(map<int,CallBackInfo *>::iterator it=mapPendingCallbacks.begin();it!=mapPendingCallbacks.end();++it)
		{
			CallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack==fnCallBack &&
					(e_PurgeExisting==pe_ALL || pCallBackInfo->m_pData==data) )
				pCallBackInfo->m_bStop=true;
		}
	}

    CallBackInfo *pCallBackInfo = new CallBackInfo( bPurgeTaskWhenScreenIsChanged );

	gettimeofday(&pCallBackInfo->m_abstime,NULL);
	pCallBackInfo->m_abstime += milliseconds;

	pCallBackInfo->m_fnCallBack=fnCallBack;
    pCallBackInfo->m_pData=data;
    pCallBackInfo->m_pOrbiter=this;

	mapPendingCallbacks[pCallBackInfo->m_iCounter]=pCallBackInfo;

  
	cm.Release();

	pthread_cond_broadcast(&m_MaintThreadCond);
}

/*
MAINTENANCE CALL BACKS

When you need something to be done at a particular time,  like popup buttons,  call 'CallMaintenance',  passing in the time you need the function
called,  and any data you want the function to get,  as a void *.  The call backs must be void Function( void *data ) syntax.
*/

void Orbiter::DeselectObjects( void *data )
{
    NeedToRender render( this, "Deselect Objects" );  // Redraw anything that was changed by this command

    PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
	DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) data;
	 
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj!=pObj->m_pObj_Screen && !m_pActivePopup)
		return; // We must have since changed screens

#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING, "Deselecting %s object, state '%s'", pObj->m_ObjectID.c_str(), pObj->m_GraphicToDisplay==GRAPHIC_SELECTED ? "'selected'" : "'normal'");
#endif

    if(  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED  )
    {
        pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;

		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        m_vectObjs_NeedRedraw.push_back( pObj );
		nd.Release();

        // Remove it from the list
        for( vector<class DesignObj_Orbiter *>::iterator it=m_vectObjs_Selected.begin(  );it!=m_vectObjs_Selected.end(  );++it )
            //      for( size_t s=0;s<m_vectObjs_Selected.size(  );++s )
        {
            DesignObj_Orbiter *pObj_Sel = *it;
            if(  pObj == pObj_Sel  )//m_vectObjs_Selected[s]  )
            {
                m_vectObjs_Selected.erase( it );
                break;
            }
        }
    }
    else
    {
        int foo=2;
    }
}

void Orbiter::GetVideoFrame( void *data )
{
    PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )

	vector < class DesignObj_Orbiter * > vectObjs_VideoOnScreen; /** < All the video on screen */
	// Since this may take a while and we don't want to block the mutex the whole time, make a local copy

	size_t s;
    for(s=0;s<m_vectObjs_VideoOnScreen.size();++s)
		vectObjs_VideoOnScreen.push_back(m_vectObjs_VideoOnScreen[s]);

	vm.Release();
    for(s=0;s<vectObjs_VideoOnScreen.size();++s)
	{
		DesignObj_Orbiter *pObj = vectObjs_VideoOnScreen[s];
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter::GetVideoFrame() The target object is: %s", pObj->m_ObjectID.c_str());
#endif
		if( !pObj->m_bOnScreen )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Orbiter::GetVideoFrame() The target object si not on screen: %s", pObj->m_ObjectID.c_str());
#endif
			return; // The object isn't on screen anymore
		}

		// If it's hidden,  keep the timer going in case it becomes visible again
		if(  !pObj->IsHidden(  )  )
		{
			char *pBuffer=NULL; int Size=0;  string sFormat;
			DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame( m_dwPK_Device,  atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Source_CONST ).c_str(  ) ), "0",  0 /* stream */, pObj->Data.m_rBackgroundPosition.Width, pObj->Data.m_rBackgroundPosition.Height, &pBuffer, &Size, &sFormat );
			if(  SendCommand( CMD_Get_Video_Frame ) && pBuffer  )
			{
				CMD_Update_Object_Image( pObj->m_ObjectID,  sFormat ,  pBuffer,  Size, "1" );
                delete [] pBuffer; //we don't need it anymore
			}
            else
            {
                g_pPlutoLogger->Write(LV_WARNING, "CMD_Get_Video_Frame failed: the image buffer is empty");

                // Don't purge existing callbacks since there can be multiple frames on the screen
                CallMaintenanceInMiliseconds( m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
            }
		}
	}
	m_bAlreadyQueuedVideo=false;
    NeedToRender render( this, "GetVideoFrame" );  // Block this at the end so other renderings may occur in the meantime
}

/*

COMMANDS TO IMPLEMENT

*/

//<-dceag-c1-b->

	/** @brief COMMAND: #1 - Capture Keyboard To Variable */
	/** As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object. */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text Object */
		/** @param #4 PK_Variable */
			/** The variable in which to store the input */
		/** @param #8 On/Off */
			/** If 0, this stops capturing */
		/** @param #14 Type */
			/** 1=normal, 2=pin, 3=phone number */
		/** @param #24 Reset */
			/** if true, the next keypress will clear the variable and start new */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */
		/** @param #55 DataGrid */
			/** If 1, we'll scroll the data grid too when typing keys. */

void Orbiter::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
//<-dceag-c1-e->
{
    BuildCaptureKeyboardParams(
        sPK_DesignObj,
        iPK_Variable,
        sOnOff,
        sType,
        sReset,
        iPK_Text,
        bDataGrid
         );
}

//<-dceag-c2-b->

	/** @brief COMMAND: #2 - Orbiter Beep */
	/** Make the orbiter beep */

void Orbiter::CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage)
//<-dceag-c2-e->
{
    cout << "Need to implement command #2 - Orbiter Beep" << endl;
}

//<-dceag-c3-b->

	/** @brief COMMAND: #3 - Display On/Off */
	/** Turn the display on or off */
		/** @param #8 On/Off */
			/** 0=Off, 1=On */
		/** @param #125 Already processed */
			/** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

void Orbiter::CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage)
//<-dceag-c3-e->
{
	m_bDisplayOn = sOnOff=="1";
	if( bAlready_processed )
		return;

	if( m_bDisplayOn )
	{
		DCE::CMD_On CMD_On(m_dwPK_Device,m_dwPK_Device,0,"");
		SendCommand(CMD_On);
	}
	else
	{
		DCE::CMD_Off CMD_Off(m_dwPK_Device,m_dwPK_Device,0);
		SendCommand(CMD_Off);
	}

}

//<-dceag-c4-b->

	/** @brief COMMAND: #4 - Go back */
	/** Make the orbiter go back to the prior screen, like the back button in a web browser */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen */
		/** @param #21 Force */
			/** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

void Orbiter::CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c4-e->
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Go Back currently: %s  cs: %s",this->m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(),sPK_DesignObj_CurrentScreen.c_str());
#endif
	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
    ScreenHistory *pScreenHistory=NULL;
    while(  m_listScreenHistory.size(  )  )
    {
DumpScreenHistory();
        // The last screen we went to
        pScreenHistory = m_listScreenHistory.back(  );

        // We now took the prior screen off teh list
        m_listScreenHistory.pop_back(  );
        if( pScreenHistory->m_bCantGoBack && sForce!="1"  )
            continue;

        if( m_pScreenHistory_Current && pScreenHistory->m_pObj==m_pScreenHistory_Current->m_pObj && !pScreenHistory->m_pObj->Data.m_bCanGoBackToSameScreen  )
            continue;

		break;   // We got one to go back to
    }

    // todo hack -- handle restoring variables,  etc. pScreenHistory

    // We've got a screen to go back to
    if(  pScreenHistory  )
	{
		// If we stored variables, be sure to restore them
		for(VariableMap::iterator it=pScreenHistory->m_mapVariable.begin();it!=pScreenHistory->m_mapVariable.end();++it)
			m_mapVariable[ (*it).first ] = (*it).second;

		vm.Release();

		//hack! :(
		if( pScreenHistory->m_pObj->Data.m_iBaseObjectID == atoi(m_sMainMenu.c_str()) )
		{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Forcing go back to the main menu");
#endif
			GotoScreen( m_sMainMenu );
		}
		else
			NeedToRender::NeedToChangeScreens( this, pScreenHistory, false );

	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Got a go back, but no screen to return to");
}

//<-dceag-c5-b->

	/** @brief COMMAND: #5 - Goto Screen */
	/** Goto a specific screen */
		/** @param #2 PK_Device */
			/** For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset */
		/** @param #3 PK_DesignObj */
			/** The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen # */
		/** @param #10 ID */
			/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use). */
		/** @param #22 Store Variables */
			/** If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen */
		/** @param #114 Cant Go Back */
			/** If true, then when this screen goes away the user won't be able to return to it -- it will be skipped over, unless Go Back with Force=1 is used.  This prevents layers of popup screens. */

void Orbiter::CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c5-e->
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"CMD_Goto_Screen: %s",sPK_DesignObj.c_str());
#endif
    PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );  // Nothing more can happen

	// We're using a popup remote, so just go to the main menu
	if( sPK_DesignObj=="<%=NP_R%>" && m_iPK_DesignObj_Remote_Popup>0 && m_sObj_Popop_RemoteControl.size() )
		sPK_DesignObj = "<%=M%>"; 
	else if( sPK_DesignObj=="**NOMEDIA**" )
	{
		sPK_DesignObj = StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST);
		CMD_Set_Text(sPK_DesignObj, m_mapTextString[TEXT_No_Media_CONST], TEXT_STATUS_CONST);
	}

    HidePopups(NULL);

	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;
    string sDestScreen = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
    PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

    DesignObj_Orbiter *pObj_New=m_ScreenMap_Find( sDestScreen );
    if(  !pObj_New  )
		pObj_New=m_ScreenMap_Find( StringUtils::itos(atoi(sDestScreen.c_str())) + ".0.0" );

	if ( !pObj_New )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Could not find design object matching this specification: %s", sDestScreen.c_str());
		return;
	}

	if( pObj_New->Data.m_iBaseObjectID == atoi(m_sMainMenu.c_str()) )
	{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Forcing go to the main menu");
#endif
		pObj_New=m_ScreenMap_Find( m_sMainMenu );
	}

	if(  !pObj_New  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "cannot find screen %s in goto", sPK_DesignObj.c_str(  ) );
		// Just go to the main menu since maybe we're stuck!
		if( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj==m_pDesignObj_Orbiter_MainMenu )
	        return;
		if( !m_pScreenHistory_Current )
			return;

		pObj_New = m_pScreenHistory_Current->m_pObj;
    }

	//hack! if the simulator is running, we won't go to pluto admin screen
	if(Simulator::GetInstance()->IsRunning() && (pObj_New->Data.m_iBaseObjectID==DESIGNOBJ_mnuAdvancedOptions_CONST || pObj_New->Data.m_iBaseObjectID==DESIGNOBJ_mnuDisplayPower_CONST) )
		return;

    // We're going to change screens,  create the new ScreenHistory object
    ScreenHistory *pScreenHistory_New = new ScreenHistory( pObj_New, m_pScreenHistory_Current );
    // See if we're going to control a new device,  or should stick with the one we're now controlling
    pScreenHistory_New->m_sID=sID;
	pScreenHistory_New->m_bCantGoBack = bCant_Go_Back ? true : pObj_New->Data.m_bCantGoBack;
    vm.Release(  );

    // See if we need to store the variables on this screen,  so we restore them in case of a go back
    if(  bStore_Variables  )
        CMD_Store_Variables(  );
	NeedToRender::NeedToChangeScreens( this, pScreenHistory_New );
}

//<-dceag-c6-b->

	/** @brief COMMAND: #6 - Show Object */
	/** Change an objects visible state. */
		/** @param #3 PK_DesignObj */
			/** The object to show or hide */
		/** @param #4 PK_Variable */
			/** The variable to use in the comparisson.  See Comparisson Value. */
		/** @param #6 Comparisson Operator */
			/** A type of comparisson: =  <  <>  !=  > */
		/** @param #7 Comparisson Value */
			/** If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true */
		/** @param #8 On/Off */
			/** 1=show object, 2=hide object */

void Orbiter::CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c6-e->
{
    if(  iPK_Variable ||
        sComparisson_Operator.length(  )==0 ||
        sComparisson_Value.length(  )==0 ||
        DoComparisson( iPK_Variable, sComparisson_Operator, sComparisson_Value )  )
    {
        sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
        DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
        if(  !pObj  )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str(  ) );
            return;
        }
        bool bShow=sOnOff!="0";

        if(  sOnOff.substr( 0, 1 )=="!"  )
        {
            string s = SubstituteVariables( sOnOff.substr( 1 ), NULL, 0, 0 );
            if(  s=="" || s=="-1"  )
                bShow=true;
            else
                bShow=false;
        }

        //PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
        if(  m_pObj_Highlighted==pObj && !bShow  )
			m_pObj_Highlighted=NULL;

        pObj->m_bHidden = !bShow;
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "Object: %s visible: %d", pObj->m_ObjectID.c_str(), (int) pObj->m_bHidden );
#endif
		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	    m_vectObjs_NeedRedraw.push_back( pObj );  // Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( pObj->m_bHidden && pObj->m_pParentObject )
			m_vectObjs_NeedRedraw.push_back( (DesignObj_Orbiter *) pObj->m_pParentObject  );
		nd.Release();

    }
#ifdef DEBUG
	else
g_pPlutoLogger->Write( LV_STATUS, "Ignoring show object for %s",sPK_DesignObj.c_str() );
#endif
}

//<-dceag-c7-b->

	/** @brief COMMAND: #7 - Terminate Orbiter */
	/** Causes the Orbiter application to exit */

void Orbiter::CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage)
//<-dceag-c7-e->
{
    m_bQuit = true;
}

//<-dceag-c8-b->

	/** @brief COMMAND: #8 - Remove Screen From History */
	/** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
		/** @param #3 PK_DesignObj */
			/** The screen to remove */
		/** @param #10 ID */
			/** If specified, only screens that match this ID will be removed */

void Orbiter::CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c8-e->
{
    PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"CMD_Remove_Screen_From_History %s - %s size: %d",sPK_DesignObj.c_str(),sID.c_str(),(int) m_listScreenHistory.size());
DumpScreenHistory();
#endif

	for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();)
	{
		ScreenHistory *pScreenHistory = *it;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Comparing %s - %s",pScreenHistory->m_pObj->m_ObjectID.c_str(),pScreenHistory->m_sID.c_str());
#endif
		if( pScreenHistory->m_pObj->m_ObjectID.find(sPK_DesignObj+".")==0 && (sID.length()==0 || sID==pScreenHistory->m_sID) )
		{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"deleting %s - %s",pScreenHistory->m_pObj->m_ObjectID.c_str(),pScreenHistory->m_sID.c_str());
#endif
			delete (*it);
			it = m_listScreenHistory.erase( it );
		}
		else
			++it;
	}
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj->m_ObjectID.find(sPK_DesignObj+".")==0 && (sID.length()==0 || sID==m_pScreenHistory_Current->m_sID) )
		CMD_Go_back("","");
}

//<-dceag-c9-b->

	/** @brief COMMAND: #9 - Scroll Grid */
	/** Scroll a datagrid */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to scroll the grid */

void Orbiter::CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c9-e->
{
    PLUTO_SAFETY_LOCK( dng, m_DatagridMutex );
    // todo 2.0?    NeedsUpdate( 2 ); // Moving grids is slow; take care of an animation if necessary

    DesignObj_Orbiter *pObj=NULL;

    int LoopNum=-1;
    if ( sPK_DesignObj.empty(  ) )
        LoopNum=0;
    else
        pObj = FindObject( sPK_DesignObj );

	DesignObj_DataGrid *pObj_Datagrid = NULL;
	if( pObj && pObj->Data.m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		pObj_Datagrid = (DesignObj_DataGrid *) pObj;
	else if( pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Trying to scroll a non-grid %s",sPK_DesignObj.c_str());
		return;
	}

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
    while( LoopNum<( int ) m_vectObjs_GridsOnScreen.size(  ) )
    {
        if(  LoopNum!=-1  )
        {
            pObj_Datagrid = m_vectObjs_GridsOnScreen[LoopNum++];
        }

        if ( pObj_Datagrid && pObj_Datagrid->m_pDataGridTable )
        {
			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
            m_vectObjs_NeedRedraw.push_back( pObj_Datagrid );
			nd.Release();

            if(  sRelative_Level=="-1"  )
            {
                if(  iPK_Direction == DIRECTION_Up_CONST  )
                    pObj_Datagrid->m_GridCurRow = 0;
            }
            else
            {
                if(  iPK_Direction == DIRECTION_Up_CONST && pObj_Datagrid->m_GridCurRow>0  )
                {
                    CalculateGridUp( ( DesignObj_DataGrid * )pObj_Datagrid,  pObj_Datagrid->m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) );
                    ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Up_CONST]=NULL;
                    delete pObj_Datagrid->m_pDataGridTable;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable set to up cache",pObj_Datagrid->m_ObjectID.c_str());
#endif
                    pObj_Datagrid->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Up_CONST];
					pObj_Datagrid->bReAcquire=true;
                }
                else if(  iPK_Direction == DIRECTION_Down_CONST  )
                {
                    CalculateGridDown( ( DesignObj_DataGrid * )pObj_Datagrid,  pObj_Datagrid->m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) );
                    ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Down_CONST]=NULL;
                    delete pObj_Datagrid->m_pDataGridTable;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable set to down cache",pObj_Datagrid->m_ObjectID.c_str());
#endif
                    pObj_Datagrid->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Down_CONST];
					pObj_Datagrid->bReAcquire=true;
                }
                else if(  iPK_Direction == DIRECTION_Left_CONST  )
                {
                    CalculateGridLeft( ( DesignObj_DataGrid * )pObj_Datagrid,  pObj_Datagrid->m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) );
                    ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Left_CONST]=NULL;
                    delete pObj_Datagrid->m_pDataGridTable;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable set to left cache",pObj_Datagrid->m_ObjectID.c_str());
#endif
                    pObj_Datagrid->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Left_CONST];
					pObj_Datagrid->bReAcquire=true;
                }
                else if(  iPK_Direction == DIRECTION_Right_CONST  )
                {
                    CalculateGridRight( ( DesignObj_DataGrid * )pObj_Datagrid,  pObj_Datagrid->m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) );
                    ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Right_CONST]=NULL;
                    delete pObj_Datagrid->m_pDataGridTable;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable set to right cache",pObj_Datagrid->m_ObjectID.c_str());
#endif
                    pObj_Datagrid->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj_Datagrid )->m_pDataGridTableCache[DIRECTION_Right_CONST];
					pObj_Datagrid->bReAcquire=true;
                }
            }
        }
        if(  LoopNum==-1  )
            break;
    }
}

//<-dceag-c10-b->

	/** @brief COMMAND: #10 - Move Highlight */
	/** Move the current highlight pointer */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to move the highlight */

void Orbiter::CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c10-e->
{
    cout << "Need to implement command #10 - Move Highlight" << endl;
    cout << "Parm #1 - Relative_Level=" << sRelative_Level << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #30 - PK_Direction=" << iPK_Direction << endl;
}

///<-dceag-c13-b->

	/** @brief COMMAND: #13 - Play Sound */
	/** Plays a sound file on the orbiter */
		/** @param #19 Data */
			/** A pointer to a block of memory representing the sound file to play */
		/** @param #20 Format */
			/** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

void Orbiter::CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c13-e->
{
    cout << "Need to implement command #13 - Play Sound" << endl;
    cout << "Parm #19 - Data  ( data value )" << endl;
    cout << "Parm #20 - Format=" << sFormat << endl;
}

//<-dceag-c14-b->

	/** @brief COMMAND: #14 - Refresh */
	/** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

void Orbiter::CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c14-e->
{
	if( sDataGrid_ID.length()==0 )
	    m_bRerenderScreen = true;

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	vector<DesignObj_DataGrid*>::iterator it;
	for(it = m_vectObjs_GridsOnScreen.begin(); it != m_vectObjs_GridsOnScreen.end(); ++it)
	{
		DesignObj_DataGrid* pDesignObj = *it;

		if(sDataGrid_ID=="*" || pDesignObj->m_sGridID == sDataGrid_ID)
		{
			pDesignObj->bReAcquire=true;

			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
			m_vectObjs_NeedRedraw.push_back(pDesignObj);
			nd.Release();
		}
	}

    NeedToRender render( this, "CMD_Refresh" );  // Redraw anything that was changed by this command
}

//<-dceag-c15-b->

	/** @brief COMMAND: #15 - Regen Screen */
	/** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

void Orbiter::CMD_Regen_Screen(string &sCMD_Result,Message *pMessage)
//<-dceag-c15-e->
{
	if( m_pScreenHistory_Current )
		NeedToRender::NeedToChangeScreens( this, m_pScreenHistory_Current );
}

//<-dceag-c16-b->

	/** @brief COMMAND: #16 - Requires Special Handling */
	/** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

void Orbiter::CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage)
//<-dceag-c16-e->
{
    cout << "Need to implement command #16 - Requires Special Handling" << endl;
}

//<-dceag-c17-b->

	/** @brief COMMAND: #17 - Seek Data Grid */
	/** Causes a datagrid to seek to a particular position. */
		/** @param #9 Text */
			/** If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one. */
		/** @param #11 Position X */
			/** The column to jump to.  If Text is not blank, the column to search. */
		/** @param #12 Position Y */
			/** The row to jump to.  Ignored if Text is not blank */
		/** @param #15 DataGrid ID */
			/** The datagrid to scroll.  If not specified, the first visible one will be used */

void Orbiter::CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c17-e->
{
    DesignObj_DataGrid *pObj_Datagrid=NULL;
    if( sDataGrid_ID.length() )
        pObj_Datagrid = m_mapObjs_AllGrids[sDataGrid_ID];
    else if( m_vectObjs_GridsOnScreen.size() )
        pObj_Datagrid = m_vectObjs_GridsOnScreen[0];

    if( !pObj_Datagrid )
        g_pPlutoLogger->Write(LV_CRITICAL,"Cannot seek to unknown grid");
    else
    {
		pObj_Datagrid->bReAcquire = true;
        pObj_Datagrid->m_sSeek = sText;
        CMD_Refresh(pObj_Datagrid->m_sGridID);
    }
}

//<-dceag-c18-b->

	/** @brief COMMAND: #18 - Set Graphic To Display */
	/** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
		/** @param #3 PK_DesignObj */
			/** The object to set */
		/** @param #10 ID */
			/** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

void Orbiter::CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c18-e->
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
    DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
    if(  !pObj  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str(  ) );
        return;
    }
    pObj->m_GraphicToDisplay=atoi( sID.c_str(  ) );
    if(  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED  )
    {
        SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0) );  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
        m_vectObjs_Selected.push_back( pObj );
    }

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	m_vectObjs_NeedRedraw.push_back(pObj);
	nd.Release();
}

//<-dceag-c20-b->

	/** @brief COMMAND: #20 - Set Object Parameter */
	/** changes one of the object's DesignObjParameters */
		/** @param #3 PK_DesignObj */
			/** The object to change */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #27 PK_DesignObjParameter */
			/** The parameter */

void Orbiter::CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage)
//<-dceag-c20-e->
{
    cout << "Need to implement command #20 - Set Object Parameter" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #27 - PK_DesignObjParameter=" << iPK_DesignObjParameter << endl;
}

//<-dceag-c21-b->

	/** @brief COMMAND: #21 - Set Object Position */
	/** Change an objects's position on the screen */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c21-e->
{
    cout << "Need to implement command #21 - Set Object Position" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c22-b->

	/** @brief COMMAND: #22 - Set Object Size */
	/** Change an object's size */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c22-e->
{
    cout << "Need to implement command #22 - Set Object Size" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c23-b->

	/** @brief COMMAND: #23 - Set Pos Rel To Parent */
	/** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c23-e->
{
    cout << "Need to implement command #23 - Set Pos Rel To Parent" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c24-b->

	/** @brief COMMAND: #24 - Set Size Rel To Parent */
	/** Change an object's size, relative to it's parent object */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/** The percentage of the parent object's width.  100=the parent's full width. */
		/** @param #12 Position Y */
			/** The percentage of the parent object's height.  100=the parent's full height. */

void Orbiter::CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c24-e->
{
    cout << "Need to implement command #24 - Set Size Rel To Parent" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c25-b->

	/** @brief COMMAND: #25 - Set Text */
	/** Change the text within a text object on the fly */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #9 Text */
			/** The text to assign */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */

void Orbiter::CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage)
//<-dceag-c25-e->
{
    DesignObj_Orbiter *pObj=( m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj : NULL );
    if(  sPK_DesignObj.length(  )  )
        pObj = FindObject( sPK_DesignObj );

	if( !pObj )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "SetText: cannot find text object %d", iPK_Text );
		return;
	}

    DesignObjText *pText = FindText( pObj, iPK_Text );
    if(  !pText  )
        g_pPlutoLogger->Write( LV_CRITICAL, "SetText: cannot find object %s text %d", sPK_DesignObj.c_str(  ), iPK_Text );
    else
        pText->m_sText = sText;

    if(  pObj->m_bOnScreen  )
	{
		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
        m_vectObjs_NeedRedraw.push_back( pObj );
	}
}

//<-dceag-c26-b->

	/** @brief COMMAND: #26 - Set Bound Icon */
	/** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
		/** @param #5 Value To Assign */
			/** The value corresponding to an alt graphic. */
		/** @param #14 Type */
			/** The type of bound icon. */

void Orbiter::CMD_Set_Bound_Icon(string sValue_To_Assign,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c26-e->
{
	DesignObj_DataList *pDesignObj_DataList = m_mapObj_Bound_Find(sType);
	if( !pDesignObj_DataList )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got SetBoundIcon for unknown type: %s",sType.c_str());
		return;
	}

	int iValue = atoi(sValue_To_Assign.c_str());
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	for(DesignObj_DataList::iterator it=pDesignObj_DataList->begin();it!=pDesignObj_DataList->end();++it)
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *it;
		pObj->Data.m_bDontResetState=true;
		pObj->m_GraphicToDisplay = iValue;
		if( pObj->m_bOnScreen )
		{
			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
			m_vectObjs_NeedRedraw.push_back(pObj);
		}
	}
}

//<-dceag-c27-b->

	/** @brief COMMAND: #27 - Set Variable */
	/** Change the value of a variable */
		/** @param #4 PK_Variable */
			/** The variable to change */
		/** @param #5 Value To Assign */
			/** The value to assign */

void Orbiter::CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c27-e->
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Variable: %d set to %s",iPK_Variable,sValue_To_Assign.c_str());
#endif
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    m_mapVariable[iPK_Variable] = sValue_To_Assign;
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Orbiter::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    ButtonDown( atoi(sPK_Button.c_str()) ); // TODO: Handle shift and send second digit if shit is down
    ButtonUp( atoi(sPK_Button.c_str()) ); // TODO: Handle shift and send second digit if shit is down
}

//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simulates a mouse click or touch on the indicated x & y coordinates */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
    cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c30-b->

	/** @brief COMMAND: #30 - Stop Sound */
	/** If a sound file is being played, it will be stopped. */

void Orbiter::CMD_Stop_Sound(string &sCMD_Result,Message *pMessage)
//<-dceag-c30-e->
{
    cout << "Need to implement command #30 - Stop Sound" << endl;
}

//<-dceag-c31-b->

	/** @brief COMMAND: #31 - Store Variables */
	/** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

void Orbiter::CMD_Store_Variables(string &sCMD_Result,Message *pMessage)
//<-dceag-c31-e->
{
	if( !m_pScreenHistory_Current )
		return;

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	for(VariableMap::iterator it=m_mapVariable.begin();it!=m_mapVariable.end();++it)
	{
		m_pScreenHistory_Current->m_mapVariable[ (*it).first ] = (*it).second;
	}
}

//<-dceag-c32-b->

	/** @brief COMMAND: #32 - Update Object Image */
	/** Changes the background image within an object */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

void Orbiter::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // We don't want to allow this while we're rendering a screen
    DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
    if(  !pObj  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got update object image but cannot find: %s", sPK_DesignObj.c_str(  ) );
        return;
    }

    int PriorWidth=0, PriorHeight=0;
    if(  pObj->m_pvectCurrentGraphic  )
        pObj->m_pvectCurrentGraphic = NULL;
    if ( pObj->m_vectGraphic.size() )
    {
        PriorWidth = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Width;
        PriorHeight = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Height;
    }

	for(size_t i = 0; i < pObj->m_vectGraphic.size(); i++)
	{
		delete pObj->m_vectGraphic[i];
	}
	pObj->m_vectGraphic.clear();
	pObj->m_iCurrentFrame = 0;

    if(  iData_Size==0  )
    {
        //pObj->m_pGraphic=NULL;*
        return;
    }

	PlutoGraphic *pPlutoGraphic = CreateGraphic();

	if(sType == "bmp")
		pPlutoGraphic->m_GraphicFormat = GR_BMP;
	else if(sType == "jpg")
		pPlutoGraphic->m_GraphicFormat = GR_JPG;
	else if(sType == "png")
		pPlutoGraphic->m_GraphicFormat = GR_PNG;
	else
		pPlutoGraphic->m_GraphicFormat = GR_UNKNOWN;

	pPlutoGraphic->LoadGraphic(pData, iData_Size, m_iRotation);  // These weren't pre-rotated
	pObj->m_vectGraphic.push_back(pPlutoGraphic);
	pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

    if (  sDisable_Aspect_Lock.length(  )  )
        pObj->m_bDisableAspectLock = ( sDisable_Aspect_Lock=="1" ) ? true : false;

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
    m_vectObjs_NeedRedraw.push_back( pObj );
}

void Orbiter::DelayedSelectObject( void *data )
{
	DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) data;
	if( TestCurrentScreen(pDelayedSelectObjectInfo->m_sPK_DesignObj_CurrentScreen) )
		SelectedObject(pDelayedSelectObjectInfo->m_pObj);

	delete pDelayedSelectObjectInfo;
}

//<-dceag-c66-b->

	/** @brief COMMAND: #66 - Select Object */
	/** The same as clicking on an object. */
		/** @param #3 PK_DesignObj */
			/** The object to select. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** Will only happen if this is the current screen. */
		/** @param #102 Time */
			/** If specified, rather than happening immediately it will happen in x seconds. */

void Orbiter::CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c66-e->
{
    DesignObj_Orbiter *pDesignObj_Orbiter = FindObject( sPK_DesignObj );
    if(  !pDesignObj_Orbiter  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Got select object %s,  but can't find it", sPK_DesignObj.c_str(  ) );
        return;
    }

	if( sTime.length()==0 && !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	if( sTime.length()!=0 && atoi(sTime.c_str()) )
	{
		timespec ts;
		gettimeofday(&ts,NULL);
		long TimeInMS = atoi(sTime.c_str()) * 1000;
		ts += TimeInMS;
		// We don't want to purge all select objects.  Only those pending for the same object.  So we'll have to do this by hand
		DelayedSelectObjectInfo *pDelayedSelectObjectInfo = new DelayedSelectObjectInfo(pDesignObj_Orbiter,sPK_DesignObj_CurrentScreen,ts);

	    PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
		for(map<int,CallBackInfo *>::iterator it=mapPendingCallbacks.begin();it!=mapPendingCallbacks.end();++it)
		{
			CallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack== &Orbiter::DelayedSelectObject )
			{
				DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) pCallBackInfo->m_pData;
				if( pDelayedSelectObjectInfo->m_pObj==pDesignObj_Orbiter )
					pCallBackInfo->m_bStop=true;
			}
		}
		cm.Release();

		CallMaintenanceInMiliseconds( TimeInMS, &Orbiter::DelayedSelectObject, pDelayedSelectObjectInfo, pe_NO );
	}
	else
	    SelectedObject( pDesignObj_Orbiter );
}

//<-dceag-c72-b->

	/** @brief COMMAND: #72 - Surrender to OS */
	/** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
		/** @param #8 On/Off */
			/** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
		/** @param #54 Fully release keyboard */
			/** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys. */

void Orbiter::CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,string &sCMD_Result,Message *pMessage)
//<-dceag-c72-e->
{
    m_bYieldScreen = ( sOnOff=="1" );
    m_bYieldInput = bFully_release_keyboard;

    // this should not be here the handler for the DesktopApplication Design Obj will render the desktop with the proper parameters.
/*
    if(  m_bYieldScreen  )
        RenderDesktop( NULL, PlutoRectangle( 0, 0, -1, -1 ) );  // Full screen
    else
        RenderDesktop( NULL, PlutoRectangle( 0, 0, 0, 0 ) );
*/
}


// Radu -- implemented scrolling into Orbiter -- Up/Down for now
//------------------------------------------------------------------------------------------------------------
void Orbiter::CalculateGridUp( DesignObj_DataGrid *pObj,  int &CurRow,  int CellsToSkip )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

    if ( CellsToSkip == 0 )
        CellsToSkip = pObj->m_MaxRow - ( pObj->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

	// Are we going to display 'scroll up/down' cells?  If so, we may not be able to scroll up a full page
    if(  pObj->m_sExtraInfo.find( 'P' )!=string::npos  )
	{
		// After doing the scroll, will we still be able to go up?  If so, reduce the cells to skip by 1
		if( CurRow - CellsToSkip > 0)
			CellsToSkip--;
		// After doing the scroll, will we still be able to go down?  If so, reduce the cells to skip by 1
		if( pObj->m_pDataGridTable->GetRows() > (CurRow - CellsToSkip) + pObj->m_MaxRow )
			CellsToSkip--;
	}

    if ( CellsToSkip < 0 )
        CellsToSkip = 0;

    CurRow -= CellsToSkip;
    if ( CurRow <= 1 )
        CurRow = 0;
}
//------------------------------------------------------------------------------------------------------------
void Orbiter::CalculateGridDown( DesignObj_DataGrid *pObj,  int &CurRow,  int CellsToSkip )
{
    PLUTO_SAFETY_LOCK( dg,  m_DatagridMutex );

    if ( !pObj->CanGoDown() && CellsToSkip == 0 )
        return;

    if ( CellsToSkip == 0 )
        CellsToSkip = pObj->m_MaxRow - ( pObj->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

    bool bCanGoDown = pObj->m_dwIDownRow >= 0;
    bool bCanGoUp = pObj->m_iUpRow >= 0;
    CellsToSkip -= ( bCanGoDown + bCanGoUp );

    if ( CellsToSkip < 0 )
        CellsToSkip = 0;

    CurRow += CellsToSkip;

    if ( CurRow+pObj->m_MaxRow > ( pObj->m_pDataGridTable )->GetRows(  ) )
    {
        // Add an extra 1,  becuase if we end  up making the top row an up ( unknown at this point )
        // We'll be one short
        CurRow = pObj->m_pDataGridTable->GetRows(  ) - pObj->m_MaxRow + 1;
        if ( CurRow<0 )
            CurRow=0;
    }
}
//------------------------------------------------------------------------------------------------------------
void Orbiter::CalculateGridLeft( DesignObj_DataGrid *pObj,  int &CurCol,  int CellsToSkip )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

	if ( !pObj->CanGoRight() && CellsToSkip == 0 )
        return;

	if ( CellsToSkip==0 )
        CellsToSkip = pObj->m_MaxCol-( pObj->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
    if ( CellsToSkip<=0 )
        CellsToSkip = 1;

    CurCol-=CellsToSkip;
    if ( CurCol<0 )
        CurCol=0;
}
//------------------------------------------------------------------------------------------------------------
void Orbiter::CalculateGridRight( DesignObj_DataGrid *pObj,  int &CurCol,  int CellsToSkip )
{
    PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
    if ( CellsToSkip==0 )
        CellsToSkip = pObj->m_MaxCol-( pObj->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
    if ( CellsToSkip<=0 )
        CellsToSkip = 1;

    CurCol+=CellsToSkip;

    if ( CurCol+pObj->m_MaxCol > ( pObj->m_pDataGridTable )->GetCols(  ) )
    {
        CurCol = ( pObj->m_pDataGridTable )->GetCols(  ) - pObj->m_MaxCol;
        if ( CurCol<0 )
            CurCol=0;
    }
}
//------------------------------------------------------------------------------------------------------------
DesignObjText *Orbiter::FindText( DesignObj_Orbiter *pObj,  int iPK_Text )
{
    DesignObjText *pText = NULL;
    vector<DesignObjText *>::iterator it_text;
    for( it_text = pObj->m_vectDesignObjText.begin(  );
        it_text != pObj->m_vectDesignObjText.end(  );
        it_text++
         )
    {
        DesignObjText *pText_Temp = *it_text;

        if( pText_Temp->Data.m_PK_Text == iPK_Text )
        {
            pText = pText_Temp;
            break;
        }
    }

    if(  pText  )
        return pText;

    // Otherwise,  search all our children
    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
    {
        pText = FindText( ( DesignObj_Orbiter * )*iHao, iPK_Text );
        if(  pText  )
            return pText;
    }


    g_pPlutoLogger->Write(LV_WARNING, "Could not find text object: %d", iPK_Text);
    return NULL;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid )
{
    //on/off parameter
    m_bCaptureKeyboard_OnOff = sOnOff == "0" ? false : true;

    //normal/pin paramter
    m_iCaptureKeyboard_EditType = atoi(sType.c_str());

    //reset parameter
    m_bCaptureKeyboard_Reset = sReset != "0";

    //data grid
    m_bCaptureKeyboard_DataGrid = bDataGrid;

    //set var
    m_iCaptureKeyboard_PK_Variable = iPK_Variable;

    if( iPK_Variable && m_bCaptureKeyboard_Reset )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		m_mapVariable[iPK_Variable] = "";
	}

    //find the parent object
    DesignObj_Orbiter *pObj;
    if(  sPK_DesignObj.length(  ) > 0  )
        pObj = FindObject( sPK_DesignObj );
    else
        if( NULL == m_pScreenHistory_Current )
        {
#ifdef DEBUG
            g_pPlutoLogger->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: m_pScreenHistory_Current is null!" );
#endif
            return true;
        }
        else
            pObj = m_pScreenHistory_Current->m_pObj;

    
    if(!pObj)
    {
#ifdef DEBUG
        g_pPlutoLogger->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: pObj is null!" );
#endif
        return true;
    }

    //find the text object
    m_pCaptureKeyboard_Text = FindText( pObj,  iPK_Text );

    if( iPK_Variable && NULL != m_pCaptureKeyboard_Text && m_bCaptureKeyboard_Reset )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
        m_pCaptureKeyboard_Text->m_sText = m_mapVariable[iPK_Variable];
	}

    //build the text string
    m_sCaptureKeyboard_Text = "";
    if( NULL == m_pCaptureKeyboard_Text )
    {
#ifdef DEBUG
        g_pPlutoLogger->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: text object is null!" );
#endif
        return true;
    }
    else
    {
	    m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->Data.m_rPosition.X ) + ",";
        m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->Data.m_rPosition.Y ) + ",";
        m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->Data.m_rPosition.Width ) + ",";
        m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->Data.m_rPosition.Height );
    }

    return true;
}

void Orbiter::GetButtonsInObject( DesignObj_Data *pObj, vector<int> &vectButtons )
{
    if( pObj->Data.m_iPK_Button )
        vectButtons.push_back(pObj->Data.m_iPK_Button);

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
        GetButtonsInObject(*iHao,vectButtons);
}

//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_DeleteLastChar(  )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    string OldValue = m_mapVariable[m_iCaptureKeyboard_PK_Variable];
    string NewValue = "";

    if( OldValue.size(  ) )
        NewValue = OldValue.substr( 0,  OldValue.size(  ) - 1 );

    m_sCaptureKeyboard_InternalBuffer = NewValue;
    m_mapVariable[m_iCaptureKeyboard_PK_Variable] = NewValue;

    if( NULL != m_pCaptureKeyboard_Text )
    {
        m_pCaptureKeyboard_Text->m_sText = NewValue;

		if( NULL != m_pCaptureKeyboard_Text )
		{
			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
			m_vectTexts_NeedRedraw.push_back( m_pCaptureKeyboard_Text );
		}
		else
			m_bRerenderScreen = true;

        return true;
    }

    return false;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_AppendChar( char ch )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    string OldValue = m_mapVariable[m_iCaptureKeyboard_PK_Variable];
    string NewValue = OldValue + ch;

    m_sCaptureKeyboard_InternalBuffer = NewValue;
    m_mapVariable[m_iCaptureKeyboard_PK_Variable] = NewValue;

    if( NULL != m_pCaptureKeyboard_Text )
    {
        m_pCaptureKeyboard_Text->m_sText = NewValue;

		if( NULL != m_pCaptureKeyboard_Text )
		{
			PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
			m_vectTexts_NeedRedraw.push_back( m_pCaptureKeyboard_Text );
		}
		else
			m_bRerenderScreen = true;
    }

    return false;
}

//<-dceag-sample-b->!
//<-dceag-c85-b->

	/** @brief COMMAND: #85 - Reset Highlight */
	/** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */

void Orbiter::CMD_Reset_Highlight(string &sCMD_Result,Message *pMessage)
//<-dceag-c85-e->
{
    HighlightFirstObject(  );
}

bool Orbiter::ReceivedMessage( class Message *pMessageOriginal )
{
	while( !m_pScreenHistory_Current && !m_bQuit )
		Sleep(100);

	if(m_bQuit)
		return false;

	string strMessage = string("ReceivedMessage: ") + StringUtils::itos(pMessageOriginal->m_dwID);
    NeedToRender render( this, strMessage.c_str() );  // Redraw anything that was changed by this command
    bool bResult = Orbiter_Command::ReceivedMessage( pMessageOriginal );
    return bResult;
}

void Orbiter::CaptureKeyboard_UpdateVariableAndText( int iVariable,  string sVariable ) //for capture keyboard cmd
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
    m_mapVariable[iVariable] = sVariable;
}

//<-dceag-c58-b->

	/** @brief COMMAND: #58 - Set Current User */
	/** Sets what user is currently using the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

void Orbiter::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
    m_dwPK_Users=iPK_Users;
    DCE::CMD_Set_Current_User CMD_Set_Current_User( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_dwPK_Users );
    SendCommand( CMD_Set_Current_User );
}

//<-dceag-c88-b->

	/** @brief COMMAND: #88 - Set Current Location */
	/** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
		/** @param #65 LocationID */
			/** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

void Orbiter::CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage)
//<-dceag-c88-e->
{
    if(  iLocationID<0 || iLocationID>=( int ) m_dequeLocation.size(  )  )
        g_pPlutoLogger->Write( LV_CRITICAL, "Got set current location with invalid location" );
    else
    {
        LocationInfo *pLocationInfo = m_dequeLocation[iLocationID];
        m_pLocationInfo = pLocationInfo;
		if( !m_bStartingUp )
		{
			DCE::CMD_Set_Entertainment_Area CMD_Set_Entertainment_Area( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(pLocationInfo->PK_EntertainArea) );
			SendCommand( CMD_Set_Entertainment_Area );
			DCE::CMD_Set_Current_Room CMD_Set_Current_Room( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, pLocationInfo->PK_Room );
			SendCommand( CMD_Set_Current_Room );
		}
        m_sMainMenu = StringUtils::itos( atoi( m_sMainMenu.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";

		string sMainMenu = StringUtils::itos( atoi( m_pDesignObj_Orbiter_MainMenu->m_ObjectID.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";
		DesignObj_Orbiter *pObj = FindObject( sMainMenu );
		if( !pObj )
			g_pPlutoLogger->Write(LV_CRITICAL,"Trying to change to non-existant main menu: %s",sMainMenu.c_str());
		else
			m_pDesignObj_Orbiter_MainMenu = pObj;

		string sSleepingMenu;
		if(m_pDesignObj_Orbiter_SleepingMenu)
			sSleepingMenu = StringUtils::itos( atoi( m_pDesignObj_Orbiter_SleepingMenu->m_ObjectID.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";

		pObj = FindObject( sSleepingMenu );
		if( !pObj )
			g_pPlutoLogger->Write(LV_CRITICAL,"Trying to change to non-existant sleeping menu: %s",sSleepingMenu.c_str());
		else
			m_pDesignObj_Orbiter_SleepingMenu = pObj;

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Set location %d ea %d rm %d (%s) now %s",iLocationID,pLocationInfo->PK_EntertainArea,
					  pLocationInfo->PK_Room,pLocationInfo->Description.c_str(),m_sMainMenu.c_str());
#endif
    }
}

void Orbiter::RenderFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen, PlutoPoint point)
{
    int Type = atoi(pDesignObj_Orbiter->GetParameterValue(DESIGNOBJPARAMETER_Type_CONST).c_str());

    string sResult;
	DCE::CMD_Get_Current_Floorplan CMD_Get_Current_Floorplan(m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(pDesignObj_Orbiter->Data.m_iPage), Type, &sResult);
    SendCommand(CMD_Get_Current_Floorplan);

    FloorplanObjectVectorMap *pFloorplanObjectVectorMap = m_mapFloorplanObjectVector_Find(pDesignObj_Orbiter->Data.m_iPage);
    FloorplanObjectVector *fpObjVector = NULL;

//  FloodFill(150,241, RGB(0,0,0), RGB(255,255,0));

    if( pFloorplanObjectVectorMap )
        fpObjVector = (*pFloorplanObjectVectorMap)[Type];

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Get current floorplan %d page %d returned %s",Type, pDesignObj_Orbiter->Data.m_iPage, sResult.c_str());
#endif

    if( fpObjVector )
    {
        string::size_type pos = 0;
        for(int i=0;i<(int) fpObjVector->size();++i)
        {
            FloorplanObject *fpObj = (*fpObjVector)[i];
			// Color is the color to fill the icon with, Description is the status which
			// appears at the bottom of the floorplan when the item is selected, OSD is
			// the text will be put into any text object within the icon (like the temperature
			// next to a thermastat, and PK_DesignObj_Toolbar is the toolbar to activate
			// when the object is selected.
            int Color = atoi(StringUtils::Tokenize(sResult,"|",pos).c_str());
            fpObj->Status = StringUtils::Tokenize(sResult,"|",pos);
            string OSD = StringUtils::Tokenize(sResult,"|",pos);
            fpObj->m_dwPK_DesignObj_Toolbar = atoi(StringUtils::Tokenize(sResult,"|",pos).c_str());

			if( fpObj->pObj )
            {
                if( fpObj->pObj->Data.m_rBackgroundPosition.X+fpObj->pObj->Data.m_rBackgroundPosition.Width>m_Width ||
                    fpObj->pObj->Data.m_rBackgroundPosition.Y+fpObj->pObj->Data.m_rBackgroundPosition.Height>m_Height )
                {
                    g_pPlutoLogger->Write(LV_CRITICAL,"Floorplan object %s is out of bounds ",fpObj->DeviceDescription.c_str());
                    continue;
                }

				if( Color )
				{
					PlutoColor Magenta(255,102,255);

#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS,"Replacing obj %s %d,%d-%d,%d with color %d",
						fpObj->pObj->m_ObjectID.c_str(),fpObj->pObj->Data.m_rBackgroundPosition.X,fpObj->pObj->Data.m_rBackgroundPosition.Y,fpObj->pObj->Data.m_rBackgroundPosition.Width,
						fpObj->pObj->Data.m_rBackgroundPosition.Height, (int) Color);
#endif
					ReplaceColorInRectangle(point.X + fpObj->pObj->Data.m_rBackgroundPosition.X, point.Y + fpObj->pObj->Data.m_rBackgroundPosition.Y,fpObj->pObj->Data.m_rBackgroundPosition.Width,
						fpObj->pObj->Data.m_rBackgroundPosition.Height, Magenta, Color);

				}

                if( fpObj->pObj->m_vectDesignObjText.size()==1 )
                {
                    DesignObjText *pDesignObjText = fpObj->pObj->m_vectDesignObjText[0];
                    pDesignObjText->m_sText = OSD;
                }
                //FloodFill(fpObj->pObj->Data.m_rectBackground.X + fpObj->FillX,fpObj->pObj->Data.m_rectBackground.Y + fpObj->FillY, RGB(0,0,0), Color);
            }
        }
    }
}

ScreenHistory *NeedToRender::m_pScreenHistory=NULL;
bool NeedToRender::m_bAddToHistory=true;

NeedToRender::NeedToRender( class Orbiter *pOrbiter, const char *pWhere )
{
/*
    if ( g_cLastTime && ( clock() - g_cLastTime ) > CLOCKS_PER_SEC * 3 && g_iDontRender )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Need to render has blocked!!!" );
        g_iDontRender=0;
    }
*/
    m_pWhere = pWhere;
    m_pOrbiter = pOrbiter;
    g_cLastTime = clock();
    g_iDontRender++;
}

void NeedToRender::NeedToChangeScreens( Orbiter *pOrbiter, ScreenHistory *pScreenHistory, bool bAddToHistory )
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Need to change screens logged to %s",pScreenHistory->m_pObj->m_ObjectID.c_str());
#endif
	m_pScreenHistory = pScreenHistory;
	m_bAddToHistory = bAddToHistory;

	//purge pending tasks, if need it.  Do it here, so that things will happen in the right order.
	//ie: CMD_GotoScreen, CMD_SelectObject. The real need to change screens won't get called until
	//after all messages are processed, wiping out CMD_SelectObject
	//Orbiter *pOrbiter = m_pOrbiter(Orbiter *) g_pCommand_Impl;
	if( pOrbiter )
	{
		PLUTO_SAFETY_LOCK( pm, pOrbiter->m_MaintThreadMutex );
		map<int,CallBackInfo *>::iterator itCallBackInfo;
		for(itCallBackInfo = mapPendingCallbacks.begin(); itCallBackInfo != mapPendingCallbacks.end();)
		{
			CallBackInfo *pCallBackInfo = (*itCallBackInfo).second;
			if(pCallBackInfo->m_bPurgeTaskWhenScreenIsChanged)
			{
				mapPendingCallbacks.erase(itCallBackInfo++);
				delete pCallBackInfo;
			}
			else
				itCallBackInfo++;
		}
		pm.Release();

		// We also want to handle reseting the selected states here for the same reason,
		// Otherwise we may have a show_object message come in and get processed afterward
		pOrbiter->ResetState(pScreenHistory->m_pObj);
	}
}

//<-dceag-c242-b->

	/** @brief COMMAND: #242 - Set Now Playing */
	/** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
		/** @param #2 PK_Device */
			/** The currently active media device */
		/** @param #3 PK_DesignObj */
			/** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote */
		/** @param #5 Value To Assign */
			/** The description of the media */
		/** @param #13 Filename */
			/** The default name to use if the user wants to rip this.  Only applies to discs. */
		/** @param #48 Value */
			/** The track number or position in the playlist */
		/** @param #120 Retransmit */
			/** If true, it will re-request the plist (current playlist) grid */

void Orbiter::CMD_Set_Now_Playing(int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sFilename,int iValue,bool bRetransmit,string &sCMD_Result,Message *pMessage)
//<-dceag-c242-e->
{
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	m_sNowPlaying = sValue_To_Assign;
	m_dwPK_Device_NowPlaying = iPK_Device;
	m_mapVariable[VARIABLE_Track_or_Playlist_Positio_CONST]=StringUtils::itos(iValue);
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"CMD_Set_Now_Playing %s %s",sValue_To_Assign.c_str(),sPK_DesignObj.c_str());
#endif
	string::size_type pos=0;
	m_iPK_DesignObj_Remote=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_Remote_Popup=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_FileList=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_FileList_Popup=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_RemoteOSD=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_sDefaultRippingName = sFilename;

	if( bRetransmit )
	{
		vector<DesignObj_DataGrid*>::iterator it;
		for(it = m_vectObjs_GridsOnScreen.begin(); it != m_vectObjs_GridsOnScreen.end(); ++it)
		{
			DesignObj_DataGrid* pDesignObj = *it;
			if(pDesignObj->m_sGridID.size()>6 && (pDesignObj->m_sGridID.substr(0,5)=="plist" || pDesignObj->m_sGridID.substr(0,6)=="tracks") )
			{
				InitializeGrid(pDesignObj);
				pDesignObj->bReAcquire=true;
			}
		}

	}
	DesignObj_Orbiter *pObj_Popop_RemoteControl=NULL;
	if( m_sObj_Popop_RemoteControl.size() && (pObj_Popop_RemoteControl=FindObject(m_sObj_Popop_RemoteControl)) && m_iPK_DesignObj_Remote_Popup>0 )
		CMD_Show_Popup(StringUtils::itos(m_iPK_DesignObj_Remote_Popup),m_Popop_RemoteControl_X,m_Popop_RemoteControl_Y,pObj_Popop_RemoteControl->m_ObjectID,"remote",false,false);
	else if( pObj_Popop_RemoteControl && m_iPK_DesignObj_Remote_Popup==0 )
		CMD_Remove_Popup(pObj_Popop_RemoteControl->m_ObjectID,"remote");
	else if( m_pObj_NowPlayingOnScreen )
		m_vectObjs_NeedRedraw.push_back(m_pObj_NowPlayingOnScreen);
}

bool Orbiter::TestCurrentScreen(string &sPK_DesignObj_CurrentScreen)
{
    PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	if( sPK_DesignObj_CurrentScreen.length(  ) && ( !m_pScreenHistory_Current || atoi( sPK_DesignObj_CurrentScreen.c_str(  ) )!=m_pScreenHistory_Current->m_pObj->Data.m_iBaseObjectID )  ) // It should be at the beginning
    {
        // Be sure it's not a -1 telling us to be at the main menu
        if(  sPK_DesignObj_CurrentScreen=="-1" )
		{
			if( !m_pScreenHistory_Current || !m_pScreenHistory_Current->m_pObj )
				return false;

			if( m_pScreenHistory_Current->m_pObj->Data.m_iBaseObjectID==m_pDesignObj_Orbiter_MainMenu->Data.m_iBaseObjectID ||
					(m_pDesignObj_Orbiter_SleepingMenu && m_pScreenHistory_Current->m_pObj->Data.m_iBaseObjectID==m_pDesignObj_Orbiter_SleepingMenu->Data.m_iBaseObjectID) ||
					(m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->m_pObj->Data.m_iBaseObjectID==m_pDesignObj_Orbiter_ScreenSaveMenu->Data.m_iBaseObjectID) )
				return true;
			else
	            return false;
		}
    }
	return true;
}

void Orbiter::ContinuousRefresh( void *data )
{
	ContinuousRefreshInfo *pContinuousRefreshInfo = (ContinuousRefreshInfo *) data;
	if( m_pScreenHistory_Current->m_pObj!=pContinuousRefreshInfo->m_pObj )
		delete pContinuousRefreshInfo;
	else
	{
		if( m_pScreenHistory_Current->m_pObj->Data.m_iBaseObjectID==DESIGNOBJ_mnuScreenSaver_CONST )
		{
			if( !m_bDisplayOn )
				return; // Nothing more to do

			DesignObjText *pText = FindText( m_pScreenHistory_Current->m_pObj, TEXT_USR_ENTRY_CONST );
			if( pText  )
			{
				pText->m_sText = StringUtils::itos(	int(m_tTimeoutTime - time(NULL)) ) + " seconds";
				pText->Data.m_rPosition.X = int(rand() * (float) (m_iImageWidth * .5 / RAND_MAX));
				pText->Data.m_rPosition.Y = int(rand() * (float) (m_iImageHeight *.9 / RAND_MAX));
			}

			CMD_Set_Text(m_pScreenHistory_Current->m_pObj->m_ObjectID, StringUtils::itos( int(m_tTimeoutTime - time(NULL)) ) + " seconds",TEXT_USR_ENTRY_CONST);
		}

		CMD_Refresh("");
		CallMaintenanceInMiliseconds( pContinuousRefreshInfo->m_iInterval * 1000, &Orbiter::ContinuousRefresh, pContinuousRefreshInfo, pe_ALL );
	}
}

//<-dceag-c238-b->

	/** @brief COMMAND: #238 - Continuous Refresh */
	/** Continuously refresh the current page.  Used when the page contains constantly changing data. */
		/** @param #102 Time */
			/** The interval time in seconds */

void Orbiter::CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c238-e->
{
	ContinuousRefreshInfo *pContinuousRefreshInfo = new ContinuousRefreshInfo(m_pScreenHistory_Current->m_pObj,atoi(sTime.c_str()));
	CallMaintenanceInMiliseconds( pContinuousRefreshInfo->m_iInterval, &Orbiter::ContinuousRefresh, pContinuousRefreshInfo, pe_ALL );
}

//timingofsetnowplaying
//<-dceag-c254-b->

	/** @brief COMMAND: #254 - Bind Icon */
	/** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
		/** @param #3 PK_DesignObj */
			/** The object which contains the icon, or whose child objects contain the icon. */
		/** @param #14 Type */
			/** The type of binding, like "housemode", "userstatus_39288", etc. */
		/** @param #104 Child */
			/** If true, it will set the property for the child object(s), rather than the designated object. */

void Orbiter::CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage)
//<-dceag-c254-e->
{
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
	if( !pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find object: %s for bind icon",sPK_DesignObj.c_str());
		return;
	}

	if( sType=="room_button" )
	{
        int iLocation=-1;
        DesignObjZoneList::iterator iZone;
        for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
        {
            DesignObjCommandList::iterator iAction;
            for( iAction=( *iZone )->m_Commands.begin(  );iAction!=( *iZone )->m_Commands.end(  );++iAction )
            {
                //int c = ( *iAction )->m_PK_Command;
                if(  ( *iAction )->Data.m_PK_Command==COMMAND_Set_Current_Location_CONST  )
                {
                    map<int,  string>::iterator iap;
                    for( iap=( *iAction )->m_ParameterList.begin(  );iap!=( *iAction )->m_ParameterList.end(  );++iap )
                    {
                        if(  ( *iap ).first==COMMANDPARAMETER_LocationID_CONST  )
                        {
                            iLocation=atoi( ( ( *iap ).second ).c_str(  ) );
                            break;
                        }
                    }
                }
            }
        }
        if(  iLocation!=-1  )
        {
            LocationInfo *pLocation = m_dequeLocation[iLocation];
            pLocation->m_pvectGraphic = &(pObj->m_vectGraphic);
        }
		return;
	}

	if( sType=="room" )
	{
		pObj->m_bIsBoundToLocation=true;
		return;
	}
	if( sType=="user_button" )
	{
        int PK_Users=0;
        DesignObjZoneList::iterator iZone;
        for( iZone=pObj->m_ZoneList.begin(  );!PK_Users && iZone!=pObj->m_ZoneList.end(  );++iZone )
        {
            DesignObjCommandList::iterator iAction;
            for( iAction=( *iZone )->m_Commands.begin(  );!PK_Users && iAction!=( *iZone )->m_Commands.end(  );++iAction )
            {
                if(  ( *iAction )->Data.m_PK_Command==COMMAND_Set_Current_User_CONST  )
                {
                    map<int,  string>::iterator iap;
                    for( iap=( *iAction )->m_ParameterList.begin(  );iap!=( *iAction )->m_ParameterList.end(  );++iap )
                    {
                        if(  ( *iap ).first==COMMANDPARAMETER_PK_Users_CONST  )
                        {
                            PK_Users=atoi( ( ( *iap ).second ).c_str(  ) );
                            break;
                        }
                    }
                }
            }
        }
        if(  PK_Users  )
            m_mapUserIcons[PK_Users]= &(pObj->m_vectGraphic);
		return;
	}

	if( sType=="user" )
	{
		pObj->m_bIsBoundToUser=true;
		return;
	}

	DesignObj_DataList *pDesignObj_DataList = m_mapObj_Bound_Find(sType);
	if( !pDesignObj_DataList )
	{
		pDesignObj_DataList = new DesignObj_DataList();
		m_mapObj_Bound[sType] = pDesignObj_DataList;
	}

	if( bChild )
	{
		for( DesignObj_DataList::iterator it=pObj->m_ChildObjects.begin(  ); it != pObj->m_ChildObjects.end(  ); ++it )
			pDesignObj_DataList->push_back( (DesignObj_Orbiter *) *it);
	}
	else
		pDesignObj_DataList->push_back(pObj);
}

/*virtual*/ void Orbiter::SimulateMouseClick(int x, int y)
{
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_WARNING, "Simulate mouse click at position: %d, %d", x, y);
#endif

	BeginPaint();
	PlutoColor color(255, 0, 0, 100);
	SolidRectangle(x - 5, y - 5, 10, 10, color, 50);
	UpdateRect(PlutoRectangle(x - 5, y - 5, 10, 10), PlutoPoint(0, 0));

	//render current screen id
	SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color, 50);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->m_pObj);
	text2.Data.m_rPosition = rect2;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Current screen: " + this->GetCurrentScreenID();
	RenderText(sText,&text2, pTextStyle);
	UpdateRect(PlutoRectangle(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25), PlutoPoint(0, 0));
	EndPaint();

	RegionDown(x, y);
}

/*virtual*/ void Orbiter::SimulateKeyPress(long key)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Simulate key press. Key code: %d", key);
#endif

	BeginPaint();

	//render a text with the
	PlutoColor color(200, 200, 200, 100);
	SolidRectangle(5, m_iImageHeight - 30, 200, 25, color, 50);
	PlutoRectangle rect(5, m_iImageHeight - 30, 200, 25);
	DesignObjText text(m_pScreenHistory_Current->m_pObj);
	text.Data.m_rPosition = rect;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Key code: " + StringUtils::ltos(key);
	RenderText(sText,&text, pTextStyle);

	//render current screen id
	PlutoColor color2(255, 0, 0, 100);
	SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color2, 50);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->m_pObj);
	text2.Data.m_rPosition = rect2;
	sText = "Current screen: " + this->GetCurrentScreenID();
	RenderText(sText,&text2, pTextStyle);
	UpdateRect(PlutoRectangle(5, m_iImageHeight - 30, 200, 25), PlutoPoint(0, 0));
	EndPaint();

	HandleButtonEvent(key);
    StopRepeatRelatedEvents(); 
}

time_t Orbiter::GetLastScreenChangedTime()
{
	return NULL != m_pScreenHistory_Current ? m_pScreenHistory_Current->m_tTime : time(NULL);
}

string Orbiter::GetCurrentScreenID()
{
	return
		(
			NULL != m_pScreenHistory_Current				&&
			NULL != m_pScreenHistory_Current->m_pObj
		)
		?
			m_pScreenHistory_Current->m_pObj->m_ObjectID :
			"UNKNOWN";
}//<-dceag-c258-b->

	/** @brief COMMAND: #258 - Clear Selected Devices */
	/** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
		/** @param #3 PK_DesignObj */
			/** If specified, the object referenced here will be invalidated and redrawn. */

void Orbiter::CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c258-e->
{
	m_mapDevice_Selected.clear();
	DesignObj_Orbiter *pObj=NULL;
	if( sPK_DesignObj.length() && (pObj=FindObject(sPK_DesignObj))!=NULL )
	{
		PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
		m_vectObjs_NeedRedraw.push_back(pObj);
	}
}

/*virtual*/ void Orbiter::RenderGraphic(DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	vector<PlutoGraphic*> *pVectorPlutoGraphic = pObj->m_pvectCurrentGraphic;

	//we have nothing to render
	if(pVectorPlutoGraphic->size() == 0)
		return;

	//just in case
	if(int(pVectorPlutoGraphic->size()) <= pObj->m_iCurrentFrame)
		pObj->m_iCurrentFrame = 0;

	int iCurrentFrame = pObj->m_iCurrentFrame;
	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
	bool bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;
if( pObj->m_ObjectID.find(".3351")!=string::npos )
{
	if( rectTotal.X!=13 )
	{
		int k4=4;
	}
int k=2;
}

    //hack: if a button doesn't have a mng as selected state (see bDisableEffects), then the png
    //used in normal state will be rendered for selected state + a blue rectangle to show the selection
    if(!bIsMNG && pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
    {
        pVectorPlutoGraphic = &pObj->m_vectGraphic; //normal state

        //we have nothing to render
        if(pVectorPlutoGraphic->size() == 0)
            return;

        if(int(pVectorPlutoGraphic->size()) <= pObj->m_iCurrentFrame)
            pObj->m_iCurrentFrame = 0;

        iCurrentFrame = pObj->m_iCurrentFrame;
        pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
        bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;
    }

	string sFileName = "";
	if(pPlutoGraphic->IsEmpty() && NULL != m_pCacheImageManager && pPlutoGraphic->m_Filename.length() && 
		m_pCacheImageManager->IsImageInCache(pPlutoGraphic->m_Filename, pObj->Data.m_Priority)
	)
	{
		//if we have the file in cache
		sFileName = m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(pPlutoGraphic->IsEmpty() && m_sLocalDirectory.length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = m_sLocalDirectory + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(pPlutoGraphic->IsEmpty() && sFileName.empty())
	{
		// Request our config info
		char *pGraphicFile=NULL;
		int iSizeGraphicFile=0;

		DCE::CMD_Request_File CMD_Request_File(
			m_dwPK_Device,m_dwPK_Device_GeneralInfoPlugIn,
			"orbiter/C" + StringUtils::itos(m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename,
			&pGraphicFile,&iSizeGraphicFile);
		SendCommand(CMD_Request_File);

		if (!iSizeGraphicFile)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());
			return;
		}

		//save the image in cache
		if(NULL != m_pCacheImageManager) //cache manager is enabled ?
		{
			m_pCacheImageManager->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, pObj->Data.m_Priority);
			sFileName = m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		//TODO: same logic for in-memory data
		if(sFileName.empty() && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
		{
			delete pGraphicFile;
			pGraphicFile = NULL;
			return;
		}

		delete pGraphicFile;
		pGraphicFile = NULL;
	}

	if(pPlutoGraphic->IsEmpty() && !sFileName.empty())
	{
		if(!FileUtils::FileExists(sFileName))
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read file %s", (sFileName).c_str());
			return;
		}

		switch(pPlutoGraphic->m_GraphicFormat)
		{
			case GR_JPG:
			case GR_GIF:
			case GR_TIF:
			case GR_PNG:
			case GR_BMP:
			case GR_OCG:
				{
					size_t size = 0;
					char *pData = FileUtils::ReadFileIntoBuffer(sFileName.c_str(), size);

					if(!size)
						return;

					if(!pPlutoGraphic->LoadGraphic(pData, size))
						return;

					delete [] pData;
				}
				break;

			case GR_MNG:
				{
					eGraphicFormat eGF = pPlutoGraphic->m_GraphicFormat;
					eGraphicManagement eGM = pPlutoGraphic->m_GraphicManagement;
					string sMNGFileName = pPlutoGraphic->m_Filename;

					for(size_t iIndex = 0; iIndex < (*pVectorPlutoGraphic).size(); iIndex++)
						delete (*pVectorPlutoGraphic)[iIndex];
					(*pVectorPlutoGraphic).clear();

					InMemoryMNG *pInMemoryMNG = InMemoryMNG::CreateInMemoryMNGFromFile(sFileName, rectTotal.Size());
					size_t framesCount = pInMemoryMNG->m_vectMNGframes.size();
					for(size_t i = 0; i < framesCount; i++)
					{
						size_t iFrameSize = 0;
						char *pFrameData = NULL;

						iFrameSize = pInMemoryMNG->GetFrame(int(i), pFrameData);

						if(iFrameSize)
						{
							PlutoGraphic *pGraphic = CreateGraphic();
							pGraphic->m_GraphicManagement = eGM;
							pGraphic->m_Filename = sMNGFileName;
							pGraphic->m_GraphicFormat = GR_PNG; //this is an mng with multiple png frames
							pGraphic->LoadGraphic(pFrameData, iFrameSize);
							pGraphic->m_GraphicFormat = GR_MNG;
							(*pVectorPlutoGraphic).push_back(pGraphic);
						}

						delete [] pFrameData;
					}

					delete pInMemoryMNG;
					pInMemoryMNG = NULL;
				}
				break;

		}
	}

	if(bIsMNG && pObj->m_pvectCurrentPlayingGraphic == NULL)
	{
		pPlutoGraphic = (*pVectorPlutoGraphic)[0];

		int iTime = 0; //hardcoding warning! don't know from where to get the framerate yet (ask Radu, libMNG)
#ifndef WINCE
		iTime = 15;
#endif

		bool bLoop = false; //hardcoding warning!  (ask Radu, libMNG)

		switch(pObj->m_GraphicToDisplay)
		{
		case GRAPHIC_HIGHLIGHTED:
			pObj->m_iTime_Highlighted = iTime;
			pObj->m_bLoop_Highlighted = bLoop;
			break;
		case GRAPHIC_SELECTED:
			pObj->m_iTime_Selected = iTime;
			pObj->m_bLoop_Selected = bLoop;
			break;
		case GRAPHIC_NORMAL:
			pObj->m_iTime_Background = iTime;
			pObj->m_bLoop_Background = bLoop;
			break;
			//todo alternate graphics ?
		}

		pObj->m_iCurrentFrame = 1;

		//schedule next frame for animation
		pObj->m_pvectCurrentPlayingGraphic = pObj->m_pvectCurrentGraphic;
		pObj->m_GraphicToPlay = pObj->m_GraphicToDisplay;
		CallMaintenanceInMiliseconds( iTime, &Orbiter::PlayMNG_CallBack, pObj , pe_NO );
	}
if( pObj->m_ObjectID.find(".2355")!=string::npos )
{
	// x=163
int k=2;
}

	if(!pPlutoGraphic->IsEmpty())
		RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
#ifdef DEBUG
	else
		g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", pObj->m_ObjectID.c_str());
#endif

    if(pObj == m_pObj_Highlighted)
        HighlightObject(pObj, point);

    if(!bIsMNG && pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
        SelectObject(pObj, point);
}

/*virtual*/ void Orbiter::GetRepeatedKeysForScreen(DesignObj_Orbiter* pObj, string& sKeysList)
{
    if(pObj->Data.m_bRepeatParm)
    {
        if(sKeysList.length())
            sKeysList += ",";

        sKeysList += StringUtils::ltos(pObj->Data.m_iPK_Button);
    }

    DesignObj_DataList::iterator it;
    for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
    {
        DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*it);
        GetRepeatedKeysForScreen(pDesignObj_Orbiter, sKeysList);
    }
}

/*virtual*/ bool Orbiter::IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button)
{
    if(pObj->Data.m_bRepeatParm && pObj->Data.m_iPK_Button == iPK_Button)
        return true;

    DesignObj_DataList::iterator it;
    for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
    {
        DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*it);
        if(IsRepeatedKeyForScreen(pDesignObj_Orbiter, iPK_Button))
            return true;
    }

    return false;
}

/*virtual*/ void Orbiter::PlayMNG_CallBack(void *data)
{
	DesignObj_Orbiter* pObj = (DesignObj_Orbiter*)data;

	if(NULL == pObj->m_pvectCurrentPlayingGraphic)
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "MNG playing was canceled for object with id %s",
			pObj->m_ObjectID.c_str());
#endif
		return; //playing was canceled
	}

	vector<PlutoGraphic*> *pVectorPlutoGraphic = pObj->m_pvectCurrentPlayingGraphic;

	if(pVectorPlutoGraphic->size() == 0) //we have nothing to render
		return;

	int iFrameNum = int(pVectorPlutoGraphic->size());

	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[pObj->m_iCurrentFrame];

	if(!pPlutoGraphic->IsEmpty())
	{
		BeginPaint();
		RenderGraphic(pPlutoGraphic, pObj->Data.m_rBackgroundPosition, pObj->m_bDisableAspectLock);

        if(pObj == m_pObj_Highlighted)
            HighlightObject(pObj);

        UpdateRect(pObj->Data.m_rPosition, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));
		EndPaint();
	}

	pObj->m_iCurrentFrame++;

	unsigned int iDelay = 0; //temp
#ifndef WINCE
	iDelay = 15;
#endif

	if(pObj->m_iCurrentFrame >= iFrameNum) //this is the last one
	{
		pObj->m_iCurrentFrame = 0;
		pObj->m_pvectCurrentPlayingGraphic = NULL;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "MNG playing was completed for object with id %s",
			pObj->m_ObjectID.c_str());
#endif
		CallMaintenanceInMiliseconds( iDelay, &Orbiter::DeselectObjects, ( void * ) pObj, pe_NO );
	}
    else
	{
		CallMaintenanceInMiliseconds( iDelay, &Orbiter::PlayMNG_CallBack, pObj , pe_NO );
	}
}

//<-dceag-c260-b->

	/** @brief COMMAND: #260 - Set Main Menu */
	/** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
		/** @param #9 Text */
			/** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

void Orbiter::CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c260-e->
{
	sText = StringUtils::ToUpper(sText); // we don't have to worry about lower case

	if( sText.length()>2 && sText[1]=='=' )
	{
		// We're assigning a new menu
		DesignObj_Orbiter *pObj = FindObject( sText.substr(2) );
		if( pObj )
		{
			if( sText[0]=='N' )
				m_pDesignObj_Orbiter_MainMenu=pObj;
			else if( sText[0]=='S' && m_pDesignObj_Orbiter_SleepingMenu )
				m_pDesignObj_Orbiter_SleepingMenu=pObj;
			else if( sText[0]=='V' && m_pDesignObj_Orbiter_ScreenSaveMenu )
				m_pDesignObj_Orbiter_ScreenSaveMenu=pObj;
		}
		else
			g_pPlutoLogger->Write(LV_CRITICAL,"Can't assign new main menu %s",sText.c_str());
	}

	if( sText.length() )  // Be sure it's not empty
	{
		// Just the normal N, S or V
		if( sText[0]=='N' )
			m_sMainMenu = m_pDesignObj_Orbiter_MainMenu->m_ObjectID;
		else if( sText[0]=='S' && m_pDesignObj_Orbiter_SleepingMenu )
			m_sMainMenu = m_pDesignObj_Orbiter_SleepingMenu->m_ObjectID;
		else if( sText[0]=='V' && m_pDesignObj_Orbiter_ScreenSaveMenu )
			m_sMainMenu = m_pDesignObj_Orbiter_ScreenSaveMenu->m_ObjectID;
	}
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Set menu: %s now %s",sText.c_str(),m_sMainMenu.c_str());
#endif
}
//<-dceag-c265-b->

	/** @brief COMMAND: #265 - Quit */
	/** Exits the orbiter application */

void Orbiter::CMD_Quit(string &sCMD_Result,Message *pMessage)
//<-dceag-c265-e->
{
	OnQuit();
}
//<-dceag-createinst-b->!

void Orbiter::KillMaintThread()
{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Kill Maint Thread %d",(int) bMaintThreadIsRunning);
#endif
	m_bQuit=true;
	pthread_cond_broadcast(&m_MaintThreadCond);  // Wake it up, it will quit when it sees the quit
	time_t tTime = time(NULL);
	while(bMaintThreadIsRunning)
	{
		Sleep(10);
		if( tTime + 5 < time(NULL) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Maint Thread had blocked!!!!");
			exit(1);
		}
	}
}

void Orbiter::StopSimulatorThread()
{
    g_pPlutoLogger->Write(LV_STATUS,"Stopping simulator thread...");
    Simulator::GetInstance()->StopRandomEventGenerator();

    time_t tTime = time(NULL);
    while(Simulator::GetInstance()->IsRunning())
    {
        Sleep(10);
        if( tTime + 5 < time(NULL) )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Failed to stop the simulator thread!");
            return;
        }
    }
    g_pPlutoLogger->Write(LV_STATUS,"Simulator thread stopped.");
}

int Orbiter::TranslateVirtualDevice(int PK_DeviceTemplate)
{
	// This is going to a virtual device
	switch(  PK_DeviceTemplate )
	{
	case DEVICETEMPLATE_VirtDev_AppServer_CONST:
		return m_pLocationInfo->m_dwPK_Device_AppServer;

	case DEVICETEMPLATE_VirtDev_Orbiter_Onscreen_CONST:
		return m_pLocationInfo->m_dwPK_Device_Orbiter;

	case DEVICETEMPLATE_VirtDev_Media_Director_CONST:
		return m_pLocationInfo->m_dwPK_Device_MediaDirector;

	case DEVICETEMPLATE_VirtDev_Local_Media_Director_CONST:
		return m_pLocationInfo_Initial->m_dwPK_Device_MediaDirector;

	case DEVICETEMPLATE_This_Orbiter_CONST:
		return m_dwPK_Device;

	case DEVICETEMPLATE_VirtDev_Security_Plugin_CONST:
		return m_dwPK_Device_SecurityPlugIn;

	case DEVICETEMPLATE_VirtDev_Telecom_Plugin_CONST:
		return m_dwPK_Device_TelecomPlugIn;

	case DEVICETEMPLATE_VirtDev_Media_Plugin_CONST:
		return m_dwPK_Device_MediaPlugIn;

	case DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST:
		return m_dwPK_Device_ClimatePlugIn;

	case DEVICETEMPLATE_VirtDev_Lighting_PlugIn_CONST:
		return m_dwPK_Device_LightingPlugIn;

	case DEVICETEMPLATE_VirtDev_Infrared_Plugin_CONST:
		return m_dwPK_Device_InfraredPlugIn;

	case DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST:
		return m_dwPK_Device_GeneralInfoPlugIn;

	case DEVICETEMPLATE_VirtDev_Event_Plugin_CONST:
		return m_dwPK_Device_EventPlugIn;

	case DEVICETEMPLATE_VirtDev_Datagrid_Plugin_CONST:
		return m_dwPK_Device_DatagridPlugIn;

	case DEVICETEMPLATE_VirtDev_Orbiter_Plugin_CONST:
		return m_dwPK_Device_OrbiterPlugIn;

	case DEVICETEMPLATE_VirtDev_Local_AppServer_CONST:
		return m_dwPK_Device_LocalAppServer;

	}
	return -1;
}

//<-dceag-c324-b->

	/** @brief COMMAND: #324 - Set Timeout */
	/** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
		/** @param #3 PK_DesignObj */
			/** The screen to set the timeout on.  If blank the current screen. */
		/** @param #102 Time */
			/** The timeout in seconds.  0 or blank means no timeout. */

void Orbiter::CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c324-e->
{
	DesignObj_Orbiter *pObj;
	if( atoi(sPK_DesignObj.c_str())==0 )
		pObj = m_pScreenHistory_Current->m_pObj;
	else
	{
	    sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
		pObj = FindObject( sPK_DesignObj );
	}
    if(  !pObj  )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find object in CMD_Set_Timeout: %s", sPK_DesignObj.c_str(  ) );
        return;
    }


	pObj->Data.m_dwTimeoutSeconds = atoi(sTime.c_str());

#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "set timeout on %s to %d  %p = %p",pObj->m_ObjectID.c_str(),pObj->Data.m_dwTimeoutSeconds,pObj,m_pScreenHistory_Current->m_pObj );
#endif
	if( pObj==m_pScreenHistory_Current->m_pObj && pObj->Data.m_dwTimeoutSeconds )
		CallMaintenanceInMiliseconds( pObj->Data.m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) pObj, pe_ALL, true );
}
//<-dceag-c325-b->

	/** @brief COMMAND: #325 - Keep Screen On */
	/** Allow or don't allow the screen to blank with the screen saver. */
		/** @param #8 On/Off */
			/** If other than "0", the screen saver will be disabled. */

void Orbiter::CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c325-e->
{
	m_bBypassScreenSaver = sOnOff!="0";
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Bypass screen saver now: %d",(int)m_bBypassScreenSaver);
#endif
}
//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn the device on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Orbiter::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn the device off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Orbiter::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	BeginPaint();
	PlutoColor color(200, 200, 200, 100);
	SolidRectangle(5, m_iImageHeight - 30, 200, 25, color, 50);
	PlutoRectangle rect(5, m_iImageHeight - 30, 200, 25);
	DesignObjText text(m_pScreenHistory_Current->m_pObj);
	text.Data.m_rPosition = rect;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Display is OFF";
	RenderText(sText,&text, pTextStyle);
	UpdateRect(PlutoRectangle(5, m_iImageHeight - 30, 200, 25), PlutoPoint(0, 0));
	EndPaint();
}
//<-dceag-c330-b->

	/** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
	/** Positions the on-screen mouse pointer centered over a certain object */
		/** @param #3 PK_DesignObj */
			/** The object to center the mouse over. */

void Orbiter::CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c330-e->
{
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj, m_pScreenHistory_Current->m_pObj );
	if( !pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"MousePointerOverUnknownObject: %s",sPK_DesignObj.c_str());
		return;
	}
	int X = pObj->Data.m_rPosition.X + pObj->Data.m_rPosition.Width/2;
	int Y = pObj->Data.m_rPosition.Y + pObj->Data.m_rPosition.Height/2;
	SetMousePointer(X,Y);
}

//<-dceag-c354-b->

	/** @brief COMMAND: #354 - Show Mouse Pointer */
	/** Shows or hides the mouse pointer. */
		/** @param #8 On/Off */
			/** 1=show it, 0=hide it. */

void Orbiter::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c354-e->
{
}

//<-dceag-c366-b->

	/** @brief COMMAND: #366 - Activate Window */
	/** ActivateApplication - Used by Linux On Screeen Orbiters only */
		/** @param #50 Name */
			/** Name as known by ratpoison. */

void Orbiter::CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c366-e->
{
}

bool Orbiter::OkayToDeserialize(int iSC_Version)
{
	if( iSC_Version==ORBITER_SCHEMA )
		return true;
	g_pPlutoLogger->Write(LV_CRITICAL,"Cannot deserialize .info file.  It is version %d and I need %d",
		iSC_Version,ORBITER_SCHEMA);
	return false;
}

// Temporary function to debug a problem with the screen history
void Orbiter::DumpScreenHistory()
{
    PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
	string s = "history size: " + StringUtils::itos(int(m_listScreenHistory.size()));

for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();++it)
{
ScreenHistory *psh = *it;
s+=psh->m_pObj->m_ObjectID + " / ";
}
g_pPlutoLogger->Write(LV_WARNING,"Screen history %s",s.c_str());
}
//<-dceag-c59-b->

	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** If you don't know the location, you can also set just the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

void Orbiter::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
    for(size_t s=0;s<m_dequeLocation.size();++s)
	{
		LocationInfo *pLocationInfo = m_dequeLocation[s];
		if( pLocationInfo->PK_EntertainArea == atoi(sPK_EntertainArea.c_str()) )
		{
			CMD_Set_Current_Location(pLocationInfo->iLocation);
			return;
		}
	}
	g_pPlutoLogger->Write( LV_CRITICAL, "Can't set ea to %s",sPK_EntertainArea.c_str() );
}

//<-dceag-c77-b->

	/** @brief COMMAND: #77 - Set Current Room */
	/** If you don't know the location, you can also set just the room */
		/** @param #57 PK_Room */
			/** The room */

void Orbiter::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
    for(size_t s=0;s<m_dequeLocation.size();++s)
	{
		LocationInfo *pLocationInfo = m_dequeLocation[s];
		if( pLocationInfo->PK_Room == iPK_Room )
		{
			CMD_Set_Current_Location(pLocationInfo->iLocation);
			return;
		}
	}
	g_pPlutoLogger->Write( LV_CRITICAL, "Can't set ea to %d",iPK_Room );
}
//<-dceag-c389-b->

	/** @brief COMMAND: #389 - Send Message */
	/** Sends a message stored in a parameter as a text object. */
		/** @param #9 Text */
			/** The message in command line-style format */

void Orbiter::CMD_Send_Message(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c389-e->
{
	string sMessage = SubstituteVariables(sText,NULL,0,0);
	Message *pMessageOut = new Message(sMessage);
	QueueMessageToRouter(pMessageOut);
}

void Orbiter::ResetState(DesignObj_Orbiter *pObj, bool bDontResetState)
{
    if(  pObj->Data.m_bDontResetState || pObj->m_bOneTimeDontReset  )
    {
        pObj->m_bOneTimeDontReset=false;
        bDontResetState=true;
    }
    else if(  !bDontResetState  )
    {
        pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;
        pObj->m_bHidden=pObj->Data.m_bHideByDefault;
    }

    DesignObj_DataList::iterator iHao;
    for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
        ResetState( (DesignObj_Orbiter * )( *iHao ), bDontResetState );
}

/*virtual*/ void Orbiter::RenderPopup(PlutoPopup *pPopup, PlutoPoint point)
{
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS,"ShowPopup: %s", pPopup->m_pObj->m_ObjectID.c_str());
#endif
    PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

    if(pPopup->m_pObj)
        RenderObject(pPopup->m_pObj, pPopup->m_pObj, point);
    else
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot render the popup %s: object %s doesn't exist", pPopup->m_sName.c_str(), pPopup->m_pObj->m_ObjectID.c_str());
}


PlutoPopup *Orbiter::FindPopupByName(DesignObj_Orbiter *pObj,string sName)
{
	if( pObj )
	{
		for(list<class PlutoPopup*>::reverse_iterator it=pObj->m_listPopups.rbegin();it!=pObj->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(pPopup->m_sName == sName )
				return pPopup;
		}
	}
	else
	{
		for(list<class PlutoPopup*>::reverse_iterator it=m_listPopups.rbegin();it!=m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(pPopup->m_sName == sName )
				return pPopup;
		}
	}

    return NULL;
}

//<-dceag-c397-b->

	/** @brief COMMAND: #397 - Show Popup */
	/** Shows a screen as a popup, at position x, y */
		/** @param #3 PK_DesignObj */
			/** The ID of the screen */
		/** @param #11 Position X */
			/** X position */
		/** @param #12 Position Y */
			/** Y position */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If specified the popup will be local to this screen, otherwise it will be global.  Global and local popups are treated separately */
		/** @param #50 Name */
			/** The popup name */
		/** @param #126 Exclusive */
			/** Hide any other popups that are also visible, unless don't hide is set. */
		/** @param #127 Dont Auto Hide */
			/** If true, this popup will not be automatically hidden when the screen changes or another exclusive is shown.  It must be explicitly hidden. */

void Orbiter::CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage)
//<-dceag-c397-e->
{
    g_pPlutoLogger->Write(LV_CRITICAL,"show popup %s",sName.c_str());

    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	DesignObj_Orbiter *pObj_Popup = FindObject(sPK_DesignObj);
	if( !pObj_Popup )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"CMD_Show_Popup bad popup %s",sPK_DesignObj.c_str());
		return;
	}
	DesignObj_Orbiter *pObj_Screen = FindObject(sPK_DesignObj_CurrentScreen);
	if( bExclusive )
		HidePopups(pObj_Screen);

	PlutoPoint pt(iPosition_X,iPosition_Y);
	if( m_iRotation==90 )
	{
		int x=pt.X;
		pt.X = m_sScreenSize.Height - pt.Y - pObj_Popup->Data.m_rPosition.Height;
		pt.Y = x;
	}

    PlutoPopup *pPopup = new PlutoPopup(pObj_Popup, sName, pt);
	bool bSuccessful;
	if( pObj_Screen )
		bSuccessful=AddPopup(pObj_Screen->m_listPopups,pPopup);
	else
		bSuccessful=AddPopup(m_listPopups,pPopup);

	if( !bSuccessful )
		return; // Popup must have already been there

    VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;
	ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pObj_Popup );
	HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );

	pPopup->m_bDontAutohide = bDont_Auto_Hide;

	CMD_Refresh("");
if( sName=="remote" )
{
int k=2;
}

}

//<-dceag-c398-b->

	/** @brief COMMAND: #398 - Remove Popup */
	/** Hides a popup. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** (optional).  The screen on which it's a local popup */
		/** @param #50 Name */
			/** The name of the popup.  If not specified all popups will be removed */

void Orbiter::CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c398-e->
{
if( sName=="remote" )
{
int k=2;
}
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj_CurrentScreen);
g_pPlutoLogger->Write(LV_CRITICAL,"remove popup %s",sName.c_str());

	if( pObj )
	{
		for(list<class PlutoPopup*>::iterator it=pObj->m_listPopups.begin();it!=pObj->m_listPopups.end();++it)
		{
			if( (*it)->m_sName==sName )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					ObjectOffScreen((*it)->m_pObj);
else
g_pPlutoLogger->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());
				delete *it;
				pObj->m_listPopups.erase(it);
				break;
			}
		}
	}
	else
	{
		for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		{
			if( (*it)->m_sName==sName )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					ObjectOffScreen((*it)->m_pObj);
else
g_pPlutoLogger->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());
				delete *it;
				m_listPopups.erase(it);
				break;
			}
		}
	}
	CMD_Refresh("");
}

/*virtual*/ void Orbiter::HidePopups(DesignObj_Orbiter *pObj)
{
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"hide popups");
#endif
	if( pObj )
	{
		for(list<class PlutoPopup*>::iterator it=pObj->m_listPopups.begin();it!=pObj->m_listPopups.end();)
		{
			ObjectOffScreen((*it)->m_pObj);
			delete *it++;
		}

        if(pObj->m_listPopups.size()) //no popup, no refresh
            CMD_Refresh("");

		pObj->m_listPopups.clear();
	}
	else
	{
		for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();)
		{
			ObjectOffScreen((*it)->m_pObj);
			delete *it++;
		}

        if(m_listPopups.size()) //no popup, no refresh
            CMD_Refresh("");

		m_listPopups.clear();
	}
}

//<-dceag-c399-b->

	/** @brief COMMAND: #399 - Show Shortcuts */
	/** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

void Orbiter::CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage)
//<-dceag-c399-e->
{
    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
    m_bShowShortcuts = true;
	CallMaintenanceInMiliseconds( 10000, &Orbiter::RemoveShortcuts, NULL, pe_ALL );
    CMD_Refresh("");
}

void Orbiter::RemoveShortcuts( void *data )
{
	if( m_bQuit )
		return;
    m_bShowShortcuts = false;
    CMD_Refresh("");
}
//<-dceag-c401-b->

	/** @brief COMMAND: #401 - Show File List */
	/** Shows the file list */
		/** @param #29 PK_MediaType */
			/** The type of media the user wants to browse. */

void Orbiter::CMD_Show_File_List(int iPK_MediaType,string &sCMD_Result,Message *pMessage)
//<-dceag-c401-e->
{
	if( !m_pOrbiterFileBrowser_Collection )
		return; // Should never happen

	OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry = 
		m_pOrbiterFileBrowser_Collection->m_mapOrbiterFileBrowser[iPK_MediaType];

	if( !pOrbiterFileBrowser_Entry )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"No way to show file list for %d",iPK_MediaType);
		return;
	}

	m_mapVariable[VARIABLE_Filename_CONST] = pOrbiterFileBrowser_Entry->m_sFilename;
	m_mapVariable[VARIABLE_PK_MediaType_CONST] = StringUtils::itos(iPK_MediaType);

	DesignObj_Orbiter *pObj_Popop_FileList;
	if( m_sObj_Popop_RemoteControl.size() && pOrbiterFileBrowser_Entry->m_DesignObj_Popup && (pObj_Popop_FileList=FindObject(m_sObj_Popop_RemoteControl)) )
	{
		CMD_Remove_Popup(pObj_Popop_FileList->m_ObjectID,"filelist");
		CMD_Show_Popup(StringUtils::itos(pOrbiterFileBrowser_Entry->m_DesignObj_Popup),m_Popop_FileList_X,m_Popop_FileList_Y,pObj_Popop_FileList->m_ObjectID,"filelist",false,false);
	}
	else
		GotoScreen(StringUtils::itos(pOrbiterFileBrowser_Entry->m_DesignObj));
}

//<-dceag-c402-b->

	/** @brief COMMAND: #402 - Use Popup Remote Controls */
	/** If this command is executed the remote controls will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen on which to put the popup */

void Orbiter::CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c402-e->
{
	m_sObj_Popop_RemoteControl = sPK_DesignObj_CurrentScreen;
	m_Popop_RemoteControl_X = iPosition_X;
	m_Popop_RemoteControl_Y = iPosition_Y;

	DCE::CMD_Set_Auto_Switch_to_Remote CMD_Set_Auto_Switch_to_Remote(m_dwPK_Device,m_dwPK_Device_OrbiterPlugIn,m_dwPK_Device,false);
	SendCommand(CMD_Set_Auto_Switch_to_Remote);
}

//<-dceag-c403-b->

	/** @brief COMMAND: #403 - Use Popup File List */
	/** If this command is executed the file lists will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen to put the popup on */

void Orbiter::CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c403-e->
{
	m_sObj_Popop_FileList = sPK_DesignObj_CurrentScreen;
	m_Popop_FileList_X = iPosition_X;
	m_Popop_FileList_Y = iPosition_Y;
}

bool Orbiter::AddPopup(list<class PlutoPopup*> &listPopups,class PlutoPopup *pPopup)
{
	for(list<class PlutoPopup*>::iterator it=listPopups.begin();it!=listPopups.end();++it)
	{
		PlutoPopup *p = *it;
		if( p->m_sName == pPopup->m_sName )
		{
			if( p->m_pObj==pPopup->m_pObj && p->m_Position==pPopup->m_Position )
				return false;

            CMD_Remove_Popup("", pPopup->m_sName);
			break;
		}
	}
	listPopups.push_back(pPopup);
	return true;
}

void Orbiter::HandleNewObjectsOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter)
{
    size_t s;
    for( s=0;s<pVectDesignObj_Orbiter->size(  );++s )
    {
        DesignObj_Orbiter *pDesignObj_Orbiter = (*pVectDesignObj_Orbiter)[s];
        if(  pDesignObj_Orbiter!=m_pScreenHistory_Current->m_pObj  )  // We just did the screen itself above
		{
            ExecuteCommandsInList( &pDesignObj_Orbiter->m_Action_LoadList, pDesignObj_Orbiter, 0, 0 );
		}
    }

    for( s=0;s<pVectDesignObj_Orbiter->size(  );++s )
    {
        DesignObj_Orbiter *pDesignObj_Orbiter = (*pVectDesignObj_Orbiter)[s];
        if ( pDesignObj_Orbiter->Data.m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST )
        {
            InitializeGrid( ( DesignObj_DataGrid * )pDesignObj_Orbiter  );
		    m_vectObjs_GridsOnScreen.push_back( ( DesignObj_DataGrid * ) pDesignObj_Orbiter );
        }
    }
}//<-dceag-c405-b->

	/** @brief COMMAND: #405 - Scale this object */
	/** If you add this command to the startup list of an object it will cause Orbiter Gen to scale this object and all it's children. */
		/** @param #48 Value */
			/** The value to scale to.  100=full size, 50=half size */

void Orbiter::CMD_Scale_this_object(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c405-e->
{
	// this only applies to orbitergen
}
//<-dceag-c407-b->

	/** @brief COMMAND: #407 - Set Floorplan */
	/** Sets the object to use for one of the following types:
light, media, climate, security, telecom */
		/** @param #3 PK_DesignObj */
			/** The screen to use for this floorplan */
		/** @param #14 Type */
			/** One of the following:
light, climate, media, security, telecom */
		/** @param #119 True/False */
			/** True if this is a popup.  False if it's full screen */

void Orbiter::CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c407-e->
{
	DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj);
	m_mapFloorplan[sType]=pObj;
}

//<-dceag-c408-b->

	/** @brief COMMAND: #408 - Show Floorplan */
	/** Shows the floorplan */
		/** @param #11 Position X */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #12 Position Y */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #14 Type */
			/** The type of floorplan */

void Orbiter::CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c408-e->
{
	DesignObj_Orbiter *pObj = m_mapFloorplan[sType];
	if( !pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"No floorplan for type %s",sType.c_str());
		return;
	}
	if( !m_pScreenHistory_Current || (pObj->Data.m_rPosition.Width==m_pScreenHistory_Current->m_pObj->Data.m_rPosition.Width && pObj->Data.m_rPosition.Height==m_pScreenHistory_Current->m_pObj->Data.m_rPosition.Height) )
		GotoScreen(pObj->m_ObjectID);
	else
		CMD_Show_Popup(pObj->m_ObjectID,iPosition_X,iPosition_Y,"","floorplan",false,false);
}
//<-dceag-c413-b->

	/** @brief COMMAND: #413 - Forward local k/b to OSD */
	/** Means this orbiter's keyboard should be controlling the application running on the media director. */
		/** @param #119 True/False */
			/** If 1, do it.  It 0, stop */

void Orbiter::CMD_Forward_local_kb_to_OSD(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c413-e->
{
	m_bForward_local_kb_to_OSD=bTrueFalse;
}
//<-dceag-c415-b->

	/** @brief COMMAND: #415 - Set Mouse Position Relative */
	/** Move the mouse relative to its current position */
		/** @param #11 Position X */
			/** The X Position to move */
		/** @param #12 Position Y */
			/** The Y Position to move */

void Orbiter::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c415-e->
{
}
//<-dceag-c416-b->

	/** @brief COMMAND: #416 - Simulate Mouse Click At Present Pos */
	/** Simulates clicking the mouse button */
		/** @param #14 Type */
			/** If L or empty, the left button.  If R the right button. */

void Orbiter::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c416-e->
{
}





bool Orbiter::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )	
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;
		
	// In some cases a class may need extra data to properly serialize in UnknownSerialize.  This is an extra void pointer that can be cast to something useful
	m_pExtraSerializationData=pExtraSerializationData;

	if( !m_bManuallySetupSerialization )
	{
		// For Symbian compatibility
		MYSTL_ITERATE_VECT( m_vectItemToSerialize, ItemToSerialize, pItem_del, it_del )
		{
			delete pItem_del;
		}
		MYSTL_CLEAR_LIST(m_vectItemToSerialize);
		if( bWriting )
		{
			Write_unsigned_long(m_iSC_Version);
			SetupSerialization(m_iSC_Version);
		}
		else
		{
			unsigned long iSC_Version = Read_unsigned_long();
			SetupSerialization(iSC_Version);
		}
	}
#ifdef DEBUG_SERIALIZATION
	cout << "Schema for: " << SerializeClassClassName();
	MYSTL_ITERATE_VECT(m_vectItemToSerialize,ItemToSerialize,pItem_cout,it_cout)
	{
		cout << " " << pItem_cout->m_iSerializeDataType;
	}
	cout << endl;
#endif
	MYSTL_ITERATE_VECT(m_vectItemToSerialize,ItemToSerialize,pItem,it)
	{

		// These are self-serializing, we don't need the if( bWriting ) split.  However, these classes are 
		// derived from SerializeClass.  Therefore, we can't include the class definitions before binary
		// serialize.  We do a forward declaration, and have to put the actual switch block in a .cpp
		// file to avoid the cylcical dependencies
		if( SelfSerializing(bWriting,pItem) )
			continue;
			
		if( bWriting )
		{
#ifdef DEBUG_SERIALIZATION
			cout << "Writing type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos: " << (int) CurrentSize() << endl;
#endif
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_CHAR:
				Write_char( *((char *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_SHORT:
				Write_short( *((short *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_LONG:
				Write_long( *((long *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
				Write_unsigned_long( *((unsigned long *) pItem->m_pItem) );
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_INT64:
				Write_int64( *((u_int64_t *) pItem->m_pItem) );
				break;
#endif
			case SERIALIZE_DATA_TYPE_STRING:
				Write_string( *((string *) pItem->m_pItem) );
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_VECT_STRING:
				{
					vector<string> *pVect = (vector<string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector strings" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_string( (*pVect)[s] );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_BLOCK:
				{
					PlutoDataBlock *pBlock = (PlutoDataBlock *) pItem->m_pItem;
					Write_unsigned_long(pBlock->m_dwSize);
					Write_block(pBlock->m_pBlock,pBlock->m_dwSize);
				}
				break;

			case SERIALIZE_DATA_TYPE_VECT_INT:
				{
					vector<int> *pVect = (vector<int> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector ints" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_unsigned_long( (*pVect)[s] );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_PAIR_INT:
				{
					vector< pair<int,int> > *pVect = (vector< pair<int,int> > *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector pair ints" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_unsigned_long( (*pVect)[s].first );
						Write_unsigned_long( (*pVect)[s].second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_STRING:
				{
					map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map int strings" << endl;
#endif
					map<int,string>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_long( (*it).first );
						Write_string( (*it).second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_STRING_STRING:
				{
					map<string,string> *pMap = (map<string,string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map string strings" << endl;
#endif
					map<string,string>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
						string key=(*it).first;
						Write_string( key );
						Write_string( (*it).second );
					}
				}
				break;

#endif

			default:
				{
					if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
					{

#ifdef DEBUG_SERIALIZATION
						cout << "Unknown Serialize failed" << endl;
#else

#ifndef SYMBIAN						

						throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
						/** @todo show a popup */
#endif
#endif
						return false; // We don't know how to serialize this type
					}
				}
			};
		}
		else
		{

#ifdef DEBUG_SERIALIZATION
			cout << "Reading type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos:" << (int) CurrentSize() << endl;
#endif
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_CHAR:
				*((char *) pItem->m_pItem)=Read_char();
				break;
			case SERIALIZE_DATA_TYPE_SHORT:
				*((short *) pItem->m_pItem)=Read_short();
				break;
			case SERIALIZE_DATA_TYPE_LONG:
				*((long *) pItem->m_pItem)=Read_long();
				break;
			case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
				*((unsigned long *) pItem->m_pItem)=Read_unsigned_long();
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_INT64:
				*((u_int64_t *) pItem->m_pItem)=Read_int64();
				break;
#endif
			case SERIALIZE_DATA_TYPE_STRING:
				Read_string( *((string *) pItem->m_pItem) );
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_VECT_STRING:
				{
					vector<string> *pVect = (vector<string> *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						string value;
						Read_string(value);
						pVect->push_back(value);
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_INT:
				{
					vector<int> *pVect = (vector<int> *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector ints" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						pVect->push_back(Read_unsigned_long());
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_PAIR_INT:
				{
					vector< pair<int,int> > *pVect = (vector< pair<int,int> > *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector pair ints" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						long l1 = Read_unsigned_long();
						long l2 = Read_unsigned_long();
						pVect->push_back( pair<int,int> (l1,l2) );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_STRING:
				{
					map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map int strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						int Key = Read_long();
						string value;
						Read_string(value);
						(*pMap)[Key]=value;
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_STRING_STRING:
				{
					map<string,string> *pMap = (map<string,string> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map string strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						string key,value;
						Read_string(key);
						Read_string(value);
						(*pMap)[key]=value;
					}
				}
				break;
#endif
			case SERIALIZE_DATA_TYPE_BLOCK:
				{
					PlutoDataBlock *pBlock = (PlutoDataBlock *) pItem->m_pItem;
					pBlock->m_dwSize = Read_unsigned_long();
					pBlock->m_pBlock=Read_block(pBlock->m_dwSize);
				}
				break;
			default:
				{
					if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
					{
#ifdef DEBUG_SERIALIZATION
						cout << "Unknown Serialize failed" << endl;
#else

#ifndef SYMBIAN							
						throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
						/** @todo show a popup */
#endif
							
#endif
						return false;
					}
				}
			};
		}
	}

	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}