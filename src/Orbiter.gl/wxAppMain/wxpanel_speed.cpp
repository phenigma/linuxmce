//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_speed.h"
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

#include "wxpanel_speed.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Speed type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Speed, wxPanel );

/*!
 * wxPanel_Speed event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Speed, wxPanel )

////@begin wxPanel_Speed event table entries
    EVT_PAINT( wxPanel_Speed::OnPaint )

////@end wxPanel_Speed event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Speed constructors
 */

wxPanel_Speed::wxPanel_Speed( )
        : v_nSpeed(0)
{
}

wxPanel_Speed::wxPanel_Speed( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_nSpeed(0)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Speed creator
 */

bool wxPanel_Speed::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Speed member initialisation
////@end wxPanel_Speed member initialisation

////@begin wxPanel_Speed creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Speed creation
    return true;
}

/*!
 * Control creation for wxPanel_Speed
 */

void wxPanel_Speed::CreateControls()
{
////@begin wxPanel_Speed content construction
    wxPanel_Speed* itemPanel1 = this;

////@end wxPanel_Speed content construction
    wxUnusedVar(itemPanel1);
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_SPEED
 */

void wxPanel_Speed::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_SPEED in wxPanel_Speed.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_SPEED in wxPanel_Speed.
    wxUnusedVar(event);
    _LOG_NFO("count=%d, speed=%d", v_anSpeeds.GetCount(), v_nSpeed);
    // init draw
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.SetBrush(GetForegroundColour());
    dc.SetPen(GetForegroundColour());
    dc.Clear();
    // check values
    if (
        (v_anSpeeds.GetCount() == 0)
        )
    {
        _LOG_WRN("Bad value: array_size=%d", v_anSpeeds.GetCount());
    }
    else
    {
        _LOG_NFO("array_size=%d", v_anSpeeds.GetCount());
    }
    // continue to draw
    int nCount = v_anSpeeds.size();
    int w = dc.GetSize().GetWidth() / nCount;
    int h = dc.GetSize().GetHeight() / nCount;
    bool bIndexFound = false;
    for (size_t i=0; i<v_anSpeeds.GetCount(); i++)
    {
        int nSpeed = v_anSpeeds[i];
        int x = i * w;
        // hilight current speed
        if (v_nSpeed == nSpeed)
        {
            bIndexFound = true;
            dc.DrawRoundedRectangle(x - w/2, 0, w, h, -1);
        }
        // draw every speed
        if (nSpeed == 0)
        {
            dc.DrawCircle(x, h/2, h/8);
        }
        else if (nSpeed > 0)
        {
            wxPoint aPoints[] =
            {
                wxPoint(x - w/2, 0),
                wxPoint(x - w/2, h),
                wxPoint(x + w/2, h/2),
            };
            dc.DrawPolygon( WXSIZEOF(aPoints), aPoints );
        }
        else // (nSpeed < 0)
        {
            wxPoint aPoints[] =
            {
                wxPoint(x + w/2, 0),
                wxPoint(x + w/2, h),
                wxPoint(x - w/2, h/2),
            };
            dc.DrawPolygon( WXSIZEOF(aPoints), aPoints );
        }
    }
    if (! bIndexFound)
    {
        _LOG_WRN("value not found: array_size=%d, speed=%d", v_anSpeeds.GetCount(), v_nSpeed);
    }
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Speed::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Speed::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Speed bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Speed bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Speed::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Speed icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Speed icon retrieval
}
