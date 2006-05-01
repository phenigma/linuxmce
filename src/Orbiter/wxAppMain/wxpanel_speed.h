//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXPANEL_SPEED_H_
#define _WXPANEL_SPEED_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_speed.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PANEL_SPEED 10038
#define SYMBOL_WXPANEL_SPEED_STYLE 0
#define SYMBOL_WXPANEL_SPEED_TITLE _T("wx Panel_Speed")
#define SYMBOL_WXPANEL_SPEED_IDNAME ID_PANEL_SPEED
#define SYMBOL_WXPANEL_SPEED_SIZE wxSize(200, 50)
#define SYMBOL_WXPANEL_SPEED_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Speed class declaration
 */

class wxPanel_Speed: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Speed );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxPanel_Speed( );
    wxPanel_Speed( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_SPEED_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_SPEED_POSITION, const wxSize& size = SYMBOL_WXPANEL_SPEED_SIZE, long style = SYMBOL_WXPANEL_SPEED_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_SPEED_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_SPEED_POSITION, const wxSize& size = SYMBOL_WXPANEL_SPEED_SIZE, long style = SYMBOL_WXPANEL_SPEED_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Speed event handler declarations

    /// wxEVT_PAINT event handler for ID_PANEL_SPEED
    void OnPaint( wxPaintEvent& event );

////@end wxPanel_Speed event handler declarations

////@begin wxPanel_Speed member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Speed member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Speed member variables
////@end wxPanel_Speed member variables
public:
    wxArrayInt v_anSpeeds;
    int v_nSpeed;
};

#endif
// _WXPANEL_SPEED_H_
