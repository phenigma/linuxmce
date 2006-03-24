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
#include "wx_dialog_safe.h"

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

    void OnWindowCreate(wxWindowCreateEvent& event);

    void Data_Refresh_Enter(); // before setting refresh data
    void Data_Refresh_Leave(); // after setting refresh data

    virtual bool ExternData_Load(void *pExternData); // load data from extern object
    virtual bool ExternData_Save(void *pExternData); // save data to extern object

    void Set_Data_Holder_Dialog(Data_Holder_Dialog *pData_Holder_Dialog);

    static const E_DIALOG_TYPE e_dialog_type;

protected:
    enum E_REFRESH_STATUS
        {
            E_Refresh_Ready,
            E_Refresh_LoadingData,
            E_Refresh_ShouldUpdate,
            E_Refresh_WindowUpdate,
        };

    struct Data_Holder_Refresh : Data_Holder_Base
    {
        Data_Holder_Refresh(void *pData_Refresh)
                : Data_Holder_Base()
                , pData_Refresh(pData_Refresh)
            {
            }
        void *pData_Refresh;
    };

    void OnEvent_Dialog(wxCommandEvent& event);
    void SafeRefresh_WhenIdle();

    // should be called instead of a normal refresh-like function
    // create a structure with all data for refresh
    // and pass the ptr to be used in the copy function or GUI function
    void SafeRefresh_NewData(Data_Holder_Refresh &rData_Holder_Refresh);

    // just create a copy of this data
    // in a member variable
    // to be used later by GUI refresh code
    // pointer-argument is always != NULL
    virtual void SafeRefresh_CopyData(void *pData_Refresh);

    // called from main thread
    // GUI refresh related code should be implemented here
    // use the member variable from the copy function
    virtual void Gui_Refresh();

    bool v_bInitialized;

    wxCriticalSection v_oCriticalRefresh;
    E_REFRESH_STATUS v_eRefreshStatus;

    Data_Holder_Dialog *v_pData_Holder_Dialog; // generic dialog functionality

  private:
    friend const char * _str_enum(wxDialog_Base::E_REFRESH_STATUS value);
};

#endif
// _WXDIALOG_BASE_H_
