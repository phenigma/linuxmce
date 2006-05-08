//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_speedcontrol.h"
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

#include "wxdialog_speedcontrol.h"
#include "wxpanel_seek.h"
#include "wxpanel_speed.h"
#include "wxpanel_time.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_SpeedControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_SpeedControl, wxDialog_Base )

/*!
 * wxDialog_SpeedControl event table definition
 */

    BEGIN_EVENT_TABLE( wxDialog_SpeedControl, wxDialog_Base )

////@begin wxDialog_SpeedControl event table entries
    EVT_CLOSE( wxDialog_SpeedControl::OnCloseWindow )

////@end wxDialog_SpeedControl event table entries

    END_EVENT_TABLE()
    ;

/*!
 * wxDialog_SpeedControl constructors
 */

wxDialog_SpeedControl::wxDialog_SpeedControl( )
{
    _WX_LOG_NFO();
}

wxDialog_SpeedControl::wxDialog_SpeedControl( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_SpeedControl creator
 */

bool wxDialog_SpeedControl::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
////@begin wxDialog_SpeedControl member initialisation
    v_pBoxV_all = NULL;
    v_pPanel_Seek = NULL;
    v_pPanel_Speed = NULL;
    v_pPanel_Time = NULL;
////@end wxDialog_SpeedControl member initialisation

////@begin wxDialog_SpeedControl creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_SpeedControl creation
    return true;
}

/*!
 * Control creation for wxDialog_SpeedControl
 */

void wxDialog_SpeedControl::CreateControls()
{
    _WX_LOG_NFO();
////@begin wxDialog_SpeedControl content construction
    wxDialog_SpeedControl* itemDialog_Base1 = this;

    this->SetBackgroundColour(wxColour(173, 216, 230));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pPanel_Seek = new wxPanel_Seek;
    v_pPanel_Seek->Create( itemDialog_Base1, ID_CTRL_SEEK, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(100, 10)), wxSUNKEN_BORDER|wxCLIP_CHILDREN  );
    v_pPanel_Seek->SetForegroundColour(wxColour(165, 42, 42));
    v_pPanel_Seek->SetBackgroundColour(wxColour(173, 216, 230));
    v_pBoxV_all->Add(v_pPanel_Seek, 1, wxGROW|wxLEFT|wxRIGHT, 5);

    v_pPanel_Speed = new wxPanel_Speed;
    v_pPanel_Speed->Create( itemDialog_Base1, ID_CTRL_SPEED, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(100, 10)), wxSUNKEN_BORDER|wxCLIP_CHILDREN  );
    v_pPanel_Speed->SetForegroundColour(wxColour(0, 0, 255));
    v_pPanel_Speed->SetBackgroundColour(wxColour(173, 216, 230));
    v_pBoxV_all->Add(v_pPanel_Speed, 1, wxGROW|wxLEFT|wxRIGHT, 5);

    v_pPanel_Time = new wxPanel_Time;
    v_pPanel_Time->Create( itemDialog_Base1, ID_CTRL_TIME, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(100, 75)), wxSUNKEN_BORDER|wxCLIP_CHILDREN  );
    v_pPanel_Time->SetBackgroundColour(wxColour(173, 216, 230));
    v_pBoxV_all->Add(v_pPanel_Time, 3, wxGROW|wxLEFT|wxRIGHT, 5);

////@end wxDialog_SpeedControl content construction
#ifdef USE_RELEASE_CODE
#endif // USE_RELEASE_CODE
    wx_WindowStyle_Del(v_pPanel_Seek, wxNO_BORDER);
    wx_WindowStyle_Del(v_pPanel_Speed, wxNO_BORDER);
    wx_WindowStyle_Del(v_pPanel_Time, wxNO_BORDER);
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL
 */

void wxDialog_SpeedControl::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL in wxDialog_SpeedControl.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL in wxDialog_SpeedControl.
    wxUnusedVar(event);
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_SpeedControl::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_SpeedControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_SpeedControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_SpeedControl bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_SpeedControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_SpeedControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_SpeedControl icon retrieval
}

bool wxDialog_SpeedControl::Gui_Refresh(CallBackData *pCallBackData)
{
    //_WX_LOG_NFO();
    SpeedControlCallBackData *pCallData = dynamic_cast<SpeedControlCallBackData *>(pCallBackData);
    _COND_RET(pCallData != NULL, false);
    _WX_LOG_NFO("Style=%s, speed=%d, seek=%d", _str_enum(pCallData->m_eStyle), pCallData->m_nSpeed, pCallData->m_nSeekToPos);
    bool bShowTime = false;
    bool bShowSeek = false;
    bool bShowSpeed = false;
    switch (pCallData->m_eStyle)
    {
        case SpeedControlCallBackData::TIME:
            bShowTime = true;
            break;
        case SpeedControlCallBackData::TIME_SEEK:
            bShowTime = true;
            bShowSeek = true;
            break;
        case SpeedControlCallBackData::TIME_SPEED:
            bShowTime = true;
            bShowSpeed = true;
            break;
        case SpeedControlCallBackData::SPEED:
            bShowSpeed = true;
            break;
        default:
            _WX_LOG_ERR("bad style : %d", pCallData->m_eStyle);
            break;
    }
    _WX_LOG_NFO("bShowTime=%d, bShowSeek=%d, bShowSpeed=%d", bShowTime, bShowSeek, bShowSpeed);
    if (bShowTime)
    {
        v_pPanel_Time->v_nTimeStart = pCallData->m_nTimeStart;
        v_pPanel_Time->v_nTimeNow = pCallData->m_nTimeNow;
        v_pPanel_Time->v_nTimeEnd = pCallData->m_nTimeEnd;
        if (! v_pPanel_Time->IsShown())
            v_pPanel_Time->Show();
        v_pPanel_Time->Refresh();
    }
    else
    {
        if (! v_pPanel_Time->IsShown())
            v_pPanel_Time->Hide();
    }
    v_pPanel_Time->Show(bShowTime);
    if (bShowSeek)
    {
        int nDeltaFull = pCallData->m_nTimeEnd - pCallData->m_nTimeStart;
        int nDeltaSeek = pCallData->m_nSeekToPos - pCallData->m_nTimeStart;
        if ( (nDeltaFull > 0) && (nDeltaSeek > 0) )
        {
            v_pPanel_Seek->v_dPosPercent = (double)nDeltaSeek / nDeltaFull;
        }
        if (! v_pPanel_Seek->IsShown())
            v_pPanel_Seek->Show();
        v_pPanel_Seek->Refresh();
    }
    else
    {
        if (v_pPanel_Seek->IsShown())
            v_pPanel_Seek->Hide();
    }
    v_pPanel_Seek->Show(bShowSeek);
    if (bShowSpeed)
    {
        Copy_StlSequence_wxArray(pCallData->m_listSpeeds, v_pPanel_Speed->v_anSpeeds);
        v_pPanel_Speed->v_nSpeed = pCallData->m_nSpeed;
        if (! v_pPanel_Speed->IsShown())
            v_pPanel_Speed->Show();
        v_pPanel_Speed->Refresh();
    }
    else
    {
        if (v_pPanel_Speed->IsShown())
            v_pPanel_Speed->Hide();
    }
    v_pPanel_Speed->Show(bShowSpeed);
    Update_Position_FullScreen(pCallData->m_rectPosition.X, pCallData->m_rectPosition.Y, pCallData->m_rectPosition.Width, pCallData->m_rectPosition.Height, pCallData->m_bShowFullScreen);
    return true;
}
