//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_VOLUMECONTROL_H_
#define _WXDIALOG_VOLUMECONTROL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_volumecontrol.h"
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
class wxPanel_Volume;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_VOLUMECONTROL 10027
#define SYMBOL_WXDIALOG_VOLUMECONTROL_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_VOLUMECONTROL_TITLE _T("wx Dialog_VolumeControl")
#define SYMBOL_WXDIALOG_VOLUMECONTROL_IDNAME ID_DIALOG_VOLUMECONTROL
#define SYMBOL_WXDIALOG_VOLUMECONTROL_SIZE wxDefaultSize
#define SYMBOL_WXDIALOG_VOLUMECONTROL_POSITION wxDefaultPosition
#define ID_CTRL_VOLUME 10044
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_VolumeControl class declaration
 */

class wxDialog_VolumeControl: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_VolumeControl );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_VolumeControl( );
    wxDialog_VolumeControl( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_VOLUMECONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_VOLUMECONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_VOLUMECONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_VOLUMECONTROL_SIZE, long style = SYMBOL_WXDIALOG_VOLUMECONTROL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_VOLUMECONTROL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_VOLUMECONTROL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_VOLUMECONTROL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_VOLUMECONTROL_SIZE, long style = SYMBOL_WXDIALOG_VOLUMECONTROL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_VolumeControl event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_VOLUMECONTROL
    void OnCloseWindow( wxCloseEvent& event );

////@end wxDialog_VolumeControl event handler declarations

////@begin wxDialog_VolumeControl member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_VolumeControl member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_VolumeControl member variables
    wxBoxSizer* v_pBoxV_all;
    wxPanel_Volume* v_pPanel_Volume;
////@end wxDialog_VolumeControl member variables

public:
    virtual bool Gui_Refresh(CallBackData *pCallBackData);
};

#endif
// _WXDIALOG_VOLUMECONTROL_H_
