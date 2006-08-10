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
    , m_nRelativePointer_SpeedShape(0)
    , m_pRelativePointer_Image(NULL)
{
	m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	m_pObj_CoverArtPopup=NULL;
	m_bCapturingOffscreenMovement=false;

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

MediaBrowserMouseHandler::~MediaBrowserMouseHandler()
{
    if (m_pRelativePointer_Image)
    {
        delete m_pRelativePointer_Image;
        m_pRelativePointer_Image = NULL;
    }
}

void MediaBrowserMouseHandler::Start()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Start()");
}

void MediaBrowserMouseHandler::Stop()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Stop()");
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
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativeMove(%d, %d)", DeltaX, DeltaY);
    //RelativePointer_SetStatus(-1);//delete this debug-only lines
    // DeltaY is always 0 here ? -1 should be DeltaY
    // not working here, because the position is always reset
#ifdef WIN32
	string s = StringUtils::Format("DeltaX:=%d, DeltaY=%d, VY=%d\r\n", DeltaX, DeltaY, m_RelativeVirtualY);
	::OutputDebugString(s.c_str());
#endif

	if (m_RelativeVirtualY <= 1) // We're at the top of the screen.
	{
		m_RelativeVirtualY+=DeltaY;
		if (DeltaY > 0)
		{
			if (DeltaY >=5 || m_RelativeVirtualY > 2)
			{
				m_saveY+=5;
				ReleaseRelative();
				return;
			} 
			else
			{
				m_RelativeVirtualY = max(m_RelativeVirtualY, DeltaY-5);
			}
		}
		while (m_RelativeVirtualY < -100)
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("", "", 	DIRECTION_Up_CONST);	
			m_pMouseBehavior->m_pOrbiter->CMD_Refresh("");
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
				m_saveY-=5;
				ReleaseRelative();
				return;
			}
			else
			{
				m_RelativeVirtualY = min(m_RelativeVirtualY, m_pMouseBehavior->m_pOrbiter->m_Height-DeltaY);
			}
		}
		while (m_RelativeVirtualY > m_pMouseBehavior->m_pOrbiter->m_Height + 100)
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("", "", 	DIRECTION_Down_CONST);		
			m_pMouseBehavior->m_pOrbiter->CMD_Refresh("");
			m_RelativeVirtualY -= 100;
		}
		return;
	}
}


void MediaBrowserMouseHandler::ReleaseRelative()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::ReleaseRelative()");
	m_bCapturingOffscreenMovement=false;
	m_pMouseBehavior->ShowMouse(true);
	m_pMouseBehavior->SetMousePosition(m_saveX, m_saveY);

}

void MediaBrowserMouseHandler::Move(int X,int Y,int PK_Direction)
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Move(%d, %d, %d)", X, Y, PK_Direction);

    //RelativePointer_SetStatus(Y-300);//delete this debug-only lines
    // here seems to be working, somehow
    // but this class is heavily broken,
    // and the effect does not look good at all

    if (m_bCapturingOffscreenMovement)
	{
		m_pMouseBehavior->GetMousePosition(&m_Rel);
		// Todo: X and Y are in relative coordinates, where we have
		// moved to an absolute.  This would be cleaner if
		// SetMousePosition moved to the relative coords.
		if (m_Rel.Y-100 != 0)
		{
			RelativeMove(m_Rel.X-300, m_Rel.Y-300);
			if (m_bCapturingOffscreenMovement)
	            m_pMouseBehavior->SetMousePosition(300,300);
		}
		return;
	}
	if (Y <= 1 || Y >= m_pMouseBehavior->m_pOrbiter->m_Height-2)
	{
		m_pMouseBehavior->GetMousePosition(&m_Rel);

		m_saveX = m_Rel.X;
		m_saveY = m_Rel.Y;
		m_RelativeVirtualY = Y;
		m_bCapturingOffscreenMovement=true;
		// Todo: SetMousePosition is not relative to the window.
		// If 100,100 is outside the orbiter, we won't receive the movement
		// information.
		m_pMouseBehavior->SetMousePosition(300,300);
		m_pMouseBehavior->ShowMouse(false);
		return;

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

	PLUTO_SAFETY_LOCK(M, m_pMouseBehavior->m_pOrbiter->Renderer()->m_bgImageReqMutex );

	if( pCell && pCell->m_pGraphic && pCell->m_pGraphic->m_pGraphicData)
	{
		char *pDup = new char[pCell->m_pGraphic->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphic->m_pGraphicData,pCell->m_pGraphic->m_GraphicLength);
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_pGraphic->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);
	}
	else
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
}

bool MediaBrowserMouseHandler::RelativePointer_SetStatus(int nSpeedShape)
{
    const int nPeakSpeed = 5;
    g_pPlutoLogger->Write(LV_STATUS, "MediaBrowserMouseHandler::RelativePointer_SetStatus(%d)", nSpeedShape);
    // check range
    if ( (nSpeedShape > 0) && (nSpeedShape > nPeakSpeed) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativePointer_SetStatus(%d) : decreasing shape to %d", nSpeedShape, nPeakSpeed);
        nSpeedShape = nPeakSpeed;
    }
    if ( (nSpeedShape < 0) && (nSpeedShape < nPeakSpeed) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativePointer_SetStatus(%d) : increasing shape to %d", nSpeedShape, -nPeakSpeed);
        nSpeedShape = -nPeakSpeed;
    }
    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;
    // restore the normal behavior in this case
    if (nSpeedShape == 0)
    {
        m_pMouseBehavior->ShowMouse(true);
    }
    // hide the pointer : will draw a fake one
    m_pMouseBehavior->ShowMouse(false);
    // get current screen size
    PlutoSize oSizeScreen = m_pMouseBehavior->m_pOrbiter->m_sScreenSize;
    // get current mouse position
    PlutoPoint posFakeMouse;
    m_pMouseBehavior->GetMousePosition(&posFakeMouse);
    // pointer move to the middle of the screen
    m_pMouseBehavior->SetMousePosition(oSizeScreen.Width/2, oSizeScreen.Height/2);
    // compute the image path
    char buffer[100];
    sprintf(buffer, "speed_shape_%d.xbm", nSpeedShape);
    string sPath = "/usr/pluto/orbiter/skins/Basic/cursors/pointers_bw/";
    sPath = sPath + buffer;

    //delete this debug-only block
    // with a real pointer, not a fake one
    if (0)
    {
        string sPathMask = sPath + ".msk";
        m_pMouseBehavior->SetMouseCursorImage(sPath, sPathMask);
        if (nSpeedShape > 0)
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y - nPeakSpeed);
        else
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y + nPeakSpeed);
    }
    
    // read the image from disk
    sPath = "/usr/pluto/orbiter/skins/Basic/cursors/pointers/standard_big_b.png";//delete this debug-only line
    PlutoRectangle rectFakePointer;
	size_t iSize = 0;
    char * pData = FileUtils::ReadFileIntoBuffer(sPath.c_str(), iSize);
    if (pData == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "MediaBrowserMouseHandler::RelativePointer_SetStatus(%d) : cannot load graphic file %s", nSpeedShape, sPath);
        return false;
    }
    // creating the image in memory
    m_pRelativePointer_Image = m_pMouseBehavior->m_pOrbiter->m_pOrbiterRenderer->CreateGraphic();
    m_pRelativePointer_Image->LoadGraphic(pData, iSize);
    delete [] pData;
    rectFakePointer.Width = m_pRelativePointer_Image->Width;
    rectFakePointer.Height = m_pRelativePointer_Image->Height;
    // X coordinate : keep the pointer on the screen
    rectFakePointer.X = posFakeMouse.X;
    if (rectFakePointer.X + rectFakePointer.Width > oSizeScreen.Width)
        rectFakePointer.X = oSizeScreen.Width - rectFakePointer.Width;
    // Y coordinate
    if (nSpeedShape > 0)
        rectFakePointer.Y = oSizeScreen.Height - rectFakePointer.Height;
    else
        rectFakePointer.Y = 0;
    // draw fake pointer
    m_pMouseBehavior->m_pOrbiter->Renderer()->RenderGraphic(m_pRelativePointer_Image, rectFakePointer);
}
