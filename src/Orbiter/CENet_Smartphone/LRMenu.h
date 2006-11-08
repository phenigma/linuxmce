#ifndef _LRMENU_H_
#define _LRMENU_H_

//---------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
using namespace std;
//---------------------------------------------------------------------------------------------------------
#include <PocketFrog.h>
using namespace Frog;
//---------------------------------------------------------------------------------------------------------
#include "OrbiterApp.h"
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
#define _LOCAL_RENDERED_OBJECTS_
//---------------------------------------------------------------------------------------------------------
#if defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//	
	#define DisplayDevice Display
#else
	using namespace Frog::Internal;
#endif
//---------------------------------------------------------------------------------------------------------
#if defined UNICODE || defined _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
//---------------------------------------------------------------------------------------------------------
typedef unsigned char uchar;

//---------------------------------------------------------------------------------------------------------
#define DEFAULT_CRTROOMITEM_CAPTION "Rooms"
#define DEFAULT_MENUITEM_WIDTH	150
#define DEFAULT_MENUITEM_HEIGHT	27
#define DEFAULT_SCROLLBAR_HEIGHT 15
#define DEFAULT_BORDER_COLOR RGB( 0, 0, 0 )
#define DRAW_ITEMTEXT_MARGIN 3
#define DRAW_SHORTCUT_WIDTH  15
#define DRAW_SHORTCUT_COLOR  RGB(255,136,136)
#define DRAW_SUBMENUMARK_WIDTH 5
#define DRAW_SUBMENUMARK_HEIGHT 10
#define DRAW_SUBMENUMARK_COLOR RGB(0,0,0)
//---------------------------------------------------------------------------------------------------------
#define MENU_UP_KEY		'w'
#define MENU_DOWN_KEY	's'
#define MENU_LEFT_KEY	'a'
#define MENU_RIGHT_KEY	'd'
#define MENU_EXEC_KEY	'x'
#define MENU_BACK_KEY	'b'
//---------------------------------------------------------------------------------------------------------

/*
#define SAFE_DELETE( pointer ) { \
							if ( pointer ) { \
								delete pointer; \
								pointer = NULL; \
							} \
		}
*/
#define RETURN_ON_TRUE( test ) { \
							if ( test ) \
								return; \
		}

#define CHECK_UPDATING( ) RETURN_ON_TRUE( m_bChanging )
//---------------------------------------------------------------------------------------------------------

/*
 *
 *	LocalRenderer - static class implementing methods for drawing on a DisplayDevice
 *
 */
class LocalRenderer {
protected:
	static Rect m_rViewport;
	static Rect m_rAppViewport;
public:
	// Draw text - for bold creates new font from the current one
	static void DrawText( LPCTSTR Text, Rect &r, COLORREF color, BOOL bBold=false );

	// Draw an image
	static void DrawImage( char *pData,int nSize, Rect &r );

	// Draw a filled rectangle
	static void FillRect( Rect &r, COLORREF color );
	
	// Update Display Device
	static void Update( Rect* r );

	// Save/restore a rectangle from/to the Display Device
	//		- used for saving/restoring area behinf submenus
	static Surface* SaveRect( Rect r );
	static void RestoreRect( int iX, int iY, Surface* pSurface );

	// Draws a rectangle
	static void DrawRect( RECT r, COLORREF color );

	// Draws a horizontal rectangle
	static void DrawHRect( RECT r, COLORREF color );

	// Draws a horizontal line  - used in DrawHRect
	static void DrawHLine( int iX, int iY, int iW, COLORREF color );

	// Draws a filled triangle
	static void FillTriangle( int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, COLORREF color );

	// Get current display device from orbiterapp
	static DisplayDevice* GetDisplay() {
		return OrbiterApp::GetInstance()->GetDisplay();
	}

	// Drawing viewport 
	static void SetViewport( Rect rViewport ) { m_rViewport = rViewport; }
	static Rect& GetViewport( ) { return m_rViewport; }

	// Application viewport - used to reset viewport 
	static void SetAppViewport( Rect rViewport ) { m_rAppViewport = rViewport; }
	static Rect& GetAppViewport( ) { return m_rAppViewport; }

};

/*
 *
 *	LRMenuItemData - menu items drawing data
 *
 */
class LRMenuItemData {
protected:
	bool m_bVisible;
	tstring m_sCaption;
	UINT m_uiWidth, m_uiHeight;	

public:	
	LRMenuItemData(){
		m_sCaption = _T("");
		m_uiWidth = DEFAULT_MENUITEM_WIDTH; m_uiHeight = DEFAULT_MENUITEM_HEIGHT;
		m_bVisible = true;
	}
	LRMenuItemData( LRMenuItemData& ItemData ){
		*this = ItemData;
	}
	LRMenuItemData& operator=( LRMenuItemData& ItemData ){
		m_sCaption = ItemData.m_sCaption;
		m_uiWidth = ItemData.m_uiWidth;
		m_uiHeight = ItemData.m_uiHeight;
		m_bVisible = ItemData.m_bVisible;
		return *this;
	}

	// Item width/height
	virtual UINT GetWidth() { return m_uiWidth; }
	virtual UINT GetHeight() { return m_uiHeight; }
	virtual void SetWidth( UINT uiWidth ) { m_uiWidth = uiWidth; }
	virtual void SetHeight( UINT uiHeight ) { m_uiHeight = uiHeight; }

	// Item caption
	virtual void SetCaption( tstring sCaption ) { m_sCaption = sCaption; }
	virtual void SetCaptionToString( string sCaption );
	virtual tstring& GetCaption( ) { return m_sCaption; }

	// Visible/Invisible item
	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool GetVisible( ) { return m_bVisible; }

};

/*
 *
 *	LRMenuItemData - menu item
 *
 */
class LRMenuItem: public LRMenuItemData {
protected:
	vector<LRMenuItem*> m_vMenuItems;
	LRMenuItem* m_pParent;
	UINT m_uiItemsHeight, m_uiItemsWidth;
	int m_iStart, m_iEnd, m_iHighlight;
	bool m_bDirty;
	Rect m_rSubmenu;	
	bool m_bHasScroll;

	Surface *m_pClone;
	void CalcSubmenuLayout( RECT rViewport );	
	map<TCHAR, int> m_mShotcuts;
	TCHAR m_ucShortcut;

	void AddShortcut( int iPos, TCHAR ucShortcut );	

	// method to override to do something on item select
	virtual bool DoAction( void ){ return false; }

	bool m_bIsShowing, m_bIsOnTop;	
	virtual void ShowSubmenu( bool bShow=true );
	
	void ClearSubmenu();

	Rect m_rClientRect;
	bool m_bPainting;

	bool BeginPaint( RECT r, bool bForceRepaint );
	bool EndPaint() { m_bDirty = false; m_bPainting = false; return true; }

public:	
	LRMenuItem( LRMenuItem* pParent=NULL ){ 
		m_pParent = pParent; 
		m_uiItemsHeight = m_uiItemsWidth = 0; 
		m_iEnd = m_iStart = 0;
		m_bDirty = true;
		m_bHasScroll = false;
		m_pClone = NULL;
		m_ucShortcut = 0;
		m_bIsShowing = m_bIsOnTop = false;
		m_iHighlight = -1;
		m_rClientRect = Rect(0,0,0,0);
		m_bPainting = false;
	}
	LRMenuItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL )
		:LRMenuItemData(ItemData)
	{
		m_uiItemsHeight = m_uiItemsWidth = 0;
		m_pParent = pParent;
		m_iEnd = m_iStart = 0;
		m_bDirty = true;
		m_bHasScroll = false;
		m_pClone = NULL;
		m_ucShortcut = 0;
		m_bIsShowing = m_bIsOnTop = false;
		m_iHighlight = -1;
		m_rClientRect = Rect(0,0,0,0);
		m_bPainting = false;
	}
	virtual ~LRMenuItem();

	// MArk all items in submenu as bDirty
	void MakeSubmenuDirty( bool bDirty = true );

	// Paint item
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
	
	/* To DO: When Item heights are not all the same and the menu doesn't fit inside the viewport ( has scrollbar )
				there should be some drawing glitches at the bottom of the menu ( as the menu height changes )
	*/
	virtual void PaintSubmenu( bool bForceRepaint=false );

	// Move cursor up/down inside submenu
	virtual void MoveCursor( int iDir );

	// Show Submenu
	virtual void Show( bool bForceRepaint=false ) {
		ShowSubmenu( true );
		PaintSubmenu( bForceRepaint );
		m_bIsOnTop = true;
	}

	// Hide submenu
	virtual void Hide( ){
		ShowSubmenu( false );
		m_bIsOnTop = false;
	}

	// Repaint submenu
	virtual void BringToFront( ){
		if ( m_bIsShowing )	{
			PaintSubmenu( );
			m_bIsOnTop = true;
		}

	}

	// Execute - launches DoAction
	virtual bool Exec();

	// Delete and Remove  submenu items
	virtual void Clear( void ) { ClearSubmenu(); }

	//Add new submenu item
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData );
	virtual void AddItem( LRMenuItem* pMenuItem );

	//Get current selected submenu item
	virtual LRMenuItem* GetCrtItem( void ) { 
		if ( m_iHighlight>=0 )	return m_vMenuItems[m_iHighlight]; 
		return NULL;
	}

	// Calculate Submenu height/width
	virtual UINT GetSubmenuHeight( int iLimit=-1 );
	virtual UINT GetSubmenuWidth( void );

	// if Dirty it'll be redraw on submenu repaint
	virtual void SetDirty( bool bDirty = true ) { m_bDirty = bDirty; }
	virtual bool IsDirty() { return m_bDirty; }	
	
	// Submenu position/rect
	virtual void GetSubmenuRect( RECT& r ){	r = m_rSubmenu; }
	virtual void SetSubmenuX( int iX ) { m_rClientRect.left = m_rClientRect.right = iX; }
	virtual void SetSubmenuY( int iY ) { m_rClientRect.top = m_rClientRect.bottom = iY; }
	virtual int GetSubmenuX() { return ( m_rClientRect.left + m_rClientRect.right ) / 2; }
	virtual int GetSubmenuY() { return ( m_rClientRect.top + m_rClientRect.bottom ) / 2; }

	// Menu item shortcut
	virtual void SetShortcut( TCHAR ucShortcut ){ m_ucShortcut = ucShortcut; }
	//Search submenu for shortcut
	virtual bool SelectItem( TCHAR ucShortcut );
	virtual bool SelectItem( int iX, int iY );

	// Check for submenu
	virtual bool HasSubMenu( void );

	// Menu item parent
	virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }
	virtual LRMenuItem* GetParent() { return m_pParent; }

	// Menu item changed
	virtual bool Changed() { 
		return ( !m_uiItemsHeight || !m_uiItemsWidth ); 
	}

	// Set item caption
	virtual void SetCaption( tstring sCaption ) { 
		SetDirty();
		LRMenuItemData::SetCaption( sCaption ); 
	}
	virtual void SetCaptionToString( string sCaption ) {
		SetDirty();
		LRMenuItemData::SetCaptionToString( sCaption ); 
	} 	

	// returns false, as this is a real menu item
	virtual bool IsLink() { return false; }

	virtual bool PointInSubmenu( int iX, int iY );
	virtual bool PointIn( int iX, int iY );

};

/*
 *
 *	LRMenuItemData - fake menu item ( link to a real menu item )
 *		 all methods are delegated to the real item except for GetVisible and IsLink
 *       which always return true 
 *
 */
class LRMenuItemLink: public LRMenuItem {
protected:	
	LRMenuItem* m_pOrigItem;
	LRMenuItemLink( LRMenuItem* pParent=NULL ){}
	LRMenuItemLink( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){}	

public:
	LRMenuItemLink( LRMenuItem* pItem, LRMenuItem* pParent=NULL ){ m_pOrigItem = pItem; m_pParent=pParent; }
	virtual ~LRMenuItemLink(){ m_pOrigItem = NULL; }

	virtual UINT GetWidth() { return m_pOrigItem->GetWidth(); }
	virtual UINT GetHeight() { return m_pOrigItem->GetHeight(); }
	virtual void SetWidth( UINT uiWidth ) { m_pOrigItem->SetWidth( uiWidth ); }
	virtual void SetHeight( UINT uiHeight ) { m_pOrigItem->SetHeight( uiHeight ); }
	virtual void SetCaption( tstring sCaption ) { m_pOrigItem->SetCaption( sCaption ); }
	virtual void SetCaptionToString( string sCaption ) { m_pOrigItem->SetCaptionToString( sCaption ); }
	virtual tstring& GetCaption( ) { return m_pOrigItem->GetCaption(); }
	virtual void SetVisible( bool bVisible ) { m_pOrigItem->SetVisible( bVisible ); }
	virtual bool GetVisible( ) { return true; } // The only one changed


	virtual void Clear( void ) { m_pOrigItem->Clear(); }
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false ) { m_pOrigItem->Paint( r, bHighlight, bForceRepaint ); }
	virtual void PaintSubmenu( bool bForceRepaint=false ) { m_pOrigItem->PaintSubmenu( bForceRepaint ); }
	virtual void MoveCursor( int iDir ) { m_pOrigItem->MoveCursor( iDir ); }
	virtual void Show( bool bForceRepaint=false ) { m_pOrigItem->Show( bForceRepaint ); }
	virtual void Hide( ) { m_pOrigItem->Hide( ); }
	virtual void BringToFront( ) { m_pOrigItem->BringToFront( ); }
	virtual bool Exec() { return m_pOrigItem->Exec(); }
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData ) { return m_pOrigItem->AddItem( ItemData ); }
	virtual void AddItem( LRMenuItem* pMenuItem ) { m_pOrigItem->AddItem( pMenuItem ); }
	virtual LRMenuItem* GetCrtItem( void ) { return m_pOrigItem->GetCrtItem(); }
	virtual UINT GetSubmenuHeight( int iLimit=-1 )  { return m_pOrigItem->GetSubmenuHeight( iLimit ); }
	virtual UINT GetSubmenuWidth( void ) { return m_pOrigItem->GetSubmenuWidth(); }
	virtual void SetDirty( bool bDirty = true ) { m_pOrigItem->SetDirty( bDirty ); }
	virtual bool IsDirty() { return m_pOrigItem->IsDirty(); }	
	virtual void GetSubmenuRect( RECT& r ){ m_pOrigItem->GetSubmenuRect( r ); }
	virtual void SetSubmenuX( int iX ) { m_pOrigItem->SetSubmenuX( iX ); }
	virtual void SetSubmenuY( int iY ) { m_pOrigItem->SetSubmenuY( iY ); }
	virtual int GetSubmenuX() { return m_pOrigItem->GetSubmenuX(); }
	virtual int GetSubmenuY() { return m_pOrigItem->GetSubmenuY(); }
	virtual void SetShortcut( TCHAR ucShortcut ) { m_pOrigItem->SetShortcut( ucShortcut); }
	virtual bool HasSubMenu( void ) { return m_pOrigItem->HasSubMenu(); }
	virtual bool SelectItem( TCHAR ucShortcut ) { return m_pOrigItem->SelectItem( ucShortcut ); } 
	virtual bool SelectItem( int iX, int iY ) { return m_pOrigItem->SelectItem( iX, iY ); } 

	virtual bool Changed() { return m_pOrigItem->Changed(); }
	virtual bool IsLink() { return true; }

	virtual bool PointInSubmenu( int iX, int iY ){ return m_pOrigItem->PointInSubmenu( iX, iY ); }
	virtual bool PointIn( int iX, int iY ) { return m_pOrigItem->PointIn( iX, iY ); }

	// GetParent/SetParent won't be rewritten
	//virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }
	//virtual LRMenuItem* GetParent() { return m_pParent; }

};

/*
 *
 *	LRMenu - main menu
 *
 */
class LRMenu {
protected:
	UINT m_uiX, m_uiY;
	LRMenuItem* m_pMenuRoot, *m_pDisplayMenu;	
	bool m_bIsShowing;
	bool m_bChanging;

	virtual void CreateMenuRoot() { 
		m_pMenuRoot = new LRMenuItem; 
		m_pDisplayMenu = m_pMenuRoot;
		m_bIsShowing = false;
	}
public:
	LRMenu();
	~LRMenu();

	// Add menu item
	LRMenuItem* AddItem( LRMenuItemData& ItemData );
	virtual void AddItem( LRMenuItem* pMenuItem );

	// Begin/End items list change - menu won't draw while items list is changing
	void BeginUpdate() { m_bChanging = true; }
	void EndUpdate() { m_bChanging = false; }

	//Show/Hide main menu
	void Show( int iX, int iY );
	void Hide();

	//(Force)Paint menu - bFull==true, repaint all opened submenus
	void Paint( bool bFull = false );	

	
	//Translates Pluto keys to Menu commans
	bool HandleKeys( int nPK_Button, bool bKeyUp );	
	// implementing menu commands
	bool KeyPress( uchar ucKey );
	//Handle stylus events
	bool HandleStylus( int iX, int iY );

	// Main menu is active
	bool IsShowing() { return m_bIsShowing; }

	//Menu viewport
	void SetViewport( RECT rViewport ) { LocalRenderer::SetViewport( Rect(rViewport) );  } //repaint
};


/*
 *
 *	LRPhoneMenuItemRoot - phone menu root item
 *
 */
class LRPhoneMenuItemRoot : public LRMenuItem {
public:
	//Remove links to scenarios
	void RemoveLinks( void );
};

/*
 *
 *	LRPhoneMenu - Phone menu
 *
 */
class LRCrtRoomItem;
class LRPhoneMenu: public LRMenu {
protected:
	LRCrtRoomItem* m_pCrtRoomItem;
private:
	LRPhoneMenu(){m_pCrtRoomItem = NULL;}
public:
	typedef enum { tbcFore=0, tbcBack, tbcHlFore, tbcHlBack, tbcNone } TextBitsColor;
protected:
	static COLORREF m_clTextColor[ tbcNone ];	
	virtual void CreateMenuRoot() { 
		m_pMenuRoot = new LRPhoneMenuItemRoot; 
		m_pDisplayMenu = m_pMenuRoot;
		m_bIsShowing = false;
	}
	

public:		
	// Constructor blocked so we can use CreateMenuRoot with LRPhoneMenuItemRoot
	static LRPhoneMenu* Create();

	// Set/Get (highlighted/foreground/background)items color
	static void SetColor( TextBitsColor ColorIdx, COLORREF Color ){
		if ( ColorIdx<tbcNone )	m_clTextColor[ColorIdx] = Color;
	}
	static COLORREF GetColor( TextBitsColor ColorIdx ){
		if ( ColorIdx==tbcNone ) return RGB(0,0,0);
		else return m_clTextColor[ColorIdx];
	}

	virtual void AddItem( LRMenuItem* pMenuItem );

	// Set current room
	void SetCrtRoom( long nCrtRoom );
};

/*
 *
 *	LRPhoneMenuItem - base class for phone menu items
 *
 */
class LRPhoneMenuItem : public LRMenuItem {
protected:
	bool m_bStyleBold;
public:
	LRPhoneMenuItem( LRMenuItem* pParent=NULL ) { 
		m_bStyleBold = false;
	}
	LRPhoneMenuItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_bStyleBold = false;
	}

	// reimplemented to draw phone menu items using received styles
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
};

/*
 *
 *	LRCrtRoomItem - Current room item ( first item in menu )
 *
 */
class LRCrtRoomItem : public LRPhoneMenuItem {
public:
	LRCrtRoomItem( LRMenuItem* pParent=NULL ) { 
		m_bStyleBold = true;
	}
	LRCrtRoomItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_bStyleBold = true;
	}
	
	// Select current room
	void SelectRoom( int iRoomId );

	// reimplemented do draw current room item in a different way
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
};

/*
 *
 *	LRRoomItem - Rooms items
 *
 */
class LRRoomItem : public LRPhoneMenuItem {
protected:
	int m_iRoomId;

	// reimplemented to add links in root menu when choosing a room
	virtual bool DoAction( void );
public:
	LRRoomItem( LRMenuItem* pParent=NULL ):LRPhoneMenuItem(pParent) { 
		m_iRoomId = -1;
	}
	LRRoomItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ):LRPhoneMenuItem( ItemData, pParent ){		
		m_iRoomId = -1;
	}
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData ) { 
		LRMenuItem* pItem = LRMenuItem::AddItem( ItemData ); 
		pItem->SetVisible( false );
		return pItem;
	}

	// All Scenarios are invisible - they'll be linked in root menu
	virtual void AddItem( LRMenuItem* pMenuItem ) { 		
		LRMenuItem::AddItem( pMenuItem ); 		
		pMenuItem->SetVisible( false );
	}

	// Room Id - for selecting current room
	void SetRoomId( int iRoomId ) { m_iRoomId = iRoomId; }
	int  GetRoomId() { return m_iRoomId; }

	// Create links to scenarios in root menu
	void CreateLinks( void );

};

/*
 *
 *	LRScenarioItem - Scenario items
 *
 */
class LRScenarioItem : public LRPhoneMenuItem {
	// nothing specific
};

/*
 *
 *	LRSubScenarioItem - SubScenario items
 *
 */
class LRSubScenarioItem : public LRPhoneMenuItem {
protected:
	string m_sId;

	// reimplemented to send SelectedItem on choosing a SubScenario
	virtual bool DoAction( void );
public:		
	LRSubScenarioItem( LRMenuItem* pParent=NULL ) { 
		m_sId = "";
	}
	LRSubScenarioItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_sId = "";
	}

	// SubScenario Id - sent to SelectedItem
	void SetId( string sId ) { m_sId = sId; }
	string& GetId( ) { return m_sId; }
};

#endif

