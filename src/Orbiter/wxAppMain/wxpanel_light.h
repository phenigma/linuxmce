//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WXPANEL_LIGHT_H_
#define _WXPANEL_LIGHT_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_light.h"
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
#define ID_PANEL_LIGHT 10043
#define SYMBOL_WXPANEL_LIGHT_STYLE wxRESIZE_BORDER
#define SYMBOL_WXPANEL_LIGHT_TITLE _T("wx Panel_Light")
#define SYMBOL_WXPANEL_LIGHT_IDNAME ID_PANEL_LIGHT
#define SYMBOL_WXPANEL_LIGHT_SIZE wxSize(10, 100)
#define SYMBOL_WXPANEL_LIGHT_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Light class declaration
 */

class wxPanel_Light: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Light );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxPanel_Light( );
    wxPanel_Light( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_LIGHT_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_LIGHT_POSITION, const wxSize& size = SYMBOL_WXPANEL_LIGHT_SIZE, long style = SYMBOL_WXPANEL_LIGHT_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_LIGHT_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_LIGHT_POSITION, const wxSize& size = SYMBOL_WXPANEL_LIGHT_SIZE, long style = SYMBOL_WXPANEL_LIGHT_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Light event handler declarations

    /// wxEVT_PAINT event handler for ID_PANEL_LIGHT
    void OnPaint( wxPaintEvent& event );

////@end wxPanel_Light event handler declarations

////@begin wxPanel_Light member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Light member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Light member variables
////@end wxPanel_Light member variables

public:
    int v_nPositions;
    int v_nCrtPosition;
};

#endif
// _WXPANEL_LIGHT_H_
