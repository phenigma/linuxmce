#include "KeyboardMouseHandler.h"
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

using namespace DCE;

void KeyboardMouseHandler::Start()
{
	m_cDirection=0;
	if( !m_pObj )
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_Width/2,
			m_pMouseBehavior->m_pOrbiter->m_Height/2);
	else if ( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		SelectFirstObject();
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
			m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
	}
}

void KeyboardMouseHandler::Stop()
{
}

bool KeyboardMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
			StringUtils::itos(BUTTON_Enter_CONST),"");
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
	}
	return false; // Keep processing
}

bool KeyboardMouseHandler::ButtonUp(int PK_Button)
{
	return false; // Keep processing
}

void KeyboardMouseHandler::Move(int X,int Y)
{
	if( !m_pObj )
	{
		MoveExternalApp(X,Y);
		return;
	}

	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		SelectFirstObject();
		return; // Shouldn't have happened
	}

	int PK_Direction = GetDirectionAwayFromHighlight(X,Y);
	if( PK_Direction )
	{
		DesignObj_Orbiter *pObj_Before = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
		int HighlightedRow=-1;

		// Let's see if we've been highlighting a datagrid and moved past the top or bottom
		// If so, we're going to engage special behavior where we start the iterator to do scroll
		if( pObj_Before->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			if( (PK_Direction==DIRECTION_Up_CONST || PK_Direction==DIRECTION_Down_CONST) &&
				MovedPastTopBottomOfDataGrid((DesignObj_DataGrid *) pObj_Before,PK_Direction,Y) )
					return; // We're no iterator mode

            HighlightedRow = ((DesignObj_DataGrid *) pObj_Before)->m_iHighlightedRow;
		}
		m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction);
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
			(pObj_Before!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || 
			(pObj_Before->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && ((DesignObj_DataGrid *) pObj_Before)->m_iHighlightedRow!=HighlightedRow)) )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::Move was %s now %s after direction %d",
					  pObj_Before->m_ObjectID.c_str(),m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectID.c_str(),PK_Direction);
			m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
			PositionMouseAtObjectEdge(PK_Direction);
			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pDataGridTable==NULL )
				m_pMouseBehavior->m_pOrbiter->RedrawObjects();  // We may have scrolled past the end of a grid and need to re-render
		}
	}
}

void KeyboardMouseHandler::MoveExternalApp(int X,int Y)
{
	bool bHorizontal = m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X;
	int NotchSize = (double) bHorizontal ? m_pMouseBehavior->m_pOrbiter->m_Width / 20 : m_pMouseBehavior->m_pOrbiter->m_Height / 20;
	int Notch;
	if( bHorizontal )
		Notch = X/NotchSize - 10;
	else
		Notch = Y/NotchSize - 10;
g_pPlutoLogger->Write(LV_FESTIVAL,"Notch %d  horizontal %d  %d,%d",Notch,(int) bHorizontal,X,Y);

	if( Notch<-10 )
		Notch = -10;
	if( Notch>10 )
		Notch = 10;

	if( Notch!=m_iLastNotch )
	{
		if( Notch==0 )
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
		else
		{
			int Frequency;
			bool bPage=false;
			if( abs(Notch)>5 )
			{
				bPage=true;
				if( Notch>0 )
					Frequency = (6-(Notch-5))*400;
				else
					Frequency = (6+(Notch+5))*400;
			}
			else
				Frequency = (6-abs(Notch))*400;

g_pPlutoLogger->Write(LV_FESTIVAL,"Frequency %d",Frequency);
			if( bHorizontal )
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'R' : 'L';
				else
					m_cDirection = Notch > 0 ? 'r' : 'l';
			}
			else
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'D' : 'U';
				else
					m_cDirection = Notch > 0 ? 'd' : 'u';
			}
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,0,Frequency,this);
		}
		m_iLastNotch=Notch;
	}
}

void KeyboardMouseHandler::DoIteration()
{
g_pPlutoLogger->Write(LV_FESTIVAL,"Direction %c",m_cDirection);
	if( m_pObj )
		IterateObject();
	else
		IterateExternalApp();
}

void KeyboardMouseHandler::IterateObject()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Up_CONST),"");
		}
		break;

		case 'D':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Down_CONST),"");
		}
		break;

		case 'u':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Up_Arrow_CONST),"");
		}
		break;

		case 'd':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Down_Arrow_CONST),"");
		}
		break;

		case 'L':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Left_CONST),"");
		}
		break;

		case 'R':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Right_CONST),"");
		}
		break;

		case 'l':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Left_Arrow_CONST),"");
		}
		break;

		case 'r':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Right_Arrow_CONST),"");
		}
		break;
	}
}

void KeyboardMouseHandler::IterateExternalApp()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Up_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'D':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Down_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'u':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Up_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'd':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Down_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'L':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Left_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'R':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Right_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'l':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Left_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'r':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Right_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;
	}
}

void KeyboardMouseHandler::SelectFirstObject()
{
	// Select the first object to highlight if we didn't already and center over it
	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted = m_pMouseBehavior->m_pOrbiter->FindFirstObjectByDirection('1',true,NULL,NULL);
	if( pObj )
	{
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
	}
}

int KeyboardMouseHandler::GetDirectionAwayFromHighlight(int X,int Y)
{
	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted; // Make the code look better

	PlutoRectangle plutoRectangle = GetHighlighedObjectCoordinates();
	if( plutoRectangle.Contains(X,Y) )
		return 0;  // We're still inside the object

	// If the highlighted object is a datagrid and we're totally outside the datagrid, then use the datagrid
	// as a whole for determining which side we're on, and not the individual cell, otherwise we'll always
	// think we're below a datagrid without a lot of cells even if we're to the left or right
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && !(pObj->m_rPosition+pObj->m_pPopupPoint).Contains(X,Y) )
		plutoRectangle = pObj->m_rPosition + pObj->m_pPopupPoint;

	g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::GetDirectionAwayFromHighlight object %s at %d,%d w:%d h:%d doesn't contain %d,%d",
		pObj->m_ObjectID.c_str(),plutoRectangle.X,plutoRectangle.Y,
		plutoRectangle.Width,plutoRectangle.Height,X,Y);
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

void KeyboardMouseHandler::PositionMouseAtObjectEdge(int PK_Direction)
{
	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted; // Make the code look better
	PlutoRectangle rect = GetHighlighedObjectCoordinates();

	switch(PK_Direction)
	{
	case DIRECTION_Left_CONST:
		m_pMouseBehavior->SetMousePosition( rect.Right(), 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Up_CONST:
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2, 
			rect.Bottom());
		return;
	case DIRECTION_Right_CONST:
		m_pMouseBehavior->SetMousePosition( rect.X, 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Down_CONST:
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2, 
			rect.Y);
		return;
	};
}

PlutoRectangle KeyboardMouseHandler::GetHighlighedObjectCoordinates()
{
	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			PlutoRectangle rect;
			m_pMouseBehavior->m_pOrbiter->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted,rect);
			return rect + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
		}
		else
			return m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
	}

	return PlutoRectangle(0,0,0,0);
}

bool KeyboardMouseHandler::MovedPastTopBottomOfDataGrid(DesignObj_DataGrid *pObj,int PK_Direction,int Y)
{
return false;

// Get this to work so moving past the bottom causes it to start the iterator and begin first scrolling
// at up to 5 speeds, and then paging at up to 5 speeds by continually moving in the same direction.  As
// soon as the direction changes, or the user moves back to the top, reset everything.
	if( PK_Direction==DIRECTION_Up_CONST )
	{
		if( Y >=pObj->m_rPosition.Y )
			return false;  // We've still got room to go up
		m_dwPK_Direction_ScrollGrid = DIRECTION_Up_CONST;
		return true;
	}

	// Down is trickier since the last visible cell may not extend all the way to the bottom (the top cell is always the top)
	// So get the coorindates of the last visible cell
	PlutoRectangle r;
	m_pMouseBehavior->m_pOrbiter->GetGridCellDimensions( pObj,
		1, // col span
		1, // row span
		0, // column
		pObj->m_MaxRow + pObj->m_GridCurRow, // We only care about the position of the last visible row, whether or not anything is there
		r.X,  r.Y,  r.Width,  r.Height );


	if( Y<r.Bottom() )
		return false;

	m_dwPK_Direction_ScrollGrid = DIRECTION_Down_CONST;
	return true;
}

