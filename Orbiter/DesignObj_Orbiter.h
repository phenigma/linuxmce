#ifndef DESIGNOBJ_Orbiter_H
#define DESIGNOBJ_Orbiter_H

#include "DesignObj_Data.h"
#include "PlutoUtils/GraphicFormat.h"

class ProntoCCF;
namespace DCE
{
	class Orbiter;
	class Logger;
	class DataGridTable;
}

using namespace DCE;

// Graphic memory management.  GR_DYNAMIC means that this is a dynamic image
// and there's no reason to ever keep it.  Discardonchange may keep the image
// if there's enough system memory. 

enum eGraphicManagement { GR_KEEPUNCOMPRESSED, GR_KEEPCOMPRESSED, GR_DISCARDONCHANGE, GR_DYNAMIC, GR_STORAGE };

// Windows-style RGB management

#ifndef RGB

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))

#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
//-------------------------------------------------------------------------------------------------------
enum GraphicType
{
	gtWinGraphic,
	gtIMGraphic,
	gtSDLGraphic
};
//-------------------------------------------------------------------------------------------------------
class CHAGraphic
{
protected :
	Orbiter *m_pOrbiter;
public:
	CHAGraphic();
	CHAGraphic(Orbiter *pOrbiter);
	CHAGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pOrbiter);
	virtual ~CHAGraphic();

	virtual void Initialize();
	virtual GraphicType GraphicType_get()=0;  // Must be implemented

	enum eGraphicManagement m_GraphicManagement;
	enum eGraphicFormat m_GraphicFormat;

	int Width, Height;
	string m_Filename;
};
//-------------------------------------------------------------------------------------------------------
class WinGraphic : public CHAGraphic
{
public:
	WinGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pOrbiter);
	WinGraphic(Orbiter *pOrbiter);
	~WinGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtWinGraphic; }

	void *m_pUncompressedImage;
	char *m_pCompressedImage;
	int m_CompressedImageLength;
};
//-------------------------------------------------------------------------------------------------------
class IMGraphic : public CHAGraphic
{
public:
	IMGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pCI);
	IMGraphic(Orbiter *pCI);
	~IMGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtIMGraphic; }

	void *m_pImage;
	struct SDL_Surface *m_pSDL_Surface;
};
//-------------------------------------------------------------------------------------------------------
class SDLGraphic: public CHAGraphic
{
public:
	SDLGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pCI) { m_pSDL_Surface=NULL; };
	SDLGraphic(Orbiter *pCI) { m_pSDL_Surface=NULL; };
	SDLGraphic(struct SDL_Surface *pSDL_Surface) { m_pSDL_Surface=pSDL_Surface; }
	~SDLGraphic() {};

	void Initialize() {};
	GraphicType GraphicType_get() { return gtSDLGraphic; }

	struct SDL_Surface *m_pSDL_Surface;
};
//-------------------------------------------------------------------------------------------------------
extern CHAGraphic *CreateGraphic(GraphicType Type, string Filename, eGraphicManagement GraphicManagement, 
						  class Orbiter *pOrbiter); //a builder method
//-------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------

typedef map<string, class DesignObj_Orbiter*> DesignObj_OrbiterMap;
typedef list<class CHAGraphic*> CHAGraphicList;
typedef vector<class DesignObj_Orbiter *> VectDesignObj_Orbiter;

#define GRAPHIC_NORMAL	0
#define GRAPHIC_SELECTED	-1
#define GRAPHIC_HIGHLIGHTED	-2


class DesignObj_Orbiter : public DesignObj_Data
{
public:
	DesignObj_Orbiter(class Orbiter *pCore);
	virtual ~DesignObj_Orbiter();

	class DesignObj_Orbiter *m_pDesignObj_Orbiter_Up,*m_pDesignObj_Orbiter_Down,*m_pDesignObj_Orbiter_Left,*m_pDesignObj_Orbiter_Right,*m_pDesignObj_Orbiter_TiedTo;
	int m_iColumnTiedTo,m_iRowTiedTo;  // this only applies if m_pDesignObj_Orbiter_TiedTo points to a datagrid, and this object is tied to a specific row in the grid

	class Orbiter *m_pCore;
	bool m_bOneTimeDontReset;
	bool m_bOnScreen,m_bDisableAspectLock,m_bContainsDataGrid;
	DataGridTable *m_pDataGridTable;
	class WebWindow *m_pWebWindow;
	class FloorplanObject *m_pFloorplanObject;
// todo: maybe datagrid related stuff should get moved into its own class to save memory.
	int m_GridCurRow, m_GridCurCol;  // The current top, left grid and column being displayed.  Zero based.  Example: In a grid with 10 rows per page, on the second page m_GridCurRow will = 10
	int m_MaxRow, m_MaxCol; // The total number of rows and columns visible on the screen.  Not related to the actual rows and columns in the grid.  
	int m_FixedRowHeight, m_FixedColumnWidth, m_RowSpacing;
	int m_ColumnSpacing, m_FirstRowHeight, m_FirstColumnWidth;
	vector<DesignObj_Orbiter *> m_vectObj_TabStops;  // If this is a screen, this is all the tab stops
	DesignObj_DataList m_OverlayObjects;

	int m_GraphicToDisplay;

	// m_pGraphicToUndoSelect is a temporary snapshot of the background that may be created during a select or restore
	// to return it to it's previous state
	CHAGraphic *m_pGraphic,*m_pSelectedGraphic,*m_pCurrentGraphic,*m_pHighlightedGraphic,*m_pGraphicToUndoSelect;
	vector<CHAGraphic *> vectAltGraphics;
	class Orbiter_CriteriaList *m_pCriteria;
	ProntoCCF *m_pCCF;

	bool IsHidden() { if( m_bHidden ) return true;  if( m_pParentObject ) return ((DesignObj_Orbiter *) m_pParentObject)->IsHidden(); return false; }

	// Runtime states
	bool m_bActive, m_bHidden;

	// Pre-parsed object paramters

	// To avoid looking this up while iterating through each cell
	// we pre-parse it at start up.

#define MAX_ALT_COLORS 15
	int m_AltColorValues[MAX_ALT_COLORS];

	string GetParameterValue(int ParameterID);
};

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

typedef list<CHAEffect *> CHAEffectList;

#define CACHE_UP 0
#define CACHE_DOWN 1
#define CACHE_LEFT 2
#define CACHE_RIGHT 3
#define CACHE_SIZE 4

// Eventually it might be good to be able to ask the server to cache a fairly signficant amount of the grid
class ProntoCCF;
class DesignObj_DataGrid : public DesignObj_Orbiter 
{
public:
	DesignObj_DataGrid(class Orbiter *pCore) : DesignObj_Orbiter(pCore)
	{
		for(int i=0;i<CACHE_SIZE;++i)
			m_pDataGridTableCache[i]=NULL;

		m_pObjLeft=m_pObjRight=m_pObjUp=m_pObjDown=NULL;
		bReAcquire=false;
		m_iDownRow=m_iUpRow=-1;
	}
	virtual ~DesignObj_DataGrid(); 

	int m_iDownRow,m_iUpRow;  // These are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
	int m_iHighlightedRow,m_iHighlightedColumn;
	bool bReAcquire;
	class DataGridTable *m_pDataGridTableCache[CACHE_SIZE];
	class TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<class TextStyle *> m_pTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable;
	string m_sGridID,m_sOptions,m_sExtraInfo;
	DesignObj_Orbiter *m_pObjLeft,*m_pObjRight,*m_pObjUp,*m_pObjDown;

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();
};

#endif
