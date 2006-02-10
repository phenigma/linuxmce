#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxdialog_roomwizard.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxdialog_roomwizard.h"
#include "wx_other.h"
#include "wxthreadbag.h"

#if (! defined USE_DEBUG_CODE)
#include "../../DCE/Logger.h"
using namespace DCE;
#include "../WizardLogic.h"
#endif // (! defined USE_DEBUG_CODE)

////@begin XPM images
////@end XPM images

//========================================
// extern helper functions & data >>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArray_RoomItems);

wxDialog_RoomWizard *g_pwxDialog_RoomWizard = NULL;

void wxDialog_RoomWizard::SetExternalData(void *pExternData)
{
  _wx_log_nfo("wxDialog_RoomWizard::SetExternalData(%p)", pExternData);
#ifdef USE_DEBUG_CODE
  wxUnusedVar(pExternData);
#else // USE_DEBUG_CODE
  m_pWizardLogic = (WizardLogic *)pExternData;
#endif // USE_DEBUG_CODE
}

bool wxDialog_RoomWizard::data_load()
{
  _wx_log_nfo("wxDialog_RoomWizard::data_load()");
#ifdef USE_DEBUG_CODE
  v_aRoomItems.Add( RoomItem("Living Room", 1, 0) );
  v_aRoomItems.Add( RoomItem("Bathroom", 2, 0) );
  v_aRoomItems.Add( RoomItem("Bedroom", 4, 0) );
#else // USE_DEBUG_CODE
  if (m_pWizardLogic == NULL)
  {
    _wx_log_wrn("wxDialog_RoomWizard::data_load() : external data not initialized");
  }
  else
  {
    m_pWizardLogic->PreSeedRoomInfo(m_map_room_types);
    for(map<int,int>::iterator it = m_map_room_types.begin(); it != m_map_room_types.end(); ++it)
    {
      int nKey = it->first;
      int nValue = it->second;
      string sName = m_pWizardLogic->GetRoomTypeName(nKey);
      m_map_room_names[nKey] = sName;
      g_pPlutoLogger->Write(LV_STATUS, "Load Roomtype: %s (pk: %d) : %d", sName.c_str(), nKey, nValue);
      v_aRoomItems.Add( RoomItem(sName.c_str(), nValue, nKey) );
    }
  }
#endif // USE_DEBUG_CODE
  _wx_log_nfo("wxDialog_RoomWizard::data_load();;");
  return true;
}

bool wxDialog_RoomWizard::data_save()
{
  _wx_log_nfo("wxDialog_RoomWizard::data_save()");
  wxASSERT(v_aRoomItems.GetCount() > 0);
  if (v_aRoomItems.GetCount() <= 0)
    _wx_log_wrn("wxDialog_RoomWizard::data_save() : Number of rooms: %d", v_aRoomItems.GetCount());
  else
    _wx_log_nfo("wxDialog_RoomWizard::data_save() : Number of rooms: %d", v_aRoomItems.GetCount());
#ifdef USE_DEBUG_CODE
#else // USE_DEBUG_CODE
  if (m_pWizardLogic == NULL)
  {
    _wx_log_wrn("wxDialog_RoomWizard::data_save() : external data not initialized");
  }
  else
  {
    wxASSERT(v_aRoomItems.GetCount() > 0);
    if (v_aRoomItems.GetCount() <= 0)
      _wx_log_wrn("wxDialog_RoomWizard::data_save() : Number of rooms: %d", v_aRoomItems.GetCount());
    else
      _wx_log_nfo("wxDialog_RoomWizard::data_save() : Number of rooms: %d", v_aRoomItems.GetCount());
    for (size_t i=0; i<v_aRoomItems.GetCount(); i++)
    {
      m_map_room_types[v_aRoomItems[i].nKey] = v_aRoomItems[i].nValue;
      g_pPlutoLogger->Write(LV_STATUS, "Save Roomtype: %s (pk: %d) : %d", v_aRoomItems[i].sName.c_str(), v_aRoomItems[i].nKey, v_aRoomItems[i].nValue);
    }
    m_pWizardLogic->ProcessUpdatedRoomInfo(m_map_room_types);
  }
#endif // USE_DEBUG_CODE
  _wx_log_nfo("wxDialog_RoomWizard::data_save();;");
  return true;
}

void wxDialog_RoomWizard::ItemWindowSelect(int nItem, bool bOn/*=true*/)
{
  _wx_log_nfo("wxDialog_RoomWizard::ItemWindowSelect(%d, %d)", nItem, bOn);
  if ( (nItem < 0) || (nItem >= (int)v_aRoomItems.GetCount()) )
    return;
  // deselect previous
  if (bOn && (nItem != v_nSelectedItem))
    ItemWindowSelect(v_nSelectedItem, false);
  // also change GUI attributes
  if (bOn)
  {
    v_nSelectedItem = nItem;
    v_oGrid->SelectRow(v_nSelectedItem);
  }
  else
  {
    v_nSelectedItem = -1;
  }
}

void wxDialog_RoomWizard::Window_SetBackgroundColour(wxWindow* pWin, bool bOn/*=true*/)
{
  _wx_log_nfo("wxDialog_RoomWizard::Window_SetBackgroundColour(*)");
  wxELSE_RET(pWin != NULL);
  if (bOn)
  {
    pWin->SetBackgroundColour(*wxBLUE);
    //pWin->SetOwnBackgroundColour(*wxBLUE);
  }
  else
  {
    pWin->SetBackgroundColour(wxNullColour);
    //pWin->SetOwnBackgroundColour(wxNullColour);
  }
}

void wxDialog_RoomWizard::eventButtonDec()
{
  _wx_log_nfo("wxDialog_RoomWizard::eventButtonDec()");
  if (v_nSelectedItem < 0)
    return;
  _wx_log_nfo("wxDialog_RoomWizard::eventButtonDec(): sel=%d, value=%d", v_nSelectedItem, v_aRoomItems[v_nSelectedItem].nValue);
  if (v_aRoomItems[v_nSelectedItem].nValue > 0)
    v_aRoomItems[v_nSelectedItem].nValue--;
  v_oGrid->SetCellValue(v_nSelectedItem, 2, Str(v_aRoomItems[v_nSelectedItem].nValue));
}

void wxDialog_RoomWizard::eventButtonInc()
{
  _wx_log_nfo("wxDialog_RoomWizard::eventButtonInc()");
  if (v_nSelectedItem < 0)
    return;
  _wx_log_nfo("wxDialog_RoomWizard::eventButtonInc(): sel=%d, value=%d", v_nSelectedItem, v_aRoomItems[v_nSelectedItem].nValue);
  v_aRoomItems[v_nSelectedItem].nValue++;
  v_oGrid->SetCellValue(v_nSelectedItem, 2, Str(v_aRoomItems[v_nSelectedItem].nValue));
}

void wxDialog_RoomWizard::_debug_log_ItemSelected(const wxString &s/* = ""*/)
{
  _wx_log_nfo("wxDialog_RoomWizard::_debug_log_ItemSelected()");
  wxString s_new;
  if ( (v_nSelectedItem < 0) || (v_nSelectedItem >= (int)v_aRoomItems.GetCount()) )
    s_new = s + wxString::Format("[%d]", v_nSelectedItem);
  else
    s_new = s + wxString::Format("[%d] %s = %d", v_nSelectedItem, v_aRoomItems[v_nSelectedItem].sName.c_str(), v_aRoomItems[v_nSelectedItem].nValue);
  _wx_log_nfo(s_new);
}

// extern helper functions & data <<
//========================================

/*!
 * wxDialog_RoomWizard type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_RoomWizard, wxDialog )

/*!
 * wxDialog_RoomWizard event table definition
 */

  BEGIN_EVENT_TABLE( wxDialog_RoomWizard, wxDialog )

////@begin wxDialog_RoomWizard event table entries
    EVT_CLOSE( wxDialog_RoomWizard::OnCloseWindow )
    EVT_CHAR( wxDialog_RoomWizard::OnChar )
    EVT_KEY_UP( wxDialog_RoomWizard::OnKeyUp )

    EVT_GRID_SELECT_CELL( wxDialog_RoomWizard::OnSelectCell )
    EVT_CHAR( wxDialog_RoomWizard::OnChar )
    EVT_KEY_UP( wxDialog_RoomWizard::OnKeyUp )

////@end wxDialog_RoomWizard event table entries

  END_EVENT_TABLE()
  ;

/*!
 * wxDialog_RoomWizard constructors
 */

wxDialog_RoomWizard::wxDialog_RoomWizard( )
{
  _wx_log_nfo("wxDialog_RoomWizard::wxDialog_RoomWizard()");
  if (g_pwxDialog_RoomWizard != NULL)
  {
    _wx_log_wrn("wxDialog_RoomWizard::wxDialog_RoomWizard() : Object is not unique, addr=%p", g_pwxDialog_RoomWizard);
  }
  g_pwxDialog_RoomWizard = this;
#if (! defined USE_DEBUG_CODE)
  m_pWizardLogic = NULL;
#endif // (! defined USE_DEBUG_CODE)
}

wxDialog_RoomWizard::wxDialog_RoomWizard( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  _wx_log_nfo("wxDialog_RoomWizard::wxDialog_RoomWizard(*)");
  if (g_pwxDialog_RoomWizard != NULL)
  {
    _wx_log_wrn("wxDialog_RoomWizard::wxDialog_RoomWizard(*) : Object is not unique, addr=%p", g_pwxDialog_RoomWizard);
  }
  g_pwxDialog_RoomWizard = this;
#if (! defined USE_DEBUG_CODE)
  m_pWizardLogic = NULL;
#endif // (! defined USE_DEBUG_CODE)
  Create(parent, id, caption, pos, size, style);
}

wxDialog_RoomWizard::~wxDialog_RoomWizard( )
{
  _wx_log_nfo("wxDialog_RoomWizard::~wxDialog_RoomWizard()");
  if (g_pwxDialog_RoomWizard == this)
    g_pwxDialog_RoomWizard = NULL;
}

bool wxDialog_RoomWizard::Destroy()
{
  _wx_log_nfo("wxDialog_RoomWizard::Destroy()");
  return wxDialog::Destroy();
}

void wxDialog_RoomWizard::OnInternalIdle()
{
  //_wx_log_nfo("wxDialog::OnInternalIdle()");
  if ( wxIdleThreadShouldStop() )
    Destroy();
  wxDialog::OnInternalIdle();
}

/*!
 * wxDialog_RoomWizard creator
 */

bool wxDialog_RoomWizard::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  _wx_log_nfo("wxDialog_RoomWizard::Create(*)");
////@begin wxDialog_RoomWizard member initialisation
  v_oBoxV_all = NULL;
  v_oGrid = NULL;
////@end wxDialog_RoomWizard member initialisation

////@begin wxDialog_RoomWizard creation
  SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
////@end wxDialog_RoomWizard creation
  return true;
}

/*!
 * Control creation for wxDialog_RoomWizard
 */

void wxDialog_RoomWizard::CreateControls()
{
  _wx_log_nfo("wxDialog_RoomWizard::CreateControls()");
  b_initialized = false;
////@begin wxDialog_RoomWizard content construction
    wxDialog_RoomWizard* itemDialog1 = this;

    this->SetForegroundColour(wxColour(255, 255, 255));
    this->SetBackgroundColour(wxColour(0, 0, 255));
    this->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_oBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(v_oBoxV_all);

    v_oGrid = new wxGrid( itemDialog1, ID_GRID, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxCLIP_CHILDREN |wxVSCROLL );
    v_oGrid->SetForegroundColour(wxColour(255, 255, 255));
    v_oGrid->SetBackgroundColour(wxColour(0, 0, 255));
    v_oGrid->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_oGrid->SetDefaultColSize(50);
    v_oGrid->SetDefaultRowSize(20);
    v_oGrid->SetColLabelSize(0);
    v_oGrid->SetRowLabelSize(0);
    v_oGrid->CreateGrid(4, 5, wxGrid::wxGridSelectRows);
    v_oBoxV_all->Add(v_oGrid, 1, wxGROW|wxALL, 5);

////@end wxDialog_RoomWizard content construction
  wxASSERT(v_aRoomItems.GetCount() > 0);
  if (v_aRoomItems.GetCount() <= 0)
    _wx_log_wrn("wxDialog_RoomWizard::CreateControls() : Number of rooms: %d", v_aRoomItems.GetCount());
  else
    _wx_log_nfo("wxDialog_RoomWizard::CreateControls() : Number of rooms: %d", v_aRoomItems.GetCount());
  v_nSelectedItem = -1;
  v_oGrid->ClearGrid();
  v_oGrid->DeleteCols(0, v_oGrid->GetNumberCols());
  v_oGrid->DeleteRows(0, v_oGrid->GetNumberRows());
  v_oGrid->AppendCols(4);
  v_oGrid->AppendRows(v_aRoomItems.GetCount());
  //v_oGrid->SetDefaultColSize(20);
  //v_oGrid->SetDefaultRowSize(20);
  v_oGrid->SetColMinimalAcceptableWidth(10);
  v_oGrid->SetColMinimalWidth(0, 10);
  v_oGrid->SetColMinimalWidth(1, 100);
  v_oGrid->SetColMinimalWidth(2, 20);
  v_oGrid->SetColMinimalWidth(3, 10);
  v_oGrid->SetColSize(0, 20);
  v_oGrid->SetColSize(1, 200);
  v_oGrid->SetColSize(2, 30);
  v_oGrid->SetColSize(3, 20);
  v_oGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
  v_oGrid->SetDefaultCellBackgroundColour(wxColour(0, 0, 127));
  v_oGrid->SetDefaultCellTextColour(wxColour(200, 200, 200));
  for (size_t i=0; i<v_aRoomItems.GetCount(); i++)
  {
    RoomItem item = v_aRoomItems[i];
    {
      v_oGrid->SetCellValue(i, 0, "-");
      v_oGrid->SetReadOnly(i, 0);
      v_oGrid->SetCellValue(i, 1, v_aRoomItems[i].sName);
      v_oGrid->SetReadOnly(i, 1);
      v_oGrid->SetCellValue(i, 2, Str(v_aRoomItems[i].nValue));
      v_oGrid->SetReadOnly(i, 2);
      v_oGrid->SetCellValue(i, 3, "+");
      v_oGrid->SetReadOnly(i, 3);
    }
  }
  v_oGrid->SetGridCursor(0, 1);
  ItemWindowSelect(0);
  Window_SetBackgroundColour(v_oGrid);
  b_initialized = true;
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
  _wx_log_nfo("wxDialog_RoomWizard::OnCloseWindow(*)");
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
}

/*!
 * wxEVT_CHAR event handler for ID_DIALOG_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnChar( wxKeyEvent& event )
{
  _debug_log_ItemSelected("wxDialog_RoomWizard::OnChar(*)");
////@begin wxEVT_CHAR event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_CHAR event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
}

/*!
 * wxEVT_KEY_UP event handler for ID_DIALOG_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnKeyUp( wxKeyEvent& event )
{
  _debug_log_ItemSelected("wxDialog_RoomWizard::OnKeyUp(*)");
////@begin wxEVT_KEY_UP event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_KEY_UP event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
}

/*!
 * wxEVT_GRID_SELECT_CELL event handler for ID_GRID
 */

void wxDialog_RoomWizard::OnSelectCell( wxGridEvent& event )
{
  if (! b_initialized)
  {
    event.Skip();
    return;
  }
  int row=event.GetRow();
  int col=event.GetCol();
  _wx_log_nfo("wxDialog_RoomWizard::OnSelectCell(): row=%d, col=%d", row, col);
  ItemWindowSelect(row);
  event.Skip(false);
  if (col == 0)
  {
    eventButtonDec();
  }
  else if (col == 3)
  {
    eventButtonInc();
  }
  //   if (col != 1)
  //     oGrid->SetGridCursor(row, 1);
  //_debug_log_ItemSelected("OnSelectCell");
  return;
  /*
////@begin wxEVT_GRID_SELECT_CELL event handler for ID_GRID in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_GRID_SELECT_CELL event handler for ID_GRID in wxDialog_RoomWizard.
*/
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_RoomWizard::ShowToolTips()
{
  _wx_log_nfo("wxDialog_RoomWizard::ShowToolTips()");
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_RoomWizard::GetBitmapResource( const wxString& name )
{
  _wx_log_nfo("wxDialog_RoomWizard::GetBitmapResource(*)");
  // Bitmap retrieval
////@begin wxDialog_RoomWizard bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_RoomWizard bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_RoomWizard::GetIconResource( const wxString& name )
{
  _wx_log_nfo("wxDialog_RoomWizard::GetIconResource(*)");
  // Icon retrieval
////@begin wxDialog_RoomWizard icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_RoomWizard icon retrieval
}

//==================================================

bool wxDialog_RoomWizard_isActive()
{
  return (g_pwxDialog_RoomWizard != NULL);
}

void wxDialog_RoomWizard_SetSize(int x, int y, int h, int w)
{
  _wx_log_nfo("wxDialog_RoomWizard_SetSize(x=%d, y=%d, h=%d, w=%d)", x, y, h, w);
  if (! wxDialog_RoomWizard_isActive())
  {
    _wx_log_wrn("Dialog is not active, ignoring SetSize request");
    return;
  }
  g_pwxDialog_RoomWizard->SetSize(x, y, h, w);
}

void wxDialog_RoomWizard_Show(void *pExternData/*=NULL*/)
{
  _wx_log_nfo("wxDialog_RoomWizard_Show(%p)", pExternData);
  if (wxDialog_RoomWizard_isActive())
  {
    _wx_log_wrn("Dialog is already active, closing without saving data");
    g_pwxDialog_RoomWizard->Destroy();
  }
  _wx_log_nfo("Launching new dialog", pExternData);
  new wxDialog_RoomWizard();
  g_pwxDialog_RoomWizard->SetExternalData(pExternData);
  g_pwxDialog_RoomWizard->data_load();
  g_pwxDialog_RoomWizard->Create(NULL, ID_DIALOG_ROOMWIZARD, _("Room Wizard Dialog"));
  g_pwxDialog_RoomWizard->Show(true);
}

void wxDialog_RoomWizard_Close()
{
  _wx_log_nfo("wxDialog_RoomWizard_Close()");
  if (! wxDialog_RoomWizard_isActive())
  {
    _wx_log_wrn("Dialog is not active, ignoring Close request");
    return;
  }
  _wx_log_nfo("Closing the dialog");
  g_pwxDialog_RoomWizard->data_save();
  g_pwxDialog_RoomWizard->Destroy();
}

//==================================================
