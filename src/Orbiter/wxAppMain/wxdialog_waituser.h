//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_WAITUSER_H_
#define _WXDIALOG_WAITUSER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waituser.h"
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
#define ID_DIALOG_WAITUSER 10005
#define SYMBOL_WXDIALOG_WAITUSER_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDIALOG_MODAL|wxDOUBLE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_WAITUSER_TITLE _T("Wait User")
#define SYMBOL_WXDIALOG_WAITUSER_IDNAME ID_DIALOG_WAITUSER
#define SYMBOL_WXDIALOG_WAITUSER_SIZE wxSize(130, 113)
#define SYMBOL_WXDIALOG_WAITUSER_POSITION wxPoint(65, 113)
#define ID_GAUGE_WAITUSER 10000
#define ID_TEXTCTRL_WAITUSER 10014
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_WaitUser class declaration
 */

class wxDialog_WaitUser: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_WaitUser )
        DECLARE_EVENT_TABLE()

        public:
    /// Constructors
    wxDialog_WaitUser( );
    wxDialog_WaitUser( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITUSER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITUSER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITUSER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITUSER_SIZE, long style = SYMBOL_WXDIALOG_WAITUSER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITUSER_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITUSER_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITUSER_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITUSER_SIZE, long style = SYMBOL_WXDIALOG_WAITUSER_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_WaitUser event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITUSER
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end wxDialog_WaitUser event handler declarations

////@begin wxDialog_WaitUser member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_WaitUser member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_WaitUser member variables
    wxBoxSizer* v_pBoxV_all;
    wxBoxSizer* v_pBoxH_top;
    wxStaticBitmap* v_pBitmap;
    wxBoxSizer* v_pBoxSpace;
    wxGauge* v_pGauge;
    wxBoxSizer* v_pBoxH_mid;
    wxTextCtrl* v_pInfoText;
    wxBoxSizer* v_pBoxH_bot;
    wxButton* v_pButtonOk;
////@end wxDialog_WaitUser member variables
  public:
    ~wxDialog_WaitUser();

    virtual bool ExternalData_Load(void *pExternData);

    void NewDataRefresh(const string &sInfo, int nTimeoutSeconds, map<int,string> *p_mapPrompts);

    static const E_wxDialog_Class_Type e_class_type;

  protected:
    void OnTimer_ExpireDialog(wxTimerEvent& event);

    int v_nExpireTime_ms; // in milliseconds
    int v_nCrtTime_ms;    // in milliseconds
    wxTimer v_oTimer_ExpireDialog;

    wxArrayString v_asLabels; // old and new

    struct Data_Refresh_WaitUser
    {
        Data_Refresh_WaitUser()
                : nTimeoutSeconds(0)
            {
            }
        Data_Refresh_WaitUser(const string &sInfo, const int nTimeoutSeconds, map<int,string> *p_mapPrompts)
            {
                Set(sInfo, nTimeoutSeconds, p_mapPrompts);
            }
        void Set(const string &sInfo, const int nTimeoutSeconds, map<int,string> *p_mapPrompts)
            {
                this->sInfo = sInfo;
                this->nTimeoutSeconds = nTimeoutSeconds;
                this->p_mapPrompts = p_mapPrompts;
            }
        string sInfo;
        int nTimeoutSeconds;
        map<int, string> *p_mapPrompts;
#ifdef USE_RELEASE_CODE
        int *m_pnButtonId;
#endif // USE_RELEASE_CODE
    };

    virtual void SafeRefresh_CopyData(void *pData_Refresh);
    virtual void SafeRefresh_Gui();

    Data_Refresh_WaitUser v_oData_Refresh;
};

#endif
// _WXDIALOG_WAITUSER_H_
