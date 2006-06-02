//
// Author : Remus C.
//
// Changed by : ...
//

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

#ifdef USE_RELEASE_CODE
#include "../CallBackData.h"
#endif // USE_RELEASE_CODE

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitUser type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitUser, wxDialog_Base )
;

/*!
 * wxDialog_WaitUser event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitUser, wxDialog_Base )

////@begin wxDialog_WaitUser event table entries
    EVT_CLOSE( wxDialog_WaitUser::OnCloseWindow )

////@end wxDialog_WaitUser event table entries

    EVT_BUTTON( wxID_ANY, wxDialog_Base::OnButton_EndModal )
    EVT_TIMER(ID_Timer_ExpireDialog, wxDialog_WaitUser::OnTimer_ExpireDialog)

    END_EVENT_TABLE();

/*!
 * wxDialog_WaitUser constructors
 */

wxDialog_WaitUser::wxDialog_WaitUser( )
        : v_oTimer_ExpireDialog(this, ID_Timer_ExpireDialog)
{
    _LOG_NFO();
}

wxDialog_WaitUser::wxDialog_WaitUser( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_oTimer_ExpireDialog(this, ID_Timer_ExpireDialog)
{
    _LOG_NFO();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitUser creator
 */

bool wxDialog_WaitUser::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _LOG_NFO();
////@begin wxDialog_WaitUser member initialisation
    v_pBoxV_all = NULL;
    v_pBoxH_top = NULL;
    v_pBoxV_logo = NULL;
    v_pBitmap = NULL;
    v_pGauge = NULL;
    v_pBoxH_mid = NULL;
    v_pInfoText = NULL;
    v_pBoxH_bot = NULL;
    v_pButtonOk = NULL;
////@end wxDialog_WaitUser member initialisation

////@begin wxDialog_WaitUser creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

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
    _LOG_NFO();
////@begin wxDialog_WaitUser content construction
    wxDialog_WaitUser* itemDialog_Base1 = this;

    this->SetBackgroundColour(wxColour(224, 224, 240));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_top = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_top, 2, wxGROW|wxALL, 5);

    v_pBoxV_logo = new wxBoxSizer(wxVERTICAL);
    v_pBoxH_top->Add(v_pBoxV_logo, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmap v_pBitmapBitmap(itemDialog_Base1->GetBitmapResource(wxT("logo_pluto.jpg")));
    v_pBitmap = new wxStaticBitmap;
    v_pBitmap->Create( itemDialog_Base1, wxID_STATIC, v_pBitmapBitmap, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(126, 87)), wxNO_BORDER );
    v_pBoxV_logo->Add(v_pBitmap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    v_pGauge = new wxGauge;
    v_pGauge->Create( itemDialog_Base1, ID_GAUGE_WAITUSER, 100, wxDefaultPosition, wxDefaultSize, wxGA_VERTICAL|wxGA_PROGRESSBAR|wxGA_SMOOTH|wxNO_BORDER );
    v_pGauge->SetValue(50);
    v_pGauge->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_top->Add(v_pGauge, 0, wxGROW|wxALL, 5);

    v_pBoxH_mid = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_mid, 1, wxGROW|wxALL, 5);

    v_pInfoText = new wxTextCtrl;
    v_pInfoText->Create( itemDialog_Base1, ID_TEXTCTRL_WAITUSER, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxNO_BORDER );
    v_pInfoText->SetBackgroundColour(wxColour(224, 224, 240));
    v_pInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_mid->Add(v_pInfoText, 1, wxGROW|wxALL, 5);

    v_pBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_bot, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    v_pButtonOk = new wxButton;
    v_pButtonOk->Create( itemDialog_Base1, wxID_OK, _T("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButtonOk->SetDefault();
    v_pButtonOk->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_bot->Add(v_pButtonOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxDialog_WaitUser content construction

    v_pInfoText->SetValue("");
    v_nExpireTime_ms = 0;
    v_nCrtTime_ms = 0;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER
 */

void wxDialog_WaitUser::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER in wxDialog_WaitUser.
    // Before editing this code, remove the block markers.
    wxDialog* dialog = wxDynamicCast(this, wxDialog);
    dialog->EndModal(ID_DIALOG_WAITUSER);
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER in wxDialog_WaitUser.
    wxUnusedVar(event);
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

wxDialog_WaitUser::~wxDialog_WaitUser()
{
    _LOG_NFO();
    v_oTimer_ExpireDialog.Stop();
}

bool wxDialog_WaitUser::Gui_DataLoad(CallBackData *pCallBackData)
{
    //_LOG_NFO();
    WaitUserPromptCallBackData *pCallData = dynamic_cast<WaitUserPromptCallBackData *>(pCallBackData);
    _COND_RET(pCallData != NULL, false);
    Update_Position_FullScreen(pCallData->m_rectPosition.X, pCallData->m_rectPosition.Y, pCallData->m_rectPosition.Width, pCallData->m_rectPosition.Height, pCallData->m_bShowFullScreen);
    // update info text
    v_pInfoText->SetValue(pCallData->m_sMessage);
    // update buttons
    if (pCallData->m_mapPrompts.size() == 0)
    {
        v_pButtonOk->Show();
    }
    else
    {
        v_pButtonOk->Hide();
        for (map<int, string>::iterator it = pCallData->m_mapPrompts.begin(); it != pCallData->m_mapPrompts.end(); ++it)
        {
            int nButtonId = it->first;
            wxString sButtonLabel = it->second.c_str();
            // search string in current data
            // add item if not found
            int idx = v_asLabels.Index(sButtonLabel);
            if (idx == wxNOT_FOUND)
            {
                idx = v_asLabels.GetCount();
                v_asLabels.Add(sButtonLabel);
                wxButton *pButton = new wxButton;
                pButton->Create( this, nButtonId, sButtonLabel, wxDefaultPosition, wxDefaultSize, 0 );
                pButton->SetDefault();
                pButton->SetFont(v_pButtonOk->GetFont());
                v_pBoxH_bot->Add(pButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            }
        }
    }
    // update expired time value and restart the timer
    bool bStartNow = (v_nExpireTime_ms == 0);
    v_nExpireTime_ms = pCallData->m_nTimeoutSeconds * 1000;
    v_pGauge->SetRange(v_nExpireTime_ms);
    if (bStartNow)
    {
        v_oTimer_ExpireDialog.Start(INTERVAL_TIMER_EXPIREDIALOG_MSEC);
    }
    v_pGauge->SetValue(v_nCrtTime_ms);
    return Gui_Refresh(pCallBackData);
}

void wxDialog_WaitUser::OnTimer_ExpireDialog(wxTimerEvent& event)
{
    //_LOG_NFO();
    _COND_RET(event.GetId() == ID_Timer_ExpireDialog);
    if ( (! v_bInitialized) || (v_nExpireTime_ms <= 0) )
        return;
    int nInterval = v_oTimer_ExpireDialog.GetInterval();
    v_nCrtTime_ms += nInterval;
    if (v_nCrtTime_ms >= v_nExpireTime_ms)
    {
        if (IsModal())
            EndModal(0);
        else
            Destroy();
    }
    else
    {
        v_pGauge->SetValue(v_nCrtTime_ms);
    }
}
