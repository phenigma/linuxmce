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
#include "wx_extern_app.h"

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
        , v_pExtern_Task_Data_WaitUser(NULL)
        , v_pExtern_Task_Data_WaitInitialized(NULL)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    wxDELETE(v_pExtern_Task_Data_WaitUser);
    wxDELETE(v_pExtern_Task_Data_WaitInitialized);
}

wxDialog_Base::wxDialog_Base( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_bInitialized(false)
        , v_pData_Holder_Dialog(NULL)
        , v_pExtern_Task_Data_WaitUser(NULL)
        , v_pExtern_Task_Data_WaitInitialized(NULL)
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
    if (v_pExtern_Task_Data_WaitInitialized && v_bInitialized)
    {
        _WX_LOG_NFO("Send response to external task : WaitInitialized");
        Extern_Task_Response(v_pExtern_Task_Data_WaitInitialized);
        wxDELETE(v_pExtern_Task_Data_WaitInitialized);
    }
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
        EndModal(GetReturnCode());
        return true;
    }
    Clean_Exit();
    return wxDialog::Destroy();
}

void wxDialog_Base::EndModal(int retCode)
{
    _WX_LOG_NFO("Label='%s' retCode=%d", GetLabel().c_str(), retCode);
    SetReturnCode(retCode);
    if (! IsModal())
    {
        _WX_LOG_WRN("Dialog '%s' Is Not Modal!", GetLabel().c_str());
        Destroy();
        return;
    }
    Clean_Exit();
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

bool wxDialog_Base::Gui_DataLoad(CallBackData * pCallBackData)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    return Gui_Refresh(pCallBackData);
}

bool wxDialog_Base::Gui_DataSave(CallBackData * pCallBackData)
{
    _WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    wxUnusedVar(pCallBackData);
    return true;
}

bool wxDialog_Base::Gui_Refresh(CallBackData * pCallBackData)
{
    //_WX_LOG_NFO("Label='%s'", GetLabel().c_str());
    PositionCallBackData *pCallData = dynamic_cast<PositionCallBackData *>(pCallBackData);
    _COND_RET(pCallData != NULL, false);
    Update_Position_FullScreen(pCallData->m_rectPosition.X, pCallData->m_rectPosition.Y, pCallData->m_rectPosition.Width, pCallData->m_rectPosition.Height, pCallData->m_bShowFullScreen);
    return true;
}

bool wxDialog_Base::IsInitialized()
{
    return v_bInitialized;
};

void wxDialog_Base::Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog)
{
    _WX_LOG_NFO("pData_Holder_Dialog=%p", pData_Holder_Dialog);
    v_pData_Holder_Dialog = pData_Holder_Dialog;
}

void wxDialog_Base::Set_WaitUser(Extern_Task_Data *pExtern_Task_Data)
{
    _WX_LOG_NFO("pExtern_Task_Data=%p", pExtern_Task_Data);
    v_pExtern_Task_Data_WaitUser = pExtern_Task_Data;
}

void wxDialog_Base::Set_WaitInitialized(Extern_Task_Data *pExtern_Task_Data)
{
    _WX_LOG_NFO("pExtern_Task_Data=%p", pExtern_Task_Data);
    v_pExtern_Task_Data_WaitInitialized = pExtern_Task_Data;
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
            Safe_Gui_Refresh(this, pData_Holder_Dialog->pCallBackData);
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(*pData_Holder_Dialog);
}

void wxDialog_Base::Clean_Exit()
{
    _WX_LOG_NFO("Label='%s' retCode=%d", GetLabel().c_str(), GetReturnCode());
    if (v_pExtern_Task_Data_WaitInitialized)
    {
        _WX_LOG_NFO("Send response to external task : WaitInitialized");
        Extern_Task_Response(v_pExtern_Task_Data_WaitInitialized);
        wxDELETE(v_pExtern_Task_Data_WaitInitialized);
    }
    if (v_pExtern_Task_Data_WaitUser)
    {
        _WX_LOG_NFO("Send response to external task : WaitUser");
        v_pExtern_Task_Data_WaitUser->nButtonId = GetReturnCode();
        Extern_Task_Response(v_pExtern_Task_Data_WaitUser);
        wxDELETE(v_pExtern_Task_Data_WaitUser);
    }
    if (v_pData_Holder_Dialog)
    {
        _WX_LOG_NFO("Send response to external thread");
        v_pData_Holder_Dialog->bRetCode = true;
        v_pData_Holder_Dialog->nRetCode = GetReturnCode();
        wx_semaphore_post(*v_pData_Holder_Dialog);
    }
}

void wxDialog_Base::Update_Position(const int x, const int y, const int width, const int height)
{
    // update position if changed
    int old_x = 0;
    int old_y = 0;
    GetPosition(&old_x, &old_y);
    if ( (x == old_x) && (y == old_y) )
        return;
    int old_width = 0;
    int old_height = 0;
    GetSize(&old_width, &old_height);
    if ( (width == old_width) && (height == old_height) )
        return;
    if ( (x == 0) && (y == 0) && (width == 0) && (height == 0) )
    {
        _WX_LOG_ERR(
            "Label='%s' : Bad Position : (%d, %d, %d, %d)",
            GetLabel().c_str(),
            x, y, width, height
        );
        return;
    }
    _WX_LOG_NFO(
        "Label='%s' : Changing Position : (%d, %d, %d, %d) -> (%d, %d, %d, %d)",
        GetLabel().c_str(),
        old_x, old_y, old_width, old_height,
        x, y, width, height
        );
    SetSize(x, y, width, height, wxSIZE_ALLOW_MINUS_ONE);
}

void wxDialog_Base::Update_FullScreen(bool bShowFullScreen)
{
    // update full screen if changed
    bool old_bShowFullScreen = IsFullScreen();
    if (old_bShowFullScreen == bShowFullScreen)
        return;
    _WX_LOG_NFO(
        "Label='%s' : Changing Full Screen Mode : (%d) -> (%d)",
        GetLabel().c_str(),
        old_bShowFullScreen, bShowFullScreen
        );
    ShowFullScreen(bShowFullScreen, 0);
}

void wxDialog_Base::Update_Position_FullScreen(const int x, const int y, const int width, const int height, bool bShowFullScreen)
{
    // no need to calculate, wx performs well
    Update_FullScreen(bShowFullScreen);
    Update_Position(x, y, width, height);
    return;
}
