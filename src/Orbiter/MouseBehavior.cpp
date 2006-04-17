#include "MouseBehavior.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"

#include "SpeedMouseHandler.h"
#include "LightMouseHandler.h"
#include "VolumeMouseHandler.h"
#include "MediaMouseHandler.h"
#include "LockedMouseHandler.h"
#include "KeyboardMouseHandler.h"

using namespace DCE;


//-----------------------------------------------------------------------------------------------------
//class MouseSensitivity
const int MouseSensitivity::SampleInterval = 250; // 250 ms
const int MouseSensitivity::DiscardSamplesOlderThanSec = 1500; // 1500 ms
const int MouseSensitivity::MinMoveToStart = 10;
const int MouseSensitivity::MaxMoveToStart = 5;
const int MouseSensitivity::MinMovePerSampleToChangeDir = 10;
const int MouseSensitivity::MaxMovePerSampleToChangeDir = 5;
const int MouseSensitivity::MinMoveAllSamplesToChangeDir = 40;
const int MouseSensitivity::MaxMoveAllSamplesToChangeDir = 20;
const int MouseSensitivity::HoldTime = 750;
const int MouseSensitivity::IgnoreMouseAfterReposition=10;

//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	m_pMouseHandler_Horizontal=m_pMouseHandler_Vertical=NULL;
	ProcessUtils::ResetMsTime();
	m_pOrbiter=pOrbiter;
	m_EMenuOnScreen=mb_None;
	m_pMouseGovernor = new MouseGovernor(this);
	m_pMouseIterator = new MouseIterator(this);
	Clear();
}

MouseBehavior::~MouseBehavior()
{
	delete m_pMouseGovernor;
	delete m_pMouseIterator;
}

void MouseBehavior::Clear()
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_EMenuOnScreen!=mb_None )
	{
		m_pOrbiter->CMD_Remove_Popup("","left");
		m_pOrbiter->CMD_Remove_Popup("","horiz");
	}

	/* there's a logic flaw that when popups are put on screen they're added to the following
	vect's, but not removed when the popup is removed, although they are marked as m_bOnScreen=false.
	These were created as vect's because the concept of popups didn't exist in the beginning.
	Perhaps they should be changed to something else that's easier to remove entries from.
	For now, with the new UI, we know that whenever the mouse behavior is reset we will
	have no grids or tab stops on screen.  Reset them so the list doesn't grow indefinately. */
	m_pOrbiter->m_vectObjs_GridsOnScreen.clear();
	m_pOrbiter->m_vectObjs_TabStops.clear();

	m_pObj_Locked_Vertical=m_pObj_Locked_Horizontal=NULL;
	ResetSamples();
	m_cLockedAxes = m_cLocked_Axis_Current = AXIS_LOCK_NONE;
	if( m_pMouseHandler_Vertical && m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
		m_pMouseHandler_Vertical=NULL; // Only delete it once
	delete m_pMouseHandler_Horizontal;
	delete m_pMouseHandler_Vertical;
	m_pMouseHandler_Horizontal=m_pMouseHandler_Vertical=NULL;
	m_iPK_Button_Mouse_Last=0;
    m_iTime_Last_Mouse_Down=m_iTime_Last_Mouse_Up=0;
	m_EMenuOnScreen=mb_None;
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
}

void MouseBehavior::Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Set_Mouse_Behavior -%s- %d -%s- -%s-",
					  sOptions.c_str(),(int) bExclusive,sDirection.c_str(),sDesignObj.c_str());
	if( sOptions.size()==0 || sDirection.size()==0 )
	{
		Clear();
		return; 
	}

	DesignObj_Orbiter *pObj=NULL;
	if( sDesignObj.size() )
		pObj = m_pOrbiter->FindObject(sDesignObj);

	MouseHandler *pMouseHandler=NULL;
//we need to add this to all projects

	switch(sOptions[0])
	{
	case 'L':
		pMouseHandler=new LockedMouseHandler(pObj,this);
		break;
	case 'S':
		pMouseHandler=new SpeedMouseHandler(pObj,this);
		break;
	case 'G':
		pMouseHandler=new LightMouseHandler(pObj,this);
		break;
	case 'V':
		pMouseHandler=new VolumeMouseHandler(pObj,this);
		break;
	case 'T':
		pMouseHandler=new MediaMouseHandler(pObj,this);
		break;
	case 'K':
		pMouseHandler=new KeyboardMouseHandler(pObj,this);
		break;
	}

	if( sDirection[0]=='Y' || sDirection[0]=='B' )
	{
		if( m_pMouseHandler_Vertical )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Horizontal=NULL; // We're deleting it
			delete m_pMouseHandler_Vertical;
		}
		m_pObj_Locked_Vertical=pObj;
		m_pMouseHandler_Vertical=pMouseHandler;
		m_sVerticalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_X || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_Y);
		if( m_cLocked_Axis_Current==AXIS_LOCK_Y )
			m_cLocked_Axis_Current=AXIS_LOCK_NONE;
	}
	if( sDirection[0]=='X' || sDirection[0]=='B' )
	{
		if( m_pMouseHandler_Horizontal )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Vertical=NULL; // We're deleting it
			delete m_pMouseHandler_Horizontal;
		}
		m_pObj_Locked_Horizontal=pObj;
		m_pMouseHandler_Horizontal=pMouseHandler;
		m_sHorizontalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_Y || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_X);
		if( m_cLocked_Axis_Current==AXIS_LOCK_X )
			m_cLocked_Axis_Current=AXIS_LOCK_NONE;
	}

	// set a call back function for each axis
	m_pMouseGovernor->SetBuffer(0);
	m_dwSamples[0]=ProcessUtils::GetMsTime();
	GetMousePosition(&m_pSamples[0]);
}


void MouseBehavior::Move(int X,int Y)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return; // Nothing to do

	char cLocked_Axis_Before = m_cLocked_Axis_Current;
	if( CheckForChangeInDirection(X,Y) )
	{
		m_pStartMovement.X=X;
		m_pStartMovement.Y=Y;

		if( m_cLocked_Axis_Current == AXIS_LOCK_X )
		{
			if( m_pObj_Locked_Horizontal )
				m_iLockedPosition = m_pObj_Locked_Horizontal->m_rPosition.Y + m_pObj_Locked_Horizontal->m_pPopupPoint.Y + m_pObj_Locked_Horizontal->m_rPosition.Height/2;
			else
				m_iLockedPosition = Y;
g_pPlutoLogger->Write(LV_CRITICAL,"Direction now X, y locked to %d",m_iLockedPosition);
		}
		else
		{
			if( m_pObj_Locked_Vertical )
				m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_pPopupPoint.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
			else
				m_iLockedPosition = X;
g_pPlutoLogger->Write(LV_CRITICAL,"Direction now Y, x locked to %d",m_iLockedPosition);
		}
		m_pMouseGovernor->SetBuffer(0);
		if( cLocked_Axis_Before==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
			m_pMouseHandler_Horizontal->Stop();
		else if( cLocked_Axis_Before==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
			m_pMouseHandler_Vertical->Stop();

		if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
			m_pMouseHandler_Horizontal->Start();
		else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
			m_pMouseHandler_Vertical->Start();
		return;
	}

	if( m_cLocked_Axis_Current == AXIS_LOCK_X )
		Y = m_iLockedPosition;
	if( m_cLocked_Axis_Current == AXIS_LOCK_Y )
		X = m_iLockedPosition;

	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
	{
		if( m_pMouseHandler_Horizontal->m_pObj && !m_pMouseHandler_Horizontal->m_pObj->m_bOnScreen )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Vertical=NULL;
			delete m_pMouseHandler_Horizontal;
			m_pMouseHandler_Horizontal=NULL;
		}
		else
			m_pMouseHandler_Horizontal->Move(X, Y);
	}
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
	{
		if( m_pMouseHandler_Horizontal->m_pObj && !m_pMouseHandler_Horizontal->m_pObj->m_bOnScreen )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Horizontal=NULL;
			delete m_pMouseHandler_Vertical;
			m_pMouseHandler_Vertical=NULL;
		}
		else
			m_pMouseHandler_Vertical->Move(X, Y);
	}
}

bool MouseBehavior::CheckForChangeInDirection(int X,int Y)
{
	unsigned long dwTime = ProcessUtils::GetMsTime();
	
	if( dwTime-m_dwSamples[0]<MouseSensitivity::SampleInterval )
	{
		// Hardly any time has passed since the last move.  We'll only consider this to be movement in a new direction if it exceeds
		// the indicated minimum and twice as much in one direction as the other

		int diffX = abs(m_pSamples[0].X - X);
		int diffY = abs(m_pSamples[0].Y - Y);
		if( m_cLocked_Axis_Current == AXIS_LOCK_NONE )
		{
			if( m_cLockedAxes & AXIS_LOCK_X && diffX>MouseSensitivity::MinMoveToStart && diffY<MouseSensitivity::MaxMoveToStart )
			{
	g_pPlutoLogger->Write(LV_FESTIVAL,"start X %d,%d",diffX,diffY);
				m_cLocked_Axis_Current = AXIS_LOCK_X;
				return true;
			}
			else if( m_cLockedAxes & AXIS_LOCK_Y && diffY>MouseSensitivity::MinMoveToStart && diffX<MouseSensitivity::MaxMoveToStart )
			{
	g_pPlutoLogger->Write(LV_FESTIVAL,"start Y %d,%d",diffX,diffY);
				m_cLocked_Axis_Current = AXIS_LOCK_Y;
				return true;
			}
		}
		else if( m_cLockedAxes & AXIS_LOCK_X && m_cLocked_Axis_Current == AXIS_LOCK_Y && diffX>MouseSensitivity::MinMovePerSampleToChangeDir && diffY<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"fast switch X %d,%d",diffX,diffY);
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
		else if( m_cLockedAxes & AXIS_LOCK_Y && m_cLocked_Axis_Current == AXIS_LOCK_X && diffY>MouseSensitivity::MinMovePerSampleToChangeDir && diffX<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"fast switch Y %d,%d",diffX,diffY);
			m_cLocked_Axis_Current = AXIS_LOCK_Y;
			return true;
		}
		return false;
	}

	// Sufficient time has passed to make a new sample
	ShiftSamples();
	m_pSamples[0].X=X; m_pSamples[0].Y=Y; m_dwSamples[0]=dwTime;

	int DiffX = abs(m_pSamples[0].X-m_pSamples[1].X)+abs(m_pSamples[2].X-m_pSamples[1].X);
	int DiffY = abs(m_pSamples[0].Y-m_pSamples[1].Y)+abs(m_pSamples[2].Y-m_pSamples[1].Y);
//g_pPlutoLogger->Write(LV_FESTIVAL,"new sample diff %d,%d at %d,%d last %d,%d",DiffX,DiffY,m_pSamples[0].X,m_pSamples[0].Y,m_pSamples[1].X,m_pSamples[1].Y);

	if( m_cLocked_Axis_Current != AXIS_LOCK_Y )
	{
		if( m_cLockedAxes & AXIS_LOCK_Y && DiffY>MouseSensitivity::MinMovePerSampleToChangeDir && DiffX<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"switch Y %d,%d",DiffX,DiffY);
			m_cLocked_Axis_Current = AXIS_LOCK_Y;
			return true;
		}
	}
	if( m_cLocked_Axis_Current != AXIS_LOCK_X )
	{
		if( m_cLockedAxes & AXIS_LOCK_X && DiffX>MouseSensitivity::MinMovePerSampleToChangeDir && DiffY<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"switch X %d,%d",DiffX,DiffY);
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
	}

	return false;
}

DesignObj_Orbiter *MouseBehavior::FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y)
{
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj_Parent->m_ChildObjects.begin(  ); iHao != pObj_Parent->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
		DesignObj_Orbiter *pObj_Child = FindChildObjectAtPosition(pObj,X,Y);
		if( pObj_Child )
			return pObj_Child;
	}
	if( pObj_Parent->m_bTabStop && (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).Contains(X,Y) )
		return pObj_Parent;
	return NULL;
}

void MouseBehavior::SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection)
{

}

bool MouseBehavior::ButtonDown(int PK_Button)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();
	m_iTime_Last_Mouse_Up=0;

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST  )
	{
		if( m_EMenuOnScreen==mb_MainMenu )
			Clear();
		else
		{
			m_EMenuOnScreen=mb_MainMenu;
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popMainMenu_CONST),0,0,"","left",false,false);
			Set_Mouse_Behavior("Lhu",true,"Y",StringUtils::itos(DESIGNOBJ_popMainMenu_CONST));
		}
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && m_EMenuOnScreen!=mb_MediaControl )
	{
		m_EMenuOnScreen=mb_MediaControl;
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_popSpeedControl_CONST);  // Temp until the widget is done and can set this
		pObj->m_rPosition.Width=964;
		pObj->m_rPosition.Height=90;
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popSpeedControl_CONST),263,526,"","horiz",false,false);
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup),0,0,"","left",false,false);
		Set_Mouse_Behavior("S",false,"X",StringUtils::itos(DESIGNOBJ_popSpeedControl_CONST));
		Set_Mouse_Behavior("T",false,"Y",StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup));
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && m_EMenuOnScreen!=mb_Ambiance )
	{
		m_EMenuOnScreen=mb_Ambiance;
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popVolume_CONST),263,526,"","horiz",false,false);
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST),0,0,"","left",false,false);
		Set_Mouse_Behavior("V",false,"X",StringUtils::itos(DESIGNOBJ_popVolume_CONST));
		Set_Mouse_Behavior("G",false,"Y",StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST));
	}
	else if( (m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && m_EMenuOnScreen==mb_MediaControl && m_cLocked_Axis_Current == AXIS_LOCK_NONE) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && m_EMenuOnScreen==mb_Ambiance && m_cLocked_Axis_Current == AXIS_LOCK_NONE) )
	{
		Clear();
	}

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return false; // Nothing to do
	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
		return m_pMouseHandler_Horizontal->ButtonDown(PK_Button);
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
		return m_pMouseHandler_Vertical->ButtonDown(PK_Button);
	return false;
}

bool MouseBehavior::ButtonUp(int PK_Button)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Up=ProcessUtils::GetMsTime();

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return false; // Nothing to do
	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
		return m_pMouseHandler_Horizontal->ButtonUp(PK_Button);
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
		return m_pMouseHandler_Vertical->ButtonUp(PK_Button);
	return false;
}
