//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_slidermedia.h"
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

#include "wxdialog_slidermedia.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_SliderMedia type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_SliderMedia, wxDialog_Base )

/*!
 * wxDialog_SliderMedia event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_SliderMedia, wxDialog_Base )

////@begin wxDialog_SliderMedia event table entries
    EVT_CLOSE( wxDialog_SliderMedia::OnCloseWindow )
    EVT_SIZE( wxDialog_SliderMedia::OnSize )

////@end wxDialog_SliderMedia event table entries

END_EVENT_TABLE()

/*!
 * wxDialog_SliderMedia constructors
 */

wxDialog_SliderMedia::wxDialog_SliderMedia( )
{
}

wxDialog_SliderMedia::wxDialog_SliderMedia( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_SliderMedia creator
 */

bool wxDialog_SliderMedia::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_SliderMedia member initialisation
    v_pBoxV_all = NULL;
////@end wxDialog_SliderMedia member initialisation

////@begin wxDialog_SliderMedia creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
////@end wxDialog_SliderMedia creation
    return true;
}

/*!
 * Control creation for wxDialog_SliderMedia
 */

void wxDialog_SliderMedia::CreateControls()
{
////@begin wxDialog_SliderMedia content construction
    wxDialog_SliderMedia* itemDialog_Base1 = this;

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    wxSlider* itemSlider3 = new wxSlider;
    itemSlider3->Create( itemDialog_Base1, ID_SLIDER, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    v_pBoxV_all->Add(itemSlider3, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxDialog_SliderMedia content construction
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SLIDERMEDIA
 */

void wxDialog_SliderMedia::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SLIDERMEDIA in wxDialog_SliderMedia.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_SLIDERMEDIA in wxDialog_SliderMedia.
}

/*!
 * wxEVT_SIZE event handler for ID_DIALOG_SLIDERMEDIA
 */

void wxDialog_SliderMedia::OnSize( wxSizeEvent& event )
{
    //_WX_LOG_NFO();
    wxDialog_Base::OnSize(event);
////@begin wxEVT_SIZE event handler for ID_DIALOG_SLIDERMEDIA in wxDialog_SliderMedia.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_SIZE event handler for ID_DIALOG_SLIDERMEDIA in wxDialog_SliderMedia.
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_SliderMedia::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_SliderMedia::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_SliderMedia bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_SliderMedia bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_SliderMedia::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_SliderMedia icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_SliderMedia icon retrieval
}
