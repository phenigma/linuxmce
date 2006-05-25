#ifndef DESIGNOBJ_Orbiter_H
#define DESIGNOBJ_Orbiter_H

#include "DesignObj_Data.h"
#include "PlutoGraphic.h"
#include "PlutoUtils/GraphicFormat.h"
#include "../SerializeClass/ShapesColors.h"
//-------------------------------------------------------------------------------------------------------
namespace DCE
{
	class Orbiter;
	class Logger;
	class DataGridTable;
	class PlutoPopup;
}
//-------------------------------------------------------------------------------------------------------
using namespace DCE;

class PlutoGraphic;
class DesignObj_Orbiter;
//-------------------------------------------------------------------------------------------------------
typedef map<string, DesignObj_Orbiter*> DesignObj_OrbiterMap;
typedef vector<DesignObj_Orbiter *> VectDesignObj_Orbiter;
//=======================================================================================================
//Concrete class DesignObj_Orbiter
//-------------------------------------------------------------------------------------------------------
class DesignObj_Orbiter : public DesignObj_Data
{
public:
	DesignObj_Orbiter();
	virtual ~DesignObj_Orbiter();

	virtual bool ObjectInRect(PlutoRectangle rect);
	virtual void CalculateSize(PlutoSize &size) { };
	virtual void RenderObject( DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0), int Layer=-1);
	virtual void RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point);

	class DesignObj_Orbiter *m_pDesignObj_Orbiter_Up,*m_pDesignObj_Orbiter_Down,*m_pDesignObj_Orbiter_Left,*m_pDesignObj_Orbiter_Right,*m_pDesignObj_Orbiter_TiedTo;
	int m_iColumnTiedTo,m_iRowTiedTo;  // this only applies if m_pDesignObj_Orbiter_TiedTo points to a datagrid, and this object is tied to a specific row in the grid

	bool m_bOneTimeDontReset;
	bool m_bIsBoundToUser,m_bIsBoundToLocation,m_bIsARemoteControl; // Redundant, but saves time from looking this up each time
	bool m_bOnScreen,m_bDisableAspectLock,m_bContainsDataGrid;
	DataGridTable *m_pDataGridTable;
	class WebWindow *m_pWebWindow;
	class FloorplanObject *m_pFloorplanObject;
// todo: maybe datagrid related stuff should get moved into its own class to save memory.
	int m_GridCurRow, m_GridCurCol;  // The current top, left grid and column being displayed.  Zero based.  Example: In a grid with 10 rows per page, on the second page m_GridCurRow will = 10
	int m_MaxRow, m_MaxCol; // The total number of rows and columns visible on the screen.  Not related to the actual rows and columns in the grid.  
	int m_FixedRowHeight, m_FixedColumnWidth, m_RowSpacing;
	int m_ColumnSpacing, m_FirstRowHeight, m_FirstColumnWidth;
	int m_iRepeatParm;  // A temporary value used to track how many times this button has repeated

	vector<DesignObj_Orbiter *> m_vectObj_TabStops;  // If this is a screen, this is all the tab stops
    list<class PlutoPopup*> m_listPopups; // If this is a screen, these are the local popups
	DesignObj_DataList m_OverlayObjects;

	int m_GraphicToDisplay;
	int m_GraphicToPlay;

	// m_pGraphicToUndoSelect is a temporary snapshot of the background that may be created during a select or restore
	// to return it to it's previous state
	PlutoGraphic *m_pGraphicToUndoSelect;
	VectorPlutoGraphic *m_pvectCurrentGraphic, *m_pvectCurrentPlayingGraphic;
	VectorPlutoGraphic m_vectGraphic,m_vectSelectedGraphic,m_vectHighlightedGraphic;
	vector<VectorPlutoGraphic> m_vectAltGraphics;
	int m_iTime_Background,m_iTime_Selected,m_iTime_Highlighted;
	vector<int> m_ivectTime_Alt;
	bool m_bLoop_Background,m_bLoop_Selected,m_bLoop_Highlighted;
	vector<bool> m_bvectLoop_Alt;
	int m_iCurrentFrame;
	DesignObj_Orbiter *m_pObj_Screen;
	PlutoPoint m_pMidPoint,m_pPopupPoint;
	PlutoRectangle GetHighlightRegion();

	class Orbiter_CriteriaList *m_pCriteria;

	bool IsHidden() { if( m_bHidden ) return true;  if( m_pParentObject ) return ((DesignObj_Orbiter *) m_pParentObject)->IsHidden(); return false; }

	// Runtime states
	bool m_bActive, m_bHidden;

	// Pre-parsed object paramters

	// To avoid looking this up while iterating through each cell
	// we pre-parse it at start up.

#define MAX_ALT_COLORS 15
	int m_iAltColorValues[MAX_ALT_COLORS];

	string GetParameterValue(int ParameterID);
};

#define LAYER_PREDRAW 0
#define LAYER_NORMAL 2
#define LAYER_POSTDRAW 4

#endif
