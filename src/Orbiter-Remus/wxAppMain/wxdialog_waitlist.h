#ifndef _WXDIALOG_WAITLIST_H_
#define _WXDIALOG_WAITLIST_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waitlist.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/grid.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_WAITLIST 10005
#define SYMBOL_WXDIALOG_WAITLIST_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_WAITLIST_TITLE _T("Wait List")
#define SYMBOL_WXDIALOG_WAITLIST_IDNAME ID_DIALOG_WAITLIST
#define SYMBOL_WXDIALOG_WAITLIST_SIZE wxSize(400, 300)
#define SYMBOL_WXDIALOG_WAITLIST_POSITION wxPoint(200, 300)
#define ID_GRID_WL 10000
#define ID_GAUGE 10013
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

class wxDialog_WaitList: public wxDialog
{
  DECLARE_DYNAMIC_CLASS( wxDialog_WaitList )
    DECLARE_EVENT_TABLE()

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
    wxBoxSizer* v_oBoxV_all;
    wxBoxSizer* v_oBoxH;
    wxStaticBitmap* v_oBitmap;
    wxGrid* v_oGrid;
    wxGauge* v_oGauge;
////@end wxDialog_WaitList member variables
public:
  ~wxDialog_WaitList();
  bool Destroy();
};

// call from the main thread
void wxDialog_WaitList_Show();
void wxDialog_WaitList_Close();

#endif
// _WXDIALOG_WAITLIST_H_
