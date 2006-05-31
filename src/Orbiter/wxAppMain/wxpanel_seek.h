//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WXPANEL_SEEK_H_
#define _WXPANEL_SEEK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_seek.h"
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
#define ID_PANEL_SEEK 10037
#define SYMBOL_WXPANEL_SEEK_STYLE wxRESIZE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXPANEL_SEEK_TITLE _T("wx Panel_Seek")
#define SYMBOL_WXPANEL_SEEK_IDNAME ID_PANEL_SEEK
#define SYMBOL_WXPANEL_SEEK_SIZE wxSize(100, 10)
#define SYMBOL_WXPANEL_SEEK_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Seek class declaration
 */

class wxPanel_Seek: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Seek );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxPanel_Seek( );
    wxPanel_Seek( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_SEEK_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_SEEK_POSITION, const wxSize& size = SYMBOL_WXPANEL_SEEK_SIZE, long style = SYMBOL_WXPANEL_SEEK_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_SEEK_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_SEEK_POSITION, const wxSize& size = SYMBOL_WXPANEL_SEEK_SIZE, long style = SYMBOL_WXPANEL_SEEK_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Seek event handler declarations

    /// wxEVT_PAINT event handler for ID_PANEL_SEEK
    void OnPaint( wxPaintEvent& event );

////@end wxPanel_Seek event handler declarations

////@begin wxPanel_Seek member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Seek member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Seek member variables
////@end wxPanel_Seek member variables

public:
    double v_dPosPercent;
};

#endif
// _WXPANEL_SEEK_H_
