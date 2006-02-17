#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_waitgrid.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxdialog_waitgrid.h"
#include "wx_other.h"
#include "wxthreadbag.h"

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitGrid type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitGrid, wxDialog )
  ;

/*!
 * wxDialog_WaitGrid event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitGrid, wxDialog )

////@begin wxDialog_WaitGrid event table entries
  EVT_CLOSE( wxDialog_WaitGrid::OnCloseWindow )

  EVT_BUTTON( wxID_CANCEL, wxDialog_WaitGrid::OnCancelClick )

////@end wxDialog_WaitGrid event table entries

  END_EVENT_TABLE()
  ;

wxDialog_WaitGrid *g_pwxDialog_WaitGrid = NULL;

/*!
 * wxDialog_WaitGrid constructors
 */

wxDialog_WaitGrid::wxDialog_WaitGrid( )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitGrid::wxDialog_WaitGrid()");
  if (g_pwxDialog_WaitGrid != NULL)
  {
    _wx_log_wrn("wxDialog_WaitGrid::wxDialog_WaitGrid() : Object is not unique, addr=%p", g_pwxDialog_WaitGrid);
  }
  g_pwxDialog_WaitGrid = this;
}

wxDialog_WaitGrid::wxDialog_WaitGrid( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitGrid::wxDialog_WaitGrid(*)");
  if (g_pwxDialog_WaitGrid != NULL)
  {
    _wx_log_wrn("wxDialog_WaitGrid::wxDialog_WaitGrid(*) : Object is not unique, addr=%p", g_pwxDialog_WaitGrid);
  }
  g_pwxDialog_WaitGrid = this;
  Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitGrid creator
 */

bool wxDialog_WaitGrid::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_WaitGrid member initialisation
  v_oBoxV_all = NULL;
  v_oBoxH_top = NULL;
  v_oBitmap = NULL;
  v_oGrid = NULL;
  v_oBoxH_mid = NULL;
  v_oInfoText = NULL;
  v_oBoxH_bot = NULL;
  v_oGauge = NULL;
  v_oButtonCancel = NULL;
////@end wxDialog_WaitGrid member initialisation

////@begin wxDialog_WaitGrid creation
  SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
////@end wxDialog_WaitGrid creation
  return true;
}

/*!
 * Control creation for wxDialog_WaitGrid
 */

void wxDialog_WaitGrid::CreateControls()
{
  _wx_log_nfo("wxDialog_WaitGrid::CreateControls()");
////@begin wxDialog_WaitGrid content construction
  wxDialog_WaitGrid* itemDialog1 = this;

  this->SetForegroundColour(wxColour(255, 255, 255));
  this->SetBackgroundColour(wxColour(224, 224, 240));
  this->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxV_all = new wxBoxSizer(wxVERTICAL);
  itemDialog1->SetSizer(v_oBoxV_all);

  v_oBoxH_top = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_top, 1, wxGROW|wxALL, 5);

  wxBitmap v_oBitmapBitmap(itemDialog1->GetBitmapResource(wxT("logo_pluto.jpg")));
  v_oBitmap = new wxStaticBitmap;
  v_oBitmap->Create( itemDialog1, wxID_STATIC, v_oBitmapBitmap, wxDefaultPosition, itemDialog1->ConvertDialogToPixels(wxSize(126, 87)), 0 );
  v_oBoxH_top->Add(v_oBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

  v_oGrid = new wxGrid( itemDialog1, ID_GRID_WAITGRID, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN |wxVSCROLL );
  v_oGrid->SetForegroundColour(wxColour(255, 255, 255));
  v_oGrid->SetBackgroundColour(wxColour(0, 0, 255));
  v_oGrid->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oGrid->SetDefaultColSize(50);
  v_oGrid->SetDefaultRowSize(20);
  v_oGrid->SetColLabelSize(0);
  v_oGrid->SetRowLabelSize(0);
  v_oGrid->CreateGrid(4, 5, wxGrid::wxGridSelectRows);
  v_oBoxH_top->Add(v_oGrid, 1, wxGROW|wxALL, 10);

  v_oBoxH_mid = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_mid, 1, wxGROW|wxALL, 5);

  v_oInfoText = new wxTextCtrl;
  v_oInfoText->Create( itemDialog1, ID_TEXTCTRL_WAITGRID, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
  v_oInfoText->SetBackgroundColour(wxColour(224, 224, 240));
  v_oInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_mid->Add(v_oInfoText, 1, wxGROW|wxALL, 10);

  v_oBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_bot, 0, wxGROW|wxALL, 5);

  v_oGauge = new wxGauge;
  v_oGauge->Create( itemDialog1, ID_GAUGE_WAITGRID, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH );
  v_oGauge->SetValue(50);
  v_oGauge->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_bot->Add(v_oGauge, 1, wxALIGN_CENTER_VERTICAL|wxALL, 10);

  v_oButtonCancel = new wxButton;
  v_oButtonCancel->Create( itemDialog1, wxID_CANCEL, _T("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
  v_oButtonCancel->SetDefault();
  v_oButtonCancel->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_bot->Add(v_oButtonCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

////@end wxDialog_WaitGrid content construction
  v_oInfoText->SetValue("");
  v_oGauge->SetValue(0);
  // clear grid
  v_oGrid->ClearGrid();
  v_oGrid->DeleteCols(0, v_oGrid->GetNumberCols());
  v_oGrid->DeleteRows(0, v_oGrid->GetNumberRows());
  v_oGrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
  v_oGrid->SetDefaultCellBackgroundColour(wxColour(0, 0, 127));
  v_oGrid->SetDefaultCellTextColour(wxColour(200, 200, 200));
  // add columns
  v_oGrid->AppendCols(2);
  v_oGrid->SetColMinimalAcceptableWidth(10);
  v_oGrid->SetColMinimalWidth(0, 10);
  v_oGrid->SetColMinimalWidth(1, 100);
  v_oGrid->SetColSize(0, 20);
  v_oGrid->SetColSize(1, 200);
  v_bInitialized = true;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID
 */

void wxDialog_WaitGrid::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid.
  // Before editing this code, remove the block markers.
  wxWindow* window = this;
  window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid.
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void wxDialog_WaitGrid::OnCancelClick( wxCommandEvent& WXUNUSED(event) )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in wxDialog_WaitGrid.
  // Before editing this code, remove the block markers.
  Destroy();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in wxDialog_WaitGrid.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_WaitGrid::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_WaitGrid::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin wxDialog_WaitGrid bitmap retrieval
  wxUnusedVar(name);
  if (name == _T("logo_pluto.jpg"))
  {
    wxBitmap bitmap(logo_pluto_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end wxDialog_WaitGrid bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_WaitGrid::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin wxDialog_WaitGrid icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end wxDialog_WaitGrid icon retrieval
}

//==================================================

wxDialog_WaitGrid::~wxDialog_WaitGrid()
{
  _wx_log_nfo("wxDialog_WaitGrid::~wxDialog_WaitGrid()");
  if (g_pwxDialog_WaitGrid == this)
    g_pwxDialog_WaitGrid = NULL;
}

bool wxDialog_WaitGrid::Destroy()
{
  _wx_log_nfo("wxDialog_WaitGrid::Destroy()");
  return wxDialog::Destroy();
}

void wxDialog_WaitGrid::SetExternalRefresh(const string &sInfo, const map<string, bool> &mapStrBool, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitGrid::SetExternalRefresh(*)");
  if (! v_bInitialized)
  {
    _wx_log_wrn("Dialog is not initialized, ignoring the request");
    return;
  }
  wxCriticalSectionLocker lock(v_oCriticalRefresh);
  v_refresh_sInfo = sInfo;
  v_refresh_mapStrBool = mapStrBool;
  v_refresh_nBarPercent = nBarPercent;
  v_bShouldRefresh = true;
}

void wxDialog_WaitGrid::OnInternalIdle()
{
  //--_wx_log_nfo("wxDialog_WaitGrid::OnInternalIdle()");
  if ( wxIdleThreadShouldStop() )
    Destroy();
  if (v_bInitialized && v_bShouldRefresh && ::wxIsMainThread())
  {
    wxCriticalSectionLocker lock(v_oCriticalRefresh);
    // update info text
    v_oInfoText->SetValue(v_refresh_sInfo);
    // update grid
    for(map<string, bool>::iterator it = v_refresh_mapStrBool.begin(); it != v_refresh_mapStrBool.end(); ++it)
    {
      wxString sName = it->first.c_str();
      bool bValue = it->second;
      // search string in current data
      // add item if not found
      int idx = v_aNames.Index(sName);
      if (idx == wxNOT_FOUND)
      {
        v_aNames.Add(sName);
        v_oGrid->AppendRows(1);
        idx = v_aNames.GetCount();
        v_oGrid->SetCellValue(idx, 1, sName);
      }
      if (bValue)
      {
        v_oGrid->SetCellValue(idx, 0, "Started");
        v_oGrid->SetCellTextColour(idx, 0, wxColour(0, 200, 0));
      }
      else
      {
        v_oGrid->SetCellValue(idx, 0, "Waiting");
        v_oGrid->SetCellTextColour(idx, 0, wxColour(200, 0, 0));
      }
    }
    // update progress bar
    v_oGauge->SetValue(v_refresh_nBarPercent);
    v_bShouldRefresh = false;
  }
  wxDialog::OnInternalIdle();
}

//==================================================

bool wxDialog_WaitGrid_Refresh(const string &sInfo, const map<string, bool> &mapStrBool, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitGrid_Refresh(*)");
  if (! (g_pwxDialog_WaitGrid != NULL))
  {
    _wx_log_wrn("Dialog is not active, ignoring SetSize request");
    return false;
  }
  _wx_log_nfo("Allocated at %p", g_pwxDialog_WaitGrid);
  g_pwxDialog_WaitGrid->SetExternalRefresh(sInfo, mapStrBool, nBarPercent);
  _wx_log_nfo("wxDialog_WaitGrid_Refresh(*);;");
  return true;
}

//==================================================
