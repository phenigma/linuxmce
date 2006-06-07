#include "MediaBrowserMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "OrbiterRenderer.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"
#include "DataGrid.h"
#include "ScreenHistory.h"

using namespace DCE;

MediaBrowserMouseHandler::MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	m_pObj_CoverArtPopup=NULL;
	if( !m_pObj )
		return; // Shouldn't happen

	DesignObj_DataList::iterator iHao;

	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if( pDesignObj_Orbiter->m_iBaseObjectID==4949 )
			m_pObj_ListGrid = (DesignObj_DataGrid *) pDesignObj_Orbiter;
		else if( pDesignObj_Orbiter->m_iBaseObjectID==5086 )
			m_pObj_PicGrid = (DesignObj_DataGrid *) pDesignObj_Orbiter;
	}
	if( !m_pObj_ListGrid || !m_pObj_PicGrid )
		g_pPlutoLogger->Write(LV_CRITICAL,"MediaBrowserMouseHandler::MediaBrowserMouseHandler");
	else
	{
		m_pObj_ListGrid->m_iHighlightedRow=0;
		m_pObj_PicGrid->m_iHighlightedRow=m_pObj_PicGrid->m_iHighlightedColumn=0;
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted = m_pObj_ListGrid;
		m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
	}
	m_pObj_CoverArtPopup=m_pMouseBehavior->m_pOrbiter->FindObject(5087);
}

void MediaBrowserMouseHandler::Start()
{
}

void MediaBrowserMouseHandler::Stop()
{
}

bool MediaBrowserMouseHandler::ButtonDown(int PK_Button)
{
		return false;
	if( m_pMouseBehavior->m_bMouseConstrained )
		return false;
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");
		return true; // Don't process any more
	}

	return false; // Keep processing
}

bool MediaBrowserMouseHandler::ButtonUp(int PK_Button)
{
		return false;
	if( m_pMouseBehavior->m_bMouseConstrained )
		return false;
	if( PK_Button==BUTTON_Mouse_7_CONST && m_bTapAndRelease==false && m_bStartedMovement )
	{
//		return false;
	}

	return false; // Keep processing
}

void MediaBrowserMouseHandler::RelativeMove(int DeltaX, int DeltaY)
{
#ifdef WIN32
	string s = StringUtils::Format("DeltaX:=%d, DeltaY=%d, VY=%d\r\n", DeltaX, DeltaY, m_RelativeVirtualY);
	::OutputDebugString(s.c_str());
#endif

	if (m_RelativeVirtualY <= 0) // We're at the top of the screen.
	{
		m_RelativeVirtualY+=DeltaY;
		if (DeltaY > 0)
		{
			if (DeltaY >=5 || m_RelativeVirtualY > 0)
			{
				m_pMouseBehavior->ReleaseRelativeMovements();
				return;
			} 
			else
			{
				m_RelativeVirtualY = max(m_RelativeVirtualY, DeltaY-5);
			}
		}
		while (m_RelativeVirtualY < -100)
		{
			// todo : scroll grid
			m_RelativeVirtualY += 100;
		}
	}
	else
	{
		m_RelativeVirtualY+=DeltaY;
		if (DeltaY < 0)
		{
			if (DeltaY < -5 || m_RelativeVirtualY < m_pMouseBehavior->m_pOrbiter->m_Height)
			{
				m_pMouseBehavior->ReleaseRelativeMovements();
				return;
			}
			else
			{
				m_RelativeVirtualY = min(m_RelativeVirtualY, m_pMouseBehavior->m_pOrbiter->m_Height-DeltaY);
			}
		}
		while (m_RelativeVirtualY > m_pMouseBehavior->m_pOrbiter->m_Height + 100)
		{
			// todo: scroll grid
			m_RelativeVirtualY -= 100;
		}
		return;
	}
}

void MediaBrowserMouseHandler::Move(int X,int Y,int PK_Direction)
{
	// add function .. is user at bottom.  If so... call function in mousebehavior capture relative movement
	// 
	if (Y <= 1 || Y >= m_pMouseBehavior->m_pOrbiter->m_Height-2)
	{
		m_RelativeVirtualY = Y;
		m_pMouseBehavior->CaptureRelativeMovements();
	}

	if( m_pMouseBehavior->m_bMouseConstrained )  // There's a popup grabbing the mouse
		return;
	if( !m_pObj_ListGrid || !m_pObj_PicGrid )
		return;  // Shouldn't happen
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	DesignObj_DataGrid *pObj_ListGrid_Active=NULL;
	if( m_pObj_ListGrid->m_rPosition.Contains(X,Y) ) 
		pObj_ListGrid_Active = m_pObj_ListGrid;
	else if( m_pObj_PicGrid->m_rPosition.Contains(X,Y) ) 
		pObj_ListGrid_Active = m_pObj_PicGrid;
	else
		return;

	PlutoRectangle rect;
	m_pMouseBehavior->m_pOrbiter->GetDataGridHighlightCellCoordinates(pObj_ListGrid_Active,rect);
	if( rect.Contains(X,Y) )
		return; // Nothing to do.  User is still on same cell

	// A very lean location of the highlighted row/cell
	int Row = (Y-pObj_ListGrid_Active->m_rPosition.Y)/pObj_ListGrid_Active->m_FixedRowHeight;
	if( pObj_ListGrid_Active==m_pObj_PicGrid )
	{
		int Col = (X-m_pObj_PicGrid->m_rPosition.X)/m_pObj_PicGrid->m_FixedColumnWidth;
		m_pObj_PicGrid->m_iHighlightedColumn=Col;
		m_pObj_PicGrid->m_iHighlightedRow=Row;
		m_pObj_ListGrid->m_iHighlightedRow = Row * m_pObj_PicGrid->m_MaxCol + Col;
	}
	else
	{
		m_pObj_ListGrid->m_iHighlightedRow = Row;
		Row = Row / m_pObj_PicGrid->m_MaxCol;
		int Col = m_pObj_ListGrid->m_iHighlightedRow - (Row * m_pObj_PicGrid->m_MaxCol);
		m_pObj_PicGrid->m_iHighlightedColumn=Col;
		m_pObj_PicGrid->m_iHighlightedRow=Row;
	}

	// This takes care of the list grid
	m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
	ShowCoverArtPopup();
}

void MediaBrowserMouseHandler::ShowCoverArtPopup()
{
	if( !m_pObj_CoverArtPopup )
		return; // Shouldn't happen

	int X=0,Y=0;
	if( m_pObj_PicGrid->m_iHighlightedColumn )
		X = (m_pObj_PicGrid->m_iHighlightedColumn*m_pObj_PicGrid->m_FixedColumnWidth) - ((m_pObj_CoverArtPopup->m_rPosition.Width - m_pObj_PicGrid->m_FixedColumnWidth)/2);
	if( m_pObj_PicGrid->m_iHighlightedRow )
		Y = (m_pObj_PicGrid->m_iHighlightedRow*m_pObj_PicGrid->m_FixedRowHeight) - ((m_pObj_CoverArtPopup->m_rPosition.Height - m_pObj_PicGrid->m_FirstRowHeight)/2);

	DataGridCell *pCell = NULL;
	if( m_pObj_PicGrid->m_pDataGridTable )
		pCell = m_pObj_PicGrid->m_pDataGridTable->GetData(m_pObj_PicGrid->m_iHighlightedColumn,m_pObj_PicGrid->m_iHighlightedRow);

	if( pCell && pCell->m_pGraphicData )
	{
		char *pDup = new char[pCell->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphicData,pCell->m_GraphicLength);
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,pCell->m_GraphicLength,"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);
	}
	else
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
}
