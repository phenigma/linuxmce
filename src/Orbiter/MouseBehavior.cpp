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
#include "DataGrid.h"

using namespace DCE;

#define AXIS_LOCK_NONE	0
#define AXIS_LOCK_X		1
#define AXIS_LOCK_Y		2
#define AXIS_LOCK_BOTH	3

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
const int MouseBehavior::m_iSpeeds[] = {0,250,500,1000,2000,3000,4000,6000,8000,10000,15000,20000,30000,50000,100000,200000,400000};

//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	ProcessUtils::ResetMsTime();
	m_pOrbiter=pOrbiter;
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
	m_pObj_Locked_Vertical=m_pObj_Locked_Horizontal=NULL;
	ResetSamples();
	m_cLockedAxes = m_cLocked_Axis_Current = AXIS_LOCK_NONE;
	m_pMouseBehaviorHandler_Horizontal=m_pMouseBehaviorHandler_Vertical=NULL;
	m_iPK_Button_Mouse_Last=0;
    m_iTime_Last_Mouse_Down=m_iTime_Last_Mouse_Up=0;
	m_EMenuOnScreen=mb_None;
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0);
}

void MouseBehavior::Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj)
{
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

	MouseBehaviorHandler pMouseBehaviorHandler=NULL;
	switch(sOptions[0])
	{
	case 'L':
		pMouseBehaviorHandler=&MouseBehavior::LockedBar;
		break;
	case 'S':
		pMouseBehaviorHandler=&MouseBehavior::SpeedControl;
		break;
	case 'G':
		pMouseBehaviorHandler=&MouseBehavior::LightControl;
		break;
	case 'V':
		pMouseBehaviorHandler=&MouseBehavior::VolumeControl;
		break;
	case 'T':
		pMouseBehaviorHandler=&MouseBehavior::MediaTracks;
		break;
	}

	if( sDirection[0]=='Y' )
	{
		m_pObj_Locked_Vertical=pObj;
		m_pMouseBehaviorHandler_Vertical=pMouseBehaviorHandler;
		m_sVerticalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_X || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_Y);
	}
	else if( sDirection[0]=='X' )
	{
		m_pObj_Locked_Horizontal=pObj;
		m_pMouseBehaviorHandler_Horizontal=pMouseBehaviorHandler;
		m_sHorizontalOptions=sOptions;
		m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_Y || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_X);
	}
	else
		m_cLockedAxes = AXIS_LOCK_NONE;

	// set a call back function for each axis
	m_pMouseGovernor->SetBuffer(0);
	m_cLocked_Axis_Current = AXIS_LOCK_NONE;
	m_dwSamples[0]=ProcessUtils::GetMsTime();
	GetMousePosition(&m_pSamples[0]);

	if( pMouseBehaviorHandler )
	{
		CALL_MEMBER_FN(*this, pMouseBehaviorHandler)(mb_SettingUp, pObj, 0, 0, 0);
	}
}

bool MouseBehavior::LockedBar( EMouseBehaviorEvent eMouseBehaviorEvent ,DesignObj_Orbiter *pObj, int Parm,int X, int Y )
{
	if( eMouseBehaviorEvent==mb_SettingUp )
	{
		if( m_cLockedAxes==AXIS_LOCK_Y )
		{
			DesignObj_Orbiter *pObj_First = m_pOrbiter->FindFirstObjectByDirection('u',false,pObj,NULL);
			if( pObj_First )
			{
				m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
				m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
				SetMousePosition(m_pStartMovement.X,m_pStartMovement.Y);
				PLUTO_SAFETY_LOCK( cm, m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
				if( m_pOrbiter->m_pObj_Highlighted && m_pOrbiter->m_pObj_Highlighted!=pObj_First )
					m_pOrbiter->ExecuteCommandsInList( &m_pOrbiter->m_pObj_Highlighted->m_Action_UnhighlightList, m_pOrbiter->m_pObj_Highlighted, smHighlight, 0, 0 );
				m_pOrbiter->m_pObj_Highlighted=pObj_First;
				m_pOrbiter->RenderObjectAsync(m_pOrbiter->m_pObj_Highlighted);
			}
		}
	}
	else if( eMouseBehaviorEvent==mb_StartMove || eMouseBehaviorEvent==mb_ChangeDirection || eMouseBehaviorEvent==mb_Movement )
	{
		LockedMove( X, Y );
	}
	else if( eMouseBehaviorEvent==mb_MouseDown && Parm==BUTTON_Mouse_1_CONST && m_pOrbiter->m_pObj_Highlighted )
	{
		m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");
		return true; // We handled this, do nothing more
	}
	return false;
}

bool MouseBehavior::MediaTracks( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y )
{
	if( !pObj || pObj->m_ChildObjects.size()==0 )
		return false; // Shouldn't happen, this should be the volume control

	DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(pObj->m_ChildObjects.begin());
	PLUTO_SAFETY_LOCK( cm, m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pOrbiter->m_DatagridMutex );
	if( pObj_Child->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return false; // Also shouldn't happen
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj_Child;
	if( !pObj_Grid->m_pDataGridTable )
		return false; // Again shouldn't happen

	int Rows = pObj_Grid->m_pDataGridTable->GetRows();
	if( eMouseBehaviorEvent==mb_StartMove || eMouseBehaviorEvent==mb_ChangeDirection )
	{
		m_pMouseGovernor->SetBuffer(2000);

		if( m_iTime_Last_Mouse_Up )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::MediaTracks  *discrete* %d %p %d,%d",
					  (int) eMouseBehaviorEvent,pObj,X,Y);
			m_bUsingDiscreteMode=true;
			int CurrentRow = pObj_Grid->m_iHighlightedRow>0 ? pObj_Grid->m_iHighlightedRow : 0;
			double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
			int Y = CurrentRow * RowHeight + pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y;
			int X = pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2);
			SetMousePosition(X,Y);
		}
		else
		{
			m_bUsingDiscreteMode=false;
		}
	}
	else if( eMouseBehaviorEvent==mb_Movement )
	{
		if( m_bUsingDiscreteMode )
		{
			double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
			int YDiff = Y - pObj_Grid->m_rPosition.Y - pObj_Grid->m_pPopupPoint.Y;
			int Row = YDiff / RowHeight;
			if( Row>Rows-1 )
				Row=Rows-1;
			pObj_Grid->m_iHighlightedRow = 1;
			pObj_Grid->m_GridCurRow = Row;
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::MediaTracks  *discrete* highlighted row %d",Row);
			delete pObj_Grid->m_pDataGridTable;
			pObj_Grid->m_pDataGridTable=NULL;
			pObj_Grid->bReAcquire=true;
			NeedToRender render( m_pOrbiter, "MOUSE BEHAVIOR SCROLL" );
			m_pOrbiter->m_pObj_Highlighted=pObj_Grid;
			m_pOrbiter->RenderObjectAsync(pObj_Grid);
			m_pOrbiter->SelectedObject(pObj_Grid,smMouseGovernor);
		}
	}
	else if( eMouseBehaviorEvent==mb_MouseDown && Parm==BUTTON_Mouse_1_CONST && m_pOrbiter->m_pObj_Highlighted )
	{
		m_pOrbiter->SelectedObject(pObj_Grid,smMouseGovernor);
		m_pOrbiter->CMD_Remove_Popup("","left");
		m_pOrbiter->CMD_Remove_Popup("","horiz");
		Set_Mouse_Behavior("",false,"","");
		m_EMenuOnScreen=mb_None;

		return true; // We handled this, do nothing more
		/*
		DataGridCell *pCell = pObj_Grid->m_pDataGridTable->GetData(
			pObj_Grid->m_iHighlightedColumn!=-1 ? pObj_Grid->m_iHighlightedColumn + pObj_Grid->m_GridCurCol : pObj_Grid->m_GridCurCol,
			pObj_Grid->m_iHighlightedRow!=-1 ? pObj_Grid->m_iHighlightedRow + pObj_Grid->m_GridCurRow - (pObj_Grid->HasMoreUp() ? 1 : 0) : 0);
		if(pCell)
		{
			m_pOrbiter->SelectedGrid(pObj_Grid, pCell);
			return true; // We handled this, do nothing more
		}
		*/
	}
	return false;
}

bool MouseBehavior::SpeedControl( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y )
{
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::SpeedControl %d %p %d,%d",
					  (int) eMouseBehaviorEvent,pObj,X,Y);
	if( !pObj )
		return false; // Shouldn't happen, this should be the volume control
	if( eMouseBehaviorEvent==mb_StartMove || eMouseBehaviorEvent==mb_ChangeDirection )
	{
		m_pMouseGovernor->SetBuffer(2000);
		g_pPlutoLogger->Write(LV_FESTIVAL,"Starting speed control");
		m_iLastSpeed=0;
		if( m_bUsingDiscreteMode )
		{
			int Percentage = (m_CurrentMedia_Stop-m_CurrentMedia_Start)/m_CurrentMedia_Pos;
			int X = (m_pObj_Locked_Horizontal->m_rPosition.Width * Percentage / 100) + m_pObj_Locked_Horizontal->m_rPosition.X + m_pObj_Locked_Horizontal->m_pPopupPoint.X;
			int Y = m_pObj_Locked_Horizontal->m_rPosition.Y + m_pObj_Locked_Horizontal->m_pPopupPoint.Y + (m_pObj_Locked_Horizontal->m_rPosition.Height/2);
			SetMousePosition(X,Y);
// remus -- we are using absolute seeking
		}
		else
			SetMousePosition(pObj->m_rPosition.X+pObj->m_pPopupPoint.X+pObj->m_rPosition.Width/2,pObj->m_rPosition.Y+pObj->m_pPopupPoint.Y+pObj->m_rPosition.Height/2);
			// remus -- we are using relative seeking
	}
	else if( eMouseBehaviorEvent==mb_Movement )
	{
		int XStart = pObj->m_rPosition.X+pObj->m_pPopupPoint.X+pObj->m_rPosition.Width/2;
		if( m_iTime_Last_Mouse_Up==0 || !m_bHasTimeline )  // holding button down, change speed, or this stream doesn't support absolute positioning anyway
		{
			int NotchWidth = pObj->m_rPosition.Width/2/MAX_SPEEDS;
			int Notch = abs(X-XStart) / NotchWidth;
			if( Notch>MAX_SPEEDS-1 )
				Notch = MAX_SPEEDS-1;
			int Speed = m_iSpeeds[Notch];
			if( X<XStart )
				Speed *= -1;
			if( Speed!=m_iLastSpeed )
			{
	g_pPlutoLogger->Write(LV_FESTIVAL,"speed %d",Speed);
				DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,0,Speed);
				m_pMouseGovernor->SendMessage(CMD_Change_Playback_Speed.m_pMessage);
				m_iLastSpeed=Speed;
//remus  pObj->SetSpeed(Speed);
			}
		}
		else  // tap and release, go to position
		{
			int Offset = X - pObj->m_rPosition.X-pObj->m_pPopupPoint.X;
			double Perc = (double) Offset / pObj->m_rPosition.Width;
			int Time = (m_CurrentMedia_Stop-m_CurrentMedia_Start * Perc) + m_CurrentMedia_Start;
			DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,0,"POS:" + StringUtils::itos(Time*1000));
			m_pMouseGovernor->SendMessage(CMD_Set_Media_Position.m_pMessage);
//remus  pObj->SetSpeed(Speed);
		}
	}
	return false;
}

bool MouseBehavior::LightControl( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y )
{
	if( !pObj )
		return false; // Shouldn't happen, this should be the volume control
	if( eMouseBehaviorEvent==mb_StartMove || eMouseBehaviorEvent==mb_ChangeDirection )
	{
		m_pMouseGovernor->SetBuffer(2000);
		m_iLastNotch=0;
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::LightControl starting %d %p %d,%d",
					  (int) eMouseBehaviorEvent,pObj,X,Y);
		SetMousePosition(pObj->m_rPosition.X+pObj->m_pPopupPoint.X+pObj->m_rPosition.Width/2,pObj->m_rPosition.Y+pObj->m_pPopupPoint.Y+pObj->m_rPosition.Height/2);
	}
	else if( eMouseBehaviorEvent==mb_Movement )
	{
		// 22 notches, -100, -90, -0, +0, +10 ... +100
		int YStart = pObj->m_rPosition.Y+pObj->m_pPopupPoint.Y+pObj->m_rPosition.Height/2;
		int NotchHeight = pObj->m_rPosition.Height/22;
		int Notch = (YStart-Y) / NotchHeight;
		if( Notch>11 )
			Notch = 11;
		if( Notch!=m_iLastNotch )
		{
			string sLevel = (Notch>m_iLastNotch ? "+" : "") + StringUtils::itos((Notch-m_iLastNotch)*10);
			DCE::CMD_Set_Level CMD_Set_Level(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_LightingPlugIn,sLevel);
			m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
			g_pPlutoLogger->Write(LV_FESTIVAL,"Lights now at %d (st %d now %d)",Notch,YStart,Y);
			m_iLastNotch=Notch;
			
		}
	}
	return false;
}

bool MouseBehavior::VolumeControl( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y )
{
	if( !pObj )
		return false; // Shouldn't happen, this should be the volume control
	if( eMouseBehaviorEvent==mb_StartMove || eMouseBehaviorEvent==mb_ChangeDirection )
	{
		if( m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume && m_iTime_Last_Mouse_Up==0 )
		{
			m_pMouseGovernor->SetBuffer(2000);
			m_bUsingDiscreteMode=true;
			m_iLastNotch = atoi(m_pOrbiter->GetEvents()->GetCurrentDeviceData(m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,DEVICEDATA_Volume_Level_CONST).c_str());
			int X = pObj->m_rPosition.Width * m_iLastNotch / 100;
g_pPlutoLogger->Write(LV_FESTIVAL,"Starting discrete volume control notch: %d  X %d",m_iLastNotch,X);
			SetMousePosition(pObj->m_rPosition.X+pObj->m_pPopupPoint.X+X,pObj->m_rPosition.Y+pObj->m_pPopupPoint.Y+pObj->m_rPosition.Height/2);
		}
		else
		{
			m_bUsingDiscreteMode=false;
			SetMousePosition(pObj->m_rPosition.X+pObj->m_pPopupPoint.X+pObj->m_rPosition.Width/2,pObj->m_rPosition.Y+pObj->m_pPopupPoint.Y+pObj->m_rPosition.Height/2);
			m_iLastNotch = 0;
		}
	}
	else if( eMouseBehaviorEvent==mb_Movement )
	{
		if( m_bUsingDiscreteMode )
		{
			int Notch = (X - pObj->m_rPosition.X - pObj->m_pPopupPoint.X) * 100 / pObj->m_rPosition.Width;
			if( Notch!=m_iLastNotch )
			{
g_pPlutoLogger->Write(LV_FESTIVAL,"Setting volume to : %d  X %d",Notch,X);
				DCE::CMD_Set_Volume CMD_Set_Volume(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(Notch));
				m_pMouseGovernor->SendMessage(CMD_Set_Volume.m_pMessage);
				m_iLastNotch = Notch;
			}
		}
		else
		{
			int XStart = pObj->m_rPosition.X+pObj->m_pPopupPoint.X+pObj->m_rPosition.Width/2;
			if( m_iTime_Last_Mouse_Up==0 || !m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume )  // holding button down, change speed, or this stream doesn't support absolute positioning anyway
			{
				int NotchWidth = pObj->m_rPosition.Width/14; // Allow for 7 repeat levels in each direction
				int Notch = (X-XStart) / NotchWidth;
				if( Notch!=m_iLastNotch )
				{
g_pPlutoLogger->Write(LV_FESTIVAL,"Setting volume iterator to : %d  X %d",Notch,X);
					m_pMouseIterator->SetIterator(MouseIterator::if_Volume,Notch,500);
					m_iLastNotch = Notch;
				}
			}
		}
	}
	return false;
}

void MouseBehavior::Move(int X,int Y)
{
	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return; // Nothing to do

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
		}
		else
		{
			if( m_pObj_Locked_Vertical )
				m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_pPopupPoint.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
			else
				m_iLockedPosition = X;
		}
		m_pMouseGovernor->SetBuffer(0);
		if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseBehaviorHandler_Horizontal )
			CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Horizontal)(mb_ChangeDirection, m_pObj_Locked_Horizontal, 0, X, Y);
		else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseBehaviorHandler_Vertical )
			CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Vertical)(mb_ChangeDirection, m_pObj_Locked_Vertical, 0, X, Y);
		return;
	}

	if( m_cLocked_Axis_Current == AXIS_LOCK_X )
		Y = m_iLockedPosition;
	if( m_cLocked_Axis_Current == AXIS_LOCK_Y )
		X = m_iLockedPosition;

	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseBehaviorHandler_Horizontal )
		CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Horizontal)(mb_Movement, m_pObj_Locked_Horizontal, 0, X, Y);
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseBehaviorHandler_Vertical )
		CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Vertical)(mb_Movement, m_pObj_Locked_Vertical, 0, X, Y);
}

void MouseBehavior::LockedMove( int X, int Y )
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"Move %d,%d last %d,%d start %d,%d locked axis: %d current %d pos: %d  h:%p v:%p",
X,Y,m_pSamples[0].X,m_pSamples[0].Y,m_pStartMovement.X,m_pStartMovement.Y,(int) m_cLockedAxes,(int) m_cLocked_Axis_Current,(int) m_iLockedPosition,
m_pObj_Locked_Horizontal,m_pObj_Locked_Vertical);

	if( m_cLocked_Axis_Current == AXIS_LOCK_X && m_pObj_Locked_Horizontal )
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
	}
	else if( m_cLocked_Axis_Current == AXIS_LOCK_Y && m_pObj_Locked_Vertical )
	{
		if( Y < m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
		{
			X = m_pObj_Locked_Vertical->m_rPosition.Y+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
			SetMousePosition(X,Y);
		}
		if( X > m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y )
		{
			X = m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pObj_Locked_Vertical->m_pPopupPoint.Y;
			SetMousePosition(X,Y);
		}
	}

	PLUTO_SAFETY_LOCK( cm, m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	if( m_pOrbiter->m_pObj_Highlighted && 
		( (m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false || m_pOrbiter->m_pObj_Highlighted->m_ChildObjects.size()) )
	{
g_pPlutoLogger->Write(LV_WARNING,"change hl doesn't contain %d,%d h:%p v:%p",X,Y,m_pObj_Locked_Horizontal,m_pObj_Locked_Vertical);
		DesignObj_Orbiter *pObj_ToHighlight=FindChildObjectAtPosition(m_cLocked_Axis_Current == AXIS_LOCK_X ? m_pObj_Locked_Horizontal : m_pObj_Locked_Vertical,X,Y);
		// The user has moved off the highlighted object.  Find the object under here to highlight

		if( pObj_ToHighlight && pObj_ToHighlight!=m_pOrbiter->m_pObj_Highlighted )
		{
			m_pOrbiter->UnHighlightObject();
			m_pOrbiter->ExecuteCommandsInList( &m_pOrbiter->m_pObj_Highlighted->m_Action_UnhighlightList, m_pOrbiter->m_pObj_Highlighted, smHighlight, 0, 0 );
			m_pOrbiter->m_pObj_Highlighted = pObj_ToHighlight;
			m_pOrbiter->DoHighlightObject();
		}
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
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();
	m_iTime_Last_Mouse_Up=0;
	m_bRepeatMenu=false;  // This will be true if the user was hitting the same button that activated the menu (ie tapping again to make it go away)

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST )
	{
		m_bRepeatMenu = m_EMenuOnScreen==mb_MainMenu;
g_pPlutoLogger->Write(LV_FESTIVAL,"menu repeate %d",(int) m_bRepeatMenu);
		if(  !m_bRepeatMenu )
		{
			m_EMenuOnScreen=mb_MainMenu;
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popMainMenu_CONST),0,0,"","left",false,false);
			Set_Mouse_Behavior("Lhu",true,"Y",StringUtils::itos(DESIGNOBJ_popMainMenu_CONST));
		}
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST )
	{
		m_bUsingDiscreteMode=false; // We'll set it to true later
		string sResponse;
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,0,0);
		m_pOrbiter->SendCommand(CMD_Change_Playback_Speed,&sResponse);

		string sText,sMediaPosition;
		DCE::CMD_Report_Playback_Position CMD_Report_Playback_Position(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,0,&sText,&sMediaPosition);
		m_bHasTimeline = m_pOrbiter->SendCommand(CMD_Report_Playback_Position) && ParsePosition(sMediaPosition);

		m_iLastSpeed=0;
		m_bRepeatMenu = m_EMenuOnScreen==mb_MediaControl;
g_pPlutoLogger->Write(LV_FESTIVAL,"down repeate %d",(int) m_bRepeatMenu);
//remus m_pObj_Locked_Horizontal->HasTimeline(m_bHasTimeline);
		if(  !m_bRepeatMenu )
		{
			m_EMenuOnScreen=mb_MediaControl;
			DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_popSpeedControl_CONST);  // Temp until the widget is done and can set this
			pObj->m_rPosition.Width=964;
			pObj->m_rPosition.Height=90;
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popSpeedControl_CONST),263,526,"","horiz",false,false);
x			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(4898 /*DESIGNOBJ_popDVDChapters_CONST*/),0,0,"","left",false,false);
			Set_Mouse_Behavior("S",false,"X",StringUtils::itos(DESIGNOBJ_popSpeedControl_CONST));
			Set_Mouse_Behavior("T",false,"Y",StringUtils::itos(4898 /*DESIGNOBJ_popDVDChapters_CONST*/));
		}
		m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();  // The above may have taken too much time already
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST )
	{
		m_bRepeatMenu = m_EMenuOnScreen==mb_Ambiance;
g_pPlutoLogger->Write(LV_FESTIVAL,"down ambiance repeate %d",(int) m_bRepeatMenu);
//remus m_pObj_Locked_Horizontal->HasTimeline(m_bHasTimeline);
		if(  !m_bRepeatMenu )
		{
			m_EMenuOnScreen=mb_Ambiance;
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popVolume_CONST),263,526,"","horiz",false,false);
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST),0,0,"","left",false,false);
			Set_Mouse_Behavior("V",false,"X",StringUtils::itos(DESIGNOBJ_popVolume_CONST));
			Set_Mouse_Behavior("G",false,"Y",StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST));
		}
	}

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return false; // Nothing to do
	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseBehaviorHandler_Horizontal )
		return CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Horizontal)(mb_MouseDown, m_pObj_Locked_Horizontal, PK_Button, 0, 0);
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseBehaviorHandler_Vertical )
		return CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Vertical)(mb_MouseDown, m_pObj_Locked_Vertical, PK_Button, 0, 0);
	return false;
}

bool MouseBehavior::ButtonUp(int PK_Button)
{
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Up=ProcessUtils::GetMsTime();

	// If the user held the button for more than HoldTime, resume normal playback
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"up repeate %d hold: %s (up %d dn  %d diff %d)",(int) m_bRepeatMenu,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime ? "Y" : "N",
			m_iTime_Last_Mouse_Up,m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down		  );
		if( m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime || m_bRepeatMenu )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
		{
			if( !m_bRepeatMenu )
			{
				PLUTO_SAFETY_LOCK( cm, m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
				if(  m_pOrbiter->m_pObj_Highlighted && !m_pOrbiter->m_pObj_Highlighted->IsHidden(  )  )
					m_pOrbiter->SelectedObject( m_pOrbiter->m_pObj_Highlighted, smNavigation );
			}
			m_pOrbiter->CMD_Remove_Popup("","left");
			m_pOrbiter->CMD_Remove_Popup("","horiz");
			Set_Mouse_Behavior("",false,"","");
			m_EMenuOnScreen=mb_None;
		}
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"up repeate %d hold: %s (up %d dn  %d diff %d)",(int) m_bRepeatMenu,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime ? "Y" : "N",
			m_iTime_Last_Mouse_Up,m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down		  );
		if( m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime || m_bRepeatMenu )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
		{
			DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000);
			m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
			m_iLastSpeed=1000;
			m_pOrbiter->CMD_Remove_Popup("","horiz");
			m_pOrbiter->CMD_Remove_Popup("","left");
			m_EMenuOnScreen=mb_None;
			Set_Mouse_Behavior("",false,"","");
		}
		else if( m_bHasTimeline )
		{
			int Percentage = (m_CurrentMedia_Stop-m_CurrentMedia_Start)/m_CurrentMedia_Pos;
			m_bUsingDiscreteMode=true;
g_pPlutoLogger->Write(LV_FESTIVAL," setting timeline position to %d",Percentage);
//remus m_pObj_Locked_Horizontal->PositionCurrentTime(X);
//remus m_pObj_Locked_Horizontal->SetMode(absolute_navigation);
		}
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"up repeate %d hold: %s (up %d dn  %d diff %d)",(int) m_bRepeatMenu,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime ? "Y" : "N",
			m_iTime_Last_Mouse_Up,m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down		  );
		if( m_iTime_Last_Mouse_Up-m_iTime_Last_Mouse_Down>MouseSensitivity::HoldTime || m_bRepeatMenu )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
		{
			m_pOrbiter->CMD_Remove_Popup("","horiz");
			m_pOrbiter->CMD_Remove_Popup("","left");
			m_EMenuOnScreen=mb_None;
			Set_Mouse_Behavior("",false,"","");
		}
	}

	if( m_cLockedAxes == AXIS_LOCK_NONE )
		return false; // Nothing to do
	if( m_cLocked_Axis_Current==AXIS_LOCK_X && m_pMouseBehaviorHandler_Horizontal )
		return CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Horizontal)(mb_MouseUp, m_pObj_Locked_Horizontal, PK_Button, 0, 0);
	else if( m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pMouseBehaviorHandler_Vertical )
		return CALL_MEMBER_FN(*this, m_pMouseBehaviorHandler_Vertical)(mb_MouseUp, m_pObj_Locked_Vertical, PK_Button, 0, 0);
	return false;
}

bool MouseBehavior::ParsePosition(string sMediaPosition)
{
	m_CurrentMedia_Start=0;
	string::size_type pos=sMediaPosition.find("POS:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Pos = atoi( sMediaPosition.substr(pos+4).c_str() ) / 1000;
	
	pos=sMediaPosition.find("TOTAL:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Stop = atoi( sMediaPosition.substr(pos+6).c_str() ) / 1000;
	
	return m_CurrentMedia_Stop>0;
}
