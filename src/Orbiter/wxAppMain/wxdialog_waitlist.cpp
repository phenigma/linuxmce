//
// Author : C Remus
//
// Changed by : ...
//

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

#ifdef USE_RELEASE_CODE
#include "../CallBackData.h"
#endif // USE_RELEASE_CODE

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitList type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitList, wxDialog_Base )
;

/*!
 * wxDialog_WaitList event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitList, wxDialog_Base )

////@begin wxDialog_WaitList event table entries
    EVT_CLOSE( wxDialog_WaitList::OnCloseWindow )

////@end wxDialog_WaitList event table entries

END_EVENT_TABLE()
;

/*!
 * wxDialog_WaitList constructors
 */

wxDialog_WaitList::wxDialog_WaitList( )
{
    _WX_LOG_NFO();
}

wxDialog_WaitList::wxDialog_WaitList( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitList creator
 */

bool wxDialog_WaitList::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
////@begin wxDialog_WaitList member initialisation
    v_pBoxV_all = NULL;
    v_pBoxH_top = NULL;
    v_pBitmap = NULL;
    v_pBoxH_mid = NULL;
    v_pLogText = NULL;
    v_pInfoText = NULL;
    v_pBoxH_bot = NULL;
    v_pGauge = NULL;
////@end wxDialog_WaitList member initialisation

////@begin wxDialog_WaitList creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

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
    _WX_LOG_NFO();
////@begin wxDialog_WaitList content construction
    wxDialog_WaitList* itemDialog_Base1 = this;

    this->SetBackgroundColour(wxColour(224, 224, 240));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_top = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_top, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBitmap v_pBitmapBitmap(itemDialog_Base1->GetBitmapResource(wxT("logo_pluto.jpg")));
    v_pBitmap = new wxStaticBitmap;
    v_pBitmap->Create( itemDialog_Base1, wxID_STATIC, v_pBitmapBitmap, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(126, 87)), wxNO_BORDER );
    v_pBoxH_top->Add(v_pBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

    v_pBoxH_mid = new wxBoxSizer(wxVERTICAL);
    v_pBoxV_all->Add(v_pBoxH_mid, 1, wxGROW|wxALL, 5);

    v_pLogText = new wxTextCtrl;
    v_pLogText->Create( itemDialog_Base1, ID_TEXTCTRL_WAITLIST, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL );
    v_pLogText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_mid->Add(v_pLogText, 1, wxGROW|wxALL, 10);

    v_pInfoText = new wxTextCtrl;
    v_pInfoText->Create( itemDialog_Base1, ID_TEXTCTRL_WAITLIST, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_WORDWRAP|wxNO_BORDER );
    v_pInfoText->SetBackgroundColour(wxColour(224, 224, 240));
    v_pInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_mid->Add(v_pInfoText, 0, wxGROW|wxALL, 10);

    v_pBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_bot, 0, wxGROW|wxALL, 5);

    v_pGauge = new wxGauge;
    v_pGauge->Create( itemDialog_Base1, ID_GAUGE_WAITLIST, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH|wxNO_BORDER );
    v_pGauge->SetValue(50);
    v_pGauge->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_bot->Add(v_pGauge, 1, wxALIGN_CENTER_VERTICAL|wxALL, 10);

////@end wxDialog_WaitList content construction

    v_pInfoText->SetValue("");
    v_pLogText->SetValue("");
    v_pGauge->SetValue(0);
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST
 */

void wxDialog_WaitList::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST in wxDialog_WaitList.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITLIST in wxDialog_WaitList.
    wxUnusedVar(event);
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

bool wxDialog_WaitList::Gui_Refresh(CallBackData *pCallBackData)
{
    //_WX_LOG_NFO();
    WaitUserListCallBackData *pCallData = dynamic_cast<WaitUserListCallBackData *>(pCallBackData);
    _COND_RET(pCallData != NULL, false);
    // update info text
    v_pInfoText->SetValue(pCallData->m_sMessage);
    // update log text
    if (v_sPrevStr != pCallData->m_sMessage)
    {
        if (! v_pLogText->GetValue().IsEmpty())
            v_pLogText->AppendText("\n");
        v_pLogText->AppendText(pCallData->m_sMessage);
        v_sPrevStr = pCallData->m_sMessage;
    }
    // update progress bar
    v_pGauge->SetValue(pCallData->m_nPercent);
    return true;
}
