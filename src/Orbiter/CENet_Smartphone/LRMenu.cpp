#include "LRMenu.h"
#include "../../pluto_main/Define_Button.h"
//---------------------------------------------------------------------------------------------------------
#if defined(SMARTPHONE2005) || defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//
	#define clock		GetTickCount
	#define Surface_GetWidth pSurface->GetWidth()
	#define Surface_GetHeight pSurface->GetHeight()
	#define pf_uint8_t PHAL::uint8_t
#else	
	#include "src/internal/graphicbuffer.h"
	#include "src/internal/raster2d.h"

	#define Surface_GetWidth pSurface->m_width
	#define Surface_GetHeight pSurface->m_height
	#define pf_uint8_t uint8_t
#endif
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenuItemData
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRMenuItemData::SetCaptionToString( string sCaption )
{
	#if defined UNICODE || defined _UNICODE
		m_sCaption.resize( sCaption.length() );
		copy( sCaption.begin(), sCaption.end(), m_sCaption.begin() );
	#else
		m_sCaption = sCaption;
	#endif
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LocalRenderer
 *
 */
//---------------------------------------------------------------------------------------------------------

Rect LocalRenderer::m_rViewport;
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawText( LPCTSTR Text, Rect &r, COLORREF color, BOOL bBold )
{
	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

 	::SetTextColor(hdc, color);
	::SetBkMode(hdc, TRANSPARENT);	

	RECT rectLocation = { r.left, r.top, r.right, r.bottom };

	if ( bBold ) { // if bold -> create new font from the current one
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfWeight = FW_BOLD;
		::GetTextFace( hdc, LF_FACESIZE, lf.lfFaceName );
		HFONT hFont = ::CreateFontIndirect(&lf);
		if ( hFont ) {
			HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);		
			::DrawText(hdc, Text, _tcslen( Text ), &rectLocation, DT_WORDBREAK | DT_NOPREFIX); 
			::SelectObject(hdc, hOldFont);
			::DeleteObject(hFont);
		}				
		
	} 
	else ::DrawText(hdc, Text, _tcslen( Text ), &rectLocation, DT_WORDBREAK | DT_NOPREFIX); 

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawImage( char *pData,int nSize, Rect &r )
{
	Surface *pSurface = LoadImage( GetDisplay(), (pf_uint8_t*)pData, (pf_uint8_t*)(pData + nSize));

	if(pSurface)
	{		
		if(Surface_GetWidth == r.GetWidth() && Surface_GetHeight == r.GetHeight() )
			GetDisplay()->Blit( r.left, r.top, pSurface );
		else //zoom
		{
			Rect dest(r);	

			double ZoomX = r.GetWidth() / double(Surface_GetWidth);
			double ZoomY = r.GetHeight() / double(Surface_GetHeight);

			dest.right = dest.left + int(Surface_GetWidth * ZoomX);
			dest.bottom = dest.top + int(Surface_GetHeight * ZoomY);     
        
			if( dest.right-dest.left>0 && dest.bottom-dest.top>0 ) 
				GetDisplay()->BlitStretch(dest, pSurface); 
		}

		delete pSurface;
	}
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::FillRect( Rect &r, COLORREF color )
{
	GetDisplay()->FillRect(r.left, r.top, r.right, r.bottom, Color( color ) );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::Update( RECT r )
{
	Rect rr(r);
	GetDisplay()->Update( &rr );
}
//---------------------------------------------------------------------------------------------------------
Surface* LocalRenderer::SaveRect( Rect r )
{
#if defined(_VC80_UPGRADE) // different implementation for PocketFrog versions
	Surface* copy = new Surface( r.GetWidth(), r.GetHeight() );
	if (copy){
        Rasterizer rasterizer( copy );
		rasterizer.Blit( 0, 0, GetDisplay()->GetBackBuffer(), &r );
    }
	return copy;
#else
	DisplayDevice *pDisplayDevice = GetDisplay();
	Surface *pSurface= pDisplayDevice->CreateSurface( r.GetWidth(), r.GetHeight() );
	Rasterizer* pRasterizer = pDisplayDevice->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &r);
	delete pRasterizer;	
	return pSurface;
#endif
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::RestoreRect( int iX, int iY, Surface* pSurface )
{
#if defined(_VC80_UPGRADE) // different implementation for PocketFrog versions
	Rasterizer rasterizer( GetDisplay()->GetBackBuffer() );
	rasterizer.Blit( iX, iY, pSurface );	
#else	
	DisplayDevice *pDisplayDevice = GetDisplay();
	Surface *pLocalSurface = pDisplayDevice->GetBackBuffer();
	Rasterizer* pRasterizer =  pDisplayDevice->CreateRasterizer( pLocalSurface );			
	pRasterizer->Blit( iX, iY, pSurface );
	delete pRasterizer;
#endif

}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawRect( RECT r, COLORREF color )
{
	Pixel cc(color);
	Rect rr(r);
	GetDisplay()->DrawRect( rr.left, rr.top, rr.right, rr.bottom, cc );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawHRect( RECT r, COLORREF color )
{
	Pixel cc(color);
	Rect rr(r);
	GetDisplay()->DrawHLine( rr.left, rr.top, rr.GetWidth(), cc );
	GetDisplay()->DrawHLine( rr.left, rr.bottom-1, rr.GetWidth(), cc );
	GetDisplay()->DrawVLine( rr.left, rr.top, rr.GetHeight(), cc );
	GetDisplay()->DrawVLine( rr.right-1, rr.top, rr.GetHeight(), cc );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::FillTriangle( int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, COLORREF color )
{
	Point points[3];
	points[0] = Point( iX1, iY1 );
	points[1] = Point( iX2, iY2 );
	points[2] = Point( iX3, iY3 );
	GetDisplay()->FillPoly( points, 3, Color(color) );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawHLine( int iX, int iY, int iW, COLORREF color )
{
	Pixel cc(color);
	GetDisplay()->DrawHLine( iX, iY, iW, cc );
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenu
 *
 */
//---------------------------------------------------------------------------------------------------------
LRMenu::LRMenu()
{
	CreateMenuRoot();
}
//---------------------------------------------------------------------------------------------------------
LRMenu::~LRMenu()
{
	SAFE_DELETE( m_pMenuRoot );
	m_pMenuRoot = NULL;
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem* LRMenu::AddItem( LRMenuItemData& ItemData )
{	
	if ( !m_bChanging ) return NULL;

	LRMenuItem* pItem = m_pMenuRoot->AddItem( ItemData );
	return pItem;
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::AddItem( LRMenuItem* pMenuItem )
{
	if ( !m_bChanging ) return;

	if ( !pMenuItem ) return;
	m_pMenuRoot->AddItem( pMenuItem );
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Show( int iX, int iY )
{
	CHECK_UPDATING();
	if ( m_bIsShowing ) return;

	m_bIsShowing = true;
	Rect r = LocalRenderer::GetViewport();
	m_pDisplayMenu->SetSubmenuX( r.left + iX );
	m_pDisplayMenu->SetSubmenuY( r.top + iY );
	m_pDisplayMenu->Show( );	
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Hide()
{
	CHECK_UPDATING();
	if ( !m_bIsShowing ) return;
	m_bIsShowing = false;
	while ( NULL != m_pDisplayMenu->GetParent() ) {
		m_pDisplayMenu->Hide();
		m_pDisplayMenu = m_pDisplayMenu->GetParent();
	}
	m_pDisplayMenu->Hide();

}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Paint( bool bFull )
{	
	CHECK_UPDATING();

	if ( !m_bIsShowing ) return;
	if ( !bFull ){
		m_pDisplayMenu->PaintSubmenu( true );
	}
	else {
		LRMenuItem *pSubmenu = m_pDisplayMenu;
		vector<LRMenuItem*> vSubmenus;
		while ( pSubmenu ) {
			vSubmenus.push_back( pSubmenu );
			pSubmenu = pSubmenu->GetParent();
		}
		for ( vector<LRMenuItem*>::reverse_iterator iter = vSubmenus.rbegin(); iter!= vSubmenus.rend(); ++iter ){
			(*iter)->Show( true );
		}
	}
}
//---------------------------------------------------------------------------------------------------------
bool LRMenu::KeyPress( uchar ucKey )
{
	if ( !LocalRenderer::Valid() ) return false;
	if ( !m_pDisplayMenu ) return false;
	if ( !IsShowing() ) return false;

	if ( ucKey>='0' && ucKey<='9' ) {
		if ( !m_pDisplayMenu->SelectItem( ucKey ) ) return true;
		m_pDisplayMenu->PaintSubmenu( );
		ucKey = MENU_EXEC_KEY;
	}
	switch( ucKey ){
		case MENU_UP_KEY:{			
			m_pDisplayMenu->MoveCursor( 0 );
			m_pDisplayMenu->PaintSubmenu( );
						 }
			break;
		case MENU_DOWN_KEY:{
			m_pDisplayMenu->MoveCursor( 1 );
			m_pDisplayMenu->PaintSubmenu( );
						   }
			break;
		case MENU_BACK_KEY:
		case MENU_LEFT_KEY:
			m_pDisplayMenu->Hide( );
			if ( m_pDisplayMenu->GetParent() ){				
				m_pDisplayMenu = m_pDisplayMenu->GetParent();
				m_pDisplayMenu->BringToFront( );
			}else {				
				m_bIsShowing = false;
			}
			break;
		case MENU_RIGHT_KEY:
			if (m_pDisplayMenu->GetCrtItem() )
				if ( m_pDisplayMenu->GetCrtItem()->HasSubMenu() ) {				
					m_pDisplayMenu = m_pDisplayMenu->GetCrtItem();
					m_pDisplayMenu->Exec( );
				}
			break;
		case MENU_EXEC_KEY:
			if ( m_pDisplayMenu->GetCrtItem() ) {
				if ( m_pDisplayMenu->GetCrtItem()->HasSubMenu() ) {
					m_pDisplayMenu = m_pDisplayMenu->GetCrtItem();
					m_pDisplayMenu->Exec( );
				}
				else {
					if ( !m_pDisplayMenu->GetCrtItem()->Exec( ) ) break;
					m_pDisplayMenu->Hide();
					if ( m_pDisplayMenu->GetParent() ){
						m_pDisplayMenu = m_pDisplayMenu->GetParent();
						if ( m_pDisplayMenu->Changed() ) {
							m_pDisplayMenu->Hide();
							m_pDisplayMenu->Show();
						}
						else m_pDisplayMenu->BringToFront( );
					}
					else {
						m_bIsShowing = false;
					}				
				}
			}
			break;

	}
	return true;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenu::HandleKeys( int nPK_Button, bool bKeyUp )
{
	if ( NULL == m_pDisplayMenu ) return false;

	bool bHandled = true;

	if ( !bKeyUp ){
		switch( nPK_Button ){
			case BUTTON_Up_Arrow_CONST:
				bHandled = KeyPress( MENU_UP_KEY );
				break;
			case BUTTON_Down_Arrow_CONST:
				bHandled = KeyPress( MENU_DOWN_KEY );
				break;
			case BUTTON_Left_Arrow_CONST:
				bHandled = KeyPress( MENU_LEFT_KEY );
				break;
			case BUTTON_Right_Arrow_CONST:
				bHandled = KeyPress( MENU_RIGHT_KEY );
				break;
			case BUTTON_Enter_CONST:
				bHandled = KeyPress( MENU_EXEC_KEY );
				break;
			case BUTTON_Phone_C_CONST:
			case BUTTON_Rept_Phone_C_CONST:
				bHandled = KeyPress( MENU_BACK_KEY );
				break;
			default:
				bHandled = false;
				break;
		}
	}
	else {
		if ( BUTTON_Phone_Soft_left_CONST==nPK_Button && !IsShowing() ) {
			Show( 0, 0 );
		}
		else if ( nPK_Button>=BUTTON_1_CONST && nPK_Button<BUTTON_0_CONST ) {
			uchar ucKey = '1' + nPK_Button - BUTTON_1_CONST;
			bHandled = KeyPress( ucKey );
		}
		else if ( BUTTON_0_CONST==nPK_Button ) {
			bHandled = KeyPress( '0' );
		}
		else bHandled = false;
	}
	return bHandled;
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenuItem
 *
 */
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::SelectItem( TCHAR ucShortcut )
{
	map<TCHAR, int>::iterator iterPos = m_mShotcuts.find( ucShortcut );
	if ( iterPos == m_mShotcuts.end() ) return false;
	if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
		m_vMenuItems[m_iHighlight]->SetDirty();
	m_iHighlight = iterPos->second;	
	if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
		m_vMenuItems[m_iHighlight]->SetDirty();	

	return true;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::ClearSubmenu()
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		SAFE_DELETE( *iter );
	}
	m_vMenuItems.empty();
	m_uiItemsHeight = m_uiItemsWidth = 0;
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem::~LRMenuItem()
{
	SAFE_DELETE( m_pClone );

	ClearSubmenu();
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem* LRMenuItem::AddItem( LRMenuItemData& ItemData )
{
	LRMenuItem *pItem = new LRMenuItem( ItemData, this );	
	m_vMenuItems.push_back( pItem );
	m_uiItemsHeight = m_uiItemsWidth = 0;
	return pItem;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::AddItem( LRMenuItem* pMenuItem )
{ 
	if ( !pMenuItem ) return;	
	pMenuItem->SetParent( this );
	m_vMenuItems.push_back( pMenuItem );
	m_uiItemsHeight = m_uiItemsWidth = 0;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::Exec( )
{
	if ( HasSubMenu() ) {
		m_bIsOnTop = false;
		Show( );
		return false;
	}

	return DoAction( );
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::MoveCursor( int iDir )
{
	if ( m_iHighlight<0 ) return;

	switch( iDir ){
		case 0: // Up
			if ( m_iHighlight==0 ) break;
			if ( m_iHighlight == m_iStart ) --m_iStart;
			m_vMenuItems[m_iHighlight--]->SetDirty();
			m_vMenuItems[m_iHighlight]->SetDirty();				
			break;
		case 1: //Down
			if ( m_iHighlight == m_vMenuItems.size()-1 ) break;
			if ( m_iHighlight == m_iEnd )	++m_iStart;
			m_vMenuItems[m_iHighlight++]->SetDirty();
			m_vMenuItems[m_iHighlight]->SetDirty();

			break;
	}
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::BeginPaint( RECT r, bool bForceRepaint )
{
	if ( m_bPainting ) return true;

	if ( !m_bDirty && !bForceRepaint ) return false;
	if ( !LocalRenderer::Valid() ) return false;

	m_rClientRect = r;
	m_bPainting = true;
	return true;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	if ( !BeginPaint( r, bForceRepaint ) ) return;

	Rect rr( r );

	// Paint background
	if ( !bHighlight )
		LocalRenderer::FillRect( rr, RGB( 0,0, 255 ) );
	else
		LocalRenderer::FillRect( rr, RGB( 255,255, 0 ) );

	// Draw shortcut
	if ( m_ucShortcut ) {
		tstring sShortcut;
		sShortcut += m_ucShortcut;
		LocalRenderer::DrawText( sShortcut.c_str(), rr, RGB(255, 0, 0 ) );
		rr.left += 10;
	}

	// Draw caption
	LocalRenderer::DrawText( m_sCaption.c_str(), rr, RGB(0,0,0) );

	EndPaint();

}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::CalcSubmenuLayout( RECT rViewport )
{
	Rect r(rViewport);
	int iX = GetSubmenuX();		
	int iY = GetSubmenuY();	

	m_bHasScroll = false;

	if ( GetSubmenuHeight()>r.GetHeight() ) {
		m_bHasScroll = true;
		m_rSubmenu.top = rViewport.top;
		m_rSubmenu.bottom = m_rSubmenu.top+GetSubmenuHeight( rViewport.bottom-DEFAULT_SCROLLBAR_HEIGHT )
			+DEFAULT_SCROLLBAR_HEIGHT;
	}
	else {				
		m_rSubmenu.top = iY;
		if ( m_rSubmenu.top+GetSubmenuHeight()>r.bottom ) 
			m_rSubmenu.top = r.bottom - GetSubmenuHeight();		
		m_rSubmenu.bottom = m_rSubmenu.top+GetSubmenuHeight();
	}
	if ( GetSubmenuWidth()>r.GetWidth() ){
		m_rSubmenu.left = rViewport.left;
		m_rSubmenu.right = rViewport.right;
	}
	else {
		m_rSubmenu.left = iX;
		if ( m_rSubmenu.left+GetSubmenuWidth()>r.right ) 
			m_rSubmenu.left = r.right - GetSubmenuWidth();
		m_rSubmenu.right = m_rSubmenu.left + GetSubmenuWidth();
	}
	m_rSubmenu.right++;
	m_rSubmenu.bottom++;	
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::MakeSubmenuDirty( bool bDirty )
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		( *iter )->SetDirty( bDirty );
	}
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::ShowSubmenu( bool bShow )
{
	CHECK_RENDERER();

	if ( bShow ){	// Show Submenu
		CalcSubmenuLayout( LocalRenderer::GetViewport() );
		// Save background
		m_pClone = LocalRenderer::SaveRect( m_rSubmenu );
		m_bIsShowing = true;
		m_iHighlight = 0;
	}
	else if (m_pClone) {	// Hide submenu		
		// Restore background
		LocalRenderer::RestoreRect( m_rSubmenu.left, m_rSubmenu.top, m_pClone );
		SAFE_DELETE( m_pClone );
		LocalRenderer::Update( m_rSubmenu );
		// Mark submenu as dirty
		MakeSubmenuDirty();
		m_iEnd = m_iStart = 0;
		m_iHighlight = -1;
		m_bIsShowing = false;
	}
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::PaintSubmenu( bool bForceRepaint )
{
	if ( !m_bIsShowing ) return;
	CHECK_RENDERER();

	Rect r(m_rSubmenu );	
	// Draw contour
	LocalRenderer::DrawHRect( r, DEFAULT_BORDER_COLOR );

	// Draw scroll if there's one
	if ( m_bHasScroll ) {
		// To DO:
		//		Paint scroll
		//
		//-------------
		r.bottom -= DEFAULT_SCROLLBAR_HEIGHT;
	}

	// Adjust margins
	r.left +=1; r.top +=1;r.right -=1; r.bottom -=1;
	UINT uiCrtPos = r.top;

	

	// remove shortcuts - reassign shortcuts to items on painting
	m_mShotcuts.clear();
	TCHAR ucShortcut=_T('1');

	// Paint dirty/visible submenu items
	for ( int i=m_iStart ; i<m_vMenuItems.size() && uiCrtPos<r.bottom; ++i )
		if ( m_vMenuItems[i]->GetVisible() ) {
			Rect rItem( r.left, uiCrtPos, r.right, 
				uiCrtPos+m_vMenuItems[i]->GetHeight() );
			m_iEnd = i;
			if ( rItem.bottom>=r.bottom ) {
				rItem.bottom = r.bottom;			
			}
			AddShortcut( i, ucShortcut );
			m_vMenuItems[i]->Paint( rItem, m_iHighlight==i, bForceRepaint );

			LocalRenderer::DrawHLine( rItem.left, rItem.bottom, rItem.GetWidth(), RGB(0,0,0) );
			uiCrtPos += m_vMenuItems[i]->GetHeight()+1;						
			++ucShortcut;
		}

	//Update Dispaly Device
	LocalRenderer::Update( m_rSubmenu );
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::AddShortcut( int iPos, TCHAR ucShortcut )
{
	m_mShotcuts[ucShortcut] = iPos;
	m_vMenuItems[iPos]->SetShortcut( ucShortcut );
}
//---------------------------------------------------------------------------------------------------------
UINT LRMenuItem::GetSubmenuHeight( int iLimit )
{
	// Calculate submenu height by adding all items heights	
	if ( iLimit<0 ){
		if ( !m_uiItemsHeight && m_vMenuItems.size() ) {
			m_uiItemsHeight = 1;
			for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter )
				if ( (*iter)->GetVisible() )
					m_uiItemsHeight += (*iter)->GetHeight()+1;
		}
		return m_uiItemsHeight;
	}

	// if iLimit>=0 limit submenu height to first height smaller than iLimit
	UINT uiItemsHeight = 1;
	vector<LRMenuItem*>::iterator iter;
	for ( iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end() && uiItemsHeight<iLimit ; ++iter ) {
		if ( (*iter)->GetVisible() )
			uiItemsHeight += (*iter)->GetHeight()+1;
	}
	if ( uiItemsHeight>iLimit && iter!=m_vMenuItems.end() ){
		uiItemsHeight -= (*iter)->GetHeight()+1;
	}
	return uiItemsHeight;
}
//---------------------------------------------------------------------------------------------------------
UINT LRMenuItem::GetSubmenuWidth( void )
{
	// Calculate max width of submenu items
	if ( !m_uiItemsWidth && m_vMenuItems.size() ) {		
		for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter )
			if ( (*iter)->GetVisible() ) {
				if ( m_uiItemsWidth < (*iter)->GetWidth() ) 
					m_uiItemsWidth = (*iter)->GetWidth();
			}
		// Add margin width
		m_uiItemsWidth += 2;
	}
	return m_uiItemsWidth;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::HasSubMenu( void )
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		if ( (*iter)->GetVisible() ) return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRPhoneMenu
 *
 */
//---------------------------------------------------------------------------------------------------------
COLORREF LRPhoneMenu::m_clTextColor[tbcNone] = { RGB(0,0,0), RGB(0,0,0), RGB(0,0,0), RGB(0,0,0) };

LRPhoneMenu* LRPhoneMenu::Create()
{
	LRPhoneMenu* pMenu = new LRPhoneMenu;
	pMenu->CreateMenuRoot();
	return pMenu;
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRPhoneMenuItem
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenuItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	if ( !BeginPaint( r, bForceRepaint ) ) return;
	
	// Set background color
	COLORREF clFillColor, clTextColor;
	if ( !bHighlight ) {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcFore );
	}
	else {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlFore );
	}

	Rect rr(r);

	// Paint background
	LocalRenderer::FillRect( rr, clFillColor );

	rr.left += DRAW_ITEMTEXT_MARGIN;
	rr.right -= DRAW_ITEMTEXT_MARGIN;
	rr.top += DRAW_ITEMTEXT_MARGIN;
	rr.bottom -= DRAW_ITEMTEXT_MARGIN;

	// Paint shorcut if he's got one
	if ( m_ucShortcut ) {
		tstring sShortcut;
		sShortcut += m_ucShortcut;		
		LocalRenderer::DrawText( sShortcut.c_str(), rr, DRAW_SHORTCUT_COLOR );
		rr.left += DRAW_SHORTCUT_WIDTH;
	}

	// Draw a triangle if it has submenu
	if ( HasSubMenu() ) {
		LocalRenderer::FillTriangle( rr.right-DRAW_SUBMENUMARK_WIDTH, (rr.top+rr.bottom)/2 - DRAW_SUBMENUMARK_HEIGHT/2, 
									 rr.right-DRAW_SUBMENUMARK_WIDTH, (rr.top+rr.bottom)/2 + DRAW_SUBMENUMARK_HEIGHT/2, 
									 rr.right, (rr.top+rr.bottom)/2, DRAW_SUBMENUMARK_COLOR );
		rr.right -= DRAW_SUBMENUMARK_WIDTH+2;
	}

	// Draw caption
	LocalRenderer::DrawText( m_sCaption.c_str(), rr, clTextColor, m_bStyleBold );

	EndPaint();
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRPhoneMenuItemRoot
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenuItemRoot::RemoveLinks( void )
{
	if ( m_vMenuItems.size()<=1 ) return;

	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin()+1 ; iter!=m_vMenuItems.end(); ++iter ){
		SAFE_DELETE( *iter );
	}
	m_vMenuItems.erase( m_vMenuItems.begin()+1, m_vMenuItems.end() );
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRRoomItem
 *
 */
//---------------------------------------------------------------------------------------------------------
bool LRRoomItem::DoAction( void )
{
	m_pParent->SetCaption( m_sCaption );
	CreateLinks();
	return true;
}
//---------------------------------------------------------------------------------------------------------
void LRRoomItem::CreateLinks( void )
{
	LRCrtRoomItem* pCrtRoom = dynamic_cast<LRCrtRoomItem*>(m_pParent);;
	LRPhoneMenuItemRoot* pRootItem = dynamic_cast<LRPhoneMenuItemRoot*>(pCrtRoom->GetParent());
	if ( !pCrtRoom || !pRootItem ) return;

	pRootItem->RemoveLinks();
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin(); iter!=m_vMenuItems.end(); ++iter ) {
		LRMenuItemLink* pLink = new LRMenuItemLink( *iter, NULL );
		pRootItem->AddItem( pLink );
	}	
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRCrtRoomItem
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRCrtRoomItem::SelectRoom( int iRoomId )
{
	if ( iRoomId<0 ) return;

	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ) {
		LRRoomItem* pRoomItem = dynamic_cast<LRRoomItem*>( *iter );
		if ( !pRoomItem ) return;
		if ( pRoomItem->GetRoomId()==iRoomId ) { // RoomId found
			// Set CrtRoomItem caption  = Room caption
			SetCaption( pRoomItem->GetCaption() );
			// Create links in root menu
			pRoomItem->CreateLinks();
			break;
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void LRCrtRoomItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	if ( !BeginPaint( r, bForceRepaint) ) return;
		
	Rect rr( r );
	// Draw a double contour
	LocalRenderer::DrawHRect( rr, DEFAULT_BORDER_COLOR );
	rr.left++;rr.top++;rr.right--;rr.bottom--;

	// Draw text in bold
	LRPhoneMenuItem::Paint( rr, bHighlight, bForceRepaint );

	EndPaint();
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRSubScenarioItem
 *
 */
//---------------------------------------------------------------------------------------------------------
bool LRSubScenarioItem::DoAction( void )
{
	if ( OrbiterApp::GetInstance() ) {
		OrbiterApp::GetInstance()->SendSelectedItem( m_sId );
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------

