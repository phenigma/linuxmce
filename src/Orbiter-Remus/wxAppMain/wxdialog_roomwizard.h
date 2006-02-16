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

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxGrid;
////@end forward declarations

#if (! defined USE_DEBUG_CODE)
class WizardLogic;
#include <map>
using namespace std;
#endif // (! defined USE_DEBUG_CODE)

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_ROOMWIZARD 10000
#define SYMBOL_WXDIALOG_ROOMWIZARD_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDOUBLE_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN 
#define SYMBOL_WXDIALOG_ROOMWIZARD_TITLE _T("Room Wizard")
#define SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME ID_DIALOG_ROOMWIZARD
#define SYMBOL_WXDIALOG_ROOMWIZARD_SIZE wxSize(300, 100)
#define SYMBOL_WXDIALOG_ROOMWIZARD_POSITION wxPoint(200, 300)
#define ID_GRID_RW 10001
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
#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(RoomItem, wxArray_RoomItems);

/*!
 * wxDialog_RoomWizard class declaration
 */

class wxDialog_RoomWizard: public wxDialog
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

    /// wxEVT_CHAR event handler for ID_DIALOG_ROOMWIZARD
    void OnChar( wxKeyEvent& event );

    /// wxEVT_KEY_UP event handler for ID_DIALOG_ROOMWIZARD
    void OnKeyUp( wxKeyEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_RW
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
    wxBoxSizer* v_oBoxV_all;
    wxGrid* v_oGrid;
////@end wxDialog_RoomWizard member variables
public:
  ~wxDialog_RoomWizard();
  bool Destroy();

  bool IsInitialized(); // dialog can be used

  // refresh when ready
  void SetExternalRefresh(int x, int y, int h, int w);

  // ptr to object containing data for the dialog
  void SetExternalData(void *pExternData);

  bool Data_Load();
  bool Data_Save();

protected:
  virtual void OnInternalIdle();

  void ItemWindowSelect(int nItem, bool bOn=true);
  void Window_SetBackgroundColour(wxWindow* pWin, bool bOn=true);

  void eventButtonDec();
  void eventButtonInc();

  void _debug_log_ItemSelected(const wxString &s = "");

  wxArray_RoomItems v_aRoomItems;
  int v_nSelectedItem; // 0 based, -1 not selected

  bool b_initialized;
  bool b_should_refresh;
  wxRect v_rect_refresh;

#if (! defined USE_DEBUG_CODE)
protected:
  WizardLogic *m_pWizardLogic;
  map <int, int> m_map_room_types;
  map <int, string> m_map_room_names;
#endif // (! defined USE_DEBUG_CODE)
};

// call from any thread
void wxDialog_RoomWizard_Refresh(int x, int y, int h, int w); // create a refresh event

// call from the main thread
void wxDialog_RoomWizard_Show(void *pExternData=NULL); // create, load, show
void wxDialog_RoomWizard_Close(bool bSaveData); // save, close

#endif
// _WXDIALOG_ROOMWIZARD_H_
