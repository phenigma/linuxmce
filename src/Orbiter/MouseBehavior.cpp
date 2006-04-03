#include "MouseBehavior.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
using namespace DCE;

#define AXIS_LOCK_NONE	0
#define AXIS_LOCK_X		1
#define AXIS_LOCK_Y		2
#define AXIS_LOCK_BOTH	3

//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	m_pOrbiter=pOrbiter;
	m_pObj_Locked_Vertical=m_pObj_Locked_Horizontal=NULL;
	ResetSamples();
	m_cLockedAxes = m_cLocked_Axis_Current = AXIS_LOCK_NONE;
}

void MouseBehavior::Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj)
{
	m_cLocked_Axis_Current = AXIS_LOCK_NONE;
	ProcessUtils::ResetMsTime();
	DesignObj_Orbiter *pObj=NULL;
	if( sDesignObj.size() )
		pObj = m_pOrbiter->FindObject(sDesignObj);

	if( sDirection.size() )
	{
		if( sDirection[0]=='Y' )
		{
			m_pObj_Locked_Vertical=pObj;
			m_sVerticalOptions=sOptions;
			m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_X || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_Y);
		}
		else if( sDirection[0]=='X' )
		{
			m_pObj_Locked_Horizontal=pObj;
			m_sHorizontalOptions=sOptions;
			m_cLockedAxes = (m_cLockedAxes==AXIS_LOCK_Y || m_cLockedAxes==AXIS_LOCK_BOTH ? AXIS_LOCK_BOTH : AXIS_LOCK_X);
		}
		else
			m_cLockedAxes = AXIS_LOCK_NONE;

		// Means auto-highlight the object from the direction following the 'h'
		string::size_type posHighlight;
		if( (posHighlight=sOptions.find('h'))!=string::npos && sOptions.size()>posHighlight+1 )
		{
			DesignObj_Orbiter *pObj_First = m_pOrbiter->FindFirstObjectByDirection(sOptions[posHighlight+1],false,pObj,NULL);
			if( pObj_First )
			{
				m_pStartMovement.X=pObj_First->m_rPosition.X + (pObj_First->m_rPosition.Width/2);
				m_pStartMovement.Y=pObj_First->m_rPosition.Y + (pObj_First->m_rPosition.Height/2);
				SetMousePosition(m_pStartMovement.X,m_pStartMovement.Y);
				m_pOrbiter->m_pObj_Highlighted=pObj_First;
				m_pOrbiter->RenderObjectAsync(m_pOrbiter->m_pObj_Highlighted);
			}
		}
	}
	else
		m_cLockedAxes = AXIS_LOCK_NONE;

	m_dwSamples[0]=ProcessUtils::GetMsTime();
	GetMousePosition(&m_pSamples[0]);
}

void MouseBehavior::Down(int X,int Y)
{
}

void MouseBehavior::Up(int X,int Y)
{
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
				m_iLockedPosition = m_pObj_Locked_Horizontal->m_rPosition.Y + m_pObj_Locked_Horizontal->m_rPosition.Height/2;
			else
				m_iLockedPosition = Y;
		}
		else
		{
			if( m_pObj_Locked_Vertical )
				m_iLockedPosition = m_pObj_Locked_Vertical->m_rPosition.X + m_pObj_Locked_Vertical->m_rPosition.Width/2;
			else
				m_iLockedPosition = X;
		}
	}

	if( m_pOrbiter->m_pObj_Highlighted && 
		(m_pOrbiter->m_pObj_Highlighted->m_rPosition.Contains(X,Y)==false || m_pOrbiter->m_pObj_Highlighted->m_ChildObjects.size()) )
	{
		DesignObj_Orbiter *pObj_ToHighlight=FindChildObjectAtPosition(m_cLocked_Axis_Current == AXIS_LOCK_X ? m_pObj_Locked_Horizontal : m_pObj_Locked_Vertical,X,Y);
		// The user has moved off the highlighted object.  Find the object under here to highlight

		if( pObj_ToHighlight && pObj_ToHighlight!=m_pOrbiter->m_pObj_Highlighted )
		{
			m_pOrbiter->UnHighlightObject();
			m_pOrbiter->m_pObj_Highlighted = pObj_ToHighlight;
			m_pOrbiter->DoHighlightObject();
		}
	}
}

bool MouseBehavior::CheckForChangeInDirection(int X,int Y)
{
	unsigned long dwTime = ProcessUtils::GetMsTime();
	if( m_cLocked_Axis_Current == AXIS_LOCK_NONE )
	{
		// See if the user clearly started moving in one direction
		int diffX = abs(m_pSamples[0].X - X);
		int diffY = abs(m_pSamples[0].Y - Y);
		if( diffX>MouseSensitivity::MinMoveToStart && diffY<MouseSensitivity::MaxMoveToStart )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
		else if( diffY>MouseSensitivity::MinMoveToStart && diffX<MouseSensitivity::MaxMoveToStart )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
		else
			return false;
	}

	if( dwTime-m_dwSamples[0]<MouseSensitivity::SampleInterval )
	{
		// Hardly any time has passed since the last move.  We'll only consider this to be movement in a new direction if it exceeds
		// the indicated minimum and twice as much in one direction as the other

		int diffX = abs(m_pSamples[0].X - X);
		int diffY = abs(m_pSamples[0].Y - Y);
		if( m_cLocked_Axis_Current = AXIS_LOCK_Y && diffX>MouseSensitivity::MinMovePerSampleToChangeDir && diffY<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
		else if( m_cLocked_Axis_Current = AXIS_LOCK_X && diffY>MouseSensitivity::MinMovePerSampleToChangeDir && diffX<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_Y;
			return true;
		}
		return false;
	}

	// Sufficient time has passed to make a new sample
	ShiftSamples();
	m_pSamples[0].X=X; m_pSamples[0].Y=Y; m_dwSamples[0]=dwTime;

	int DiffX = abs(m_pSamples[0].X-m_pSamples[1].X);
	int DiffY = abs(m_pSamples[0].Y-m_pSamples[1].Y);
	if( m_cLocked_Axis_Current == AXIS_LOCK_X )
	{
		if( DiffY>MouseSensitivity::MinMovePerSampleToChangeDir && DiffX<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_Y;
			return true;
		}
	}
	else
	{
		if( DiffX>MouseSensitivity::MinMovePerSampleToChangeDir && DiffY<MouseSensitivity::MaxMovePerSampleToChangeDir )
		{
			m_cLocked_Axis_Current = AXIS_LOCK_X;
			return true;
		}
	}

	// This sample period we kept moving in the same direction, stay locked in the track
	if( m_cLocked_Axis_Current == AXIS_LOCK_X && X!=m_iLockedPosition )
		SetMousePosition(m_iLockedPosition,Y);
	else if( m_cLocked_Axis_Current == AXIS_LOCK_Y && Y!=m_iLockedPosition )
		SetMousePosition(m_iLockedPosition,Y);

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
		if( pObj->m_bTabStop && pObj->m_rPosition.Contains(X,Y) )
			return pObj;
	}
	return NULL;
}
