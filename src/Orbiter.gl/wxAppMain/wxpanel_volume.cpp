//
// Author : Remus C.
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
#include <wx/dcbuffer.h>

////@begin XPM images
////@end XPM images

/*!
 * wxPanel_Volume type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Volume, wxPanel );

/*!
 * wxPanel_Volume event table definition
 */

BEGIN_EVENT_TABLE( wxPanel_Volume, wxPanel )

////@begin wxPanel_Volume event table entries
    EVT_SIZE( wxPanel_Volume::OnSize )
    EVT_PAINT( wxPanel_Volume::OnPaint )

////@end wxPanel_Volume event table entries

    END_EVENT_TABLE();

/*!
 * wxPanel_Volume constructors
 */

wxPanel_Volume::wxPanel_Volume( )
        : v_nPositions(0)
        , v_nCrtPosition(0)
        , v_eStyle(VolumeControlCallBackData::UNUSED)
{
}

wxPanel_Volume::wxPanel_Volume( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
        : v_nPositions(0)
        , v_nCrtPosition(0)
        , v_eStyle(VolumeControlCallBackData::UNUSED)
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

////@end wxPanel_Volume content construction
    wxUnusedVar(itemPanel1);
    v_oImageBuffer.LoadFile("control_volume.png");
    _COND_RET(v_oImageBuffer.Ok());
    v_oBitmapDraw = v_oImageBuffer;
}

/*!
 * wxEVT_SIZE event handler for ID_PANEL_VOLUME
 */

void wxPanel_Volume::OnSize( wxSizeEvent& event )
{
////@begin wxEVT_SIZE event handler for ID_PANEL_VOLUME in wxPanel_Volume.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_SIZE event handler for ID_PANEL_VOLUME in wxPanel_Volume.
    int w = event.GetSize().GetWidth();
    int h = event.GetSize().GetHeight();
    _COND(v_oImageBuffer.Ok());
    _COND(v_oBitmapDraw.Ok());
    if ( (w != v_oBitmapDraw.GetWidth()) || (h != v_oBitmapDraw.GetHeight()) )
    {
        _LOG_NFO("Rescale image : (%d, %d) -> (%d, %d)", v_oBitmapDraw.GetWidth(), v_oBitmapDraw.GetHeight(), w, h);
        v_oBitmapDraw = wxBitmap(v_oImageBuffer.Scale(w, h));
    }
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
    _LOG_NFO("pos=%d, crt_pos=%d, style=%s", v_nPositions, v_nCrtPosition, _str_enum(v_eStyle));
    // check values
    if (
        (v_nPositions < 0) || (v_nCrtPosition < 0) ||
        ( ! (
            (v_eStyle == VolumeControlCallBackData::RULER) || (v_eStyle == VolumeControlCallBackData::SPEED)
            ))
        )
    {
        _LOG_ERR("Bad values: pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
        return;
    }
    else if (
        (v_nCrtPosition > v_nPositions)
        )
    {
        _LOG_WRN("Bad values: pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
    }
    else
    {
        _LOG_NFO("pos=%d, crt_pos=%d", v_nPositions, v_nCrtPosition);
    }
    // get dimensions
    wxSize oSize = dc.GetSize();
    int width = oSize.GetWidth();
    int height = oSize.GetHeight();
    // prepare colors
    wxColour oBgColour(GetBackgroundColour());
    wxBrush oBgBrush(oBgColour);
    wxColour oFgColour(GetForegroundColour());
    // prepare an alpha surface
    wxImage oFgImage(width, height, true);
    oFgImage.InitAlpha();
    //oFgImage.SetAlpha();
    wxBitmap oFgBitmap(oFgImage);
    wxMemoryDC dcFg;
    dcFg.SelectObject(oFgBitmap);
    dcFg.Clear(); // set alpha for all pixels in the surface
    {
        wxMemoryDC &dc = dcFg;
        // init draw
        dc.SetBackground(oBgBrush);
        dc.SetBrush(oFgColour);
        dc.SetPen(oFgColour);
        dc.Clear();
        // continue to draw notches
        double w_all = width;
        double h_all = height;
        for (int i=0; i<v_nPositions; i++)
        {
            // relative notch position
            double x_rel = (i+1) * w_all / (v_nPositions+1);
            if (i < v_nCrtPosition)
            {
                // draw selected notch
                double w = w_all * 0.80 / v_nPositions;
                double h = h_all * 0.80;
                double x = x_rel - ( w / 2 );
                double y = ( h_all - h ) / 2;
                dc.DrawRectangle((int)x, (int)y, (int)w, (int)h);
            }
            else
            {
                // draw notch
                double w = w_all * 0.50 / v_nPositions;
                double h = h_all * 0.50;
                double x = x_rel - ( w / 2 );
                double y = ( h_all - h ) / 2;
                dc.DrawRectangle((int)x, (int)y, (int)w, (int)h);
            }
        }
    }
    // actual blending
    dc.Blit(0, 0, width, height, &dcFg, 0, 0);
    dc.DrawBitmap(v_oBitmapDraw, 0, 0, false);
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
