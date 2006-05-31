//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WXPANEL_TIME_H_
#define _WXPANEL_TIME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxpanel_time.h"
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
class wxBoxSizer;
class wxPanel_Arrow;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PANEL_TIME 10036
#define SYMBOL_WXPANEL_TIME_STYLE wxRESIZE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXPANEL_TIME_TITLE _T("wx Panel_Time")
#define SYMBOL_WXPANEL_TIME_IDNAME ID_PANEL_TIME
#define SYMBOL_WXPANEL_TIME_SIZE wxSize(100, 75)
#define SYMBOL_WXPANEL_TIME_POSITION wxDefaultPosition
#define ID_SLIDER_TIME 10000
#define ID_CTRL_TIME_ARROW 10029
#define wxID_TIME_START 10030
#define wxID_TIME_NOW 10031
#define wxID_TIME_END 10032
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxPanel_Time class declaration
 */

class wxPanel_Time: public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxPanel_Time );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxPanel_Time( );
    wxPanel_Time( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_TIME_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_TIME_POSITION, const wxSize& size = SYMBOL_WXPANEL_TIME_SIZE, long style = SYMBOL_WXPANEL_TIME_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPANEL_TIME_IDNAME, const wxPoint& pos = SYMBOL_WXPANEL_TIME_POSITION, const wxSize& size = SYMBOL_WXPANEL_TIME_SIZE, long style = SYMBOL_WXPANEL_TIME_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxPanel_Time event handler declarations

////@end wxPanel_Time event handler declarations

////@begin wxPanel_Time member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxPanel_Time member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxPanel_Time member variables
    wxBoxSizer* v_pBoxV_all;
    wxSlider* v_pSliderTime;
    wxPanel_Arrow* v_pPanel_Arrow;
    wxBoxSizer* v_pBoxH_Start_End;
    wxStaticText* v_pTimeStart;
    wxStaticText* v_pTimeNow;
    wxStaticText* v_pTimeEnd;
////@end wxPanel_Time member variables

public:
    virtual void Refresh(bool eraseBackground = true, const wxRect* rect = NULL);

    int v_nTimeStart;
    int v_nTimeNow;
    int v_nTimeEnd;
};

#endif
// _WXPANEL_TIME_H_
