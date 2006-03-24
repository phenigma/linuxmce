//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXDIALOG_ROOMWIZARD_H_
#define _WXDIALOG_ROOMWIZARD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_roomwizard.h"
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

#ifdef USE_RELEASE_CODE
class WizardLogic;
#endif // USE_RELEASE_CODE

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_ROOMWIZARD 10000
#define SYMBOL_WXDIALOG_ROOMWIZARD_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_ROOMWIZARD_TITLE _T("Room Wizard")
#define SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME ID_DIALOG_ROOMWIZARD
#define SYMBOL_WXDIALOG_ROOMWIZARD_SIZE wxSize(300, 100)
#define SYMBOL_WXDIALOG_ROOMWIZARD_POSITION wxPoint(200, 300)
#define ID_GRID_ROOMWIZARD 10001
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

struct RoomItem
{
    RoomItem(const char *sName, const int nValue, const int nKey)
        {
            this->sName = sName;
            this->nValue = nValue;
            this->nKey = nKey;
        }
    wxString sName;
    int nValue;
    int nKey;
};
WX_DECLARE_OBJARRAY(RoomItem, wxArray_RoomItems);

/*!
 * wxDialog_RoomWizard class declaration
 */

class wxDialog_RoomWizard: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_RoomWizard )
        ;
    DECLARE_EVENT_TABLE()
        ;

public:
    /// Constructors
    wxDialog_RoomWizard( );
    wxDialog_RoomWizard( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_ROOMWIZARD_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_ROOMWIZARD_POSITION, const wxSize& size = SYMBOL_WXDIALOG_ROOMWIZARD_SIZE, long style = SYMBOL_WXDIALOG_ROOMWIZARD_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_ROOMWIZARD_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_ROOMWIZARD_POSITION, const wxSize& size = SYMBOL_WXDIALOG_ROOMWIZARD_SIZE, long style = SYMBOL_WXDIALOG_ROOMWIZARD_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_RoomWizard event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_SIZE event handler for ID_DIALOG_ROOMWIZARD
    void OnSize( wxSizeEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_ROOMWIZARD
    void OnCellLeftClick( wxGridEvent& event );

////@end wxDialog_RoomWizard event handler declarations

////@begin wxDialog_RoomWizard member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_RoomWizard member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_RoomWizard member variables
    wxBoxSizer* v_pBoxV_all;
    wxGrid* v_pGrid;
////@end wxDialog_RoomWizard member variables
public:
    ~wxDialog_RoomWizard();

    virtual bool ExternData_Load(void *pExternData);
    virtual bool ExternData_Save(void *pExternData);
    virtual void Gui_Refresh();

    void NewDataRefresh(int x, int y, int h, int w);

    static const E_DIALOG_TYPE e_dialog_type;

    struct Data_Refresh
    {
        wxRect coord;
    };

protected:
    void ItemWindowSelect(int nItem, bool bOn=true);
    void eventButtonDec();
    void eventButtonInc();

    virtual void SafeRefresh_CopyData(void *pData_Refresh);
    Data_Refresh v_oData_Refresh;

    wxArray_RoomItems v_aRoomItems;
    int v_nSelectedItem; // 0 based, -1 not selected

#ifdef USE_RELEASE_CODE
    struct Persistent_Data
    {
        map <int, int> map_room_types;
        map <int, string> map_room_names;
    };
    Persistent_Data m_Persistent_Data;
#endif // USE_RELEASE_CODE
};

#endif
// _WXDIALOG_ROOMWIZARD_H_
