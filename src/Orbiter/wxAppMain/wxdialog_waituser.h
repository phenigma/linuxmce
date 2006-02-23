#ifndef _WXDIALOG_WAITUSER_H_
#define _WXDIALOG_WAITUSER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waituser.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wx_other.h"

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
#define ID_DIALOG_WAITUSER 10005
#define SYMBOL_WXDIALOG_WAITUSER_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_WAITUSER_TITLE _T("Wait User")
#define SYMBOL_WXDIALOG_WAITUSER_IDNAME ID_DIALOG_WAITUSER
#define SYMBOL_WXDIALOG_WAITUSER_SIZE wxSize(130, 113)
#define SYMBOL_WXDIALOG_WAITUSER_POSITION wxPoint(65, 113)
#define ID_TEXTCTRL_WAITUSER 10014
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_WaitUser class declaration
 */

class wxDialog_WaitUser: public wxDialog
{
  DECLARE_DYNAMIC_CLASS( wxDialog_WaitUser )
    DECLARE_EVENT_TABLE()

    public:
  /// Constructors
  wxDialog_WaitUser( );
  wxDialog_WaitUser( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITUSER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITUSER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITUSER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITUSER_SIZE, long style = SYMBOL_WXDIALOG_WAITUSER_STYLE );

  /// Creation
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITUSER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITUSER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITUSER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITUSER_SIZE, long style = SYMBOL_WXDIALOG_WAITUSER_STYLE );

  /// Creates the controls and sizers
  void CreateControls();

////@begin wxDialog_WaitUser event handler declarations

  /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER
  void OnCloseWindow( wxCloseEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
  void OnOkClick( wxCommandEvent& event );

////@end wxDialog_WaitUser event handler declarations

////@begin wxDialog_WaitUser member function declarations

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end wxDialog_WaitUser member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

////@begin wxDialog_WaitUser member variables
  wxBoxSizer* v_oBoxV_all;
  wxBoxSizer* v_oBoxH_top;
  wxStaticBitmap* v_oBitmap;
  wxBoxSizer* v_oBoxH_mid;
  wxTextCtrl* v_oInfoText;
  wxBoxSizer* v_oBoxH_bot;
  wxButton* v_oButtonOk;
////@end wxDialog_WaitUser member variables
public:
  ~wxDialog_WaitUser();
  bool Destroy();

protected:
  bool v_bInitialized;
  wxArrayString v_aNames; // old and new

public:
  // refresh when ready
  void SetExternalRefresh(const string &sInfo, int nBarPercent);

protected:
  virtual void OnInternalIdle();

  wxCriticalSection v_oCriticalRefresh;
  bool v_bShouldRefresh;
  wxString v_refresh_sInfo;
  int v_refresh_nBarPercent;
};

// call from any thread
bool wxDialog_WaitUser_Refresh(const string &sInfo, int nBarPercent); // create a refresh event

#endif
// _WXDIALOG_WAITUSER_H_
