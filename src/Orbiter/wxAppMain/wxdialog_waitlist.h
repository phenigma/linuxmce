//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_WAITLIST_H_
#define _WXDIALOG_WAITLIST_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waitlist.h"
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
#define ID_DIALOG_WAITLIST 10023
#define SYMBOL_WXDIALOG_WAITLIST_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_WAITLIST_TITLE _T("Wait List")
#define SYMBOL_WXDIALOG_WAITLIST_IDNAME ID_DIALOG_WAITLIST
#define SYMBOL_WXDIALOG_WAITLIST_SIZE wxSize(400, 300)
#define SYMBOL_WXDIALOG_WAITLIST_POSITION wxPoint(427, 325)
#define ID_TEXTCTRL_WAITLIST 10015
#define ID_GAUGE_WAITLIST 10013
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_WaitList class declaration
 */

class wxDialog_WaitList: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_WaitList )
        ;
    DECLARE_EVENT_TABLE()
        ;

public:
    /// Constructors
    wxDialog_WaitList( );
    wxDialog_WaitList( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITLIST_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITLIST_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITLIST_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITLIST_SIZE, long style = SYMBOL_WXDIALOG_WAITLIST_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITLIST_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITLIST_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITLIST_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITLIST_SIZE, long style = SYMBOL_WXDIALOG_WAITLIST_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_WaitList event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST
    void OnCloseWindow( wxCloseEvent& event );

////@end wxDialog_WaitList event handler declarations

////@begin wxDialog_WaitList member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_WaitList member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_WaitList member variables
    wxBoxSizer* v_pBoxV_all;
    wxBoxSizer* v_pBoxH_top;
    wxStaticBitmap* v_pBitmap;
    wxBoxSizer* v_pBoxH_mid;
    wxTextCtrl* v_pLogText;
    wxTextCtrl* v_pInfoText;
    wxBoxSizer* v_pBoxH_bot;
    wxGauge* v_pGauge;
////@end wxDialog_WaitList member variables

public:
    ~wxDialog_WaitList();
    virtual void Gui_Refresh(void *pExternData);

    struct Data_Refresh
    {
        string m_sMessage;
        int m_nPercent;
    };

protected:
    wxString v_sPrevStr; // previous string message
};

#endif
// _WXDIALOG_WAITLIST_H_
