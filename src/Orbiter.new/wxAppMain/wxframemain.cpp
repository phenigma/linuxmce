//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxframemain.h"
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
#include "wxdialog_pointer.h"
#include "wxdialog_gl.h"
#include "wxdialog_sdl.h"
////@end includes

#include "wxframemain.h"
#include "wxappmain.h"
#include "wx_thread_bag.h"
#include "wx_safe_dialog.h"
#include "wx_dialog_all_include.h"

////@begin XPM images
#include "exit.xpm"
#include "contexthelp.xpm"
#include "exec.xpm"
#include "bug.xpm"
#include "clock.xpm"
////@end XPM images

/*!
 * wxFrameMain type definition
 */

IMPLEMENT_CLASS( wxFrameMain, wxFrame )
    ;

/*!
 * wxFrameMain event table definition
 */

BEGIN_EVENT_TABLE( wxFrameMain, wxFrame )

////@begin wxFrameMain event table entries
    EVT_CLOSE( wxFrameMain::OnCloseWindow )
    EVT_IDLE( wxFrameMain::OnIdle )

    EVT_MENU( wxID_EXIT, wxFrameMain::OnExitClick )

    EVT_MENU( ID_TOOL_POINTER, wxFrameMain::OnToolPointerClick )

    EVT_MENU( ID_TOOL_GL, wxFrameMain::OnToolGlClick )

    EVT_MENU( ID_TOOL_SDL, wxFrameMain::OnToolSdlClick )

    EVT_MENU( ID_TOOL_DBG, wxFrameMain::OnToolDbgClick )
    EVT_UPDATE_UI( ID_TOOL_DBG, wxFrameMain::OnToolDbgUpdate )

    EVT_MENU( ID_TOOL_SPEEDCONTOL, wxFrameMain::OnToolSpeedcontolClick )
    EVT_UPDATE_UI( ID_TOOL_SPEEDCONTOL, wxFrameMain::OnToolSpeedcontolUpdate )

    EVT_MENU( ID_TOOL_VOLUMECONTROL, wxFrameMain::OnToolVolumecontrolClick )
    EVT_UPDATE_UI( ID_TOOL_VOLUMECONTROL, wxFrameMain::OnToolVolumecontrolUpdate )

    EVT_MENU( ID_TOOL_LIGHTCONTROL, wxFrameMain::OnToolLightcontrolClick )
    EVT_UPDATE_UI( ID_TOOL_LIGHTCONTROL, wxFrameMain::OnToolLightcontrolUpdate )

    EVT_MENU( ID_TOOL_ROOMWIZARD, wxFrameMain::OnToolRoomwizardClick )
    EVT_UPDATE_UI( ID_TOOL_ROOMWIZARD, wxFrameMain::OnToolRoomwizardUpdate )

    EVT_MENU( ID_TOOL_WAITGRID, wxFrameMain::OnToolWaitgridClick )
    EVT_UPDATE_UI( ID_TOOL_WAITGRID, wxFrameMain::OnToolWaitgridUpdate )

    EVT_MENU( ID_TOOL_WAITLIST, wxFrameMain::OnToolWaitlistClick )
    EVT_UPDATE_UI( ID_TOOL_WAITLIST, wxFrameMain::OnToolWaitlistUpdate )

    EVT_MENU( ID_TOOL_WAITUSER, wxFrameMain::OnToolWaituserClick )
    EVT_UPDATE_UI( ID_TOOL_WAITUSER, wxFrameMain::OnToolWaituserUpdate )

    EVT_MENU( ID_TOOL_U_B_D, wxFrameMain::OnToolUBDClick )
    EVT_UPDATE_UI( ID_TOOL_U_B_D, wxFrameMain::OnToolUBDUpdate )

    EVT_MENU( ID_TOOL_U_B_J, wxFrameMain::OnToolUBJClick )
    EVT_UPDATE_UI( ID_TOOL_U_B_J, wxFrameMain::OnToolUBJUpdate )

    EVT_MENU( ID_TOOL_U_N_D, wxFrameMain::OnToolUNDClick )
    EVT_UPDATE_UI( ID_TOOL_U_N_D, wxFrameMain::OnToolUNDUpdate )

    EVT_MENU( ID_TOOL_U_N_J, wxFrameMain::OnToolUNJClick )
    EVT_UPDATE_UI( ID_TOOL_U_N_J, wxFrameMain::OnToolUNJUpdate )

    EVT_MENU( ID_TOOL_STOP, wxFrameMain::OnToolStopClick )
    EVT_UPDATE_UI( ID_TOOL_STOP, wxFrameMain::OnToolStopUpdate )

////@end wxFrameMain event table entries

    EVT_IDLE(wxFrameMain::OnIdle)

    END_EVENT_TABLE()
    ;

/*!
 * wxFrameMain constructors
 */

wxFrameMain::wxFrameMain( )
{
    _WX_LOG_NFO();
}

wxFrameMain::wxFrameMain( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
    Create( parent, id, caption, pos, size, style );
}

/*!
 * wxFrameMain creator
 */

bool wxFrameMain::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
////@begin wxFrameMain member initialisation
    v_pToolBar = NULL;
    v_pLogTextCtrl = NULL;
    v_pStatusBar = NULL;
////@end wxFrameMain member initialisation

////@begin wxFrameMain creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
////@end wxFrameMain creation
    return true;
}

/*!
 * Control creation for wxFrameMain
 */

void wxFrameMain::CreateControls()
{
    _WX_LOG_NFO();
////@begin wxFrameMain content construction
    wxFrameMain* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu3 = new wxMenu;
    itemMenu3->AppendSeparator();
    {
        wxMenuItem* menuItem = new wxMenuItem(itemMenu3, wxID_EXIT, _T("&Quit"), _T("Quit Application"), wxITEM_NORMAL);
        wxBitmap bitmap(itemFrame1->GetBitmapResource(wxT("exit.png")));
        menuItem->SetBitmap(bitmap);
        itemMenu3->Append(menuItem);
    }
    menuBar->Append(itemMenu3, _T("&Menu"));
    itemFrame1->SetMenuBar(menuBar);

    v_pToolBar = CreateToolBar( wxTB_FLAT|wxTB_DOCKABLE|wxTB_HORIZONTAL|wxTB_TEXT|wxCLIP_CHILDREN , ID_TOOLBAR_MAIN );
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("exit.png")));
    wxBitmap itemtool7BitmapDisabled;
    v_pToolBar->AddTool(wxID_EXIT, _T("Quit"), itemtool7Bitmap, itemtool7BitmapDisabled, wxITEM_NORMAL, _T("Quit Application"), _T("Quit Application"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("contexthelp.png")));
    wxBitmap itemtool9BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_POINTER, _T("Ptr"), itemtool9Bitmap, itemtool9BitmapDisabled, wxITEM_NORMAL, _T("Pointer Demo"), _T("Pointer Demo"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool11Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool11BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_GL, _T("GL"), itemtool11Bitmap, itemtool11BitmapDisabled, wxITEM_NORMAL, _T("GL window"), _T("GL window"));
    wxBitmap itemtool12Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool12BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_SDL, _T("SDL"), itemtool12Bitmap, itemtool12BitmapDisabled, wxITEM_NORMAL, _T("SDL window"), _T("SDL window"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool14Bitmap(itemFrame1->GetBitmapResource(wxT("bug.png")));
    wxBitmap itemtool14BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_DBG, _T("dbg"), itemtool14Bitmap, itemtool14BitmapDisabled, wxITEM_CHECK, _T("debug"), _T("debug"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool16Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool16BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_SPEEDCONTOL, _T("Spd"), itemtool16Bitmap, itemtool16BitmapDisabled, wxITEM_CHECK, _T("Speed Control"), _T("Speed Control"));
    wxBitmap itemtool17Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool17BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_VOLUMECONTROL, _T("Vol"), itemtool17Bitmap, itemtool17BitmapDisabled, wxITEM_CHECK, _T("Volume Control"), _T("Volume Control"));
    wxBitmap itemtool18Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool18BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_LIGHTCONTROL, _T("Lght"), itemtool18Bitmap, itemtool18BitmapDisabled, wxITEM_CHECK, _T("Light Control"), _T("Light Control"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool20Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool20BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_ROOMWIZARD, _T("R.W."), itemtool20Bitmap, itemtool20BitmapDisabled, wxITEM_CHECK, _T("Room Wizard"), _T("Room Wizard"));
    wxBitmap itemtool21Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool21BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITGRID, _T("W.G."), itemtool21Bitmap, itemtool21BitmapDisabled, wxITEM_CHECK, _T("Wait Grid"), _T("Wait Grid"));
    wxBitmap itemtool22Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool22BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITLIST, _T("W.L."), itemtool22Bitmap, itemtool22BitmapDisabled, wxITEM_CHECK, _T("Wait List"), _T("Wait List"));
    wxBitmap itemtool23Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool23BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITUSER, _T("W.U."), itemtool23Bitmap, itemtool23BitmapDisabled, wxITEM_CHECK, _T("Wait User"), _T("Wait User"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool25Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool25BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_B_D, _T("U B D"), itemtool25Bitmap, itemtool25BitmapDisabled, wxITEM_CHECK, _T("U B D"), _T("Unknown Detached Blocking"));
    wxBitmap itemtool26Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool26BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_B_J, _T("U B J"), itemtool26Bitmap, itemtool26BitmapDisabled, wxITEM_CHECK, _T("U B J"), _T("Unknown Joinable Blocking"));
    wxBitmap itemtool27Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool27BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_N_D, _T("U N D"), itemtool27Bitmap, itemtool27BitmapDisabled, wxITEM_CHECK, _T("U N D"), _T("Unknown Detached NonBlocking"));
    wxBitmap itemtool28Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool28BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_N_J, _T("U N J"), itemtool28Bitmap, itemtool28BitmapDisabled, wxITEM_CHECK, _T("U N J"), _T("Unknown Joinable NonBlocking"));
    wxBitmap itemtool29Bitmap(itemFrame1->GetBitmapResource(wxT("exit.png")));
    wxBitmap itemtool29BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_STOP, _T("stop"), itemtool29Bitmap, itemtool29BitmapDisabled, wxITEM_NORMAL, _T("stop all threads"), _T("stop all threads"));
    v_pToolBar->AddSeparator();
    v_pToolBar->Realize();
    itemFrame1->SetToolBar(v_pToolBar);

    v_pLogTextCtrl = new wxTextCtrl;
    v_pLogTextCtrl->Create( itemFrame1, ID_TEXTCTRL_LOG, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    v_pLogTextCtrl->SetForegroundColour(wxColour(0, 0, 128));
    v_pLogTextCtrl->SetBackgroundColour(wxColour(229, 229, 229));
    v_pLogTextCtrl->SetFont(wxFont(8, wxTELETYPE, wxNORMAL, wxNORMAL, false, _T("Console")));

    v_pStatusBar = new wxStatusBar;
    v_pStatusBar->Create( itemFrame1, ID_STATUSBAR_MAIN, wxST_SIZEGRIP|wxNO_BORDER|wxCLIP_CHILDREN  );
    v_pStatusBar->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Sans")));
    v_pStatusBar->SetFieldsCount(1);
    itemFrame1->SetStatusBar(v_pStatusBar);

////@end wxFrameMain content construction

#ifdef USE_RELEASE_CODE
    itemMenu3->Enable(wxID_EXIT, false);
    v_pToolBar->EnableTool(wxID_EXIT, false);
    v_pToolBar->EnableTool(ID_TOOL_DBG, false);
    v_pToolBar->EnableTool(ID_TOOL_SPEEDCONTOL, false);
    v_pToolBar->EnableTool(ID_TOOL_VOLUMECONTROL, false);
    v_pToolBar->EnableTool(ID_TOOL_LIGHTCONTROL, false);
    v_pToolBar->EnableTool(ID_TOOL_ROOMWIZARD, false);
    v_pToolBar->EnableTool(ID_TOOL_WAITGRID, false);
    v_pToolBar->EnableTool(ID_TOOL_WAITLIST, false);
    v_pToolBar->EnableTool(ID_TOOL_WAITUSER, false);
    v_pToolBar->EnableTool(ID_TOOL_U_B_D, false);
    v_pToolBar->EnableTool(ID_TOOL_U_B_J, false);
    v_pToolBar->EnableTool(ID_TOOL_U_N_D, false);
    v_pToolBar->EnableTool(ID_TOOL_U_N_J, false);
    v_pToolBar->EnableTool(ID_TOOL_STOP, false);
    v_pToolBar->EnableTool(ID_TOOL_GL, false);
    v_pToolBar->EnableTool(ID_TOOL_SDL, false);
#endif // USE_RELEASE_CODE

#if (!defined WX_GL_DEMO)
    v_pToolBar->RemoveTool(ID_TOOL_GL);
#endif // (!defined WX_GL_DEMO)
#if (!defined WX_SDL_DEMO)
    v_pToolBar->RemoveTool(ID_TOOL_SDL);
#endif // (!defined WX_SDL_DEMO)
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN
 */

void wxFrameMain::OnCloseWindow( wxCloseEvent& event )
{
    _WX_LOG_NFO();
////@begin wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
    wxUnusedVar(event);
}

/*!
 * wxEVT_IDLE event handler for ID_FRAMEMAIN
 */

void wxFrameMain::OnIdle( wxIdleEvent& event )
{
    //_WX_LOG_NFO();
    App_SetReady();
    wxFrame::OnIdle(event);
////@begin wxEVT_IDLE event handler for ID_FRAMEMAIN in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler for ID_FRAMEMAIN in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void wxFrameMain::OnExitClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in wxFrameMain.
    // Before editing this code, remove the block markers.
    Destroy();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in wxFrameMain.
    wxUnusedVar(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_POINTER
 */

void wxFrameMain::OnToolPointerClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_POINTER in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxDialog_Pointer* window = new wxDialog_Pointer(NULL, ID_DIALOG_POINTER, _T("Pointer Demo"));
    window->Show(true);
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_POINTER in wxFrameMain. 
    wxUnusedVar(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_GL
 */

void wxFrameMain::OnToolGlClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_GL in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxDialog_GL* window = new wxDialog_GL(NULL, ID_DIALOG_GL, _T("OpenGL Demo"));
    window->Show(true);
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_GL in wxFrameMain.
    wxUnusedVar(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SDL
 */

void wxFrameMain::OnToolSdlClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SDL in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxDialog_SDL* window = new wxDialog_SDL(NULL, ID_DIALOG_SDL, _T("SDL Demo"));
    window->Show(true);
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SDL in wxFrameMain.
    wxUnusedVar(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_DBG
 */

void wxFrameMain::OnToolDbgClick( wxCommandEvent& event )
{
    wxThread_Cmd *pwxThread_Cmd = wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_refresh_update");
    if (pwxThread_Cmd)
        pwxThread_Cmd->Stop();
    else
        ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_refresh_update", _debug_refresh_update) )->Start();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_DBG in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_DBG in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_DBG
 */

void wxFrameMain::OnToolDbgUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_DBG, wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_refresh_update"));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_DBG in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_DBG in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SPEEDCONTOL
 */

void wxFrameMain::OnToolSpeedcontolClick( wxCommandEvent& event )
{
    wxDialog_SpeedControl *pwxDialog = ptr_wxDialogByType<wxDialog_SpeedControl>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        SpeedControlCallBackData *pCallBackData = new SpeedControlCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_SpeedControl>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SPEEDCONTOL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SPEEDCONTOL in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_SPEEDCONTOL
 */

void wxFrameMain::OnToolSpeedcontolUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_SPEEDCONTOL, ptr_wxDialogByType<wxDialog_SpeedControl>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_SPEEDCONTOL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_SPEEDCONTOL in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_VOLUMECONTROL
 */

void wxFrameMain::OnToolVolumecontrolClick( wxCommandEvent& event )
{
    wxDialog_VolumeControl *pwxDialog = ptr_wxDialogByType<wxDialog_VolumeControl>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        VolumeControlCallBackData *pCallBackData = new VolumeControlCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_VolumeControl>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_VOLUMECONTROL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_VOLUMECONTROL in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_VOLUMECONTROL
 */

void wxFrameMain::OnToolVolumecontrolUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_VOLUMECONTROL, ptr_wxDialogByType<wxDialog_VolumeControl>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_VOLUMECONTROL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_VOLUMECONTROL in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_LIGHTCONTROL
 */

void wxFrameMain::OnToolLightcontrolClick( wxCommandEvent& event )
{
    wxDialog_LightControl *pwxDialog = ptr_wxDialogByType<wxDialog_LightControl>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        LightControlCallBackData *pCallBackData = new LightControlCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_LightControl>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_LIGHTCONTROL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_LIGHTCONTROL in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_LIGHTCONTROL
 */

void wxFrameMain::OnToolLightcontrolUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_LIGHTCONTROL, ptr_wxDialogByType<wxDialog_LightControl>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_LIGHTCONTROL in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_LIGHTCONTROL in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD
 */

void wxFrameMain::OnToolRoomwizardClick( wxCommandEvent& event )
{
    wxDialog_RoomWizard *pwxDialog = ptr_wxDialogByType<wxDialog_RoomWizard>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        RoomWizardCallBackData *pCallBackData = new RoomWizardCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_RoomWizard>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_ROOMWIZARD
 */

void wxFrameMain::OnToolRoomwizardUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_ROOMWIZARD, ptr_wxDialogByType<wxDialog_RoomWizard>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_ROOMWIZARD in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_ROOMWIZARD in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITGRID
 */

void wxFrameMain::OnToolWaitgridClick( wxCommandEvent& event )
{
    wxDialog_WaitGrid *pwxDialog = ptr_wxDialogByType<wxDialog_WaitGrid>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        WaitUserGridCallBackData *pCallBackData = new WaitUserGridCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_WaitGrid>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITGRID in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITGRID in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_WAITGRID
 */

void wxFrameMain::OnToolWaitgridUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_WAITGRID, ptr_wxDialogByType<wxDialog_WaitGrid>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_WAITGRID in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_WAITGRID in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITLIST
 */

void wxFrameMain::OnToolWaitlistClick( wxCommandEvent& event )
{
    wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        WaitUserListCallBackData *pCallBackData = new WaitUserListCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_WaitList>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITLIST in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITLIST in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_WAITLIST
 */

void wxFrameMain::OnToolWaitlistUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_WAITLIST, ptr_wxDialogByType<wxDialog_WaitList>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_WAITLIST in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_WAITLIST in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITUSER
 */

void wxFrameMain::OnToolWaituserClick( wxCommandEvent& event )
{
    wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        WaitUserPromptCallBackData *pCallBackData = new WaitUserPromptCallBackData;
        _debug_init(pCallBackData);
        pwxDialog = Safe_CreateUnique<wxDialog_WaitUser>(pCallBackData);
        Safe_Show(pwxDialog);
        wxDELETE(pCallBackData);
    }
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITUSER in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITUSER in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER
 */

void wxFrameMain::OnToolWaituserUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_WAITUSER, ptr_wxDialogByType<wxDialog_WaitUser>());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D
 */

void wxFrameMain::OnToolUBDClick( wxCommandEvent& event )
{
    wxThread_Cmd *pwxThread_Cmd = wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_block_D");
    if (pwxThread_Cmd)
        pwxThread_Cmd->Stop();
    else
        ( new wxThread_Cmd(wxTHREAD_DETACHED, "_debug_thread_block_D", _debug_thread_block) )->Start();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_D
 */

void wxFrameMain::OnToolUBDUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_U_B_D, wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_block_D"));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_D in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_D in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_J
 */

void wxFrameMain::OnToolUBJClick( wxCommandEvent& event )
{
    wxThread_Cmd *pwxThread_Cmd = wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_block_J");
    if (pwxThread_Cmd)
        pwxThread_Cmd->Stop();
    ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_thread_block_J", _debug_thread_block) )->Start();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_J in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_J in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_J
 */

void wxFrameMain::OnToolUBJUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_U_B_J, wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_block_J"));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_J in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_J in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_D
 */

void wxFrameMain::OnToolUNDClick( wxCommandEvent& event )
{
    wxThread_Cmd *pwxThread_Cmd = wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_nonblock_D");
    if (pwxThread_Cmd)
        pwxThread_Cmd->Stop();
    else
        ( new wxThread_Cmd(wxTHREAD_DETACHED, "_debug_thread_nonblock_D", _debug_thread_nonblock) )->Start();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_D in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_D in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_D
 */

void wxFrameMain::OnToolUNDUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_U_N_D, wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_nonblock_D"));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_D in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_D in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_J
 */

void wxFrameMain::OnToolUNJClick( wxCommandEvent& event )
{
    wxThread_Cmd *pwxThread_Cmd = wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_nonblock_J");
    if (pwxThread_Cmd)
        pwxThread_Cmd->Stop();
    else
        ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_thread_nonblock_J", _debug_thread_nonblock) )->Start();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_J in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_J in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J
 */

void wxFrameMain::OnToolUNJUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_U_N_J, wxGetApp().ptr_ThreadBag()->ptr_ThreadItem("_debug_thread_nonblock_J"));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_STOP
 */

void wxFrameMain::OnToolStopClick( wxCommandEvent& event )
{
    if (wxGetApp().ptr_ThreadBag()->GetCount())
        wxGetApp().ptr_ThreadBag()->DestroyAll();
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_STOP in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_STOP in wxFrameMain.
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_STOP
 */

void wxFrameMain::OnToolStopUpdate( wxUpdateUIEvent& event )
{
    v_pToolBar->ToggleTool(ID_TOOL_STOP, wxGetApp().ptr_ThreadBag()->GetCount());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_STOP in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_STOP in wxFrameMain.
}

/*!
 * Should we show tooltips?
 */

bool wxFrameMain::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxFrameMain::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxFrameMain bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("exit.png"))
    {
        wxBitmap bitmap(exit_xpm);
        return bitmap;
    }
    else if (name == _T("contexthelp.png"))
    {
        wxBitmap bitmap(contexthelp_xpm);
        return bitmap;
    }
    else if (name == _T("exec.png"))
    {
        wxBitmap bitmap(exec_xpm);
        return bitmap;
    }
    else if (name == _T("bug.png"))
    {
        wxBitmap bitmap(bug_xpm);
        return bitmap;
    }
    else if (name == _T("clock.png"))
    {
        wxBitmap bitmap(clock_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end wxFrameMain bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxFrameMain::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxFrameMain icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxFrameMain icon retrieval
}

bool wxFrameMain::Destroy()
{
    _WX_LOG_NFO();
    wxGetApp().Clean_Exit(false);
    return wxFrame::Destroy();
}
