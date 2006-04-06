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

#include "PlutoUtils/ProcessUtils.h"
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
#include "pluto_main/Define_Screen.h"
#include "Floorplan.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_Text.h"
#include "PlutoUtils/minilzo.h"
#include "../Orbiter/RendererMNG.h"
#include "OrbiterFileBrowser.h"
#include "ScreenHandler.h"
#include "ScreenHistory.h"

#include "GraphicBuilder.h"
#include "Simulator.h"
#include "MouseBehavior.h"

#ifdef TEST_OSD
#include "Linux/OSDScreenHandler.h"
#endif

//#define PROFILING_GRID

#if defined(WIN32) && !defined(BLUETOOTH_DONGLE)
#include "MainDialog.h" //for debugging in main window
#else //linux
void WriteStatusOutput(const char *) {} //do nothing
#endif

#define  VERSION "<=version=>"
extern const char *g_szCompile_Date;
extern Command_Impl *g_pCommand_Impl;

#ifdef WINCE
#include "wince.h"

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

extern void (*g_pReceivePingHandler)(Socket *pSocket);
extern bool (*g_pSendPingHandler)(Socket *pSocket);
void ReceivePingHandler(Socket *pSocket);
bool SendPingHandler(Socket *pSocket);

//------------------------------------------------------------------------
// DelayedSelectObjectInfo
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
// ContinuousRefreshInfo
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
bool ScreenHistory::m_bAddToHistory=true;

//------------------------------------------------------------------------
// Orbiter
//------------------------------------------------------------------------

/*
EXTRAINFO codes for datagrids:
'H' means select a cell when a user highlights it
'R' or 'C' means only highlight/select whole rows/columns
Either 'A' means arrows scroll the grid.  If 'H' is not specified, the scrolling will be selecting cells.  Otherwise it will be highlighting them
'As' means arrows scroll the grid 1 cell at a time
'Ap' means arrows scroll the grid 1 page at a time
'P' means make the first/last cell a 'page up'/'page down'
'h' means that moving the highlight should de-select any selected cells so you don't have both a highlighted and selected cell.  Only meaningfull with 'H'.
'X' means that clear the selected cell entry whenever the user scrolls
'S' means that on the initial acquire it should scroll to the highlighted cell
'cx' means if this is a phone on Bluetooth dongle, render this grid as a single column on the phone using the column x
'T'  - (for PlutoMO only) PlutoMO will send SelectedItem everytime the position on the grid is changed.
'F'  - force rendering in Orbiter
*/

/*

Constructors/Destructor

*/

//<-dceag-const-b->!
Orbiter::Orbiter( int DeviceID, int PK_DeviceTemplate, string ServerAddress,  string sLocalDirectory,
				 bool bLocalMode,  int iImageWidth,  int iImageHeight, pluto_pthread_mutex_t* pExternalScreenMutex/*=NULL*/)
				 : Orbiter_Command( DeviceID,  ServerAddress, true, bLocalMode ),
				 m_NeedRedrawVarMutex( "need redraw variables" ), m_MaintThreadMutex("MaintThread"), m_ScreenMutex( "rendering" ),
				 m_VariableMutex( "variable" ), m_DatagridMutex( "datagrid" )
				 //<-dceag-const-e->
{
	WriteStatusOutput((char *)(string("Orbiter version: ") + VERSION).c_str());
	WriteStatusOutput("Orbiter constructor");

	g_pPlutoLogger->Write(LV_STATUS,"Orbiter %p constructor",this);
	m_nCallbackCounter = 0;
	m_pObj_NowPlayingOnScreen = NULL;
	m_pObj_NowPlaying_Section_OnScreen = NULL;
	m_dwPK_DeviceTemplate = PK_DeviceTemplate;
	m_pScreenHandler = NULL;
	m_sLocalDirectory=sLocalDirectory;
	m_iImageWidth=iImageWidth;
	m_iImageHeight=iImageHeight;
	m_bStartingUp=true;
	m_pLocationInfo = NULL;
	m_dwPK_Users = 0;
	m_dwPK_Device_NowPlaying = 0;
	m_pOrbiterFileBrowser_Collection=NULL;
	m_bForward_local_kb_to_OSD=false;
	m_bYieldScreen=false;
	m_bYieldInput=false;
	m_bRerenderScreen=false;
	m_bWeCanRepeat=false;
	m_bRepeatingObject=false;
	m_bShowShortcuts = false;
	m_iPK_DesignObj_Remote=m_iPK_DesignObj_Remote_Popup=m_iPK_DesignObj_FileList=m_iPK_DesignObj_FileList_Popup=m_iPK_DesignObj_RemoteOSD=m_iPK_DesignObj_Guide=0;
	m_iPK_MediaType=0;

	m_pScreenHistory_Current=NULL;
	m_pObj_LastSelected=m_pObj_Highlighted=m_pObj_Highlighted_Last=NULL;
	m_pObj_SelectedLastScreen=NULL;
	m_pGraphicBeforeHighlight=NULL;
	m_pContextToBeRestored=NULL;
	m_LastActivityTime=time( NULL );
	m_iLastEntryInDeviceGroup=-1;
	m_pActivePopup=NULL;
	m_dwIDataGridRequestCounter=0;
	m_bCaptureKeyboard_OnOff = false;
	m_bCaptureKeyboard_Reset = false;
	m_iCaptureKeyboard_EditType = 0;
	m_bCaptureKeyboard_DataGrid = false;
	m_iCaptureKeyboard_PK_Variable = 0;
	m_sCaptureKeyboard_Text = "";
	m_sCaptureKeyboard_InternalBuffer = "";
	m_pCaptureKeyboard_Text = NULL;
	m_bBypassScreenSaver = false;
	m_bShiftDown = false;
	m_bControlDown = false;
	m_bAltDown = false;
	m_bCapsLock = false;
	m_pCacheImageManager = NULL;
	m_pScreenHistory_NewEntry = NULL;
	m_pMouseBehavior = NULL;

	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_VariableMutex.Init( &m_MutexAttr );
	m_DatagridMutex.Init( &m_MutexAttr );
	m_NeedRedrawVarMutex.Init( &m_MutexAttr );
	pthread_cond_init(&m_MaintThreadCond, NULL);
	m_MaintThreadMutex.Init(NULL,&m_MaintThreadCond);

	m_bUsingExternalScreenMutex = NULL != pExternalScreenMutex;
	if(m_bUsingExternalScreenMutex)
	{
		m_ScreenMutex.mutex = pExternalScreenMutex->mutex;
		m_ScreenMutex.m_pthread_cond_t = pExternalScreenMutex->m_pthread_cond_t;
		m_ScreenMutex.m_bInitialized = pExternalScreenMutex->m_bInitialized = true;
	}
	else
		m_ScreenMutex.Init( &m_MutexAttr );

	pthread_create(&m_MaintThreadID, NULL, MaintThread, (void*)this);
	pthread_detach(m_MaintThreadID);

	srand( (unsigned)time( NULL ) );  // For the screen saver
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter::~Orbiter()
//<-dceag-dest-e->
{
	WriteStatusOutput("Orbiter destructor");
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter  %p is exiting",this);

	//stop the simulator, if running
	if(Simulator::IsRunning())
		StopSimulatorThread();

	// Be sure we get the maint thread to cleanly exit
	KillMaintThread();

	PLUTO_SAFETY_LOCK( pm, m_MaintThreadMutex );
	map<int,PendingCallBackInfo *>::iterator itCallBackInfo;
	for(itCallBackInfo = m_mapPendingCallbacks.begin(); itCallBackInfo != m_mapPendingCallbacks.end(); itCallBackInfo++)
		delete (*itCallBackInfo).second;
	m_mapPendingCallbacks.clear();
	pm.Release();

	g_pPlutoLogger->Write(LV_STATUS,"Maint thread dead");

	if( m_pcRequestSocket ) // Will be NULL if we died without successfully getting our config
	{
		char *pData=NULL;int iSize=0;
		DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "0", 0, "", 0, &pData, &iSize );
		SendCommand( CMD_Orbiter_Registered );
	}

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
	DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
	for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
	{
		DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
		delete pObj;
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
		//ScreenHistory * psh = *itScreenHistory;
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

	for(map<int,Message *>::iterator itm=m_mapHardKeys.begin(); itm!=m_mapHardKeys.end();++itm)
	{
		Message *pMessage = itm->second;
		delete pMessage;
	}
	m_mapHardKeys.clear();

	delete m_pOrbiterFileBrowser_Collection;
	delete m_pMouseBehavior;

	if(m_pScreenHandler)
		delete m_pScreenHandler;

	vm.Release();
	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_VariableMutex.mutex);
	pthread_mutex_destroy(&m_DatagridMutex.mutex);
	pthread_mutex_destroy(&m_MaintThreadMutex.mutex);
	pthread_mutex_destroy(&m_NeedRedrawVarMutex.mutex);

	if(!m_bUsingExternalScreenMutex)
		pthread_mutex_destroy(&m_ScreenMutex.mutex);
}

//<-dceag-getconfig-b->!
bool Orbiter::GetConfig()
{
	WriteStatusOutput("GetConfig");

	int PK_Device=m_dwPK_Device;
	bool bResult=Orbiter_Command::GetConfig();
	if( PK_Device!=m_dwPK_Device && m_dwPK_Device )  // We have another device id or host ip address
	{
		Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(m_dwPK_Device);
		Simulator::GetInstance()->SaveConfigurationFile();
	}
	if( !bResult )
	{
		if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_CannotConnect && m_sHostName!="192.168.80.1" )
		{
			m_sHostName="192.168.80.1";
			if( !Orbiter_Command::GetConfig() )
			{
				WriteStatusOutput("Couldn't connect to the router.");
				return false;
			}
			else
			{
				if( m_dwPK_Device )
					Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(m_dwPK_Device);
				Simulator::GetInstance()->m_sRouterIP = m_sIPAddress;
				Simulator::GetInstance()->SaveConfigurationFile();
			}
		}
		else
		{
			WriteStatusOutput("Cannot connect.");
			return false;
		}
	}


	WriteStatusOutput("Got the config");

	if( DATA_Get_Leave_Monitor_on_for_OSD() )
		m_bDisplayOn=false;  // So the first touch will turn it on
	else
		m_bDisplayOn=true;  // The display should already be on

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"monitor m_bDisplayOn initially set to %d",(int) m_bDisplayOn);
#endif

	m_nSelectionBehaviour = DATA_Get_Using_Infrared();

	if(  !m_bLocalMode  )
	{
		GetEvents(  )->m_pClientSocket->SetReceiveTimeout( 15 );
		m_pcRequestSocket->m_pClientSocket->SetReceiveTimeout( 15 );
	}

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

	if(m_iCacheSize > 0)
		m_pCacheImageManager = new CacheImageManager(FileUtils::IncludeTrailingSlash(m_sCacheFolder) + StringUtils::ltos(m_dwPK_Device), m_iCacheSize);

	m_iVideoFrameInterval = DATA_Get_VideoFrameInterval();
	if(!m_iVideoFrameInterval) //this device data doesn't exist or it's 0
		m_iVideoFrameInterval = 6000; //6 sec

	m_pScreenHandler = CreateScreenHandler();

	return true;
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
void Orbiter::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
	if( m_bQuit )
		return;
	if( !m_pScreenHistory_Current || !m_pScreenHistory_Current->GetObj() )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Got attempt to render null screen: %s", m_pScreenHistory_Current );
		return;
	}

	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnRenderScreen);
	if(pCallBackData)
	{
		RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pCallBackData;
		pRenderScreenCallBackData->m_nPK_Screen = m_pScreenHistory_Current->PK_Screen();
		pRenderScreenCallBackData->m_pObj = m_pScreenHistory_Current->GetObj();
	}

	ExecuteScreenHandlerCallback(cbOnRenderScreen);

#ifndef WINCE
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s", m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif

#if ( defined( PROFILING ) )
	clock_t clkStart = clock();
#endif
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	if ( m_pScreenHistory_Current  )
	{
		RenderObject( m_pScreenHistory_Current->GetObj(),  m_pScreenHistory_Current->GetObj());
	}

	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		RenderPopup(*it, (*it)->m_Position);

	if( m_pScreenHistory_Current  )
	{
		for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.begin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
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
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s finished", m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif
}

//-----------------------------------------------------------------------------------------------------------
void Orbiter::RedrawObjects(  )
{
	PLUTO_SAFETY_LOCK(rm,m_NeedRedrawVarMutex);
	if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0 && m_bRerenderScreen==false)
		return; // Nothing to do anyway

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

	if( m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu )
	{
		CMD_Display_OnOff("0",false);
	}
	else
	{
		CMD_Set_Main_Menu("V");
		GotoScreen(m_sMainMenu);

		//make sure the display is on
		m_bDisplayOn = true;
	}
}

void Orbiter::Timeout( void *data )
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Screen %s timed out data: %p",m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),data);
#endif
	if( !data || data!=(void *) m_pScreenHistory_Current->GetObj() )
		return;

	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;
	if(  pObj->m_Action_TimeoutList.size(  )>0  )
		ExecuteCommandsInList( &pObj->m_Action_TimeoutList, pObj, smLoadUnload );
}

void Orbiter::ReselectObject( void *data )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;

	// Not used
	DesignObjZoneList::iterator iZone;
	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
	{
		DesignObjZone *pDesignObjZone = ( *iZone );
		ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, smLoadUnload, -1, -1, pObj->m_iRepeatParm+1 );
	}
}

void Orbiter::RedrawObject( void *iData )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) iData;
	if( !pObj || !pObj->m_bOnScreen )
		return;

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	NeedToRender render( this, "RedrawObject" );

	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) pObj;
		pGrid->bReAcquire=true;
	}

	RenderObjectAsync(pObj);
	CallMaintenanceInMiliseconds(pObj->m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);
}

void Orbiter::RealRedraw( void *data )
{
	if( m_bQuit )
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "In Redraw Objects: rerender %d",(int) m_bRerenderScreen );
#endif
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	// We don't have a good solution for rendering objects under popups--so re-render the whole screen if there are popups and we're going to render something else anyway
	if(  m_bRerenderScreen ||
		((m_listPopups.size() || (m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_listPopups.size())) &&
		(m_vectObjs_NeedRedraw.size() || m_vectTexts_NeedRedraw.size()) ))
	{
		if(m_pGraphicBeforeHighlight)
			UnHighlightObject(true);
		m_vectObjs_NeedRedraw.clear();
		m_vectTexts_NeedRedraw.clear();
		nd.Release();

		RenderScreen(  );
		if(NULL != m_pObj_Highlighted)
			DoHighlightObject();

		m_bRerenderScreen = false;
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "Exiting Redraw Objects" );
#endif
		return;
	}

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_CONTROLLER, "I won't render the whole screen. Objects to be rendered: %d, texts to be rendered: %d",	m_vectObjs_NeedRedraw.size(), m_vectTexts_NeedRedraw.size()	);
#endif

	if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0)
		return;

	BeginPaint();
	size_t s;

	bool bRehighlight=false;
	if(m_pGraphicBeforeHighlight && NULL != m_pObj_Highlighted && (m_pObj_Highlighted != m_pObj_Highlighted_Last))
	{
		bRehighlight=true;
		UnHighlightObject();
	}

	PlutoPoint AbsolutePosition = NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0);

	//render objects
	for( s = 0; s < m_vectObjs_NeedRedraw.size(); ++s )
	{
		class DesignObj_Orbiter *pObj = m_vectObjs_NeedRedraw[s];
		if(pObj && pObj->m_bOnScreen)
		{
			bool bIntersectedWith = pObj->m_rPosition.IntersectsWith(m_rectLastHighlight);
			bool bIncludedIn = pObj->m_rPosition.Contains(m_rectLastHighlight);

			if(!bRehighlight && (bIntersectedWith || bIncludedIn))
			{
				bRehighlight=true;
				UnHighlightObject();
			}
			RenderObject( pObj, m_pScreenHistory_Current->GetObj(), AbsolutePosition );
			UpdateRect(pObj->m_rPosition, AbsolutePosition);
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
				SolidRectangle( m_pActivePopup->m_Position.X + pText->m_rPosition.Left(),  m_pActivePopup->m_Position.Y + pText->m_rPosition.Top(), pText->m_rPosition.Width,  pText->m_rPosition.Height,  pTextStyle->m_BackColor);
			else
				SolidRectangle( pText->m_rPosition.Left(),  pText->m_rPosition.Top(), pText->m_rPosition.Width,  pText->m_rPosition.Height,  pTextStyle->m_BackColor);

			string TextToDisplay = SubstituteVariables(SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0).c_str();
			RenderText(TextToDisplay,pText, pTextStyle, AbsolutePosition);
			UpdateRect(pText->m_rPosition, AbsolutePosition);
		}
		else
		{
			RenderObjectAsync(pText->m_pObject);
		}
	}

	if(NULL != m_pObj_Highlighted && (bRehighlight || m_pObj_Highlighted != m_pObj_Highlighted_Last))
	{
		if(m_pObj_Highlighted->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED && m_pObj_Highlighted->m_vectHighlightedGraphic.size())
		{
			//this is a special kind of object; it has a highlighted version - saving the surface here
			//for un-highlighting will be too late (so we already did it in RenderObject)
			g_pPlutoLogger->Write(LV_STATUS, "Object already highlighted %s", m_pObj_Highlighted->m_ObjectID.c_str());
		}
		else
		{
			m_pObj_Highlighted_Last = m_pObj_Highlighted;
			DoHighlightObject();
		}
	}

	m_vectObjs_NeedRedraw.clear();
	m_vectTexts_NeedRedraw.clear();
	EndPaint();
}
//-----------------------------------------------------------------------------------------------------------
void Orbiter::RenderObject( DesignObj_Orbiter *pObj,  DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	//g_pPlutoLogger->Write(LV_CRITICAL, "Render object %s", pObj->m_ObjectID.c_str());

	if(pObj->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
	{
		CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnWxWidgetRefresh);
		if(pCallBackData)
		{
			PositionCallBackData *pPositionData = (PositionCallBackData *)pCallBackData;
			pPositionData->m_rectPosition = pObj->m_rPosition;
		}

		if(ExecuteScreenHandlerCallback(cbOnWxWidgetRefresh))
			return;
	}

	if(  pObj->m_pDesignObj_Orbiter_TiedTo  )
	{
		pObj->m_bHidden = pObj->m_pDesignObj_Orbiter_TiedTo->IsHidden(  );
		if(  ( pObj->m_iRowTiedTo==-1 && pObj->m_iColumnTiedTo==-1 ) || pObj->m_pDesignObj_Orbiter_TiedTo->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST  )
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

	if ( pObj->m_bHidden || pObj->m_rPosition.Width==0 || pObj->m_rPosition.Height==0 )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "object: %s  not visible: %d", pObj->m_ObjectID.c_str(), (int) pObj->m_bHidden );
#endif
		if(m_bShowShortcuts && pObj->m_iPK_Button)
			RenderShortcut(pObj);

		return;
	}

	PROFILE_START( ctObj )

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
	PlutoRectangle rectBackground = pObj->m_rBackgroundPosition;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	vm.Release(  );

	if( (pObj == m_pObj_Highlighted || pObj->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ||
		(m_pObj_Highlighted && pObj->m_bTabStop && pObj==m_pObj_Highlighted->m_pParentObject)  // If I'm also a tab stop, and my child is a highlighted object, leave me highlighted too
		) && pObj->m_vectHighlightedGraphic.size() )
	{
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectHighlightedGraphic);

		//we'll need to do the highlighting here, since we need the normal surface for un-highlighting
		//we won't do it on RealRedraw for this kind of objects
		if(NULL != m_pObj_Highlighted && m_pObj_Highlighted != m_pObj_Highlighted_Last)
		{
			m_pObj_Highlighted_Last = m_pObj_Highlighted;
			DoHighlightObject();
		}
	}
	else if(pObj->m_GraphicToDisplay == GRAPHIC_SELECTED && pObj->m_vectSelectedGraphic.size())
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectSelectedGraphic);
	else if(  pObj->m_GraphicToDisplay >= 1 && pObj->m_GraphicToDisplay <= int(pObj->m_vectAltGraphics.size()))
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectAltGraphics[pObj->m_GraphicToDisplay - 1]);
	else // Something that was invalid was chosen,  or just the normal version
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

	if(  !pObj->m_pvectCurrentGraphic && pObj->m_GraphicToDisplay != GRAPHIC_NORMAL  )
		pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

	// This is somewhat of a hack, but we don't have a clean method for setting the graphics on the user & location buttons to
	if( pObj->m_bIsBoundToUser )
	{
		vector<PlutoGraphic*> *pvectGraphic = m_mapUserIcons[m_dwPK_Users];

		// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
		g_pPlutoLogger->Write(LV_STATUS,"Orbiter::RenderObject obj %s bound to user %d, got ptr %p",
			pObj->m_ObjectID.c_str(),m_dwPK_Users,pvectGraphic);

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
		RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );
		pObj->m_pvectCurrentGraphic = pvectGraphic_Hold;

		vectGraphicToUndoSelect.clear();
		delete pObj->m_pGraphicToUndoSelect;
		pObj->m_pGraphicToUndoSelect=NULL;
	}
	else if(  pObj->m_pvectCurrentGraphic  )
		RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );

	switch( pObj->m_ObjectType )
	{
	case DESIGNOBJTYPE_Rectangle_CONST:
		// todo 2.0     SolidRectangle( rectTotal.X,  rectTotal.Y,  rectTotal.Width,  rectTotal.Height,  atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Cell_Color_CONST ).c_str(  ) ),  atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Transparency_CONST ).c_str(  ) ) );
		break;
	case DESIGNOBJTYPE_Datagrid_CONST:
		RenderDataGrid( ( DesignObj_DataGrid * )pObj, point );
		break;
	case DESIGNOBJTYPE_Floorplan_CONST:
		RenderFloorplan(pObj,pObj_Screen, point);
		break; // Render the child objects with their text
	case DESIGNOBJTYPE_Web_Browser_CONST:
		//      LocalHandleWebWindow( pObj,  rectTotal );
		break;
	case DESIGNOBJTYPE_App_Desktop_CONST:
	case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
		if ( m_bYieldScreen )
			RenderDesktop( pObj, PlutoRectangle( 0, 0, -1, -1 ), point );  // Full screen
		else
			RenderDesktop( pObj, pObj->m_rPosition, point );
		break;
#ifdef PRONTO
	case DESIGNOBJTYPE_Pronto_File_CONST:
		if ( pObj->m_pCCF == NULL )
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

						pObj->m_pCCF = new ProntoCCF( this,  pMessage->m_mapData_Parameters[1],  pMessage->m_mapData_Lengths[1],  pObj,  IRDevice );
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
		if ( pObj->m_pCCF )
		{
			pObj->m_pCCF->RenderPronto( XOffset,  YOffset, point );
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
	}

	PROFILE_STOP( ctObj,  pObj->m_ObjectID.c_str(  ) )

	DesignObj_DataList::reverse_iterator iHao;

	//#pragma warning( "todo - nasty hack -- on the hard drive screen,  you select music,  then when you go to video,  it redraws the video section,  but then redraws the deselected music with m_pGraphicToUndoSelect,  which includes a snapshot of the prior highlighters,  that are overwriting the new ones.  Redraw these first!  After render object won't draw anything" )
	for( iHao=pObj->m_ChildObjects.rbegin(  ); iHao != pObj->m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pObj->m_pGraphicToUndoSelect || pObj->m_GraphicToDisplay!=GRAPHIC_NORMAL  )
			continue;
		RenderObject( pDesignObj_Orbiter,  pObj_Screen, point );
	}


	///#pragma warning( "todo - nasty hack -- the datagrid highlighters need to be drawn after the music button.  Until I can implement layering,  scan twice and do the tied to's last" )
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

	VectorDesignObjText::iterator iText;
	for( iText=pObj->m_vectDesignObjText.begin(  ); iText != pObj->m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;
		if(  pText->m_bPreRender  )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = SubstituteVariables(SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		RenderText( TextToDisplay, pText, pTextStyle, point );
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}
	if( pObj->m_pFloorplanObject && m_mapDevice_Selected.find(pObj->m_pFloorplanObject->PK_Device)!=m_mapDevice_Selected.end() )
	{
		int i;
		for(i = 0; i < 4; ++i)
			HollowRectangle(point.X + pObj->m_rBackgroundPosition.X-i, point.Y + pObj->m_rBackgroundPosition.Y-i, pObj->m_rBackgroundPosition.Width+i+i, pObj->m_rBackgroundPosition.Height+i+i,
			(i==1 || i==2 ? PlutoColor::Black() : PlutoColor::White()));

		//force an update because the object boundaries are not respected
		PlutoRectangle rect(point.X + pObj->m_rBackgroundPosition.X-i, point.Y + pObj->m_rBackgroundPosition.Y-i, pObj->m_rBackgroundPosition.Width+i+i, pObj->m_rBackgroundPosition.Height+i+i);
		UpdateRect(rect, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));
	}

	if(m_bShowShortcuts && pObj->m_iPK_Button)
		RenderShortcut(pObj);
}
//-----------------------------------------------------------------------------------------------------------
/*virtual*/bool Orbiter::RenderCell( class DesignObj_DataGrid *pObj,  class DataGridTable *pT,  class DataGridCell *pCell,  int j,  int i,  int GraphicToDisplay, PlutoPoint point )
{
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
		Text.m_rPosition = PlutoRectangle( x,  y,  w,  h );
		//pTextStyle->m_BackColor = PlutoColor( 0, 0, 0, 255 );
		//pTextStyle->m_ForeColor = PlutoColor( 255, 255, 255, 255 );
		//pTextStyle->m_iPK_VertAlignment=2;

		Text.m_iPK_HorizAlignment = pTextStyle->m_iPK_HorizAlignment;
		Text.m_iPK_VertAlignment = pTextStyle->m_iPK_VertAlignment;

		string sText = SubstituteVariables(pCell->GetText(  ),pObj,0,0);
		RenderText( sText, &Text, pTextStyle, point );
	}
	else
		g_pPlutoLogger->Write( LV_WARNING,  "Datagrid width or height is too small" );


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
	string sSelectedIndex = "0";
	if ( !pObj->sSelVariable.empty(  ) )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		delSelections = "|" + m_mapVariable[atoi(pObj->sSelVariable.c_str())] + "|";
		sSelectedIndex = m_mapVariable[atoi(pObj->sSelVariable.c_str())];
		vm.Release(  );
	}

#if ( defined( PROFILING_GRID ) )
	clock_t clkStart = clock(  );
#endif

	PrepareRenderDataGrid( pObj,  delSelections );

#if ( defined( PROFILING_GRID ) )
	clock_t clkAcquired = clock(  );
#endif

	if( !pObj->m_pDataGridTable )
		return;

	SolidRectangle( point.X + pObj->m_rPosition.X, point.Y + pObj->m_rPosition.Y, pObj->m_rPosition.Width, pObj->m_rPosition.Height, PlutoColor( 0, 0, 0 ) );

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
					{
						GraphicType = GRAPHIC_SELECTED;
						if( pObj->m_bHighlightSelectedCell )
						{
							pObj->m_iHighlightedRow = pObj->m_sExtraInfo.find( 'C' )==string::npos ? DGRow : -1;
							pObj->m_iHighlightedColumn = pObj->m_sExtraInfo.find( 'R' )==string::npos ? DGColumn : -1;
							pObj->m_bHighlightSelectedCell=false; // Only on the initial draw
						}
					}

					RenderCell( pObj,  pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GraphicType, point );
				}
				else
					RenderCell( pObj,  pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GRAPHIC_NORMAL, point );

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

	OnSelectedCell(pObj, pT, NULL);
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
/*virtual*/ void Orbiter::NeedToChangeScreens( ScreenHistory *pScreenHistory/*, bool bAddToHistory*/ )
{
	if(m_bQuit)
		return;

	m_bShiftDown = false;

#ifdef DEBUG
	if(pScreenHistory)
		g_pPlutoLogger->Write(LV_STATUS,"Need to change screens executed to %s",
		pScreenHistory->GetObj()->m_ObjectID.c_str());
#endif

	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	m_vectObjs_GridsOnScreen.clear(  );
	m_vectObjs_VideoOnScreen.clear(  );
	m_pObj_NowPlayingOnScreen=NULL;
	m_pObj_NowPlaying_Section_OnScreen=NULL;
	m_pObj_NowPlaying_TimeShort_OnScreen=NULL;
	m_pObj_NowPlaying_TimeLong_OnScreen=NULL;
	m_pObj_NowPlaying_Speed_OnScreen=NULL;

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

		if( m_pScreenHistory_Current->GetObj()==m_pDesignObj_Orbiter_ScreenSaveMenu && m_pDesignObj_Orbiter_MainMenu!=m_pDesignObj_Orbiter_ScreenSaveMenu )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_WARNING,"Goto Screen -- wakign up from screen saver");
#endif
			CMD_Set_Main_Menu("N");
		}

		m_pScreenHistory_Current->GetObj()->m_bActive=false;
		ObjectOffScreen( m_pScreenHistory_Current->GetObj() );
		for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.begin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
		{
			PlutoPopup *pPlutoPopup = *it;
			ObjectOffScreen( pPlutoPopup->m_pObj );
			if( m_pActivePopup==pPlutoPopup )
				m_pActivePopup=NULL;
		}

		if(
			!m_listScreenHistory.size() ||
			(m_pScreenHistory_Current != pScreenHistory && m_listScreenHistory.back()->PK_Screen() != m_pScreenHistory_Current->PK_Screen())
			)
		{
			//update the list only if the screen is changed
			PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );

			if(ScreenHistory::m_bAddToHistory)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Adding to screens history list screen %d", m_pScreenHistory_Current->PK_Screen());
				m_listScreenHistory.push_back( m_pScreenHistory_Current );
			}

			if ( m_listScreenHistory.size(  ) > 64 )
			{
				delete m_listScreenHistory.front();
				m_listScreenHistory.pop_front(  );
			}
		}
	}

	//restoring variables and visibility status for objects
	if(m_pContextToBeRestored)
	{
		map<DesignObj_Orbiter *, bool> mapVisibilityContext;
		m_pContextToBeRestored->RestoreContext(m_mapVariable, mapVisibilityContext);
		SetChildrenVisibilityContext(m_pContextToBeRestored->GetObj(), mapVisibilityContext);
		m_pContextToBeRestored = NULL;
	}

	ScreenHistory::m_bAddToHistory = true;

	// todo 2.0 SelectFirstObject(  );
	if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && pScreenHistory->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutBlank )
	{
		m_tTimeoutTime = time(NULL) + m_iTimeoutBlank;

#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "About to call maint for display off with timeout: %d ms", m_iTimeoutBlank * 1000);
#endif

		CallMaintenanceInMiliseconds( m_iTimeoutBlank * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}
	else if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && pScreenHistory->GetObj() != m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutScreenSaver )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "About to call maint for screen saver with timeout: %d ms", m_iTimeoutScreenSaver * 1000);
#endif
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}

	m_pScreenHistory_Current=pScreenHistory;
	m_pScreenHistory_Current->GetObj()->m_bActive=true;
	if( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds )
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->GetObj(), pe_ALL );

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "Changing screen to %s ir %d type %c",
		m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ), m_dwPK_Device_LocalOsdIRReceiver, m_pScreenHistory_Current->GetObj()->m_cScreenType);
#endif
	if( m_dwPK_Device_LocalOsdIRReceiver )
	{
		DCE::CMD_Set_Screen_Type CMD_Set_Screen_Type(m_dwPK_Device,m_dwPK_Device_LocalOsdIRReceiver,m_pScreenHistory_Current->GetObj()->m_cScreenType);
		SendCommand(CMD_Set_Screen_Type);
	}
	ObjectOnScreenWrapper(  );

	//if we are now in main menu, we'll purge the history; this is the initial nod from the navigation "tree"
	if(m_pScreenHistory_Current->GetObj()->m_iBaseObjectID == atoi(m_sMainMenu.c_str()))
	{
		list<ScreenHistory *>::iterator it_end = m_listScreenHistory.end(); //cache the end of the list
		for(list<ScreenHistory *>::iterator it = m_listScreenHistory.begin(); it != it_end; ++it)
			if(m_pScreenHistory_Current != *it)
				delete *it;

		m_listScreenHistory.clear();
	}

#ifdef DEBUG
	DumpScreenHistory();
#endif
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
	m_mapFloorplanObject_Selected.clear();
	vm.Release(  );

	//also reset the last selected object from the floorplan.
	m_pObj_LastSelected = NULL;

	ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, m_pScreenHistory_Current->GetObj() );
	for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.begin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
	{
		PlutoPopup *pPlutoPopup = *it;
		ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pPlutoPopup->m_pObj );
	}

	switch(m_nSelectionBehaviour)
	{
	case sbAllowsSelected:
		HighlightFirstObject();
		break;

	case sbSelectOnFirstMove:
	case sbNoSelection:
		m_pObj_Highlighted=NULL;
		break;
	}

	// Do the on load actions for the screen itself,  and objects on it
	ExecuteCommandsInList( &m_pScreenHistory_Current->GetObj()->m_Action_LoadList, m_pScreenHistory_Current->GetObj(), smLoadUnload, 0, 0 );
	HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );
}
//------------------------------------------------------------------------
// If an object has the don't reset state true,  it won't reset to normal,  and it's children won't reset either
void Orbiter::ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup )
{
	if(pObj->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
	{
		CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnWxWidgetCreate);
		if(pCallBackData)
		{
			PositionCallBackData *pPositionData = (PositionCallBackData *)pCallBackData;
			pPositionData->m_rectPosition = pObj->m_rPosition;
		}

		if(ExecuteScreenHandlerCallback(cbOnWxWidgetCreate))
			return;
	}

	// Do this again since sometimes there will be several grids with the same name within the application and if
	// we're going to do a lookup, such as with seek grid, we want to find the one most recently on screen
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pObj_Datagrid = (DesignObj_DataGrid *) pObj;
		m_mapObjs_AllGrids[pObj_Datagrid->m_sGridID] = pObj_Datagrid;
	}
	if( pObj->m_iRegenInterval )
		CallMaintenanceInMiliseconds(pObj->m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);

	pVectDesignObj_Orbiter->push_back( pObj );
	pObj->m_bOnScreen=true;

	// Add it to the list of tab stops whether it's visible or not.  The findfirst/next will skip of hidden objects anyway
	// And this way we don't need to worry about the changing state of objects that are hidden/shown
	if(  pObj->m_bTabStop  )
		m_vectObjs_TabStops.push_back( pObj );

	if( ptPopup )
		pObj->m_pPopupPoint = *ptPopup;

	if( pObj->m_ObjectType==DESIGNOBJTYPE_Broadcast_Video_CONST )
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
		ObjectOnScreen( pVectDesignObj_Orbiter, ( DesignObj_Orbiter * )( *iHao ), ptPopup );
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
	if(pObj->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST && ExecuteScreenHandlerCallback(cbOnWxWidgetDelete))
		return;

	pObj->m_bOnScreen=false;
	if(  pObj->m_pGraphicToUndoSelect  )
	{
		delete pObj->m_pGraphicToUndoSelect;
		pObj->m_pGraphicToUndoSelect=NULL;
	}

	if( !pObj->m_bKeepGraphicInCache )
	{
		GraphicOffScreen( &(pObj->m_vectGraphic) );
		GraphicOffScreen( &(pObj->m_vectSelectedGraphic) );
		GraphicOffScreen( &(pObj->m_vectHighlightedGraphic) );

		size_t i;
		for(i = 0; i < pObj->m_vectAltGraphics.size(); ++i)
			GraphicOffScreen(&(pObj->m_vectAltGraphics[i]));
	}

	pObj->m_pvectCurrentGraphic = NULL;
	pObj->m_pvectCurrentPlayingGraphic = NULL;

	ExecuteCommandsInList( &pObj->m_Action_UnloadList, pObj, smLoadUnload, 0, 0 );

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		ObjectOffScreen( ( DesignObj_Orbiter * )*iHao );
	}
}

//------------------------------------------------------------------------
void Orbiter::SelectedObject( DesignObj_Orbiter *pObj,  SelectionMethod selectionMethod, int X,  int Y)
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

	//it's the background object? it's not our object!
	if(NULL != pObj->m_pParentObject)
		m_pObj_SelectedLastScreen = pObj;

	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbObjectSelected);
	if(pCallBackData)
	{
		ObjectInfoBackData *pObjectData = (ObjectInfoBackData *)pCallBackData;
		pObjectData->m_PK_DesignObj_SelectedObject = pObj->m_iBaseObjectID;
	}

	if(ExecuteScreenHandlerCallback(cbObjectSelected))
		return;

	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST )
	{
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		if( X>=0 && Y>=0 )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write( LV_WARNING,  "Selected datagrid: %s", pObj->m_ObjectID.c_str(  ) );
#endif
			if ( !SelectedGrid( ( DesignObj_DataGrid * ) pObj,  X-pObj->m_rPosition.X,  Y-pObj->m_rPosition.Y ) )
				return;
		}
		else if( pObj->m_pDataGridTable )
		{
			DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) pObj;
			PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Enter key press. Status: iHighlightedColumn %d, GridCurCol %d, iHighlightedRow %d, GridCurRow %d",
				pDesignObj_DataGrid->m_iHighlightedColumn, pDesignObj_DataGrid->m_GridCurCol,
				pDesignObj_DataGrid->m_iHighlightedRow, pDesignObj_DataGrid->m_GridCurRow);
#endif
			DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(
				pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol,
				pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow - (pDesignObj_DataGrid->HasMoreUp() ? 1 : 0) : 0);
			if(pCell)
			{
				SelectedGrid(pDesignObj_DataGrid, pCell);
				RenderObjectAsync(pDesignObj_DataGrid);
			}
		}
	}

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_WARNING,  "Selected objs: %s with %d zones", pObj->m_ObjectID.c_str(  ), ( int ) pObj->m_ZoneList.size(  ) );
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
			ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, selectionMethod, X, Y );
		}
	}

	if(  pObj->m_vectSelectedGraphic.size() && pObj->m_GraphicToDisplay != GRAPHIC_SELECTED ) // TODO 2.0 && m_ChangeToScreen.length(  ) == 0 )
	{
		pObj->m_GraphicToDisplay=GRAPHIC_SELECTED;
		RenderObjectAsync(pObj);

		if(pObj->m_vectSelectedGraphic.size())
		{
			PlutoGraphic *pPlutoGraphic = pObj->m_vectSelectedGraphic[0];

			if(pPlutoGraphic->m_GraphicFormat != GR_MNG)
			{
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
				if(  !pObj->m_bDontResetState  )
					CallMaintenanceInMiliseconds( 500, &Orbiter::DeselectObjects, ( void * ) pObj, pe_NO );
			}
		}

		// Unless the screen's don't reset state is set,  we'll clear any other selected graphics
		if(  !m_pScreenHistory_Current->GetObj()->m_bDontResetState  )
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
					RenderObjectAsync(pObj_Sel);
				}
			}
			m_vectObjs_Selected.clear(  );
		}
		m_vectObjs_Selected.push_back( pObj );
	}

#ifdef PRONTO
	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Pronto_File_CONST )
	{
		if ( pObj->m_pCCF )
			pObj->m_pCCF->Touch( X-pObj->m_rPosition.X,  Y-pObj->m_rPosition.Y );
		return;
	}
#endif

	//#pragma warning( "bad,  bad hack" )
	if(  pObj->m_iBaseObjectID==2706  )
	{
		// There's a problem when you select the video button.  The music button was already selected,  and saved a snapshop in the 'savebackgroundfordeselect'.
		// The new video objects are drawn,  then the deselected music object,  which includes a snapshot of the music datagrid.  If I put refresh in the video object
		// it has no effect,  since the commands are executed before the deselect state.  Until we find a solution,  hack in a redraw after resetting the video state
		RenderObjectAsync(m_pScreenHistory_Current->GetObj());
	}
	/*
	string VariableData = "";
	map<string, string>::iterator ivv;
	for( ivv=pObj->m_VariableValue.begin(  );ivv!=pObj->m_VariableValue.end(  );++ivv )
	{
	if ( VariableData.length(  )==0 )
	VariableData = StringUtils::itos( pObj->m_VariableValue.size(  ) );

	VariableData+="|";
	VariableData+=( *ivv ).first+"|"+( *ivv ).second;
	m_mapVariable[atoi( ( *ivv ).first.c_str(  ) )] = ( *ivv ).second;
	}
	*/

	/* todo 2.0
	if( pObj->m_bProcessActionsAtServer )
	{
	Message *pThisMessage = new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_PROCESS_BUTTON_AT_SERVER_CONST, 3,
	COMMANDPARAMETER_PK_DesignObj_CONST, StringUtils::itos( pObj->m_iBaseObjectID ),
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
	C_EVENTPARAMETER_XPOS_CONST, StringUtils::itos( X-pObj->m_rPosition.X ).c_str(  ),
	C_EVENTPARAMETER_YPOS_CONST, StringUtils::itos( Y-pObj->m_rPosition.Y ).c_str(  ) );

	pButtonTouchMessage->m_vectExtraMessages.push_back( pThisMessage );

	QueueMessageToRouter( pButtonTouchMessage );
	//      GetEvents(  )->Orbiter_Object_Actions( StringUtils::itos( NumActions ) + "\t" +ActionData,
	//          pObj->m_ObjectID,  StringUtils::itos( CTRLCOMMAND_ACTIVATE ),  StringUtils::itos( X-pObj->m_rPosition.X ),
	//          StringUtils::itos( Y-pObj->m_rPosition.Y ) );
	}
	*/

	if( X<0 && Y<0 )
	{
		// This wasn't selected by a touch.  With a touch all objects under that area are selected
		// automatically.  So if there are keys on a keyboard, both the key, and the keyboard are
		// selected.  If it wasn't selected by touch, such as by i/r or keyboard, we should select
		// our parent object's as well
		if( pObj->m_pParentObject )
			SelectedObject((DesignObj_Orbiter *)pObj->m_pParentObject,selectionMethod);
	}
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
					int ContainsX=X+pDesignObj_DataGrid->m_rPosition.X;
				int ContainsY=Y+pDesignObj_DataGrid->m_rPosition.Y;
				vm.Release(  );
				if ( PlutoRectangle( x,  y,  w,  h ).Contains( ContainsX,  ContainsY ) )
				{
					if(  i==pDesignObj_DataGrid->m_dwIDownRow  )
					{
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
						if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, "");
						}
						dg.Release();
						CMD_Scroll_Grid( "",  pDesignObj_DataGrid->m_ObjectID,  DIRECTION_Down_CONST );
						return false;  // No more processing
					}
					else
					{
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
						if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, "");
						}
						dg.Release();
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
					int ContainsX=X+pDesignObj_DataGrid->m_rPosition.X;
				int ContainsY=Y+pDesignObj_DataGrid->m_rPosition.Y;
				vm.Release(  );
				if ( ( delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos ) )
				{
					if ( bFoundSelection )
					{
						if ( RenderCell( ( DesignObj_DataGrid * )pDesignObj_DataGrid,  pT,  pCell,  j,  i ,  GRAPHIC_NORMAL ) )
						{
							RenderObjectAsync(pDesignObj_DataGrid);
						}
						bFinishLoop = true;
					}
					pLastCell = pCell;
					LastI = i;
					LastJ = j;
				}

				if ( PlutoRectangle( x,  y,  w,  h ).Contains( ContainsX,  ContainsY ) )
				{
					pDesignObj_DataGrid->m_iHighlightedColumn=DGColumn;

					if(pDesignObj_DataGrid->m_pDataGridTable->m_StartingRow)
						pDesignObj_DataGrid->m_iHighlightedRow = DGRow - pDesignObj_DataGrid->m_pDataGridTable->m_StartingRow + 1;

					SelectedGrid( pDesignObj_DataGrid,  pCell );
					bFinishLoop = true;
					bFoundSelection = true; // Is this correct????  Hacked in this time
					RenderObjectAsync(pDesignObj_DataGrid);
				}
			}
		}
	}

	return bFoundSelection;
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  DataGridCell *pCell )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbDataGridSelected);
	if(pCallBackData)
	{
		DatagridCellBackData *pCellData = (DatagridCellBackData *)pCallBackData;
		pCellData->m_sText = pCell->GetText();
		pCellData->m_sValue = pCell->GetValue();
		pCellData->m_nPK_Datagrid = pDesignObj_DataGrid->m_iPK_Datagrid;
	}

	if(ExecuteScreenHandlerCallback(cbDataGridSelected))
		return true;

	// AB 2005-08-20 is this necessary?  It means you don't stay on the highlighted selection	pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
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
					string::size_type posH;
					if(  (posH=pDesignObj_DataGrid->m_sExtraInfo.find( 'H' ))==string::npos && pDesignObj_DataGrid->sSelVariable.length()==0 )
					{
						pDesignObj_DataGrid->m_iHighlightedRow = (posH==pDesignObj_DataGrid->m_sExtraInfo.length() || pDesignObj_DataGrid->m_sExtraInfo[posH+1]!='C') ? 0 : -1;
						pDesignObj_DataGrid->m_iHighlightedColumn = (posH==pDesignObj_DataGrid->m_sExtraInfo.length() || pDesignObj_DataGrid->m_sExtraInfo[posH+1]!='R') ? 0 : -1;
					}
					else
					{
						pDesignObj_DataGrid->m_iHighlightedRow=-1;
						pDesignObj_DataGrid->m_iHighlightedColumn=-1;
					}
					bRefreshGrids=true;
				}
			}

			string Response;

			// This message has out parameters with a variable to assign
			Message *pMessage_Out = m_pcRequestSocket->SendReceiveMessage( pMessage );
			if(  pMessage_Out  )
			{
				int iPK_Variable = atoi( pMessage_Out->m_mapParameters[COMMANDPARAMETER_PK_Variable_CONST].c_str(  ) );

				if(iPK_Variable)
				{
					CMD_Set_Variable(iPK_Variable, pMessage_Out->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST]);
				}
				else
				{
#ifdef DEBUG
					g_pPlutoLogger->Write(LV_WARNING, "NOT setting the datagrid variable. It's 0!");
#endif
				}

				delete pMessage_Out;
			}
			pDesignObj_DataGrid->m_GridCurCol = pDesignObj_DataGrid->m_iInitialColNum;
			pDesignObj_DataGrid->m_GridCurRow = pDesignObj_DataGrid->m_iInitialRowNum;
			pDesignObj_DataGrid->bReAcquire=true;

			if(  bRefreshGrids  )
			{
				RenderObjectAsync(pDesignObj_DataGrid);
			}
		}
		else if( pMessage->m_dwPK_Device_To==m_dwPK_Device || pMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
		{
			pMessage->m_dwPK_Device_To=m_dwPK_Device;
			ReceivedMessage(pMessage);
		}
		else
		{
			//#pragma warning( "Look into this and the sendreceive above.  one deletes,  the other not.  if it deletes,  you can't select the grid twice.  if not,  the framework will delete.  should make a copy I think" );
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
		//DataGridTable *pT = pDesignObj_DataGrid->m_pDataGridTable;

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

	CMD_Set_Variable(PK_Variable, NewValue);
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

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if(m_pObj_Highlighted == pDesignObj_DataGrid) //datagrid already highlighted. remove first old highlighting
		UnHighlightObject();

	m_pObj_Highlighted = pDesignObj_DataGrid;
	pDesignObj_DataGrid->m_iHighlightedColumn = -1;
	pDesignObj_DataGrid->m_iHighlightedRow = DGRow;
	pDesignObj_DataGrid->m_GridCurCol = iSelectedColumn;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "Selected row: %d, selected column: %d",
		DGRow, iSelectedColumn);
#endif

	if( pCell )
		CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, pCell->GetValue());

	return true;
}
void Orbiter::SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter)
{
	if( pDesignObj_Orbiter->m_pFloorplanObject )
		SelectedFloorplan(pDesignObj_Orbiter);

	if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butPINGo_CONST)
	{
		if( m_bRestrictedOp_IsUser )
		{
			bool bValid=false;
			DCE::CMD_Verify_PIN CMD_Verify_PIN(m_dwPK_Device, m_dwPK_Device_SecurityPlugIn,
				m_iRestrictedOp_ID,m_mapVariable[VARIABLE_Datagrid_Input_CONST],&bValid);
			if( SendCommand(CMD_Verify_PIN) && bValid )
			{
				CMD_Set_Current_User(m_iRestrictedOp_ID);
				CMD_Goto_DesignObj(0,m_sMainMenu,"","",false,false);
			}
			else
			{
				CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
				CMD_Set_Text(m_pScreenHistory_Current->GetObj()->m_ObjectID,"***INVALID***",TEXT_PIN_Code_CONST);
			}
		}
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_mnuPingPong_CONST)
	{
		Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_START_PING,m_dwPK_Device,0);
		QueueMessageToRouter(pMessage);
		g_pReceivePingHandler=ReceivePingHandler;
		g_pSendPingHandler=SendPingHandler;
		string sMessage = "Starting PING/PONG S" + StringUtils::itos(m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(m_MaintThreadMutex.m_NumLocks)
			+ " R" + StringUtils::itos(m_NeedRedrawVarMutex.m_NumLocks);
		CMD_Set_Text(pDesignObj_Orbiter->m_ObjectID,sMessage,TEXT_STATUS_CONST);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butStopPing_CONST)
	{
		Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_STOP_PING,m_dwPK_Device,0);
		QueueMessageToRouter(pMessage);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_mnuPower_CONST)
	{
		if( !m_pLocationInfo->m_dwPK_Device_MediaDirector || m_pLocationInfo->m_dwPK_Device_MediaDirector==DEVICEID_NULL )
		{
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayON_OtherControlling_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
			CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
		}
		else
		{
			// We know we've got an m/d and it's not us.  Find out the state and status
			string sState = GetState(m_pLocationInfo->m_dwPK_Device_MediaDirector);
			string sStatus = GetStatus(m_pLocationInfo->m_dwPK_Device_MediaDirector);

			if( sStatus.length()>1 && sStatus.substr(0,2)=="MD" )
			{
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
				// See if we've got media playing
				if( m_sNowPlaying.length() )
					CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "1" );
				else
					CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			}
			else
			{
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
			}

			// See if it's on.  If so, enable the halt and displayoff buttons
			if( sStatus.length()>3 && (sStatus.substr(3) == "ON" || sStatus.substr(3) == "BLACK") )
			{
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "1" );
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "1" );
			}
			else
			{
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
				CMD_Show_Object( m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
			}
		}
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_mnuScreenSaver_CONST )
	{
		CMD_Set_Text(pDesignObj_Orbiter->m_ObjectID, StringUtils::itos(	int(m_tTimeoutTime - time(NULL)) ) + " seconds",TEXT_USR_ENTRY_CONST);
		CMD_Continuous_Refresh("10");
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_objHouseStatusIndicator_CONST )
	{
		// This is actually at startup time.  Figure out what device group this is controlling
		int PK_DeviceGroup=0;
		for(DesignObjZoneList::iterator itZ=pDesignObj_Orbiter->m_ZoneList.begin();itZ!=pDesignObj_Orbiter->m_ZoneList.end();++itZ)
		{
			DesignObjZone *pDesignObjZone = *itZ;

			for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
			{
				DesignObjCommand *pDesignObjCommand = *it;
				if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
				{
					int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
					if( PK_Variable == VARIABLE_PK_DeviceGroup_CONST )
						PK_DeviceGroup = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				}
			}
		}
		CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "housemode" + StringUtils::itos(PK_DeviceGroup),true);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butUserStatus_CONST )
	{
		// This is actually at startup time.  Figure out what user this is controlling
		int PK_Users=0;
		for(DesignObjZoneList::iterator itZ=pDesignObj_Orbiter->m_ZoneList.begin();itZ!=pDesignObj_Orbiter->m_ZoneList.end();++itZ)
		{
			DesignObjZone *pDesignObjZone = *itZ;

			for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
			{
				DesignObjCommand *pDesignObjCommand = *it;
				if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
				{
					int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
					if( PK_Variable == VARIABLE_PK_Users_CONST )
						PK_Users = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				}
			}
		}
		CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "user" + StringUtils::itos(PK_Users),true);
	}
}

void Orbiter::SelectedFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter)
{
//	m_pScreenHistory_Current->m_dwPK_Device = pDesignObj_Orbiter->m_pFloorplanObject->PK_Device;
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
			RenderObjectAsync((DesignObj_Orbiter *) pDesignObj_Orbiter->m_pParentObject);

			m_mapDevice_Selected.clear();
			m_mapFloorplanObject_Selected.clear();
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
				m_mapFloorplanObject_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject;
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
		{
			m_mapDevice_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base;
			m_mapFloorplanObject_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject;
		}
		m_pObj_LastSelected=pDesignObj_Orbiter;
	}

	// The toolbar will be a direct child of the topmost object, whether it's a screen or a popup
	if( PK_DesignObj_Toolbar_ToTurnOn )
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOn),0,"","","1");
	if( PK_DesignObj_Toolbar_ToTurnOff )
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOff),0,"","","0");

	/*
	if( pDesignObj_Orbiter->m_pParentObject->m_iBaseObjectID==OBJECT_FPENTERTAINMENT_CONST )
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
	/*
	if( pObj->m_ObjectID.find("1255.2.0.3407")!=string::npos )
	{
	int k=2;
	}
	*/
	if(
		pObj->m_iPK_Button == PK_Button ||
		pObj->m_iPK_Button == BUTTON_Any_key_CONST ||
		(
		pObj->m_iPK_Button == BUTTON_Alphanumeric_keys_CONST &&
		(
		(PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST) ||
		(PK_Button >= BUTTON_A_CONST && PK_Button <= BUTTON_Z_CONST) ||
		(PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST)
		)
		)
		)
	{
		SelectedObject( pObj, smKeyboard );
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

	if( pObj->m_rPosition.Contains( X, Y ) )
	{
		if(  pObj->m_bAnimate && !pTopMostAnimatedObject  )  // These get called from topmost down,  so don't overwrite if a higher object already did
		{
			pTopMostAnimatedObject = pObj;
		}

		if( pObj->m_dbHitTest.m_dwSize )
		{
			int LineWidth = pObj->m_rBackgroundPosition.Width / 8 + 1; // The width of each line
			int x = X - pObj->m_rPosition.Left();
			int y = Y - pObj->m_rPosition.Top();
			char c = pObj->m_dbHitTest.m_pBlock[ y * LineWidth + x/8 ];
			if( !(c & 1 << (x%8)) )
				return false;
		}

		SelectedObject( pObj, smMouse, X, Y );
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::OnSelectedCell(class DesignObj_DataGrid *pObj,  
	class DataGridTable *pT,  class DataGridCell *pCell)
{
}

//------------------------------------------------------------------------
/*virtual*/ void Orbiter::DoHighlightObject()
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if(sbNoSelection == m_nSelectionBehaviour || !m_pObj_Highlighted->m_bOnScreen )
		return;
g_pPlutoLogger->Write(LV_WARNING,"Orbiter::DoHighlightObject1 %p",m_pObj_Highlighted);
	UnHighlightObject();
g_pPlutoLogger->Write(LV_WARNING,"Orbiter::DoHighlightObject2 %p",m_pObj_Highlighted);

	if( !m_pObj_Highlighted )
		return;

g_pPlutoLogger->Write(LV_WARNING,"Orbiter::DoHighlightObject3 %p",m_pObj_Highlighted);
	ExecuteCommandsInList( &m_pObj_Highlighted->m_Action_HighlightList, m_pObj_Highlighted, smHighlight, 0, 0 );
g_pPlutoLogger->Write(LV_WARNING,"Orbiter::DoHighlightObject4 %p",m_pObj_Highlighted);

	if( m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) m_pObj_Highlighted;
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

		int nHColumn = pGrid->m_iHighlightedColumn!=-1 ? pGrid->m_iHighlightedColumn + pGrid->m_GridCurCol : pGrid->m_GridCurCol;
		int nHRow = pGrid->m_iHighlightedRow!=-1 ? pGrid->m_iHighlightedRow + pGrid->m_GridCurRow - (pGrid->m_iUpRow >= 0 ? 1 : 0) : 0;

		if( nHColumn==-1 && nHRow==-1 )
			return;

		if(!pGrid->m_pDataGridTable)
			return;

		if(nHRow < pGrid->m_pDataGridTable->m_StartingRow)
		{
			pGrid->m_iHighlightedRow = 1;
			nHRow = pGrid->m_pDataGridTable->m_StartingRow; //set the highlighted row
		}

		DataGridCell *pCell = pGrid->m_pDataGridTable->GetData(nHColumn, nHRow);
		if( !pCell )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter::DoHighlightObject cell is null.  obj %s col %d row %d",
				m_pObj_Highlighted->m_ObjectID.c_str(), nHColumn, nHRow);
			return;

		}

		//the datagrid is highlighted, but no row is highlighted; we don't want to select the whole datagrid
		if(pGrid->m_iHighlightedRow == -1)
			pGrid->m_iHighlightedRow = 0;

		PlutoRectangle r;
		GetGridCellDimensions( pGrid,
			pGrid->m_iHighlightedColumn==-1 ? pGrid->m_MaxCol : pCell->m_Colspan,
			pGrid->m_iHighlightedRow==-1 ? pGrid->m_MaxRow : pCell->m_Rowspan,
			pGrid->m_iHighlightedColumn==-1 ? 0 : pGrid->m_iHighlightedColumn,
			pGrid->m_iHighlightedRow==-1 ? 0 : pGrid->m_iHighlightedRow,
			r.X,  r.Y,  r.Width,  r.Height );

		m_rectLastHighlight.X = max(0,r.X);
		m_rectLastHighlight.Y = max(0,r.Y);
		m_rectLastHighlight.Right( min(r.Right(),m_Width-1) );
		m_rectLastHighlight.Bottom( min(r.Bottom(),m_Height-1) );
	}
	else
		m_rectLastHighlight = m_pObj_Highlighted->GetHighlightRegion();

	m_rectLastHighlight.X += m_pObj_Highlighted->m_pPopupPoint.X;
	m_rectLastHighlight.Y += m_pObj_Highlighted->m_pPopupPoint.Y;

	m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	m_rectLastHighlight.Height++;

	m_pGraphicBeforeHighlight = GetBackground(m_rectLastHighlight);

	PlutoGraphic *pPlutoGraphic = m_pGraphicBeforeHighlight->GetHighlightedVersion();
	if(pPlutoGraphic)
	{
		RenderGraphic(pPlutoGraphic, m_rectLastHighlight);
		delete pPlutoGraphic;
		pPlutoGraphic = NULL;
	}

	for(int i = 0; i < 4; i++)
        HollowRectangle(
			m_rectLastHighlight.X + i, m_rectLastHighlight.Y + i,
			m_rectLastHighlight.Width - 2 * i - 2, m_rectLastHighlight.Height - 2 * i - 2,
			i < 2 ? PlutoColor::Red() : PlutoColor::White()
		);

	UpdateRect(m_rectLastHighlight);
}

/*virtual*/ void Orbiter::UnHighlightObject( bool bDeleteOnly )
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object

	if( !m_pGraphicBeforeHighlight )
		return;

	if( !bDeleteOnly && m_pObj_Highlighted)
	{
		RenderGraphic(m_pGraphicBeforeHighlight, m_rectLastHighlight);
		UpdateRect(m_rectLastHighlight);
	}

	delete m_pGraphicBeforeHighlight;
	m_pGraphicBeforeHighlight=NULL;
}

//------------------------------------------------------------------------
/*virtual*/ void Orbiter::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if(sbNoSelection != m_nSelectionBehaviour)
		for(int i = 0; i < 4; i++)
			HollowRectangle(
				point.X + pObj->m_rBackgroundPosition.X + i, point.Y + pObj->m_rBackgroundPosition.Y + i,
				pObj->m_rBackgroundPosition.Width - 2 * i - 2, pObj->m_rBackgroundPosition.Height - 2 * i - 2,
				i < 2 ? PlutoColor::Blue() : PlutoColor::White()
			);
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::HighlightFirstObject()
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	m_pObj_Highlighted = FindFirstObjectByDirection(1,true,NULL,NULL);
}

DesignObj_Orbiter *Orbiter::FindFirstObjectByDirection(char cDirection /* u,d,l,r,1 (ul),2 (ur),3(dl),4(dr) */,bool bPreferGrid,DesignObj_Orbiter *pObj_Parent,DesignObj_Orbiter *pObj_RelativeTo)
{
	int Position=-1,PositionGrid=-1;  // The X+Y coordinates, we're looking for the upper left-most item, so we just add
	DesignObj_Orbiter *pObj=NULL,*pObjGrid=NULL;  // We give preference to datagrids, so we track them separately
	for(size_t s=0;s<m_vectObjs_TabStops.size();++s)
	{
		DesignObj_Orbiter *p = m_vectObjs_TabStops[s];
		if( p->IsHidden() || !p->m_bOnScreen || (pObj_Parent && !p->ChildOf(pObj_Parent)) )
			continue;
		if( p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && (!p->m_pDataGridTable || !p->m_pDataGridTable->m_RowCount))
			continue;

		int OldPositionGrid = PositionGrid;
		int ThisPosition;
		switch(cDirection)
		{
		case 'u':
			if( pObj_RelativeTo && p->m_rPosition.Y>pObj_RelativeTo->m_rPosition.Y )
				continue;
			ThisPosition = p->m_rPosition.Y;
			break;
		case 'd':
			if( pObj_RelativeTo && p->m_rPosition.Bottom()>pObj_RelativeTo->m_rPosition.Bottom() )
				continue;
			ThisPosition = m_Height - p->m_rPosition.Bottom();
			break;
		case 'l':
			if( pObj_RelativeTo && p->m_rPosition.X>pObj_RelativeTo->m_rPosition.X )
				continue;
			ThisPosition = p->m_rPosition.X;
			break;
		case 'r':
			if( pObj_RelativeTo && p->m_rPosition.Right()>pObj_RelativeTo->m_rPosition.Right() )
				continue;
			ThisPosition = m_Width - p->m_rPosition.Right();
			break;
		case '1':
			ThisPosition = p->m_rPosition.X + p->m_rPosition.Y;
			break;
		case '2':
			ThisPosition = m_Width - p->m_rPosition.Right() + p->m_rPosition.Y;
			break;
		case '3':
			ThisPosition = p->m_rPosition.X + m_Height - p->m_rPosition.Bottom();
			break;
		case '4':
			ThisPosition = m_Width - p->m_rPosition.Right() + m_Height - p->m_rPosition.Bottom();
			break;
		default:
			return NULL; // shouldn't happen
		}
		if( bPreferGrid && p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			if( ThisPosition<PositionGrid || PositionGrid==-1 )
			{
				if(!p->m_pDataGridTable || !p->m_pDataGridTable->m_RowCount)
					PositionGrid = OldPositionGrid;
				else
				{
					PositionGrid=ThisPosition;
					pObjGrid=p;
				}
			}
		}
		else
		{
			if( ThisPosition<Position || Position==-1 )
			{
				Position=ThisPosition;
				pObj=p;
			}
		}
	}

	if( pObjGrid )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) pObjGrid;
		// If nothing is highlighted in this grid, start with the first row/column
		if( pDesignObj_DataGrid->m_iHighlightedColumn==-1 && pDesignObj_DataGrid->m_iHighlightedRow==-1 )
		{
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedRow=0;
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedColumn=0;
		}
		return pObjGrid;
	}
	else
		return pObj;
	return NULL;
}
//------------------------------------------------------------------------
DesignObj_Orbiter *Orbiter::FindObjectToHighlight( DesignObj_Orbiter *pObjCurrent, int PK_Direction )
{
	// Primary means an object primarily in that direction, Secondary means primarily in another direction
	// if an object is 5 pixels to the right, and 10 pixels below, it's primary direction is right, secondary is down
	// pObj_Primary will be the closes object primarily in the direction we want, pObj_Secondary will be a
	// backup choice if there are no objects in that primary direction
	DesignObj_Orbiter *pObj_Primary=NULL,*pObj_Secondary=NULL;
	int Distance_Primary=-1,Distance_Secondary=-1;
	for(size_t s=0;s<m_vectObjs_TabStops.size();++s)
	{
		DesignObj_Orbiter *p = m_vectObjs_TabStops[s];
		if( p==pObjCurrent || p->IsHidden() || !p->m_bOnScreen )
			continue;

		if( p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && (!p->m_pDataGridTable || !p->m_pDataGridTable->m_RowCount))
			continue;

		bool bSkip=false;
		list<class PlutoPopup*>::reverse_iterator it;
		for(it=m_pScreenHistory_Current->GetObj()->m_listPopups.rbegin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			PlutoRectangle pos2=p->m_rPosition+p->m_pPopupPoint;

			if( (pPopup->m_pObj->m_rPosition+pPopup->m_pObj->m_pPopupPoint).IntersectsWith(pos2) && p->TopMostObject()!=pPopup->m_pObj )
			{
				bSkip=true;
				break;
			}
		}

		if( bSkip )
			continue;

		for(it=m_listPopups.rbegin();it!=m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			PlutoRectangle pos2=p->m_rPosition+p->m_pPopupPoint;

			if( (pPopup->m_pObj->m_rPosition+pPopup->m_pObj->m_pPopupPoint).IntersectsWith(pos2) && p->TopMostObject()!=pPopup->m_pObj )
			{
				bSkip=true;
				break;
			}
		}

		if( bSkip )
			continue;

		int Direction_Primary,Direction_Secondary,Distance;
		PlutoPoint pp=p->m_pMidPoint+p->m_pPopupPoint;
		(pObjCurrent->m_pMidPoint+pObjCurrent->m_pPopupPoint).RelativePosition(pp,Direction_Primary,Direction_Secondary,Distance);

		if( Direction_Primary==PK_Direction )
		{
			int Test_Distance_Primary;
			if( PK_Direction==DIRECTION_Left_CONST )
				Test_Distance_Primary=(pObjCurrent->m_rPosition.X+pObjCurrent->m_pPopupPoint.X)-(p->m_rPosition.Right()+p->m_pPopupPoint.X);
			else if( PK_Direction==DIRECTION_Right_CONST )
				Test_Distance_Primary=(p->m_rPosition.X+p->m_pPopupPoint.X)-(pObjCurrent->m_rPosition.Right()+pObjCurrent->m_pPopupPoint.X);
			else if( PK_Direction==DIRECTION_Up_CONST )
				Test_Distance_Primary=(pObjCurrent->m_rPosition.Y+pObjCurrent->m_pPopupPoint.Y)-(p->m_rPosition.Bottom()+p->m_pPopupPoint.Y);
			else if( PK_Direction==DIRECTION_Down_CONST )
				Test_Distance_Primary=(p->m_rPosition.Y+p->m_pPopupPoint.Y)-(pObjCurrent->m_rPosition.Bottom()+pObjCurrent->m_pPopupPoint.Y);
			if(Test_Distance_Primary+Distance<Distance_Primary || Distance_Primary==-1)
			{
				Distance_Primary=Test_Distance_Primary+Distance;
				pObj_Primary=p;
			}
		}
		else if( Direction_Secondary==PK_Direction && (Distance<Distance_Secondary || Distance_Secondary==-1) )
		{
			Distance_Secondary=Distance;
			pObj_Secondary=p;
		}
	}

	if( pObj_Primary )
		return pObj_Primary;
	if( pObj_Secondary )
		return pObj_Secondary;
	return pObjCurrent;
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::HighlightNextObject( int PK_Direction )
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	// Nothing is selected, select the first object
	if(NULL == m_pObj_Highlighted || (m_pObj_Highlighted && (!m_pObj_Highlighted->m_bOnScreen || m_pObj_Highlighted->IsHidden())))
	{
		if(sbNoSelection != m_nSelectionBehaviour)
			HighlightFirstObject();

		if(m_pObj_Highlighted)
			RenderObjectAsync(m_pObj_Highlighted);

		return;
	}

	if( m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		bool bScrolledOutsideGrid=false;  // Will be true if we scroll past the edge of the grid
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pObj_Highlighted;
		if( !pDesignObj_DataGrid->m_pDataGridTable )
			return;
		switch( PK_Direction )
		{
		case DIRECTION_Up_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )!=string::npos )
				bScrolledOutsideGrid=true;
			else if(
				(pDesignObj_DataGrid->m_iHighlightedRow > 0 || pDesignObj_DataGrid->m_GridCurRow > 0) &&
				pDesignObj_DataGrid->m_MaxRow > 1
				)
			{
				pDesignObj_DataGrid->m_iHighlightedRow--;
				if(pDesignObj_DataGrid->m_iHighlightedRow < 0 || (pDesignObj_DataGrid->m_iHighlightedRow == 0 && pDesignObj_DataGrid->HasMoreUp()))
				{
					// Save which row in the actual table we are pointing to,  so we can point there again after doing the scroll
					int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
					dg.Release();
					CMD_Scroll_Grid( "", "", PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedRow=iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Down_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )!=string::npos )
				bScrolledOutsideGrid=true;
			// Continue only if we're not already highlighting the last cell
			else if(
				pDesignObj_DataGrid->m_GridCurRow + pDesignObj_DataGrid->m_iHighlightedRow <
					pDesignObj_DataGrid->m_pDataGridTable->GetRows() &&
				pDesignObj_DataGrid->m_MaxRow > 1
				)  // Add 1 since the highlight is 0 based and get rows is not, add 2 if the last row is just a 'scroll down'
			{
				pDesignObj_DataGrid->m_iHighlightedRow++;
				// See if we've scrolled past the visible end, in which case we need to page.  Subtract 1 or 2 cells for the scroll up/down cells if any
				if(pDesignObj_DataGrid->m_iHighlightedRow >= pDesignObj_DataGrid->m_MaxRow - (pDesignObj_DataGrid->m_dwIDownRow >= 0 ? 1 : 0) /*- (pDesignObj_DataGrid->m_iUpRow >= 0 ? 1 : 0)*/)
				{
					dg.Release();
					CMD_Scroll_Grid( "", "", PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedRow = 1;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Left_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )!=string::npos )
				bScrolledOutsideGrid=true;
			else if(pDesignObj_DataGrid->m_iHighlightedColumn>0 || pDesignObj_DataGrid->m_GridCurCol>0  )
			{
				pDesignObj_DataGrid->m_iHighlightedColumn--;
				if(  pDesignObj_DataGrid->m_iHighlightedColumn<0  )
				{
					dg.Release();
					CMD_Scroll_Grid( "", "", PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_MaxCol;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Right_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )!=string::npos )
				bScrolledOutsideGrid=true;
			// See if we scrolled past the physical end of all the columns
			else if(  pDesignObj_DataGrid->m_GridCurCol+pDesignObj_DataGrid->m_iHighlightedColumn+1 < pDesignObj_DataGrid->m_pDataGridTable->GetCols(  )  ) // Add 1 since the highlight is 0 based and get cols is not
			{
				pDesignObj_DataGrid->m_iHighlightedColumn++;
				// See if we've scrolled past the visible end, in which case we need to page
				if(  pDesignObj_DataGrid->m_iHighlightedColumn>=pDesignObj_DataGrid->m_MaxCol  )
				{
					dg.Release();
					CMD_Scroll_Grid( "", "", PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedColumn=0;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		};

		// If we're capturing arrows, then we don't let the user move outside the grid
		if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'A' )!=string::npos )
			bScrolledOutsideGrid=false;

		// Check that pDesignObj_DataGrid->m_pDataGridTable wasn't deleted
		if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'H' )!=string::npos )
		{
			if( pDesignObj_DataGrid->m_pDataGridTable )
			{
				// We don't want the user to be able to just highlight cells without selecting, so select this cell
				DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(
					pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol,
					pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow : 0);

				if( pCell )
					CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, pCell->GetValue());
			}
			else
			{
				// Be sure nothing is selected since we moved the highlight
				CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, "");
			}
			dg.Release();
			SelectedObject(pDesignObj_DataGrid,smNavigation);
		}
		dg.Release();
		RenderObjectAsync(pDesignObj_DataGrid);
		m_pObj_Highlighted_Last=NULL; // Be sure we always re-highlight this object since the grid changed

		if( !bScrolledOutsideGrid )
		{
			//if we have hidden columns/rows, skip them
			PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

			DataGridCell *pCell = NULL;
			while(!pCell)
			{
				int nHColumn = pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol;
				int nHRow = pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow - (pDesignObj_DataGrid->m_iUpRow >= 0 ? 1 : 0) : 0;

				if(!pDesignObj_DataGrid->m_pDataGridTable)
					break;

				pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData(nHColumn, nHRow);
				if(pCell)
					break; //all ok, we got it

				bool bGiveUp = false;
				switch(PK_Direction)
				{
				case DIRECTION_Up_CONST:
					if(nHRow == 0)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedRow--;
					break;

				case DIRECTION_Down_CONST:

					if(nHRow >= pDesignObj_DataGrid->m_pDataGridTable->m_RowCount)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedRow--;
					break;

				case DIRECTION_Left_CONST:
					if(nHColumn == 0)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedColumn--;
					break;

				case DIRECTION_Right_CONST:
					if(nHColumn >= pDesignObj_DataGrid->m_pDataGridTable->m_ColumnCount)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedColumn++;
					break;
				}

				if(bGiveUp)
					break;
			}

			return; // We just moved around within the grid
		}
	}

	//DesignObj_Orbiter *pDesignObj_Orbiter_OriginallyHighlight=m_pObj_Highlighted;
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

	DesignObj_Orbiter *pObj_Highlighted_Before = m_pObj_Highlighted;
	if(!pNextObject || pNextObject == m_pObj_Highlighted)
		m_pObj_Highlighted = FindObjectToHighlight( m_pObj_Highlighted, PK_Direction );
	else
		m_pObj_Highlighted = pNextObject;

	if( pObj_Highlighted_Before && m_pObj_Highlighted!=pObj_Highlighted_Before )
		ExecuteCommandsInList( &pObj_Highlighted_Before->m_Action_UnhighlightList, pObj_Highlighted_Before, smHighlight, 0, 0 );

	if( m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pObj_Highlighted;
		// If nothing is highlighted in this grid, start with the first row/column
		if( pDesignObj_DataGrid->m_iHighlightedColumn==-1 && pDesignObj_DataGrid->m_iHighlightedRow==-1 )
		{
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedRow=0;
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedColumn=0;
		}
	}

	/*
	if(pDesignObj_Orbiter_OriginallyHighlight)
		if(pDesignObj_Orbiter_OriginallyHighlight->m_vectGraphic.size())
			RenderObjectAsync(pDesignObj_Orbiter_OriginallyHighlight);
		else //this button is embedded in the background, we must rerender all
			RenderObjectAsync(m_pScreenHistory_Current->GetObj());
	*/

	if(m_pObj_Highlighted)
		RenderObjectAsync(m_pObj_Highlighted);
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
			while(true)
			{
				string sStatus,sRegenStatus;
				int iRegenPercent;
				DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
					m_dwPK_Device,&sStatus,&sRegenStatus,&iRegenPercent);
				bool bResult = SendCommand(CMD_Get_Orbiter_Status_DT);
				g_pPlutoLogger->Write(LV_STATUS,"Initialize Get_Orbiter_Status (%d) -%s-",(int) bResult,sStatus.c_str());
				int iResponse;
				if( !bResult || sStatus!="O" )
				{
					g_pPlutoLogger->Write(LV_STATUS,"Orbiter plugin reported status of <%s>",sStatus.c_str());
					if( (iResponse=HandleNotOKStatus(sStatus,sRegenStatus,iRegenPercent))==0 )
					{
						g_pPlutoLogger->Write(LV_STATUS,"Handle not ok said to quit");
						m_bQuit = true;
						return;
					}
					else if( iResponse==1 )
					{
						g_pPlutoLogger->Write(LV_STATUS,"Handle not ok said to load anyway");
						break;
					}
					else
						g_pPlutoLogger->Write(LV_STATUS,"Handle not ok said to try again");
				}
				else
					break;
			}

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
			bool bReadLocalFile=false;
			if(  bFileExists  )
			{
				size_t size;
				char *buffer = FileUtils::ReadFileIntoBuffer( m_sLocalDirectory + "/" + Filename, size );

				if(  !buffer || !SerializeRead( ( long ) size, buffer, ( void * ) this ) || !ParseConfigurationData( Type ) )
					g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to read Orbiter data from file: %s/%s.  Will try the serve", m_sLocalDirectory.c_str(  ), Filename.c_str(  ) );
				else
					bReadLocalFile=true;
				delete [] buffer;
			}
			if( !bReadLocalFile )
			{
				if(  m_sLocalDirectory.length(  )  )
				{
					g_pPlutoLogger->Write( LV_CRITICAL, "Directory: %s doesn't contain the orbiter's .info file.  Requesting files from the server.", m_sLocalDirectory.c_str(  ) );
					m_sLocalDirectory="";  // It's not valid
				}

				// Request our config info
				char *pConfigFile=NULL;
				int iSizeConfigFile=0;
				// We can't send it to the General Info virtual device since we won't have that until we get our config data
				DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false,  BL_SameHouse, "orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/" + Filename + ".lzo",
					&pConfigFile, &iSizeConfigFile );
				SendCommand( CMD_Request_File_Cat );

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

				if ( !iSizeConfigFile )
				{
					g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to get Orbiter data" );
					PromptUser("I cannot read the Orbiter configuration from the server.  I'll try to regenerate it");
					RegenOrbiter();
					Sleep(2000);
					OnQuit();
					return;
				}

				// The framework needs a pointer to us when it's creating Orbiter objects,  pass in this as a void *
				if(  !SerializeRead( iSizeConfigFile, pConfigFile, ( void * ) this ) || !ParseConfigurationData( Type ) )
				{
					delete pMessage;
					PromptUser("The Orbiter configuration from the server is corrupt.  I'll try to regenerate it");
					RegenOrbiter();
					Sleep(2000);
					OnQuit();
					return;
				}

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
				PromptUser("Something went very wrong. No initial location!");
				exit( 1 );
			}
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS,"Setting current location");
#endif
			CMD_Set_Current_Location(m_pLocationInfo_Initial->iLocation);
			m_pLocationInfo = m_pLocationInfo_Initial;
			m_dwPK_Users = m_dwPK_Users_Default;

			DesignObj_OrbiterMap::iterator iHao=m_ScreenMap.begin(  );
			if ( iHao==m_ScreenMap.end(  ) )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "No screens found." );
				PromptUser("Something went very wrong. No screens found!");
				exit( 1 );
			}
			else if( m_sInitialScreen.size() )
				CMD_Goto_Screen("", atoi(m_sInitialScreen.c_str()));
			else
				GotoMainMenu();

			// AB 3/26/2006.  The sequence here is tough.  We can't process received messages
			// until the m_pScreenHistory_Current is valid.  Normally it's not valid until the
			// screen goto is complete.  Previously the end of scope was here, so it did the
			// goto screen at this point.  But this meant the onload actions for the screen 
			// were executed before the onstartup.  So, we'll temporarily fake the current screen
			// so the startup commands can go through.
			m_pScreenHistory_Current=render.m_pScreenHistory_get();
			
			// AB 12/29/2005 -- Needed to move this up since orbiter registered may cause
			// bind icon commands to come back, which aren't available until after the startup commands
			// And we can't call the startup commands until the above scope is terminated so a goto a screen with NeedToRender
			if( m_pScreenHistory_Current )
			{
				DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
				for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
				{
					DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
					if(  pObj->m_Action_StartupList.size(  )>0  )
						ExecuteCommandsInList( &pObj->m_Action_StartupList, pObj, smLoadUnload );
				}
			}

			char *pData=NULL; int iSize=0;
			DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "1",
				m_dwPK_Users,StringUtils::itos(m_pLocationInfo->PK_EntertainArea),m_pLocationInfo->PK_Room, &pData, &iSize);
			DCE::CMD_Set_Current_User CMD_Set_Current_User( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_dwPK_Users );
			CMD_Orbiter_Registered.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Current_User.m_pMessage);
			if( !SendCommand( CMD_Orbiter_Registered ) )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Cannot register with router" );
				PromptUser("Orbiter cannot register with the router. Please try again later.");

				exit(1);
				return;
			}
			m_pOrbiterFileBrowser_Collection = new OrbiterFileBrowser_Collection;
			m_pOrbiterFileBrowser_Collection->SerializeRead(iSize,pData);
			delete pData;

			// Debug various issues with startup sequence irregularities
			g_pPlutoLogger->Write(LV_STATUS,"Orbiter::Initialize finished startup phase");
		} // Originally this context ended before the startup actions.  This meant onload actions,
		// like Send Orbiter Popups, were getting sent out before the orbiter registered command.  In the 
		// case of popups, this meant popups were disabled and then immediately enabled again

		if( !m_pScreenHistory_Current )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "No initial screen" );
			PromptUser("Something went very wrong. No initial screen!");
			OnQuit();
			return;
		}

		AdjustWindowSize(m_pScreenHistory_Current->GetObj()->m_rPosition.Width,
			m_pScreenHistory_Current->GetObj()->m_rPosition.Height);

		m_bStartingUp=false;
	}
}

void Orbiter::InitializeGrid( DesignObj_DataGrid *pObj )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	pObj->m_bHighlightSelectedCell=false;
	if( !pObj->m_bParsed )
		ParseGrid(pObj);

	/* todo 2.0
	bool bEPG = ( GridID.substr( 0, 3 )=="EPG" || GridID.substr( 0, 5 )=="CURSH" );
	*/
	// Initially the first row and column will be highlighted
	string::size_type posH;
	if(  (posH=pObj->m_sExtraInfo.find( 'H' ))==string::npos && pObj->sSelVariable.length()==0 )
	{
		pObj->m_iHighlightedRow = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='C') ? 0 : -1;
		pObj->m_iHighlightedColumn = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='R') ? 0 : -1;
	}
	else if(sbAllowsSelected == m_nSelectionBehaviour)
	{
		pObj->m_iHighlightedRow = pObj->m_sExtraInfo.find( 'C' )==string::npos ? 0 : -1;
		pObj->m_iHighlightedColumn = pObj->m_sExtraInfo.find( 'R' )==string::npos ? 0 : -1;
		pObj->m_bHighlightSelectedCell=true;
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
		pObj->m_iPopulatedWidth=pObj->m_MaxCol;  // Pass in the grid's on screen width/height -- we'll get back the total populated size
		pObj->m_iPopulatedHeight=pObj->m_MaxRow;
		/*
		if( pObj->m_iPopulatedWidth<0 )
		{
		int k=2;
		}
		*/
		string sParams = SubstituteVariables( pObj->m_sOptions, pObj, 0, 0 );
		DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID,
			pObj->m_iPK_Datagrid, sParams, pObj->m_iPK_DeviceTemplate, &iPK_Variable, &sValue_To_Assign, &bResponse, &pObj->m_iPopulatedWidth, &pObj->m_iPopulatedHeight  );
		if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
			g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid: %d failed", pObj->m_iPK_Datagrid );
		else if(iPK_Variable)
			CMD_Set_Variable(iPK_Variable, sValue_To_Assign);
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
}
//------------------------------------------------------------------------
bool Orbiter::ParseConfigurationData( GraphicType Type )
{
	m_LastActivityTime = time( NULL );

	int nIndex = 0;
	int nCount = int(m_ScreenMap.size());
	for(ScreenMap::iterator it = m_ScreenMap.begin(); it != m_ScreenMap.end(); ++it)
	{
		DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) ( *it ).second;
		ParseObject(pObj, pObj, pObj, Type, 0);
		ShowProgress(50 + (nIndex++) * 50 / nCount);
	}

	bool bCacheAll = DATA_Get_CacheSize()==-1;
	for( DesignObj_OrbiterMap::iterator oi = m_mapObj_All.begin(  ); oi != m_mapObj_All.end(  ); ++oi  )
	{
		DesignObj_Orbiter *pObj = ( *oi ).second;
		if( bCacheAll )
			pObj->m_bKeepGraphicInCache=true;

		// Now match up all the up/down/left/right
		if(  pObj->m_PK_DesignObj_Up  )
			pObj->m_pDesignObj_Orbiter_Up = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Up ), pObj );
		if(  pObj->m_PK_DesignObj_Down  )
			pObj->m_pDesignObj_Orbiter_Down = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Down ), pObj );
		if(  pObj->m_PK_DesignObj_Left  )
			pObj->m_pDesignObj_Orbiter_Left = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Left ), pObj );
		if(  pObj->m_PK_DesignObj_Right  )
			pObj->m_pDesignObj_Orbiter_Right = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Right ), pObj );
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
	g_pPlutoLogger->Write(LV_STATUS,"Get Floorplan layout found %d pages in %s", NumPages,sResult.c_str());
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
						pDesignObjCommand->m_PK_Command=COMMAND_Requires_Special_Handling_CONST;
						pDesignObjCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
						pDesignObjZone->m_Commands.push_back(pDesignObjCommand);
						fpObj->pObj->m_ZoneList.push_back(pDesignObjZone);
					}
				}
			}
		}
	}

	ParseHardKeys();

	return true;
}
//--------------------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::ParseHardKeys()
{
	string::size_type pos = 0;
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
}
//--------------------------------------------------------------------------------------------------------------
void Orbiter::ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type,  int Lev )
{
	for(size_t s=0;s<pObj->m_vectDesignObjText.size();++s)
		pObj->m_vectDesignObjText[s]->m_pObject = pObj;

	m_mapObj_All[pObj->m_ObjectID] = pObj;  // Do this earlier since the bind to action may require looking this up.
	pObj->m_pObj_Screen=pObj_Screen;

	if(  pObj->m_bHideByDefault  )
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

	if(  pObj->m_bTabStop  )
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
		pObj->m_iBaseObjectID = atoi( x.c_str(  ) );
	}
	else
	{
		pObj->m_iBaseObjectID = atoi( pObj->m_ObjectID.c_str(  ) );
	}

	// On any screen all child objects should inherit the screen's priority so the whole screen is cached
	pObj->m_Priority = pObj_Screen->m_Priority;

	if ( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		pObj->m_bContainsDataGrid=true;
		DesignObj_DataGrid *pObj_Datagrid = ( DesignObj_DataGrid * ) pObj;
		// Fix up the parameters
		pObj_Datagrid->m_sGridID = SubstituteVariables( pObj->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ), pObj, 0, 0 );
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

	pObj->m_pMidPoint.X = pObj->m_rPosition.X + (pObj->m_rPosition.Width/2);
	pObj->m_pMidPoint.Y = pObj->m_rPosition.Y + (pObj->m_rPosition.Height/2);
	/*
	Floorplan
	else if(  pObj->m_iBaseObjectID==DESIGNOBJ_GRPHOUSEINDICATORS_CONST  )
	{
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
	DesignObj_Orbiter *p = ( *iHao );
	if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW0_CONST  )
	m_FloorplanObjects.m_Windows->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW1_CONST  )
	m_FloorplanObjects.m_Windows->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW2_CONST  )
	m_FloorplanObjects.m_Windows->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW3_CONST  )
	m_FloorplanObjects.m_Windows->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR0_CONST  )
	m_FloorplanObjects.m_Doors->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR1_CONST  )
	m_FloorplanObjects.m_Doors->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR2_CONST  )
	m_FloorplanObjects.m_Doors->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR3_CONST  )
	m_FloorplanObjects.m_Doors->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION0_CONST  )
	m_FloorplanObjects.m_Motions->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION1_CONST  )
	m_FloorplanObjects.m_Motions->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION2_CONST  )
	m_FloorplanObjects.m_Motions->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION3_CONST  )
	m_FloorplanObjects.m_Motions->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA0_CONST  )
	m_FloorplanObjects.m_Cameras->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA1_CONST  )
	m_FloorplanObjects.m_Cameras->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA2_CONST  )
	m_FloorplanObjects.m_Cameras->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA3_CONST  )
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
	SolidRectangle( point.X + pObj->m_rPosition.X, point.Y + pObj->m_rPosition.Y, pObj->m_rPosition.Width, pObj->m_rPosition.Height, PlutoColor( 0, 0, 255 ) );
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
			g_pPlutoLogger->Write(LV_STATUS, "Button %s: button ID: %d", (event.type == Orbiter::Event::BUTTON_DOWN) ? "down" : "up", event.data.button.m_iPK_Button );

		if ( event.type == Orbiter::Event::REGION_DOWN || event.type == Orbiter::Event::REGION_UP )
			g_pPlutoLogger->Write(LV_STATUS, "Region %s: position: [%d, %d]", (event.type == Orbiter::Event::REGION_DOWN) ? "down" : "up", event.data.region.m_iX, event.data.region.m_iY);
	}
#endif
// some temporary stuff
if ( event.type == Orbiter::Event::BUTTON_DOWN && event.data.button.m_iPK_Button==BUTTON_F6_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonDown(BUTTON_Mouse_1_CONST);

if ( event.type == Orbiter::Event::BUTTON_DOWN && event.data.button.m_iPK_Button==BUTTON_F7_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonDown(BUTTON_Mouse_2_CONST);

if ( event.type == Orbiter::Event::BUTTON_DOWN && event.data.button.m_iPK_Button==BUTTON_F8_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonDown(BUTTON_Mouse_3_CONST);

if ( event.type == Orbiter::Event::BUTTON_UP && event.data.button.m_iPK_Button==BUTTON_F6_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonUp(BUTTON_Mouse_1_CONST);

if ( event.type == Orbiter::Event::BUTTON_UP && event.data.button.m_iPK_Button==BUTTON_F7_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonUp(BUTTON_Mouse_2_CONST);

if ( event.type == Orbiter::Event::BUTTON_UP && event.data.button.m_iPK_Button==BUTTON_F8_CONST && m_pMouseBehavior )
m_pMouseBehavior->ButtonUp(BUTTON_Mouse_3_CONST);

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

	// 2005-dec-7  Do the capture keyboard stuff first in case one of the
	// objects we selected needs the current value
#ifndef PHONEKEYS
	if( m_bCaptureKeyboard_OnOff )
	{
		if( m_iCaptureKeyboard_PK_Variable )
		{
			if( PK_Button == BUTTON_space_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ' ' );
			else if( PK_Button == BUTTON_comma_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ',' );
			else if( PK_Button == BUTTON_dash_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '-' );
			else if( PK_Button == BUTTON_equals_sign_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '=' );
			else if( PK_Button == BUTTON_semicolumn_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ';' );
			else if( PK_Button == BUTTON_single_quote_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '\'' );
			else if( PK_Button == BUTTON_double_quote_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '"' );
			else if( PK_Button == BUTTON_dot_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '.' );
			if( PK_Button == BUTTON_Asterisk_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '*' );
			if( PK_Button == BUTTON_Pound_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '#' );
			if( PK_Button == BUTTON_right_parenthesis_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ')' );
			if( PK_Button == BUTTON_left_parenthesis_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '(' );
			if( PK_Button == BUTTON_exclamation_point_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '!' );
			if( PK_Button == BUTTON_at_sign_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '@' );
			if( PK_Button == BUTTON_dollar_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '$' );
			if( PK_Button == BUTTON_percent_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '%' );
			if( PK_Button == BUTTON_caret_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '^' );
			if( PK_Button == BUTTON_ampersand_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '&' );
			if( PK_Button == BUTTON_underscore_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '_' );
			if( PK_Button == BUTTON_plus_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '+' );
			if( PK_Button == BUTTON_colon_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ':' );
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

			if(bHandled)
			{
				if(NULL != m_pCaptureKeyboard_Text && NULL != m_pCaptureKeyboard_Text->m_pObject)
				{
					RenderObjectAsync(m_pCaptureKeyboard_Text->m_pObject);
				}
			}
		}
	}
#else
	if( m_bCaptureKeyboard_OnOff && m_iCaptureKeyboard_PK_Variable )
	{
		//TODO: if keys = 0,  1,  ... 9,  transform this to abc,  def,  etc.

		if( PK_Button == BUTTON_Phone_C_CONST )
			bHandled |= CaptureKeyboard_EditText_DeleteLastChar(  );
	}
#endif

	if(  PK_Button == BUTTON_Enter_CONST  )
	{
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(  m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden(  )  )
		{
			SelectedObject( m_pObj_Highlighted, smNavigation );
			bHandled=true;
		}
	}
	if(  !( bHandled=ClickedButton( m_pScreenHistory_Current->GetObj(),  PK_Button ) )  )
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
			GotoScreen( m_sMainMenu );
			bHandled=true;
		}
	}

	if(  pScreenHistory != m_pScreenHistory_Current  )
		m_bRerenderScreen = true;

	return bHandled;
}

bool Orbiter::ButtonDown( int iPK_Button )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnKeyDown);
	if(pCallBackData)
	{
		KeyCallBackData *pKeyData = (KeyCallBackData *)pCallBackData;
		pKeyData->m_nPlutoKey = iPK_Button;
	}

	if(ExecuteScreenHandlerCallback(cbOnKeyDown))
		return true;

	//if this is a repeated button, we'll handle it right away
	if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->GetObj(), iPK_Button))
	{
		return HandleButtonEvent(iPK_Button);
	}

	//this is not a repeated button
	if(iPK_Button == BUTTON_left_shift_CONST || iPK_Button == BUTTON_right_shift_CONST)
	{
		m_bShiftDown = true; //different logic with the on screen keyboard
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Button Down - shift");
#endif
	}
	else if(iPK_Button == BUTTON_caps_lock_CONST)
		m_bCapsLock = !m_bCapsLock;
	else
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Button Down - key");
#endif
	}

	//this is not a repeated button
	//it will be handled in ButtonUp
	gettimeofday(&m_tButtonDown,NULL);
	return false;
}

bool Orbiter::ButtonUp( int iPK_Button )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnKeyUp);
	if(pCallBackData)
	{
		KeyCallBackData *pKeyData = (KeyCallBackData *)pCallBackData;
		pKeyData->m_nPlutoKey = iPK_Button;
	}

	if(ExecuteScreenHandlerCallback(cbOnKeyUp))
		return true;

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
		GenerateSimulateKeypress(iPK_Button);
		return true;
	}

	//if this was a repeated button, we might want to stop all repeat related events
	if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->GetObj(), iPK_Button))
	{
		StopRepeatRelatedEvents();
		return false;
	}

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
	bool bResult = HandleButtonEvent(iPK_Button);

	if(iPK_Button != BUTTON_left_shift_CONST && iPK_Button != BUTTON_right_shift_CONST)
	{
		if(m_bShiftDown && m_bShiftDownOnScreenKeyboard)
			m_bShiftDown = false;

#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Button up - key");
#endif
	}
	else
	{
		if(m_bShiftDown && !m_bShiftDownOnScreenKeyboard)
			m_bShiftDown = false;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Button up - shift");
#endif
	}

	if(!m_bShiftDown)
		m_bShiftDownOnScreenKeyboard = true;

	return bResult;

}

/*virtual*/ void Orbiter::StopRepeatRelatedEvents()
{
	m_bWeCanRepeat=true;
	if( m_bRepeatingObject )
	{
		PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack== &Orbiter::ReselectObject )
				pCallBackInfo->m_bStop=true;
		}
		m_bRepeatingObject=false;
	}
}

/*virtual*/ void Orbiter::GenerateSimulateKeypress(int iPK_Button)
{
	int nPK_Button_To_Send = iPK_Button;

	if((!m_bShiftDown && m_bCapsLock) || (m_bShiftDown && !m_bCapsLock))
	{
		if(iPK_Button >= BUTTON_A_CONST && iPK_Button <= BUTTON_Z_CONST)
			nPK_Button_To_Send = iPK_Button - BUTTON_A_CONST + BUTTON_a_CONST;
		else if(iPK_Button >= BUTTON_a_CONST && iPK_Button <= BUTTON_z_CONST)
			nPK_Button_To_Send = iPK_Button - BUTTON_a_CONST + BUTTON_A_CONST;
	}

	if(m_bShiftDown)
	{
		switch(iPK_Button)
		{
		case BUTTON_0_CONST:        nPK_Button_To_Send = BUTTON_right_parenthesis_CONST ; break;
		case BUTTON_1_CONST:        nPK_Button_To_Send = BUTTON_exclamation_point_CONST ; break;
		case BUTTON_2_CONST:        nPK_Button_To_Send = BUTTON_at_sign_CONST ; break;
		case BUTTON_3_CONST:        nPK_Button_To_Send = BUTTON_Pound_CONST ; break;
		case BUTTON_4_CONST:        nPK_Button_To_Send = BUTTON_dollar_CONST ; break;
		case BUTTON_5_CONST:        nPK_Button_To_Send = BUTTON_percent_CONST ; break;
		case BUTTON_6_CONST:        nPK_Button_To_Send = BUTTON_caret_CONST ; break;
		case BUTTON_7_CONST:        nPK_Button_To_Send = BUTTON_ampersand_CONST ; break;
		case BUTTON_8_CONST:        nPK_Button_To_Send = BUTTON_Asterisk_CONST ; break;
		case BUTTON_9_CONST:        nPK_Button_To_Send = BUTTON_left_parenthesis_CONST ; break;

		case BUTTON_dash_CONST:         nPK_Button_To_Send = BUTTON_underscore_CONST ; break;
		case BUTTON_plus_CONST:         nPK_Button_To_Send = BUTTON_equals_sign_CONST ; break;
		case BUTTON_semicolumn_CONST:   nPK_Button_To_Send = BUTTON_colon_CONST ; break;
		case BUTTON_single_quote_CONST: nPK_Button_To_Send = BUTTON_double_quote_CONST ; break;
		}
	}

	DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress_(m_dwPK_Device, m_pLocationInfo->m_dwPK_Device_Orbiter,
		StringUtils::itos(nPK_Button_To_Send), "");
	SendCommand(CMD_Simulate_Keypress_);
}

bool Orbiter::RegionUp( int x,  int y )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnMouseUp);
	if(pCallBackData)
	{
		MouseClickCallBackData *pKeyData = (MouseClickCallBackData *)pCallBackData;
		pKeyData->m_x = x;
		pKeyData->m_y = y;
	}

	if(ExecuteScreenHandlerCallback(cbOnMouseUp))
		return true;

	StopRepeatRelatedEvents();
	return false;
}
bool Orbiter::RegionDown( int x,  int y )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnMouseDown);
	if(pCallBackData)
	{
		MouseClickCallBackData *pKeyData = (MouseClickCallBackData *)pCallBackData;
		pKeyData->m_x = x;
		pKeyData->m_y = y;
	}

	if(ExecuteScreenHandlerCallback(cbOnMouseDown))
		return true;

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

	if ( m_pScreenHistory_Current  && m_pScreenHistory_Current->GetObj()->m_rPosition.Contains( x,  y ) )
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
		DesignObj_Orbiter* pTopMostObject = m_pScreenHistory_Current->GetObj();
		PlutoPoint RelativePoint(x, y);
		m_pActivePopup = NULL;

		if( m_pScreenHistory_Current )
		{
			for(list<class PlutoPopup*>::reverse_iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.rbegin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.rend();++it)
			{
				PlutoPopup *pPopup = *it;
				if( pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->m_rPosition.Right() &&
					pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->m_rPosition.Bottom()
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
					pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->m_rPosition.Right()    &&
					pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->m_rPosition.Bottom())
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
			g_pPlutoLogger->Write( LV_WARNING, "SelectedObject took %d ms", clkFinished - clkStart );
		}
#endif
	}

	if(  pScreenHistory != m_pScreenHistory_Current  )
		m_bRerenderScreen = true;

	return bHandled;
}
//------------------------------------------------------------------------
bool Orbiter::GotActivity(  )
{
	m_LastActivityTime=time( NULL );

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"m_bDisplayOn is %d",(int) m_bDisplayOn);
#endif

	if( !m_bDisplayOn ||
		(m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()==m_pDesignObj_Orbiter_ScreenSaveMenu &&
		m_pDesignObj_Orbiter_MainMenu!=m_pDesignObj_Orbiter_ScreenSaveMenu) )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"GotActiity monitor m_bDisplayOn is %d",(int) m_bDisplayOn);
#endif
		if( !m_bDisplayOn )
			CMD_Display_OnOff( "1",false );
		if( m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu )
		{
			CMD_Set_Main_Menu("N");
			GotoMainMenu();
		}
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring click because screen saver was active");
#endif
		return false; // Don't do anything with this
	}

	if(  m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds  )
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->GetObj(), pe_ALL );

	if( m_pDesignObj_Orbiter_ScreenSaveMenu && !m_bBypassScreenSaver && m_pScreenHistory_Current->GetObj() != m_pDesignObj_Orbiter_ScreenSaveMenu && m_iTimeoutScreenSaver )
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
			PK_DesignObj += "." + StringUtils::itos(pDesignObj_Orbiter->m_iVersion) + "." + StringUtils::itos(pDesignObj_Orbiter->m_iPage);
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
				PK_DesignObj = m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + OriginalValue;
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
				FindSingleNumberObject( atoi( OriginalValue.c_str(  ) ), m_pScreenHistory_Current->GetObj() );
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
		if(  pChild->m_iBaseObjectID==PK_Object  )
			return pChild;
		pChild = FindSingleNumberObject( PK_Object, pChild );
		if(  pChild  )
			return pChild;
	}

	for( iHao=pObj->m_OverlayObjects.begin(  ); iHao != pObj->m_OverlayObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
		if(  pChild->m_iBaseObjectID==PK_Object  )
			return pChild;

		pChild = FindSingleNumberObject( PK_Object, pChild );
		if(  pChild  )
			return pChild;
	}
	return NULL;
}
//------------------------------------------------------------------------
void Orbiter::ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList,
									DesignObj_Orbiter *pObj, SelectionMethod selectionMethod,
									int X,  int Y, int Repeat )
{
	if(m_bQuit)
		return;

	if(  pDesignObjCommandList->size(  )==0  )
		return;

	if( pObj->m_iRepeatIntervalInMS && m_bWeCanRepeat )
	{
		m_bRepeatingObject=true;
		pObj->m_iRepeatParm = Repeat;
		// Arbitrary decision that there should be a bit of a delay before the object starts repeating that
		// is 3 times the repeat interval
		CallMaintenanceInMiliseconds( (Repeat==0 ? pObj->m_iRepeatIntervalInMS * 3 : pObj->m_iRepeatIntervalInMS),
			&Orbiter::ReselectObject, pObj, pe_ALL, true );
	}

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "Executing %d commands in object: %s", ( int ) pDesignObjCommandList->size(  ),  pObj->m_ObjectID.c_str(  ) );
#endif

	bool bRefreshGrids=false;
	Message *pMessage=NULL;

	// The commands are in the list backwards, so use a reverse iterator so that they are executed in the same order they appear within designer
	DesignObjCommandList::reverse_iterator iCommand;
	bool bGetConfirmation=false;
	for(iCommand=pDesignObjCommandList->rbegin();iCommand!=pDesignObjCommandList->rend();++iCommand)
	{
		DesignObjCommand *pCommand = *iCommand;
		int PK_Command = pCommand->m_PK_Command;
		if( pCommand->m_bDeliveryConfirmation )
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
					CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_mnuUserPinCode_CONST),"","",false,true);
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
					CMD_Goto_DesignObj( 0, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "", "", false, true );
					return;
				}
			}
		}

		// We may attach a simulate keypress action to an onscreen button, like a keyboard key, which simulates pressing
		// that key if the user touches it.  In this case, we don't want to execute the simulate keypress command
		// if the button was selected because the user hit the button associated with it (ie x==-1 and y==-1 when input was ButtonDown)
		// and create an endless loop
		else if( PK_Command==COMMAND_Simulate_Keypress_CONST && selectionMethod==smKeyboard )
			continue;


		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT

		if(  pCommand->m_PK_DeviceTemplate  )
		{
			if(  pCommand->m_PK_DeviceTemplate<0  )
			{
				pCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;  // Set this to a temporary value,  so we'll know if it's changes at teh end of the if
				if( pCommand->m_PK_DeviceTemplate!=DEVICEID_HANDLED_INTERNALLY )  // Just leave it there
				{
					pCommand->m_PK_Device = TranslateVirtualDevice(pCommand->m_PK_DeviceTemplate);
					if(  pCommand->m_PK_Device==DEVICEID_NULL  )
					{
						// We recognized it as one of the known cases in the switch block,  but apparently this option isn't sent
						g_pPlutoLogger->Write( LV_CRITICAL, "Sending command to unidentified virtual device: %d", pCommand->m_PK_DeviceTemplate );
						pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // Maybe there's some interceptor that will know what to do with it
					}
					else if(  pCommand->m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
						pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // It's not a virtual device we know how to handle.  Let the interceptors have it
				}
			}
			else
				pCommand->m_PK_Device=DEVICEID_MASTERDEVICE;
		}
		else if(  pCommand->m_PK_DeviceCategory  )
			pCommand->m_PK_Device=DEVICEID_CATEGORY;

		if(  pCommand->m_PK_Device==0 && pCommand->m_PK_DeviceGroup==0  )
		{
			// See if it's one we handle locally
			if(  m_bLocalMode )
			{
				pCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
			}
			else if( m_mapDevice_Selected.size() )
				pCommand->m_PK_Device = DEVICEID_LIST;
			else
			{
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
					pCommand->m_PK_Device = atoi( m_mapVariable[VARIABLE_PK_Device_CONST].c_str(  ) );
				vm.Release(  );
			}
		}

		if(  pCommand->m_PK_Device==DEVICEID_NULL  )
		{
			g_pPlutoLogger->Write( LV_CONTROLLER,  "\x1b[33;1m****ACTION %d GOING TO DEVICEID_NULL***%d\x1b[0m\n", PK_Command );
		}
		else
		{
			if(PK_Command==COMMAND_Populate_Datagrid_CONST)
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
				int iPK_Variable=atoi(pCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());;
				string sValue_To_Assign=pCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST];
				int iWidth=pObj->m_MaxCol,iHeight=pObj->m_MaxRow;
				/*
				if( iWidth<0 )
				{
				int k=2;
				}
				*/

				string sParams;
				if(pDesignObj_DataGrid_OnScreen)
					sParams += SubstituteVariables( pDesignObj_DataGrid_OnScreen->m_sOptions, pObj, 0, 0 );
				sParams += SubstituteVariables( pCommand->m_ParameterList[COMMANDPARAMETER_Options_CONST], pObj, 0, 0 );
				DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ),
					GridID, atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ),
					sParams,
					atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DeviceTemplate_CONST].c_str(  ) ),
					&iPK_Variable, &sValue_To_Assign, &bResponse, &iWidth, &iHeight );
				if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
					g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid from command: %d failed", atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ) );
				else if(iPK_Variable)
					CMD_Set_Variable(iPK_Variable, sValue_To_Assign);

				if( m_vectObjs_GridsOnScreen.size() )
				{
					DesignObj_DataGrid *pObj = m_vectObjs_GridsOnScreen[0];
					if( pObj->m_sGridID==GridID && pObj->m_sExtraInfo.find('S')!=string::npos && pObj->m_sSeek.length()==0 && !pObj->sSelVariable.empty(  )  )
					{
						PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
							pObj->m_sSeek = "~" + m_mapVariable[atoi( pObj->sSelVariable.c_str(  ) )];
					}
				}


				continue; // processed this one ourselves
			}

			Message *pThisMessage = new Message( m_dwPK_Device, pCommand->m_PK_Device, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, PK_Command, 0 );
			if(  pCommand->m_PK_Device==DEVICEID_MASTERDEVICE  )
				pThisMessage->m_dwPK_Device_Template=pCommand->m_PK_DeviceTemplate;
			else if(  pCommand->m_PK_Device==DEVICEID_CATEGORY  )
				pThisMessage->m_dwPK_Device_Category_To=pCommand->m_PK_DeviceCategory;
			else if(  pCommand->m_PK_Device==DEVICEID_LIST  )
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


			pThisMessage->m_dwPK_Device_Group_ID_To=pCommand->m_PK_DeviceGroup;

			if(  pCommand->m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
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

		RenderObjectAsync(m_pScreenHistory_Current->GetObj());
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
			x = pDesignObj_DataGrid->m_rPosition.Right()-( Row*( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX - pDesignObj_DataGrid->m_FixedRowHeight;
			y = pDesignObj_DataGrid->m_rPosition.Y+( Column *( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY;
		}
		else if( m_iRotation==180 )
		{
			x = pDesignObj_DataGrid->m_rPosition.Right()-( (Column+Colspan-1)*( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX - pDesignObj_DataGrid->m_FixedColumnWidth;
			y = pDesignObj_DataGrid->m_rPosition.Bottom()-( (Row+Rowspan-1) *( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY - pDesignObj_DataGrid->m_FixedRowHeight;
		}
		else if( m_iRotation==270 )
		{
			x = pDesignObj_DataGrid->m_rPosition.X+( Row*( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX;
			y = pDesignObj_DataGrid->m_rPosition.Bottom()-( (Column+Colspan-1) *( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY - pDesignObj_DataGrid->m_FixedColumnWidth;
		}
		else
		{
			x = pDesignObj_DataGrid->m_rPosition.X+( Column*( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX;
			y = pDesignObj_DataGrid->m_rPosition.Y+( Row *( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY;
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
			if ( x+w > pDesignObj_DataGrid->m_rPosition.X + pDesignObj_DataGrid->m_rPosition.Width )
				// Oops,  width is greater than the size of the object.  Truncate.
				w = ( pDesignObj_DataGrid->m_rPosition.X+pDesignObj_DataGrid->m_rPosition.Width ) - x;
			if ( y+h > pDesignObj_DataGrid->m_rPosition.Y + pDesignObj_DataGrid->m_rPosition.Height )
				// Oops,  height is greater than the size of the object.  Truncate.
				h = ( pDesignObj_DataGrid->m_rPosition.Y+pDesignObj_DataGrid->m_rPosition.Height ) - y;
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
		{
#ifdef WIN32
			Output += "router=" + m_sIPAddress;
#else
            // sometimes GetCommandOutput looses a thread
            // only under gdb ?
            char * sCmd = "/usr/pluto/bin/Network_DisplaySettings.sh";
            if (FileUtils::FileExists(sCmd))
            {
                char * args[] = { sCmd, "--orbiter", NULL };
                ProcessUtils::GetCommandOutput(args[0], &args[0], Output);
            }
#endif
		}
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
		else if(  Variable=="ID"  )
			Output += m_pScreenHistory_Current->ScreenID();
		else if(  Variable=="V" )
			Output += string(VERSION) + "(" + g_szCompile_Date + ")";
		else if(  Variable=="MT" )
		{
			Output += StringUtils::itos(m_iPK_MediaType);
		}
		else if(  Variable=="NP" )
		{
			Output += m_sNowPlaying;
			m_pObj_NowPlayingOnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_SEC" )
		{
			Output += m_sNowPlaying_Section;
			m_pObj_NowPlaying_Section_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_TIME_SHORT" )
		{
			Output += m_sNowPlaying_TimeShort;
			m_pObj_NowPlaying_TimeShort_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_TIME_LONG" )
		{
			Output += m_sNowPlaying_TimeLong;
			m_pObj_NowPlaying_TimeLong_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_SPEED" )
		{
			Output += m_sNowPlaying_Speed;
			m_pObj_NowPlaying_Speed_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				SaveBackgroundForDeselect( pObj, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NPD" )
		{
			Output += StringUtils::itos(m_dwPK_Device_NowPlaying);
		}
		else if(  Variable=="NP_R" )
		{
			if( m_bIsOSD && m_iPK_DesignObj_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
				Output += StringUtils::itos(m_iPK_DesignObj_RemoteOSD);
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
		else if(  Variable=="N" )
			Output += "\n";
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
		else if(Variable == "SIZE")
		{
			Output += StringUtils::itos(m_iImageWidth) + "x" + StringUtils::itos(m_iImageHeight);
		}
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
			for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
			{
				PendingCallBackInfo *pCallBackInfo = (*it).second;
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
		else if(  Variable=="LS" && m_pObj_LastSelected && m_pObj_LastSelected->m_pFloorplanObject )
			Output += StringUtils::itos(m_pObj_LastSelected->m_pFloorplanObject->PK_Device);
		else if(  Variable=="SD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				Output += StringUtils::itos((*it).first) + ",";
		else if(  Variable=="SDD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
			{
				if(it->second)
				{
					Output += it->second->m_sDescription + ", ";
				}
				else
				{
					//this is a media floorplan object; it doesn't have a device associated
					FloorplanObject *pfObj = m_mapFloorplanObject_Selected[it->first];
					string sStatus = pfObj->Status == "offline" ? " (" + pfObj->Status + ")" : "";
					Output += pfObj->DeviceDescription + sStatus + ", ";
				}
			}
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
				int ClickX = X - pObj->m_rPosition.X;
				Output+=StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Width * ClickX / pObj->m_rPosition.Width );
			}
		}
		else if(  Variable=="YS"  )
		{
			if(pObj->m_vectGraphic.size() && pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Height > 0)
			{
				int ClickY = Y - pObj->m_rPosition.Y;
				Output += StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->Height * ClickY / pObj->m_rPosition.Height );
			}
		}
		else if(  Variable=="S"  )
			Output += m_pScreenHistory_Current->GetObj()->m_ObjectID;
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

			if(  pCommand->m_PK_Command == COMMAND_Scroll_Grid_CONST  )
			{
				if(  pCommand->m_ParameterList[COMMANDPARAMETER_Level_CONST]=="-1"  )
					continue; // This is just a move to the top/bottom
				// If this Command points to this grid or to nothing,  it's a match
				if(  atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str(  ) ) == pDGObj->m_iBaseObjectID ||
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
bool Orbiter::AcquireGrid( DesignObj_DataGrid *pObj,  int &GridCurCol,  int &GridCurRow,  DataGridTable* &pDataGridTable )
{
	if (  pObj->bReAcquire || !pDataGridTable || pDataGridTable->m_StartingColumn != GridCurCol || pDataGridTable->m_StartingRow != GridCurRow || pObj->m_sSeek.length() )
	{
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
			StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID,
			pObj->m_MaxRow, pObj->m_MaxCol, pObj->m_bKeepRowHeader, pObj->m_bKeepColHeader, true, pObj->m_sSeek, pObj->m_iSeekColumn, &data, &size, &GridCurRow, &GridCurCol );

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
				return pDataGridTable!=NULL;
			}
			else
			{
				g_pPlutoLogger->Write( LV_CRITICAL,  "Error loading grid ID: %d (not implemented?)",  m_dwIDataGridRequestCounter );
			}
		}
	}
	return false;
}

void *MaintThread(void *p)
{
	bMaintThreadIsRunning = true;
	Orbiter* pOrbiter = (Orbiter *)p;

	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, pOrbiter->m_MaintThreadMutex);// Keep this locked to protect the map
	while(!pOrbiter->m_bQuit)
	{
		if(pOrbiter->m_mapPendingCallbacks.size() == 0)
		{
			//nothing to process. let's sleep...
			cm.CondWait(); // This will unlock the mutex and lock it on awakening
		}
		else
		{
			// We've got stuff to check out
			PendingCallBackInfo *pCallBackInfoGood = NULL;
			timespec ts_NextCallBack,ts_now;
			ts_NextCallBack.tv_sec=0;
			gettimeofday(&ts_now,NULL);

			//let's choose the one which must be processed first
			for(map<int,PendingCallBackInfo *>::iterator it=pOrbiter->m_mapPendingCallbacks.begin();it!=pOrbiter->m_mapPendingCallbacks.end();)
			{
				PendingCallBackInfo *pCallBackInfo = (*it).second;
				if( pCallBackInfo->m_bStop )
				{
					pOrbiter->m_mapPendingCallbacks.erase( it++ );  // This is dead anyway
					delete pCallBackInfo;
					continue;
				}
				else if(pCallBackInfo->m_abstime <= ts_now)
				{
					pOrbiter->m_mapPendingCallbacks.erase( it );
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
#ifdef DEBUG
				g_pPlutoLogger->Write(LV_STATUS,"Going to call member %p",pCallBackInfoGood->m_fnCallBack);
#endif
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
		m_mapPendingCallbacks.size());
#endif

	if( e_PurgeExisting!=pe_NO )
	{
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack==fnCallBack &&
				(e_PurgeExisting==pe_ALL || pCallBackInfo->m_pData==data) )
				pCallBackInfo->m_bStop=true;
		}
	}

	PendingCallBackInfo *pCallBackInfo = new PendingCallBackInfo(m_nCallbackCounter++, bPurgeTaskWhenScreenIsChanged );

	gettimeofday(&pCallBackInfo->m_abstime,NULL);
	pCallBackInfo->m_abstime += milliseconds;

	pCallBackInfo->m_fnCallBack=fnCallBack;
	pCallBackInfo->m_pData=data;
	pCallBackInfo->m_pOrbiter=this;

	m_mapPendingCallbacks[pCallBackInfo->m_nCallbackID]=pCallBackInfo;


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

	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()!=pObj->m_pObj_Screen && !m_pActivePopup)
		return; // We must have since changed screens

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Deselecting %s object, state '%s'", pObj->m_ObjectID.c_str(), pObj->m_GraphicToDisplay==GRAPHIC_SELECTED ? "'selected'" : "'normal'");
#endif

	if(pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
		RenderObjectAsync(pObj);

		// Remove it from the list
		for( vector<class DesignObj_Orbiter *>::iterator it=m_vectObjs_Selected.begin(  );it!=m_vectObjs_Selected.end(  );++it )
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
		//int foo=2;
	}
}

void Orbiter::GetVideoFrame( void *data )
{
	//Since this may take a while and we don't want to block the mutex the whole time, make a local copy
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
	vector<DesignObj_Orbiter *> vectObjs_VideoOnScreen; /** < All the video on screen */
	vectObjs_VideoOnScreen.resize(m_vectObjs_VideoOnScreen.size());
	std::copy(m_vectObjs_VideoOnScreen.begin(), m_vectObjs_VideoOnScreen.end(), vectObjs_VideoOnScreen.begin());
	vm.Release();

	vector<DesignObj_Orbiter *>::iterator it;
	for(it = vectObjs_VideoOnScreen.begin(); it != vectObjs_VideoOnScreen.end(); it++)
	{
		DesignObj_Orbiter *pObj = *it;
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
			char *pBuffer=NULL; int Size=0;  string sFormat = "jpg";
			int PK_Device = atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Source_CONST ).c_str(  ) );
			if( PK_Device )
			{
				DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame( m_dwPK_Device, PK_Device, "0",  0 /* stream */, pObj->m_rPosition.Width, pObj->m_rPosition.Height, &pBuffer, &Size, &sFormat );
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
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Setting monitor display on/off: %s %d m_bDisplayOn: %d",sOnOff.c_str(),(int) bAlready_processed,(int) m_bDisplayOn);
#endif
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
	g_pPlutoLogger->Write(LV_STATUS,"Go Back currently: %s  cs: %s",this->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),sPK_DesignObj_CurrentScreen.c_str());
#endif
	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
	ScreenHistory *pScreenHistory=NULL;

	if(m_pScreenHistory_Current && m_pScreenHistory_Current->GoBack())
		pScreenHistory = m_pScreenHistory_Current;
	else
	{
		while(  m_listScreenHistory.size(  )  )
		{
			// The last screen we went to
			pScreenHistory = m_listScreenHistory.back(  );

			if(!pScreenHistory->GoBack())
			{
#ifdef DEBUG
				g_pPlutoLogger->Write(LV_WARNING, "Removing from screen history screen %d", pScreenHistory->PK_Screen());
#endif

				// We now took the prior screen off teh list
				m_listScreenHistory.pop_back(  );
				//if( pScreenHistory->m_bCantGoBack && sForce!="1"  )
				continue;
			}

			if(pScreenHistory->HistoryEmpty())
				m_listScreenHistory.pop_back();

			break;   // We got one to go back to
		}
	}

	vm.Release();

#ifdef DEBUG
	DumpScreenHistory();
#endif

	// We've got a screen to go back to
	if(  pScreenHistory  )
	{
		m_pContextToBeRestored = pScreenHistory;
		ScreenHistory::m_bAddToHistory = false;
		m_pScreenHistory_NewEntry = pScreenHistory;
		CMD_Goto_DesignObj(0, pScreenHistory->GetObj()->m_ObjectID, pScreenHistory->ScreenID(),
			"", false, pScreenHistory->GetObj()->m_bCantGoBack);
	}
	else
		GotoMainMenu();

#ifdef DEBUG
	DumpScreenHistory();
#endif
}

//<-dceag-c5-b->

	/** @brief COMMAND: #5 - Goto DesignObj */
	/** Goto a specific design obj */
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

void Orbiter::CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c5-e->
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"CMD_Goto_Screen: %s",sPK_DesignObj.c_str());
#endif
	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );  // Nothing more can happen

	bool bIsRemote=false;
	// We're using a popup remote, so just go to the main menu
	if( sPK_DesignObj=="<%=NP_R%>" )
	{
		if( m_iUiVersion==2 )
		{
			CMD_Remove_Popup("","");
			return; // With UI version 2, there is not 'goto remote'
		}
		if( m_iPK_DesignObj_Remote_Popup>0 && m_sObj_Popop_RemoteControl.size() )
			sPK_DesignObj = "<%=M%>";
		else
			bIsRemote=true;
	}

	else if( sPK_DesignObj=="**NOMEDIA**" )
	{
		sPK_DesignObj = StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST);
		CMD_Set_Text(sPK_DesignObj, m_mapTextString[TEXT_No_Media_CONST], TEXT_STATUS_CONST);
	}

	HidePopups(NULL);

	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	if(sPK_DesignObj == "")
		sPK_DesignObj = StringUtils::ltos(m_pDesignObj_Orbiter_MainMenu->m_iBaseObjectID);

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

	if( pObj_New->m_iBaseObjectID == atoi(m_sMainMenu.c_str()) )
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
		if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()==m_pDesignObj_Orbiter_MainMenu )
			return;
		if( !m_pScreenHistory_Current )
			return;

		pObj_New = m_pScreenHistory_Current->GetObj();
	}

	//if the simulator is running, we won't go to pluto admin screen
	if(Simulator::GetInstance()->IsRunning() && (pObj_New->m_iBaseObjectID==DESIGNOBJ_mnuAdvancedOptions_CONST || pObj_New->m_iBaseObjectID==DESIGNOBJ_mnuPower_CONST || pObj_New->m_iBaseObjectID==DESIGNOBJ_PVR_FS_CONST) )
		return;

	if( bIsRemote || pObj_New->m_iBaseObjectID==m_iPK_DesignObj_Remote || pObj_New->m_iBaseObjectID==m_iPK_DesignObj_RemoteOSD )
		pObj_New->m_bIsARemoteControl=true;

	ScreenHistory *pScreenHistory_New = NULL;
	if(NULL != m_pScreenHistory_NewEntry)
	{
		pScreenHistory_New = m_pScreenHistory_NewEntry; //just create by CMD_Goto_Screen
		m_pScreenHistory_NewEntry = NULL;
	}
	else
	{
		//about the hide last current screen.
		ObjectOffScreen(m_pScreenHistory_Current->GetObj());
		pScreenHistory_New = m_pScreenHistory_Current; //another designobj for the same screen
		pScreenHistory_New->AddToHistory();
	}

	string sLastObject = pScreenHistory_New->GetObj() ? pScreenHistory_New->GetObj()->m_ObjectID : "";
	bool bLastCantGoBack = pScreenHistory_New->m_bCantGoBack;

	pScreenHistory_New->SetObj(pObj_New);

    map<DesignObj_Orbiter *, bool> mapVisibilityContext;
	GetChildrenVisibilityContext(pScreenHistory_New->GetObj(), mapVisibilityContext);
	pScreenHistory_New->SaveContext(m_mapVariable, mapVisibilityContext);
	pScreenHistory_New->ScreenID(sID);
	pScreenHistory_New->m_bCantGoBack = bCant_Go_Back ? true : pObj_New->m_bCantGoBack;

	if(ScreenHistory::m_bAddToHistory && m_pScreenHistory_Current != pScreenHistory_New)
	{
		if(pScreenHistory_New->GetObj() && !bLastCantGoBack && sLastObject != pObj_New->m_ObjectID && sLastObject != "")
			pScreenHistory_New->AddToHistory();
		else if(m_pScreenHistory_Current && m_pScreenHistory_Current != pScreenHistory_New && m_pScreenHistory_Current->GetObj() && !m_pScreenHistory_Current->GetObj()->m_bCantGoBack)
			m_pScreenHistory_Current->AddToHistory();
	}

#ifdef DEBUG
	DumpScreenHistory();
#endif

	vm.Release(  );
	NeedToRender::NeedToChangeScreens( this, pScreenHistory_New);
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
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(  m_pObj_Highlighted==pObj && !bShow  )
			UnHighlightObject();

		pObj->m_bHidden = !bShow;
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "Object: %s visible: %d", pObj->m_ObjectID.c_str(), (int) !pObj->m_bHidden );
#endif
		RenderObjectAsync(pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( pObj->m_bHidden && pObj->m_pParentObject )
			RenderObjectAsync((DesignObj_Orbiter *) pObj->m_pParentObject);
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
		/** @param #10 ID */
			/** If specified, only screens that match this ID will be removed */
		/** @param #159 PK_Screen */
			/** The screen to remove */

void Orbiter::CMD_Remove_Screen_From_History(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c8-e->
{
	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"CMD_Remove_Screen_From_History %d - %s size: %d",iPK_Screen,sID.c_str(),(int) m_listScreenHistory.size());
	DumpScreenHistory();
#endif

	for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();)
	{
		ScreenHistory *pScreenHistory = *it;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Comparing %s - %s",pScreenHistory->GetObj()->m_ObjectID.c_str(),pScreenHistory->ScreenID().c_str());
#endif
		if( pScreenHistory->PK_Screen() == iPK_Screen && (sID.length()==0 || sID == pScreenHistory->ScreenID()) )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS,"deleting %d - %s",iPK_Screen,pScreenHistory->ScreenID().c_str());
#endif
			delete (*it);
			it = m_listScreenHistory.erase( it );
		}
		else
			++it;
	}

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"After CMD_Remove_Screen_From_History %d - %s size: %d",iPK_Screen,sID.c_str(),(int) m_listScreenHistory.size());
	DumpScreenHistory();
#endif

	if( m_pScreenHistory_Current && m_pScreenHistory_Current->PK_Screen() == iPK_Screen && (sID.length()==0 || sID==m_pScreenHistory_Current->ScreenID()) )
	{
		m_pScreenHistory_Current->PurgeHistory(); // User wants to delete this entire screen instance.  If we leave this populated, we may just go to a prior design obj in this screen
		vm.Release();
		CMD_Go_back("","");
	}
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
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	PLUTO_SAFETY_LOCK( dng, m_DatagridMutex );  // Lock them in the same order as render screen
	// todo 2.0?    NeedsUpdate( 2 ); // Moving grids is slow; take care of an animation if necessary

	DesignObj_Orbiter *pObj=NULL;

	int LoopNum=-1;
	if ( sPK_DesignObj.empty(  ) )
		LoopNum=0;
	else
		pObj = FindObject( sPK_DesignObj );

	DesignObj_DataGrid *pObj_Datagrid = NULL;
	if( pObj && pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		pObj_Datagrid = (DesignObj_DataGrid *) pObj;
	else if( pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Trying to scroll a non-grid %s",sPK_DesignObj.c_str());
		return;
	}

	while( LoopNum<( int ) m_vectObjs_GridsOnScreen.size(  ) )
	{
		if(  LoopNum!=-1  )
		{
			pObj_Datagrid = m_vectObjs_GridsOnScreen[LoopNum++];
		}

		if ( pObj_Datagrid && pObj_Datagrid->m_pDataGridTable )
		{
			RenderObjectAsync(pObj_Datagrid);

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
			PLUTO_SAFETY_LOCK( cm, m_DatagridMutex );
			InitializeGrid(pDesignObj);
			pDesignObj->bReAcquire = true;
			RenderObjectAsync(pDesignObj);
		}
	}

	if( m_pScreenHistory_Current )
		RenderObjectAsync(m_pScreenHistory_Current->GetObj());

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

	RenderObjectAsync(pObj);
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
	DesignObj_Orbiter *pObj=( m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj() : NULL );
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
	{
		pText->m_sText = sText;
	}

	if(pObj->m_bOnScreen)
		RenderObjectAsync(pObj);
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
		g_pPlutoLogger->Write(LV_CRITICAL,"Got SetBoundIcon for unknown type: %s size: %d",sType.c_str(),(int) m_mapObj_Bound.size());
		return;
	}

	int iValue = atoi(sValue_To_Assign.c_str());
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	for(DesignObj_DataList::iterator it=pDesignObj_DataList->begin();it!=pDesignObj_DataList->end();++it)
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *it;
		pObj->m_bDontResetState=true;
		pObj->m_GraphicToDisplay = iValue;
		if(pObj->m_bOnScreen)
			RenderObjectAsync(pObj);
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

	for(VariableMap::iterator it=m_mapVariable.begin();it!=m_mapVariable.end();++it)
		CMD_Set_Variable((*it).first, (*it).second);
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

	RenderObjectAsync(pObj);
}

void Orbiter::DelayedSelectObject( void *data )
{
	DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) data;
	if( TestCurrentScreen(pDelayedSelectObjectInfo->m_sPK_DesignObj_CurrentScreen) )
		SelectedObject(pDelayedSelectObjectInfo->m_pObj,smCommand);

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
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
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
		SelectedObject( pDesignObj_Orbiter, smCommand );
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
	if( !pObj )
		return NULL;

	DesignObjText *pText = NULL;
	vector<DesignObjText *>::iterator it_text;
	for( it_text = pObj->m_vectDesignObjText.begin(  );
		it_text != pObj->m_vectDesignObjText.end(  );
		it_text++
		)
	{
		DesignObjText *pText_Temp = *it_text;

		if( pText_Temp->m_PK_Text == iPK_Text )
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
		CMD_Set_Variable(iPK_Variable, "");

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
			pObj = m_pScreenHistory_Current->GetObj();


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
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.X ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Y ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Width ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Height );
	}

	return true;
}

void Orbiter::GetButtonsInObject( DesignObj_Data *pObj, vector<int> &vectButtons )
{
	if( pObj->m_iPK_Button )
		vectButtons.push_back(pObj->m_iPK_Button);

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
	CMD_Set_Variable(m_iCaptureKeyboard_PK_Variable, NewValue);
	vm.Release();

	ExecuteScreenHandlerCallback(cbCapturedKeyboardBufferChanged);

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
	if((!m_bShiftDown && m_bCapsLock) || (m_bShiftDown && !m_bCapsLock))
	{
		if(ch >= 'A' && ch <= 'Z')
			ch = ch - 'A' + 'a';
		else if(ch >= 'a' && ch <= 'z')
			ch = ch - 'a' + 'A';
	}

	if(m_bShiftDown)
	{
		char cOldCh = ch;
		switch(cOldCh)
		{
		case '0': ch = ')'; break;
		case '1': ch = '!'; break;
		case '2': ch = '@'; break;
		case '3': ch = '#'; break;
		case '4': ch = '$'; break;
		case '5': ch = '%'; break;
		case '6': ch = '^'; break;
		case '7': ch = '&'; break;
		case '8': ch = '*'; break;
		case '9': ch = '('; break;
		case '-': ch = '_'; break;
		case '+': ch = '='; break;
		case ';': ch = ':'; break;
		case '\'': ch = '"'; break;
		}
	}

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		string OldValue = m_mapVariable[m_iCaptureKeyboard_PK_Variable];
	string NewValue = OldValue + ch;

	m_sCaptureKeyboard_InternalBuffer = NewValue;
	CMD_Set_Variable(m_iCaptureKeyboard_PK_Variable, NewValue);
	vm.Release();

	ExecuteScreenHandlerCallback(cbCapturedKeyboardBufferChanged);

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
		/** @param #3 PK_DesignObj */
			/** If specified, this object will be highlighted.  Otherwise the first detected one. */

void Orbiter::CMD_Reset_Highlight(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c85-e->
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if( sPK_DesignObj.c_str() )
	{
		DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj);
		if( pObj )
		{
			if( m_pObj_Highlighted && m_pObj_Highlighted!=pObj )
			{
				UnHighlightObject();
				ExecuteCommandsInList( &m_pObj_Highlighted->m_Action_UnhighlightList, m_pObj_Highlighted, smHighlight, 0, 0 );
			}
			m_pObj_Highlighted = pObj;
			return;
		}
		else
			g_pPlutoLogger->Write(LV_WARNING,"Orbiter::CMD_Reset_Highlight Cannot find object %s",sPK_DesignObj.c_str());
	}

	HighlightFirstObject(  );
}

bool Orbiter::ReceivedMessage( class Message *pMessageOriginal )
{
	while( !m_pScreenHistory_Current && !m_bQuit )
		Sleep(100);

	if(m_bQuit)
		return false;

	string strMessage = string("ReceivedMessage: ") + StringUtils::itos(pMessageOriginal->m_dwID);

	if(m_pScreenHandler->m_mapCallBackData_Find(cbMessageIntercepted) && ScreenHandlerMsgInterceptor(this, pMessageOriginal, NULL, NULL))
		return true;

	NeedToRender render( this, strMessage.c_str() );  // Redraw anything that was changed by this command
	bool bResult = Orbiter_Command::ReceivedMessage( pMessageOriginal );
	return bResult;
}

void Orbiter::CaptureKeyboard_UpdateVariableAndText( int iVariable,  string sVariable ) //for capture keyboard cmd
{
	CMD_Set_Variable(iVariable, sVariable);
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

void Orbiter::FireEntAreaRoomCommands()
{
	if( !m_pLocationInfo )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter::FireEntAreaRoomCommands no location");
		return;
	}
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"set current location to ea %d room %d",
		m_pLocationInfo->PK_EntertainArea,m_pLocationInfo->PK_Room);
#endif
	DCE::CMD_Set_Entertainment_Area CMD_Set_Entertainment_Area( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(m_pLocationInfo->PK_EntertainArea) );
	SendCommand( CMD_Set_Entertainment_Area );
	DCE::CMD_Set_Current_Room CMD_Set_Current_Room( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_pLocationInfo->PK_Room );
	SendCommand( CMD_Set_Current_Room );
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
			FireEntAreaRoomCommands();
		}
#ifdef DEBUG
		else
			g_pPlutoLogger->Write(LV_STATUS,"Not firing set current location because we're starting up");
#endif

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
	DCE::CMD_Get_Current_Floorplan CMD_Get_Current_Floorplan(m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(pDesignObj_Orbiter->m_iPage), Type, &sResult);
	SendCommand(CMD_Get_Current_Floorplan);

	FloorplanObjectVectorMap *pFloorplanObjectVectorMap = m_mapFloorplanObjectVector_Find(pDesignObj_Orbiter->m_iPage);
	FloorplanObjectVector *fpObjVector = NULL;

	//  FloodFill(150,241, RGB(0,0,0), RGB(255,255,0));

	if( pFloorplanObjectVectorMap )
		fpObjVector = (*pFloorplanObjectVectorMap)[Type];

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Get current floorplan %d page %d returned %s",Type, pDesignObj_Orbiter->m_iPage, sResult.c_str());
#endif

	if( fpObjVector )
	{
		string::size_type pos = 0;
		FloorplanObjectVector::iterator it;
		for(it = fpObjVector->begin(); it != fpObjVector->end(); ++it)
		{
			FloorplanObject *fpObj = *it;

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
				if( fpObj->pObj->m_rBackgroundPosition.X+fpObj->pObj->m_rBackgroundPosition.Width>m_Width ||
					fpObj->pObj->m_rBackgroundPosition.Y+fpObj->pObj->m_rBackgroundPosition.Height>m_Height )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"Floorplan object %s is out of bounds ",fpObj->DeviceDescription.c_str());
					continue;
				}

				if( Color )
				{
					PlutoColor Magenta(255,102,255);

#ifdef DEBUG
					g_pPlutoLogger->Write(LV_STATUS,"Replacing obj %s %d,%d-%d,%d with color %d",
						fpObj->pObj->m_ObjectID.c_str(),fpObj->pObj->m_rBackgroundPosition.X,fpObj->pObj->m_rBackgroundPosition.Y,fpObj->pObj->m_rBackgroundPosition.Width,
						fpObj->pObj->m_rBackgroundPosition.Height, (int) Color);
#endif
					ReplaceColorInRectangle(point.X + fpObj->pObj->m_rBackgroundPosition.X, point.Y + fpObj->pObj->m_rBackgroundPosition.Y,fpObj->pObj->m_rBackgroundPosition.Width,
						fpObj->pObj->m_rBackgroundPosition.Height, Magenta, Color);

				}

				if( fpObj->pObj->m_vectDesignObjText.size()==1 )
				{
					DesignObjText *pDesignObjText = fpObj->pObj->m_vectDesignObjText[0];
					pDesignObjText->m_sText = OSD;
				}
				//FloodFill(fpObj->pObj->m_rectBackground.X + fpObj->FillX,fpObj->pObj->m_rectBackground.Y + fpObj->FillY, RGB(0,0,0), Color);
			}
		}
	}
}

ScreenHistory *NeedToRender::m_pScreenHistory=NULL;

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

void NeedToRender::NeedToChangeScreens( Orbiter *pOrbiter, ScreenHistory *pScreenHistory/*, bool bAddToHistory*/ )
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Need to change screens logged to %s",pScreenHistory->GetObj()->m_ObjectID.c_str());
#endif
	m_pScreenHistory = pScreenHistory;

	//purge pending tasks, if need it.  Do it here, so that things will happen in the right order.
	//ie: CMD_GotoScreen, CMD_SelectObject. The real need to change screens won't get called until
	//after all messages are processed, wiping out CMD_SelectObject
	//Orbiter *pOrbiter = m_pOrbiter(Orbiter *) g_pCommand_Impl;
	if( pOrbiter )
	{
		PLUTO_SAFETY_LOCK( pm, pOrbiter->m_MaintThreadMutex );
		map<int,PendingCallBackInfo *>::iterator itCallBackInfo;
		for(itCallBackInfo = pOrbiter->m_mapPendingCallbacks.begin(); itCallBackInfo != pOrbiter->m_mapPendingCallbacks.end();)
		{
			PendingCallBackInfo *pCallBackInfo = (*itCallBackInfo).second;
			if(pCallBackInfo->m_bPurgeTaskWhenScreenIsChanged)
			{
				pOrbiter->m_mapPendingCallbacks.erase(itCallBackInfo++);
				delete pCallBackInfo;
			}
			else
				itCallBackInfo++;
		}
		pm.Release();

		// We also want to handle reseting the selected states here for the same reason,
		// Otherwise we may have a show_object message come in and get processed afterward
		pOrbiter->ResetState(pScreenHistory->GetObj());
	}
}

//<-dceag-c242-b->

	/** @brief COMMAND: #242 - Set Now Playing */
	/** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
		/** @param #2 PK_Device */
			/** The currently active media device */
		/** @param #3 PK_DesignObj */
			/** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote, guide */
		/** @param #5 Value To Assign */
			/** The description of the media */
		/** @param #9 Text */
			/** The description of the current section (ie chapter in a dvd, etc.) */
		/** @param #13 Filename */
			/** The default name to use if the user wants to rip this.  Only applies to discs. */
		/** @param #29 PK_MediaType */
			/** The type of media playing */
		/** @param #48 Value */
			/** The track number or position in the playlist */
		/** @param #50 Name */
			/** The name of the window for the application to remain in the foreground */
		/** @param #120 Retransmit */
			/** If true, it will re-request the plist (current playlist) grid */

void Orbiter::CMD_Set_Now_Playing(int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sText,string sFilename,int iPK_MediaType,int iValue,string sName,bool bRetransmit,string &sCMD_Result,Message *pMessage)
//<-dceag-c242-e->
{
	if(m_bQuit)
		return;

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	m_iPK_MediaType=iPK_MediaType;
	m_sNowPlaying = SubstituteVariables(sValue_To_Assign, NULL, 0, 0);
	m_sNowPlaying_Section = SubstituteVariables(sText, NULL, 0, 0);
	m_dwPK_Device_NowPlaying = iPK_Device;
	CMD_Set_Variable(VARIABLE_Track_or_Playlist_Positio_CONST, StringUtils::itos(iValue));
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"CMD_Set_Now_Playing %s %s",sValue_To_Assign.c_str(),sPK_DesignObj.c_str());
#endif
	string::size_type pos=0;
	m_iPK_DesignObj_Remote=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_Remote_Popup=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_FileList=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_FileList_Popup=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_RemoteOSD=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_Guide=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_sDefaultRippingName = sFilename;
	m_sNowPlaying_TimeShort="";
	m_sNowPlaying_TimeLong="";

	if( bRetransmit )
	{
		vector<DesignObj_DataGrid*>::iterator it;
		for(it = m_vectObjs_GridsOnScreen.begin(); it != m_vectObjs_GridsOnScreen.end(); ++it)
		{
			DesignObj_DataGrid* pDesignObj = *it;
			if(pDesignObj->m_sGridID.size()>6 && (pDesignObj->m_sGridID.substr(0,5)=="plist" || pDesignObj->m_sGridID.substr(0,6)=="tracks") )
			{
				PLUTO_SAFETY_LOCK( cm, m_DatagridMutex );

				int nOldHightlightedRow = pDesignObj->m_iHighlightedRow;
				int nOldHightlightedCol = pDesignObj->m_iHighlightedColumn;
				int nGridCurRow = pDesignObj->m_GridCurRow;
				int nGridCurCol = pDesignObj->m_GridCurCol;
				int nOldSelectedIndex = atoi(m_mapVariable[atoi(pDesignObj->sSelVariable.c_str())].c_str());
				int nStartingRow = pDesignObj->m_pDataGridTable ? pDesignObj->m_pDataGridTable->m_StartingRow : 0;

				InitializeGrid(pDesignObj);
				pDesignObj->bReAcquire = true;
				RenderObjectAsync(pDesignObj);

				if(nOldHightlightedRow + nStartingRow == nOldSelectedIndex + !!nStartingRow)
				{
					//we changed the current playing song,
					//so we can restore the datagrid context (selected item, highlighted item)
					pDesignObj->m_iHighlightedRow = nOldHightlightedRow;
					pDesignObj->m_iHighlightedColumn = nOldHightlightedCol;
					pDesignObj->m_GridCurRow = nGridCurRow;
					pDesignObj->m_GridCurCol = nGridCurCol;
				}
			}
		}

	}
	DesignObj_Orbiter *pObj_Popop_RemoteControl=NULL;
	if( m_sObj_Popop_RemoteControl.size() && (pObj_Popop_RemoteControl=FindObject(m_sObj_Popop_RemoteControl)) && m_iPK_DesignObj_Remote_Popup>0 )
		CMD_Show_Popup(StringUtils::itos(m_iPK_DesignObj_Remote_Popup),m_Popop_RemoteControl_X,m_Popop_RemoteControl_Y,pObj_Popop_RemoteControl->m_ObjectID,"remote",false,false);
	else if( pObj_Popop_RemoteControl && m_iPK_DesignObj_Remote_Popup==0 )
		CMD_Remove_Popup(pObj_Popop_RemoteControl->m_ObjectID,"remote");
	else
	{
		if( m_pObj_NowPlayingOnScreen )
			RenderObjectAsync(m_pObj_NowPlayingOnScreen);
		if( m_pObj_NowPlaying_Section_OnScreen )
			RenderObjectAsync(m_pObj_NowPlaying_Section_OnScreen);
	}
}

bool Orbiter::TestCurrentScreen(string &sPK_DesignObj_CurrentScreen)
{
	//PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	if( sPK_DesignObj_CurrentScreen.length(  ) && ( !m_pScreenHistory_Current || atoi( sPK_DesignObj_CurrentScreen.c_str(  ) )!=m_pScreenHistory_Current->GetObj()->m_iBaseObjectID )  ) // It should be at the beginning
	{
		// Be sure it's not a -1 telling us to be at the main menu
		if(  sPK_DesignObj_CurrentScreen=="-1" )
		{
			if( !m_pScreenHistory_Current || !m_pScreenHistory_Current->GetObj() )
				return false;

			if( m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_MainMenu->m_iBaseObjectID ||
				(m_pDesignObj_Orbiter_SleepingMenu && m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_SleepingMenu->m_iBaseObjectID) ||
				(m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_ScreenSaveMenu->m_iBaseObjectID) )
				return true;
			else
				return false;
		}
		return false;
	}
	return true;
}

void Orbiter::ContinuousRefresh( void *data )
{
	ContinuousRefreshInfo *pContinuousRefreshInfo = (ContinuousRefreshInfo *) data;
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::ContinuousRefresh %d %p %p %s %s",
		pContinuousRefreshInfo->m_iInterval,m_pScreenHistory_Current->GetObj(),pContinuousRefreshInfo->m_pObj,
		m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),
		(pContinuousRefreshInfo->m_pObj ? pContinuousRefreshInfo->m_pObj->m_ObjectID.c_str() : "NULL"));
#endif

	if( m_pScreenHistory_Current->GetObj()!=pContinuousRefreshInfo->m_pObj )
		delete pContinuousRefreshInfo;
	else
	{
		if( m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==DESIGNOBJ_mnuScreenSaver_CONST )
		{
			if( !m_bDisplayOn )
				return; // Nothing more to do

			DesignObjText *pText = FindText( m_pScreenHistory_Current->GetObj(), TEXT_USR_ENTRY_CONST );
			if( pText  )
			{
				pText->m_sText = StringUtils::itos(	int(m_tTimeoutTime - time(NULL)) ) + " seconds";
				pText->m_rPosition.X = int(rand() * (float) (m_iImageWidth * .5 / RAND_MAX));
				pText->m_rPosition.Y = int(rand() * (float) (m_iImageHeight *.9 / RAND_MAX));
			}

			CMD_Set_Text(m_pScreenHistory_Current->GetObj()->m_ObjectID, StringUtils::itos( int(m_tTimeoutTime - time(NULL)) ) + " seconds",TEXT_USR_ENTRY_CONST);
		}

		CMD_Refresh("*");
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Orbiter::ContinuousRefresh restarting");
#endif
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
	ContinuousRefreshInfo *pContinuousRefreshInfo = new ContinuousRefreshInfo(m_pScreenHistory_Current->GetObj(),atoi(sTime.c_str()));
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::CMD_Continuous_Refresh %d %p",
		pContinuousRefreshInfo->m_iInterval,&Orbiter::ContinuousRefresh);
#endif
	CallMaintenanceInMiliseconds( pContinuousRefreshInfo->m_iInterval * 1000, &Orbiter::ContinuousRefresh, pContinuousRefreshInfo, pe_ALL );
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
	// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
	g_pPlutoLogger->Write(LV_STATUS,"CMD_Bind_Icon %s %s %d (size %d)",sPK_DesignObj.c_str(),sType.c_str(),(int) bChild,(int) m_mapObj_Bound.size());
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
				if(  ( *iAction )->m_PK_Command==COMMAND_Set_Current_Location_CONST  )
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
		if(  iLocation!=-1 && iLocation < int(m_dequeLocation.size()))
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
				if(  ( *iAction )->m_PK_Command==COMMAND_Set_Current_User_CONST  )
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

	g_pPlutoLogger->Write(LV_STATUS,"CMD_Bind_Icon added %s %s %d (size %d)",sPK_DesignObj.c_str(),sType.c_str(),(int) bChild,(int) m_mapObj_Bound.size());

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
	//don't simulate any event; we're reloading
	if(m_bQuit)
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Simulate mouse click at position: %d, %d", x, y);
#endif

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	BeginPaint();
	PlutoColor color(255, 0, 0, 100);
	SolidRectangle(x - 5, y - 5, 10, 10, color, 50);
	UpdateRect(PlutoRectangle(x - 5, y - 5, 10, 10), PlutoPoint(0, 0));

	//render current screen id
	SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color, 50);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->GetObj());
	text2.m_rPosition = rect2;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Current screen: " + this->GetCurrentScreenID();
	RenderText(sText,&text2, pTextStyle);
	UpdateRect(PlutoRectangle(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25), PlutoPoint(0, 0));
	EndPaint();

	RegionDown(x, y);
}

/*virtual*/ void Orbiter::SimulateKeyPress(long key)
{
	//don't simulate any event; we're reloading
	if(m_bQuit)
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Simulate key press. Key code: %d", key);
#endif

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	BeginPaint();

	//render a text with the
	PlutoColor color(200, 200, 200, 100);
	SolidRectangle(5, m_iImageHeight - 30, 200, 25, color, 50);
	PlutoRectangle rect(5, m_iImageHeight - 30, 200, 25);
	DesignObjText text(m_pScreenHistory_Current->GetObj());
	text.m_rPosition = rect;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Key code: " + StringUtils::ltos(key);
	RenderText(sText,&text, pTextStyle);

	//render current screen id
	PlutoColor color2(255, 0, 0, 100);
	SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color2, 50);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->GetObj());
	text2.m_rPosition = rect2;
	sText = "Current screen: " + this->GetCurrentScreenID();
	RenderText(sText,&text2, pTextStyle);
	UpdateRect(PlutoRectangle(5, m_iImageHeight - 30, 200, 25), PlutoPoint(0, 0));
	EndPaint();

	HandleButtonEvent(key);
	StopRepeatRelatedEvents();
}

time_t Orbiter::GetLastScreenChangedTime()
{
	return NULL != m_pScreenHistory_Current ? m_pScreenHistory_Current->TimeCreated() : time(NULL);
}

string Orbiter::GetCurrentScreenID()
{
	return
		(
		NULL != m_pScreenHistory_Current				&&
		NULL != m_pScreenHistory_Current->GetObj()
		)
		?
		m_pScreenHistory_Current->GetObj()->m_ObjectID :
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
		RenderObjectAsync(pObj);
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

	//if a button doesn't have a mng as selected state (see bDisableEffects), then the png
	//used in normal state will be rendered for selected state + a blue rectangle to show the selection
	if(!bIsMNG && pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		if(pObj->m_vectSelectedGraphic.size() != 0)
			pVectorPlutoGraphic = &pObj->m_vectSelectedGraphic;
		else
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
		m_pCacheImageManager->IsImageInCache(pPlutoGraphic->m_Filename, pObj->m_Priority)
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
			m_pCacheImageManager->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, pObj->m_Priority);
			sFileName = m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		//TODO: same logic for in-memory data
		if( (sFileName.empty() || iSizeGraphicFile) && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
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
				//eGraphicFormat eGF = pPlutoGraphic->m_GraphicFormat;
				eGraphicManagement eGM = pPlutoGraphic->m_GraphicManagement;
				string sMNGFileName = pPlutoGraphic->m_Filename;

				vector<PlutoGraphic *>::iterator itPlutoGraphic;
				for(itPlutoGraphic = pVectorPlutoGraphic->begin(); itPlutoGraphic != pVectorPlutoGraphic->end(); ++itPlutoGraphic)
					delete *itPlutoGraphic;
				pVectorPlutoGraphic->clear();

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

		default:;

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

	if(!pPlutoGraphic->IsEmpty())
		RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
#ifdef DEBUG
	else
		g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", pObj->m_ObjectID.c_str());
#endif

	if(!bIsMNG && pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
		SelectObject(pObj, point);
}

/*virtual*/ void Orbiter::GetRepeatedKeysForScreen(DesignObj_Orbiter* pObj, string& sKeysList)
{
	if(pObj->m_bRepeatParm)
	{
		if(sKeysList.length())
			sKeysList += ",";

		sKeysList += StringUtils::ltos(pObj->m_iPK_Button);
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
	if(pObj->m_bRepeatParm && pObj->m_iPK_Button == iPK_Button)
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
		RenderGraphic(pPlutoGraphic, pObj->m_rBackgroundPosition, pObj->m_bDisableAspectLock);

		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(pObj == m_pObj_Highlighted)
			DoHighlightObject();

		UpdateRect(pObj->m_rPosition, NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0));
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
	case DEVICETEMPLATE_VirtDev_IR_Receiver_CONST:
		return m_pLocationInfo->m_dwPK_Device_IRReceiver;

	case DEVICETEMPLATE_VirtDev_LCDVFD_CONST:
		return m_pLocationInfo->m_dwPK_Device_LCD_VFD;

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

	case DEVICETEMPLATE_This_Orbiters_Now_Playing_CONST:
		return m_dwPK_Device_NowPlaying;

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

	case DEVICETEMPLATE_VirtDev_Local_Media_Player_CONST:
		return m_dwPK_Device_LocalMediaPlayer;

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
		pObj = m_pScreenHistory_Current->GetObj();
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


	pObj->m_dwTimeoutSeconds = atoi(sTime.c_str());

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "set timeout on %s to %d  %p = %p",pObj->m_ObjectID.c_str(),pObj->m_dwTimeoutSeconds,pObj,m_pScreenHistory_Current->GetObj() );
#endif
	if( pObj==m_pScreenHistory_Current->GetObj() && pObj->m_dwTimeoutSeconds )
		CallMaintenanceInMiliseconds( pObj->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) pObj, pe_ALL, true );
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
	DesignObjText text(m_pScreenHistory_Current->GetObj());
	text.m_rPosition = rect;
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
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj, m_pScreenHistory_Current->GetObj() );
	if( !pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"MousePointerOverUnknownObject: %s",sPK_DesignObj.c_str());
		return;
	}
	int X = pObj->m_rPosition.X + pObj->m_rPosition.Width/2;
	int Y = pObj->m_rPosition.Y + pObj->m_rPosition.Height/2;
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

	string sErrorMessage = "Cannot deserialize .info file.  It is version " + StringUtils::ltos(iSC_Version) +
		" and I need " + StringUtils::ltos(ORBITER_SCHEMA) + ".";
	g_pPlutoLogger->Write(LV_CRITICAL, sErrorMessage.c_str());

	if( iSC_Version>ORBITER_SCHEMA )
	{
		PromptUser("I'm sorry.  This version of Orbiter is too old.  It uses schema " + StringUtils::itos(ORBITER_SCHEMA)
			+ " instead of " + StringUtils::itos(iSC_Version) + ".  Please install a newer version.");
		OnQuit();
		exit(0);
	}
	else
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes";
		mapPrompts[prNo]     = "No";
		int iResponse = PromptUser("The user interface which your Core generated (" + StringUtils::itos(iSC_Version) + ") is too old for this Orbiter(" + StringUtils::itos(ORBITER_SCHEMA) + ").  Shall I ask the Core to rebuild it now?",0,&mapPrompts);
		if( iResponse==prNo )
		{
			OnQuit();
			exit(0);
			return false;
		}

		RegenOrbiter();
		OnQuit();

#ifdef WIN32
		OnUnexpectedDisconnect(); //force a reload
#else
		exit(0);
#endif
	}

	return false;
}

// Temporary function to debug a problem with the screen history
void Orbiter::DumpScreenHistory()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
	string s = "Screen history: size: " + StringUtils::itos(int(m_listScreenHistory.size())) + "; screens: ";

	for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();++it)
	{
		ScreenHistory *pScreenHistory = *it;
		s += pScreenHistory->ToString() + ", ";
	}
	g_pPlutoLogger->Write(LV_WARNING, "%s", s.c_str());
}
//<-dceag-c59-b->

	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** If you don't know the location, you can also set just the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

void Orbiter::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
	deque<class LocationInfo *>::iterator it;
	for(it = m_dequeLocation.begin(); it != m_dequeLocation.end(); ++it)
	{
		LocationInfo *pLocationInfo = *it;
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
	deque<LocationInfo *>::iterator it;
	for(it = m_dequeLocation.begin(); it != m_dequeLocation.end(); ++it)
	{
		LocationInfo *pLocationInfo = *it;
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
		/** @param #144 Go Back */
			/** Go back after sending the command if it does not contain another goto screen or go back */

void Orbiter::CMD_Send_Message(string sText,bool bGo_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c389-e->
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::CMD_Send_Message %s",sText.c_str());
#endif
	bool bContainsGoto=false;
	string sMessage = SubstituteVariables(sText,NULL,0,0);

	if(sMessage != "")
	{
		Message *pMessageOut = new Message(sMessage);
		if( pMessageOut->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
			pMessageOut->m_dwPK_Device_To=m_dwPK_Device;
		if( pMessageOut->m_dwMessage_Type==MESSAGETYPE_COMMAND && (pMessageOut->m_dwID==COMMAND_Go_back_CONST || pMessageOut->m_dwID==COMMAND_Goto_Screen_CONST) )
			bContainsGoto=true;

		for(vector<Message *>::iterator it = pMessageOut->m_vectExtraMessages.begin();
			it != pMessageOut->m_vectExtraMessages.end(); ++it)
		{
			Message *pExtraMessage = *it;
			if( pExtraMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
				pExtraMessage->m_dwPK_Device_To=m_dwPK_Device;
			if( pExtraMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && (pExtraMessage->m_dwID==COMMAND_Go_back_CONST || pExtraMessage->m_dwID==COMMAND_Goto_Screen_CONST) )
				bContainsGoto=true;
		}

		QueueMessageToRouter(pMessageOut);
	}
	if( bGo_Back && !bContainsGoto )
		CMD_Go_back("","");
}

void Orbiter::ResetState(DesignObj_Orbiter *pObj, bool bDontResetState)
{
	if(  pObj->m_bDontResetState || pObj->m_bOneTimeDontReset  )
	{
		pObj->m_bOneTimeDontReset=false;
		bDontResetState=true;
	}
	else if(  !bDontResetState  )
	{
		pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;
		pObj->m_bHidden=pObj->m_bHideByDefault;
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
if( sPK_DesignObj.find("4883")!=string::npos )
int k=1;
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
		pt.X = m_sScreenSize.Height - pt.Y - pObj_Popup->m_rPosition.Height;
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
	ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pObj_Popup, &pt );
	HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );

	pPopup->m_bDontAutohide = bDont_Auto_Hide;

	CMD_Refresh("");
	/*
	if( sName=="remote" )
	{
	int k=2;
	}
	*/
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
	/*
	if( sName=="remote" )
	{
	int k=2;
	}
	*/
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if( m_pObj_Highlighted )
		UnHighlightObject();

	DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj_CurrentScreen);
	g_pPlutoLogger->Write(LV_CRITICAL,"remove popup %s",sName.c_str());

	if( pObj )
	{
		for(list<class PlutoPopup*>::iterator it=pObj->m_listPopups.begin();it!=pObj->m_listPopups.end();++it)
		{
			if( (*it)->m_sName==sName || sName.size()==0 )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					ObjectOffScreen((*it)->m_pObj);
				else
					g_pPlutoLogger->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());

				if( m_pActivePopup==(*it) )
					m_pActivePopup=NULL;

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
			if( (*it)->m_sName==sName || sName.size()==0 )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					ObjectOffScreen((*it)->m_pObj);
				else
					g_pPlutoLogger->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());

				if( m_pActivePopup==(*it) )
					m_pActivePopup=NULL;

				delete *it;
				m_listPopups.erase(it);
				break;
			}
		}
	}
//	if( m_pObj_Highlighted )
//		HighlightFirstObject();
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

	CMD_Set_Variable(VARIABLE_Filename_CONST, pOrbiterFileBrowser_Entry->m_sFilename);
	CMD_Set_Variable(VARIABLE_PK_MediaType_CONST, StringUtils::itos(iPK_MediaType));

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
	VectDesignObj_Orbiter::iterator it;
	for(it = pVectDesignObj_Orbiter->begin(); it != pVectDesignObj_Orbiter->end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = *it;
		if(  pDesignObj_Orbiter!=m_pScreenHistory_Current->GetObj()  )  // We just did the screen itself above
		{
			ExecuteCommandsInList( &pDesignObj_Orbiter->m_Action_LoadList, pDesignObj_Orbiter, smLoadUnload, 0, 0 );
		}
	}

	for(it = pVectDesignObj_Orbiter->begin(); it != pVectDesignObj_Orbiter->end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = *it;
		if ( pDesignObj_Orbiter->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST )
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
	if( !m_pScreenHistory_Current || (pObj->m_rPosition.Width==m_pScreenHistory_Current->GetObj()->m_rPosition.Width && pObj->m_rPosition.Height==m_pScreenHistory_Current->GetObj()->m_rPosition.Height) )
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

void Orbiter::ParseGrid(DesignObj_DataGrid *pObj_Datagrid)
{
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
			pObj_Datagrid->m_MaxCol = ( pObj_Datagrid->m_rPosition.Height - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
		else
			pObj_Datagrid->m_MaxCol = ( pObj_Datagrid->m_rPosition.Width - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
		if( pObj_Datagrid->m_FirstColumnWidth )
			pObj_Datagrid->m_MaxCol++;  // WE deducted the first column above
	}
	else if ( pObj_Datagrid->m_FixedColumnWidth == 0 ) // Do we know the number of columns but not their size?
	{
		if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->m_rPosition.Height / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
		else
			pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->m_rPosition.Width / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
	}
	pObj_Datagrid->m_MaxRow = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Rows_CONST ).c_str(  ) );
	if ( pObj_Datagrid->m_MaxRow == 0 )
	{
		if(  pObj_Datagrid->m_FixedRowHeight==0  )
			pObj_Datagrid->m_MaxRow=1;
		else if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_MaxRow = ( pObj_Datagrid->m_rPosition.Width - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
		else
			pObj_Datagrid->m_MaxRow = ( pObj_Datagrid->m_rPosition.Height - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
		if( pObj_Datagrid->m_FirstRowHeight )
			pObj_Datagrid->m_MaxRow++;  // WE deducted the first column above
	}
	else if ( pObj_Datagrid->m_FixedRowHeight == 0 ) // Do we know the number of columns but not their size?
	{
		if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_FixedRowHeight = (pObj_Datagrid->m_rPosition.Width - (pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1))/pObj_Datagrid->m_MaxRow;
		else
			pObj_Datagrid->m_FixedRowHeight = (pObj_Datagrid->m_rPosition.Height - (pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1))/pObj_Datagrid->m_MaxRow;
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
	pObj_Datagrid->m_iPK_DeviceTemplate = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_DeviceTemplate_CONST ).c_str(  ) );
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

	//also adjust the datagrid height not to show the black slide on the bottom.
	if( m_iRotation == 90 || m_iRotation == 270 )
		pObj_Datagrid->m_rPosition.Width =
		pObj_Datagrid->m_MaxRow * pObj_Datagrid->m_FixedRowHeight +
		(pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1);
	else
		pObj_Datagrid->m_rPosition.Height =
		pObj_Datagrid->m_MaxRow * pObj_Datagrid->m_FixedRowHeight +
		(pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1);

	pObj_Datagrid->m_bParsed=true;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Go to the media guide if applicable, otherwise forward to media plugin. */

void Orbiter::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	if( m_iPK_DesignObj_Guide )
		CMD_Goto_DesignObj( 0, StringUtils::itos(m_iPK_DesignObj_Guide), "","", false,false );
	else
	{
		DCE::CMD_Guide CMD_Guide(m_dwPK_Device,m_dwPK_Device_MediaPlugIn);
		SendCommand(CMD_Guide);
	}
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** First time stops any media playing.  Second time turns the display off.  Third time powers off the media director. */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void Orbiter::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
	if( m_dwPK_Device_NowPlaying )
	{
		DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,0,0,0,StringUtils::itos( m_pLocationInfo->PK_EntertainArea ));
		SendCommand(CMD_MH_Stop_Media);
	}
	else if( m_bDisplayOn )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Powering off monitor");
#endif
		CMD_Display_OnOff("0",false);
	}
	else
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Monitor already off");
#endif
		/*  TODO -- This makes it too easy to power off accidentally by hitting the button multiple times
		For now the user will just hit the 'power' option
		DCE::CMD_Halt_Device CMD_Halt_Device(m_dwPK_Device,m_dwPK_Device_LocalAppServer,m_dwPK_Device,"0");
		SendCommand(CMD_Halt_Device);
		*/
	}
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** If at a remote control, forward to media plugin.  Otherwise go back 1 screen. */

void Orbiter::CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_bIsARemoteControl )
	{
		DCE::CMD_Back_Prior_Menu CMD_Back_Prior_Menu(m_dwPK_Device,m_dwPK_Device_MediaPlugIn);
		SendCommand(CMD_Back_Prior_Menu);
	}
	else
		CMD_Go_back("","");
}

//<-dceag-c363-b->

	/** @brief COMMAND: #363 - Back / Clear Entry */
	/** If at a remote control, forward to media plugin.  Otherwise clear typing */

void Orbiter::CMD_Back_Clear_Entry(string &sCMD_Result,Message *pMessage)
//<-dceag-c363-e->
{
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_bIsARemoteControl )
	{
		DCE::CMD_Back_Clear_Entry CMD_Back_Clear_Entry(m_dwPK_Device,m_dwPK_Device_MediaPlugIn);
		SendCommand(CMD_Back_Clear_Entry);
	}
	else
		CaptureKeyboard_EditText_DeleteLastChar(  );
}
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the highlighted item */

void Orbiter::CMD_EnterGo(string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	if(GotActivity())
	{
		/*
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden() )
		{
			// Selected the highlighted object as well as it's parents.  If this was clicked, this would happen
			// automatically, but since we're using the navigation we have to walk the tree ourselves
			DesignObj_Orbiter *pObj = m_pObj_Highlighted;
			while(pObj)
			{
				SelectedObject(pObj,smNavigation);
				pObj = (DesignObj_Orbiter *) pObj->m_pParentObject;
			}
		}

		HandleButtonEvent(BUTTON_Any_key_CONST);
		*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");
	}
#ifdef DEBUG
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring enter");
	}
#endif
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlight up */

void Orbiter::CMD_Move_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	if(GotActivity())
	{
		/*
		HighlightNextObject(DIRECTION_Up_CONST);
		HandleButtonEvent(BUTTON_Any_key_CONST);
		*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Up_Arrow_CONST), "");
	}
#ifdef DEBUG
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlight down */

void Orbiter::CMD_Move_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	if(GotActivity())
	{
		/*
		HighlightNextObject( DIRECTION_Down_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Down_Arrow_CONST), "");
	}
#ifdef DEBUG
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlight left */

void Orbiter::CMD_Move_Left(string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	if( GotActivity(  ) )
	{
		/*
		HighlightNextObject( DIRECTION_Left_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Left_Arrow_CONST), "");
	}
#ifdef DEBUG
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlight right */

void Orbiter::CMD_Move_Right(string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	if( GotActivity(  ) )
	{
		/*
		HighlightNextObject( DIRECTION_Right_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Right_Arrow_CONST), "");
	}
#ifdef DEBUG
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring move");
	}
#endif
}
//<-dceag-c689-b->

	/** @brief COMMAND: #689 - Update Time Code */
	/** Update the time code of the current media */
		/** @param #41 StreamID */
			/** The Stream to update */
		/** @param #102 Time */
			/** The current time.  If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
		/** @param #132 Total */
			/** If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
		/** @param #133 Speed */
			/** The current speed */
		/** @param #134 Title */
			/** For DVD's, the title */
		/** @param #135 Section */
			/** For DVD's, the section */

void Orbiter::CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,Message *pMessage)
//<-dceag-c689-e->
{
	m_sNowPlaying_Speed = sSpeed;

	if( !m_pObj_NowPlaying_TimeShort_OnScreen && !m_pObj_NowPlaying_TimeLong_OnScreen && !m_pObj_NowPlaying_Speed_OnScreen )
		return;

	string::size_type tabTime = sTime.find('\t');
	string::size_type tabTotal = sTotal.find('\t');

	if( tabTime==string::npos )
	{
		if( m_pObj_NowPlaying_TimeShort_OnScreen )
			m_sNowPlaying_TimeShort = sTime;

		if( m_pObj_NowPlaying_TimeLong_OnScreen )
			m_sNowPlaying_TimeLong = sTime;
	}
	else
	{
		if( m_pObj_NowPlaying_TimeShort_OnScreen )
			m_sNowPlaying_TimeShort = sTime.substr(0,tabTime);

		if( m_pObj_NowPlaying_TimeLong_OnScreen )
			m_sNowPlaying_TimeLong = m_sNowPlaying_TimeShort + " (" + sTime.substr(tabTime) + ")";
	}

	if( sTotal.size() )
	{
		m_sNowPlaying_TimeLong += m_mapTextString[TEXT_Total_CONST];

		if( tabTotal==string::npos )
			m_sNowPlaying_TimeLong += sTotal;
		else
			m_sNowPlaying_TimeLong = sTotal.substr(0,tabTotal) + " (" + sTime.substr(tabTotal) + ")";
	}

	if( m_pObj_NowPlaying_TimeShort_OnScreen && m_pObj_NowPlaying_TimeShort_OnScreen->m_bOnScreen )
		RenderObjectAsync(m_pObj_NowPlaying_TimeShort_OnScreen);
	if( m_pObj_NowPlaying_TimeLong_OnScreen && m_pObj_NowPlaying_TimeLong_OnScreen->m_bOnScreen && m_pObj_NowPlaying_TimeShort_OnScreen!=m_pObj_NowPlaying_TimeShort_OnScreen )
		RenderObjectAsync(m_pObj_NowPlaying_TimeLong_OnScreen);
	if( m_pObj_NowPlaying_Speed_OnScreen && m_pObj_NowPlaying_Speed_OnScreen->m_bOnScreen && m_pObj_NowPlaying_Speed_OnScreen!=m_pObj_NowPlaying_TimeShort_OnScreen )
		RenderObjectAsync(m_pObj_NowPlaying_Speed_OnScreen);
	if( m_pMouseBehavior )
		m_pMouseBehavior->SetMediaInfo(sTime,sTotal,sSpeed,sTitle,sSection);
}

void Orbiter::RenderShortcut(DesignObj_Orbiter *pObj)
{
	string sCharToRender;
	if(pObj->m_iPK_Button >= BUTTON_1_CONST && pObj->m_iPK_Button <= BUTTON_9_CONST)
		sCharToRender += '0' + pObj->m_iPK_Button - BUTTON_1_CONST + 1;
	else if(pObj->m_iPK_Button == BUTTON_0_CONST)
		sCharToRender += '0';

	if(sCharToRender != "")
	{
		PlutoPoint AbsPos = NULL != m_pActivePopup ? m_pActivePopup->m_Position : PlutoPoint(0, 0);
		PlutoPoint textPos(AbsPos.X + pObj->m_rPosition.X + 5, AbsPos.Y + pObj->m_rPosition.Y + 5);

		TextStyle *pTextStyle = m_mapTextStyle_Find(1);
		PlutoColor OldColor = pTextStyle->m_ForeColor;
		pTextStyle->m_ForeColor.m_Value = 0xFF2020;
		pTextStyle->m_iPixelHeight += 15;

		PlutoRectangle rect(textPos.X, textPos.Y, 30, pTextStyle->m_iPixelHeight + 5);
		DesignObjText text(m_pScreenHistory_Current->GetObj());
		text.m_rPosition = rect;

		RenderText(sCharToRender,&text, pTextStyle);
		pTextStyle->m_iPixelHeight -= 15;
		pTextStyle->m_ForeColor = OldColor;
	}
}

int Orbiter::HandleNotOKStatus(string sStatus,string sRegenStatus,int iRegenPercent)
{
	g_pPlutoLogger->Write(LV_STATUS,"HandleNotOKStatus %s",sStatus.c_str());
	if( sStatus=="R" || sStatus=="r" )
		return MonitorRegen(m_dwPK_Device);

	if( sStatus=="N" || ((sStatus=="D" || sStatus=="U") && !m_dwPK_Device) )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes - Reset it now";
		mapPrompts[prNo]     = "No - I'll do it later";
		int iResponse = PromptUser("This new Orbiter is ready to go.  But all your devices need to do a reload before you can use it.  This takes about 15 seconds.", 0, &mapPrompts);
		if( iResponse==prYes )
		{
			string sResponse;
			Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
			event_Impl.m_pClientSocket->SendString( "RELOAD" );
			if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
			{
				CannotReloadRouter();
				g_pPlutoLogger->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
			}
			Sleep(10000);
			return 2; // Retry
		}
		else
		{
			OnQuit();
			exit(0);
		}

		return 0; // Quit
	}
	else if( sStatus=="n" )
	{
		PromptUser("Something went wrong and this orbiter's user interface wasn't created.  In Pluto Admin, go to Wizard, Devices, Orbiters and click Regen for Orbiter #" + StringUtils::itos(m_dwPK_Device) + " and try later");
		OnQuit();
		exit(0);
		return 0;
	}
	else if( sStatus=="D" || sStatus=="U" )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes";
		mapPrompts[prNo]     = "No";

		int iResponse = prNo;
		if( sStatus=="D" )
			iResponse = PromptUser("Something went wrong. The device number, " + StringUtils::itos(m_dwPK_Device) + ", doesn't seem to be an orbiter.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);
		else
			iResponse = PromptUser("Something went wrong.  The device number, " + StringUtils::itos(m_dwPK_Device) + ", is not known to the Core.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);

		if( iResponse==prYes )
		{
			m_dwPK_Device = 0;
			Simulator::GetInstance()->m_sDeviceID = "";
			Simulator::GetInstance()->SaveConfigurationFile();
		}
		OnQuit();
		exit(0);
		return 0;
	}

	return 1;
}

bool Orbiter::RouterNeedsReload()
{
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::RouterNeedsReload");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	string sStatus,sRegenStatus;
	int iRegenPercent;
	DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		m_dwPK_Device,&sStatus,&sRegenStatus,&iRegenPercent);
	CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		PromptUser("Sorry.  There is a problem creating the new orbiter");
		return 0;
	}
	CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
	delete pResponse;

	HandleNotOKStatus(sStatus,sRegenStatus,iRegenPercent);
	return false;  // If the user wants to do a reload, this will have already sent the request
}

int Orbiter::DeviceIdInvalid()
{
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::DeviceIdInvalid");
	if( m_dwPK_Device )
	{
		PromptUser("Something went wrong.  The device number, " + StringUtils::itos(m_dwPK_Device) + ", is reported as invalid.");
		OnQuit();
		exit(0);
		return 0;
	}

	int PK_Device = 0;
	if(IsSelfInstallable())
	{
		//Skip prompt dialogs. Create the orbiter automatically, using preseeded install values
		PK_Device = SetupNewOrbiter();
	}
	else
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo, prCancel};
		mapPrompts[prYes]    = "Yes - This is a new Orbiter";
		mapPrompts[prNo]     = "No - There is already a Device ID for this Orbiter";
		mapPrompts[prCancel] = "Cancel";
		int iResponse = PromptUser("This seems to be a new Orbiter.  Shall I set it up for you?", 0, &mapPrompts);
		if( iResponse == prCancel || PROMPT_CANCEL == iResponse )
		{
			OnQuit();
			exit(0);
			return 0;
		}
		if( iResponse == prYes )
			PK_Device=SetupNewOrbiter();
		else
		{
			PK_Device=PickOrbiterDeviceID();
			if(PROMPT_CANCEL == PK_Device)
			{
				OnQuit();
				exit(0);
				return 0;
			}
		}
	}

	if( !PK_Device )
	{
		OnQuit();
		exit(0);
	}

	return PK_Device;
}

int Orbiter::PickOrbiterDeviceID()
{
	map<int,string> mapDevices;
	GetDevicesByCategory(DEVICECATEGORY_Orbiter_CONST,&mapDevices);
	return PromptUser("Which Orbiter is this?  Be careful.  Don't choose an Orbiter that is running on another device or it will be disconnected when this one connects.",0, &mapDevices);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int Orbiter::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
#ifndef WIN32
	return PROMPT_CANCEL;  // TEMP TODO HACK -- we need to implement this for Linux SDL
#endif

#ifndef WINCE
	map<int,int> mapResponse;
	cout << sPrompt << endl;
	int iCount=1;

	if( p_mapPrompts )
	{
		for(map<int,string>::iterator it=p_mapPrompts->begin();it!=p_mapPrompts->end();++it)
		{
			mapResponse[iCount]=it->first;
			cout << StringUtils::itos(iCount++) + ". " + it->second << endl;
		}

		cout << endl
			<< "Please choose: ";
	}
	else
		cout << "Press Enter to continue";

	int iResponse=0;
	while(true)
	{
		cin >> iResponse;
		if( !p_mapPrompts )
			return 0;
		if( iResponse>0 && iResponse<=int(mapResponse.size()) )
			return mapResponse[iResponse];
		cout << endl << "Invalid.  Please try again: ";
	}
#endif
	return PROMPT_CANCEL;
}
//-----------------------------------------------------------------------------------------------------
int Orbiter::SetupNewOrbiter()
{
	g_pPlutoLogger->Write(LV_STATUS,"start SetupNewOrbiter");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	while(true)
	{
		string sResponse;
		if( !event_Impl.m_pClientSocket->SendString("READY") ||
			!event_Impl.m_pClientSocket->ReceiveString(sResponse) ||
			sResponse.size()==0 )
			return 0;  // Something went wrong

		if( sResponse=="YES" )
			break;
		Sleep(2000);
	}

	g_pPlutoLogger->Write(LV_STATUS,"SetupNewOrbiter prompting for inputs");

	int PK_Users = PromptFor("Users");
	if( PROMPT_CANCEL == PK_Users )
		return 0;

	int PK_Room = PromptFor("Room");
	if( PROMPT_CANCEL == PK_Room )
		return 0;

	int PK_Skin = PromptFor("Skin");
	if( PROMPT_CANCEL == PK_Skin )
		return 0;

	int PK_Language = PromptFor("Language");
	if( PROMPT_CANCEL == PK_Language )
		return 0;

	map<int,string> mapResponseYNC;
	mapResponseYNC[0]="Yes";
	mapResponseYNC[1]="No";
	mapResponseYNC[2]="Cancel";

	int Width=0,Height=0;
	string sType;
#ifdef WIN32
#ifdef WINCE
	sType="CE";
#else
	sType="Windows";
#endif
	RECT rc;
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &rc);
	Width=rc.right;
	Height=rc.bottom;
#else
	sType="Linux";
	// HOW to do this??  TODO
#endif

	int PK_Size=0;
	PK_Size=PromptFor("Size");
	if( PROMPT_CANCEL == PK_Size)
		return 0;

	int PK_Device=0;
	DCE::CMD_New_Orbiter_DT CMD_New_Orbiter_DT(m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse, sType,
		PK_Users,m_dwPK_DeviceTemplate,m_sMacAddress,PK_Room,Width,Height,PK_Skin,PK_Language,PK_Size,&PK_Device);

	CMD_New_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_New_Orbiter_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		g_pPlutoLogger->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter");
		PromptUser("Sorry.  There is a problem creating the new orbiter.  Please check the logs.");
		return 0;
	}
	CMD_New_Orbiter_DT.ParseResponse( pResponse );
	delete pResponse;

	if( !PK_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter #2");
		PromptUser("Sorry.  Orbiter Plugin could not create the device for some reason.  Please check the logs.");
		return 0;
	}

	Simulator::GetInstance()->m_sRouterIP = m_sIPAddress;
	Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(PK_Device);
	Simulator::GetInstance()->SaveConfigurationFile();

	g_pPlutoLogger->Write(LV_STATUS,"SetupNewOrbiter new orbiter %d",PK_Device);

	if( MonitorRegen(PK_Device)==0 )  // User hit cancel
	{
		OnQuit();
		exit(1);
		return 0; // Don't retry to load now
	}


	return PK_Device;  // Retry loading as the specified device
}
//-----------------------------------------------------------------------------------------------------
int Orbiter::MonitorRegen(int PK_Device)
{
	g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - starting");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	while(true)
	{
		string sStatus,sRegenStatus;
		int iRegenPercent;
		DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
			PK_Device,&sStatus,&sRegenStatus,&iRegenPercent);

		CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
		Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
		if( !pResponse || pResponse->m_dwID != 0 )
		{
			if(pResponse)
				delete pResponse;

			g_pPlutoLogger->Write(LV_CRITICAL,"MonitorRegen - unable to check status");
			DisplayProgress("",-1);
			PromptUser("Sorry.  There is a problem creating the new orbiter");
			return 0;
		}
		CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
		delete pResponse;

		if( sStatus!="r" && sStatus!="R" )
		{
			g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - DONE");
			break;
		}

		if( DisplayProgress(sRegenStatus,iRegenPercent) )
		{
			g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - User cancelled");
			DisplayProgress("",-1);
			return 0; // Don't try to start
		}
		Sleep(5000);
	}

	DisplayProgress("",-1);
	return 2; // Try again
}
//-----------------------------------------------------------------------------------------------------
int Orbiter::PromptFor(string sToken)
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);

	string sResults;
	DCE::CMD_Get_Orbiter_Options_DT CMD_Get_Orbiter_Options_DT(m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		sToken,&sResults);

	CMD_Get_Orbiter_Options_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Options_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		PromptUser("Sorry.  There is a problem getting the list of " + sToken);
		return 0;
	}
	CMD_Get_Orbiter_Options_DT.ParseResponse( pResponse );
	delete pResponse;

	map<int,string> mapResponse;
	vector<string> Choices;
	StringUtils::Tokenize(sResults,"\n",Choices);
	if( sToken=="Size" )
		mapResponse[0]="Default";

	for(vector<string>::iterator it = Choices.begin(); it != Choices.end(); ++it)
	{
		string sChoise = *it;
		string::size_type pos=0;
		int Choice = atoi(StringUtils::Tokenize(sChoise,"\t",pos).c_str());
		string sDescription = StringUtils::Tokenize(sChoise,"\t",pos);

		if( Choice && sDescription.size() )
			mapResponse[Choice]=sDescription;
	}

	if(Simulator::GetInstance()->PreseededInstallValueDefined(sToken))
	{
		int nValue = Simulator::GetInstance()->GetPreseededInstallValue(sToken);

		if(mapResponse.find(nValue) != mapResponse.end())
            return nValue;
		else if(mapResponse.size())
			return mapResponse.begin()->first;
	}

	return PromptUser("Please select the " + sToken,0,&mapResponse);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter::DisplayProgress(string sMessage, int nProgress)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::RegenOrbiter()
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	string sResponse;
	DCE::CMD_Regen_Orbiter_DT CMD_Regen_Orbiter_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		m_dwPK_Device,"","");
	CMD_Regen_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
	if( !event_Impl.SendMessage(CMD_Regen_Orbiter_DT.m_pMessage,sResponse) || sResponse!="OK" )
	{
		PromptUser("Sorry.  I was unable to send this message to the Core.  Please try again or use the Pluto Admin site.");
		return true;
	}
	PromptUser("The UI is being regenerated.  This will take 15-30 minutes.  If you get this same message again after the regeneration is finished, then that means the generator on the Core is too old and you will need to reset your Core so it can update itself.  Click 'OK' to monitor the progress.");
	return false;
}


void ReceivePingHandler(Socket *pSocket)
{

	g_pPlutoLogger->Write(LV_STATUS,"ReceivePingHandler - got ping for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
	pSocket->SendString("PONG");
	Orbiter *pOrbiter = (Orbiter *) g_pCommand_Impl;
	if( !pOrbiter || pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_mnuPingPong_CONST )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	else
	{
		string sMessage = StringUtils::PrecisionTime() + " Got PING: " +
			" S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#" +
			StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		NeedToRender render( pOrbiter, "ReceivePingHandler" );
		DesignObjText *pText = pOrbiter->FindText( pOrbiter->m_pScreenHistory_Current->GetObj(), TEXT_STATUS_CONST );
		if( pText  )
			pText->m_sText = sMessage + pText->m_sText;

		pOrbiter->RenderObjectAsync(pOrbiter->m_pScreenHistory_Current->GetObj());
	}
}

bool SendPingHandler(Socket *pSocket)
{
	g_pPlutoLogger->Write(LV_STATUS,"ReceivePingHandler - sending ping for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());

	Orbiter *pOrbiter = (Orbiter *) g_pCommand_Impl;
	if( !pOrbiter )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	NeedToRender render( pOrbiter, "SendPingHandler" );
	DesignObjText *pText = pOrbiter->FindText( pOrbiter->m_pScreenHistory_Current->GetObj(), TEXT_STATUS_CONST );
	if( pText  )
	{
		string sMessage = StringUtils::PrecisionTime() + " Send PING: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
	}

	string sResponse=pSocket->SendReceiveString("PING",PING_TIMEOUT);
	if( sResponse!="PONG" )
	{
		string sMessage = StringUtils::PrecisionTime() + " ***FAILED TO GET PONG***: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
		g_pPlutoLogger->Write(LV_STATUS,"ReceivePingHandler - failed to get pong for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
		return false;
	}
	if( !g_pCommand_Impl || pOrbiter->m_bQuit ) // The app closed on us
		return true;

	g_pPlutoLogger->Write(LV_STATUS,"ReceivePingHandler - got pong for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
	if( pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_mnuPingPong_CONST )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	else if( pText )
	{
		string sMessage = StringUtils::PrecisionTime() + " GOT PONG: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
	}
	return true;
}

//DEL
//todo: disable this to use wx custom progress bar dialogs
//#define X11_PROGRESS_BARS 0

bool Orbiter::WaitForRelativesIfOSD()
{
#if X11_PROGRESS_BARS
	if( !m_bIsOSD )
		return true;

	time_t tTimeout=time(NULL) + 20; // Wait 20 seconds for child devices to register
	map<int,bool> mapUnregisteredRelatives;
  map<string, bool> mapChildDevices;
	while( true )
	{
		int iUnregisteredRelatives = FindUnregisteredRelatives(&mapUnregisteredRelatives);
		if( !iUnregisteredRelatives )
			break;
		if( time(NULL)>tTimeout )
		{
			string sMessage = m_mapTextString[TEXT_Not_all_devices_started_CONST];
			PromptUser(sMessage);
			g_pPlutoLogger->Write(LV_WARNING,"Continuing anyway with %d devices not registered",iUnregisteredRelatives);
			break;
		}

		string sDescription = m_mapTextString[TEXT_Waiting_for_related_devices_CONST];

		for(map<int,bool>::iterator it=mapUnregisteredRelatives.begin();it!=mapUnregisteredRelatives.end();++it)
		{
			if( it->second )
				continue;
			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(it->first);
			sDescription += "  #" + StringUtils::itos(it->first) + "/" + (pDevice ? pDevice->m_sDescription : string(""));

		}

		g_pPlutoLogger->Write(LV_STATUS,"Waiting %d devices %s",iUnregisteredRelatives,sDescription.c_str());
		if( DisplayProgress(sDescription,100-(iUnregisteredRelatives*100/int(mapUnregisteredRelatives.size()))) )
		{
			DisplayProgress("",-1);
			g_pPlutoLogger->Write(LV_WARNING,"Orbiter::WaitForRelativesIfOSD user wants to abort");
			OnQuit();
			return false;
		}
		Sleep(1000); // Sleep and try again
	}
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::WaitForRelativesIfOSD exiting");
	DisplayProgress("",-1);
	return true;

#else // X11_PROGRESS_BARS
	if( !m_bIsOSD )
		return true;

	time_t tTimeout=time(NULL) + 20; // Wait 20 seconds for child devices to register
	map<int,bool> mapUnregisteredRelatives;
    map<string, bool> mapChildDevices;
	while( true )
	{
		int iUnregisteredRelatives = FindUnregisteredRelatives(&mapUnregisteredRelatives);
		if( !iUnregisteredRelatives )
			break;
		if( time(NULL)>tTimeout )
		{
			string sMessage = m_mapTextString[TEXT_Not_all_devices_started_CONST];
            g_pPlutoLogger->Write(LV_WARNING,"Deleting progress bar dialog");
			DisplayProgress("", mapChildDevices, -1);
            g_pPlutoLogger->Write(LV_WARNING,"Starting prompt user");
            PromptUser(sMessage);
			g_pPlutoLogger->Write(LV_WARNING,"Continuing anyway with %d devices not registered",iUnregisteredRelatives);
			return;
		}

		string sDescription = m_mapTextString[TEXT_Waiting_for_related_devices_CONST];
		for(map<int,bool>::iterator it=mapUnregisteredRelatives.begin();it!=mapUnregisteredRelatives.end();++it)
		{
			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(it->first);
			string sDevice = StringUtils::itos(it->first) + "|" + (pDevice ? pDevice->m_sDescription : string(""));
			mapChildDevices[sDevice] = it->second;
		}

		g_pPlutoLogger->Write(LV_STATUS,"Waiting %d devices %s",iUnregisteredRelatives,sDescription.c_str());
		if( DisplayProgress(sDescription, mapChildDevices, 100-(iUnregisteredRelatives*100/int(mapUnregisteredRelatives.size()))) )
		{
			DisplayProgress("", mapChildDevices, -1);
			g_pPlutoLogger->Write(LV_WARNING,"Orbiter::WaitForRelativesIfOSD user wants to abort");
			OnQuit();
			return false;
		}
		Sleep(1000); // Sleep and try again
	}
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::WaitForRelativesIfOSD finishing progress dialog");
	DisplayProgress("", mapChildDevices, -1);
	g_pPlutoLogger->Write(LV_STATUS,"Orbiter::WaitForRelativesIfOSD exiting");
	return true;
#endif // X11_PROGRESS_BARS
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-c741-b->

	/** @brief COMMAND: #741 - Goto Screen */
	/** Goto a specific screen. */
		/** @param #10 ID */
			/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
		/** @param #159 PK_Screen */
			/** The screen id. */

void Orbiter::CMD_Goto_Screen(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c741-e->
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbOnGotoScreen);
	if(pCallBackData)
	{
		GotoScreenCallBackData *pGotoScreenCallBackData = (GotoScreenCallBackData *)pCallBackData;
		pGotoScreenCallBackData->m_nPK_Screen = iPK_Screen;
	}
	if( ExecuteScreenHandlerCallback(cbOnGotoScreen) )
		return;

	if(NULL != m_pScreenHistory_Current && NULL != m_pScreenHistory_Current->GetObj())
		FireDeleteWxWidget(m_pScreenHistory_Current->GetObj());

	m_pScreenHandler->ResetCallBacks();
	bool bCreatedMessage=pMessage==NULL;
	if( pMessage==NULL )
		pMessage = new Message();

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Received goto screen message id %d params: message %p", iPK_Screen, pMessage);
	for(map<long, string>::iterator it = pMessage->m_mapParameters.begin(); it != pMessage->m_mapParameters.end(); it++)
	{
		string sValue = it->second;
		g_pPlutoLogger->Write(LV_WARNING, "Param id: %5d, value: %s", it->first, sValue.c_str());
	}
#endif

	m_pScreenHistory_NewEntry = new ScreenHistory(iPK_Screen, m_pScreenHistory_Current);
	m_pScreenHistory_NewEntry->ScreenID(sID);
	m_pScreenHandler->ReceivedGotoScreenMessage(iPK_Screen, pMessage);
	if( bCreatedMessage )
		delete pMessage;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ ScreenHandler *Orbiter::CreateScreenHandler()
{
#ifdef TEST_OSD
	return new OSDScreenHandler(this, &m_mapDesignObj);
#else
	return new ScreenHandler(this, &m_mapDesignObj);
#endif
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::LoadPlugins()
{
	/*
	list<string> listFiles;
	#ifdef WIN32
	FileUtils::FindFiles(listFiles,"/pluto/orbiter/","*.dll",false,true);
	#else
	FileUtils::FindFiles(listFiles,"/usr/pluto/orbiter/","*.so",false,true);
	#endif

	for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
	{
	ScreenHandler *pScreenHandler = PlugIn_Load(*it);
	if (pScreenHandler)
	{
	pScreenHandler->Register();
	}
	else
	{
	g_pPlutoLogger->Write(LV_CRITICAL, "Cannot load plug-in for %s",
	(*it).c_str());
	}
	}
	*/
}

ScreenHandler *Orbiter::PlugIn_Load(string sCommandLine)
{
	/*
	RAOP_FType RegisterAsPlugin;
	void * so_handle;
	string ErrorMessage;
	char MS_ErrorMessage[1024];

	if (sCommandLine == "")
	return NULL;

	#ifndef WIN32
	sCommandLine += ".so";
	if (sCommandLine.find("/") == string::npos)
	sCommandLine = "./" + sCommandLine;
	so_handle = dlopen(sCommandLine.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	#else
	so_handle = LoadLibrary(sCommandLine.c_str());
	#endif

	if (so_handle == NULL)
	{
	#ifndef WIN32
	ErrorMessage = dlerror();
	#else
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
	ErrorMessage = MS_ErrorMessage;
	#endif
	g_pPlutoLogger->Write(LV_CRITICAL, "Can't open plug-in file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
	return NULL;
	}

	#ifndef WIN32
	RegisterAsPlugin = (RAOP_FType) dlsym(so_handle, "RegisterAsOrbiterPlugIn");
	#else
	RegisterAsPlugin = (RAOP_FType) GetProcAddress((HMODULE) so_handle, "RegisterAsOrbiterPlugIn");
	#endif

	if (RegisterAsPlugin == NULL)
	{
	#ifndef WIN32
	ErrorMessage = dlerror();
	#else
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
	ErrorMessage = MS_ErrorMessage;
	#endif
	g_pPlutoLogger->Write(LV_CRITICAL, "Failed to load symbol 'RegisterAsPlugin' from file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
	return NULL;
	}

	g_pPlutoLogger->Write(LV_WARNING, "Loaded plug-in %s", sCommandLine.c_str());

	return RegisterAsPlugin(this,&m_mapDesignObj,g_pPlutoLogger);
	*/
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
bool Orbiter::ExecuteScreenHandlerCallback(CallBackType aCallBackType)
{
	ScreenHandlerCallBack pCallBack = m_pScreenHandler->m_mapCallBack_Find(aCallBackType);
	if(NULL != pCallBack)
		return CALL_MEMBER_FN(*m_pScreenHandler, pCallBack)(m_pScreenHandler->m_mapCallBackData_Find(aCallBackType));

	return false;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::GotoMainMenu()
{
	DCE::SCREEN_Main SCREEN_Main_(m_dwPK_Device, m_dwPK_Device,
		StringUtils::ltos(m_pLocationInfo->iLocation));
	string sResult;
	CMD_Goto_Screen("", SCREEN_Main_CONST, sResult, SCREEN_Main_.m_pMessage);
}
//-----------------------------------------------------------------------------------------------------
DesignObj_DataGrid *Orbiter::FindGridOnScreen(string sGridID)
{
	for(vector<class DesignObj_DataGrid *>::iterator it = m_vectObjs_GridsOnScreen.begin();
		it != m_vectObjs_GridsOnScreen.end(); ++it)
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = *it;
		if(pDesignObj_DataGrid->m_sGridID == sGridID)
			return pDesignObj_DataGrid;
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::ServiceScreenHandler(void *data)
{
	PLUTO_SAFETY_LOCK(vm, m_VariableMutex);
	if(!ExecuteScreenHandlerCallback(cbOnTimer))
		return;

	int *pInterval = (int *)data;
	StartScreenHandlerTimer(*pInterval);
	delete pInterval;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::StartScreenHandlerTimer(int nInterval /*in miliseconds*/)
{
	CallMaintenanceInMiliseconds(nInterval, &Orbiter::ServiceScreenHandler, new int(nInterval), pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::ScreenHandlerMsgInterceptor( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbMessageIntercepted);
	if(pCallBackData)
	{
		MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *)pCallBackData;
		pMsgInterceptorCellBackData->m_pSocket = pSocket;
		pMsgInterceptorCellBackData->m_pMessage = pMessage;
		pMsgInterceptorCellBackData->m_pDeviceFrom = pDeviceFrom;
		pMsgInterceptorCellBackData->m_pDeviceTo = pDeviceTo;

		return ExecuteScreenHandlerCallback(cbMessageIntercepted);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::PendingCallbackScheduled(OrbiterCallBack fnCallBack)
{
	PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
	for(map<int, PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		if(pCallBackInfo->m_fnCallBack == fnCallBack)
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::IsSelfInstallable()
{
	Simulator *pSimulator = Simulator::GetInstance();

	return
		pSimulator->PreseededInstallValueDefined("Users")		&&
		pSimulator->PreseededInstallValueDefined("Room")		&&
		pSimulator->PreseededInstallValueDefined("Skin")		&&
		pSimulator->PreseededInstallValueDefined("Language")	&&
		pSimulator->PreseededInstallValueDefined("Size");
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::RenderObjectAsync(DesignObj_Orbiter *pObj)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "RenderObjectAsync: %s", pObj->m_ObjectID.c_str());
#endif

	PLUTO_SAFETY_LOCK(nd, m_NeedRedrawVarMutex);
	m_vectObjs_NeedRedraw.push_back(pObj);
	nd.Release();
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::GetChildrenVisibilityContext(DesignObj_Orbiter *pObj,
										  map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	DesignObj_DataList::iterator it;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pChildObj = (DesignObj_Orbiter *)*it;
		mapVisibilityContext[pChildObj] = pChildObj->IsHidden();

		GetChildrenVisibilityContext(pChildObj, mapVisibilityContext);
	}
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::SetChildrenVisibilityContext(DesignObj_Orbiter *pObj,
											const map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	DesignObj_DataList::const_iterator it;
	map<DesignObj_Orbiter *, bool>::const_iterator itmap;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pChildObj = (DesignObj_Orbiter *)*it;

		itmap = mapVisibilityContext.find(pChildObj);
		if(itmap != mapVisibilityContext.end() && pChildObj->m_bHidden != itmap->second)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "* Changing visibility status for %s from %d to %d",
				pChildObj->m_ObjectID.c_str(), pChildObj->m_bHidden, itmap->second);
			pChildObj->m_bHidden = itmap->second;
		}

		SetChildrenVisibilityContext(pChildObj, mapVisibilityContext);
	}
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-c697-b->

	/** @brief COMMAND: #697 - Set Active Application */
	/** Indicates if there is an active computing application, other than some media, such as a file browser. */
		/** @param #2 PK_Device */
			/** The media director */
		/** @param #50 Name */
			/** The name of the application */
		/** @param #146 PK_QuickStartTemplate */
			/** The quick start template */

void Orbiter::CMD_Set_Active_Application(int iPK_Device,string sName,int iPK_QuickStartTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c697-e->
{
}

//<-dceag-c795-b->

	/** @brief COMMAND: #795 - Set Mouse Behavior */
	/** Indicates if the mouse should be locked to horizontal or vertical movements, how to handle range of motion, etc. */
		/** @param #3 PK_DesignObj */
			/** An object to lock the movement to */
		/** @param #39 Options */
			/** The following letter(s): [r/a]ramp/absolute */
		/** @param #126 Exclusive */
			/** If true, all existing mouse behavior will be removed */
		/** @param #211 Direction */
			/** a letter: [h]orizontal, [v]ertical, [b]oth */

void Orbiter::CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection,string &sCMD_Result,Message *pMessage)
//<-dceag-c795-e->
{
	if( m_pMouseBehavior )
		m_pMouseBehavior->Set_Mouse_Behavior(sOptions,bExclusive,sDirection,sPK_DesignObj);
}

void Orbiter::FireDeleteWxWidget(DesignObj_Orbiter *pObj)
{
	DesignObj_DataList::iterator it;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pChildObj = (DesignObj_Orbiter *)*it;
		if(pChildObj->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
			ExecuteScreenHandlerCallback(cbOnWxWidgetDelete);

		FireDeleteWxWidget(pChildObj);
	}
}
