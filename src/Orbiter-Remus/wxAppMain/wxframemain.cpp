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
#include "wx_other.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wxthreadbag.h"
#include "wxthreadevent.h"

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

    EVT_MENU( wxID_EXIT, wxFrameMain::OnExitClick )

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

  EVT_THREAD_COMMAND(wxID_ANY, wxFrameMain::OnThreadCommandEvent)
  EVT_IDLE(wxFrameMain::OnIdle)

  END_EVENT_TABLE()
  ;

wxFrameMain *g_pwxFrameMain = NULL;

/*!
 * wxFrameMain constructors
 */

wxFrameMain::wxFrameMain( )
{
  _wx_log_nfo("wxFrameMain::wxFrameMain()");
}

wxFrameMain::wxFrameMain( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  _wx_log_nfo("wxFrameMain::wxFrameMain(*)");
  Create( parent, id, caption, pos, size, style );
}

/*!
 * wxFrameMain creator
 */

bool wxFrameMain::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  _wx_log_nfo("wxFrameMain::Create(*)");
  wxASSERT(g_pwxFrameMain == NULL);
  g_pwxFrameMain = this;
////@begin wxFrameMain member initialisation
    v_oToolBar = NULL;
    v_oLogTextCtrl = NULL;
    v_oStatusBar = NULL;
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
  _wx_log_nfo("wxFrameMain::CreateControls()");
////@begin wxFrameMain content construction
    wxFrameMain* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu3 = new wxMenu;
    itemMenu3->AppendSeparator();
    {
        wxMenuItem* menuItem = new wxMenuItem(itemMenu3, wxID_EXIT, _T("Quit"), _T("Quit Application"), wxITEM_NORMAL);
        wxBitmap bitmap(itemFrame1->GetBitmapResource(wxT("exit.png")));
        menuItem->SetBitmap(bitmap);
        itemMenu3->Append(menuItem);
    }
    menuBar->Append(itemMenu3, _T("Menu"));
    itemFrame1->SetMenuBar(menuBar);

    v_oToolBar = CreateToolBar( wxTB_FLAT|wxTB_DOCKABLE|wxTB_HORIZONTAL|wxTB_TEXT|wxCLIP_CHILDREN , ID_TOOLBAR_MAIN );
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("exit.png")));
    v_oToolBar->AddTool(wxID_EXIT, _T("Quit"), itemtool7Bitmap, wxNullBitmap, wxITEM_NORMAL, _T("Quit Application"), _T("Quit Application"));
    v_oToolBar->AddSeparator();
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    v_oToolBar->AddTool(ID_TOOL_ROOMWIZARD, _T("R.W."), itemtool9Bitmap, wxNullBitmap, wxITEM_CHECK, _T("Room Wizard"), _T("Room Wizard Dialog"));
    wxBitmap itemtool10Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    v_oToolBar->AddTool(ID_TOOL_WAITGRID, _T("W.G."), itemtool10Bitmap, wxNullBitmap, wxITEM_CHECK, _T("Wait Grid"), _T("Dialog Wait Grid"));
    wxBitmap itemtool11Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    v_oToolBar->AddTool(ID_TOOL_WAITLIST, _T("W.L."), itemtool11Bitmap, wxNullBitmap, wxITEM_CHECK, _T("Wait List"), _T("Dialog Wait List"));
    wxBitmap itemtool12Bitmap(itemFrame1->GetBitmapResource(wxT("clock.png")));
    v_oToolBar->AddTool(ID_TOOL_WAITUSER, _T("W.U."), itemtool12Bitmap, wxNullBitmap, wxITEM_CHECK, _T("Wait User"), _T("Dialog Wait User"));
    v_oToolBar->AddSeparator();
    wxBitmap itemtool14Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    v_oToolBar->AddTool(ID_TOOL_U_B_D, _T("U B D"), itemtool14Bitmap, wxNullBitmap, wxITEM_CHECK, _T("U B D"), _T("Unknown Detached Blocking"));
    wxBitmap itemtool15Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    v_oToolBar->AddTool(ID_TOOL_U_B_J, _T("U B J"), itemtool15Bitmap, wxNullBitmap, wxITEM_CHECK, _T("U B J"), _T("Unknown Joinable Blocking"));
    wxBitmap itemtool16Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    v_oToolBar->AddTool(ID_TOOL_U_N_D, _T("U N D"), itemtool16Bitmap, wxNullBitmap, wxITEM_CHECK, _T("U N D"), _T("Unknown Detached NonBlocking"));
    wxBitmap itemtool17Bitmap(itemFrame1->GetBitmapResource(wxT("exec.png")));
    v_oToolBar->AddTool(ID_TOOL_U_N_J, _T("U N J"), itemtool17Bitmap, wxNullBitmap, wxITEM_CHECK, _T("U N J"), _T("Unknown Joinable NonBlocking"));
    v_oToolBar->AddSeparator();
    v_oToolBar->Realize();
    itemFrame1->SetToolBar(v_oToolBar);

    v_oLogTextCtrl = new wxTextCtrl;
    v_oLogTextCtrl->Create( itemFrame1, ID_TEXTCTRL_LOG, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    v_oLogTextCtrl->SetForegroundColour(wxColour(0, 0, 128));
    v_oLogTextCtrl->SetBackgroundColour(wxColour(229, 229, 229));
    v_oLogTextCtrl->SetFont(wxFont(8, wxTELETYPE, wxNORMAL, wxNORMAL, false, _T("Console")));

    v_oStatusBar = new wxStatusBar;
    v_oStatusBar->Create( itemFrame1, ID_STATUSBAR_MAIN, wxST_SIZEGRIP|wxNO_BORDER|wxCLIP_CHILDREN  );
    v_oStatusBar->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Sans")));
    v_oStatusBar->SetFieldsCount(1);
    itemFrame1->SetStatusBar(v_oStatusBar);

////@end wxFrameMain content construction
  pLogChain = NULL;
  pLogChain = new wxLogChain(new wxLogTextCtrl(v_oLogTextCtrl));
  new wxThreadBag;
#ifdef USE_DEBUG_CODE
#else // USE_DEBUG_CODE
  CommandLineParams commandlineparams;
  bool bStartedOK = ParseCommandLineParams(wxTheApp->argc, wxTheApp->argv, commandlineparams);
  if (! bStartedOK)
    return;
	sdl_event_loop_data.pOrbiter = CreateOrbiter(commandlineparams.PK_Device, commandlineparams.PK_DeviceTemplate, commandlineparams.sRouter_IP, commandlineparams.sLocalDirectory, commandlineparams.bLocalMode, commandlineparams.Width, commandlineparams.Height, commandlineparams.bFullScreen);
#endif // USE_DEBUG_CODE
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN
 */

void wxFrameMain::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
  _wx_log_nfo("wxFrameMain::OnCloseWindow(*)");
////@begin wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN in wxFrameMain.
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
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD
 */

void wxFrameMain::OnToolRoomwizardClick( wxCommandEvent& event )
{
  if (wxWindow::FindWindowByName(SYMBOL_WXDIALOG_ROOMWIZARD_TITLE))
    wxDialog_RoomWizard_Close(false);
  else
    wxDialog_RoomWizard_Show();
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
  v_oToolBar->ToggleTool(SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME, wxWindow::FindWindowByName(SYMBOL_WXDIALOG_ROOMWIZARD_TITLE));
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
  wxDialog_WaitGrid *pwxDialog = (wxDialog_WaitGrid *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITGRID_TITLE);
  if (pwxDialog)
  {
    pwxDialog->Destroy();
  }
  else
  {
    pwxDialog = new wxDialog_WaitGrid(wxTheApp->GetTopWindow(), SYMBOL_WXDIALOG_WAITGRID_IDNAME, SYMBOL_WXDIALOG_WAITGRID_TITLE);
    pwxDialog->Show(true);
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
  v_oToolBar->ToggleTool(SYMBOL_WXDIALOG_WAITGRID_IDNAME, wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITGRID_TITLE));
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
  wxDialog_WaitList *pwxDialog = (wxDialog_WaitList *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITLIST_TITLE);
  if (pwxDialog)
  {
    pwxDialog->Destroy();
  }
  else
  {
    pwxDialog = new wxDialog_WaitList(wxTheApp->GetTopWindow(), SYMBOL_WXDIALOG_WAITLIST_IDNAME, SYMBOL_WXDIALOG_WAITLIST_TITLE);
    pwxDialog->Show(true);
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
  v_oToolBar->ToggleTool(SYMBOL_WXDIALOG_WAITLIST_IDNAME, wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITLIST_TITLE));
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
  wxDialog_WaitUser *pwxDialog = (wxDialog_WaitUser *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITUSER_TITLE);
  if (pwxDialog)
  {
    pwxDialog->Destroy();
  }
  else
  {
    pwxDialog = new wxDialog_WaitUser(wxTheApp->GetTopWindow(), SYMBOL_WXDIALOG_WAITUSER_IDNAME, SYMBOL_WXDIALOG_WAITUSER_TITLE);
    pwxDialog->Show(true);
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
  v_oToolBar->ToggleTool(SYMBOL_WXDIALOG_WAITUSER_IDNAME, wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITUSER_TITLE));
////@begin wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER in wxFrameMain.
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D
 */

void wxFrameMain::OnToolUBDClick( wxCommandEvent& WXUNUSED(event) )
{
  if (g_pwxThreadBag->GetCount())
  {
    g_pwxThreadBag->DestroyAll();
  }
  else
  {
    g_pwxThreadBag->Start(debug_thread_sample_block, wxTHREAD_DETACHED, "debug_thread_sample_block_D");
  }
  /*
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D in wxFrameMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D in wxFrameMain.
*/
}

/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_D
 */

void wxFrameMain::OnToolUBDUpdate( wxUpdateUIEvent& event )
{
  v_oToolBar->ToggleTool(ID_TOOL_U_B_D, g_pwxThreadBag->GetCount());
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
  if (g_pwxThreadBag->GetCount())
  {
    g_pwxThreadBag->DestroyAll();
  }
  else
  {
    g_pwxThreadBag->Start(debug_thread_sample_block, wxTHREAD_JOINABLE, "debug_thread_sample_block_J");
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
  v_oToolBar->ToggleTool(ID_TOOL_U_B_J, g_pwxThreadBag->GetCount());
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
  if (g_pwxThreadBag->GetCount())
  {
    g_pwxThreadBag->DestroyAll();
  }
  else
  {
    g_pwxThreadBag->Start(debug_thread_sample_nonblock, wxTHREAD_DETACHED, "debug_thread_sample_nonblock_D");
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
  v_oToolBar->ToggleTool(ID_TOOL_U_N_D, g_pwxThreadBag->GetCount());
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
  if (g_pwxThreadBag->GetCount())
  {
    g_pwxThreadBag->DestroyAll();
  }
  else
  {
    g_pwxThreadBag->Start(debug_thread_sample_nonblock, wxTHREAD_JOINABLE, "debug_thread_sample_nonblock_J");
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
  v_oToolBar->ToggleTool(ID_TOOL_U_N_J, g_pwxThreadBag->GetCount());
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
  //_wx_log_nfo("wxFrameMain::GetBitmapResource(*)");
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
  _wx_log_nfo("wxFrameMain::GetIconResource(*)");
  // Icon retrieval
////@begin wxFrameMain icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxFrameMain icon retrieval
}

bool wxFrameMain::Destroy()
{
  _wx_log_nfo("wxFrameMain::Destroy()");
  if (wxThreadBag *pwxThreadBag = g_pwxThreadBag)
    delete pwxThreadBag;
  if (pLogChain)
    pLogChain->SetLog(NULL);
  // NO : wxDELETE(pLogChain);
#ifdef USE_DEBUG_CODE
#else // USE_DEBUG_CODE
  if (sdl_event_loop_data.pOrbiter->m_bReload)
    g_nExitCode = 2;
  SDL_Event_Loop_End(sdl_event_loop_data);
	delete g_pPlutoLogger;
	//if( bReload )
	//	return 2;
	//else
	//    return 0;
#endif // USE_DEBUG_CODE
  return wxFrame::Destroy();
}

wxFrameMain::~wxFrameMain()
{
  wxASSERT(g_pwxFrameMain == this);
  g_pwxFrameMain = NULL;
}

void wxFrameMain::OnThreadCommandEvent(wxCommandEvent& event)
{
  _wx_log_nfo("wxFrameMain::OnThreadCommandEvent(*) : %s", _str_thread_event(event));
}

void wxFrameMain::OnIdle(wxIdleEvent& WXUNUSED(event))
{
  //_wx_log_nfo("wxFrameMain::OnIdle(*)");
  //_wx_log_status_threads();
#if (! defined USE_DEBUG_CODE)
  if (sdl_event_loop_data.pOrbiter)
  {
    if (! SDL_Event_Process(sdl_event_loop_data))
      Destroy();
  }
  wx_Sleep(0, 10);
  ::wxWakeUpIdle();
#endif // (! defined USE_DEBUG_CODE)
}
