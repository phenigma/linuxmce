#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_speedcontrol.h"
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

#include "wxdialog_speedcontrol.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_SpeedControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_SpeedControl, wxDialog_Base )

/*!
 * wxDialog_SpeedControl event table definition
 */

    BEGIN_EVENT_TABLE( wxDialog_SpeedControl, wxDialog_Base )

////@begin wxDialog_SpeedControl event table entries
    EVT_CLOSE( wxDialog_SpeedControl::OnCloseWindow )

////@end wxDialog_SpeedControl event table entries

    END_EVENT_TABLE()

/*!
 * wxDialog_SpeedControl constructors
 */

    wxDialog_SpeedControl::wxDialog_SpeedControl( )
{
}

wxDialog_SpeedControl::wxDialog_SpeedControl( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_SpeedControl creator
 */

bool wxDialog_SpeedControl::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_SpeedControl member initialisation
    v_pBoxV_all = NULL;
    v_pBoxH_Seek = NULL;
    v_pWinSeek = NULL;
    v_pBoxH_Speed = NULL;
    v_pWinSpeed = NULL;
    v_pBoxH_Slider = NULL;
    v_pSliderTimeLine = NULL;
    v_pBoxH_Labels = NULL;
    v_pWinTimeShow = NULL;
    v_pTimeBeg = NULL;
    v_pTimeNow = NULL;
    v_pTimeEnd = NULL;
////@end wxDialog_SpeedControl member initialisation

////@begin wxDialog_SpeedControl creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_SpeedControl creation
    return true;
}

/*!
 * Control creation for wxDialog_SpeedControl
 */

void wxDialog_SpeedControl::CreateControls()
{
////@begin wxDialog_SpeedControl content construction
    wxDialog_SpeedControl* itemDialog_Base1 = this;

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_Seek = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Seek, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pWinSeek = new wxWindow;
    v_pWinSeek->Create( itemDialog_Base1, ID_WINDOW_SEEK, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(200, 20)), wxNO_BORDER|wxCLIP_CHILDREN |wxHSCROLL|wxVSCROLL );
    v_pWinSeek->SetBackgroundColour(wxColour(30, 144, 255));
    v_pBoxH_Seek->Add(v_pWinSeek, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pBoxH_Speed = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Speed, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pWinSpeed = new wxWindow;
    v_pWinSpeed->Create( itemDialog_Base1, ID_WINDOW_SPEED, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(200, 20)), wxSIMPLE_BORDER );
    v_pWinSpeed->SetBackgroundColour(wxColour(30, 144, 255));
    v_pBoxH_Speed->Add(v_pWinSpeed, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pBoxH_Slider = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Slider, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pSliderTimeLine = new wxSlider;
    v_pSliderTimeLine->Create( itemDialog_Base1, ID_SLIDER_TIMELINE, 0, 0, 100, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(200, 50)), wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS|wxSL_TOP );
    v_pBoxH_Slider->Add(v_pSliderTimeLine, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxSHAPED, 5);

    v_pBoxH_Labels = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Labels, 1, wxGROW|wxALL|wxSHAPED, 5);

    v_pWinTimeShow = new wxWindow;
    v_pWinTimeShow->Create( itemDialog_Base1, ID_WINDOW_LABELS, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(200, 20)), wxSIMPLE_BORDER );
    v_pWinTimeShow->SetBackgroundColour(wxColour(30, 144, 255));
    v_pBoxH_Labels->Add(v_pWinTimeShow, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    v_pWinTimeShow->SetSizer(itemBoxSizer11);

    v_pTimeBeg = new wxStaticText;
    v_pTimeBeg->Create( v_pWinTimeShow, wxID_TIME_BEG, _T("11:11:11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(v_pTimeBeg, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemBoxSizer11->Add(8, 8, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pTimeNow = new wxStaticText;
    v_pTimeNow->Create( v_pWinTimeShow, wxID_TIME_NOW, _T("22:22:22"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(v_pTimeNow, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemBoxSizer11->Add(8, 8, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pTimeEnd = new wxStaticText;
    v_pTimeEnd->Create( v_pWinTimeShow, wxID_TIME_END, _T("99:99:99"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(v_pTimeEnd, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

////@end wxDialog_SpeedControl content construction
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_SpeedControl::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_SpeedControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_SpeedControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_SpeedControl bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_SpeedControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_SpeedControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_SpeedControl icon retrieval
}
/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL
 */

void wxDialog_SpeedControl::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL in wxDialog_SpeedControl.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SPEEDCONTROL in wxDialog_SpeedControl. 
}


