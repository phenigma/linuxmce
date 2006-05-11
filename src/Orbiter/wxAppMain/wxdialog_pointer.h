#ifndef _WXDIALOG_POINTER_H_
#define _WXDIALOG_POINTER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_pointer.h"
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
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_POINTER 10050
#define SYMBOL_WXDIALOG_POINTER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_WXDIALOG_POINTER_TITLE _T("Pointer Demo")
#define SYMBOL_WXDIALOG_POINTER_IDNAME ID_DIALOG_POINTER
#define SYMBOL_WXDIALOG_POINTER_SIZE wxSize(130, 113)
#define SYMBOL_WXDIALOG_POINTER_POSITION wxDefaultPosition
#define ID_BUTTON_CONSTRAINMOUSE 10052
#define ID_BUTTON_LOADIMAGE 10051
#define ID_CHOICE_STD 10054
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_Pointer class declaration
 */

class wxDialog_Pointer: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_Pointer );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_Pointer( );
    wxDialog_Pointer( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_POINTER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_POINTER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_POINTER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_POINTER_SIZE, long style = SYMBOL_WXDIALOG_POINTER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_POINTER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_POINTER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_POINTER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_POINTER_SIZE, long style = SYMBOL_WXDIALOG_POINTER_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_Pointer event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE
    void OnButtonConstrainmouseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE
    void OnButtonLoadimageClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE
    void OnCloseClick( wxCommandEvent& event );

////@end wxDialog_Pointer event handler declarations

////@begin wxDialog_Pointer member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_Pointer member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_Pointer member variables
    wxBoxSizer* v_pBoxV_all;
    wxBoxSizer* v_pBoxH_Buttons;
    wxButton* v_pButton_ConstrainMouse;
    wxButton* v_pButton_LoadImage;
    wxBitmapButton* v_pButton_Close;
    wxChoice* v_pChoice;
    wxStaticText* v_pStaticText;
////@end wxDialog_Pointer member variables

public:
    ~wxDialog_Pointer();
};

#endif
// _WXDIALOG_POINTER_H_
