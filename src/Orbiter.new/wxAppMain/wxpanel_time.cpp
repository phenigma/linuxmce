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
#include "wxpanel_arrow.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Time type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Time, wxPanel );

/*!
 * wxPanel_Time event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Time, wxPanel )

////@begin wxPanel_Time event table entries
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
    v_pBoxV_all = NULL;
    v_pSliderTime = NULL;
    v_pPanel_Arrow = NULL;
    v_pBoxH_Start_End = NULL;
    v_pTimeStart = NULL;
    v_pTimeNow = NULL;
    v_pTimeEnd = NULL;
////@end wxPanel_Time member initialisation

////@begin wxPanel_Time creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
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

    this->SetForegroundColour(wxColour(0, 0, 255));
    this->SetBackgroundColour(wxColour(173, 216, 230));
    this->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, false, _T("Helvetica")));
    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(v_pBoxV_all);

    v_pSliderTime = new wxSlider;
    v_pSliderTime->Create( itemPanel1, ID_SLIDER_TIME, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_AUTOTICKS );
    v_pSliderTime->SetBackgroundColour(wxColour(173, 216, 230));
    v_pSliderTime->Enable(false);
    v_pBoxV_all->Add(v_pSliderTime, 1, wxGROW, 5);

    v_pPanel_Arrow = new wxPanel_Arrow;
    v_pPanel_Arrow->Create( itemPanel1, ID_CTRL_TIME_ARROW, wxDefaultPosition, itemPanel1->ConvertDialogToPixels(wxSize(100, 10)), wxSUNKEN_BORDER|wxCLIP_CHILDREN  );
    v_pBoxV_all->Add(v_pPanel_Arrow, 1, wxGROW, 5);

    v_pBoxH_Start_End = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Start_End, 0, wxGROW, 5);

    v_pTimeStart = new wxStaticText;
    v_pTimeStart->Create( itemPanel1, wxID_TIME_START, _T("11:11:11"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxH_Start_End->Add(v_pTimeStart, 0, wxGROW, 5);

    v_pBoxH_Start_End->Add(8, 8, 1, wxGROW, 5);

    v_pTimeNow = new wxStaticText;
    v_pTimeNow->Create( itemPanel1, wxID_TIME_NOW, _T("22:22:22"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxH_Start_End->Add(v_pTimeNow, 0, wxGROW, 5);

    v_pBoxH_Start_End->Add(8, 8, 1, wxGROW, 5);

    v_pTimeEnd = new wxStaticText;
    v_pTimeEnd->Create( itemPanel1, wxID_TIME_END, _T("99:99:99"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxH_Start_End->Add(v_pTimeEnd, 0, wxGROW, 5);

////@end wxPanel_Time content construction
    wx_CopyWindowAttributes(v_pPanel_Arrow, this);
    wx_CopyWindowAttributes(v_pTimeStart, this);
    wx_CopyWindowAttributes(v_pTimeNow, this);
    wx_CopyWindowAttributes(v_pTimeEnd, this);
#ifdef USE_RELEASE_CODE
    wx_WindowStyle_Del(v_pPanel_Arrow, wxNO_BORDER);
#endif // USE_RELEASE_CODE
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

void wxPanel_Time::Refresh(bool eraseBackground /*= true*/, const wxRect* rect /*= NULL*/)
{
    // check values
    if (
        (v_nTimeStart < 0) || (v_nTimeNow < 0) || (v_nTimeEnd < 0)
        )
    {
        _WX_LOG_ERR("Bad values: ""start=%d, now=%d, end=%d", v_nTimeStart, v_nTimeNow, v_nTimeEnd);
        return;
    }
    else if (
        (v_nTimeEnd <= v_nTimeStart) || (v_nTimeNow < v_nTimeStart) || (v_nTimeEnd < v_nTimeNow)
        )
    {
        _WX_LOG_WRN("Bad values: ""start=%d, now=%d, end=%d", v_nTimeStart, v_nTimeNow, v_nTimeEnd);
    }
    else
    {
        _WX_LOG_NFO("start=%d, now=%d, end=%d", v_nTimeStart, v_nTimeNow, v_nTimeEnd);
    }
    // update controls
    v_pSliderTime->SetRange(v_nTimeStart, v_nTimeEnd);
    v_pSliderTime->SetValue(v_nTimeNow);
    v_pTimeStart->SetLabel(StrTimeHMS(v_nTimeStart));
    v_pTimeEnd->SetLabel(StrTimeHMS(v_nTimeEnd));
    v_pTimeNow->SetLabel(StrTimeHMS(v_nTimeNow));
    int nDeltaFull = v_nTimeEnd - v_nTimeStart;
    int nDeltaSeek = v_nTimeNow - v_nTimeStart;
    if ( (nDeltaFull > 0) && (nDeltaSeek >= 0) )
    {
        double dPosPercent = (double)nDeltaSeek / nDeltaFull;
        v_pPanel_Arrow->v_dPosPercent = dPosPercent;
        int x_label_time = (int)( v_pTimeStart->GetRect().GetLeft() + dPosPercent * (v_pTimeStart->GetRect().GetLeft() - v_pTimeEnd->GetRect().GetRight()) );
        if (x_label_time != v_pTimeNow->GetPosition().x)
        {
            _WX_LOG_NFO("Changing Label Position : (%d) -> (%d)", v_pTimeNow->GetPosition().x, x_label_time);
            v_pTimeNow->SetSize(x_label_time, -1, -1, -1);
        }
    }
    // call refresh
    wxPanel::Refresh(eraseBackground, rect);
}
