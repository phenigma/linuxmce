#ifndef _WXDIALOG_SPEEDCONTROL_H_
#define _WXDIALOG_SPEEDCONTROL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_speedcontrol.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wxdialog_base.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxWindow;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SPEEDCONTROL 10026
#define SYMBOL_WXDIALOG_SPEEDCONTROL_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_SPEEDCONTROL_TITLE _T("wx Dialog_SpeedControl")
#define SYMBOL_WXDIALOG_SPEEDCONTROL_IDNAME ID_DIALOG_SPEEDCONTROL
#define SYMBOL_WXDIALOG_SPEEDCONTROL_SIZE wxSize(74, 69)
#define SYMBOL_WXDIALOG_SPEEDCONTROL_POSITION wxDefaultPosition
#define ID_WINDOW_SEEK 10017
#define ID_WINDOW_SPEED 10025
#define ID_SLIDER_TIMELINE 10000
#define ID_WINDOW_LABELS 10029
#define wxID_TIME_BEG 10030
#define wxID_TIME_NOW 10031
#define wxID_TIME_END 10032
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_SpeedControl class declaration
 */

class wxDialog_SpeedControl: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_SpeedControl )
        DECLARE_EVENT_TABLE()

        public:
    /// Constructors
    wxDialog_SpeedControl( );
    wxDialog_SpeedControl( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SPEEDCONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SPEEDCONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SPEEDCONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SPEEDCONTROL_SIZE, long style = SYMBOL_WXDIALOG_SPEEDCONTROL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SPEEDCONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SPEEDCONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SPEEDCONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SPEEDCONTROL_SIZE, long style = SYMBOL_WXDIALOG_SPEEDCONTROL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_SpeedControl event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL
    void OnCloseWindow( wxCloseEvent& event );

////@end wxDialog_SpeedControl event handler declarations

////@begin wxDialog_SpeedControl member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_SpeedControl member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_SpeedControl member variables
    wxBoxSizer* v_pBoxV_all;
    wxBoxSizer* v_pBoxH_Seek;
    wxWindow* v_pWinSeek;
    wxBoxSizer* v_pBoxH_Speed;
    wxWindow* v_pWinSpeed;
    wxBoxSizer* v_pBoxH_Slider;
    wxSlider* v_pSliderTimeLine;
    wxBoxSizer* v_pBoxH_Labels;
    wxWindow* v_pWinTimeShow;
    wxStaticText* v_pTimeBeg;
    wxStaticText* v_pTimeNow;
    wxStaticText* v_pTimeEnd;
////@end wxDialog_SpeedControl member variables
};

#endif
// _WXDIALOG_SPEEDCONTROL_H_
