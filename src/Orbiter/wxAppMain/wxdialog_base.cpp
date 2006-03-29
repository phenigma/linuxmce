//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_base.h"
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

#include "wxdialog_base.h"
#include "wx_event_dialog.h"
#include "wx_safe_dialog.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_Base type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_Base, wxDialog )
;

/*!
 * wxDialog_Base event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_Base, wxDialog )

////@begin wxDialog_Base event table entries
    EVT_IDLE( wxDialog_Base::OnIdle )

////@end wxDialog_Base event table entries
    EVT_WINDOW_CREATE( wxDialog_Base::OnWindowCreate )
    EVTC_DIALOG(wxID_ANY, wxDialog_Base::OnEvent_Dialog)

    END_EVENT_TABLE()
    ;

/*!
 * wxDialog_Base constructors
 */

wxDialog_Base::wxDialog_Base( )
    : v_bInitialized(false)
    , v_pData_Holder_Dialog(NULL)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
}

wxDialog_Base::wxDialog_Base( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : v_bInitialized(false)
    , v_pData_Holder_Dialog(NULL)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_Base creator
 */

bool wxDialog_Base::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    //_WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    style |= wxSTAY_ON_TOP;
#endif // USE_RELEASE_CODE
#ifdef USE_DEBUG_CODE
    style &= (~wxSTAY_ON_TOP);
#endif // USE_DEBUG_CODE
////@begin wxDialog_Base member initialisation
////@end wxDialog_Base member initialisation

////@begin wxDialog_Base creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
////@end wxDialog_Base creation
    return true;
}

/*!
 * Control creation for wxDialog_Base
 */

void wxDialog_Base::CreateControls()
{
    //_WX_LOG_NFO();
////@begin wxDialog_Base content construction
    wxDialog_Base* itemDialog1 = this;

    this->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
////@end wxDialog_Base content construction
    wxUnusedVar(itemDialog1);
}

/*!
 * wxEVT_IDLE event handler for ID_DIALOG_BASE
 */

void wxDialog_Base::OnIdle( wxIdleEvent& event )
{
    //_WX_LOG_NFO();
    wxDialog::OnIdle(event);
////@begin wxEVT_IDLE event handler for ID_DIALOG_BASE in wxDialog_Base.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler for ID_DIALOG_BASE in wxDialog_Base.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_Base::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_Base::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_Base bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_Base bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_Base::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_Base icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_Base icon retrieval
}

wxDialog_Base::~wxDialog_Base()
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
}

bool wxDialog_Base::Destroy()
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    if (IsModal())
    {
        _WX_LOG_WRN("Dialog '%s' Is Modal!", GetLabel().c_str());
        wxDialog::EndModal(GetReturnCode());
        return true;
    }
    if (v_pData_Holder_Dialog)
        wx_semaphore_post(v_pData_Holder_Dialog->oSemaphore, v_pData_Holder_Dialog->bInThread);
    return wxDialog::Destroy();
}

void wxDialog_Base::EndModal(int retCode)
{
    _WX_LOG_NFO("retCode=%d Label='%s'", retCode, GetLabel().c_str());
    if (! IsModal())
    {
        _WX_LOG_WRN("Dialog '%s' Is Not Modal!", GetLabel().c_str());
        wxDialog::Destroy();
        return;
    }
    if (v_pData_Holder_Dialog)
    {
        v_pData_Holder_Dialog->nRetCode = retCode;
        wx_semaphore_post(v_pData_Holder_Dialog->oSemaphore, v_pData_Holder_Dialog->bInThread);
    }
    return wxDialog::EndModal(retCode);
}

void wxDialog_Base::OnButton_EndModal( wxCommandEvent& event )
{
    //_WX_LOG_NFO("%s", _str_event(event));
    OnButton_SetReturnCode(event);
    EndModal(GetReturnCode());
}

void wxDialog_Base::OnButton_SetReturnCode( wxCommandEvent& event )
{
    _WX_LOG_NFO("%s", _str_event(event));
    wxWindow *pwxWindow = wxDynamicCast(event.GetEventObject(), wxWindow);
    _COND_RET(pwxWindow != NULL);
    SetReturnCode(pwxWindow->GetId());
}

bool wxDialog_Base::Gui_DataLoad(void * WXUNUSED(pExternData))
{
    return true;
}

bool wxDialog_Base::Gui_DataSave(void * WXUNUSED(pExternData))
{
    return true;
}

void wxDialog_Base::Gui_Refresh(void * WXUNUSED(pExternData))
{
    _WX_LOG_ERR("virtual method not implemented in a derived class");
}

bool wxDialog_Base::IsInitialized()
{
    return v_bInitialized;
};

void wxDialog_Base::Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog)
{
    v_pData_Holder_Dialog = pData_Holder_Dialog;
}

void wxDialog_Base::OnWindowCreate(wxWindowCreateEvent& event)
{
    //_WX_LOG_NFO();
    wxDialog *pwxDialog = wxDynamicCast(event.GetEventObject(), wxDialog);
    if (pwxDialog)
    {
        wxString sLabel = GetLabel();
        if (pwxDialog->GetLabel() == sLabel)
        {
            _WX_LOG_NFO("Initialized : %s", sLabel.c_str());
            v_bInitialized = true;
        }
    }
}

void wxDialog_Base::OnEvent_Dialog(wxCommandEvent& event)
{
    _WX_LOG_NFO("Received event : %s", _str_event(event));
    Data_Holder_Dialog *pData_Holder_Dialog = wx_static_cast(Data_Holder_Dialog *, event.GetClientData());
    _COND_RET(pData_Holder_Dialog != NULL);
    E_ACTION_TYPE action = (E_ACTION_TYPE)event.GetId();
    _WX_LOG_NFO("Received command event : action='%s'", _str_enum(action));
    switch (action)
    {
        case E_Action_Refresh:
        {
            Safe_Gui_Refresh(this, pData_Holder_Dialog->pExternData);
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(pData_Holder_Dialog->oSemaphore, pData_Holder_Dialog->bInThread);
}
