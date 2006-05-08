//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_seek.h"
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

#include "wxpanel_seek.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Seek type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Seek, wxPanel );

/*!
 * wxPanel_Seek event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Seek, wxPanel )

////@begin wxPanel_Seek event table entries
    EVT_PAINT( wxPanel_Seek::OnPaint )

////@end wxPanel_Seek event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Seek constructors
 */

wxPanel_Seek::wxPanel_Seek( )
        : v_dPosPercent(0)
{
}

wxPanel_Seek::wxPanel_Seek( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_dPosPercent(0)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Seek creator
 */

bool wxPanel_Seek::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Seek member initialisation
////@end wxPanel_Seek member initialisation

////@begin wxPanel_Seek creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Seek creation
    return true;
}

/*!
 * Control creation for wxPanel_Seek
 */

void wxPanel_Seek::CreateControls()
{
////@begin wxPanel_Seek content construction
    wxPanel_Seek* itemPanel1 = this;

////@end wxPanel_Seek content construction
    wxUnusedVar(itemPanel1);
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_SEEK
 */

void wxPanel_Seek::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_SEEK in wxPanel_Seek.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_SEEK in wxPanel_Seek.
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
            wxPoint(x - y/2, 0),
            wxPoint(x + y/2, 0),
            wxPoint(x, y),
        };
    dc.DrawPolygon( WXSIZEOF(aPoints), aPoints );
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Seek::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Seek::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Seek bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Seek bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Seek::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Seek icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Seek icon retrieval
}
