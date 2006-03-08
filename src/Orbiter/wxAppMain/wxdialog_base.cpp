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
EVTC_DIALOG(wxID_ANY, wxDialog_Base::OnEvent_Dialog)

EVT_WINDOW_CREATE( wxDialog_Base::OnWindowCreate )

END_EVENT_TABLE()
;

const E_wxDialog_Class_Type wxDialog_Base::e_class_type = E_wxDialog_Undefined;

/*!
 * wxDialog_Base constructors
 */

wxDialog_Base::wxDialog_Base( )
    : v_bInitialized(false), v_eRefreshStatus(E_Refresh_Ready), v_pData_Holder_Dialog(NULL)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
}

wxDialog_Base::wxDialog_Base( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : v_bInitialized(false), v_eRefreshStatus(E_Refresh_Ready), v_pData_Holder_Dialog(NULL)
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
    SafeRefresh_WhenIdle();
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
        _wx_log_wrn("Dialog '%s' Is Modal!", GetLabel().c_str());
        wxDialog::EndModal(0);
        return true;
    }
    if (v_pData_Holder_Dialog)
    {
        wx_semaphore_post(v_pData_Holder_Dialog->oSemaphore);
    }
    return wxDialog::Destroy();
}

void wxDialog_Base::EndModal(int retCode)
{
    _WX_LOG_NFO("retCode=%d Label='%s'", retCode, GetLabel().c_str());
    if (! IsModal())
    {
        _wx_log_wrn("Dialog '%s' Is Not Modal!", GetLabel().c_str());
        wxDialog::Destroy();
        return;
    }
    if (v_pData_Holder_Dialog)
    {
        v_pData_Holder_Dialog->nRetCode = retCode;
        wx_semaphore_post(v_pData_Holder_Dialog->oSemaphore);
    }
    return wxDialog::EndModal(retCode);
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
            _wx_log_nfo("Initialized : %s", sLabel.c_str());
            v_bInitialized = true;
        }
    }
}

void wxDialog_Base::Data_Refresh_Enter()
{
    //_WX_LOG_NFO();
    if (! v_bInitialized)
    {
        _WX_LOG_WRN("Dialog is not initialized");
    }
    if (! ::wxIsMainThread())
        v_oCriticalRefresh.Enter();
    _COND_ASSIGN(v_eRefreshStatus, E_Refresh_Ready, E_Refresh_LoadingData);
}

void wxDialog_Base::Data_Refresh_Leave()
{
    //_WX_LOG_NFO();
    _COND_ASSIGN(v_eRefreshStatus, E_Refresh_LoadingData, E_Refresh_ShouldUpdate);
    if (! ::wxIsMainThread())
        v_oCriticalRefresh.Leave();
}

void wxDialog_Base::Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog)
{
    v_pData_Holder_Dialog = pData_Holder_Dialog;
}

bool wxDialog_Base::ExternalData_Load(void *pExternData)
{
    _WX_LOG_WRN("virtual method not implemented in a derived class");
    wxUnusedVar(pExternData);
    return true;
}

bool wxDialog_Base::ExternalData_Save(void *pExternData)
{
    _WX_LOG_WRN("virtual method not implemented in a derived class");
    wxUnusedVar(pExternData);
    return true;
}

void wxDialog_Base::OnEvent_Dialog(wxCommandEvent& event)
{
    Data_Holder_Refresh *pData_Holder_Refresh = wx_static_cast(Data_Holder_Refresh *, event.GetClientData());
    _COND_RET(pData_Holder_Refresh != NULL);
    event.Skip();
    E_DIALOG_ACTION action = (E_DIALOG_ACTION)event.GetId();
    _WX_LOG_NFO("Received command event : action='%s'", _str_enum(action));
    switch (action)
    {
        case E_Dialog_Refresh:
        {
            SafeRefresh_NewData(*pData_Holder_Refresh);
            break;
        }
        default:
            _WX_LOG_ERR("unknown action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(pData_Holder_Refresh->oSemaphore);
}

void wxDialog_Base::SafeRefresh_WhenIdle()
{
    //_WX_LOG_NFO();
    if ( wxIdleThreadShouldStop() )
        Destroy();
    if (v_bInitialized && (v_eRefreshStatus == E_Refresh_ShouldUpdate))
    {
        wxCriticalSectionLocker lock(v_oCriticalRefresh);
        _COND_ASSIGN(v_eRefreshStatus, E_Refresh_ShouldUpdate, E_Refresh_WindowUpdate);
        _WX_LOG_NFO();
        SafeRefresh_Gui();
        Refresh(false);
        Update();
        _COND_ASSIGN(v_eRefreshStatus, E_Refresh_WindowUpdate, E_Refresh_Ready);
    }
}

void wxDialog_Base::SafeRefresh_NewData(Data_Holder_Refresh &rData_Holder_Refresh)
{
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        _WX_LOG_DBG("%p", &rData_Holder_Refresh);
        wx_post_event_thread(this, wxEVTC_DIALOG, E_Dialog_Refresh, "Dialog_Refresh", &rData_Holder_Refresh);
        wx_semaphore_wait(rData_Holder_Refresh.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        return;
    }
    Data_Refresh_Enter();
    SafeRefresh_CopyData(rData_Holder_Refresh.pData_Refresh);
    Data_Refresh_Leave();
}

void wxDialog_Base::SafeRefresh_CopyData(void *pData_Refresh)
{
    _WX_LOG_ERR("virtual method not implemented in a derived class");
    wxUnusedVar(pData_Refresh);
}

void wxDialog_Base::SafeRefresh_Gui()
{
    _WX_LOG_ERR("virtual method not implemented in a derived class");
}

const char * _str_enum(wxDialog_Base::E_REFRESH_STATUS value)
{
    switch (value)
    {
        CASE_const_ret_str(wxDialog_Base::E_Refresh_Ready);
        CASE_const_ret_str(wxDialog_Base::E_Refresh_LoadingData);
        CASE_const_ret_str(wxDialog_Base::E_Refresh_ShouldUpdate);
        CASE_const_ret_str(wxDialog_Base::E_Refresh_WindowUpdate);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
