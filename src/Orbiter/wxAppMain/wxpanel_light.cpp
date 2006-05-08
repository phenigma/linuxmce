//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_light.h"
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

#include "wxpanel_light.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Light type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Light, wxPanel );

/*!
 * wxPanel_Light event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Light, wxPanel )

////@begin wxPanel_Light event table entries
    EVT_PAINT( wxPanel_Light::OnPaint )

////@end wxPanel_Light event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Light constructors
 */

wxPanel_Light::wxPanel_Light( )
        : v_nPositions(0)
        , v_nCrtPosition(0)
{
}

wxPanel_Light::wxPanel_Light( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_nPositions(0)
        , v_nCrtPosition(0)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Light creator
 */

bool wxPanel_Light::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Light member initialisation
////@end wxPanel_Light member initialisation

////@begin wxPanel_Light creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Light creation
    return true;
}

/*!
 * Control creation for wxPanel_Light
 */

void wxPanel_Light::CreateControls()
{
////@begin wxPanel_Light content construction
    wxPanel_Light* itemPanel1 = this;

////@end wxPanel_Light content construction
    wxUnusedVar(itemPanel1);
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_LIGHT
 */

void wxPanel_Light::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_LIGHT in wxPanel_Light.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_LIGHT in wxPanel_Light.
    wxUnusedVar(event);
    // init draw
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.SetBrush(GetForegroundColour());
    dc.SetPen(GetForegroundColour());
    dc.Clear();
    // check values
    if (
        (v_nPositions < 0) || (v_nCrtPosition < 0)
        )
    {
        _WX_LOG_ERR("Bad values: pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
        return;
    }
    else if (
        (v_nCrtPosition > v_nPositions)
        )
    {
        _WX_LOG_WRN("Bad values: pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
    }
    else
    {
        _WX_LOG_NFO("pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
    }
    // continue to draw
    wxSize oSize = dc.GetSize();
    int w = oSize.GetWidth();
    int h = oSize.GetHeight();
    const int NOTCH_SMALL = 4;
    const int NOTCH_BIG = 20;
    for (int i=0; i<v_nPositions; i++)
    {
        int y = (v_nPositions - i) * h / (v_nPositions+1);
        // draw selected notch
        if (i < v_nCrtPosition)
        {
            dc.DrawRectangle(0, y - NOTCH_BIG/2, w, NOTCH_BIG);
        }
        // draw notch
        dc.DrawRectangle(w/4, y - NOTCH_SMALL/2, w/2, NOTCH_SMALL);
    }
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Light::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Light::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Light bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Light bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Light::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Light icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Light icon retrieval
}
