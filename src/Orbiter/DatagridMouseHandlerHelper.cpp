#include "DatagridMouseHandlerHelper.h"
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
#include "OrbiterRenderer.h"
#include "DataGrid.h"

#ifdef ORBITER_OPENGL
#include "OpenGL/OrbiterRenderer_OpenGL.h"
#include "OpenGL/DataGridRenderer_OpenGL.h"
#include "OpenGL/OpenGL3DEngine.h"
#endif

using namespace DCE;

DatagridMouseHandlerHelper::DatagridMouseHandlerHelper(MouseHandler *pMouseHandler)
{ 
	m_pMouseHandler=pMouseHandler;
	m_pMouseBehavior=m_pMouseHandler->m_pMouseBehavior;
	m_dwPK_Direction_ScrollGrid=0; m_pObj_MediaBrowser_Alpha=m_pObj_ScrollingGrid=NULL;
    m_nRelativePointer_SpeedShape=0;
    m_pRelativePointer_Image=NULL;
	m_eCapturingOffscreenMovement=cosm_NO;
	m_bHitBottom=false;
}

DatagridMouseHandlerHelper::~DatagridMouseHandlerHelper()
{
	RelativePointer_Clear();
}

void DatagridMouseHandlerHelper::Start(DesignObj_DataGrid *pObj_ScrollingGrid,int NumNotches,int Top,int Bottom)
{
	m_dwPK_Direction_ScrollGrid = 0;
	m_pObj_ScrollingGrid=pObj_ScrollingGrid;
	m_NumNotches=NumNotches; m_Top=Top; m_Bottom=Bottom;
	m_bHitBottom=false;

	m_iLastNotch=-1;
	m_pObj_MediaBrowser_Alpha = m_pMouseBehavior->m_pOrbiter->FindObject(m_pMouseHandler->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoAlpha_CONST));
	if( m_pObj_MediaBrowser_Alpha && !m_pObj_MediaBrowser_Alpha->m_bOnScreen )
		m_pObj_MediaBrowser_Alpha = NULL;
}

void DatagridMouseHandlerHelper::Stop()
{
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
}

// todo -- delete this
bool DatagridMouseHandlerHelper::StayInGrid(int PK_Direction,int X,int Y)
{
	if( (m_pObj_ScrollingGrid->m_rPosition + m_pObj_ScrollingGrid->m_pPopupPoint).Contains(X,Y) )
	{
		if( !m_pObj_ScrollingGrid->DataGridTable_Get() )
			return true;  // Must be in the middle of rendering

		// We're still inside the grid, just highlight whatever cell is underneath us, figure that out
		int Row_Before = m_pObj_ScrollingGrid->m_iHighlightedRow;
		int Column_Before = m_pObj_ScrollingGrid->m_iHighlightedColumn;

		int DeltaY=0,DeltaX=0;
		if ( m_pObj_ScrollingGrid->m_FirstRowHeight > 0 && ( m_pObj_ScrollingGrid->m_GridCurRow == 0 || m_pObj_ScrollingGrid->m_bKeepRowHeader )  )
			DeltaY = m_pObj_ScrollingGrid->m_FirstRowHeight - m_pObj_ScrollingGrid->m_FixedRowHeight;
		if ( m_pObj_ScrollingGrid->m_FirstColumnWidth > 0 && ( m_pObj_ScrollingGrid->m_GridCurCol == 0 || m_pObj_ScrollingGrid->m_bKeepColHeader )  )
			DeltaX = m_pObj_ScrollingGrid->m_FirstColumnWidth - m_pObj_ScrollingGrid->m_FixedColumnWidth;

		int X2 = X - m_pObj_ScrollingGrid->m_rPosition.X - m_pObj_ScrollingGrid->m_pPopupPoint.X - DeltaX;
		int Y2 = Y - m_pObj_ScrollingGrid->m_rPosition.Y - m_pObj_ScrollingGrid->m_pPopupPoint.Y - DeltaY;
		
		int Row = Y2 / (m_pObj_ScrollingGrid->m_FixedRowHeight);
		int Column = X2 / (m_pObj_ScrollingGrid->m_FixedColumnWidth);
		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 )
			m_pObj_ScrollingGrid->m_iHighlightedRow = Row;
		if( m_pObj_ScrollingGrid->m_iHighlightedColumn!=-1 )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = Column;

		if( m_pObj_ScrollingGrid->m_iHighlightedRow > m_pObj_ScrollingGrid->m_MaxRow - 1 )
			m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->m_MaxRow - 1;
		if( m_pObj_ScrollingGrid->m_iHighlightedColumn > m_pObj_ScrollingGrid->m_MaxCol - 1 )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = m_pObj_ScrollingGrid->m_MaxCol - 1;

		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 && m_pObj_ScrollingGrid->m_GridCurRow + m_pObj_ScrollingGrid->m_iHighlightedRow >= m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() )
			m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() - m_pObj_ScrollingGrid->m_GridCurRow - 1;

		if( m_pObj_ScrollingGrid->m_iHighlightedColumn!=-1 && m_pObj_ScrollingGrid->m_GridCurCol + m_pObj_ScrollingGrid->m_iHighlightedColumn >= m_pObj_ScrollingGrid->DataGridTable_Get()->GetCols() )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = m_pObj_ScrollingGrid->DataGridTable_Get()->GetCols() - m_pObj_ScrollingGrid->m_GridCurCol - 1;
g_pPlutoLogger->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::StayInGrid %d-%d,%d  old row:%d col %d, now row:%d,col:%d",PK_Direction,X,Y,Row_Before,Column_Before,Row,Column);
if( Row_Before==Row && Column_Before==Column )
int k=2;
		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=Row_Before || m_pObj_ScrollingGrid->m_iHighlightedColumn!=Column_Before )
		{
			m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_ScrollingGrid);

//m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction);
			m_pMouseBehavior->m_pOrbiter->Renderer()->RedrawObjects();  // We may have scrolled past the end of a grid and need to re-render
		}
		return true;
	}

	if( (PK_Direction==DIRECTION_Up_CONST || PK_Direction==DIRECTION_Down_CONST) &&
		MovedPastTopBottomOfDataGrid((DesignObj_DataGrid *) m_pObj_ScrollingGrid,PK_Direction,Y) )
			return true; // We're now iterator mode
	return false;
}
		
// todo -- delete this
bool DatagridMouseHandlerHelper::MovedPastTopBottomOfDataGrid(DesignObj_DataGrid *pObj,int PK_Direction,int Y)
{
	// Moving past the bottom causes it to start the iterator and begin first scrolling
	// at up to 5 speeds, and then paging at up to 5 speeds by continually moving in the same direction.  As
	// soon as the direction changes, or the user moves back to the top, reset everything.
	if( PK_Direction==DIRECTION_Up_CONST )
	{ 
		if( Y >=pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y || pObj->m_GridCurRow==0 )
			return false;  // We've still got room to go up
		m_dwPK_Direction_ScrollGrid = DIRECTION_Up_CONST;
		pObj->m_iHighlightedRow=0;

		// Move to the bottom of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Bottom() + pObj->m_pPopupPoint.Y );
	}
	else
	{
		// Down is trickier since the last visible cell may not extend all the way to the bottom (the top cell is always the top)
		// So get the coorindates of the last visible cell
		PlutoRectangle r;
		pObj->GetGridCellDimensions(
			1, // col span
			1, // row span
			0, // column
			pObj->m_MaxRow - 1 /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
			r.X,  r.Y,  r.Width,  r.Height );

		r = r + pObj->m_pPopupPoint;

		if( Y<r.Bottom() )
			return false;

		m_dwPK_Direction_ScrollGrid = DIRECTION_Down_CONST;
		pObj->m_iHighlightedRow=pObj->m_MaxRow-1;
		if( pObj->m_iHighlightedRow!=-1 && pObj->m_GridCurRow + pObj->m_iHighlightedRow >= pObj->DataGridTable_Get()->GetRows() )
			pObj->m_iHighlightedRow = pObj->DataGridTable_Get()->GetRows() - pObj->m_GridCurRow - 1;

		// Move to the top of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y );
	}

	m_pObj_ScrollingGrid = (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
	m_pMouseHandler->m_iLastNotch = -999; // Start with no movement

	return true;
}

void DatagridMouseHandlerHelper::ScrollGrid(int dwPK_Direction,int X,int Y)
{
	if( dwPK_Direction!=m_dwPK_Direction_ScrollGrid)
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid direction changed from %d to %d",
					  m_dwPK_Direction_ScrollGrid,dwPK_Direction);
		// We moved past the top
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid

		if( m_dwPK_Direction_ScrollGrid==DIRECTION_Up_CONST )
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , m_pObj_ScrollingGrid->m_rPosition.Y + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		else
		{
			if( m_pObj_ScrollingGrid->DataGridTable_Get() && m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 && m_pObj_ScrollingGrid->m_GridCurRow + m_pObj_ScrollingGrid->m_iHighlightedRow >= m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() )
				m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() - m_pObj_ScrollingGrid->m_GridCurRow - 1;

			PlutoRectangle r;
			m_pObj_ScrollingGrid->GetGridCellDimensions(
				1, // col span
				1, // row span
				0, // column
				m_pObj_ScrollingGrid->m_iHighlightedRow /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
				r.X,  r.Y,  r.Width,  r.Height );

			// Move back to the bottom of the grid, since we're done with the iterator
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , r.Bottom() + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		}

		// Unhighlight the speed
		m_dwPK_Direction_ScrollGrid = 0;
		m_pMouseHandler->m_iLastNotch = -999;

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

	if( Notch==0 )
	{
		m_pMouseHandler->m_iLastNotch=Notch;
		return;
	}

	if( Notch!=m_pMouseHandler->m_iLastNotch )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid Notch %d",Notch);
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "scroll" );
		m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_ScrollingGrid);
		// We know the notch can't go in reverse since that would stop the scrolling
		if( Notch<=5 )
		{
			if( dwPK_Direction==DIRECTION_Up_CONST )
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Up_CONST);
			else
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Down_CONST);

			m_pMouseHandler->m_iLastNotch=Notch;
			return;
		}

		int Frequency;
		bool bPage=false;
		Frequency = (11-Notch)*100;
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,dwPK_Direction==DIRECTION_Down_CONST ? 2 : -2,"",Frequency,NULL);
		m_pMouseHandler->m_iLastNotch=Notch;
	}
}


void DatagridMouseHandlerHelper::RelativeMove(int X, int Y)
{
	m_LastX=X;
    g_pPlutoLogger->Write(LV_WARNING, "DatagridMouseHandlerHelper::RelativeMove(%d, %d, %d)", X, Y, m_iLastNotch);
	int Notch;
	if( m_eCapturingOffscreenMovement == cosm_UP )
	{
#ifdef WIN32
		if( Y >= m_Bottom-6 )
#else
		if( Y >= m_Bottom )
#endif
		{
			ReleaseRelative();
			return;
		}
		Notch = (m_pMouseBehavior->m_pOrbiter->m_Height - Y) / (m_pMouseBehavior->m_pOrbiter->m_Height/5) +1;
		if( Notch==m_iLastNotch )
			return;

        RelativePointer_ImageLoad(-Notch);
        if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,0,m_pRelativePointer_Image->Width, m_pRelativePointer_Image->Height);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}
	else
	{
#ifdef WIN32
		if (Y <=m_Top+6 )  
#else
		if (Y <=m_Top )  
#endif
		{
			ReleaseRelative();
			return;
		}
		Notch = Y / (m_pMouseBehavior->m_pOrbiter->m_Height/5) +1;
		if( Notch==m_iLastNotch )
			return;

        RelativePointer_ImageLoad(Notch);
		if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,m_pMouseBehavior->m_pOrbiter->m_Height-m_pRelativePointer_Image->Height,m_pRelativePointer_Image->Width, m_pRelativePointer_Image->Height);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}

	m_iLastNotch=Notch;

	int Frequency;
	Frequency = (6-Notch)*400;
g_pPlutoLogger->Write(LV_ACTION, "Frequency: %d Y: %d notch %d",Frequency,Y,Notch);
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaGrid,0,"",Frequency,this->m_pMouseHandler);
	#ifdef ORBITER_OPENGL
		if( m_pObj_ScrollingGrid )
			(dynamic_cast<DataGridRenderer_OpenGL *>(m_pObj_ScrollingGrid->Renderer()))->m_AnimationSpeed_set(int(Frequency*.5));
	#endif
}


void DatagridMouseHandlerHelper::ReleaseRelative()
{
g_pPlutoLogger->Write(LV_ACTION, "**stop**");
	m_iLastNotch=-1;
    g_pPlutoLogger->Write(LV_WARNING, "DatagridMouseHandlerHelper::ReleaseRelative()");
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
    RelativePointer_Clear();
#ifdef WIN32
	m_pMouseBehavior->SetMousePosition(m_LastX, m_eCapturingOffscreenMovement == cosm_DOWN ? m_Bottom-15 : m_Top+15);
#else
	m_pMouseBehavior->SetMousePosition(m_LastX, m_eCapturingOffscreenMovement == cosm_DOWN ? m_Bottom-1 : m_Top+1);
#endif
	m_pMouseBehavior->ShowMouse(true);
	m_eCapturingOffscreenMovement=cosm_NO;
}

bool DatagridMouseHandlerHelper::Move(int X,int Y,int PK_Direction)
{
	if (m_eCapturingOffscreenMovement != cosm_NO)
	{
		RelativeMove(X,Y);
		return true;
	}
	
	// For testing under windows add extra padding of 6 pixels on the top and bottom since it's usually not a full screen app
#ifdef WIN32
	if ( (Y <= m_Top+6 && m_pObj_ScrollingGrid->m_GridCurRow>0) || (Y >= m_Bottom-6 && !m_bHitBottom) )
#else
	if ( (Y <= m_Top && m_pObj_ScrollingGrid->m_GridCurRow>0) || (Y >= m_Bottom && !m_bHitBottom) )
#endif
	{
g_pPlutoLogger->Write(LV_ACTION, "**go**");
#ifdef WIN32
		if( Y <= m_Top+6 ) 
#else
		if( Y <= m_Top ) 
#endif
		{
			m_eCapturingOffscreenMovement = cosm_UP;
#ifdef WIN32
			m_pMouseBehavior->SetMousePosition(X,m_Bottom-15); 
#else
			m_pMouseBehavior->SetMousePosition(X,m_Bottom-1); 
#endif
			m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");

		}
		else
		{
			m_eCapturingOffscreenMovement = cosm_DOWN;
#ifdef WIN32
			m_pMouseBehavior->SetMousePosition(X,m_Top+15);
#else
			m_pMouseBehavior->SetMousePosition(X,m_Top+1);
#endif
			m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
		}

#ifndef WIN32
		m_pMouseBehavior->ShowMouse(false);
#endif
		return true;
	}
	return false;
}

bool DatagridMouseHandlerHelper::DoIteration()
{
	if(NULL == m_pMouseBehavior)
	{
		g_pPlutoLogger("m_pMouseBehavior is NULL! Race condition!?");
		return true;
	}
	m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
g_pPlutoLogger->Write(LV_ACTION,"********SCROLL  --  START***");
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
	bool bResult;

	if( m_eCapturingOffscreenMovement==cosm_UP )
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ScrollingGrid->m_ObjectID,DIRECTION_Up_CONST,false);
		m_bHitBottom=false;
	}
	else
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ScrollingGrid->m_ObjectID,DIRECTION_Down_CONST,false);
		if( !bResult )
			m_bHitBottom=true;
	}

	if( !bResult )
	{
		ReleaseRelative();
		return false;
	}
	// For the first notch, scroll only 1 time
	if( m_iLastNotch==1 )
		return false;

	return bResult;
}

bool DatagridMouseHandlerHelper::RelativePointer_SetStatus(int nSpeedShape)
{
    g_pPlutoLogger->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_SetStatus(%d)", nSpeedShape);
    nSpeedShape = RelativePointer_AdjustSpeedShape(nSpeedShape);
    // check range
    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;

    // load image from disk
    RelativePointer_ImageLoad(nSpeedShape);
    
    // get current screen size
    PlutoSize oSizeScreen = m_pMouseBehavior->m_pOrbiter->m_sScreenSize;
    // get current mouse position
    PlutoPoint posFakeMouse;
    m_pMouseBehavior->GetMousePosition(&posFakeMouse);
    // pointer move to the middle of the screen, on Y axis only
    m_pMouseBehavior->SetMousePosition(posFakeMouse.X, oSizeScreen.Height/2);

    //delete this debug-only block
    // with a real pointer, not a fake one
    if (0)
    {
        string sPathMask = m_sImagePath + ".msk";
        m_pMouseBehavior->SetMouseCursorImage(m_sImagePath, sPathMask);
        if (nSpeedShape > 0)
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y - 2);
        else
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y + 2);
    }

    // draw fake pointer
	if( m_pRelativePointer_Image )
	{
	    PlutoRectangle rectFakePointer = RelativePointer_ComputeRectangle(posFakeMouse.X, posFakeMouse.Y, oSizeScreen.Width, oSizeScreen.Height);
        RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
	}
	return true;
}

void DatagridMouseHandlerHelper::RelativePointer_Clear()
{
    m_nRelativePointer_SpeedShape = 0;
    if (m_pRelativePointer_Image)
    {
        delete m_pRelativePointer_Image;
        m_pRelativePointer_Image = NULL;
    }
    RelativePointer_ImageRemove();
}

int DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(int nSpeedShape)
{
    // check range
    const int nPeakSpeed = 5;
    if ( (nSpeedShape > 0) && (nSpeedShape > nPeakSpeed) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(%d) : decreasing shape to %d", nSpeedShape, nPeakSpeed);
        nSpeedShape = nPeakSpeed;
    }
    if ( (nSpeedShape < 0) && (nSpeedShape < nPeakSpeed*-1) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(%d) : increasing shape to %d", nSpeedShape, -nPeakSpeed);
        nSpeedShape = -nPeakSpeed;
    }
    return nSpeedShape;
}

bool DatagridMouseHandlerHelper::RelativePointer_ImageLoad(int nSpeedShape)
{
    g_pPlutoLogger->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d)", nSpeedShape);
    nSpeedShape = RelativePointer_AdjustSpeedShape(nSpeedShape);

    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;

    // restore the normal behavior in this case
    if (nSpeedShape == 0)
    {
        RelativePointer_Clear();
        return false;
    }
    // compute the image path
    char buffer[100];
    sprintf(buffer, "speed_shape_%d.png", nSpeedShape);
    m_sImagePath = "/usr/pluto/orbiter/skins/Basic/cursors/pointers/";
    m_sImagePath = m_sImagePath + buffer;

    // read the image from disk
	size_t iSize = 0;
    char * pData = FileUtils::ReadFileIntoBuffer(m_sImagePath.c_str(), iSize);
    if (pData == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d) : cannot load graphic file %s", nSpeedShape, m_sImagePath.c_str());
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d) : loaded graphic file %s", nSpeedShape, m_sImagePath.c_str());
    m_pRelativePointer_Image = m_pMouseBehavior->m_pOrbiter->m_pOrbiterRenderer->CreateGraphic();
    m_pRelativePointer_Image->LoadGraphic(pData, iSize);
    delete [] pData;
    pData = NULL;
    return true;
}

void DatagridMouseHandlerHelper::RelativePointer_ImageDraw(PlutoGraphic *pImage, const PlutoRectangle &rectFakePointer)
{
#ifdef ORBITER_OPENGL
	g_pPlutoLogger->Write(LV_ACTION,"DatagridMouseHandlerHelper::RelativePointer_ImageDraw");
    OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pMouseBehavior->m_pOrbiter->Renderer());
    if (pOrbiterRenderer_OpenGL)
    {
        // opengl mode
        // reusing the same image id
        pOrbiterRenderer_OpenGL->RenderGraphic(pImage, rectFakePointer, false, PlutoPoint(), 255,
			"", "image_fake_pointer");
		pOrbiterRenderer_OpenGL->Engine->AddTopMostObject("image_fake_pointer");
        return;
    }
#endif
    // sdl mode, not tested
    m_pMouseBehavior->m_pOrbiter->Renderer()->RenderGraphic(pImage, rectFakePointer);
}

void DatagridMouseHandlerHelper::RelativePointer_ImageRemove()
{
#ifdef ORBITER_OPENGL
	g_pPlutoLogger->Write(LV_ACTION,"DatagridMouseHandlerHelper::RelativePointer_ImageRemove");
    OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pMouseBehavior->m_pOrbiter->Renderer());
    if (pOrbiterRenderer_OpenGL)
    {
        pOrbiterRenderer_OpenGL->RemoveGraphic("image_fake_pointer");
        return;
    }
#endif
    // sdl mode, no method implemented yet in sdl
}

PlutoRectangle DatagridMouseHandlerHelper::RelativePointer_ComputeRectangle(int X, int Y, int screenWidth, int screenHeight)
{
    PlutoRectangle rectFakePointer;
    if (m_pRelativePointer_Image == NULL)
        return rectFakePointer;
    //compute fake image position
    rectFakePointer.Width = m_pRelativePointer_Image->Width;
    rectFakePointer.Height = m_pRelativePointer_Image->Height;
    // X coordinate : keep the pointer on the screen
    rectFakePointer.X = X;
    if (rectFakePointer.X + rectFakePointer.Width > screenWidth)
        rectFakePointer.X = screenWidth - rectFakePointer.Width;
    // Y coordinate
    if (m_nRelativePointer_SpeedShape > 0)
        rectFakePointer.Y = screenHeight - rectFakePointer.Height;
    else
        rectFakePointer.Y = 0;
    return rectFakePointer;
}
