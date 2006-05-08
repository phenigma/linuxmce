//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_BASE_H_
#define _WXDIALOG_BASE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_base.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wx_dialog_types.h"
#include "wx_extern_helpers.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_BASE 10024
#define SYMBOL_WXDIALOG_BASE_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN
#define SYMBOL_WXDIALOG_BASE_TITLE _T("wx Dialog_Base")
#define SYMBOL_WXDIALOG_BASE_IDNAME ID_DIALOG_BASE
#define SYMBOL_WXDIALOG_BASE_SIZE wxDefaultSize
#define SYMBOL_WXDIALOG_BASE_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

struct Data_Holder_Dialog;

/*!
 * wxDialog_Base class declaration
 */

class wxDialog_Base: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( wxDialog_Base );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_Base( );
    wxDialog_Base( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_BASE_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_BASE_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_BASE_POSITION, const wxSize& size = SYMBOL_WXDIALOG_BASE_SIZE, long style = SYMBOL_WXDIALOG_BASE_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_BASE_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_BASE_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_BASE_POSITION, const wxSize& size = SYMBOL_WXDIALOG_BASE_SIZE, long style = SYMBOL_WXDIALOG_BASE_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_Base event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_BASE
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_IDLE event handler for ID_DIALOG_BASE
    void OnIdle( wxIdleEvent& event );

////@end wxDialog_Base event handler declarations

////@begin wxDialog_Base member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_Base member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_Base member variables
////@end wxDialog_Base member variables

public:
    virtual ~wxDialog_Base();
    virtual bool Destroy();
    virtual void EndModal(int retCode);
    void OnButton_EndModal( wxCommandEvent& event );
    void OnButton_SetReturnCode( wxCommandEvent& event );

    // load data from external object, called after create
    // by default calls Gui_Refresh
    virtual bool Gui_DataLoad(CallBackData *pCallBackData);

    // save data to external object
    // not called by default
    virtual bool Gui_DataSave(CallBackData *pCallBackData);

    // GUI refresh related code should be implemented here
    // by default set full-screen or update position
    virtual bool Gui_Refresh(CallBackData *pCallBackData);

    bool IsInitialized();
    void Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog);
    void Set_WaitUser(Extern_Task_Data *pExtern_Task_Data);
    void Set_WaitInitialized(Extern_Task_Data *pExtern_Task_Data);

protected:
    void OnWindowCreate(wxWindowCreateEvent& event);
    void OnEvent_Dialog(wxCommandEvent& event);
    void Clean_Exit();
    void Update_Position(const int x, const int y, const int width, const int height);
    void Update_FullScreen(bool bShowFullScreen);
    void Update_Position_FullScreen(const int x, const int y, const int width, const int height, bool bShowFullScreen);

    bool v_bInitialized;
    // generic dialog functionality
    Data_Holder_Dialog *v_pData_Holder_Dialog;
    // wait user action
    Extern_Task_Data *v_pExtern_Task_Data_WaitUser;
    // wait the dialog to be fully initialized
    Extern_Task_Data *v_pExtern_Task_Data_WaitInitialized;
};

#endif
// _WXDIALOG_BASE_H_
