//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXPANEL_VOLUME_H_
#define _WXPANEL_VOLUME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_volume.h"
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
#define ID_PANEL_VOLUME 10042
#define SYMBOL_WXPANEL_VOLUME_STYLE 0
#define SYMBOL_WXPANEL_VOLUME_TITLE _T("wx Panel_Volume")
#define SYMBOL_WXPANEL_VOLUME_IDNAME ID_PANEL_VOLUME
#define SYMBOL_WXPANEL_VOLUME_SIZE wxSize(200, 50)
#define SYMBOL_WXPANEL_VOLUME_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Volume class declaration
 */

class wxPanel_Volume: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Volume );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxPanel_Volume( );
    wxPanel_Volume( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_VOLUME_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_VOLUME_POSITION, const wxSize& size = SYMBOL_WXPANEL_VOLUME_SIZE, long style = SYMBOL_WXPANEL_VOLUME_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_VOLUME_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_VOLUME_POSITION, const wxSize& size = SYMBOL_WXPANEL_VOLUME_SIZE, long style = SYMBOL_WXPANEL_VOLUME_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Volume event handler declarations

////@end wxPanel_Volume event handler declarations

////@begin wxPanel_Volume member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Volume member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Volume member variables
////@end wxPanel_Volume member variables
};

#endif
// _WXPANEL_VOLUME_H_
