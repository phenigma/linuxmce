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
#include "DataGridRenderer.h"

#ifdef ORBITER_OPENGL
#include "OpenGL/OrbiterRenderer_OpenGL.h"
#endif

using namespace DCE;

MediaBrowserMouseHandler::MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
    , m_nRelativePointer_SpeedShape(0)
    , m_pRelativePointer_Image(NULL)
{
	m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	m_pObj_CoverArtPopup=NULL;
	m_eCapturingOffscreenMovement=cosm_NO;

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
	m_pObj_CoverArtPopup=m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_popCoverArt_CONST);
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
}

MediaBrowserMouseHandler::~MediaBrowserMouseHandler()
{
    RelativePointer_Clear();
}

void MediaBrowserMouseHandler::Start()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Start()");
	m_iLastNotch=m_LastRow=-1;
}

void MediaBrowserMouseHandler::Stop()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Stop()");
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
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

void MediaBrowserMouseHandler::RelativeMove(int X, int Y)
{
	m_LastX=X;
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativeMove(%d, %d, %d)", X, Y, m_iLastNotch);
	int Notch;
	if( m_eCapturingOffscreenMovement == cosm_UP )
	{
		if( Y >= m_pMouseBehavior->m_pOrbiter->m_Height-10 )  // Should be 2
		{
			ReleaseRelative(X,Y);
			return;
		}
		Notch = (m_pMouseBehavior->m_pOrbiter->m_Height - Y) / (m_pMouseBehavior->m_pOrbiter->m_Height/10);
		if( Notch==m_iLastNotch )
			return;
		//m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
		//	X-40,0,
		//	80,5,
		//	PlutoColor::White());
		//m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
		//	X-Notch*2,0,
		//	Notch*2,5,
		//	PlutoColor::Green());
        RelativePointer_ImageLoad(-Notch);
        if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,0,m_pRelativePointer_Image->Width, m_pRelativePointer_Image->Height);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}
	else
	{
		if (Y <= 10 && m_eCapturingOffscreenMovement == cosm_DOWN)  // Should be 1
		{
			ReleaseRelative(X,Y);
			return;
		}
		Notch = Y / (m_pMouseBehavior->m_pOrbiter->m_Height/10);
		//m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
		//	X-40,m_pMouseBehavior->m_pOrbiter->m_Height-5,
		//	80,5,
		//	PlutoColor::White());
		//m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
		//	X-Notch*2,m_pMouseBehavior->m_pOrbiter->m_Height-5,
		//	Notch*2,5,
		//	PlutoColor::Green());
        RelativePointer_ImageLoad(Notch);
		if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,m_pMouseBehavior->m_pOrbiter->m_Height-m_pRelativePointer_Image->Height,m_pRelativePointer_Image->Width, m_pRelativePointer_Image->Height);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}

	m_iLastNotch=Notch;
	if( Notch==0 )
	{
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
		return;
	}

	int Frequency;
	Frequency = (11-Notch)*200;
g_pPlutoLogger->Write(LV_ACTION, "Frequency: %d Y: %d",Frequency,Y);
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaGrid,0,"",Frequency,this);
}


void MediaBrowserMouseHandler::ReleaseRelative(int X, int Y)
{
g_pPlutoLogger->Write(LV_ACTION, "**stop**");
	m_iLastNotch=-1;
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::ReleaseRelative()");
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
	m_pMouseBehavior->SetMousePosition(X, m_eCapturingOffscreenMovement == cosm_DOWN ? m_pMouseBehavior->m_pOrbiter->m_Height-20 : 20); // Should be 2 (-2) and 1
	m_pMouseBehavior->ShowMouse(true);
	m_eCapturingOffscreenMovement=cosm_NO;
}

void MediaBrowserMouseHandler::Move(int X,int Y,int PK_Direction)
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Move(%d, %d, %d)", X, Y, PK_Direction);

	if( m_pMouseBehavior->m_bMouseConstrained )  // There's a popup grabbing the mouse
		return;
	if( !m_pObj_ListGrid || !m_pObj_PicGrid )
		return;  // Shouldn't happen

    if (m_eCapturingOffscreenMovement != cosm_NO)
	{
		RelativeMove(X,Y);
		return;
	}
	if (Y <= 10 || Y >= m_pMouseBehavior->m_pOrbiter->m_Height-10)  // Should be 1 and 2
	{
g_pPlutoLogger->Write(LV_ACTION, "**go**");
		if( Y <= 10 ) // should be 1
		{
			m_eCapturingOffscreenMovement = cosm_UP;
			m_pMouseBehavior->SetMousePosition(X,m_pMouseBehavior->m_pOrbiter->m_Height-20);  // Temp should be 2
		}
		else
		{
			m_eCapturingOffscreenMovement = cosm_DOWN;
			m_pMouseBehavior->SetMousePosition(X,20);   // Temp, should be 2
		}

		m_pMouseBehavior->ShowMouse(false);
		return;

	}

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

	if( m_pObj_ListGrid->m_iHighlightedRow!=m_LastRow )
	{
		m_LastRow=m_pObj_ListGrid->m_iHighlightedRow;
		// This takes care of the list grid
		ShowCoverArtPopup();
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted = m_pObj_ListGrid;
		m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
	}
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
	DataGridTable *pDataGridTable = m_pObj_PicGrid->DataGridTable_Get();
	if( pDataGridTable )
		pCell = pDataGridTable->GetData(m_pObj_PicGrid->m_iHighlightedColumn + m_pObj_PicGrid->m_GridCurCol,m_pObj_PicGrid->m_iHighlightedRow + m_pObj_PicGrid->m_GridCurRow );

	PLUTO_SAFETY_LOCK(M, m_pMouseBehavior->m_pOrbiter->m_VariableMutex );
	if( pCell && pCell->m_pGraphic && pCell->m_pGraphic->m_pGraphicData)
	{
		char *pDup = new char[pCell->m_pGraphic->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphic->m_pGraphicData,pCell->m_pGraphic->m_GraphicLength);
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_pGraphic->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);
	}
	else if( pCell && pCell->m_pGraphic )
	{
		char *pDup = new char[pCell->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphicData,pCell->m_GraphicLength);
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);
	}
	else
	{
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
	}
}

bool MediaBrowserMouseHandler::DoIteration()
{
g_pPlutoLogger->Write(LV_ACTION,"********SCROLL  --  START***");
	m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
	bool bResult;

	if( m_eCapturingOffscreenMovement==cosm_UP )
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ListGrid->m_ObjectID,DIRECTION_Up_CONST,false);
		bResult = bResult && m_pMouseBehavior->m_pOrbiter->Scroll_Grid("", m_pObj_PicGrid->m_ObjectID,DIRECTION_Up_CONST,false);
	}
	else
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ListGrid->m_ObjectID,DIRECTION_Down_CONST,false);
		bResult = bResult && m_pMouseBehavior->m_pOrbiter->Scroll_Grid("", m_pObj_PicGrid->m_ObjectID,DIRECTION_Down_CONST,false);
	}

	if( !bResult )
	{
		m_pMouseBehavior->SetMousePosition(m_LastX, m_eCapturingOffscreenMovement == cosm_DOWN ? m_pMouseBehavior->m_pOrbiter->m_Height-20 : 20); // Should be 2 (-2) and 1
		m_pMouseBehavior->ShowMouse(true);
		m_eCapturingOffscreenMovement=cosm_NO;
		return false;
	}

	m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_PicGrid);
/*
	// Now sync the pic grid
	PLUTO_SAFETY_LOCK( dg, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
	DataGridRenderer *pDataGridRenderer = (DataGridRenderer *) m_pObj_PicGrid->Renderer();
	PLUTO_SAFETY_LOCK( dgc, pDataGridRenderer->m_DataGridCacheMutex );
	m_pObj_PicGrid->m_GridCurRow = m_pObj_ListGrid->m_GridCurRow / m_pObj_PicGrid->m_MaxCol;
	if( m_eCapturingOffscreenMovement==cosm_UP )
	{
		if( m_pObj_PicGrid->m_iCacheRows > 0) // Are we caching? 
		{                            // If so the current table needs to be pushed onto the opposing cache list.
			pDataGridRenderer->m_listDataGridCache[DIRECTION_Up_CONST].push_front(m_pObj_PicGrid->DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
		}
		else 
		{
//			delete m_pObj_PicGrid->DataGridTable_Get();
		}	

		if (pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].size() > 0)
		{
			m_pObj_PicGrid->DataGridTable_Get() = pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].front();
			pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].pop_front();
		}
		else
		{
			m_pObj_PicGrid->DataGridTable_Get() = NULL;
		}
	}
	else
	{
		if (m_pObj_PicGrid->m_iCacheRows > 0) // Are we caching? 
		{                            // If so the current table needs to be pushed onto the opposing cache list.
			pDataGridRenderer->m_listDataGridCache[DIRECTION_Up_CONST].push_front(m_pObj_PicGrid->DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
		}
		else 
		{
			//delete m_pObj_PicGrid->DataGridTable_Get();
		}	

		if (pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].size() > 0)
		{
			m_pObj_PicGrid->DataGridTable_Get() = pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].front();
			pDataGridRenderer->m_listDataGridCache[DIRECTION_Down_CONST].pop_front();
		}
		else
		{
			m_pObj_PicGrid->DataGridTable_Get() = NULL;
		}
	}
g_pPlutoLogger->Write(LV_ACTION,"********SCROLL  --  STOP***");
*/
	return bResult;
}

bool MediaBrowserMouseHandler::RelativePointer_SetStatus(int nSpeedShape)
{
    g_pPlutoLogger->Write(LV_STATUS, "MediaBrowserMouseHandler::RelativePointer_SetStatus(%d)", nSpeedShape);
    nSpeedShape = RelativePointer_AdjustSpeedShape(nSpeedShape);
    // check range
    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;

    // load image from disk
    RelativePointer_ImageLoad(nSpeedShape);
    
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

void MediaBrowserMouseHandler::RelativePointer_Clear()
{
    m_nRelativePointer_SpeedShape = 0;
    if (m_pRelativePointer_Image)
    {
        delete m_pRelativePointer_Image;
        m_pRelativePointer_Image = NULL;
    }
    RelativePointer_ImageRemove();
    //m_pMouseBehavior->ShowMouse(true);
}

int MediaBrowserMouseHandler::RelativePointer_AdjustSpeedShape(int nSpeedShape)
{
    // check range
    const int nPeakSpeed = 5;
    if ( (nSpeedShape > 0) && (nSpeedShape > nPeakSpeed) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativePointer_AdjustSpeedShape(%d) : decreasing shape to %d", nSpeedShape, nPeakSpeed);
        nSpeedShape = nPeakSpeed;
    }
    if ( (nSpeedShape < 0) && (nSpeedShape < nPeakSpeed) )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::RelativePointer_AdjustSpeedShape(%d) : increasing shape to %d", nSpeedShape, -nPeakSpeed);
        nSpeedShape = -nPeakSpeed;
    }
    return nSpeedShape;
}

bool MediaBrowserMouseHandler::RelativePointer_ImageLoad(int nSpeedShape)
{
    g_pPlutoLogger->Write(LV_STATUS, "MediaBrowserMouseHandler::RelativePointer_ImageLoad(%d)", nSpeedShape);
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
        g_pPlutoLogger->Write(LV_CRITICAL, "MediaBrowserMouseHandler::RelativePointer_ImageLoad(%d) : cannot load graphic file %s", nSpeedShape, m_sImagePath.c_str());
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS, "MediaBrowserMouseHandler::RelativePointer_ImageLoad(%d) : loaded graphic file %s", nSpeedShape, m_sImagePath.c_str());
    m_pRelativePointer_Image = m_pMouseBehavior->m_pOrbiter->m_pOrbiterRenderer->CreateGraphic();
    m_pRelativePointer_Image->LoadGraphic(pData, iSize);
    delete [] pData;
    pData = NULL;
    // hide the real pointer
    m_pMouseBehavior->ShowMouse(false);
    return true;
}

void MediaBrowserMouseHandler::RelativePointer_ImageDraw(PlutoGraphic *pImage, const PlutoRectangle &rectFakePointer)
{
#ifdef ORBITER_OPENGL
    OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pMouseBehavior->m_pOrbiter->Renderer());
    if (pOrbiterRenderer_OpenGL)
    {
        // opengl mode
        // reusing the same image id
        pOrbiterRenderer_OpenGL->RenderGraphic("", "image_fake_pointer", pImage, rectFakePointer);
        return;
    }
#endif
    // sdl mode, not tested
    m_pMouseBehavior->m_pOrbiter->Renderer()->RenderGraphic(pImage, rectFakePointer);
}

void MediaBrowserMouseHandler::RelativePointer_ImageRemove()
{
#ifdef ORBITER_OPENGL
    OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pMouseBehavior->m_pOrbiter->Renderer());
    if (pOrbiterRenderer_OpenGL)
    {
        pOrbiterRenderer_OpenGL->RemoveGraphic("image_fake_pointer");
        return;
    }
#endif
    // sdl mode, no method implemented yet in sdl
}

PlutoRectangle MediaBrowserMouseHandler::RelativePointer_ComputeRectangle(int X, int Y, int screenWidth, int screenHeight)
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
