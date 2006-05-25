#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_arrow.h"
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

#include "wxpanel_arrow.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Arrow type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Arrow, wxPanel );

/*!
 * wxPanel_Arrow event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Arrow, wxPanel )

////@begin wxPanel_Arrow event table entries
    EVT_PAINT( wxPanel_Arrow::OnPaint )

////@end wxPanel_Arrow event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Arrow constructors
 */

wxPanel_Arrow::wxPanel_Arrow( )
        : v_dPosPercent(0)
{
}

wxPanel_Arrow::wxPanel_Arrow( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_dPosPercent(0)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Arrow creator
 */

bool wxPanel_Arrow::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Arrow member initialisation
////@end wxPanel_Arrow member initialisation

////@begin wxPanel_Arrow creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Arrow creation
    return true;
}

/*!
 * Control creation for wxPanel_Arrow
 */

void wxPanel_Arrow::CreateControls()
{
////@begin wxPanel_Arrow content construction
    wxPanel_Arrow* itemPanel1 = this;

////@end wxPanel_Arrow content construction
    wxUnusedVar(itemPanel1);
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_ARROW
 */

void wxPanel_Arrow::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_ARROW in wxPanel_Arrow.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_ARROW in wxPanel_Arrow.
    wxUnusedVar(event);
    // init draw
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.SetBrush(GetForegroundColour());
    dc.SetPen(GetForegroundColour());
    dc.Clear();
    // check values
    if (
        (v_dPosPercent < 0) || (v_dPosPercent > 100)
        )
    {
        _WX_LOG_ERR("Bad value: PosPercent=%f", v_dPosPercent);
        return;
    }
    else
    {
        //_WX_LOG_NFO("PosPercent=%f", v_dPosPercent);
    }
    // continue to draw
    wxSize oSize = dc.GetSize();
    int x = (int)(v_dPosPercent * oSize.GetWidth());
    int y = oSize.GetHeight();
    // draw filled triangle
    wxPoint aPoints[] =
        {
            wxPoint(x, 0),
            wxPoint(x + y/4, y),
            wxPoint(x - y/4, y),
        };
    dc.DrawPolygon( WXSIZEOF(aPoints), aPoints );
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Arrow::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Arrow::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Arrow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Arrow bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Arrow::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Arrow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Arrow icon retrieval
}
