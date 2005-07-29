#ifndef DESIGNOBJ_Orbiter_H
#define DESIGNOBJ_Orbiter_H

#include "DesignObj_Data.h"
#include "PlutoUtils/GraphicFormat.h"
//-------------------------------------------------------------------------------------------------------
class ProntoCCF;
namespace DCE
{
	class Orbiter;
	class Logger;
	class DataGridTable;
	class PlutoPopup;
}
//-------------------------------------------------------------------------------------------------------
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
// Graphic memory management.  GR_DYNAMIC means that this is a dynamic image
// and there's no reason to ever keep it.  Discardonchange may keep the image
// if there's enough system memory. 
//-------------------------------------------------------------------------------------------------------
enum eGraphicManagement { GR_KEEPUNCOMPRESSED, GR_KEEPCOMPRESSED, GR_DISCARDONCHANGE, GR_DYNAMIC, GR_STORAGE };

// Windows-style RGB management
//-------------------------------------------------------------------------------------------------------
#ifndef RGB
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
	#define GetRValue(rgb)      ((BYTE)(rgb))
	#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
	#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#endif
//-------------------------------------------------------------------------------------------------------
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
//-------------------------------------------------------------------------------------------------------
enum GraphicType
{
	gtSDLGraphic,
	gtPocketFrogGraphic
};
//-------------------------------------------------------------------------------------------------------
class PlutoGraphic;
class DesignObj_Orbiter;
//-------------------------------------------------------------------------------------------------------
typedef map<string, DesignObj_Orbiter*> DesignObj_OrbiterMap;
typedef list<PlutoGraphic*> PlutoGraphicList;
typedef vector<DesignObj_Orbiter *> VectDesignObj_Orbiter;
typedef vector<PlutoGraphic *> VectorPlutoGraphic;

#define GRAPHIC_NORMAL		 0
#define GRAPHIC_SELECTED	-1
#define GRAPHIC_HIGHLIGHTED	-2
//=======================================================================================================
//Generic abstract class PlutoGraphic
//-------------------------------------------------------------------------------------------------------
class PlutoGraphic
{
protected :
	Orbiter *m_pOrbiter;
public:
	PlutoGraphic();
	PlutoGraphic(Orbiter *pOrbiter);
	PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pOrbiter);
	virtual ~PlutoGraphic();

	virtual void Initialize();
	virtual GraphicType GraphicType_get()=0;  // Must be implemented
	virtual bool IsEmpty()=0; //Must be implemented
	virtual bool LoadGraphic(char *pData, size_t iSize,int iRotation=0) = 0; //Must be implemented
	virtual void Clear() = 0; //Must be implemented

	enum eGraphicManagement m_GraphicManagement;
	enum eGraphicFormat m_GraphicFormat;

	int Width, Height;
	string m_Filename;
};
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

	class Orbiter *m_pCore;
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

	class Orbiter_CriteriaList *m_pCriteria;
	ProntoCCF *m_pCCF;

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

#define CACHE_UP 0
#define CACHE_DOWN 1
#define CACHE_LEFT 2
#define CACHE_RIGHT 3
#define CACHE_SIZE 4
//-------------------------------------------------------------------------------------------------------
// Eventually it might be good to be able to ask the server to cache a fairly signficant amount of the grid
class ProntoCCF;
//=======================================================================================================
//Concrete class DesignObj_DataGrid
//-------------------------------------------------------------------------------------------------------
class DesignObj_DataGrid : public DesignObj_Orbiter 
{
public:
	DesignObj_DataGrid(class Orbiter *pCore) : DesignObj_Orbiter(pCore)
	{
		for(int i=0;i<CACHE_SIZE;++i)
			m_pDataGridTableCache[i]=NULL;

		m_pObjLeft=m_pObjRight=m_pObjUp=m_pObjDown=NULL;
		bReAcquire=false;
		m_dwIDownRow=m_iUpRow=-1;
		m_iPopulatedWidth=m_iPopulatedHeight=0;
		m_bParsed=false;
	}
	virtual ~DesignObj_DataGrid(); 

	int m_dwIDownRow,m_iUpRow;  // These are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
	int m_iHighlightedRow,m_iHighlightedColumn;
	int m_iPopulatedWidth,m_iPopulatedHeight; // The last known size during populate grid
	bool bReAcquire,m_bParsed;
	class DataGridTable *m_pDataGridTableCache[CACHE_SIZE];
	class TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<class TextStyle *> m_vectTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable,m_iSeekColumn;
	string m_sGridID,m_sOptions,m_sExtraInfo,m_sSeek;
	DesignObj_Orbiter *m_pObjLeft,*m_pObjRight,*m_pObjUp,*m_pObjDown;

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();
};
//-------------------------------------------------------------------------------------------------------
#endif
