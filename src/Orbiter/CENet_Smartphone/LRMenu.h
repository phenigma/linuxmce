#ifndef _LRMENU_H_
#define _LRMENU_H_

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <PocketFrog.h>
using namespace Frog;
//#include "Orbiter/PocketFrog/PocketFrogWrapper.h"
#include "OrbiterApp.h"

#define _LOCAL_RENDERED_OBJECTS_

#if defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//	
	#define DisplayDevice Display
#else
	using namespace Frog::Internal;
#endif

#if defined UNICODE || defined _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif

typedef unsigned char uchar;

#define SAFE_DELETE( pointer ) { \
							if ( pointer ) { \
								delete pointer; \
								pointer = NULL; \
							} \
		}


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


#define MENU_UP_KEY		'w'
#define MENU_DOWN_KEY	's'
#define MENU_LEFT_KEY	'a'
#define MENU_RIGHT_KEY	'd'
#define MENU_EXEC_KEY	'x'
#define MENU_BACK_KEY	'b'


#define RETURN_ON_TRUE( test ) { \
							if ( test ) \
								return; \
		}

#define CHECK_RENDERER( ) RETURN_ON_TRUE( !LocalRenderer::Valid() )
#define CHECK_UPDATING( ) RETURN_ON_TRUE( m_bChanging )



class LocalRenderer {
protected:
	static Rect m_rViewport;
public:
	static void DrawText( LPCTSTR Text, Rect &r, COLORREF color, BOOL bBold=false );
	static void DrawImage( char *pData,int nSize, Rect &r );
	static void FillRect( Rect &r, COLORREF color );		
	static void Update( RECT r );
	static Surface* SaveRect( Rect r );
	static void RestoreRect( int iX, int iY, Surface* pSurface );
	static void DrawRect( RECT r, COLORREF color );
	static void DrawHRect( RECT r, COLORREF color );
	static void DrawHLine( int iX, int iY, int iW, COLORREF color );
	static void FillTriangle( int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, COLORREF color );

	static DisplayDevice* GetDisplay() {
		return OrbiterApp::GetInstance()->GetDisplay();
	}
	static void SetViewport( Rect rViewport ) { m_rViewport = rViewport; }
	static Rect& GetViewport( ) { return m_rViewport; }

	static bool Valid() {  return GetDisplay()!=NULL; }

};

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

	virtual UINT GetWidth() { return m_uiWidth; }
	virtual UINT GetHeight() { return m_uiHeight; }
	virtual void SetWidth( UINT uiWidth ) { m_uiWidth = uiWidth; }
	virtual void SetHeight( UINT uiHeight ) { m_uiHeight = uiHeight; }
	virtual void SetCaption( tstring sCaption ) { m_sCaption = sCaption; }
	virtual void SetCaptionToString( string sCaption );
	virtual tstring& GetCaption( ) { return m_sCaption; }
	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool GetVisible( ) { return m_bVisible; }

};

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

	void MakeSubmenuDirty( bool bDirty = true );

	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
	/* To DO: When Item heights are not all the same and the menu doesn't fit inside the viewport ( has scrollbar )
				there should be some drawing glitches at the bottom of the menu ( as the menu height changes )
	*/
	virtual void PaintSubmenu( bool bForceRepaint=false );

	virtual void MoveCursor( int iDir );
	virtual void Show( bool bForceRepaint=false ) {
		ShowSubmenu( true );
		PaintSubmenu( bForceRepaint );
		m_bIsOnTop = true;
	}
	virtual void Hide( ){
		ShowSubmenu( false );
		m_bIsOnTop = false;
	}
	virtual void BringToFront( ){
		if ( m_bIsShowing )	{
			PaintSubmenu( );
			m_bIsOnTop = true;
		}

	}

	virtual bool Exec();

	virtual void Clear( void ) { ClearSubmenu(); }
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData );
	virtual void AddItem( LRMenuItem* pMenuItem );
	virtual LRMenuItem* GetCrtItem( void ) { 
		if ( m_iHighlight>=0 )	return m_vMenuItems[m_iHighlight]; 
		return NULL;
	}
	virtual UINT GetSubmenuHeight( int iLimit=-1 );
	virtual UINT GetSubmenuWidth( void );
	virtual void SetDirty( bool bDirty = true ) { m_bDirty = bDirty; }
	virtual bool IsDirty() { return m_bDirty; }
	virtual LRMenuItem* GetParent() { return m_pParent; }
	
	virtual void GetSubmenuRect( RECT& r ){	r = m_rSubmenu; }
	virtual int GetSubmenuX() { return ( m_rClientRect.left + m_rClientRect.right ) / 2; }
	virtual int GetSubmenuY() { return ( m_rClientRect.top + m_rClientRect.bottom ) / 2; }
	virtual void SetSubmenuX( int iX ) { m_rClientRect.left = m_rClientRect.right = iX; }
	virtual void SetSubmenuY( int iY ) { m_rClientRect.top = m_rClientRect.bottom = iY; }

	virtual void SetShortcut( TCHAR ucShortcut ){ m_ucShortcut = ucShortcut; }
	virtual bool HasSubMenu( void );
	virtual bool SelectItem( TCHAR ucShortcut );

	virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }

	virtual bool Changed() { 
		return ( !m_uiItemsHeight || !m_uiItemsWidth ); 
	}

	virtual void SetCaption( tstring sCaption ) { 
		SetDirty();
		LRMenuItemData::SetCaption( sCaption ); 
	}
	virtual void SetCaptionToString( string sCaption ) {
		SetDirty();
		LRMenuItemData::SetCaptionToString( sCaption ); 
	} 	

	virtual bool IsLink() { return false; }

};

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
	virtual int GetSubmenuX() { return m_pOrigItem->GetSubmenuX(); }
	virtual int GetSubmenuY() { return m_pOrigItem->GetSubmenuY(); }
	virtual void SetSubmenuX( int iX ) { m_pOrigItem->SetSubmenuX( iX ); }
	virtual void SetSubmenuY( int iY ) { m_pOrigItem->SetSubmenuY( iY ); }
	virtual void SetShortcut( TCHAR ucShortcut ) { m_pOrigItem->SetShortcut( ucShortcut); }
	virtual bool HasSubMenu( void ) { return m_pOrigItem->HasSubMenu(); }
	virtual bool SelectItem( TCHAR ucShortcut ) { return m_pOrigItem->SelectItem( ucShortcut ); } 

	virtual bool Changed() { return m_pOrigItem->Changed(); }
	virtual bool IsLink() { return true; }

	// GetParent/SetParent won't be rewritten
	//virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }
	//virtual LRMenuItem* GetParent() { return m_pParent; }

};

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
	LRMenuItem* AddItem( LRMenuItemData& ItemData );
	void BeginUpdate() { m_bChanging = true; }
	void EndUpdate() { m_bChanging = false; }
	void AddItem( LRMenuItem* pMenuItem );
	void Show( int iX, int iY );
	void Hide();
	void Paint( bool bFull = false );	
	bool KeyPress( uchar ucKey );
	void SetViewport( RECT rViewport ) { LocalRenderer::SetViewport( Rect(rViewport) );  } //repaint
	bool IsShowing() { return m_bIsShowing; }
	bool HandleKeys( int nPK_Button, bool bKeyUp );	
};


class LRPhoneMenuItemRoot : public LRMenuItem {
public:
	void RemoveLinks( void );
};

class LRPhoneMenu: public LRMenu {
private:
	LRPhoneMenu(){}
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
	static LRPhoneMenu* Create();
	static void SetColor( TextBitsColor ColorIdx, COLORREF Color ){
		if ( ColorIdx<tbcNone )	m_clTextColor[ColorIdx] = Color;
	}
	static COLORREF GetColor( TextBitsColor ColorIdx ){
		if ( ColorIdx==tbcNone ) return RGB(0,0,0);
		else return m_clTextColor[ColorIdx];
	}
};

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
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
};

class LRCrtRoomItem : public LRPhoneMenuItem {
public:
	LRCrtRoomItem( LRMenuItem* pParent=NULL ) { 
		m_bStyleBold = true;
	}
	LRCrtRoomItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_bStyleBold = true;
	}
	void SelectRoom( int iRoomId );

	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
};

class LRRoomItem : public LRPhoneMenuItem {
protected:
	int m_iRoomId;
	virtual bool DoAction( void );
public:
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData ) { 
		LRMenuItem* pItem = LRMenuItem::AddItem( ItemData ); 
		pItem->SetVisible( false );
		m_iRoomId = 0;
		return pItem;
	}
	virtual void AddItem( LRMenuItem* pMenuItem ) { 		
		LRMenuItem::AddItem( pMenuItem ); 
		m_iRoomId = 0;
		pMenuItem->SetVisible( false );
	}

	void SetRoomId( int iRoomId ) { m_iRoomId = iRoomId; }
	int  GetRoomId() { return m_iRoomId; }
	void CreateLinks( void );

};

class LRScenarioItem : public LRPhoneMenuItem {
};

class LRSubScenarioItem : public LRPhoneMenuItem {
protected:
	string m_sId;
	virtual bool DoAction( void );
public:		
	LRSubScenarioItem( LRMenuItem* pParent=NULL ) { 
		m_sId = "";
	}
	LRSubScenarioItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_sId = "";
	}

	void SetId( string sId ) { m_sId = sId; }
	string& GetId( ) { return m_sId; }
};

#endif

