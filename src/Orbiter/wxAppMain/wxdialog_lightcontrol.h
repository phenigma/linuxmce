//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_LIGHTCONTROL_H_
#define _WXDIALOG_LIGHTCONTROL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_lightcontrol.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wxdialog_base.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxPanel_Light;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_LIGHTCONTROL 10028
#define SYMBOL_WXDIALOG_LIGHTCONTROL_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_LIGHTCONTROL_TITLE _T("wx Dialog_LightControl")
#define SYMBOL_WXDIALOG_LIGHTCONTROL_IDNAME ID_DIALOG_LIGHTCONTROL
#define SYMBOL_WXDIALOG_LIGHTCONTROL_SIZE wxDefaultSize
#define SYMBOL_WXDIALOG_LIGHTCONTROL_POSITION wxDefaultPosition
#define ID_CTRL_LIGHT 10045
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_LightControl class declaration
 */

class wxDialog_LightControl: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_LightControl );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_LightControl( );
    wxDialog_LightControl( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_LIGHTCONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_LIGHTCONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_LIGHTCONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_LIGHTCONTROL_SIZE, long style = SYMBOL_WXDIALOG_LIGHTCONTROL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_LIGHTCONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_LIGHTCONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_LIGHTCONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_LIGHTCONTROL_SIZE, long style = SYMBOL_WXDIALOG_LIGHTCONTROL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_LightControl event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_LIGHTCONTROL
    void OnCloseWindow( wxCloseEvent& event );

////@end wxDialog_LightControl event handler declarations

////@begin wxDialog_LightControl member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_LightControl member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_LightControl member variables
    wxBoxSizer* v_pBoxH_all;
    wxPanel_Light* v_pPanel_Light;
////@end wxDialog_LightControl member variables

public:
    virtual bool Gui_Refresh(CallBackData *pCallBackData);
};

#endif
// _WXDIALOG_LIGHTCONTROL_H_
