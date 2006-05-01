//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_time.h"
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

#include "wxpanel_time.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Time type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Time, wxPanel )

/*!
 * wxPanel_Time event table definition
 */

    BEGIN_EVENT_TABLE( wxPanel_Time, wxPanel )

////@begin wxPanel_Time event table entries
    EVT_PAINT( wxPanel_Time::OnPaint )

////@end wxPanel_Time event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Time constructors
 */

wxPanel_Time::wxPanel_Time( )
        : v_nTimeStart(0)
        , v_nTimeNow(0)
        , v_nTimeEnd(0)
{
}

wxPanel_Time::wxPanel_Time( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_nTimeStart(0)
        , v_nTimeNow(0)
        , v_nTimeEnd(0)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Time creator
 */

bool wxPanel_Time::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Time member initialisation
    v_pSliderTime = NULL;
    v_pWinTime = NULL;
    v_pTimeStart = NULL;
    v_pTimeNow = NULL;
    v_pTimeEnd = NULL;
////@end wxPanel_Time member initialisation

////@begin wxPanel_Time creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Time creation
    return true;
}

/*!
 * Control creation for wxPanel_Time
 */

void wxPanel_Time::CreateControls()
{
////@begin wxPanel_Time content construction
    wxPanel_Time* itemPanel1 = this;

    this->SetForegroundColour(wxColour(165, 42, 42));
    this->SetBackgroundColour(wxColour(173, 216, 230));
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    v_pSliderTime = new wxSlider;
    v_pSliderTime->Create( itemPanel1, ID_SLIDER_TIME, 50, 0, 100, wxDefaultPosition, itemPanel1->ConvertDialogToPixels(wxSize(75, -1)), wxSL_HORIZONTAL|wxSL_AUTOTICKS );
    v_pSliderTime->SetBackgroundColour(wxColour(173, 216, 230));
    v_pSliderTime->Enable(false);
    itemBoxSizer2->Add(v_pSliderTime, 1, wxGROW|wxSHAPED, 5);

    v_pWinTime = new wxWindow;
    v_pWinTime->Create( itemPanel1, ID_WIN_TIME, wxDefaultPosition, itemPanel1->ConvertDialogToPixels(wxSize(100, 100)), wxNO_BORDER|wxCLIP_CHILDREN  );
    itemBoxSizer2->Add(v_pWinTime, 1, wxGROW, 5);

    v_pTimeStart = new wxStaticText;
    v_pTimeStart->Create( v_pWinTime, wxID_TIME_START, _T("11:11:11"), wxDefaultPosition, wxDefaultSize, 0 );

    v_pTimeNow = new wxStaticText;
    v_pTimeNow->Create( v_pWinTime, wxID_TIME_NOW, _T("22:22:22"), wxDefaultPosition, wxDefaultSize, 0 );

    v_pTimeEnd = new wxStaticText;
    v_pTimeEnd->Create( v_pWinTime, wxID_TIME_END, _T("99:99:99"), wxDefaultPosition, wxDefaultSize, 0 );

////@end wxPanel_Time content construction
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_TIME
 */

void wxPanel_Time::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_TIME in wxPanel_Time.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_TIME in wxPanel_Time.
    wxUnusedVar(event);
    _WX_LOG_NFO("start=%d, now=%d, end=%d", v_nTimeStart, v_nTimeNow, v_nTimeEnd);
    // init draw
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    dc.SetBrush(GetForegroundColour());
    dc.SetPen(GetForegroundColour());
    v_pSliderTime->SetRange(v_nTimeStart, v_nTimeEnd);
    v_pSliderTime->SetValue(v_nTimeNow);
    v_pTimeStart->SetLabel(StrTimeHMS(v_nTimeStart));
    v_pTimeStart->Move(0, 0);
    v_pTimeEnd->SetLabel(StrTimeHMS(v_nTimeEnd));
    v_pTimeEnd->Move(dc.GetSize().GetWidth() - v_pTimeEnd->GetSize().GetWidth(), 0);
    v_pTimeNow->SetLabel(StrTimeHMS(v_nTimeNow));
    int nDeltaFull = v_nTimeEnd - v_nTimeStart;
    int nDeltaSeek = v_nTimeNow - v_nTimeStart;
    if ( (nDeltaFull > 0) && (nDeltaSeek > 0) )
    {
        int x = dc.GetSize().GetWidth() * nDeltaSeek / nDeltaFull;
        int y = dc.GetSize().GetHeight();
        v_pTimeNow->Move(x - v_pTimeEnd->GetSize().GetWidth() / 2, 0);
        // draw filled triangle
        wxPoint aPoints[] =
            {
                wxPoint(x, 0),
                wxPoint(x - y/2, y),
                wxPoint(x + y/2, y),
            };
        dc.DrawPolygon( WXSIZEOF(aPoints), aPoints );
    }
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Time::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Time::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Time bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Time bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Time::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Time icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Time icon retrieval
}
