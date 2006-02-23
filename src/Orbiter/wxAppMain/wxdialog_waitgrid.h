#ifndef _WXDIALOG_WAITGRID_H_
#define _WXDIALOG_WAITGRID_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waitgrid.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/grid.h"
////@end includes
#include "wx_other.h"

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
#define ID_DIALOG_WAITGRID 10020
#define SYMBOL_WXDIALOG_WAITGRID_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_WAITGRID_TITLE _T("Wait Grid")
#define SYMBOL_WXDIALOG_WAITGRID_IDNAME ID_DIALOG_WAITGRID
#define SYMBOL_WXDIALOG_WAITGRID_SIZE wxSize(228, 184)
#define SYMBOL_WXDIALOG_WAITGRID_POSITION wxPoint(114, 184)
#define ID_GRID_WAITGRID 10016
#define ID_TEXTCTRL_WAITGRID 10021
#define ID_GAUGE_WAITGRID 10022
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_WaitGrid class declaration
 */

class wxDialog_WaitGrid: public wxDialog
{
  DECLARE_DYNAMIC_CLASS( wxDialog_WaitGrid )
    DECLARE_EVENT_TABLE()

    public:
  /// Constructors
  wxDialog_WaitGrid( );
  wxDialog_WaitGrid( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITGRID_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITGRID_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITGRID_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITGRID_SIZE, long style = SYMBOL_WXDIALOG_WAITGRID_STYLE );

  /// Creation
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITGRID_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITGRID_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITGRID_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITGRID_SIZE, long style = SYMBOL_WXDIALOG_WAITGRID_STYLE );

  /// Creates the controls and sizers
  void CreateControls();

////@begin wxDialog_WaitGrid event handler declarations

  /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID
  void OnCloseWindow( wxCloseEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
  void OnCancelClick( wxCommandEvent& event );

////@end wxDialog_WaitGrid event handler declarations

////@begin wxDialog_WaitGrid member function declarations

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end wxDialog_WaitGrid member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

////@begin wxDialog_WaitGrid member variables
  wxBoxSizer* v_oBoxV_all;
  wxBoxSizer* v_oBoxH_top;
  wxStaticBitmap* v_oBitmap;
  wxGrid* v_oGrid;
  wxBoxSizer* v_oBoxH_mid;
  wxTextCtrl* v_oInfoText;
  wxBoxSizer* v_oBoxH_bot;
  wxGauge* v_oGauge;
  wxButton* v_oButtonCancel;
////@end wxDialog_WaitGrid member variables
public:
  ~wxDialog_WaitGrid();
  bool Destroy();

protected:
  bool v_bInitialized;
  wxArrayString v_aNames; // old and new

public:
  // refresh when ready
  void SetExternalRefresh(const string &sInfo, const map<string, bool> &mapStrBool, int nBarPercent);

protected:
  virtual void OnInternalIdle();

  wxCriticalSection v_oCriticalRefresh;
  bool v_bShouldRefresh;
  wxString v_refresh_sInfo;
  map<string, bool> v_refresh_mapStrBool;
  int v_refresh_nBarPercent;
};

bool wxDialog_WaitGrid_Refresh(const string &sInfo, const map<string, bool> &mapStrBool, int nBarPercent);

#endif
// _WXDIALOG_WAITGRID_H_
