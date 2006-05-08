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

#include "Linux/OSDCompass.h"

using namespace DCE;

#include <SDL_image.h>
#include "SDL/OrbiterSDL.h"
#include "SDL/SDLGraphic.h"
#include "SDL/PlutoSDLDefs.h"

//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	//TODO: set custom position for the compass
	m_spCompass.reset(new OSDCompass(pOrbiter, PlutoRectangle(10, 10, 100, 100)));

	m_pMouseHandler_Horizontal=m_pMouseHandler_Vertical=NULL;
	ProcessUtils::ResetMsTime();
	m_pOrbiter=pOrbiter;
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
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
	m_bMouseHandler_Vertical_Exclusive=m_bMouseHandler_Horizontal_Exclusive=false;
	m_dwTime_Last_Notch=0;
	m_dwLastSampleShift=0;

	if( bGotoMainMenu )
	{
		NeedToRender render( m_pOrbiter, "start speed" );
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
	}
}

// If options contains a 's', this will be selected by default
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
	if( !pObj && sOptions[0] !='K' )  // Only the keyboard handler does not require an object
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior options %s, obj %s invalid",sOptions.c_str(),sDesignObj.c_str());
		return;
	}
	
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
		if( m_bMouseHandler_Vertical_Exclusive || sOptions.find('s')!=string::npos )
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
		if( m_bMouseHandler_Horizontal_Exclusive || sOptions.find('s')!=string::npos )
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
//g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Move %d,%d locked %d current %d vert %p hor %p",
//					  X,Y,(int) m_cLockedAxes,(int) m_cLocked_Axis_Current,m_pMouseHandler_Vertical,m_pMouseHandler_Horizontal);

	unsigned long dwTime = ProcessUtils::GetMsTime();
	m_spCompass->Update(X, Y, dwTime);

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return; // Nothing to do

	//	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Move %d,%d   (%d)",X,Y,m_pOrbiter->m_pObj_Highlighted && m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST ? ((DesignObj_DataGrid *)m_pOrbiter->m_pObj_Highlighted)->m_iHighlightedRow : -999);
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);

	if( m_pLastPosition.X==X && m_pLastPosition.Y==Y )
		return; // Nothing to do 

	if( dwTime-m_dwLastSampleShift>m_MouseSensitivity.SampleInterval )
	{
		m_dwLastSampleShift=dwTime;
		if( abs(m_pSamples[0].X)<m_MouseSensitivity.IgnoreMovesLessThanThisPerSample && abs(m_pSamples[0].Y)<m_MouseSensitivity.IgnoreMovesLessThanThisPerSample )
		{
			bool bCancel=false;
			if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseHandler_Horizontal )
				bCancel=m_pMouseHandler_Horizontal->SlowDrift(X,Y);
			else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseHandler_Vertical )
				bCancel=m_pMouseHandler_Vertical->SlowDrift(X,Y);
			if( bCancel )
			{
				ResetSamples();
				m_pLastPosition.X=X; m_pLastPosition.Y=Y;
				return;
			}
		}
		
		//g_pPlutoLogger->Write(LV_FESTIVAL,"Move ****************SHIFT*************");
		ShiftSamples();
		m_dwSamples[0]=dwTime;
//		SetMousePosition(m_pOrbiter->m_iImageWidth/2,m_pOrbiter->m_iImageHeight/2); // So we don't hit limits too fast

	}

	if( m_pLastPosition.X!=-1 && m_pLastPosition.Y!=-1 )
	{
		m_pSamples[0].X += X - m_pLastPosition.X;
		m_pSamples[0].Y += Y - m_pLastPosition.Y;
	}

	m_pLastPosition.X=X; m_pLastPosition.Y=Y;
	if( abs(m_pSamples[0].X)<m_MouseSensitivity.IgnoreMovesLessThanThisPerSample && abs(m_pSamples[0].Y)<m_MouseSensitivity.IgnoreMovesLessThanThisPerSample )
	{
		/*
g_pPlutoLogger->Write(LV_FESTIVAL,"MOVESXX %d,%d at %d last: %d,%d XX %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d XX %d %d %d %d %d %d %d %d %d %d ",
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
*/
		return;
	}
/*
g_pPlutoLogger->Write(LV_WARNING,"MOVESXX %d,%d at %d last: %d,%d XX %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d XX %d %d %d %d %d %d %d %d %d %d ",
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
*/
//if( CheckForChangeInDirection() )
//g_pPlutoLogger->Write(LV_FESTIVAL,"change!!!!!!!!!");
//return;

	// Now m_pSamples[0] should be the amount of movement cumulative during the past sample period
	char cLocked_Axis_Before = m_cLocked_Axis_Current;
	int PK_Direction=0;
	if( CheckForChangeInDirection(PK_Direction) )
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
		}
		else
		{
			if( m_pObj_Locked_Vertical )
				m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_pPopupPoint.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
			else
				m_iLockedPosition = X;
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
if( (m_cLocked_Axis_Current==AXIS_LOCK_Y && !m_pMouseHandler_Vertical) || (m_cLocked_Axis_Current==AXIS_LOCK_X && !m_pMouseHandler_Horizontal) )
int k=2;

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
			if( m_pObj_Locked_Horizontal && X < m_pObj_Locked_Horizontal->m_rPosition.X+m_pObj_Locked_Horizontal->m_pPopupPoint.X )
			{
				if( !m_pMouseHandler_Horizontal->MovedOutside(DIRECTION_Left_CONST) )
				{
					X = m_pObj_Locked_Horizontal->m_rPosition.X+m_pObj_Locked_Horizontal->m_pPopupPoint.X;
					SetMousePosition(X,Y);
				}
				else
					return;
			}
			if( m_pObj_Locked_Horizontal && X > m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pObj_Locked_Horizontal->m_pPopupPoint.X )
			{
				if( !m_pMouseHandler_Horizontal->MovedOutside(DIRECTION_Right_CONST) )
				{
					X = m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pObj_Locked_Horizontal->m_pPopupPoint.X;
					SetMousePosition(X,Y);
				}
				else
					return;
			}
			m_pMouseHandler_Horizontal->Move(X, Y, PK_Direction);
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
			if( m_pObj_Locked_Vertical && Y < m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
			{
				if( !m_pMouseHandler_Vertical->MovedOutside(DIRECTION_Up_CONST) )
				{
					Y = m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
					SetMousePosition(X,Y);
				}
				else
					return;
			}
			if( m_pObj_Locked_Vertical && Y > m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
			{
				if( !m_pMouseHandler_Vertical->MovedOutside(DIRECTION_Down_CONST) )
				{
					Y = m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
					SetMousePosition(X,Y);
				}
				else
					return;
			}
			m_pMouseHandler_Vertical->Move(X, Y, PK_Direction);
		}
	}
}

int iRound=0;

bool MouseBehavior::CheckForChangeInDirection(int &PK_Direction)
{
	int CumulativeThisDirection=0,CumulativeOtherDirection=0;
	PK_Direction = GetDirection(m_pSamples[0]);
	if( !PK_Direction )
		return false;
	for(int i=0;i<NUM_SAMPLES;++i)
	{
		int PK_Direction_Sample = GetDirection(m_pSamples[i],&CumulativeThisDirection,&CumulativeOtherDirection);
		if( m_dwSamples[0]-m_dwSamples[i]>m_MouseSensitivity.DiscardSamplesOlderThanSec || PK_Direction_Sample!=PK_Direction )
		{
			return false;
		}

		int Minimum,Ratio;
		if( i>=m_MouseSensitivity.Threshhold_3_Samples-1 )
		{
			Minimum=m_MouseSensitivity.Threshhold_3_Minimum;
			Ratio=m_MouseSensitivity.Threshhold_3_Ratio;
		}
		else if( i>=m_MouseSensitivity.Threshhold_2_Samples-1 )
		{
			Minimum=m_MouseSensitivity.Threshhold_2_Minimum;
			Ratio=m_MouseSensitivity.Threshhold_2_Ratio;
		}
		else
		{
			Minimum=m_MouseSensitivity.Threshhold_1_Minimum;
			Ratio=m_MouseSensitivity.Threshhold_1_Ratio;
		}

		if( CumulativeThisDirection>=Minimum && (CumulativeOtherDirection==0 || CumulativeThisDirection*100/CumulativeOtherDirection>=Ratio) )
		{
			if( m_pMouseHandler_Horizontal )
				m_pMouseHandler_Horizontal->m_bStartedMovement=true;
			if( m_pMouseHandler_Vertical )
				m_pMouseHandler_Vertical->m_bStartedMovement=true;

			if( (m_cLocked_Axis_Current == AXIS_LOCK_Y && m_bMouseHandler_Vertical_Exclusive) ||
				(m_cLocked_Axis_Current == AXIS_LOCK_X && m_bMouseHandler_Horizontal_Exclusive) )
				return false;

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

	// CIPRIAN 
	// int LengthOfCompassArm=20;  // I'll figure this out laters
	// int X=m_pSamples[0].X;
	// int Y=m_pSamples[0].Y;
	// MouseCompass->ShowCompass(X,Y,LengthOfCompassArm);
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
	int PK_Screen_OnScreen = m_pOrbiter->m_pScreenHistory_Current->PK_Screen();

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && PK_Screen_OnScreen!=SCREEN_tempmnumain2_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing main menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_tempmnumain2_CONST);
		/*
		m_pOrbiter->CMD_Remove_Popup("",""); // Remove all popups
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popMainMenu_CONST),0,0,"","left",false,false);
		Set_Mouse_Behavior("Lhu",true,"Y",StringUtils::itos(DESIGNOBJ_popMainMenu_CONST));
		*/
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && PK_Screen_OnScreen!=m_pOrbiter->m_iPK_Screen_Remote )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing media menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_Remote);
			
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
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && PK_Screen_OnScreen!=SCREEN_tempmnuambiance_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing ambiance menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("", SCREEN_tempmnuambiance_CONST);
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
	else if( (m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_Remote && m_cLocked_Axis_Current == AXIS_LOCK_NONE) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && PK_Screen_OnScreen==SCREEN_tempmnuambiance_CONST && m_cLocked_Axis_Current == AXIS_LOCK_NONE) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && PK_Screen_OnScreen==SCREEN_tempmnumain2_CONST) )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown removing menu");
		Clear(true);
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_2_CONST )
	{
		if( m_pOrbiter->m_iPK_MediaType )
		{
			DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,0,0,"");
			m_pOrbiter->SendCommand(CMD_MH_Stop_Media);
		}
		Clear(true);
	}

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return false; // Nothing to do
	if( m_pMouseHandler_Horizontal && (m_cLocked_Axis_Current==AXIS_LOCK_X || m_pMouseHandler_Vertical==m_pMouseHandler_Horizontal) )
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
	DesignObj_Orbiter *pObj_Previously_Highlighted=m_pOrbiter->m_pObj_Highlighted;

	m_pOrbiter->UnHighlightObject();
	if( pObj_Previously_Highlighted && pObj!=pObj_Previously_Highlighted )
		m_pOrbiter->ExecuteCommandsInList( &pObj_Previously_Highlighted->m_Action_UnhighlightList, pObj_Previously_Highlighted, smHighlight, 0, 0 );
	m_pOrbiter->m_pObj_Highlighted = pObj;
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

PlutoRectangle MouseBehavior::GetHighlighedObjectCoordinates()
{
	if( m_pOrbiter->m_pObj_Highlighted )
	{
		if( m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			PlutoRectangle rect;
			m_pOrbiter->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) m_pOrbiter->m_pObj_Highlighted,rect);

g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::GetHighlighedObjectCoordinates %d,%d - %d,%d",rect.X,rect.Y,rect.Width,rect.Height);
			return rect + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
		}
		else
			return m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
	}
	return PlutoRectangle(0,0,0,0);
}

void MouseBehavior::SelectFirstObject(char cDirection,DesignObj_Orbiter *pObj_Parent)
{
	// Select the first object to highlight if we didn't already and center over it
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted = m_pOrbiter->FindFirstObjectByDirection(cDirection,true,pObj_Parent,NULL);
	if( pObj )
	{
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );

		if( m_cLocked_Axis_Current==AXIS_LOCK_X )
			m_iLockedPosition = rect.Y + rect.Height/2;
		else
			m_iLockedPosition = rect.X + rect.Width/2;
	}
}

int MouseBehavior::GetDirectionAwayFromHighlight(int X,int Y)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better

	PlutoRectangle plutoRectangle = GetHighlighedObjectCoordinates();
	if( plutoRectangle.Contains(X,Y) )
		return 0;  // We're still inside the object

	// If the highlighted object is a datagrid and we're totally outside the datagrid, then use the datagrid
	// as a whole for determining which side we're on, and not the individual cell, otherwise we'll always
	// think we're below a datagrid without a lot of cells even if we're to the left or right
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && !(pObj->m_rPosition+pObj->m_pPopupPoint).Contains(X,Y) )
		plutoRectangle = pObj->m_rPosition + pObj->m_pPopupPoint;

//	g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::GetDirectionAwayFromHighlight object %s at %d,%d w:%d h:%d doesn't contain %d,%d",
//		pObj->m_ObjectID.c_str(),plutoRectangle.X,plutoRectangle.Y,
//		plutoRectangle.Width,plutoRectangle.Height,X,Y);
	int FromLeft = plutoRectangle.X - X;
	int FromTop = plutoRectangle.Y - Y;
	int FromRight = X - plutoRectangle.Right();
	int FromBottom = Y - plutoRectangle.Bottom();

	if( FromLeft>0 && FromLeft>=FromTop && FromLeft>=FromBottom )
		return DIRECTION_Left_CONST;
	if( FromTop>0 && FromTop>=FromLeft && FromTop>=FromRight )
		return DIRECTION_Up_CONST;
	if( FromRight>0 && FromRight>=FromTop && FromRight>=FromBottom )
		return DIRECTION_Right_CONST;
	if( FromBottom>0 && FromBottom>=FromLeft && FromBottom>=FromRight )
		return DIRECTION_Down_CONST;

	return 0; // shouldn't happen
}

void MouseBehavior::PositionMouseAtObjectEdge(int PK_Direction)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better
	PlutoRectangle rect = GetHighlighedObjectCoordinates();

	switch(PK_Direction)
	{
	case DIRECTION_Left_CONST:
		SetMousePosition( rect.Right(), 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Up_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Bottom());
		return;
	case DIRECTION_Right_CONST:
		SetMousePosition( rect.X, 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Down_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Y);
		return;
	};
}

const MouseHandler* MouseBehavior::GetHorizontalMouseHandler() const
{
	return m_pMouseHandler_Horizontal;
}

const MouseHandler* MouseBehavior::GetVerticalMouseHandler() const
{
	return m_pMouseHandler_Vertical;
}

void MouseSensitivity::SetSensitivity(int Level)
{
	switch(Level)
	{
	case 2:
		SampleInterval = 350;
		DiscardSamplesOlderThanSec = 2250; // 1500 ms
		Threshhold_1_Minimum=70;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=100;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=4; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=160;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=20;
		NumNotchesForExternalApp=12;
		break;
	case 3:
		SampleInterval = 500;
		DiscardSamplesOlderThanSec = 3000; // 1500 ms
		Threshhold_1_Minimum=80;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=120;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=3; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=200;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=30;
		NumNotchesForExternalApp=5;
		break;
	case 1:
	default:
		SampleInterval = 200; //250; // 250 ms
		DiscardSamplesOlderThanSec = 1500; // 1500 ms
		Threshhold_1_Minimum=60;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=80;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=5; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=80;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=10;
		IgnoreMovesLessThanThisPerSample=10;//30;
		NumNotchesForExternalApp=20;
		break;
	}
}

MouseHandler::MouseHandler(DesignObj_Orbiter *pObj, string sOptions, MouseBehavior *pMouseBehavior)
{
    m_bStartedMovement=false;
    m_pObj=pObj;
    m_sOptions=sOptions;
    m_pMouseBehavior=pMouseBehavior;
    m_bLockAxis=true;
    m_bIsActive=false;
    m_pPrevSurface = NULL;
    m_pPrevRect = NULL;
}

MouseHandler::~MouseHandler()
{
    if (m_pPrevSurface)
        SDL_FreeSurface(m_pPrevSurface);
    if (m_pPrevRect)
        delete m_pPrevRect;
}

void MouseHandler::PrevSurfaceRestore()
{
    //TODO: proper restore that surface elsewhere, where it is not restored correctly
    //TODO: and remove this function
    OrbiterSDL *pOrbiterSDL = dynamic_cast<OrbiterSDL *>(m_pMouseBehavior->m_pOrbiter);
    if (pOrbiterSDL == NULL)
        return;
    if (m_pPrevSurface == NULL)
    {
        // remember the original surface
        m_pPrevRect = new SDL_Rect;
        m_pPrevRect->x = m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;
        m_pPrevRect->y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y;
        m_pPrevRect->w = m_pObj->m_rPosition.Width;
        m_pPrevRect->h = m_pObj->m_rPosition.Height;
        m_pPrevSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_pPrevRect->w, m_pPrevRect->h, 32, rmask, gmask, bmask, amask);
        if (m_pPrevSurface == NULL)
		{
            delete m_pPrevRect;
            m_pPrevRect = NULL;
			g_pPlutoLogger->Write(LV_WARNING, "MouseBehavior::PrevSurfaceRestore() : SDL_CreateRGBSurface failed! %s",SDL_GetError());
		}
        SDL_BlitSurface(pOrbiterSDL->m_pScreenImage, m_pPrevRect, m_pPrevSurface, NULL);
    }
    // draw the original surface again
    SDL_BlitSurface(m_pPrevSurface, NULL, pOrbiterSDL->m_pScreenImage, m_pPrevRect);
}
