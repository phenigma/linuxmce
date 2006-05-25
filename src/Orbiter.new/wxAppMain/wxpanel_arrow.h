#ifndef _WXPANEL_ARROW_H_
#define _WXPANEL_ARROW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_arrow.h"
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
#define ID_PANEL_ARROW 10017
#define SYMBOL_WXPANEL_ARROW_STYLE wxRESIZE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXPANEL_ARROW_TITLE _T("wxPanel_Arrow")
#define SYMBOL_WXPANEL_ARROW_IDNAME ID_PANEL_ARROW
#define SYMBOL_WXPANEL_ARROW_SIZE wxSize(100, 10)
#define SYMBOL_WXPANEL_ARROW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Arrow class declaration
 */

class wxPanel_Arrow: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Arrow )
        DECLARE_EVENT_TABLE()

        public:
    /// Constructors
    wxPanel_Arrow( );
    wxPanel_Arrow( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_ARROW_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_ARROW_POSITION, const wxSize& size = SYMBOL_WXPANEL_ARROW_SIZE, long style = SYMBOL_WXPANEL_ARROW_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_ARROW_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_ARROW_POSITION, const wxSize& size = SYMBOL_WXPANEL_ARROW_SIZE, long style = SYMBOL_WXPANEL_ARROW_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Arrow event handler declarations

    /// wxEVT_PAINT event handler for ID_PANEL_ARROW
    void OnPaint( wxPaintEvent& event );

////@end wxPanel_Arrow event handler declarations

////@begin wxPanel_Arrow member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Arrow member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Arrow member variables
////@end wxPanel_Arrow member variables

public:
    double v_dPosPercent;
};

#endif
// _WXPANEL_ARROW_H_
