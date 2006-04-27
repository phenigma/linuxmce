#include "MouseBehavior.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "ScreenHistory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"

#include "SpeedMouseHandler.h"
#include "LightMouseHandler.h"
#include "VolumeMouseHandler.h"
#include "MediaMouseHandler.h"
#include "LockedMouseHandler.h"
#include "KeyboardMouseHandler.h"

using namespace DCE;


//-----------------------------------------------------------------------------------------------------
//class MouseSensitivity
const int MouseSensitivity::SampleInterval = 150; //250; // 250 ms
const int MouseSensitivity::DiscardSamplesOlderThanSec = 1500; // 1500 ms

const int MouseSensitivity::Threshhold_1_Minimum=60;  // Minimum number of pixels to move
const int MouseSensitivity::Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
const int MouseSensitivity::Threshhold_2_Minimum=80;  // 20 pixels over 5 samples
const int MouseSensitivity::Threshhold_2_Ratio=300;
const int MouseSensitivity::Threshhold_2_Samples=4; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
const int MouseSensitivity::Threshhold_3_Minimum=80;
const int MouseSensitivity::Threshhold_3_Ratio=200;
const int MouseSensitivity::Threshhold_3_Samples=9;

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
	m_iPK_Button_Mouse_Last=0;
    m_iTime_Last_Mouse_Down=m_iTime_Last_Mouse_Up=0;
	Clear();
}

MouseBehavior::~MouseBehavior()
{
	delete m_pMouseGovernor;
	delete m_pMouseIterator;
}

void MouseBehavior::Clear(bool bGotoMainMenu)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_EMenuOnScreen!=mb_None )
	{
//		m_pOrbiter->CMD_Remove_Popup("","left");
//		m_pOrbiter->CMD_Remove_Popup("","horiz");
	}

	/* there's a logic flaw that when popups are put on screen they're added to the following
	vect's, but not removed when the popup is removed, although they are marked as m_bOnScreen=false.
	These were created as vect's because the concept of popups didn't exist in the beginning.
	Perhaps they should be changed to something else that's easier to remove entries from.
	For now, with the new UI, we know that whenever the mouse behavior is reset we will
	have no grids or tab stops on screen.  Reset them so the list doesn't grow indefinately. */
//	m_pOrbiter->m_vectObjs_GridsOnScreen.clear();
//	m_pOrbiter->m_vectObjs_TabStops.clear();

	m_pObj_Locked_Vertical=m_pObj_Locked_Horizontal=NULL;
	ResetSamples();
	m_pLastPosition.X=m_pLastPosition.Y=-1;
	m_cLockedAxes = m_cLocked_Axis_Current = AXIS_LOCK_NONE;
	if( m_pMouseHandler_Vertical && m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
		m_pMouseHandler_Vertical=NULL; // Only delete it once
	delete m_pMouseHandler_Horizontal;
	delete m_pMouseHandler_Vertical;
	m_pMouseHandler_Horizontal=m_pMouseHandler_Vertical=NULL;
	m_EMenuOnScreen=mb_None;
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
	m_bMouseHandler_Vertical_Exclusive=m_bMouseHandler_Horizontal_Exclusive=false;

	if( bGotoMainMenu )
	{
		NeedToRender render( m_pOrbiter, "start speed" );
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
	}
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
if( sOptions=="LS" )
int k=2;
	DesignObj_Orbiter *pObj=NULL;
	if( sDesignObj.size() )
		pObj = m_pOrbiter->FindObject(sDesignObj);

	g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior object found %p,"
		" obj id %s ", pObj, sDesignObj.c_str());
		
	MouseHandler *pMouseHandler=NULL;
	if( !pObj && sOptions[0] !='K' )  // Only the keyboard handler does not require an object
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior options %s, obj %s invalid",sOptions.c_str(),sDesignObj.c_str());
		return;
	}
	
	g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior options %s", sOptions.c_str());

	switch(sOptions[0])
	{
	case 'L':
	case 'M':
		pMouseHandler=new LockedMouseHandler(pObj,sOptions,this);
		break;
	case 'S':
		pMouseHandler=new SpeedMouseHandler(pObj,sOptions,this);
		break;
	case 'G':
		pMouseHandler=new LightMouseHandler(pObj,sOptions,this);
		break;
	case 'V':
		pMouseHandler=new VolumeMouseHandler(pObj,sOptions,this);
		break;
	case 'T':
		pMouseHandler=new MediaMouseHandler(pObj,sOptions,this);
		break;
	case 'K':
		pMouseHandler=new KeyboardMouseHandler(pObj,sOptions,this);
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
		m_bMouseHandler_Vertical_Exclusive=bExclusive;
		m_sVerticalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_X || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_Y);
		if( m_bMouseHandler_Vertical_Exclusive )
		{
			m_pMouseHandler_Vertical->Start();
			m_cLocked_Axis_Current=AXIS_LOCK_Y;
			m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_pPopupPoint.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
		}
		else if( m_cLocked_Axis_Current==AXIS_LOCK_Y )
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
		m_bMouseHandler_Horizontal_Exclusive=bExclusive;
		m_sHorizontalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_Y || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_X);
		if( m_bMouseHandler_Horizontal_Exclusive )
		{
			m_pMouseHandler_Horizontal->Start();
			m_cLocked_Axis_Current=AXIS_LOCK_X;
			m_iLockedPosition = m_pObj_Locked_Horizontal->m_rPosition.Y + m_pObj_Locked_Horizontal->m_pPopupPoint.Y + m_pObj_Locked_Horizontal->m_rPosition.Height/2;
		}
		else if( m_cLocked_Axis_Current==AXIS_LOCK_X )
			m_cLocked_Axis_Current=AXIS_LOCK_NONE;
	}
}


void MouseBehavior::Move(int X,int Y)
{
	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return; // Nothing to do

	//	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Move %d,%d   (%d)",X,Y,m_pOrbiter->m_pObj_Highlighted && m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST ? ((DesignObj_DataGrid *)m_pOrbiter->m_pObj_Highlighted)->m_iHighlightedRow : -999);
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);

	unsigned long dwTime = ProcessUtils::GetMsTime();
	if( dwTime-m_dwSamples[0]>MouseSensitivity::SampleInterval )
	{
//g_pPlutoLogger->Write(LV_FESTIVAL,"Move ****************SHIFT*************");
		ShiftSamples();
		m_dwSamples[0]=dwTime;
	}


	if( m_pLastPosition.X==X && m_pLastPosition.Y==Y )
		return; // Nothing to do 
	if( m_pLastPosition.X!=-1 && m_pLastPosition.Y!=-1 )
	{
		m_pSamples[0].X += X - m_pLastPosition.X;
		m_pSamples[0].Y += Y - m_pLastPosition.Y;
	}
	
g_pPlutoLogger->Write(LV_FESTIVAL,"Move %d,%d at %d last: %d,%d XX %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d XX %d %d %d %d %d %d %d %d %d %d ",
					  X,Y,dwTime,m_pLastPosition.X,m_pLastPosition.Y,
					  m_pSamples[0].X,m_pSamples[0].Y,
					  m_pSamples[1].X,m_pSamples[1].Y,
					  m_pSamples[2].X,m_pSamples[2].Y,
					  m_pSamples[3].X,m_pSamples[3].Y,
					  m_pSamples[4].X,m_pSamples[4].Y,
					  m_pSamples[5].X,m_pSamples[5].Y,
					  m_pSamples[6].X,m_pSamples[6].Y,
					  m_pSamples[7].X,m_pSamples[7].Y,
					  m_pSamples[8].X,m_pSamples[8].Y,
					  m_pSamples[9].X,m_pSamples[9].Y,
m_dwSamples[0],m_dwSamples[1],m_dwSamples[2],m_dwSamples[3],m_dwSamples[4],m_dwSamples[5],m_dwSamples[6],m_dwSamples[7],m_dwSamples[8],m_dwSamples[9]);
	m_pLastPosition.X=X; m_pLastPosition.Y=Y;

//if( CheckForChangeInDirection() )
//g_pPlutoLogger->Write(LV_FESTIVAL,"change!!!!!!!!!");
//return;

	// Now m_pSamples[0] should be the amount of movement cumulative during the past sample period
	char cLocked_Axis_Before = m_cLocked_Axis_Current;
	if( CheckForChangeInDirection() )
	{
	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Move Direction changed");
		m_pStartMovement.X=X;
		m_pStartMovement.Y=Y;

		if( m_cLocked_Axis_Current == AXIS_LOCK_X )
		{
			if( m_pObj_Locked_Horizontal )
				m_iLockedPosition = m_pObj_Locked_Horizontal->m_rPosition.Y + m_pObj_Locked_Horizontal->m_pPopupPoint.Y + m_pObj_Locked_Horizontal->m_rPosition.Height/2;
			else
				m_iLockedPosition = Y;
g_pPlutoLogger->Write(LV_CRITICAL,"Move Direction now X, y locked to %d",m_iLockedPosition);
		}
		else
		{
			if( m_pObj_Locked_Vertical )
				m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_pPopupPoint.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
			else
				m_iLockedPosition = X;
g_pPlutoLogger->Write(LV_CRITICAL,"Move Direction now Y, x locked to %d",m_iLockedPosition);
		}
		m_pMouseGovernor->SetBuffer(0);
		if( cLocked_Axis_Before==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
		{
			m_pMouseHandler_Horizontal->m_bIsActive=false;
			m_pMouseHandler_Horizontal->Stop();
		}
		else if( cLocked_Axis_Before==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
		{
			m_pMouseHandler_Vertical->m_bIsActive=false;
			m_pMouseHandler_Vertical->Stop();
		}

		if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
		{
			m_pMouseHandler_Horizontal->m_bIsActive=true;
			m_pMouseHandler_Horizontal->Start();
		}
		else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
		{
			m_pMouseHandler_Vertical->m_bIsActive=true;
			m_pMouseHandler_Vertical->Start();
		}
		return;
	}

	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
	{
		if( m_pMouseHandler_Horizontal->m_bLockAxis )
			Y = m_iLockedPosition;
		if( m_pMouseHandler_Horizontal->m_pObj && !m_pMouseHandler_Horizontal->m_pObj->m_bOnScreen )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Vertical=NULL;
			delete m_pMouseHandler_Horizontal;
			m_pMouseHandler_Horizontal=NULL;
		}
		else
		{
			if( X < m_pObj_Locked_Horizontal->m_rPosition.X+m_pObj_Locked_Horizontal->m_pPopupPoint.X )
			{
				X = m_pObj_Locked_Horizontal->m_rPosition.X+m_pObj_Locked_Horizontal->m_pPopupPoint.X;
				SetMousePosition(X,Y);
			}
			if( X > m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pObj_Locked_Horizontal->m_pPopupPoint.X )
			{
				X = m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pObj_Locked_Horizontal->m_pPopupPoint.X;
				SetMousePosition(X,Y);
			}
			m_pMouseHandler_Horizontal->Move(X, Y);
		}
	}
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
	{
		if( m_pMouseHandler_Vertical->m_bLockAxis )
			X = m_iLockedPosition;
		if( m_pMouseHandler_Vertical->m_pObj && !m_pMouseHandler_Vertical->m_pObj->m_bOnScreen )
		{
			if( m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal )
				m_pMouseHandler_Horizontal=NULL;
			delete m_pMouseHandler_Vertical;
			m_pMouseHandler_Vertical=NULL;
		}
		else
		{
			if( Y < m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
			{
				Y = m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
				SetMousePosition(X,Y);
			}
			if( Y > m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
			{
				Y = m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
				SetMousePosition(X,Y);
			}
			m_pMouseHandler_Vertical->Move(X, Y);
		}
	}
}

bool MouseBehavior::CheckForChangeInDirection()
{
	if( (m_cLocked_Axis_Current == AXIS_LOCK_Y && m_bMouseHandler_Vertical_Exclusive) ||
		(m_cLocked_Axis_Current == AXIS_LOCK_X && m_bMouseHandler_Horizontal_Exclusive) )
			return false;
	
	int CumulativeThisDirection=0,CumulativeOtherDirection=0;
	int PK_Direction = GetDirection(m_pSamples[0]);
	if( !PK_Direction )
		return false;
	for(int i=0;i<NUM_SAMPLES;++i)
	{
		int PK_Direction_Sample = GetDirection(m_pSamples[i],&CumulativeThisDirection,&CumulativeOtherDirection);
		if( m_dwSamples[0]-m_dwSamples[i]>MouseSensitivity::DiscardSamplesOlderThanSec )
		{
//			g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::CheckForChangeInDirection Move ignoring sample %d (%d) too old",i,m_dwSamples[i]);
			return false;
		}
		if( PK_Direction_Sample!=PK_Direction )
		{
//			g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::CheckForChangeInDirection Move sample %d is not direction %d it is %d",
//				i,PK_Direction,PK_Direction_Sample);
			return false;
		}
		int Minimum,Ratio;
		if( i>=MouseSensitivity::Threshhold_3_Samples-1 )
		{
			Minimum=MouseSensitivity::Threshhold_3_Minimum;
			Ratio=MouseSensitivity::Threshhold_3_Ratio;
		}
		else if( i>=MouseSensitivity::Threshhold_2_Samples-1 )
		{
			Minimum=MouseSensitivity::Threshhold_2_Minimum;
			Ratio=MouseSensitivity::Threshhold_2_Ratio;
		}
		else
		{
			Minimum=MouseSensitivity::Threshhold_1_Minimum;
			Ratio=MouseSensitivity::Threshhold_1_Ratio;
		}

		if( CumulativeThisDirection>=Minimum && (CumulativeOtherDirection==0 || CumulativeThisDirection*100/CumulativeOtherDirection>=Ratio) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::CheckForChangeInDirection i=%d cumulative this=%d other=%d  min=%d ratio=%d",i,CumulativeThisDirection,CumulativeOtherDirection,Minimum,Ratio);
			if( PK_Direction==DIRECTION_Left_CONST || PK_Direction==DIRECTION_Right_CONST )
			{
				if( m_cLocked_Axis_Current==AXIS_LOCK_X )
					return false;
				m_cLocked_Axis_Current = AXIS_LOCK_X;
			}
			else
			{
				if( m_cLocked_Axis_Current==AXIS_LOCK_Y )
					return false;
				m_cLocked_Axis_Current = AXIS_LOCK_Y;
			}
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
	if( m_pMouseHandler_Horizontal && m_pMouseHandler_Horizontal->TypeOfMouseHandler()==MouseHandler::mh_Speed )
	{
		SpeedMouseHandler *pSpeedMouseHandler = (SpeedMouseHandler *) m_pMouseHandler_Horizontal;
		int Stop = pSpeedMouseHandler->ParseTime(sTotal);
		int Current = pSpeedMouseHandler->ParseTime(sTime);
		if( Stop==pSpeedMouseHandler->m_CurrentMedia_Stop && Current==pSpeedMouseHandler->m_CurrentMedia_Pos )
			return;
		pSpeedMouseHandler->m_CurrentMedia_Stop = Stop;
		pSpeedMouseHandler->m_CurrentMedia_Pos = Current;
NeedToRender render( m_pOrbiter, "start speed" );
m_pOrbiter->RenderObjectAsync(pSpeedMouseHandler->m_pObj);

		pSpeedMouseHandler->Update();
	}
}

bool MouseBehavior::ButtonDown(int PK_Button)
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown %d",(int) PK_Button);
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();
	m_iTime_Last_Mouse_Up=0;

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && m_EMenuOnScreen!=mb_MainMenu )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing main menu");
		m_EMenuOnScreen=mb_MainMenu;
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_tempmnumain2_CONST);
		/*
		m_pOrbiter->CMD_Remove_Popup("",""); // Remove all popups
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popMainMenu_CONST),0,0,"","left",false,false);
		Set_Mouse_Behavior("Lhu",true,"Y",StringUtils::itos(DESIGNOBJ_popMainMenu_CONST));
		*/
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && m_EMenuOnScreen!=mb_MediaControl )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing media menu");
		m_EMenuOnScreen=mb_MediaControl;
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_tempmnuspeed_CONST);
/*
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_popSpeedControl_temp_CONST);  // Temp until the widget is done and can set this
		pObj->m_rPosition.Width=964;
		pObj->m_rPosition.Height=90;
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popSpeedControl_temp_CONST),263,526,"","horiz",false,false);
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup),0,0,"","left",false,false);
		Set_Mouse_Behavior("S",false,"X",StringUtils::itos(DESIGNOBJ_popSpeedControl_temp_CONST));
		Set_Mouse_Behavior("T",false,"Y",StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup));
*/
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && m_EMenuOnScreen!=mb_Ambiance )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing ambiance menu");
		m_EMenuOnScreen=mb_Ambiance;
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_tempmnuambiance_CONST);
/*
		DesignObj_Orbiter *pObj_Lights = m_pOrbiter->FindObject(DESIGNOBJ_popLightsInRoom_CONST);
		DesignObj_Orbiter *pObj_Volume = m_pOrbiter->FindObject(DESIGNOBJ_popVolume_CONST);
		if( pObj_Lights && pObj_Volume )
		{
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popVolume_CONST),pObj_Lights->m_rPosition.Width + m_pOrbiter->m_Width/20,m_pOrbiter->m_Height*.95 - pObj_Volume->m_rPosition.Height,"","horiz",false,false);
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST),m_pOrbiter->m_Width/20,0,"","left",false,false);
			Set_Mouse_Behavior("V",false,"X",StringUtils::itos(DESIGNOBJ_popVolume_CONST));
			Set_Mouse_Behavior("G",false,"Y",StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST));
		}
*/
	}
	else if( (m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && m_EMenuOnScreen==mb_MediaControl && m_cLocked_Axis_Current == AXIS_LOCK_NONE) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && m_EMenuOnScreen==mb_Ambiance && m_cLocked_Axis_Current == AXIS_LOCK_NONE) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && m_EMenuOnScreen==mb_MainMenu) )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown removing menu");
		Clear(true);
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_2_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"Canel button on menu %d",(int) m_EMenuOnScreen);
		if( m_EMenuOnScreen==mb_None )
		{
			DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,0,0,"");
			m_pOrbiter->SendCommand(CMD_MH_Stop_Media);
		}
		Clear(true);
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

void MouseBehavior::HighlightObject(DesignObj_Orbiter *pObj)
{
	m_pOrbiter->UnHighlightObject();
	if( pObj )
		m_pOrbiter->ExecuteCommandsInList( &pObj->m_Action_UnhighlightList, pObj, smHighlight, 0, 0 );
	m_pOrbiter->DoHighlightObject();
//			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
}

int MouseBehavior::GetDirection(PlutoPoint &pt,int *CumulativeThisDirection,int *CumulativeOtherDirection)
{
	if( abs(pt.X)>abs(pt.Y) )
	{
		if( pt.X>0 )
		{
			if( CumulativeThisDirection )
				*CumulativeThisDirection += pt.X;
			if( CumulativeOtherDirection )
				*CumulativeOtherDirection += abs(pt.Y);
			return DIRECTION_Right_CONST;
		}
		else
		{
			if( CumulativeThisDirection )
				*CumulativeThisDirection += pt.X*-1;
			if( CumulativeOtherDirection )
				*CumulativeOtherDirection += abs(pt.Y);
			return DIRECTION_Left_CONST;
		}
	}
	else if( pt.X==pt.Y )
		return 0;
	else
	{
		if( pt.Y>0 )
		{
			if( CumulativeThisDirection )
				*CumulativeThisDirection += pt.Y;
			if( CumulativeOtherDirection )
				*CumulativeOtherDirection += abs(pt.X);
			return DIRECTION_Down_CONST;
		}
		else
		{
			if( CumulativeThisDirection )
				*CumulativeThisDirection += pt.Y*-1;
			if( CumulativeOtherDirection )
				*CumulativeOtherDirection += abs(pt.X);
			return DIRECTION_Up_CONST;
		}
	}
}

