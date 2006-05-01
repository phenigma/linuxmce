//
// Author : C Remus
//
// Changed by : ...
//

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
class wxPanel_Seek;
class wxPanel_Speed;
class wxPanel_Time;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SPEEDCONTROL 10026
#define SYMBOL_WXDIALOG_SPEEDCONTROL_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_SPEEDCONTROL_TITLE _T("wx Dialog_SpeedControl")
#define SYMBOL_WXDIALOG_SPEEDCONTROL_IDNAME ID_DIALOG_SPEEDCONTROL
#define SYMBOL_WXDIALOG_SPEEDCONTROL_SIZE wxDefaultSize
#define SYMBOL_WXDIALOG_SPEEDCONTROL_POSITION wxDefaultPosition
#define ID_CTRL_SEEK 10039
#define ID_CTRL_SPEED 10040
#define ID_CTRL_TIME 10041
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
    DECLARE_DYNAMIC_CLASS( wxDialog_SpeedControl );
    DECLARE_EVENT_TABLE();

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
    wxPanel_Seek* v_pPanel_Seek;
    wxBoxSizer* v_pBoxH_Speed;
    wxPanel_Speed* v_pPanel_Speed;
    wxBoxSizer* v_pBoxH_Time;
    wxPanel_Time* v_pPanel_Time;
////@end wxDialog_SpeedControl member variables

public:
    virtual bool Gui_Refresh(CallBackData *pCallBackData);

protected:
    struct Persistent_Data
    {
        Persistent_Data()
                : bInitialized(false)
                , oData_Refresh()
                , bShowTime(false)
                , bShowSeek(false)
                , bShowSpeed(false)
            {
            }
        bool bInitialized;
        SpeedControlCallBackData oData_Refresh;
        bool bShowTime;
        bool bShowSeek;
        bool bShowSpeed;
        int idxSpeed;
    };
    Persistent_Data v_oPersistent_Data;
};

#endif
// _WXDIALOG_SPEEDCONTROL_H_
