//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#ifdef USE_RELEASE_CODE
#include "../WizardLogic.h"
#include "../CallBackData.h"
#endif // USE_RELEASE_CODE

const int idxResizableColumn = 1;
const int nColumnCount = 4;

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_RoomWizard type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_RoomWizard, wxDialog_Base )
;

/*!
 * wxDialog_RoomWizard event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_RoomWizard, wxDialog_Base )

////@begin wxDialog_RoomWizard event table entries
    EVT_CLOSE( wxDialog_RoomWizard::OnCloseWindow )
    EVT_SIZE( wxDialog_RoomWizard::OnSize )

    EVT_GRID_CELL_LEFT_CLICK( wxDialog_RoomWizard::OnCellLeftClick )

////@end wxDialog_RoomWizard event table entries

END_EVENT_TABLE()
;

/*!
 * wxDialog_RoomWizard constructors
 */

wxDialog_RoomWizard::wxDialog_RoomWizard( )
{
    _LOG_NFO();
}

wxDialog_RoomWizard::wxDialog_RoomWizard( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _LOG_NFO();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_RoomWizard creator
 */

bool wxDialog_RoomWizard::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _LOG_NFO();
////@begin wxDialog_RoomWizard member initialisation
    v_pBoxV_all = NULL;
    v_pGrid = NULL;
////@end wxDialog_RoomWizard member initialisation

////@begin wxDialog_RoomWizard creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_RoomWizard creation
    return true;
}

/*!
 * Control creation for wxDialog_RoomWizard
 */

void wxDialog_RoomWizard::CreateControls()
{
    _LOG_NFO();
////@begin wxDialog_RoomWizard content construction
    wxDialog_RoomWizard* itemDialog_Base1 = this;

    this->SetForegroundColour(wxColour(255, 255, 255));
    this->SetBackgroundColour(wxColour(0, 0, 128));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pGrid = new wxGrid( itemDialog_Base1, ID_GRID_ROOMWIZARD, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxCLIP_CHILDREN |wxVSCROLL );
    v_pGrid->SetForegroundColour(wxColour(255, 255, 255));
    v_pGrid->SetBackgroundColour(wxColour(0, 0, 128));
    v_pGrid->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pGrid->SetDefaultColSize(50);
    v_pGrid->SetDefaultRowSize(20);
    v_pGrid->SetColLabelSize(0);
    v_pGrid->SetRowLabelSize(0);
    v_pGrid->CreateGrid(4, 5, wxGrid::wxGridSelectRows);
    v_pBoxV_all->Add(v_pGrid, 1, wxGROW, 5);

////@end wxDialog_RoomWizard content construction

    // clear grid
    wx_Clear(v_pGrid);
    wx_Align(v_pGrid);
    wx_Set_Color(v_pGrid, this->GetBackgroundColour(), this->GetForegroundColour());
    v_pGrid->SetEditable(false);
    // add columns
    v_pGrid->AppendCols(4);
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    wxUnusedVar(event);
}

/*!
 * wxEVT_SIZE event handler for ID_DIALOG_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnSize( wxSizeEvent& event )
{
    //_LOG_NFO();
    wxDialog_Base::OnSize(event);
    wx_Grid_Resize_Column(v_pGrid, idxResizableColumn);
////@begin wxEVT_SIZE event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_SIZE event handler for ID_DIALOG_ROOMWIZARD in wxDialog_RoomWizard.
}

/*!
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_ROOMWIZARD
 */

void wxDialog_RoomWizard::OnCellLeftClick( wxGridEvent& event )
{
    if (v_bInitialized)
    {
        int row=event.GetRow();
        int col=event.GetCol();
        _LOG_NFO("row=%d, col=%d", row, col);
        ItemWindowSelect(row);
        if (col == 0)
        {
            eventButtonDec();
        }
        else if (col == 3)
        {
            eventButtonInc();
        }
        return;
    }
////@begin wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_ROOMWIZARD in wxDialog_RoomWizard.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_ROOMWIZARD in wxDialog_RoomWizard.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_RoomWizard::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_RoomWizard::GetBitmapResource( const wxString& name )
{
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
    // Icon retrieval
////@begin wxDialog_RoomWizard icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_RoomWizard icon retrieval
}

bool wxDialog_RoomWizard::Gui_DataLoad(CallBackData *pCallBackData)
{
    RoomWizardCallBackData *pCallData = dynamic_cast<RoomWizardCallBackData *>(pCallBackData);
    _COND_RET(pCallData, false);
    v_aRoomItems.Clear();
    _LOG_NFO("pCallData=%p", pCallData);
#ifdef USE_DEBUG_CODE
    v_oPersistent_Data.map_room_types = pCallData->map_room_types;
    for (map<int,int>::iterator it = v_oPersistent_Data.map_room_types.begin(); it != v_oPersistent_Data.map_room_types.end(); ++it)
    {
        int nKey = it->first;
        int nValue = it->second;
        string sName = pCallData->map_fn_GetRoomTypeName[nKey];
        v_oPersistent_Data.map_room_names[nKey] = sName;
        _LOG_NFO("Load Roomtype: '%s' (pk: %d) : %d", sName.c_str(), nKey, nValue);
        v_aRoomItems.Add( RoomItem(sName.c_str(), nValue, nKey) );
    }
#endif // USE_DEBUG_CODE
#ifdef USE_RELEASE_CODE
    WizardLogic *pWizardLogic = pCallData->m_pWizardLogic;
    if (pWizardLogic == NULL)
    {
        _LOG_WRN("extern data not initialized");
    }
    else
    {
        _LOG_NFO("extern data (%p)", pWizardLogic);
        pWizardLogic->PreSeedRoomInfo(v_oPersistent_Data.map_room_types);
        for (map<int,int>::iterator it = v_oPersistent_Data.map_room_types.begin(); it != v_oPersistent_Data.map_room_types.end(); ++it)
        {
            int nKey = it->first;
            int nValue = it->second;
            string sName = pWizardLogic->GetRoomTypeName(nKey);
            v_oPersistent_Data.map_room_names[nKey] = sName;
            _LOG_NFO("Load Roomtype: '%s' (pk: %d) : %d", sName.c_str(), nKey, nValue);
            v_aRoomItems.Add( RoomItem(sName.c_str(), nValue, nKey) );
        }
    }
#endif // USE_RELEASE_CODE
    v_nSelectedItem = -1;
    if (v_aRoomItems.GetCount() <= 0)
        _LOG_WRN("Number of rooms: %d", v_aRoomItems.GetCount());
    else
        _LOG_NFO("Number of rooms: %d", v_aRoomItems.GetCount());
    v_pGrid->AppendRows(v_aRoomItems.GetCount());
    for (size_t i=0; i<v_aRoomItems.GetCount(); i++)
    {
        RoomItem item = v_aRoomItems[i];
        {
            v_pGrid->SetCellValue(i, 0, "-");
            v_pGrid->SetCellValue(i, 1, v_aRoomItems[i].sName);
            v_pGrid->SetCellValue(i, 2, Str(v_aRoomItems[i].nValue));
            v_pGrid->SetCellValue(i, 3, "+");
        }
    }
    v_pGrid->AutoSizeColumns();
    v_pGrid->AutoSizeRows();
    v_pGrid->SetGridCursor(0, 1);
    ItemWindowSelect(0);
    return Gui_Refresh(pCallBackData);
}

bool wxDialog_RoomWizard::Gui_DataSave(CallBackData *pCallBackData)
{
    RoomWizardCallBackData *pCallData = dynamic_cast<RoomWizardCallBackData *>(pCallBackData);
    _COND_RET(pCallData, false);
    _LOG_NFO("pCallData=%p", pCallData);
    if (v_aRoomItems.GetCount() <= 0)
        _LOG_WRN("Number of rooms: %d", v_aRoomItems.GetCount());
    else
        _LOG_NFO("Number of rooms: %d", v_aRoomItems.GetCount());
#ifdef USE_RELEASE_CODE
    WizardLogic *pWizardLogic = pCallData->m_pWizardLogic;
    if (pWizardLogic == NULL)
    {
        _LOG_WRN("extern data not initialized");
    }
    else
    {
        _LOG_NFO("extern data (%p)", pWizardLogic);
        _LOG_NFO("Number of rooms: %d", v_aRoomItems.GetCount());
        _COND(v_aRoomItems.GetCount() > 0);
        for (size_t i=0; i<v_aRoomItems.GetCount(); i++)
        {
            v_oPersistent_Data.map_room_types[v_aRoomItems[i].nKey] = v_aRoomItems[i].nValue;
            _LOG_NFO("Save Roomtype: '%s' (pk: %d) : %d", v_aRoomItems[i].sName.c_str(), v_aRoomItems[i].nKey, v_aRoomItems[i].nValue);
        }
        pWizardLogic->ProcessUpdatedRoomInfo(v_oPersistent_Data.map_room_types);
    }
#endif // USE_RELEASE_CODE
    return true;
}

void wxDialog_RoomWizard::ItemWindowSelect(int nItem, bool bOn/*=true*/)
{
    if ( (nItem < 0) || (nItem >= (int)v_aRoomItems.GetCount()) )
        return;
    _LOG_NFO("item=%d, bool=%d", nItem, bOn);
    // deselect previous
    if (bOn && (nItem != v_nSelectedItem))
        ItemWindowSelect(v_nSelectedItem, false);
    // also change GUI attributes
    if (bOn)
    {
        v_nSelectedItem = nItem;
        v_pGrid->SelectRow(v_nSelectedItem);
    }
    else
    {
        v_nSelectedItem = -1;
    }
}

void wxDialog_RoomWizard::eventButtonDec()
{
    _LOG_NFO();
    if (v_nSelectedItem < 0)
        return;
    _LOG_NFO("sel=%d, value=%d", v_nSelectedItem, v_aRoomItems[v_nSelectedItem].nValue);
    if (v_aRoomItems[v_nSelectedItem].nValue > 0)
        v_aRoomItems[v_nSelectedItem].nValue--;
    v_pGrid->SetCellValue(v_nSelectedItem, 2, Str(v_aRoomItems[v_nSelectedItem].nValue));
}

void wxDialog_RoomWizard::eventButtonInc()
{
    _LOG_NFO();
    if (v_nSelectedItem < 0)
        return;
    _LOG_NFO("sel=%d, value=%d", v_nSelectedItem, v_aRoomItems[v_nSelectedItem].nValue);
    v_aRoomItems[v_nSelectedItem].nValue++;
    v_pGrid->SetCellValue(v_nSelectedItem, 2, Str(v_aRoomItems[v_nSelectedItem].nValue));
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArray_RoomItems);
