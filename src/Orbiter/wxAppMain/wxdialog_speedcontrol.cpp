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
    v_pBoxH_Seek = NULL;
    v_pPanel_Seek = NULL;
    v_pBoxH_Speed = NULL;
    v_pPanel_Speed = NULL;
    v_pBoxH_Time = NULL;
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

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_Seek = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Seek, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pPanel_Seek = new wxPanel_Seek;
    v_pPanel_Seek->Create( itemDialog_Base1, ID_CTRL_SEEK, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    v_pBoxH_Seek->Add(v_pPanel_Seek, 1, wxGROW|wxALL, 5);

    v_pBoxH_Speed = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Speed, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pPanel_Speed = new wxPanel_Speed;
    v_pPanel_Speed->Create( itemDialog_Base1, ID_CTRL_SPEED, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    v_pBoxH_Speed->Add(v_pPanel_Speed, 1, wxGROW|wxALL, 5);

    v_pBoxH_Time = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Time, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pPanel_Time = new wxPanel_Time;
    v_pPanel_Time->Create( itemDialog_Base1, ID_CTRL_TIME, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    v_pBoxH_Time->Add(v_pPanel_Time, 1, wxGROW|wxALL, 5);

////@end wxDialog_SpeedControl content construction
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
    v_oPersistent_Data.oData_Refresh = *pCallData;
    _WX_LOG_NFO("Style=%d, seek=%d, speed=%d", pCallData->m_eStyle, pCallData->m_nSeekToPos, pCallData->m_nSpeed);
    switch (pCallData->m_eStyle)
    {
        case SpeedControlCallBackData::TIME:
            v_oPersistent_Data.bShowTime = true;
            break;
        case SpeedControlCallBackData::TIME_SEEK:
            v_oPersistent_Data.bShowTime = true;
            v_oPersistent_Data.bShowSeek = true;
            break;
        case SpeedControlCallBackData::TIME_SPEED:
            v_oPersistent_Data.bShowTime = true;
            v_oPersistent_Data.bShowSpeed = true;
            break;
        case SpeedControlCallBackData::SPEED:
            v_oPersistent_Data.bShowSpeed = true;
            break;
        default:
            _WX_LOG_ERR("bad style : %d", pCallData->m_eStyle);
            break;
    }
    v_oPersistent_Data.bInitialized = true;
    if (v_oPersistent_Data.bShowSeek)
    {
        int nDeltaFull = pCallData->m_nTimeEnd - pCallData->m_nTimeStart;
        int nDeltaSeek = pCallData->m_nSeekToPos - pCallData->m_nTimeStart;
        if ( (nDeltaFull > 0) && (nDeltaSeek > 0) )
        {
            v_pPanel_Seek->v_nPosRatio = nDeltaSeek / nDeltaFull;
            v_pPanel_Seek->Refresh();
        }
        v_pPanel_Seek->Show();
    }
    else
    {
        v_pPanel_Seek->Hide();
    }
    if (v_oPersistent_Data.bShowSpeed)
    {
        Copy_StlSequence_wxArray(pCallData->m_listSpeeds, v_pPanel_Speed->v_anSpeeds);
        v_pPanel_Speed->v_nSpeed = pCallData->m_nSpeed;
        v_pPanel_Speed->Show();
    }
    else
    {
        v_pPanel_Speed->Hide();
    }
    if (v_oPersistent_Data.bShowTime)
    {
        v_pPanel_Time->v_nTimeStart = pCallData->m_nTimeStart;
        v_pPanel_Time->v_nTimeNow = pCallData->m_nTimeNow;
        v_pPanel_Time->v_nTimeEnd = pCallData->m_nTimeEnd;
        v_pPanel_Time->Show();
    }
    else
    {
        v_pPanel_Time->Hide();
    }
    UpdatePosition(pCallData->m_rectPosition.X, pCallData->m_rectPosition.Y, pCallData->m_rectPosition.Width, pCallData->m_rectPosition.Height);
    return true;
}
