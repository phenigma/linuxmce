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
#include "DataGrid.h"

using namespace DCE;

KeyboardMouseHandler::KeyboardMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) 
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{ 
	m_bLockAxis=true; m_dwPK_Direction_ScrollGrid=0; m_pObj_MediaBrowser_Alpha=m_pObj_ScrollingGrid=NULL;
}

void KeyboardMouseHandler::Start()
{
	m_PK_Direction_Last=0;
	m_cDirection=0;
	m_dwPK_Direction_ScrollGrid = 0;
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL); // In case we're scrolling a grid

	if( !m_pObj )
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_Width/2,
			m_pMouseBehavior->m_pOrbiter->m_Height/2);
	else
	{
		if ( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			SelectFirstObject();
			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
				m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
		}
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			PlutoRectangle rect = GetHighlighedObjectCoordinates();
			m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
				rect.Y + rect.Height/2 );

			if( m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X )
				m_pMouseBehavior->m_iLockedPosition = rect.Y + rect.Height/2;
			else
				m_pMouseBehavior->m_iLockedPosition = rect.X + rect.Width/2;
		}


		m_pObj_MediaBrowser_Down = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoDownIndicator_CONST));
		m_pObj_MediaBrowser_Up = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoUpIndicator_CONST));
		m_pObj_MediaBrowser_Alpha = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoAlpha_CONST));
		if( m_pObj_MediaBrowser_Down && !m_pObj_MediaBrowser_Down->m_bOnScreen )
			m_pObj_MediaBrowser_Down = NULL;
		if( m_pObj_MediaBrowser_Up && !m_pObj_MediaBrowser_Up->m_bOnScreen )
			m_pObj_MediaBrowser_Up = NULL;
		if( m_pObj_MediaBrowser_Alpha && !m_pObj_MediaBrowser_Alpha->m_bOnScreen )
			m_pObj_MediaBrowser_Alpha = NULL;
		if( m_pObj_MediaBrowser_Down )
			m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_NORMAL;
		if( m_pObj_MediaBrowser_Up )
			m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_NORMAL;
		m_iLastNotch = -999;
	}
}

void KeyboardMouseHandler::Stop()
{
	if( m_pObj )
	{
		// temp hack -- todo.  temporary redraw the screen since the keyboard/speed indicators may be messed up
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change k/b" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
	}
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

void KeyboardMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( !m_dwPK_Direction_ScrollGrid && m_PK_Direction_Last && PK_Direction!=m_PK_Direction_Last && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		// The user is reversing direction.  Recenter so we don't switch back to easily, to give that 'pop' feel to the button
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
		m_PK_Direction_Last=PK_Direction;
		return;
	}
	m_PK_Direction_Last=PK_Direction;

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

	if( m_dwPK_Direction_ScrollGrid )
	{
		ScrollGrid(PK_Direction,X,Y);
		return;
	}

	PK_Direction = GetDirectionAwayFromHighlight(X,Y);
	if( PK_Direction )
	{
		DesignObj_Orbiter *pObj_Before = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;

		// Let's see if we've been highlighting a datagrid and moved past the top or bottom
		// If so, we're going to engage special behavior where we start the iterator to do scroll
		if( pObj_Before->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			if( (pObj_Before->m_rPosition + pObj_Before->m_pPopupPoint).Contains(X,Y) )
			{
				DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) pObj_Before;

				// We're still inside the grid, just highlight whatever cell is underneath us, figure that out
				int Row_Before = pDesignObj_DataGrid->m_iHighlightedRow;
				int Column_Before = pDesignObj_DataGrid->m_iHighlightedColumn;

				int DeltaY=0,DeltaX=0;
				if ( pDesignObj_DataGrid->m_FirstRowHeight > 0 && ( pDesignObj_DataGrid->m_GridCurRow == 0 || pDesignObj_DataGrid->m_bKeepRowHeader )  )
					DeltaY = pDesignObj_DataGrid->m_FirstRowHeight - pDesignObj_DataGrid->m_FixedRowHeight;
				if ( pDesignObj_DataGrid->m_FirstColumnWidth > 0 && ( pDesignObj_DataGrid->m_GridCurCol == 0 || pDesignObj_DataGrid->m_bKeepColHeader )  )
					DeltaX = pDesignObj_DataGrid->m_FirstColumnWidth - pDesignObj_DataGrid->m_FixedColumnWidth;

				int X2 = X - pObj_Before->m_pPopupPoint.X - DeltaX;
				int Y2 = Y - pObj_Before->m_pPopupPoint.Y - DeltaY;
				
				int Row = Y2 / (pDesignObj_DataGrid->m_FixedRowHeight+1);
				int Column = X2 / (pDesignObj_DataGrid->m_FixedColumnWidth+1);
				if( pDesignObj_DataGrid->m_iHighlightedRow!=-1 )
					pDesignObj_DataGrid->m_iHighlightedRow = Row;
				if( pDesignObj_DataGrid->m_iHighlightedColumn!=-1 )
					pDesignObj_DataGrid->m_iHighlightedColumn = Column;

				if( pDesignObj_DataGrid->m_iHighlightedRow > pDesignObj_DataGrid->m_MaxRow - 1 )
					pDesignObj_DataGrid->m_iHighlightedRow = pDesignObj_DataGrid->m_MaxRow - 1;
				if( pDesignObj_DataGrid->m_iHighlightedColumn > pDesignObj_DataGrid->m_MaxCol - 1 )
					pDesignObj_DataGrid->m_iHighlightedColumn = pDesignObj_DataGrid->m_MaxCol - 1;

				if( pDesignObj_DataGrid->m_iHighlightedRow!=-1 && pDesignObj_DataGrid->m_GridCurRow + pDesignObj_DataGrid->m_iHighlightedRow >= pDesignObj_DataGrid->m_pDataGridTable->GetRows() )
					pDesignObj_DataGrid->m_iHighlightedRow = pDesignObj_DataGrid->m_pDataGridTable->GetRows() - pDesignObj_DataGrid->m_GridCurRow - 1;

				if( pDesignObj_DataGrid->m_iHighlightedColumn!=-1 && pDesignObj_DataGrid->m_GridCurCol + pDesignObj_DataGrid->m_iHighlightedColumn >= pDesignObj_DataGrid->m_pDataGridTable->GetCols() )
					pDesignObj_DataGrid->m_iHighlightedColumn = pDesignObj_DataGrid->m_pDataGridTable->GetCols() - pDesignObj_DataGrid->m_GridCurCol - 1;

				if( pDesignObj_DataGrid->m_iHighlightedRow!=Row_Before || pDesignObj_DataGrid->m_iHighlightedColumn!=Column_Before )
				{
					m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(pDesignObj_DataGrid);

//m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction);
					m_pMouseBehavior->m_pOrbiter->RedrawObjects();  // We may have scrolled past the end of a grid and need to re-render
				}
				TempHack_DrawAlphaSquare();
				return;
			}

			if( (PK_Direction==DIRECTION_Up_CONST || PK_Direction==DIRECTION_Down_CONST) &&
				MovedPastTopBottomOfDataGrid((DesignObj_DataGrid *) pObj_Before,PK_Direction,Y) )
					return; // We're now iterator mode
		}
		
		if( !m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction) )
		{
			// We're off in no-mans land, and not moving toward any selectable object, get back on the highlighted object
			PlutoRectangle rect = GetHighlighedObjectCoordinates();
			m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
				rect.Y + rect.Height/2 );
			return;
		}

		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
			pObj_Before!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::Move was %s now %s after direction %d",
					  pObj_Before->m_ObjectID.c_str(),m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectID.c_str(),PK_Direction);
			m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);

			// If we're just moving within the same object, don't reposition the mouse.  It's a grid anyway.
			// But if we're moving to another object, we don't want to be trapped inside 'dead' space, so move to the 
			// edge of that object
			if( pObj_Before!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
			{
				PositionMouseAtObjectEdge(PK_Direction);
				PlutoRectangle rect = GetHighlighedObjectCoordinates();
				if( m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X )
					m_pMouseBehavior->m_iLockedPosition = rect.Y + rect.Height/2;
				else
					m_pMouseBehavior->m_iLockedPosition = rect.X + rect.Width/2;
			}
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

		if( m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X )
			m_pMouseBehavior->m_iLockedPosition = rect.Y + rect.Height/2;
		else
			m_pMouseBehavior->m_iLockedPosition = rect.X + rect.Width/2;
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
	// Moving past the bottom causes it to start the iterator and begin first scrolling
	// at up to 5 speeds, and then paging at up to 5 speeds by continually moving in the same direction.  As
	// soon as the direction changes, or the user moves back to the top, reset everything.
	if( PK_Direction==DIRECTION_Up_CONST )
	{
		if( Y >=pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y )
			return false;  // We've still got room to go up
		m_dwPK_Direction_ScrollGrid = DIRECTION_Up_CONST;

		// Move to the bottom of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Bottom() + pObj->m_pPopupPoint.Y );
	}
	else
	{
		// Down is trickier since the last visible cell may not extend all the way to the bottom (the top cell is always the top)
		// So get the coorindates of the last visible cell
		PlutoRectangle r;
		m_pMouseBehavior->m_pOrbiter->GetGridCellDimensions( pObj,
			1, // col span
			1, // row span
			0, // column
			pObj->m_MaxRow - 1 /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
			r.X,  r.Y,  r.Width,  r.Height );

		r = r + pObj->m_pPopupPoint;

		if( Y<r.Bottom() )
			return false;

		m_dwPK_Direction_ScrollGrid = DIRECTION_Down_CONST;

		// Move to the top of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y );
	}

	m_pObj_ScrollingGrid = (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
	m_iLastNotch = -999; // Start with no movement
	m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_SELECTED;
	m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_SELECTED;

	return true;
}

void KeyboardMouseHandler::ScrollGrid(int dwPK_Direction,int X,int Y)
{
	if( dwPK_Direction!=m_dwPK_Direction_ScrollGrid)
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::ScrollGrid direction changed from %d to %d",
					  m_dwPK_Direction_ScrollGrid,dwPK_Direction);
		// We moved past the top
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL); // In case we're scrolling a grid

		if( m_dwPK_Direction_ScrollGrid==DIRECTION_Up_CONST )
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , m_pObj_ScrollingGrid->m_rPosition.Y + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		else
		{
			PlutoRectangle r;
			m_pMouseBehavior->m_pOrbiter->GetGridCellDimensions( m_pObj_ScrollingGrid,
				1, // col span
				1, // row span
				0, // column
				m_pObj_ScrollingGrid->m_MaxRow - 1 /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
				r.X,  r.Y,  r.Width,  r.Height );

			// Move back to the bottom of the grid, since we're done with the iterator
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , r.Bottom() + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		}

		// Unhighlight the speed
		m_dwPK_Direction_ScrollGrid = 0;
		m_iLastNotch = -999;
		TempHack_DrawAlphaSquare();
		m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_SELECTED;
		m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_SELECTED;

		return;
	}

	// 11 notches, 0=pause, 5 scroll, 5 pages
	int NotchWidth = m_pObj_ScrollingGrid->m_rPosition.Height/11;
	int Notch = (Y-m_pObj_ScrollingGrid->m_rPosition.Y-m_pObj_ScrollingGrid->m_pPopupPoint.Y)/NotchWidth;
	if( Notch<0 )
		Notch = 0;
	if( Notch>10 )
		Notch = 10;

	if( dwPK_Direction==DIRECTION_Up_CONST )
		Notch = 10 - Notch; // It's reversed if we're moving up

	if( Notch!=m_iLastNotch )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::ScrollGrid Notch %d",Notch);
		if( Notch==0 )
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
		else
		{
			int Frequency;
			bool bPage=false;
			if( Notch>5 )
				Notch=5;
				/*
			{
		m_iLastNotch=Notch;
		return;
				bPage=true;
				Frequency = (11-Notch)*100;
			}
			else
			*/
				Frequency = (6-Notch)*100;
				bPage=true;

			if( bPage )
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,dwPK_Direction==DIRECTION_Down_CONST ? 2 : -2,Frequency,NULL);
			else
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,dwPK_Direction==DIRECTION_Down_CONST ? 1 : -1,Frequency,NULL);
		}
		m_iLastNotch=Notch;
		if( dwPK_Direction==DIRECTION_Down_CONST )
		{
			m_pObj_MediaBrowser_Down->m_GraphicToDisplay=m_iLastNotch;
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Down);
		}
		else
		{
			m_pObj_MediaBrowser_Up->m_GraphicToDisplay=m_iLastNotch;
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Down);
		}
//remus  m_pObj_ScrollingGrid->SetSpeed(Speed);
	}
	TempHack_DrawAlphaSquare();
}


void KeyboardMouseHandler::TempHack_DrawAlphaSquare()
{
	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
	if( !pObj || pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST || !pObj->m_pDataGridTable || !m_pObj_MediaBrowser_Alpha )
		return;
	
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj;
	if( pObj_Grid->m_iHighlightedRow==-1 )
		return;

	DataGridCell *pCell=pObj_Grid->m_pDataGridTable->GetData( 1,  pObj_Grid->m_GridCurRow + pObj_Grid->m_iHighlightedRow );
	if( !pCell )
		return;

	char cLetter = pCell->m_Text[0];
	if( cLetter>'Z' )
		cLetter -= 32; 

	if( cLetter<65 )
		cLetter = 0;
	else
		cLetter = cLetter - 64; // 0=anything non-alpha, 1=a, etc.

	float NotchWidth = ((float) m_pObj_MediaBrowser_Alpha->m_rPosition.Height)/27; // Allow for 0,A-Z
	int NotchStart = cLetter * NotchWidth + m_pObj_MediaBrowser_Alpha->m_rPosition.Y + m_pObj_MediaBrowser_Alpha->m_pPopupPoint.Y;
	m_pMouseBehavior->m_pOrbiter->HollowRectangle(
		m_pObj_MediaBrowser_Alpha->m_rPosition.X + m_pObj_MediaBrowser_Alpha->m_pPopupPoint.X, NotchStart,
		m_pObj_MediaBrowser_Alpha->m_rPosition.Width, NotchWidth,
		PlutoColor::White());
	m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj_MediaBrowser_Alpha->m_rPosition + m_pObj_MediaBrowser_Alpha->m_pPopupPoint);

}

bool KeyboardMouseHandler::SlowDrift(int &X,int &Y)
{
	if( m_dwPK_Direction_ScrollGrid )
		return false;
g_pPlutoLogger->Write(LV_FESTIVAL,"xxKeyboardMouseHandler::SlowDrift");
	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
		return true;
	}
	return false;
}

