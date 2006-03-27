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
////@end includes

#include "wxframemain.h"
#include "wx_thread_bag.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wx_safe_dialog.h"

////@begin XPM images
#include "exit.xpm"
#include "exec.xpm"
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

    EVT_MENU( ID_TOOL_DBG, wxFrameMain::OnToolDbgClick )
    EVT_UPDATE_UI( ID_TOOL_DBG, wxFrameMain::OnToolDbgUpdate )

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

////@end wxFrameMain event table entries

EVT_IDLE(wxFrameMain::OnIdle)

END_EVENT_TABLE()
;

wxFrameMain *g_pwxFrameMain = NULL;

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
    _COND_ASSIGN(g_pwxFrameMain, NULL, this);
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
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool9BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_DBG, _T("dbg"), itemtool9Bitmap, itemtool9BitmapDisabled, wxITEM_CHECK, _T("debug"), _T("debug"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool11Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool11BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_ROOMWIZARD, _T("R.W."), itemtool11Bitmap, itemtool11BitmapDisabled, wxITEM_CHECK, _T("Room Wizard"), _T("Room Wizard Dialog"));
    wxBitmap itemtool12Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool12BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITGRID, _T("W.G."), itemtool12Bitmap, itemtool12BitmapDisabled, wxITEM_CHECK, _T("Wait Grid"), _T("Dialog Wait Grid"));
    wxBitmap itemtool13Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool13BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITLIST, _T("W.L."), itemtool13Bitmap, itemtool13BitmapDisabled, wxITEM_CHECK, _T("Wait List"), _T("Dialog Wait List"));
    wxBitmap itemtool14Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    wxBitmap itemtool14BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_WAITUSER, _T("W.U."), itemtool14Bitmap, itemtool14BitmapDisabled, wxITEM_CHECK, _T("Wait User"), _T("Dialog Wait User"));
    v_pToolBar->AddSeparator();
    wxBitmap itemtool16Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool16BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_B_D, _T("U B D"), itemtool16Bitmap, itemtool16BitmapDisabled, wxITEM_CHECK, _T("U B D"), _T("Unknown Detached Blocking"));
    wxBitmap itemtool17Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool17BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_B_J, _T("U B J"), itemtool17Bitmap, itemtool17BitmapDisabled, wxITEM_CHECK, _T("U B J"), _T("Unknown Joinable Blocking"));
    wxBitmap itemtool18Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool18BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_N_D, _T("U N D"), itemtool18Bitmap, itemtool18BitmapDisabled, wxITEM_CHECK, _T("U N D"), _T("Unknown Detached NonBlocking"));
    wxBitmap itemtool19Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    wxBitmap itemtool19BitmapDisabled;
    v_pToolBar->AddTool(ID_TOOL_U_N_J, _T("U N J"), itemtool19Bitmap, itemtool19BitmapDisabled, wxITEM_CHECK, _T("U N J"), _T("Unknown Joinable NonBlocking"));
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

    new wxThread_Bag;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN
 */

void wxFrameMain::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
    _WX_LOG_NFO();
////@begin wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
}

/*!
 * wxEVT_IDLE event handler for ID_FRAMEMAIN
 */

void wxFrameMain::OnIdle( wxIdleEvent& event )
{
    wxAppSetReady();
    wxFrame::OnIdle(event);
////@begin wxEVT_IDLE event handler for ID_FRAMEMAIN in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler for ID_FRAMEMAIN in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void wxFrameMain::OnExitClick( wxCommandEvent& WXUNUSED(event) )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in wxFrameMain.
    // Before editing this code, remove the block markers.
    Destroy();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_DBG
 */

void wxFrameMain::OnToolDbgClick( wxCommandEvent& event )
{
#ifdef USE_DEBUG_CODE
    if (g_pwxThread_Bag->GetCount())
    {
        g_pwxThread_Bag->DestroyAll();
    }
    else
    {
        ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_refresh_update", _debug_refresh_update) )->Start();
    }
#endif // USE_DEBUG_CODE
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
    v_pToolBar->ToggleTool(ID_TOOL_DBG, g_pwxThread_Bag->GetCount());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_DBG in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_DBG in wxFrameMain. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD
 */

void wxFrameMain::OnToolRoomwizardClick( wxCommandEvent& event )
{
#ifdef USE_DEBUG_CODE
    wxDialog_RoomWizard *pwxDialog = ptr_wxDialogByType<wxDialog_RoomWizard>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        pwxDialog = Safe_CreateUnique<wxDialog_RoomWizard>(_debug_RoomWizard());
        Safe_Show(pwxDialog);
    }
#endif // USE_DEBUG_CODE
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
#ifdef USE_DEBUG_CODE
    wxDialog_WaitGrid *pwxDialog = ptr_wxDialogByType<wxDialog_WaitGrid>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        pwxDialog = Safe_CreateUnique<wxDialog_WaitGrid>();
        Safe_Show(pwxDialog);
    }
#endif // USE_DEBUG_CODE
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
#ifdef USE_DEBUG_CODE
    wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        pwxDialog = Safe_CreateUnique<wxDialog_WaitList>();
        Safe_Show(pwxDialog);
    }
#endif // USE_DEBUG_CODE
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
#ifdef USE_DEBUG_CODE
    wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
    if (pwxDialog)
        Safe_Close(pwxDialog);
    else
    {
        pwxDialog = Safe_CreateUnique<wxDialog_WaitUser>();
        Safe_Show(pwxDialog);
    }
#endif // USE_DEBUG_CODE
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
    if (g_pwxThread_Bag->GetCount())
    {
        g_pwxThread_Bag->DestroyAll();
    }
    else
    {
        ( new wxThread_Cmd(wxTHREAD_DETACHED, "_debug_thread_block_D", _debug_thread_block) )->Start();
    }
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
    v_pToolBar->ToggleTool(ID_TOOL_U_B_D, g_pwxThread_Bag->GetCount());
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
    if (g_pwxThread_Bag->GetCount())
    {
        g_pwxThread_Bag->DestroyAll();
    }
    else
    {
        ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_thread_block_J", _debug_thread_block) )->Start();
    }
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
    v_pToolBar->ToggleTool(ID_TOOL_U_B_J, g_pwxThread_Bag->GetCount());
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
    if (g_pwxThread_Bag->GetCount())
    {
        g_pwxThread_Bag->DestroyAll();
    }
    else
    {
        ( new wxThread_Cmd(wxTHREAD_DETACHED, "_debug_thread_nonblock_D", _debug_thread_nonblock) )->Start();
    }
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
    v_pToolBar->ToggleTool(ID_TOOL_U_N_D, g_pwxThread_Bag->GetCount());
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
    if (g_pwxThread_Bag->GetCount())
    {
        g_pwxThread_Bag->DestroyAll();
    }
    else
    {
        ( new wxThread_Cmd(wxTHREAD_JOINABLE, "_debug_thread_nonblock_J", _debug_thread_nonblock) )->Start();
    }
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
    v_pToolBar->ToggleTool(ID_TOOL_U_N_J, g_pwxThread_Bag->GetCount());
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J in wxFrameMain.
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
    else if (name == _T("exec.png"))
    {
        wxBitmap bitmap(exec_xpm);
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

wxFrameMain::~wxFrameMain()
{
    _COND_ASSIGN(g_pwxFrameMain, this, NULL);
}

bool wxFrameMain::Destroy()
{
    _WX_LOG_NFO();
    if (wxThread_Bag *pwxThread_Bag = g_pwxThread_Bag)
        delete pwxThread_Bag;
    return wxFrame::Destroy();
}
