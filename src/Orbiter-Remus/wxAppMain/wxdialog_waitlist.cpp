#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_waitlist.h"
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

#include "wxdialog_waitlist.h"
#include "wx_other.h"
#include "wxthreadbag.h"

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitList type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitList, wxDialog )
  ;

/*!
 * wxDialog_WaitList event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitList, wxDialog )

////@begin wxDialog_WaitList event table entries
  EVT_CLOSE( wxDialog_WaitList::OnCloseWindow )

////@end wxDialog_WaitList event table entries

  END_EVENT_TABLE()
  ;

wxDialog_WaitList *g_pwxDialog_WaitList = NULL;

/*!
 * wxDialog_WaitList constructors
 */

wxDialog_WaitList::wxDialog_WaitList( )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitList::wxDialog_WaitList()");
  if (g_pwxDialog_WaitList != NULL)
  {
    _wx_log_wrn("wxDialog_WaitList::wxDialog_WaitList() : Object is not unique, addr=%p", g_pwxDialog_WaitList);
  }
  g_pwxDialog_WaitList = this;
}

wxDialog_WaitList::wxDialog_WaitList( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : v_bInitialized(false), v_bShouldRefresh(false)
{
  _wx_log_nfo("wxDialog_WaitList::wxDialog_WaitList(*)");
  if (g_pwxDialog_WaitList != NULL)
  {
    _wx_log_wrn("wxDialog_WaitList::wxDialog_WaitList(*) : Object is not unique, addr=%p", g_pwxDialog_WaitList);
  }
  g_pwxDialog_WaitList = this;
  Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitList creator
 */

bool wxDialog_WaitList::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_WaitList member initialisation
  v_oBoxV_all = NULL;
  v_oBoxH_top = NULL;
  v_oBitmap = NULL;
  v_oLogText = NULL;
  v_oBoxH_mid = NULL;
  v_oInfoText = NULL;
  v_oBoxH_bot = NULL;
  v_oGauge = NULL;
////@end wxDialog_WaitList member initialisation

////@begin wxDialog_WaitList creation
  SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
////@end wxDialog_WaitList creation
  return true;
}

/*!
 * Control creation for wxDialog_WaitList
 */

void wxDialog_WaitList::CreateControls()
{
  _wx_log_nfo("wxDialog_WaitList::CreateControls()");
////@begin wxDialog_WaitList content construction
  wxDialog_WaitList* itemDialog1 = this;

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

  v_oLogText = new wxTextCtrl;
  v_oLogText->Create( itemDialog1, ID_TEXTCTRL_WAITLIST, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL );
  v_oLogText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_top->Add(v_oLogText, 1, wxGROW|wxALL, 10);

  v_oBoxH_mid = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_mid, 1, wxGROW|wxALL, 5);

  v_oInfoText = new wxTextCtrl;
  v_oInfoText->Create( itemDialog1, ID_TEXTCTRL_WAITLIST, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
  v_oInfoText->SetBackgroundColour(wxColour(224, 224, 240));
  v_oInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_mid->Add(v_oInfoText, 1, wxGROW|wxALL, 10);

  v_oBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
  v_oBoxV_all->Add(v_oBoxH_bot, 0, wxGROW|wxALL, 5);

  v_oGauge = new wxGauge;
  v_oGauge->Create( itemDialog1, ID_GAUGE_WAITLIST, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH );
  v_oGauge->SetValue(50);
  v_oGauge->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
  v_oBoxH_bot->Add(v_oGauge, 1, wxALIGN_CENTER_VERTICAL|wxALL, 10);

////@end wxDialog_WaitList content construction
  v_oInfoText->SetValue("");
  v_oLogText->SetValue("");
  v_oGauge->SetValue(0);
  v_bInitialized = true;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST
 */

void wxDialog_WaitList::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
  _wx_log_nfo("wxDialog_WaitList::OnCloseWindow(*)");
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST in wxDialog_WaitList.
  // Before editing this code, remove the block markers.
  wxWindow* window = this;
  window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST in wxDialog_WaitList.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_WaitList::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_WaitList::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin wxDialog_WaitList bitmap retrieval
  wxUnusedVar(name);
  if (name == _T("logo_pluto.jpg"))
  {
    wxBitmap bitmap(logo_pluto_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end wxDialog_WaitList bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_WaitList::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin wxDialog_WaitList icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end wxDialog_WaitList icon retrieval
}

//==================================================

wxDialog_WaitList::~wxDialog_WaitList()
{
  _wx_log_nfo("wxDialog_WaitList::~wxDialog_WaitList()");
  if (g_pwxDialog_WaitList == this)
    g_pwxDialog_WaitList = NULL;
}

bool wxDialog_WaitList::Destroy()
{
  _wx_log_nfo("wxDialog_WaitList::Destroy()");
  return wxDialog::Destroy();
}

void wxDialog_WaitList::SetExternalRefresh(const string &sInfo, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitList::SetExternalRefresh(*)");
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

void wxDialog_WaitList::OnInternalIdle()
{
  //--_wx_log_nfo("wxDialog_WaitList::OnInternalIdle()");
  if ( wxIdleThreadShouldStop() )
    Destroy();
  if (v_bInitialized && v_bShouldRefresh && ::wxIsMainThread())
  {
    wxCriticalSectionLocker lock(v_oCriticalRefresh);
    // update info text
    v_oInfoText->SetValue(v_refresh_sInfo);
    // update progress bar
    v_oGauge->SetValue(v_refresh_nBarPercent);
    v_bShouldRefresh = false;
  }
  wxDialog::OnInternalIdle();
}

//==================================================

bool wxDialog_WaitList_Refresh(const string &sInfo, int nBarPercent)
{
  _wx_log_nfo("wxDialog_WaitList_Refresh(*)");
  if (! (g_pwxDialog_WaitList != NULL))
  {
    _wx_log_wrn("Dialog is not active, ignoring SetSize request");
    return false;
  }
  _wx_log_nfo("Allocated at %p", g_pwxDialog_WaitList);
  g_pwxDialog_WaitList->SetExternalRefresh(sInfo, nBarPercent);
  _wx_log_nfo("wxDialog_WaitList_Refresh(*);;");
  return true;
}

//==================================================
