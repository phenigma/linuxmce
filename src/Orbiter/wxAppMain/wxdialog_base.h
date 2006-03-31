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
struct Extern_Event_Data;

/*!
 * wxDialog_Base class declaration
 */

class wxDialog_Base: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( wxDialog_Base )
        ;
    DECLARE_EVENT_TABLE()
        ;

  public:
    /// Constructors
    wxDialog_Base( );
    wxDialog_Base( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_BASE_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_BASE_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_BASE_POSITION, const wxSize& size = SYMBOL_WXDIALOG_BASE_SIZE, long style = SYMBOL_WXDIALOG_BASE_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_BASE_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_BASE_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_BASE_POSITION, const wxSize& size = SYMBOL_WXDIALOG_BASE_SIZE, long style = SYMBOL_WXDIALOG_BASE_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_Base event handler declarations

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
    ~wxDialog_Base();
    virtual bool Destroy();
    virtual void EndModal(int retCode);
    void OnButton_EndModal( wxCommandEvent& event );
    void OnButton_SetReturnCode( wxCommandEvent& event );

    virtual bool Gui_DataLoad(void *pExternData); // load data from extern object, called after create
    virtual bool Gui_DataSave(void *pExternData); // save data to extern object, not called by default
    virtual void Gui_Refresh(void *pExternData); // GUI refresh related code should be implemented here

    bool IsInitialized();
    void Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog);
    void Set_WaitUser(Extern_Event_Data *pExtern_Event_Data);

protected:
    void OnWindowCreate(wxWindowCreateEvent& event);
    void OnEvent_Dialog(wxCommandEvent& event);

    bool v_bInitialized;
    Data_Holder_Dialog *v_pData_Holder_Dialog; // generic dialog functionality
    Extern_Event_Data *v_pExtern_Event_Data;
};

#endif
// _WXDIALOG_BASE_H_
