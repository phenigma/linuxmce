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

using namespace DCE;

MediaBrowserMouseHandler::MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pDatagridMouseHandlerHelper = new DatagridMouseHandlerHelper(this);
	m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	m_pObj_FileDetailsText=m_pObj_CoverArtPopup=NULL;

	if( !m_pObj )
		return; // Shouldn't happen

	DesignObj_DataList::iterator iHao;

	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_dgFileList2_CONST )
			m_pObj_ListGrid = (DesignObj_DataGrid *) pDesignObj_Orbiter;
		else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_dgFileList2_Pics_CONST )
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
	m_pObj_FileDetailsText = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_butFileBrowserDetails_CONST);
	if( m_pObj_FileDetailsText && m_pObj_FileDetailsText->m_vectDesignObjText.size() )
		m_pObj_FileDetailsText->m_vectDesignObjText[0]->m_sText = "";
}

MediaBrowserMouseHandler::~MediaBrowserMouseHandler()
{
	delete m_pDatagridMouseHandlerHelper;
}

void MediaBrowserMouseHandler::Start()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Start()");
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
	m_LastRow=-1;
	m_pDatagridMouseHandlerHelper->Start(m_pObj_ListGrid,5,1,m_pMouseBehavior->m_pOrbiter->m_iImageHeight-1);
}

void MediaBrowserMouseHandler::Stop()
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Stop()");
	m_pDatagridMouseHandlerHelper->Stop();
}

bool MediaBrowserMouseHandler::ButtonDown(int PK_Button)
{
	if( m_pDatagridMouseHandlerHelper->CapturingMouse() )
		return true;
	if( m_pMouseBehavior->m_bMouseConstrained )
		return false;

	return false; // Keep processing
}

bool MediaBrowserMouseHandler::ButtonUp(int PK_Button)
{
	if( m_pMouseBehavior->m_bMouseConstrained )
		return false;
	if( PK_Button==BUTTON_Mouse_7_CONST && m_bTapAndRelease==false && m_bStartedMovement )
	{
//		return false;
	}

	return false; // Keep processing
}


void MediaBrowserMouseHandler::Move(int X,int Y,int PK_Direction)
{
    g_pPlutoLogger->Write(LV_WARNING, "MediaBrowserMouseHandler::Move(%d, %d, %d)", X, Y, PK_Direction);

	if( m_pMouseBehavior->m_bMouseConstrained )  // There's a popup grabbing the mouse
		return;
	if( !m_pObj_ListGrid || !m_pObj_PicGrid )
		return;  // Shouldn't happen

    if (m_pDatagridMouseHandlerHelper->Move(X,Y,PK_Direction))
		return;

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
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "Move on grid" );
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
		Y = m_pObj_PicGrid->m_rPosition.Y + (m_pObj_PicGrid->m_iHighlightedRow*m_pObj_PicGrid->m_FixedRowHeight) - ((m_pObj_CoverArtPopup->m_rPosition.Height - m_pObj_PicGrid->m_FirstRowHeight)/2);

	if( X + m_pObj_CoverArtPopup->m_rPosition.Width > m_pObj_PicGrid->m_rPosition.Right() )
		X = m_pObj_PicGrid->m_rPosition.Right() - m_pObj_CoverArtPopup->m_rPosition.Width;
	if( Y + m_pObj_CoverArtPopup->m_rPosition.Height > m_pObj_PicGrid->m_rPosition.Bottom() )
		Y = m_pObj_PicGrid->m_rPosition.Bottom() - m_pObj_CoverArtPopup->m_rPosition.Height;
	else if( Y < m_pObj_PicGrid->m_rPosition.Y )
		Y = m_pObj_PicGrid->m_rPosition.Y;

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

		delete [] pDup;
	}
	else if( pCell && pCell->m_pGraphicData )
	{
		char *pDup = new char[pCell->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphicData,pCell->m_GraphicLength);
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);

		delete [] pDup;
	}
	else
	{
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
	}

	if( m_pObj_FileDetailsText && m_pObj_FileDetailsText->m_vectDesignObjText.size() )
	{
		DataGridTable *pDataGridTable = m_pObj_ListGrid->DataGridTable_Get();
		if( pDataGridTable )
		{
			pCell = pDataGridTable->GetData(0,m_pObj_ListGrid->m_iHighlightedRow + m_pObj_ListGrid->m_GridCurRow );
			m_pObj_FileDetailsText->m_vectDesignObjText[0]->m_sText = pCell ? pCell->GetText() : "";
			m_pMouseBehavior->m_pOrbiter->Renderer()->RenderTextAsync(m_pObj_FileDetailsText->m_vectDesignObjText[0]);
		}
	}
}

