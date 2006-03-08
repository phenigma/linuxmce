//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_waitgrid.h"
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

#include "wxdialog_waitgrid.h"

////@begin XPM images
#include "logo_pluto.xpm"
////@end XPM images

/*!
 * wxDialog_WaitGrid type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_WaitGrid, wxDialog_Base )
;

/*!
 * wxDialog_WaitGrid event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_WaitGrid, wxDialog_Base )

////@begin wxDialog_WaitGrid event table entries
    EVT_CLOSE( wxDialog_WaitGrid::OnCloseWindow )
    EVT_SIZE( wxDialog_WaitGrid::OnSize )

    EVT_BUTTON( wxID_CANCEL, wxDialog_WaitGrid::OnCancelClick )

////@end wxDialog_WaitGrid event table entries

END_EVENT_TABLE()
;

const E_wxDialog_Class_Type wxDialog_WaitGrid::e_class_type = E_wxDialog_WaitGrid;
const int idxResizableColumn = 1;
const int nColumnCount = 2;
#define S_WAITING wxString("Waiting")
#define S_STARTED wxString("Started")

/*!
 * wxDialog_WaitGrid constructors
 */

wxDialog_WaitGrid::wxDialog_WaitGrid( )
{
    _WX_LOG_NFO();
}

wxDialog_WaitGrid::wxDialog_WaitGrid( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_WaitGrid creator
 */

bool wxDialog_WaitGrid::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _WX_LOG_NFO();
////@begin wxDialog_WaitGrid member initialisation
    v_pBoxV_all = NULL;
    v_pBoxH_top = NULL;
    v_pBitmap = NULL;
    v_pGrid = NULL;
    v_pBoxH_mid = NULL;
    v_pInfoText = NULL;
    v_pBoxH_bot = NULL;
    v_pGauge = NULL;
    v_pButtonCancel = NULL;
////@end wxDialog_WaitGrid member initialisation

////@begin wxDialog_WaitGrid creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_WaitGrid creation
    return true;
}

/*!
 * Control creation for wxDialog_WaitGrid
 */

void wxDialog_WaitGrid::CreateControls()
{
    _WX_LOG_NFO();
////@begin wxDialog_WaitGrid content construction
    wxDialog_WaitGrid* itemDialog_Base1 = this;

    this->SetForegroundColour(wxColour(255, 255, 255));
    this->SetBackgroundColour(wxColour(224, 224, 240));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_top = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_top, 1, wxGROW|wxALL, 5);

    wxBitmap v_pBitmapBitmap(itemDialog_Base1->GetBitmapResource(wxT("logo_pluto.jpg")));
    v_pBitmap = new wxStaticBitmap;
    v_pBitmap->Create( itemDialog_Base1, wxID_STATIC, v_pBitmapBitmap, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(126, 87)), wxNO_BORDER );
    v_pBoxH_top->Add(v_pBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

    v_pGrid = new wxGrid( itemDialog_Base1, ID_GRID_WAITGRID, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN |wxVSCROLL );
    v_pGrid->SetForegroundColour(wxColour(224, 224, 240));
    v_pGrid->SetBackgroundColour(wxColour(224, 224, 240));
    v_pGrid->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pGrid->SetDefaultColSize(50);
    v_pGrid->SetDefaultRowSize(20);
    v_pGrid->SetColLabelSize(0);
    v_pGrid->SetRowLabelSize(0);
    v_pGrid->CreateGrid(4, 5, wxGrid::wxGridSelectRows);
    v_pBoxH_top->Add(v_pGrid, 1, wxGROW|wxALL, 10);

    v_pBoxH_mid = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_mid, 1, wxGROW|wxALL, 5);

    v_pInfoText = new wxTextCtrl;
    v_pInfoText->Create( itemDialog_Base1, ID_TEXTCTRL_WAITGRID, _T(" Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1 Info line 1\n Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2 Info line 2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxNO_BORDER );
    v_pInfoText->SetBackgroundColour(wxColour(224, 224, 240));
    v_pInfoText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_mid->Add(v_pInfoText, 1, wxGROW|wxALL, 10);

    v_pBoxH_bot = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_bot, 0, wxGROW|wxALL, 5);

    v_pGauge = new wxGauge;
    v_pGauge->Create( itemDialog_Base1, ID_GAUGE_WAITGRID, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH|wxNO_BORDER );
    v_pGauge->SetValue(50);
    v_pGauge->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_bot->Add(v_pGauge, 1, wxALIGN_CENTER_VERTICAL|wxALL, 10);

    v_pButtonCancel = new wxButton;
    v_pButtonCancel->Create( itemDialog_Base1, wxID_CANCEL, _T("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButtonCancel->SetDefault();
    v_pButtonCancel->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, false, _T("Sans")));
    v_pBoxH_bot->Add(v_pButtonCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);

////@end wxDialog_WaitGrid content construction
    v_pInfoText->SetValue("");
    v_pGauge->SetValue(0);
    // clear grid
    wx_Clear(v_pGrid);
    wx_Align(v_pGrid, wxALIGN_LEFT);
    wx_Set_Color(v_pGrid, this->GetBackgroundColour(), this->GetForegroundColour(), this->GetBackgroundColour());
    v_pGrid->SetEditable(false);
    // add columns
    v_pGrid->AppendCols(2);
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID
 */

void wxDialog_WaitGrid::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid.
}

/*!
 * wxEVT_SIZE event handler for ID_DIALOG_WAITGRID
 */

void wxDialog_WaitGrid::OnSize( wxSizeEvent& event )
{
    //_WX_LOG_NFO();
    wxDialog_Base::OnSize(event);
    wx_Grid_Resize_Column(v_pGrid, idxResizableColumn);
////@begin wxEVT_SIZE event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_SIZE event handler for ID_DIALOG_WAITGRID in wxDialog_WaitGrid. 
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void wxDialog_WaitGrid::OnCancelClick( wxCommandEvent& WXUNUSED(event) )
{
#ifdef USE_RELEASE_CODE
    if (v_oData_Refresh.m_pbButtonPressed)
        *v_oData_Refresh.m_pbButtonPressed = true;
#endif // USE_RELEASE_CODE
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in wxDialog_WaitGrid.
    // Before editing this code, remove the block markers.
    Destroy();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in wxDialog_WaitGrid.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_WaitGrid::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_WaitGrid::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_WaitGrid bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("logo_pluto.jpg"))
    {
        wxBitmap bitmap(logo_pluto_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end wxDialog_WaitGrid bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_WaitGrid::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_WaitGrid icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_WaitGrid icon retrieval
}

//==================================================

wxDialog_WaitGrid::~wxDialog_WaitGrid()
{
    _WX_LOG_NFO();
}

bool wxDialog_WaitGrid::ExternalData_Load(void *pExternData)
{
    _WX_LOG_NFO("ptr=%p", pExternData);
#ifdef USE_DEBUG_CODE
    wxUnusedVar(pExternData);
#endif // USE_DEBUG_CODE
#ifdef USE_RELEASE_CODE
    v_oData_Refresh.m_pbButtonPressed = (bool *)pExternData;
    if (v_oData_Refresh.m_pbButtonPressed)
        *v_oData_Refresh.m_pbButtonPressed = false;
#endif // USE_RELEASE_CODE
    return true;
}

void wxDialog_WaitGrid::NewDataRefresh(const string &sInfo, const map<string, bool> &mapStrBool, int nPercent)
{
    _WX_LOG_NFO("(string '%s', map<string, bool>, int %d)", sInfo.c_str(), nPercent);
    Data_Refresh_WaitGrid data_refresh(sInfo, mapStrBool, nPercent);
    Data_Holder_Refresh data_holder_refresh(&data_refresh);
    _WX_LOG_DBG("%p", &data_holder_refresh);
    SafeRefresh_NewData(data_holder_refresh);
}

void wxDialog_WaitGrid::SafeRefresh_CopyData(void *pData_Refresh)
{
    _WX_LOG_NFO();
    _WX_LOG_DBG("%p", pData_Refresh);
    Data_Refresh_WaitGrid *pData_Refresh_WaitGrid = wx_static_cast(Data_Refresh_WaitGrid *, pData_Refresh);
    v_oData_Refresh = *pData_Refresh_WaitGrid;
}

void wxDialog_WaitGrid::SafeRefresh_Gui()
{
    //_WX_LOG_NFO();
    // update info text
    v_pInfoText->SetValue(v_oData_Refresh.sInfo);
    // update grid
    for(map<string, bool>::iterator it = v_oData_Refresh.mapStrBool.begin(); it != v_oData_Refresh.mapStrBool.end(); ++it)
    {
        wxString sName = it->first.c_str();
        bool bValue = it->second;
        // search string in current data
        // add item if not found
        int idx = v_asName.Index(sName);
        if (idx == wxNOT_FOUND)
        {
            idx = v_asName.GetCount();
            v_asName.Add(sName);
            v_pGrid->AppendRows(1);
            v_pGrid->SetCellValue(idx, 1, sName);
        }
        if (bValue)
        {
            v_pGrid->SetCellValue(idx, 0, S_STARTED);
            v_pGrid->SetCellTextColour(idx, 0, wxColour(0, 200, 0));
        }
        else
        {
            v_pGrid->SetCellValue(idx, 0, S_WAITING);
            v_pGrid->SetCellTextColour(idx, 0, wxColour(200, 0, 0));
        }
    }
    v_pGrid->AutoSizeColumns();
    v_pGrid->AutoSizeRows();
    wx_Grid_Resize_Column(v_pGrid, idxResizableColumn);
    // update progress bar
    v_pGauge->SetValue(v_oData_Refresh.nPercent);
}
