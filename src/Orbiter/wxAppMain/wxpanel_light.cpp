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

IMPLEMENT_DYNAMIC_CLASS( wxPanel_Light, wxPanel )

/*!
 * wxPanel_Light event table definition
 */

    BEGIN_EVENT_TABLE( wxPanel_Light, wxPanel )

////@begin wxPanel_Light event table entries
////@end wxPanel_Light event table entries

    END_EVENT_TABLE()

/*!
 * wxPanel_Light constructors
 */

    wxPanel_Light::wxPanel_Light( )
{
}

wxPanel_Light::wxPanel_Light( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
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
    Centre();
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

    this->SetBackgroundColour(wxColour(173, 216, 230));
////@end wxPanel_Light content construction
    wxUnusedVar(itemPanel1);
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
