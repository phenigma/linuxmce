//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_SLIDERMEDIA_H_
#define _WXDIALOG_SLIDERMEDIA_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_slidermedia.h"
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
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SLIDERMEDIA 10017
#define SYMBOL_WXDIALOG_SLIDERMEDIA_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_SLIDERMEDIA_TITLE _T("Slider Media")
#define SYMBOL_WXDIALOG_SLIDERMEDIA_IDNAME ID_DIALOG_SLIDERMEDIA
#define SYMBOL_WXDIALOG_SLIDERMEDIA_SIZE wxSize(200, 50)
#define SYMBOL_WXDIALOG_SLIDERMEDIA_POSITION wxPoint(808, 200)
#define ID_SLIDER 10025
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_SliderMedia class declaration
 */

class wxDialog_SliderMedia: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_SliderMedia )
        ;
    DECLARE_EVENT_TABLE()
        ;

public:
    /// Constructors
    wxDialog_SliderMedia( );
    wxDialog_SliderMedia( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SLIDERMEDIA_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SLIDERMEDIA_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SLIDERMEDIA_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SLIDERMEDIA_SIZE, long style = SYMBOL_WXDIALOG_SLIDERMEDIA_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SLIDERMEDIA_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SLIDERMEDIA_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SLIDERMEDIA_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SLIDERMEDIA_SIZE, long style = SYMBOL_WXDIALOG_SLIDERMEDIA_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_SliderMedia event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SLIDERMEDIA
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_SIZE event handler for ID_DIALOG_SLIDERMEDIA
    void OnSize( wxSizeEvent& event );

////@end wxDialog_SliderMedia event handler declarations

////@begin wxDialog_SliderMedia member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_SliderMedia member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_SliderMedia member variables
    wxBoxSizer* v_pBoxV_all;
////@end wxDialog_SliderMedia member variables
};

#endif
// _WXDIALOG_SLIDERMEDIA_H_
