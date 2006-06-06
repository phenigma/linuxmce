#ifndef DESIGNOBJ_Orbiter_H
#define DESIGNOBJ_Orbiter_H

#include "DesignObj_Data.h"
#include "PlutoGraphic.h"
#include "PlutoUtils/GraphicFormat.h"
#include "ObjectRendererFactory.h"

#include "Gen_Devices/AllCommandsRequests.h"
//-------------------------------------------------------------------------------------------------------
class ProntoCCF;
namespace DCE
{
	class Orbiter;
	class Logger;
	class DataGridTable;
	class PlutoPopup;
}
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
class PlutoGraphic;
class DesignObj_Orbiter;
class ObjectRenderer;
//-------------------------------------------------------------------------------------------------------
typedef map<string, DesignObj_Orbiter*> DesignObj_OrbiterMap;
typedef list<PlutoGraphic*> PlutoGraphicList;
typedef vector<DesignObj_Orbiter *> VectDesignObj_Orbiter;
typedef vector<PlutoGraphic *> VectorPlutoGraphic;
//=======================================================================================================
//Concrete class DesignObj_Orbiter
//-------------------------------------------------------------------------------------------------------
class DesignObj_Orbiter : public DesignObj_Data
{
public:
	DesignObj_Orbiter(class Orbiter *pCore);
	virtual ~DesignObj_Orbiter();

	class DesignObj_Orbiter *m_pDesignObj_Orbiter_Up,*m_pDesignObj_Orbiter_Down,*m_pDesignObj_Orbiter_Left,*m_pDesignObj_Orbiter_Right,*m_pDesignObj_Orbiter_TiedTo;
	int m_iColumnTiedTo,m_iRowTiedTo;  // this only applies if m_pDesignObj_Orbiter_TiedTo points to a datagrid, and this object is tied to a specific row in the grid

	class Orbiter *m_pOrbiter;
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
	ProntoCCF *m_pCCF;

	bool IsHidden();

	virtual void RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));
	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));

	string GetArrayValue(); // If this is an item in an array, it will return the id of the array
	// Runtime states
	bool m_bActive, m_bHidden;

	// Pre-parsed object paramters

	// To avoid looking this up while iterating through each cell
	// we pre-parse it at start up.

#define MAX_ALT_COLORS 15
	int m_iAltColorValues[MAX_ALT_COLORS];

	string GetParameterValue(int ParameterID);
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);

protected:

	virtual bool PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual bool PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));

	ObjectRenderer *m_pObjectRenderer;
};
//=======================================================================================================
//Concrete class CHAEffect
//-------------------------------------------------------------------------------------------------------
class CHAEffect
{
public:
	void *m_pOriginalBackground;
	clock_t m_NextFrameTime;
	int m_FrameCount,m_XOffset,m_YOffset;
	DesignObj_Orbiter *m_pObj;

	CHAEffect(DesignObj_Orbiter *pObj,int XOffset,int YOffset)
	{
		m_pOriginalBackground = NULL;
		m_XOffset=XOffset;
		m_YOffset=YOffset;
		m_FrameCount = 0;
		m_pObj = pObj;
	}	
	virtual ~CHAEffect()
	{
	}

	virtual void UpdateEffect(bool bRedrawFrame = false)=0;
};
//-------------------------------------------------------------------------------------------------------
typedef list<CHAEffect *> CHAEffectList;

#define CACHE_SIZE 5 // The maximum number of directions
//-------------------------------------------------------------------------------------------------------
// Eventually it might be good to be able to ask the server to cache a fairly signficant amount of the grid
class ProntoCCF;
//-------------------------------------------------------------------------------------------------------
#endif
