//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxpanel_volume.h"
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

#include "wxpanel_volume.h"

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Volume type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Volume, wxPanel )

/*!
 * wxPanel_Volume event table definition
 */

    BEGIN_EVENT_TABLE( wxPanel_Volume, wxPanel )

////@begin wxPanel_Volume event table entries
    EVT_PAINT( wxPanel_Volume::OnPaint )

////@end wxPanel_Volume event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Volume constructors
 */

wxPanel_Volume::wxPanel_Volume( )
    : m_nPositions(0)
    , m_nCrtPosition(0)
    , bTypeSpeed(false)
    , bTypeRuler(false)
{
}

wxPanel_Volume::wxPanel_Volume( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
    : m_nPositions(0)
    , m_nCrtPosition(0)
    , bTypeSpeed(false)
    , bTypeRuler(false)
{
    Create(parent, id, pos, size, style);
}

/*!
 * wxPanel_Volume creator
 */

bool wxPanel_Volume::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxPanel_Volume member initialisation
////@end wxPanel_Volume member initialisation

////@begin wxPanel_Volume creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end wxPanel_Volume creation
    return true;
}

/*!
 * Control creation for wxPanel_Volume
 */

void wxPanel_Volume::CreateControls()
{
////@begin wxPanel_Volume content construction
    wxPanel_Volume* itemPanel1 = this;

    this->SetBackgroundColour(wxColour(173, 216, 230));
////@end wxPanel_Volume content construction
    wxUnusedVar(itemPanel1);
}

/*!
 * wxEVT_PAINT event handler for ID_PANEL_VOLUME
 */

void wxPanel_Volume::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_PANEL_VOLUME in wxPanel_Volume.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
////@end wxEVT_PAINT event handler for ID_PANEL_VOLUME in wxPanel_Volume.
    wxUnusedVar(event);
    _WX_LOG_NFO("pos=%d, crt_pos=%d, isSpeed=%d, isRuler=%d", m_nPositions, m_nCrtPosition, bTypeSpeed, bTypeRuler);
    // init draw
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    dc.SetBrush(GetForegroundColour());
    dc.SetPen(GetForegroundColour());
    wxSize oSize = dc.GetSize();
    int w = oSize.GetWidth();
    int h = oSize.GetHeight();
    const int NOTCH_SMALL = 4;
    const int NOTCH_BIG = 20;
    for (int i=0; i<m_nPositions; i++)
    {
        int x = (i+1) * w / (m_nPositions+1);
        // draw selected notch
        if (i < m_nCrtPosition)
        {
            dc.DrawRectangle(x - NOTCH_BIG/2, 0, NOTCH_BIG, h);
        }
        // draw notch
        dc.DrawRectangle(x - NOTCH_SMALL/2, h/4, NOTCH_SMALL, h/2);
    }
}

/*!
 * Should we show tooltips?
 */

bool wxPanel_Volume::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxPanel_Volume::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxPanel_Volume bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxPanel_Volume bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxPanel_Volume::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxPanel_Volume icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxPanel_Volume icon retrieval
}
