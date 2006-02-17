#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_waituser.h"
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

#include "wxdialog_waituser.h"
#include "wx_other.h"
#include "wxthreadbag.h"

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitUser type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitUser, wxDialog )
  ;

/*!
 * wxDialog_WaitUser event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitUser, wxDialog )

////@begin wxDialog_WaitUser event table entries
  EVT_CLOSE( wxDialog_WaitUser::OnCloseWindow )

  EVT_BUTTON( wxID_OK, wxDialog_WaitUser::OnOkClick )

////@end wxDialog_WaitUser event table entries

  END_EVENT_TABLE()
  ;

wxDialog_WaitUser *g_pwxDialog_WaitUser = NULL;

/*!
 * wxDialog_WaitUser constructors
 */

wxDialog_WaitUser::wxDialog_WaitUser( )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitUser::wxDialog_WaitUser()");
  if (g_pwxDialog_WaitUser != NULL)
  {
    _wx_log_wrn("wxDialog_WaitUser::wxDialog_WaitUser() : Object is not unique, addr=%p", g_pwxDialog_WaitUser);
  }
  g_pwxDialog_WaitUser = this;
}

wxDialog_WaitUser::wxDialog_WaitUser( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitUser::wxDialog_WaitUser(*)");
  if (g_pwxDialog_WaitUser != NULL)
  {
    _wx_log_wrn("wxDialog_WaitUser::wxDialog_WaitUser(*) : Object is not unique, addr=%p", g_pwxDialog_WaitUser);
  }
  g_pwxDialog_WaitUser = this;
  Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitUser creator
 */

bool wxDialog_WaitUser::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_WaitUser member initialisation
  v_oBoxV_all = NULL;
  v_oBoxH_top = NULL;
  v_oBitmap = NULL;
  v_oBoxH_mid = NULL;
  v_oInfoText = NULL;
  v_oBoxH_bot = NULL;
  v_oButtonOk = NULL;
////@end wxDialog_WaitUser member initialisation

////@begin wxDialog_WaitUser creation
  SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
////@end wxDialog_WaitUser creation
  return true;
}

/*!
 * Control creation for wxDialog_WaitUser
 */

void wxDialog_WaitUser::CreateControls()
{
  _wx_log_nfo("wxDialog_WaitUser::CreateControls()");
////@begin wxDialog_WaitUser content construction
  wxDialog_WaitUser* itemDialog1 = this;

  this->SetForegroundColour(wxColour(255, 255, 255));
  this->SetBackgroundColour(wxColour(224, 224, 240));
  this->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxV_all = new wxBoxSizer(wxVERTICAL);
  itemDialog1->SetSizer(v_oBoxV_all);

  v_oBoxH_top = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_top, 0, wxGROW|wxALL, 5);

  wxBitmap v_oBitmapBitmap(itemDialog1->GetBitmapResource(wxT("logo_pluto.jpg")));
  v_oBitmap = new wxStaticBitmap;
  v_oBitmap->Create( itemDialog1, wxID_STATIC, v_oBitmapBitmap, wxDefaultPosition, itemDialog1->ConvertDialogToPixels(wxSize(126, 87)), 0 );
  v_oBoxH_top->Add(v_oBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

  v_oBoxH_mid = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_mid, 1, wxGROW|wxALL, 5);

  v_oInfoText = new wxTextCtrl;
  v_oInfoText->Create( itemDialog1, ID_TEXTCTRL_WAITUSER, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
  v_oInfoText->SetBackgroundColour(wxColour(224, 224, 240));
  v_oInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_mid->Add(v_oInfoText, 1, wxGROW|wxALL, 10);

  v_oBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_bot, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  v_oButtonOk = new wxButton;
  v_oButtonOk->Create( itemDialog1, wxID_OK, _T("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
  v_oButtonOk->SetDefault();
  v_oButtonOk->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_bot->Add(v_oButtonOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxDialog_WaitUser content construction
  v_oInfoText->SetValue("");
  v_bInitialized = true;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER
 */

void wxDialog_WaitUser::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER in wxDialog_WaitUser.
  // Before editing this code, remove the block markers.
  wxWindow* window = this;
  window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER in wxDialog_WaitUser.
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void wxDialog_WaitUser::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK in wxDialog_WaitUser.
  // Before editing this code, remove the block markers.
  Destroy();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK in wxDialog_WaitUser.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_WaitUser::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_WaitUser::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin wxDialog_WaitUser bitmap retrieval
  wxUnusedVar(name);
  if (name == _T("logo_pluto.jpg"))
  {
    wxBitmap bitmap(logo_pluto_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end wxDialog_WaitUser bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_WaitUser::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin wxDialog_WaitUser icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end wxDialog_WaitUser icon retrieval
}

//==================================================

wxDialog_WaitUser::~wxDialog_WaitUser()
{
  _wx_log_nfo("wxDialog_WaitUser::~wxDialog_WaitUser()");
  if (g_pwxDialog_WaitUser == this)
    g_pwxDialog_WaitUser = NULL;
}

bool wxDialog_WaitUser::Destroy()
{
  _wx_log_nfo("wxDialog_WaitUser::Destroy()");
  return wxDialog::Destroy();
}

void wxDialog_WaitUser::SetExternalRefresh(const string &sInfo, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitUser::SetExternalRefresh(*)");
  if (! v_bInitialized)
  {
    _wx_log_wrn("Dialog is not initialized, ignoring the request");
    return;
  }
  wxCriticalSectionLocker lock(v_oCriticalRefresh);
  v_refresh_sInfo = sInfo;
  v_refresh_nBarPercent = nBarPercent;
  v_bShouldRefresh = true;
}

void wxDialog_WaitUser::OnInternalIdle()
{
  //--_wx_log_nfo("wxDialog_WaitUser::OnInternalIdle()");
  if ( wxIdleThreadShouldStop() )
    Destroy();
  if (v_bInitialized && v_bShouldRefresh && ::wxIsMainThread())
  {
    wxCriticalSectionLocker lock(v_oCriticalRefresh);
    // update info text
    v_oInfoText->SetValue(v_refresh_sInfo);
    v_bShouldRefresh = false;
  }
  wxDialog::OnInternalIdle();
}

//==================================================

bool wxDialog_WaitUser_Refresh(const string &sInfo, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitUser_Refresh(*)");
  if (! (g_pwxDialog_WaitUser != NULL))
  {
    _wx_log_wrn("Dialog is not active, ignoring SetSize request");
    return false;
  }
  _wx_log_nfo("Allocated at %p", g_pwxDialog_WaitUser);
  g_pwxDialog_WaitUser->SetExternalRefresh(sInfo, nBarPercent);
  _wx_log_nfo("wxDialog_WaitUser_Refresh(*);;");
  return true;
}

//==================================================
