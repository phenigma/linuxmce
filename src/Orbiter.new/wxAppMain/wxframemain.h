//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXFRAMEMAIN_H_
#define _WXFRAMEMAIN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxframemain.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxStatusBar;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAMEMAIN 10003
#define SYMBOL_WXFRAMEMAIN_STYLE wxDEFAULT_FRAME_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE|wxMAXIMIZE|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxDOUBLE_BORDER
#define SYMBOL_WXFRAMEMAIN_TITLE _T("wxApplication")
#define SYMBOL_WXFRAMEMAIN_IDNAME ID_FRAMEMAIN
#define SYMBOL_WXFRAMEMAIN_SIZE wxSize(700, 300)
#define SYMBOL_WXFRAMEMAIN_POSITION wxPoint(0, 0)
#define ID_TOOLBAR_MAIN 10006
#define ID_TOOL_POINTER 10025
#define ID_TOOL_GL 10046
#define ID_TOOL_SDL 10047
#define ID_TOOL_DBG 10000
#define ID_TOOL_SPEEDCONTOL 10033
#define ID_TOOL_VOLUMECONTROL 10034
#define ID_TOOL_LIGHTCONTROL 10035
#define ID_TOOL_ROOMWIZARD 10004
#define ID_TOOL_WAITGRID 10009
#define ID_TOOL_WAITLIST 10018
#define ID_TOOL_WAITUSER 10019
#define ID_TOOL_U_B_D 10012
#define ID_TOOL_U_B_J 10002
#define ID_TOOL_U_N_D 10011
#define ID_TOOL_U_N_J 10010
#define ID_TOOL_STOP 10001
#define ID_TEXTCTRL_LOG 10007
#define ID_STATUSBAR_MAIN 10008
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxFrameMain class declaration
 */

class wxFrameMain: public wxFrame
{
    DECLARE_CLASS( wxFrameMain );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxFrameMain( );
    wxFrameMain( wxWindow* parent, wxWindowID id = SYMBOL_WXFRAMEMAIN_IDNAME, const wxString& caption = SYMBOL_WXFRAMEMAIN_TITLE, const wxPoint& pos = SYMBOL_WXFRAMEMAIN_POSITION, const wxSize& size = SYMBOL_WXFRAMEMAIN_SIZE, long style = SYMBOL_WXFRAMEMAIN_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXFRAMEMAIN_IDNAME, const wxString& caption = SYMBOL_WXFRAMEMAIN_TITLE, const wxPoint& pos = SYMBOL_WXFRAMEMAIN_POSITION, const wxSize& size = SYMBOL_WXFRAMEMAIN_SIZE, long style = SYMBOL_WXFRAMEMAIN_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxFrameMain event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_FRAMEMAIN
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_IDLE event handler for ID_FRAMEMAIN
    void OnIdle( wxIdleEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
    void OnExitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_POINTER
    void OnToolPointerClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_GL
    void OnToolGlClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SDL
    void OnToolSdlClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_DBG
    void OnToolDbgClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_DBG
    void OnToolDbgUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SPEEDCONTOL
    void OnToolSpeedcontolClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_SPEEDCONTOL
    void OnToolSpeedcontolUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_VOLUMECONTROL
    void OnToolVolumecontrolClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_VOLUMECONTROL
    void OnToolVolumecontrolUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_LIGHTCONTROL
    void OnToolLightcontrolClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_LIGHTCONTROL
    void OnToolLightcontrolUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_ROOMWIZARD
    void OnToolRoomwizardClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_ROOMWIZARD
    void OnToolRoomwizardUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITGRID
    void OnToolWaitgridClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_WAITGRID
    void OnToolWaitgridUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITLIST
    void OnToolWaitlistClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_WAITLIST
    void OnToolWaitlistUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_WAITUSER
    void OnToolWaituserClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_WAITUSER
    void OnToolWaituserUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_D
    void OnToolUBDClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_D
    void OnToolUBDUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_B_J
    void OnToolUBJClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_U_B_J
    void OnToolUBJUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_D
    void OnToolUNDClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_D
    void OnToolUNDUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_U_N_J
    void OnToolUNJClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_U_N_J
    void OnToolUNJUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_STOP
    void OnToolStopClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TOOL_STOP
    void OnToolStopUpdate( wxUpdateUIEvent& event );

////@end wxFrameMain event handler declarations

////@begin wxFrameMain member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxFrameMain member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxFrameMain member variables
    wxToolBar* v_pToolBar;
    wxTextCtrl* v_pLogTextCtrl;
    wxStatusBar* v_pStatusBar;
////@end wxFrameMain member variables

public:
    bool Destroy();
};

#endif
// _WXFRAMEMAIN_H_
