//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_WAITGRID_H_
#define _WXDIALOG_WAITGRID_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_waitgrid.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/grid.h"
////@end includes
#include "wxdialog_base.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_WAITGRID 10020
#define SYMBOL_WXDIALOG_WAITGRID_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_WAITGRID_TITLE _T("Wait Grid")
#define SYMBOL_WXDIALOG_WAITGRID_IDNAME ID_DIALOG_WAITGRID
#define SYMBOL_WXDIALOG_WAITGRID_SIZE wxSize(228, 184)
#define SYMBOL_WXDIALOG_WAITGRID_POSITION wxPoint(114, 184)
#define ID_GRID_WAITGRID 10016
#define ID_TEXTCTRL_WAITGRID 10021
#define ID_GAUGE_WAITGRID 10022
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_WaitGrid class declaration
 */

class wxDialog_WaitGrid: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_WaitGrid )
        ;
    DECLARE_EVENT_TABLE()
        ;

public:
    /// Constructors
    wxDialog_WaitGrid( );
    wxDialog_WaitGrid( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITGRID_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITGRID_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITGRID_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITGRID_SIZE, long style = SYMBOL_WXDIALOG_WAITGRID_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_WAITGRID_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_WAITGRID_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_WAITGRID_POSITION, const wxSize& size = SYMBOL_WXDIALOG_WAITGRID_SIZE, long style = SYMBOL_WXDIALOG_WAITGRID_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_WaitGrid event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_SIZE event handler for ID_DIALOG_WAITGRID
    void OnSize( wxSizeEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end wxDialog_WaitGrid event handler declarations

////@begin wxDialog_WaitGrid member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_WaitGrid member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_WaitGrid member variables
    wxBoxSizer* v_pBoxV_all;
    wxBoxSizer* v_pBoxH_top;
    wxStaticBitmap* v_pBitmap;
    wxGrid* v_pGrid;
    wxBoxSizer* v_pBoxH_mid;
    wxTextCtrl* v_pInfoText;
    wxBoxSizer* v_pBoxH_bot;
    wxGauge* v_pGauge;
    wxButton* v_pButtonCancel;
////@end wxDialog_WaitGrid member variables
public:
    ~wxDialog_WaitGrid();

    virtual bool ExternalData_Load(void *pExternData);

    void NewDataRefresh(const string &sInfo, const map<string, bool> &mapStrBool, int nPercent);

    static const E_wxDialog_Class_Type e_class_type;

protected:
    wxArrayString v_asName; // old and new

    struct Data_Refresh_WaitGrid
    {
        Data_Refresh_WaitGrid()
                : nPercent(0)
            {
            }
        Data_Refresh_WaitGrid(const string &sInfo, const map<string, bool> &mapStrBool, const int nPercent)
            {
                Set(sInfo, mapStrBool, nPercent);
            }
        void Set(const string &sInfo, const map<string, bool> &mapStrBool, const int nPercent)
            {
                this->sInfo = sInfo;
                this->mapStrBool = mapStrBool;
                this->nPercent = nPercent;
            }
        string sInfo;
        map<string, bool> mapStrBool;
        int nPercent;
#ifdef USE_RELEASE_CODE
        bool *m_pbButtonPressed;
#endif // USE_RELEASE_CODE
    };

    Data_Refresh_WaitGrid v_oData_Refresh;

    virtual void SafeRefresh_CopyData(void *pData_Refresh);
    virtual void SafeRefresh_Gui();
};

#endif
// _WXDIALOG_WAITGRID_H_
